#!/usr/bin/perl
# Author: atrmat@gmail.com
# Ceph nagios plugin
# Copyright (c) 2013
# All rights reserved.
#
# function: check the pg status
#

use strict;
use Getopt::Long qw(:config no_ignore_case);

my ($mon_ips, $key);
my $CEPH = '/usr/bin/ceph';
my $HELP = 0;

my %STATUSCODE = (
	'OK'       => '0',
	'WARNING'  => '1',
	'CRITICAL' => '2',
	'UNKNOWN'  => '3',
);

my $usage = <<EOF;

This plugin checks the osds state of a ceph cluster.

Usage: check_ceph [-m|--mon] [-k|--key]
 -m, --mon=ADDRESS[,ADDRESS,ADDRESS]
   IP address(es) of ceph monitors
 -k, --key=string
  secret key to access the ceph cluster
 -h, --help
   Print detailed help screen

EOF

my $result = GetOptions(
	"m|mon=s" => \$mon_ips,
	"k|key=s" => \$key,
	"h|help"  => \$HELP,
);

if ( $HELP || !$result ) {
	print $usage;
	exit ($STATUSCODE{'UNKNOWN'});
}

if (! -x $CEPH) {
	print "Where is the ceph binary?  Not here: $CEPH\n";
	exit ($STATUSCODE{'UNKNOWN'});

}

# run ceph command and grab output
my $cephcmd = "ceph ";
$cephcmd .= "-m $mon_ips " if $mon_ips;
$cephcmd .= "--key $key " if $key;
$cephcmd .= "pg stat";
open(CEPHOUT, "$cephcmd |") or exit ($STATUSCODE{'UNKNOWN'});
while (defined( my $line = <CEPHOUT> )) {
	chomp $line;
    my $cluster_status = '';
	# matching on the expected second line of output with the arrow pointing to the right (->)
    #if (($cluster_status) = ($line =~ /.+\ ->\ \'(HEALTH_(OK|WARN|ERR).*)\'.*/)) {
    if (($cluster_status) = ($line)) {
		print "status: '$cluster_status'\n";
		my $cephdump = "ceph ";
		$cephdump .= "-m $mon_ips " if $mon_ips;
		$cephdump .= "--key $key " if $key;
		$cephdump .= "pg dump";
		open(CEPHDUMPOUT, "$cephdump |") or exit ($STATUSCODE{'UNKNOWN'});
		print "pg dump: \n";
		print <CEPHDUMPOUT>;
		#@pgstatus=split(/ /,$cluster_status);
		if ($cluster_status =~ /creating|active|clean|peering/) {
			#print "ok\n";
			exit ($STATUSCODE{'OK'});
		}
        	if ($cluster_status =~ /recover|replay|repair/) {
			exit ($STATUSCODE{'WARNING'});
		}
		elsif ($cluster_status =~ /stale|down/) {
			exit ($STATUSCODE{'CRITICAL'});
		}
		else {
			exit ($STATUSCODE{'UNKNOWN'});
		}
    }
    print $cluster_status;
}
close (CEPHOUT);

# if we got to here, something didn't work right...
exit ($STATUSCODE{'UNKNOWN'});
