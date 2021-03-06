#!./perl

BEGIN {
    unless (-d 'blib') {
	chdir 't' if -d 't';
	@INC = '../lib';
	require Config; import Config;
	keys %Config; # Silence warning
	if ($Config{extensions} !~ /\bList\/Util\b/) {
	    print "1..0 # Skip: List::Util was not built\n";
	    exit 0;
	}
    }
}


use List::Util qw(sum);

print "1..3\n";

print "not " if defined sum;
print "ok 1\n";

print "not " unless sum(9) == 9;
print "ok 2\n";

print "not " unless sum(1,2,3,4) == 10;
print "ok 3\n";

