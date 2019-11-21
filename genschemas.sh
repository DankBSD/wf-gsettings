#!/bin/sh
WCM=../../WayfireWM/wcm
for p in $(ls $WCM/metadata | grep .xml); do
	xsltproc -o "schemas/org.wayfire.plugin.${p%.xml}.gschema.xml" genschema.xsl "$WCM/metadata/$p"
done
