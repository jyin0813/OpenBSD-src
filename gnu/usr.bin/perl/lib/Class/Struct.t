#!./perl -w

BEGIN {
	chdir 't' if -d 't';
	@INC = '../lib';
}

print "1..12\n";

package aClass;

sub new { bless {}, shift }

sub meth { 42 }

package RecClass;

sub new { bless {}, shift }

package MyObj;

use Class::Struct;
use Class::Struct 'struct'; # test out both forms

use Class::Struct SomeClass => { SomeElem => '$' };

struct( s => '$', a => '@', h => '%', c => 'aClass' );

my $obj = MyObj->new;

$obj->s('foo');

print "not " unless $obj->s() eq 'foo';
print "ok 1\n";

my $arf = $obj->a;

print "not " unless ref $arf eq 'ARRAY';
print "ok 2\n";

$obj->a(2, 'secundus');

print "not " unless $obj->a(2) eq 'secundus';
print "ok 3\n";

my $hrf = $obj->h;

print "not " unless ref $hrf eq 'HASH';
print "ok 4\n";

$obj->h('x', 10);

print "not " unless $obj->h('x') == 10;
print "ok 5\n";

my $orf = $obj->c;

print "not " if defined($orf);
print "ok 6\n";

$obj = MyObj->new( c => aClass->new );
$orf = $obj->c;

print "not " if ref $orf ne 'aClass';
print "ok 7\n";

print "not " unless $obj->c->meth() == 42;
print "ok 8\n";

my $obk = SomeClass->new();

$obk->SomeElem(123);

print "not " unless $obk->SomeElem() == 123;
print "ok 9\n";

$obj->a([4,5,6]);

print "not " unless $obj->a(1) == 5;
print "ok 10\n";

$obj->h({h=>7,r=>8,f=>9});

print "not " unless $obj->h('r') == 8;
print "ok 11\n";

my $recobj = RecClass->new() or print "not ";
print "ok 12\n";

