#!./perl -Tw

BEGIN {
    chdir 't' if -d 't';
    @INC = '../lib';
    require Config; import Config;
    if ($^O ne 'VMS' and $Config{'extensions'} !~ /\bPOSIX\b/) {
	print "1..0\n";
	exit 0;
    }
}

use Test::More tests => 7;
use Scalar::Util qw/tainted/;


use POSIX qw(fcntl_h open read mkfifo);
use strict ;

$| = 1;

my $buffer;
my @buffer;
my $testfd;

# Sources of taint:
#   The empty tainted value, for tainting strings

my $TAINT = substr($^X, 0, 0);

# there is a bug in GUSI that causes problems trying to open
# files and directories ... it is being fixed, this is just
# a stopgap -- pudge
my $file = $^O eq 'MacOS' ? 'TEST-OLD' : 'TEST';

eval { mkfifo($TAINT. $file, 0) };
like($@, qr/^Insecure dependency/,              'mkfifo with tainted data');

eval { $testfd = open($TAINT. $file, O_WRONLY, 0) };
like($@, qr/^Insecure dependency/,              'open with tainted data');

eval { $testfd = open($file, O_RDONLY, 0) };
is($@, "",                                  'open with untainted data');

read($testfd, $buffer, 2) if $testfd > 2;
is( $buffer, "#!",	                          '    read' );
ok(tainted($buffer),                          '    scalar tainted');

TODO: {
    local $TODO = "POSIX::read won't taint an array element";

    read($testfd, $buffer[1], 2) if $testfd > 2;

    is( $buffer[1], "./",	                      '    read' );
    ok(tainted($buffer[1]),                       '    array element tainted');
}
