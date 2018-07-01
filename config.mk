# Directories
PREFIX = /usr/local
VERSION = 0.1

# You probably don't need to touch this
BACKUP_DIR=".."
MANPREFIX = ${PREFIX}/share/man
LDDIRS = -L$(PREFIX)/lib
LDFLAGS = -lSDL -lm
DFLAGS = -DENABLE_VERSION_BANNER -DVERSION="$(VERSION)" -DPRINT_ANIMATION_INFO #-DPLAY_EMBEDDED
SHARED_LDFLAGS =
CC = cc
# User flags?
UFLAGS = 
CFLAGS = -g -Wall -Werror -Wno-maybe-uninitialized -Wno-unused -ansi -std=c99 -Wno-deprecated-declarations -O2 -pedantic-errors $(LDDIRS) $(LDFLAGS) $(DFLAGS) $(UFLAGS) -Wno-strict-aliasing
# Cygwin 
# CFLAGS = -Wall -Werror -Wno-unused -std=c99 -Wno-deprecated-declarations -O2 -pedantic ${LDFLAGS}
