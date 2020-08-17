# wf-gsettings

The main goal is to have a wayfire config backend that uses gsettings
or fork wayfire to use gsettings.

Meanwhile this is the gsettings plugin for [Wayfire].

- install latest Wayfire, GLib, `xsltproc` (from `libxslt`)
- install this (regular Meson build)
- (for both Wayfire and this, use a prefix that gets read by GSettings!)
- run `wf-gsettings-gen-schemas`
	- rerun when updating Wayfire/plugins
	- it generates GSettings schemas from Wayfire plugin metadata
		- schemas are named `org.wayfire.plugin.<name>`
	- and runs `glib-compile-schemas` for you
- add `gsettings` to the plugin list in Wayfire config
- start Wayfire, play around with [dconf-editor]

[Wayfire]: https://github.com/WayfireWM/wayfire
[dconf-editor]: https://wiki.gnome.org/Apps/DconfEditor
