
# Unfortunately, meson is not powerful enough
# https://github.com/mesonbuild/meson/issues/3206
# so for now, the files are generated beforehand
#
# prog_xslt = find_program('xsltproc')
# gen_schema = generator(prog_xslt,
# 	output: ['org.wayfire.plugin.@BASENAME@.gschema.xml'],
# 	arguments: ['-o', '@OUTPUT@', 'genschema.xsl', '@INPUT@'])

xml_schemas = files(
	'org.wayfire.plugin.alpha.gschema.xml',
	'org.wayfire.plugin.animate.gschema.xml',
	'org.wayfire.plugin.autorotate-iio.gschema.xml',
	'org.wayfire.plugin.autostart.gschema.xml',
	'org.wayfire.plugin.blur.gschema.xml',
	'org.wayfire.plugin.command.gschema.xml',
	'org.wayfire.plugin.core.gschema.xml',
	'org.wayfire.plugin.cube.gschema.xml',
	'org.wayfire.plugin.decoration.gschema.xml',
	'org.wayfire.plugin.example.gschema.xml',
	'org.wayfire.plugin.expo.gschema.xml',
	'org.wayfire.plugin.fast-switcher.gschema.xml',
	'org.wayfire.plugin.fisheye.gschema.xml',
	'org.wayfire.plugin.grid.gschema.xml',
	'org.wayfire.plugin.idle.gschema.xml',
	'org.wayfire.plugin.input.gschema.xml',
	'org.wayfire.plugin.invert.gschema.xml',
	'org.wayfire.plugin.matcher.gschema.xml',
	'org.wayfire.plugin.move.gschema.xml',
	'org.wayfire.plugin.oswitch.gschema.xml',
	'org.wayfire.plugin.place.gschema.xml',
	'org.wayfire.plugin.resize.gschema.xml',
	'org.wayfire.plugin.switcher.gschema.xml',
	'org.wayfire.plugin.tile.gschema.xml',
	'org.wayfire.plugin.vswipe.gschema.xml',
	'org.wayfire.plugin.vswitch.gschema.xml',
	'org.wayfire.plugin.wobbly.gschema.xml',
	'org.wayfire.plugin.wrot.gschema.xml',
	'org.wayfire.plugin.zoom.gschema.xml')

settings_schemas = gnome.compile_schemas(build_by_default: true,
	depend_files: xml_schemas)

install_data(xml_schemas,
	install_dir: 'share/glib-2.0/schemas')
