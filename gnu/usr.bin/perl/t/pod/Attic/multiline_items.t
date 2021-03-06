#!./perl
BEGIN {
   chdir 't' if -d 't';
   unshift @INC, './pod', '../lib';
   require "testp2pt.pl";
   import TestPodIncPlainText;
}

my %options = map { $_ => 1 } @ARGV;  ## convert cmdline to options-hash
my $passed  = testpodplaintext \%options, $0;
exit( ($passed == 1) ? 0 : -1 )  unless $ENV{HARNESS_ACTIVE};


__END__


=head1 Test multiline item lists

This is a test to ensure that multiline =item paragraphs
get indented appropriately.

=over 4 

=item This 
is
a
test.

=back

=cut
