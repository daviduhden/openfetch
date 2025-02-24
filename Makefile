# Set the default compiler to clang if not specified
CC?=	clang

# Common compilation flags
CFLAGS=	-O2 -fomit-frame-pointer -pipe -std=c11

# Additional flags for clang
.if ${CC} == "clang"
CFLAGS+=	-Weverything
.else
# Additional flags for other compilers
CFLAGS+=	-Wall -Wconversion -Wshadow -Wextra -Wpedantic
.endif

# Debugging flags
DFLAGS=	-g -ggdb

# Executable and debug target names
EXE=	openfetch
DBG=	${EXE}.debug

# Source files
SRC=	openfetch.c
# Include additional source file for OpenBSD
OS != uname -s
.if ${OS} == "OpenBSD"
SRC+=	sysctlbyname.c
.endif

# Manual page
MAN=	openfetch.1

# Rule to build the executable
${EXE}: ${SRC}
	@echo "Building ${EXE}..."
	@${CC} ${CFLAGS} -s -o $@ $>

# Rule to build the debug version
${DBG}: ${SRC}
	@echo "Building debug version ${DBG}..."
	@${CC} ${DFLAGS} -o $@ $>

# Default target to build both release and debug versions
.PHONY: all
all: ${EXE} ${DBG}

# Phony target to clean up build artifacts
.PHONY: clean
clean:
	@echo "Cleaning up..."
	@rm -f ${EXE} ${DBG}

# Phony target to install the executable and other files
.PHONY: install
install: ${EXE}
	@echo "Installing ${EXE} to /usr/local/bin..."
	@install -d /usr/local/bin
	@install -m 755 ${EXE} /usr/local/bin
	@echo "Installing logo files to /usr/local/share/doc/logo/..."
	@install -d /usr/local/share/doc/logo
	@install -m 644 logo/* /usr/local/share/doc/logo/
	@echo "Installing man page to /usr/local/man/man1..."
	@install -d /usr/local/man/man1
	@install -m 644 ${MAN} /usr/local/man/man1

# Phony target to uninstall the executable and other files
.PHONY: uninstall
uninstall:
	@echo "Uninstalling ${EXE} from /usr/local/bin..."
	@rm -f /usr/local/bin/${EXE}
	@echo "Removing /usr/local/share/doc/logo/..."
	@rm -rf /usr/local/share/doc/logo/
	@echo "Removing man page from /usr/local/man/man1..."
	@rm -f /usr/local/man/man1/${MAN}

# Phony target to display help
.PHONY: help
help:
	@echo "Available targets:"
	@echo "  all        - Build both release and debug versions"
	@echo "  clean      - Clean up build artifacts"
	@echo "  install    - Install the executable and other files"
	@echo "  uninstall  - Uninstall the executable and other files"
	@echo "  help       - Display this help message"