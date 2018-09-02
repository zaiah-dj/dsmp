dsmp
----
Simple command line media player that uses SDL for audio sequencing.


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
| p         | Pause playing audio.                     |  
| a         | Seek backwards in a song.                |  
| s         | Seek forwards in a song.                 |  
| w         | Lower volume.                            |  
| e         | Increase volume.                         |
| r         | Start at the beginning?                  |  
| j         | Move to the next song in a directory.    |  
| k         | Move to the previous song in a directory.|  



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



## Caveats

Sadly, there are a few right now and the list will be maintained at:

[Bug List]( http://ramarcollins.com/dsmp/bugs.html )

Since I don't have time to get to it right now, let's maintain it here:
- Crashes on skip forward and backward
- Cannot play anything but WAV files, this is not good
- Get rid of SDL dependency	



<!-- Style and more... -->
<link rel="stylesheet" type=text/css href="dsmp.css">
<link rel="stylesheet" type=text/css href="https://fonts.googleapis.com/css?family=Righteous">
