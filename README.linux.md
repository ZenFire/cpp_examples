<A name="toc1-0" title="Build Instructions" />
# Build Instructions

<A name="toc2-3" title="Requirements" />
## Requirements

g++>=4.4, libssl-dev.  Refer to your distribution's documentation to find out how to install these.

<A name="toc2-8" title="Step-by-step" />
## Step-by-step

To build, use these commands in this directory, after copying cpp_examples
into the extracted directory for libzenfire.

* mkdir cpp_examples/build
* cd cpp_examples/build
* make -f ../Makefile

If you're building against the 32 bit library on a 64 bit system, you'll need
to use this command instead:

* USE32=yes make -f ../Makefile

That was easy!

If you'd like to build the examples against the shared library instead of the static library, add USESHARED=yes to the environment of make (just like USE32 above).  You'll need to set LD_LIBRARY_PATH to include the directory containing the .so files in the distributed library to run the built examples.
