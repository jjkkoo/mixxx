#!/usr/bin/perl

#
# Perl script for installing Mixxx on Linux.
# 
# August 2003, Tue Haste Andersen <haste@diku.dk>
#

# If you change the base install dir, you have to make changes to
# src/mixxx.pro and recompile first
$BASE='/usr'; 

# Extract version info from README file
open(F, 'README');
$line = <F>;
chop($line);
close(F);
($t1,$t2,$t3,$version) = split(' ',$line);
$version = substr($version, 0, -1);

# Check if this is running on Linux
$_ = `uname`;
if (!m/Linux/) 
{
	die('This script only works on Linux');
}

# Check if mixxx exists
@out = `ls src/mixxx`;
if ($#out<0)
{
	die('You need to compile Mixxx first');
}

# Copy skins to $BASE/share/mixxx
`install -m 755 -d $BASE/share/mixxx/skins/outline`;
`install -m 755 -d $BASE/share/mixxx/skins/traditional`;
`install -m 755 -d $BASE/share/mixxx/skins/outlineClose`;
`install -m 755 -d $BASE/share/mixxx/skins/outlineSmall`;
`install -m 644 src/skins/outline/* $BASE/share/mixxx/skins/outline`;
`install -m 644 src/skins/outlineClose/* $BASE/share/mixxx/skins/outlineClose`;
`install -m 644 src/skins/outlineSmall/* $BASE/share/mixxx/skins/outlineSmall`;
`install -m 644 src/skins/traditional/* $BASE/share/mixxx/skins/traditional`;

# Copy midi config files
`install -m 755 -d $BASE/share/mixxx/midi`;
`install -m 644 src/midi/* $BASE/share/mixxx/midi`;

# Copy keyboard config files
`install -m 755 -d $BASE/share/mixxx/keyboard`;
`install -m 644 src/keyboard/* $BASE/share/mixxx/keyboard`;

# Copy mixxx binary to $BASE/bin
`install -m 755 -d $BASE/bin`;
`install -m 755 src/mixxx $BASE/bin`;
`install -m 755 src/mixxx-with-jack $BASE/bin`;

# Copy doc files
`install -m 755 -d $BASE/share/doc/mixxx-$version`;
`install -m 644 README $BASE/share/doc/mixxx-$version`;
`install -m 644 LICENSE $BASE/share/doc/mixxx-$version`;
`install -m 644 COPYING $BASE/share/doc/mixxx-$version`;
`install -m 644 Mixxx-Manual.pdf $BASE/share/doc/mixxx-$version`;

printf("Install finished\n");
printf("\n");
printf("Start Mixxx by writing mixxx at the command prompt. If you have\n");
printf("the Jack sound server installed you can start Mixxx with mixxx-with-jack\n");
printf("\n");
