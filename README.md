<html>
<head>
<link rel="stylesheet" type=text/css href="index.css">
<link rel="stylesheet" type=text/css href="https://fonts.googleapis.com/css?family=Righteous">
</head>
<h1>dsmp</h1>
Dead simple music player.  It should just work.  But it uses SDL.  Cuz I'm lazy.

<h2>Usage</h2>
<p>
Here is the complete list of options thus far:
<pre>
<code>
-l, --loop               Enable looping. 
-d, --dir (arg)          Choose audio files from $dir.
-p, --play (arg)         Play a file on the command line.
-x, --no-fork (arg)      Use $arg resolution.
</code>
</pre>
</p>

<p>

Within the app, there are a few simple controls:

| Command   | ?                                        |
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


<h2>Screenshots</h2>
<p>
</p>


<h2>Extension and Hacking</h2>
<p>
</p>

<p>
Flags that are supported:
<pre>
<code>
-DENABLE_VERSION_BANNER - Turns version announcement on or off.
-DPLAY_EMBEDDED         - Use embedded audio instead of an external audio file.
</code>
</pre>
</p>


<h2>Caveats</h2>
<p>
</p>


</html>
