#!/bin/sh
: ${PREFIX:="/usr/local"}
for p in $(ls "$PREFIX/share/wayfire/metadata"); do
	xsltproc -o "$PREFIX/share/glib-2.0/schemas/org.wayfire.plugin.${p%.xml}.gschema.xml" \
		genschema.xsl \
		"$PREFIX/share/wayfire/metadata/$p"
done
glib-compile-schemas "$PREFIX/share/glib-2.0/schemas"
