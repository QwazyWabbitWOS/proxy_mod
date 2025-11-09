ProxyMod v1.03 -- Basic Proxy Game Module for Quake II
Copyright © 1998, by Avi Rozen (Zung!)
e-mail: zungbang@telefragged.com

Intro
=====
This is a basic proxy mod (plug-in mod) for Quake II (v3.17 thru 3.20).
ProxyMod does not add any new functionality,
it only demonstrates how a proxy module should be built and
used. 

Installation
============
1. Create a new folder named c:\quake2\release (replace c:\quake2
   with the correct path)
2. Extract gamex86.dll to this new folder

To uninstall simply delete the new gamex86.dll

Usage
=====
Just start up the server as usual and ProxyMod
will be loaded automatically before the real game dll.

In itself ProxyMod provides no functionality, but it can
be used as a loader for other proxy mods based on it.

To load any number of proxy modules you should follow the
next procedure:

1. place one proxy module in the c:\quake2\release folder
   (I use ProxyMod - but you can use any other)
2. create a new folder: c:\quake2\proxy
3. place all other proxy mods in their own folders:
   
   GameCam - c:\quake2\proxy\gamecam\
   EntZ    - c:\quake2\proxy\entz\
   ZKick   - c:\quake2\proxy\zkick
   GameBot - c:\quake2\proxy\gamebot

   etc.

4. to run a server with ZKick, EntZ and GameCam:

   quake2 +set proxy zkick:entz:gamecam  <rest_of_command_line>

   (set the proxy cvar to a colon-separated list of proxy mods
   you want to load)

   This will load ProxyMod, and then ZKick, EntZ and GameCam, and
   then the real game dll will be loaded from the folder specified
   by the 'game' cvar.


For more info visit: 

	http://www.telefragged.com/zungbang/gamecam


Legal Stuff
===========
ProxyMod is copyright by Avi "Zung!" Rozen.
ProxyMod is distributed under the GNU license (see the file
"Copying." in this archive). For more info visit: 
	
	http://www.fsf.org


Avi "Zung!" Rozen.


Change Log:
==========

02 Feb. 1998 v1.00
	- created (Quake2 v3.10)

29 Jun. 1998 v1.01
	- made game API compatible with Quake2 v3.15
	- proxies in a chain are reloaded when game is changed
	- fixed some bugs that caused infinite loops

17 Jul. 1998 v1.02
	- tested with Quake2 v3.17
	- will not attempt to use NULL as folder name

17 Jul. 1998 v1.03
	- use "/" instead of "\" as path separator

14 Feb. 1999 v1.03
        - updated readme.txt for public release





