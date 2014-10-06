GunsnDiamonds
=============

Guns n' Diamonds is a bejeweled-like game developed using C and the Allegro gaming library.


------------------
Linux Instructions 
------------------

The 'release' folder has a binary file for Debian 7.6.0 amd64 (release/jogo_linux). To run, you need to install the Allegro 4.4 libraries (liballegro4.4).

You need install the Allegro 4.2 development libraries package in order to compile the game (liballegro4.2-dev)
Go to the 'src' directory and rename 'Makefile.linux' to 'Makefile' and run the command 'make'. Alternativelly, you can just run 'make -f Makefile.linux'.


--------------------
Windows Instructions 
--------------------

The 'release' folder has a binary file for Windows 7 32/64 bits (release/jogo_win7.exe). To run, you just need the Allegro 4.2 dll file (already in the 'release' folder).

In order to compile, you will need the MinGW and the Allegro 4.2 development library installed, under the MinGW directory. In 'src' directory, rename 'Makefile.windows' to 'Makefile' and run 'mingw32-make' or just run the command 'mingw32-make -f Makefile.Windows'.



--------------
Release Notes
--------------

Version 1.93
	- Included ALMP3 library. Playing mp3 intro music.
	- Code refactor

