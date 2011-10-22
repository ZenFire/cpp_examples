Building on Windows
---------

* Copy cpp_examples into the root directory of the extracted release.
* Open a Visual Studio 2010 Command Prompt.  Use the Windows SDK 7.1 Command Prompt instead when compiling against the 64 bit library.
* Navigate to the cpp_examples directory.
* Run `msbuild.exe all_examples.sln /p:Configuration=Release|Debug /p:Platform=x64|Win32` to build the examples.  Choose one of x64 or Win32 in this command and Release or Debug.  When using a MDd and MTd ZenFire API release, use Debug. When using MD or MT, use Release.  Use x64 for the 64 bit builds, and Win32 for the 32 bit builds.  If you are using an MD or MDd release, use `all_examples_dynamic.sln` instead.
