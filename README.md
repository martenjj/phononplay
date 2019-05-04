Phonon and VLC Player Tools
===========================

These tools allow media files in any supported format to be played
through either the KDE Phonon sound system, or directly via the VLC
libraries, from the command line.  They may be useful for either
playing sounds from shell scripts, or for debugging sound output.

The package and all the files contained in it is licensed under the
GNU GPL V3+; see the file LICENSE for more information.


Requirements
------------

To build the tools the KDE Frameworks 5, Phonon and VLC libraries are
required.  CMake is also required, but there is no need for Autotools.


Building and installing
-----------------------

Assuming that you have the requirements as above installed or already
provided by your distro, go to a suitable build location (e.g. your
home directory) and do:

     git clone https://github.com/martenjj/phononplay.git
     cd phononplay
     mkdir build
     cd build
     cmake ..
     make
     sudo make install


Running
-------

The commands <tt>phononplay</tt> and <tt>vlcplay</tt> will be
available.  In their simplest invocation, specify a media file to be
played, for example:

    $ phononplay ring.mp3
    $ vlcplay ring.mp3

Each command has a listing mode which will show the available output devices:

    $ phononplay -l
    0    alsa:default:CARD=SB
         HDA ATI SB, ALC888 Analog Default Audio Device
    1    alsa:null
         Discard all samples (playback) or generate zero samples (capture)
    2    alsa:sysdefault:CARD=SB
         HDA ATI SB, ALC888 Analog Default Audio Device
    3    alsa:default
         Default
    $ vlcplay -D
    Available VLC output devices for module 'alsa':
    0    null
         Discard all samples (playback) or generate zero samples (capture)
    1    default:CARD=SB
         HDA ATI SB, ALC888 Analog Default Audio Device
    2    sysdefault:CARD=SB
         HDA ATI SB, ALC888 Analog Default Audio Device
    3    default
         Default

An output device can be specified to direct the sound output to the
appropriate device.  The device can be a name or a device index as
listed by the commands above:

    $ phononplay -d alsa:sysdefault:CARD=SB ring.mp3
    $ vlcplay -d 2 ring.mp3

Each command also has a help mode (using the option <tt>-h</tt>) which
will show the command syntax and all available options.


Problems?
---------

Please raise an issue on GitHub (at http://github.com/martenjj/phononplay)
if there are any problems with installing or using this package.


Thanks for your interest!
-------------------------

Jonathan Marten, http://github.com/martenjj
