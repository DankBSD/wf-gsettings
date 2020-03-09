#define WLR_USE_UNSTABLE
#define WAYFIRE_PLUGIN
#include <gio/gio.h>
#include <unistd.h>

#include <queue>
#include <thread>
#include <unordered_map>
#include <wayfire/core.hpp>
#include <wayfire/debug.hpp>
#include <wayfire/output.hpp>
#include <wayfire/plugin.hpp>
#include <wayfire/util/log.hpp>

struct conf_change {
	std::string sec;
	std::string key;
	GVariant *val;
};

static std::unordered_map<std::string, GSettings *> gsets;
static std::unordered_map<GSettings *, std::string> gsets_rev;
static std::queue<conf_change> changes;

static void gsettings_callback(GSettings *settings, gchar *key, gpointer user_data) {
	changes.push(
	    conf_change{gsets_rev[settings], std::string(key), g_settings_get_value(settings, key)});
	int fd = (int)(intptr_t)user_data;
	write(fd, "!", 1);
	char buff;
	read(fd, &buff, 1);
}

static void gsettings_loop(int fd) {
	// XXX: not really safe to read the list from this secondary thread
	usleep(100000);
	auto *gctx = g_main_context_new();
	g_main_context_push_thread_default(gctx);
	auto *loop = g_main_loop_new(gctx, false);
	for (auto sec : wf::get_core().config.get_all_sections()) {
		auto schema_name = "org.wayfire.plugin." + sec->get_name();
		if (g_settings_schema_source_lookup(g_settings_schema_source_get_default(), schema_name.c_str(),
		                                    FALSE) == nullptr) {
			LOGI("GSettings schema not found: ", schema_name.c_str());
			continue;
		}
		auto *gs = g_settings_new(schema_name.c_str());
		gsets.emplace(sec->get_name(), gs);
		gsets_rev.emplace(gs, sec->get_name());
		// For future changes
		g_signal_connect(gsets[sec->get_name()], "changed", G_CALLBACK(gsettings_callback), (void *)fd);
		// Initial values
		GSettingsSchema *schema = nullptr;
		g_object_get(gs, "settings-schema", &schema, NULL);
		gchar **keys = g_settings_schema_list_keys(schema);
		while (*keys != nullptr) {
			gsettings_callback(gs, *keys++, (void *)fd);
		}
		g_settings_schema_unref(schema);
	}
	g_main_loop_run(loop);
}

static int handle_update(int fd, uint32_t mask, void *data);

struct gsettings_ctx : public wf::custom_data_t {
	std::thread loopthread;
	int fd[2] = {0, 0};
	wf::wl_timer sig_debounce;

	gsettings_ctx() {
		pipe(fd);
		loopthread = std::thread(gsettings_loop, fd[1]);
		wl_event_loop_add_fd(wf::get_core().ev_loop, fd[0], WL_EVENT_READABLE, handle_update, this);
	}
};

static int handle_update(int fd, uint32_t mask, void *data) {
	auto *ctx = reinterpret_cast<gsettings_ctx *>(data);
	char buff;
	read(fd, &buff, 1);
	while (!changes.empty()) {
		auto chg = changes.front();
		// GSettings does not support underscores
		std::replace(chg.key.begin(), chg.key.end(), '-', '_');
		try {
			auto opt = wf::get_core().config.get_section(chg.sec)->get_option(chg.key);
			const auto *typ = g_variant_get_type(chg.val);
			if (opt == nullptr) {
				LOGI("GSettings update found nullptr opt: ", chg.sec.c_str(), "/", chg.key.c_str());
			} else if (g_variant_type_equal(typ, G_VARIANT_TYPE_STRING)) {
				opt->set_value_str(std::string(g_variant_get_string(chg.val, NULL)));
			} else if (g_variant_type_equal(typ, G_VARIANT_TYPE_BOOLEAN)) {
				auto topt = std::dynamic_pointer_cast<wf::config::option_t<bool>>(opt);
				if (topt == nullptr) {
					LOGW("GSettings update could not cast opt to bool: ", chg.sec.c_str(), "/", chg.key.c_str());
				} else {
					topt->set_value(g_variant_get_boolean(chg.val));
				}
			} else if (g_variant_type_equal(typ, G_VARIANT_TYPE_INT32)) {
				auto topt = std::dynamic_pointer_cast<wf::config::option_t<int>>(opt);
				if (topt == nullptr) {
					LOGW("GSettings update could not cast opt to int: ", chg.sec.c_str(), "/", chg.key.c_str());
				} else {
					topt->set_value(g_variant_get_int32(chg.val));
				}
			} else if (g_variant_type_equal(typ, G_VARIANT_TYPE_DOUBLE)) {
				auto topt = std::dynamic_pointer_cast<wf::config::option_t<double>>(opt);
				if (topt == nullptr) {
					LOGW("GSettings update could not cast opt to double: ", chg.sec.c_str(), "/", chg.key.c_str());
				} else {
					topt->set_value(static_cast<float>(g_variant_get_double(chg.val)));
				}
			} else if (g_variant_type_equal(typ, G_VARIANT_TYPE("(dddd)"))) {
				auto topt = std::dynamic_pointer_cast<wf::config::option_t<wf::color_t>>(opt);
				if (topt == nullptr) {
					LOGW("GSettings update could not cast opt to color: ", chg.sec.c_str(), "/", chg.key.c_str());
				} else {
					topt->set_value(wf::color_t{
					    static_cast<float>(g_variant_get_double(g_variant_get_child_value(chg.val, 0))),
					    static_cast<float>(g_variant_get_double(g_variant_get_child_value(chg.val, 1))),
					    static_cast<float>(g_variant_get_double(g_variant_get_child_value(chg.val, 2))),
					    static_cast<float>(g_variant_get_double(g_variant_get_child_value(chg.val, 3)))});
				}
			} else {
				LOGI("GSettings update has unsupported type: ", chg.sec.c_str(), "/", chg.key.c_str());
			}
		} catch (std::invalid_argument &e) {
			LOGE("GSettings update could not apply: ", chg.sec.c_str(), "/", chg.key.c_str());
		}
		g_variant_unref(chg.val);
		changes.pop();
	}
	// The signal triggers relatively heavy stuff like cursor theme loading
	// Firing it per value e.g. when initially applying everything is a bad idea
	// TODO: if possible, add more efficient way to wayfire, without readding source
	ctx->sig_debounce.disconnect();
	ctx->sig_debounce.set_timeout(69, []() {
		wf::get_core().emit_signal("reload-config", nullptr);
		LOGI("GSettings applied");
	});
	write(fd, "!", 1);
	return 1;
}

// Plugins are per-output, this wrapper/data thing is for output independence
struct wayfire_gsettings : public wf::plugin_interface_t {
	void init() override {
		if (!wf::get_core().has_data<gsettings_ctx>()) {
			wf::get_core().store_data(std::make_unique<gsettings_ctx>());
		}
	}

	bool is_unloadable() override { return false; }
};

DECLARE_WAYFIRE_PLUGIN(wayfire_gsettings);
