<A name="toc2-1" title="To Build" />
To Build
--------

To build, use these commands in this directory, after copying cpp_examples
into the extracted directory for libzenfire.

* mkdir cpp_examples/build
* cd cpp_examples/build
* make -f ../Makefile

If you're building against the 32 bit library on a 64 bit system, you'll need
to use this command instead:

* USE32=yes make -f ../Makefile

That was easy!
