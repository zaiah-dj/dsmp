# dsmp - Simple media player
NAME = dsmp
SRC = vendor/single.c main.c
OBJ = ${SRC:.c=.o}
PREFIX = /usr/local
VERSION = 0.2
MANPREFIX = ${PREFIX}/share/man
LDDIRS = -L$(PREFIX)/lib
LDFLAGS = -lSDL -lm
DFLAGS = -DSQROOGE_H -DENABLE_VERSION_BANNER -DVERSION="$(VERSION)" -DPRINT_ANIMATION_INFO -DENABLE_LOG -DNO_FORK #-DPLAY_EMBEDDED -DNO_KEY
CC = gcc
CFLAGS = -g -Wall -Werror -Wno-maybe-uninitialized -Wno-unused -ansi -std=c99 -Wno-deprecated-declarations -O2 -pedantic-errors $(LDDIRS) $(LDFLAGS) $(DFLAGS) -Wno-strict-aliasing -Wno-format-truncation
FLAGS = -p dte.wav -l -f 60 
IGNORE = $(ALIAS) archive/* bin/* tools/* vendor/* wk/*
ARCHIVEDIR = ..
ARCHIVEFMT = gz
ARCHIVEFILE = $(NAME).`date +%F`.`date +%H.%M.%S`.tar.${ARCHIVEFMT}

$(NAME): ${OBJ}
	@echo CC -o $@ single.o main.o ${CFLAGS}
	@${CC} -o $@ single.o main.o ${CFLAGS}

debug:
	@gdb 2>/dev/null || echo "Error: Dependency 'GDB' not present!"; gdb
	@echo gdb -ex run --args ./$(NAME) $(FLAGS)
	@gdb -ex run --args ./$(NAME) $(FLAGS)

leak:
	$(LEAKCHK) $(LEAKFLAGS) ./$(NAME) $(FLAGS)
	@valgrind 2>/dev/null || echo "Error: Dependency 'Valgrind' not present!"; valgrind
	@test -f PeaktimeFunk.wav || echo "File 'PeaktimeFunk.wav' not found."; test -f PeaktimeFunk.wav
	@echo valgrind --leak-check=full ./$(NAME) -f PeaktimeFunk.wav 
	@valgrind --leak-check=full ./$(NAME) -f PeaktimeFunk.wav

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

clean:
	@echo Cleaning
	@rm -f $(NAME) *.o *.so *.dll

veryclean:
	@echo Cleaning up version control files...
	-find -type f -name "*.swo" | xargs rm
	-find -type f -name "*.swp" | xargs rm
	-find -type f -name "*.swn" | xargs rm
	-find -type f -name "*.swl" | xargs rm

install:
	@cp dsmp $(PREFIX)/bin/ 

doc:
	git checkout gh-pages
	git show `git show-ref master --heads --hash`:README.md > index.html
	git add index.*
	git commit -m "Updated documentation `date +%F`"
	git push
	git checkout master	

html:
	markdown -o index.html README.md

.PHONY: all options clean dist install uninstall permissions archive
