CC?=	clang
CFLAGS=	-O2 -fomit-frame-pointer -pipe
.if ${CC} == "clang"
CFLAGS+=	-Weverything
.else
CFLAGS+=	-Wall -Wconversion -Wshadow -Wextra -Wpedantic
.endif
DFLAGS=	-g -ggdb
EXE=	bsdfetch
DBG=	${EXE}.debug

SRC=	bsdfetch.c
OS != uname -s
.if ${OS} == "OpenBSD"
SRC+=	sysctlbyname.c
.endif

${EXE}: ${SRC}
	${CC} ${CFLAGS} -s -o $@ $>

${DBG}: ${SRC}
	${CC} ${DFLAGS} -o $@ $>

all: ${EXE} ${DBG}

.PHONY: clean
clean:
	rm -f ${EXE} ${DBG}

.PHONY: install
install:
	install -d /usr/local/bin
	install -m 755 ${EXE} /usr/local/bin
	install -d /usr/local/share/doc/logo
	install -m 644 logo/* /usr/local/share/doc/logo/

.PHONY: uninstall
uninstall:
	rm -f /usr/local/bin/${EXE}
	rm -rf /usr/local/share/doc/logo/
