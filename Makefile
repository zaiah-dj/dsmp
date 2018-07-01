# dsmp - Simple media player
NAME = dsmp
SRC = vendor/single.c main.c
OBJ = ${SRC:.c=.o}
PREFIX = /usr/local
VERSION = 0.2
MANPREFIX = ${PREFIX}/share/man
LDDIRS = -L$(PREFIX)/lib
LDFLAGS = -lSDL -lm
DFLAGS = -DENABLE_VERSION_BANNER -DVERSION="$(VERSION)" -DPRINT_ANIMATION_INFO #-DPLAY_EMBEDDED
CC = gcc
CFLAGS = -g -Wall -Werror -Wno-maybe-uninitialized -Wno-unused -ansi -std=c99 -Wno-deprecated-declarations -O2 -pedantic-errors $(LDDIRS) $(LDFLAGS) $(DFLAGS) -Wno-strict-aliasing -Wno-format-truncation
FLAGS = -p dte.wav -l -f 60 

IGNORE = $(ALIAS) archive/* bin/* tools/* vendor/* wk/*
ARCHIVEDIR = ..
ARCHIVEFMT = gz
ARCHIVEFILE = $(NAME).`date +%F`.`date +%H.%M.%S`.tar.${ARCHIVEFMT}

$(NAME): ${OBJ}
	@echo CC -o $@ ${OBJ} ${CFLAGS}
	@${CC} -o $@ ${OBJ} ${CFLAGS}

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

permissions:
	@find | grep -v './tools' | grep -v './examples' | grep -v './.git' | sed '1d' | xargs stat -c 'chmod %a %n' > PERMISSIONS

restore-permissions:
	chmod 744 PERMISSIONS
	./PERMISSIONS
	chmod 644 PERMISSIONS

backup:
	@echo tar chzf $(ARCHIVEDIR)/${ARCHIVEFILE} --exclude-backups \
		`echo $(IGNORE) | sed '{ s/^/--exclude=/; s/ / --exclude=/g; }'` ./*
	@tar chzf $(ARCHIVEDIR)/${ARCHIVEFILE} --exclude-backups \
		`echo $(IGNORE) | sed '{ s/^/--exclude=/; s/ / --exclude=/g; }'` ./*

archive: ARCHIVEDIR = archive
archive: backup

commit:
	git add .
	git commit -m "Changes made."
#	git push 

install:
	@cp dsmp $(PREFIX)/bin/ 

doc:
	git checkout gh-pages
	git show `git show-ref master --heads --hash`:README.md > index.html
	git add index.*
	git commit -m "Updated documentation `date +%F`"
	git push
	git checkout master	

.PHONY: all options clean dist install uninstall permissions archive
