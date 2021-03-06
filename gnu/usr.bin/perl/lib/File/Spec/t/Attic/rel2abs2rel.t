#!./perl -w

# Herein we apply abs2rel, rel2abs and canonpath against various real
# world files and make sure it all actually works.

BEGIN {
    chdir 't';
    @INC = '../lib';
}
BEGIN {                                # Set up a tiny script file
    open(F, ">rel2abs2rel$$.pl")
      or die "Can't open rel2abs2rel$$.pl file for script -- $!\n";
    print F qq(print "ok\\n"\n);
    close(F);
}
END {
    unlink("rel2abs2rel$$.pl");
    unlink("rel2abs2rel$$.tmp");
}

use Config;

use Test::More tests => 5;
use File::Spec;

# Change 'perl' to './perl' so the shell doesn't go looking through PATH.
sub safe_rel {
    my($perl) = shift;
    $perl = File::Spec->catfile(File::Spec->curdir, $perl) unless
      File::Spec->file_name_is_absolute($perl);

    return $perl;
}
# Make a putative perl binary say "ok\n". We have to do it this way
# because the filespec of the binary may contain characters that a
# command interpreter considers special, so we can't use the obvious
# `$perl -le "print 'ok'"`. And, for portability, we can't use fork().
sub sayok{
    my $perl = shift;
    open(STDOUTDUP, '>&STDOUT');
    open(STDOUT, ">rel2abs2rel$$.tmp")
        or die "Can't open scratch file rel2abs2rel$$.tmp -- $!\n";
    system($perl, "rel2abs2rel$$.pl");
    open(STDOUT, '>&STDOUTDUP');
    close(STDOUTDUP);
    open(F, "rel2abs2rel$$.tmp");
    local $/ = undef;
    my $output = <F>;
    close(F);
    return $output;
}

# Here we make sure File::Spec can properly deal with executables.
# VMS has some trouble with these.
my $perl = safe_rel($^X);
is( sayok($perl), "ok\n",   '`` works' );

$perl = File::Spec->rel2abs($^X);
is( sayok($perl), "ok\n",   '`` works' );

$perl = File::Spec->canonpath($perl);
is( sayok($perl), "ok\n",   'rel2abs($^X)' );

$perl = safe_rel(File::Spec->abs2rel($perl));
is( sayok($perl), "ok\n",   'canonpath on abs executable' );

$perl = safe_rel(File::Spec->canonpath($^X));
is(sayok($perl), "ok\n",   'canonpath on rel executable' );
