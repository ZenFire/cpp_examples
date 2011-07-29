Building on Windows
---------

* Copy cpp_examples into the root directory of the extracted release.
* Open a Visual Studio 2010 Command Prompt.  Use the Windows SDK 7.1 Command Prompt instead when compiling against the 64 bit library.
* Navigate to the cpp_examples directory.
* Run msbuild.exe examples[64].sln /p:Configuration=Release to build the examples.  Use examples64.sln when compiling against the 64 bit library.
