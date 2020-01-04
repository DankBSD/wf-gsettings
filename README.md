# wf-gsettings

GSettings plugin for [Wayfire].

- install latest Wayfire, GLib, `xsltproc` (from `libxslt`)
- install this (regular Meson build)
- (for both Wayfire and this, use a prefix that gets read by GSettings!)
- run `wf-gsettings-gen-schemas`
	- rerun when updating Wayfire/plugins
	- it generates GSettings schemas from Wayfire plugin metadata
		- schemas are named `org.wayfire.plugin.<name>`
	- and runs `glib-compile-schemas` for you
- add `gsettings` to the plugin list in Wayfire config
- start Wayfire, play around with [dconf-editor], [numbernine]'s settings app, etc

[Wayfire]: https://github.com/WayfireWM/wayfire
[dconf-editor]: https://wiki.gnome.org/Apps/DconfEditor
[numbernine]: https://github.com/myfreeweb/numbernine

## License

This is free and unencumbered software released into the public domain.  
For more information, please refer to the `UNLICENSE` file or [unlicense.org](http://unlicense.org).
