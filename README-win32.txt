Extension Manager on Windows
============================

Requirements
------------
* Extension Manager sources
* You need Visual Studio or compatible compiler (Windows Libraries)
* You need to download the full source code of PHP 5 and the archive with the precompiled PHP 5 binary (or build PHP 5 yourself from source)
  Download the PHP sources and binaries for the version of PHP you wish to use Extension Manager with. The downloads can be obtained from <http://www.php.net/downloads.php>. The sources are compressed in two formats .tar.gz and .tar.bz2. If you use WinZip, download the .tar.gz format. WinRAR is capable of extracting .tar.bz2 formats.

Note: The reason why you need the binary package is because it is not necessary to compile PHP sources; you only need the .lib file that is included in the binary distribution and PHP sources.

Instructions
------------
1) Unzip the PHP source to a directory, we will use c:\php for this example.

2) Unzip the Extension Manager source in the ext\ subdirectory of the PHP source.
   You should have something like c:\php, c:\php\ext and c:\php\ext\manager

3) Unpack binary PHP in any other directory, and search for the .lib file. php5ts.lib is located in the dev\ subdirectory of the root folder.
   Copy the .lib file to the PHP sources root directory that you unpacked the source files to earlier. For our example, this is in c:\php. This file will be used for linking

4) Start Visual Studio and open the Extension Manager project:
	File > Open > Project/Solution ...
	The project is located in the win32 subdirectory of the Extension Manager source (c:\php\ext\manager\win32\manager.sln)

6) Go to Build > Set active configuration ... and select the target you want to build. Included Visual Studio profiles are:
	Debug PHP5
	Release PHP5
Select the profile according the PHP version you will compile. 
Debug PHP5 and Release PHP5 are used for PHP5. For our example, we will compile with Release PHP5.

7) Go to Build > Build manager.dll to build the manager extension.

The .dll file will be created in sub-directories according to profile selection. Our example will place the compiled .dll file in c:\php\ext\manager\win32\Release

------------
* Windows is a registered trademark of Microsoft Corporation in the United States and other countries.
* WinRAR is a registered trademark of win.rar GmbH.
* WinZip is a Registered Trademark of WinZip International LLC