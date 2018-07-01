dsmp
----
Dead simple music player.  It should just work.  But it uses SDL.  Cuz I'm lazy.

## Usage

Here is the complete list of options thus far:
<pre>
<code>
-l, --loop               Enable looping. 
-d, --dir (arg)          Choose audio files from $dir.
-p, --play (arg)         Play a file on the command line.
-x, --no-fork (arg)      Use $arg resolution.
</code>
</pre>

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

</p>


## Screenshots


## Extension and Hacking

Flags that are supported:
<pre>
<code>
-DENABLE_VERSION_BANNER - Turns version announcement on or off.
-DPLAY_EMBEDDED         - Use embedded audio instead of an external audio file.
</code>
</pre>

## Caveats



<!-- Style and more... -->
<link rel="stylesheet" type=text/css href="index.css">
<link rel="stylesheet" type=text/css href="https://fonts.googleapis.com/css?family=Righteous">
