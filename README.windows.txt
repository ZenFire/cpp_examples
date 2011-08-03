Building on Windows
---------

* Copy cpp_examples into the root directory of the extracted release.
* Open a Visual Studio 2010 Command Prompt.  Use the Windows SDK 7.1 Command Prompt instead when compiling against the 64 bit library.
* Navigate to the cpp_examples directory.
* Run msbuild.exe all_examples.sln /p:Configuration=Release /p:Platform=x64|Win32 to build the examples.  Choose one of x64 or Win32 in this command.
