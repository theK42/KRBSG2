# KRBSG2
Kelson's Really Boring Space Game 2

Don't expect to have fun.  Requires CMake.

Windows:  Default CMake build should work, but make sure you specify a build directory other than the KRBSG2 root.  Tested with Visual Studio 2019

MacOS:  `cmake . --preset MacOS` then open the xcode project in build_macos and select the KRBSG target to build and run.

Linux:  I'm told this can work but I haven't tested it myself and it might require some setup.

WebASM: `cmake. --preset WebASM` then use cd into the build_webasm directory and run make.  You'll need to host the build_webasm directory on a webserver to run.  This assumes a specific location and version of emscripten is installed, please let me know if you have a good idea for fixing that.  
