# OpenFetch

`openfetch` is a simple tool to display information about a running [**FreeBSD**](https://www.freebsd.org/), [**OpenBSD**](https://www.openbsd.org/), [**NetBSD**](https://www.netbsd.org/), or [**DragonFly**](https://www.dragonflybsd.org/) **BSD** system written in C11. Any contribution is highly appreciated.

> `openfetch` is a fork of `bsdfetch` version 1.1.1

## Usage

1. Clone or download this repository:
	```sh
	git clone https://github.com/daviduhden/openfetch.git
	```
	Or download the ZIP file and unpack it.

2. Navigate to the directory:
	```sh
	cd openfetch
	```

3. Build the executable:
	```sh
	make
	```

4. Run `openfetch`:
	```sh
	./openfetch
	```

## Installation

To install `openfetch` and the logo files:

```sh
sudo make install
```

This will install `openfetch` to `/usr/local/bin` and the logo files to `/usr/local/share/doc/logo/`.

## Cleaning Up

To clean up the build files:

```sh
make clean
```
