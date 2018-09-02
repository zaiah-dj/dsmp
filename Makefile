# dsmp - Simple media player
NAME = dsmp
SRC = vendor/single.c main.c
OBJ = ${SRC:.c=.o}
PREFIX = /usr/local
PKGDIR = $(NAME)-$(VERSION)
VERSION = 0.3
MANPREFIX = ${PREFIX}/share/man
LDDIRS = -L$(PREFIX)/lib
LDFLAGS = -lSDL -lm
DFLAGS = -DSQROOGE_H -DENABLE_VERSION_BANNER -DVERSION="$(VERSION)" -DPRINT_ANIMATION_INFO -DENABLE_LOG -DNO_FORK #-DPLAY_EMBEDDED -DNO_KEY
CC = gcc
CFLAGS = -g -Wall -Werror -Wno-maybe-uninitialized -Wno-unused -ansi -std=c99 -Wno-deprecated-declarations -O2 -pedantic-errors $(LDDIRS) $(LDFLAGS) $(DFLAGS) -Wno-strict-aliasing -Wno-format-truncation
FLAGS = -p dte.wav -l -f 60 
WILDCARD = *
IGNORE = archive/$(WILDCARD) vendor/$(WILDCARD)

.PHONY: all options clean install uninstall

$(NAME): ${OBJ}
	@echo CC -o $@ single.o main.o ${CFLAGS}
	@${CC} -o $@ single.o main.o ${CFLAGS}

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

clean:
	@echo Cleaning
	@rm -f $(NAME) *.o *.so *.dll

install:
	@cp $(NAME) $(PREFIX)/bin/ 

#if 0
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

veryclean:
	@echo Cleaning up version control files...
	-find -type f -name "*.swo" | xargs rm
	-find -type f -name "*.swp" | xargs rm
	-find -type f -name "*.swn" | xargs rm
	-find -type f -name "*.swl" | xargs rm

pkg: clean
pkg: veryclean 
pkg:
	@test -d $(PKGDIR) && rm -rf $(PKGDIR) || echo 'no problem'
	@mkdir $(PKGDIR)/
	@cp *.c README.md CHANGELOG style.css $(PKGDIR)/
	@cp -r vendor $(PKGDIR)/
	@cp -r img $(PKGDIR)/
	echo sed "{ s/#[^if,^endif].$(WILDCARD)//; }" Makefile cpp 2>/dev/stderr sed "{ /^#/d; s/^ /\t/ }" > $(PKGDIR)/Makefile
	@sed "{ s/#[^if,^endif].$(WILDCARD)//; }" Makefile | \
		cpp 2>/dev/stderr | \
		sed "{ /^#/d; s/^ /\t/; }" > $(PKGDIR)/Makefile
	@echo tar chzf $(PKGDIR).tar.gz $(PKGDIR)
	@tar chzf $(PKGDIR).tar.gz $(PKGDIR)

doc:
	git checkout gh-pages
	git show `git show-ref master --heads --hash`:README.md > index.html
	git add index.*
	git commit -m "Updated documentation `date +%F`"
	git push
	git checkout master	

html:
	markdown -o index.html README.md

#endif

