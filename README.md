# dsmp

![Download]( "img/icon.png" )

dsmp is a simple command line media player that uses SDL for audio sequencing.


## Setup and Dependencies

dsmp depends on SDL 1.2 (ancient, I know) and some sort of Unix subsystem.

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



## Bugs / Issues

dsmp is a project I get very little time to work on.  With this being the case there are a few issues I have not had time to look at:

- Crashes on skip forward and backward
- Cannot play anything but WAV files, this is not good
- Get rid of SDL dependency	



<!-- Style and more... -->
<link rel="stylesheet" type=text/css href="style.css">
<link rel="stylesheet" type=text/css href="https://fonts.googleapis.com/css?family=Righteous|PT+Serif">
