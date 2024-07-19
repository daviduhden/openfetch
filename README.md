# bsdfetch

**bsdfetch** is a simple tool to display information about a running [**FreeBSD**](https://www.freebsd.org/), [**OpenBSD**](https://www.openbsd.org/), [**NetBSD**](https://www.netbsd.org/), or [**DragonFly**](https://www.dragonflybsd.org/) **BSD** system.

## Usage

1. **Clone or download this repository:**
	```sh
	git clone https://github.com/daviduhden/bsdfetch.git
	```
	Or download the ZIP file and unpack it.

2. **Navigate to the directory:**
	```sh
	cd bsdfetch
	```

3. **Build the executable:**
	```sh
	make
	```

4. **Run bsdfetch:**
	```sh
	./bsdfetch
	```

## Installation

To install bsdfetch and the logo files:

1. **Install the executable and logo files:**
	```sh
	sudo make install
	```

This will install `bsdfetch` to `/usr/local/bin` and the logo files to `/usr/local/share/doc/logo/`.

## Cleaning Up

To clean up the build files:

	```sh
	make clean
	```