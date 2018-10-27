#!/usr/bin/perl

use strict;
use v5.24;
use File::Temp qw(tempdir);
use File::Copy qw(copy);
use File::Path qw(rmtree);
use Cwd;
use Carp;


#rm -fr build
#rm -fr dist
#rm -fr pcm2mp3.egg-info

sub getVersion {
	open(my $cfg,'<','setup.cfg');
	while (<$cfg>) {
		if($_ =~ /\Aversion = ([0-9.]+)\s*\Z/) { 
			close $cfg;
			return $1; 
		}
	}
	close $cfg;
	undef;
}

my $arg=shift @ARGV;




my $version=getVersion;
say "Processing version $version";

`sudo rm -fr /usr/local/lib/python3.6/dist-packages/pcm2mp3*`; 
`rm -fr /home/julianporter/.cache/Python-Eggs/pcm2mp3*`;
`python3 setup.py build`;
`python3 setup.py bdist`;

my $pwd = cwd;

chdir "/";
`sudo tar xzf /opt/projects/singlr/pcm2mp3/dist/pcm2mp3-$version.linux-x86_64.tar.gz`;
chdir $pwd;

`rm test.mp3`;
say "Implementing test";
my $out=`python3 test.py $arg`;
say $out;

say "Checking output";
say `mp3check -v -l test.mp3`;




