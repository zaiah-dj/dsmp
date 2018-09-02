/* -------------------------------------- *
dsmp
----

@summary
	Simple command line media player
@end

@usage
	-l, --loop      Enable looping.
			--describe	Describe an audio stream.
	-f, --file      Choose a file to play.
	-x, --no-fork   Leave in the foreground.
	-d, --dir       Select a directory.
@end

@building
	All you need is:
	make && make install 
@end

@todo
	- Crashes on skip forward and backward
	- Cannot play anything but WAV files, this is not good
	- Get rid of SDL dependency	
@end

@changelog
	Simple command line media player
@end
 * -------------------------------------- */
//#define _POSIX_C_SOURCE 2
#define PROG "dsmp"
#define NAME "dsmp"
#define AUTHOR "Antonio R. Collins II"
#define PID "/tmp/"
#define PID_FILE_NAME_LENGTH 25
#define WIDTH 40
#define HEIGHT 40
#ifndef PATH_MAX
 #define PATH_MAX 2048
#endif

//#define NO_FORK
//#define NO_KEY

#include "vendor/single.h"
#include <stdio.h>
#include <SDL/SDL.h>
#include <signal.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/ioctl.h>

#define print_settings() \
	nsprintf( settings.filename ); \
	nsprintf( settings.dirname ); \
	niprintf( settings.nofork ); \
	niprintf( settings.loop );

#define reader() \
	fprintf(stderr, (read(0, &key, 1) == -1 && errno == EAGAIN) ? "try again...\n" : "Got code '%c'\n");

#define jerr(stat, ...) \
	while (1) { fprintf(stderr, "ERR: "); fprintf(stderr, __VA_ARGS__); return stat; }

#ifdef ENABLE_LOG
 #define log(...) fprintf( stderr, __VA_ARGS__ )
#else
 #define log(...)
#endif

#ifdef ENABLE_VERSION_BANNER
 #define print_version() \
	fprintf(stderr, "%s - v%.2f - compiled %s at %s\n", NAME, VERSION, __DATE__, __TIME__);
#else
 #define print_version()
#endif

#define clear() \
	write(1, "\033c", strlen("\033c")); 

#define pause_if_playing() \
	switch (SDL_GetAudioStatus()) { \
		case SDL_AUDIO_STOPPED: \
		case SDL_AUDIO_PAUSED: \
			break; \
		case SDL_AUDIO_PLAYING: \
			SDL_PauseAudio(1); \
			break; \
	}

//Increment the current position of audio buffer by samples received and some other incrementor (i)
#define cp(i) *(v->ap + v->sp + i) 

#ifndef NO_KEY
 #define set_eat() \
	struct termios old={0}; \
	fflush(stdout); \
	if (tcgetattr(0, &old) < 0) \
		fprintf(stderr, "Couldn't receive keypress: %s\n", strerror(errno)); \
	old.c_lflag &= ~ICANON; \
	old.c_lflag &= ~ECHO; \
	old.c_cc[VMIN] = 1; \
	old.c_cc[VTIME] = 1; \
	if (tcsetattr(0, TCSANOW, &old) < 0) \
		fprintf(stderr, "Termio set error: %s\n", strerror(errno))

 #define set_norm() \
	fprintf(stderr, "Read error: %s\n", strerror(errno)); \
	old.c_lflag |= ICANON; \
	old.c_lflag |= ECHO; \
	if (tcsetattr(0, TCSADRAIN, &old) < 0) \
		fprintf(stderr, "Termio reset error: %s\n", strerror(errno))
#else
 #define set_eat()
 #define set_norm()
#endif

//Globals and constants
const int8_t DEFAULT_FRAME_RATE = 20;
static int j=0;
static int64_t current_tv_sec=0;
char *dsmpDirname=NULL;
int buffsz = 64;
double samprt = 44100;
static FILE *wo=NULL;
int interval=0;
static int32_t sampRate = 44100;	
static int re=100;
static int xp=0, xe=0; 
const char *RNDCHR="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
char pid_buf[PID_FILE_NAME_LENGTH]={'/','t','m','p','/'};
const char *start_code = "\x1b[";
const char *reset_code = "\x1b[0m";
const char *color_names[] = {
	"red", "yellow", "blue", "violet", "cyan", "white", "gray"
};
//0-13
//const char *colors="30m31m33m34m35m36m37m40m41m43m44m45m46m47m";
const char *colors[] = {
"\x1b[30m","\x1b[31m","\x1b[33m","\x1b[34m",
"\x1b[35m","\x1b[36m","\x1b[37m",
"\x1b[40m","\x1b[41m","\x1b[43m","\x1b[44m",
"\x1b[45m","\x1b[46m","\x1b[47m"
};


typedef struct { 

	//Audio tracking
	uint8_t      *ap;   //Buffer as modified (but updates slowly)
	uint32_t      al;	  //Length as modified by audio_callback
	SDL_AudioSpec as;   
	uint32_t      sp;   //Position within the current buffer of samples
	uint8_t       vol;

	//General audio info
	char         *filename;
	char         *dirname;
	_Bool         paused;

	//Information passed by visual functions
	void         *opaque;
	_Bool         opqset;
	SDL_Event	    event;
} Av;


typedef struct {
	char filename[PATH_MAX];
	char artist[256];
	char track[256];
	const int32_t length;
	int hash;
	_Bool sentinel;
} Track;


typedef struct { 
	Track   *tracks;
	int32_t  count;
	char    *names;
} Playlist;


struct Settings {
	const char *filename, 
						 *dirname;
	_Bool       nofork,   
							loop;
}	settings = { NULL, NULL, 1, 0 };


#ifdef IVORBIS_H
void decode_vorbis ( ) {
}
#endif

#ifdef IFLAC_H
void decode_flac ( ) {
}
#endif

#ifdef ILAME_H
void decode_mpeglayer3 ( ) {
}
#endif


//Copy a path
void pathcpy (char *path, char *base, const char *filename) {
	snprintf(path, PATH_MAX, "%s/%s", base, filename);
}


//Load all audiofiles
uint32_t get_directory_count (DIR *dir) {
	//Load the directory here
	struct dirent *md;
	uint32_t c=0;
	while ((md = readdir(dir))) {
		c += (strlen(md->d_name) > 4 && strcmp(&md->d_name[strlen(md->d_name) - 4], ".wav") == 0);
	}
	return c;
}


//Load all audiofiles
_Bool load_directory (DIR *dir, Track *tracks, int32_t count) {
	//Load the directory here
	int hash=0, cnt=0;
	Track *trk = tracks;
	struct dirent *md;
	rewinddir(dir);
	while ((md = readdir(dir)) && cnt < count) {
		if (strlen(md->d_name) > 4 && strcmp(&md->d_name[strlen(md->d_name) - 4], ".wav") == 0) {
		#ifdef DEBUG
			fprintf(stderr, "Adding '%s' to playlist.\n", md->d_name);
		#endif
			//fprintf(stderr, "Hash: %d, count: %d\n", hash, cnt); 
			pathcpy(trk->filename, dsmpDirname, md->d_name);
			trk->hash = hash;
			trk++, hash++, cnt++;
		}
	}
	trk->sentinel = 1; 
	return 1;
}


//Print the track structure
void tracks_printf (Track *tracks) {
	Track *t = &tracks[0];
	while (!t->sentinel) {
	#ifdef DEBUG
		fprintf(stderr, "File:     %s\n", t->filename);
		fprintf(stderr, "hash:     %d\n", t->hash);
	#endif
		t++;
	}
}


//This is only for the things...
void get_term_size () {
	int fd=1;
	struct winsize W;
	ioctl(fd, TIOCGWINSZ, &W);
}


//???
void test_colors () {
	char *mytext = "blablablamcbla\n";
	for (int i=0; i<sizeof(colors)/sizeof(char *); i++) {	
		write(1, colors[i], strlen(colors[i]));
		write(1, "color", strlen("color"));
		write(1, mytext, strlen(mytext));
		write(1, reset_code, strlen(reset_code));
	} 
}


//???
void test_bar () {
	static int rate;
	int i=10;
	char buf[15]={0};
	snprintf(buf, 15, "     %2d     ", (rate += (rate < 60) ? 1 : -60));
	write(1, colors[i], strlen(colors[i]));
	write(1, buf, strlen(buf));
	write(1, reset_code, strlen(reset_code));
}

//0-60 = info screen?, 61-79 = sparklines ish interface

void write_membuf_as_screen (uint8_t *buf) {
	//why uint8_t?, support Unicode...	
}

void write_playlist (Track *track) {
	//Write each track in the playlist
}

void write_info (Track *track, void (*get_metadata)(const char *)) {

}

//This function can be greatly modified later.
void print_audiofile (SDL_AudioSpec *as) {
	const char *sfmt = "%-30s %s\n";
	const char *nfmt = "%-30s %d\n";
	//fprintf(wo, sfmt, "Filename",  *as->userdata)->filename); 
	fprintf(stderr, nfmt, "Frequency", as->freq); 
	fprintf(stderr, nfmt, "Format"   , as->format); 
	fprintf(stderr, nfmt, "Channels" , as->channels); 
	fprintf(stderr, nfmt, "Silence"  , as->silence); 
	fprintf(stderr, nfmt, "Samples"  , as->samples); 
	fprintf(stderr, nfmt, "Size"     , as->size); 
}


//Print the contents of audio structure
void print_av (Av *av) {
	fprintf(stderr, "Audio pointer:    %p\n", (void *)av->ap);
	fprintf(stderr, "Audio length:     %d\n", av->al);
	fprintf(stderr, "Audio spec (SDL): %p\n", (void *)&av->as);
	print_audiofile (&av->as);
}


//Play that audio for real
void audio_callback (void *userdata, uint8_t *stream, int len) {
	Av *av=(Av *)userdata;
	//print_av(av);
	if (av->al == 0)
		return;
	len = (len > (int)av->al ? (int)av->al : len ); // never seen this...
	SDL_MixAudio(stream, av->ap, len, av->vol);
	av->ap += len;
	av->al -= len;
}


//Options array
Option opts[] = {
	{ "-l", "--loop",     "Enable looping.", },
	{ "-f", "--file",     "Choose a file to play.", 's' /*1, .callback=opt_set_filename*/ },
	{ "-d", "--dir",      "Select a directory.", 's' /*1, .callback=opt_set_dir*/ },
	{ "-h", "--help",     "Show help and quit."  }, 
#if 0
	{ "-x", "--no-fork",  "Leave in foreground.", },
	{ NULL, "--describe", "Describe an audio stream."                 },
#endif
	{ .sentinel=1 }
};


/*main*/
int main (int argc, char *argv[]) {
	//Print banner and process options
	print_version();

	//Then go through and set each.
	( argc < 2 ) ? opt_usage( opts, "a", "Nothing to do.", 0 ) : opt_eval( opts, argc, argv );

	settings.loop = opt_set( opts, "--loop" ); 		
	settings.nofork = opt_set( opts, "--no-fork" ); 		
	settings.filename = opt_set( opts, "--file" ) ? opt_get(opts, "--file").s : NULL;
	dsmpDirname = opt_set( opts, "--dir" ) ? opt_get( opts, "--dir" ).s : NULL;

	if ( settings.filename && dsmpDirname ) {
		jerr( 0, "As currently written, specifying both --file and --dir makes no sense because " PROG " is operating in two disparate ways.  Specify just one of these options and we'll continue..." );
	}

	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		jerr(0, "Couldn't initialize audio abstraction layer.\n");
	}

	//Register signals and set up non-blocking reads
	//signal(SIGSEGV, SIG_DFL);
	signal(SIGINT, SIG_DFL);
	int flags = fcntl(0, F_GETFL, 0);
	fcntl(0, F_SETFL, flags | O_NONBLOCK);

	//Define a bunch of variables
	SDL_Event event;
	pid_t pid;
	DIR *dir;
	int32_t count=0;
	int16_t delay_rate = DEFAULT_FRAME_RATE;
	Av mv={.ap /*audio_pos*/ = NULL, .al /*audio_len*/= 0, .event=event};
	Track *track = NULL;
	uint8_t buf[WIDTH * HEIGHT]; //int32_t will accomodate unicode...
	memset(buf, 0, WIDTH * HEIGHT);

	//Load and initialize the audio
#ifdef PLAY_EMBEDDED
	uint8_t wav[] = {
		#include "audio.c"
	};
	mv.al = sizeof(wav)/sizeof(uint8_t);
	mv.ap = wav;  /*This is a guess of where the audio would start*/
	mv.as.freq = 44100;
	mv.as.channels = 2;
	mv.as.format = AUDIO_S16;
	/*mv.as.samples= 2048;
	mv.as.size = 8192;*/
	fprintf(stderr, "PLAY_EMBEDDED is enabled in build flags. Please disable it to play other files.\n");
#else

	if ( dsmpDirname ) {
		if (!(dir = opendir(dsmpDirname)))
			jerr(0, "Directory open failed...");

		if ((count = get_directory_count(dir)) < 0) 
			jerr(0, "Directory error.");	

		if (!(track = (Track *)malloc(sizeof(Track) * (count + 1))))
			jerr(0, "Couldn't load up all tracks...");
		
		if (!load_directory(dir, track, count))
			jerr(0, "Load error.");

		fprintf(stderr, "Directory '%s' contains %d files\n", dsmpDirname, count); 
		fprintf(stderr, "First file: %s\n", track->filename);
		
		if (closedir(dir) == -1) {
			jerr(0, "Error closing directory.");
		}
	
		if (!SDL_LoadWAV(track->filename, &(mv.as), &(mv.ap), &(mv.al))) {
			jerr(0, "Failed to load audio file: %s\n", track->filename);
		}	
		//tracks_printf(track);
	}

	else if ( settings.filename ) {
		//Die if there are no files in the buffer (for now)
		if ( !settings.filename ) {
			jerr(0, "No audio file supplied! (Try the --play flag to select one.)\n");
		}
	
		if (!SDL_LoadWAV(settings.filename, &(mv.as), &(mv.ap), &(mv.al))) {
			jerr(0, "Failed to load audio file: %s\n", settings.filename);
		}
	}

	else {
		fprintf(stderr, "ERR: No options supplied.\n");
		SDL_Quit();
		_exit(0);
	}

	print_settings();
#endif

	//Populate unknown data and describe the audio stream.
	mv.as.callback = audio_callback;
	mv.as.userdata = &mv;

	//Open a device and play...
	if ( SDL_OpenAudio(&(mv.as), NULL) < 0) {
		jerr(0, "Failed to open audio stream: %s\n", SDL_GetError());
	}

	//Show the dump
	print_av(&mv);	

 #ifndef NO_FORK 
	//Create a random file for you and for me.
	srand(mv.al);
	pid_buf[PID_FILE_NAME_LENGTH-1]='\0';
	for (int ii=5; ii<PID_FILE_NAME_LENGTH-2; ii++) {
		int i = rand() % 62;
		while (i < 0 || i > 62) i = rand() % 62;
		pid_buf[ii]=RNDCHR[i];
	}

	//Fork - suspend
	if ((pid = (settings.nofork) ? 0 : fork()) == -1) {
		jerr(0, "Error occurred while backgrounding: %s\n", strerror(errno));	
	}
	else if (!pid) {
 #endif
		//Play and control playback.
		int cts=0, m=0, gl = mv.al;
		mv.vol=SDL_MIX_MAXVOLUME;
		SDL_PauseAudio(mv.paused);
		set_eat();
		clear();

		//Set the screen
		while ( mv.al ) {
			//clear();
			//test_colors();
			//test_bar();
			uint8_t key=0;
			SDL_Delay(10);
			mv.sp += (mv.sp > 44100) ? -mv.sp : 100;

			//A crude way to handle moving to the next track?
			if ( ( mv.al < 200 ) && ( dsmpDirname ) ) {
				//track += ((track + 1)->sentinel) ? -(count-1) : 1;
				log( "current audio: %p, next audio: %p\n", (void*)track, (void*)(track + 1) );
				int proc = ( track + 1 )->sentinel; 

				//Decide how to proceed between the tracks
				if ( proc && !settings.loop ) {
					fprintf( stderr, "Thanks for using dsmp, bye!\n" );
					break;
				}
				else if ( proc && settings.loop ) { 
					track += -(count - 1);
				}
				else { 
					track++;
				}
		
				//Tear down the stream and reinitialize everything	
				pause_if_playing();
				mv.ap = NULL;
				if ( !SDL_LoadWAV(track->filename, &(mv.as), &(mv.ap), &(mv.al)) ) { 
					jerr(0, "Failed to load audio file: %s\n", track->filename);
				}
				SDL_PauseAudio(0);
				continue;
			}

			//Process keypresses
			if (read(0, &key, 1) > -1) {
				log( "Got code '%d'\n", key);
			}

			//Loop a playing track
			if (settings.loop && (mv.al < 10)) {
				mv.ap -= (gl - re);
			  mv.al = (gl - re); 
			}

			//Handle some degree of rewinds...
			if (key == 'l') {
				settings.loop = settings.loop ? 0 : 1;	
				log( "'L' pressed: looping activated!\n" );
			}
			if (key == 'p') {
				SDL_PauseAudio((mv.paused += (!mv.paused) ? 1 : -1));	
				log( "'P' pressed: audio paused.\n" );
			}
			if (key == 'a') {
				mv.ap -= mv.sp/2, mv.al += mv.sp/2;
				log( "'A' pressed: moved backwards through audio.\n" );
			}
			if (key == 's') {
				mv.ap += mv.sp/2, mv.al -= mv.sp/2;
				log( "'S' pressed: moved forwards through audio.\n" );
			}
			if (key == 'w') {
				mv.vol -= (!mv.vol) ? 0 : 10;
				log( "'W' pressed: volume lowered.\n" );
			}
			if (key == 'e') {
				mv.vol += (mv.vol == SDL_MIX_MAXVOLUME) ? 0 : 10;
				log( "'E' pressed: volume increased.\n" );
			}
			if (key == 'r') {
				log( "'R' pressed: current song rewinded to beginning.\n" );
				pause_if_playing();
				if (mv.al > 10) {
					mv.ap -= (gl - mv.al) + re, mv.al = (gl - re);
				}
				SDL_PauseAudio(0);
			}
			if ( dsmpDirname ) {
				if (key == 'j' || key == 'k') {
					if (key == 'j') {
						track -= (!track->hash) ? 0 : 1;
						log( "'J' pressed: next song selected in list.\n" );
					}
					else {
						track += ((track + 1)->sentinel) ? -(count-1) : 1;
						log( "'K' pressed: previous song selected in list.\n" );
					}
					log( "Now playing:      %s\n", track->filename );
					log( "hash %d, sentinel %d\n", track->hash, track->sentinel );
					pause_if_playing();
					mv.ap = NULL;
					if ( !SDL_LoadWAV(track->filename, &(mv.as), &(mv.ap), &(mv.al)) ) {
						jerr(0, "Failed to load audio file: %s\n", track->filename);
					}
					SDL_PauseAudio(0);
				}
			}
		}
		
		//Clean up
		//remove(pid_buf);
		set_norm();
		SDL_CloseAudio();
		SDL_Quit();
		_exit(0);
 #ifndef NO_FORK
	}
	else if (pid) {
		FILE *f = fopen(pid_buf, "w");
		fprintf(f, "%d\n", pid);
		fclose(f);
	}
 #endif

	return 0;
}
