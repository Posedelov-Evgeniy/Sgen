#!/bin/bash

sourceDir=$1
buildDir=$2
mode=$3

echo "Deploying";

if [ "$sourceDir" = '' ] ; then
    exit 0
fi
if [ "$buildDir" = '' ] ; then
    exit 0
fi
if [ -e "$buildDir/functions.cpp.cfg" ] ; then
    echo "Files already deployed. Exit.";
    exit 0
fi

cp "$sourceDir/config.cfg" "$buildDir"
cp "$sourceDir/functions.cpp.cfg" "$buildDir"
cp "$sourceDir/base_functions.cpp" "$buildDir"
cp "$sourceDir/base_functions.h" "$buildDir"

mkdir -p "$buildDir/translations"
cp "$sourceDir/translations"/*.qm "$buildDir/translations"

mkdir -p "$buildDir/examples"
cp "$sourceDir/examples"/*.* "$buildDir/examples"

if [ $mode = 'release' ] ; then
    mkdir -p "$buildDir/api"
    mkdir -p "$buildDir/api/linux"
    mkdir -p "$buildDir/api/linux/lib"
    cp -r "$sourceDir/api/linux/lib"/* "$buildDir/api/linux/lib/"
fi

echo "Deploying is finished";
