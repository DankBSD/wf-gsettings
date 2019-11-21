project('wf-gsettings', ['cpp'], version: '0.0.1', license: 'Unlicense',
	default_options: ['cpp_std=c++17', 'warning_level=2'])

wayfire = dependency('wayfire')
wlroots = dependency('wlroots', fallback: ['wlroots', 'wlroots'])
gio = dependency('gio-2.0')
gnome = import('gnome')

subdir('schemas')

plugin_gsettings = shared_module('gsettings', 'gsettings.cpp',
	dependencies: [wayfire, wlroots, gio],
	install: true,
	install_dir: 'lib/wayfire')
