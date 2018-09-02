# dsmp

![Download]( "img/icon.png" )

dsmp is a simple command line media player that uses SDL for audio sequencing.


## Setup and Dependencies

dsmp depends on SDL 1.2 (ancient, I know) and some sort of Unix subsystem. In
other words, Windows users probably won't be able to run this, but Linux, BSD,
and OSX users should be fine.

To build dsmp, download the latest copy and run:
<pre>
$ make && sudo make install
</pre>


## Usage

Here is the complete list of CLI options thus far:

| Flag                 |  Description
| ---------------------|------------------------------------------
|-f, --file (arg)      |  Play one file from the command line.
|-l, --loop            |  Enable looping. 
|-d, --dir (arg)       |  Play all audio files in a chosen directory.
|-h, --help            |  Show help and quit.


Within the app, there are a few simple controls:

| Command   | Description                              |
| ----------|------------------------------------------|
|  P        | Pause playing audio.                     |  
|  A        | Seek backwards in a song.                |  
|  S        | Seek forwards in a song.                 |  
|  W        | Lower volume.                            |  
|  E        | Increase volume.                         |
|  R        | Start at the beginning?                  |  
|  J        | Move to the next song in a directory.    |  
|  K        | Move to the previous song in a directory.|  


## Screenshots

![Pretty Colors]( "img/icon.png" )


## Extension and Hacking

Flags that are supported:

| Flag      | Description
| ----------|------------------------------------------
|-DENABLE_VERSION_BANNER | Turns version announcement on or off.
|-DPLAY_EMBEDDED         | Use embedded audio instead of an external audio file.
|-DNO_FORK | Do not compile the ability to fork into the background.
|-DNO_KEY | Do not compile the ability to process keypresses.
|-DENABLE_LOG | Tell me everything that the program is doing, all the time.


## Bugs / Issues

- Crashes on skip forward and backward
- Cannot play anything but WAV files, this is not good
- Get rid of SDL dependency	


## Author Information

The guts of this program were written by me, Antonio R. Collins II, during a weekend when I had too much time on my hands.
Fixes are merged in when I have the time and inclination to write them.
You should reach out to me for any issues that you may encounter while using this program.
Either of the following emails will work fine.

- ramar@tubularmodular.com
- ramar.collins@gmail.com


<!-- Style and more... -->
<link rel="stylesheet" type=text/css href="style.css">
<link rel="stylesheet" type=text/css href="https://fonts.googleapis.com/css?family=Righteous|PT+Serif">
