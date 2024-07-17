/*
 * Copyright (c) 2022 - 2023 jhx <jhx0x00@gmail.com>
 * Copyright (c) 2024 David Uhden Collado <david@uhden.dev>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/utsname.h>
#ifdef __OpenBSD__
#include <sys/time.h>
#include <sys/sensors.h>
#include "sysctlbyname.h"
#endif

#include <err.h>
#include <pwd.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define VERSION "1.1.1"
#define RED "\033[1;31m"   /* Bright red */
#define GREEN "\033[1;32m" /* Bright green */
#define BLUE "\033[1;34m"  /* Bright blue */
#define MAGENTA "\033[1;35m" /* Bright magenta */
#define YELLOW "\033[1;33m" /* Bright yellow */
#define WHITE "\033[1;37m" /* Bright white */
#define CYAN "\033[1;36m" /* Bright cyan */
#define CEND "\033[0m"    /* Reset color */

static char buf[BUFSIZ]; /* buffer large enough for everything */
static int color_flag = 1;   /* 1 => color; 0 => no color in labels */
const char *dynamic_colors[10];

// Logos with colors
typedef struct {
	const char *name;
	const char *lines[30];
	const char *colors[10];
} Logo;

Logo logos[] = {
	{
		"FreeBSD",
		{
			"   ${c2} ",
			"${c1}`",
			"  ${c2}` `.....---...${c1}....--.",
			"   -/",
			"  ${c2}+o   .--         ${c1}/y:      +.",
			"  ${c2} yo:.            ${c1}:o      +-",
			"    ${c2}y/               ${c1}-/   -o/",
			"   ${c2}.-                  ${c1}::/sy+:.",
			"   ${c2}/                     ${c1}--  /",
			"  ${c2}:                          ${c1}:",
			"  ${c2}:                          ${c1}:",
			"   ${c2}/                          ${c1}/",
			"   ${c2}.-                        ${c1}-.",
			"    ${c2}--                      ${c1}-.",
			"     ${c2}:                  ${c1}:",
			"       .--             --.",
			"          .---.....----.",
			NULL
		},
		{WHITE, RED, GREEN, "", ""}
	},
	{
		"OpenBSD",
		{
			"${c3}                                     _",
			"                                    (_)",
			"${c1}              |    .",
			"${c1}          .   |L  /|   .         ${c3} _",
			"${c1}      _ . |\\ _| \\--+._/| .       ${c3}(_)",
			"${c1}     / ||\\| Y J  )   / |/| ./",
			"    J  |)'( |         F.'/       ${c3} _",
			"${c1}  -<|  F         __     .-<        ${c3}(_)",
			"${c1}    | /       .-'${c3}. ${c1}.  /${c3}-. ${c1}L___",
			"    J \\      <    ${c3}\\ ${c1} | | ${c5}O${c3}\\${c1}|.-' ${c3} _",
			"${c1}  _J \\  .-    \\${c3}/ ${c5}O ${c3}| ${c1}| \\  |${c1}F    ${c3}(_)",
			"${c1} '-F  -<_.     \\   .-'  -' L__",
			"__J  _   _.     >-'  ${c1})${c4}._.   ${c1}|-'",
			"${c1} -|.'   /_.          ${c4}\\_|  ${c1} F",
			"  /.-   .                _.<",
			" /'    /.'             .'  \\",
			"  /L  /'   |/      _.-'-\\",
			" /'J       ___.---'\\|",
			"   |\\  .--' V  | . ",
			"   |/. -.     ._)",
			"      / .-.\\",
			"      \\ (  \\",
			"       .\\",
			NULL
		},
		{YELLOW, WHITE, CYAN, RED, ""}
	},
	{
		"NetBSD",
		{
			"${c1}                     -/oshdmNMNdhyo+:-",
			"${c2}y${c1}/s+:-`    .-:+oydNMMMMNhs/-`",
			"${c2}-m+${c1}NMMMMMMMMMMMMMMMMMMMNdhmNMMMmdhs+/-",
			" ${c2}-m+${c1}NMMMMMMMMMMMMMMMMMMMMmy+:",
			"  ${c2}-N/${c1}dMMMMMMMMMMMMMMMds:",
			"   ${c2}-N/${c1}hMMMMMMMMMmho:",
			"    ${c2}-N/${c1}-:/++/:.",
			"${c2}     :M+",
			"      :Mo",
			"       :Ms",
			"        :Ms",
			"         :Ms",
			"          :Ms",
			"           :Ms",
			"            :Ms",
			"             :Ms",
			"              :Ms",
			NULL
		},
		{MAGENTA, WHITE, "", "", ""}
	},
	{
		"DragonFly",
		{
			"${c2},--,           ${c1}|           ${c2},--,",
			"${c2}|   -,       ${c1},^,       ${c2},-'   |",
			"${c2} ,    -,   ${c3}(/ \\)   ${c2},-'    ,'",
			"${c2}   -,    -,${c1}/   \\${c2},-'    ,-'",
			"${c2}      ------${c1}(   )${c2}------'",
			"${c2}  ,----------${c1}(   )${c2}----------,",
			"${c2} |        _,-${c1}(   )${c2}-,_        |",
			"${c2}  -,__,-'   ${c1}\\   /${c2}   -,__,-'",
			"${c1}              | |",
			"              | |",
			"              | |",
			"              | |",
			"              | |",
			"              | |",
			"              |'",
			NULL
		},
		{RED, WHITE, "", "", ""}
	},
};

void set_colors(int num_colors, const char *colors[]) {
	for (int i = 0; i < num_colors && i < 10; i++) {
		dynamic_colors[i] = colors[i];
	}
}

void detect_and_set_colors(void) {
	#if defined(__FreeBSD__)
		const char *freebsd_colors[] = {WHITE, RED, GREEN};
		set_colors(3, freebsd_colors);
	#elif defined(__OpenBSD__)
		const char *openbsd_colors[] = {YELLOW, WHITE, CYAN, RED};
		set_colors(4, openbsd_colors);
	#elif defined(__NetBSD__)
		const char *netbsd_colors[] = {MAGENTA, WHITE};
		set_colors(2, netbsd_colors);
	#elif defined(__DragonFly__)
		const char *dragonfly_colors[] = {RED, WHITE};
		set_colors(2, dragonfly_colors);
	#endif
}

void replace_placeholder_with_color(char *line) {
	char buffer[256];
	char *pos;
	size_t len;

	strlcpy(buffer, line, sizeof(buffer));

	for (int k = 0; k < 10; k++) {
		if (dynamic_colors[k] == NULL || dynamic_colors[k][0] == '\0') continue;
		char placeholder[5];
		snprintf(placeholder, sizeof(placeholder), "${c%d}", k + 1);

		while ((pos = strstr(buffer, placeholder)) != NULL) {
			len = (size_t)(pos - buffer);
			buffer[len] = '\0';
			strlcat(buffer, dynamic_colors[k], sizeof(buffer));
			strlcat(buffer, pos + strlen(placeholder), sizeof(buffer));
		}
	}

	strlcpy(line, buffer, sizeof(buffer));
}

void print_logo(const char *system) {
	for (size_t i = 0; i < sizeof(logos) / sizeof(logos[0]); i++) {
		if (strcmp(system, logos[i].name) == 0) {
			for (int j = 0; logos[i].lines[j] != NULL; j++) {
				if (color_flag) {
					char line[256];
					strlcpy(line, logos[i].lines[j], sizeof(line));
					replace_placeholder_with_color(line);
					printf("%s%s\n", line, CEND);
				} else {
					printf("%s\n", logos[i].lines[j]);
				}
			}
			return;
		}
	}
	fprintf(stderr, "Unsupported BSD variant: %s\n", system);
}

void detect_and_print_logo(void) {
#if defined(__FreeBSD__)
	print_logo("FreeBSD");
#elif defined(__OpenBSD__)
	print_logo("OpenBSD");
#elif defined(__NetBSD__)
	print_logo("NetBSD");
#elif defined(__DragonFly__)
	print_logo("DragonFly");
#else
#error Unsupported BSD variant
#endif
}

static void cpr(char *fld, char *clr, char *fmt, ...) {
	va_list ap;

	if (color_flag)
		printf("%s%s%s: ", clr, fld, CEND);
	else
		printf("%s: ", fld);
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
	printf("\n");
}

/**
 * Squeeze multiple adjacent blank chars. into a single space.
 */
static void sqz(char *s) {
	for (char *p = s; *p; p++) {
		size_t n;
		if ((n = strspn(p, " \t")) > 0) {
			*p = ' ';
			memmove(p + 1, p + n, strlen(p + n) + 1);
		}
	}
}

static void get_shell(void) {
	struct passwd *pw;
	char *sh, *p;

	if ((sh = getenv("SHELL")) == NULL || *sh == '\0') {
		if ((pw = getpwuid(getuid())) == NULL)
			err(1, "getpwuid() failed");
		sh = pw->pw_shell;
	}
	if ((p = strrchr(sh, '/')) != NULL && *(p + 1) != '\0')
		sh = ++p;
	cpr("Shell", RED, sh);
}

static void get_user(void) {
	struct passwd *pw;
	char *p;

	if ((p = getenv("USER")) == NULL || *p == '\0') {
		if ((pw = getpwuid(getuid())) == NULL)
			err(1, "getpwuid() failed");
		p = pw->pw_name;
	}
	cpr("User", RED, p);
}

static void get_cpu(void) {
	long ncpu, nmax;
	size_t sz;

	if ((ncpu = sysconf(_SC_NPROCESSORS_ONLN)) == -1)
		err(1, "sysconf(_SC_NPROCESSORS_ONLN) failed");
	if ((nmax = sysconf(_SC_NPROCESSORS_CONF)) == -1)
		err(1, "sysconf(_SC_NPROCESSORS_CONF) failed");

	sz = sizeof buf;
	if (sysctlbyname("machdep.cpu_brand", buf, &sz, NULL, 0) == -1)
		if (sysctlbyname("hw.model", buf, &sz, NULL, 0) == -1)
			err(1, "error getting CPU info.");

	buf[sz] = '\0';
	sqz(buf); /* NetBSD needs this */
	cpr("CPU", RED, buf);
	cpr("Cores", RED, "%ld of %ld processors online", ncpu, nmax);

#if defined(__FreeBSD__) || defined(__DragonFly__)
#define CELSIUS 273.15
	for (int i = 0; i < (int)ncpu; i++) {
		int temp = 0;

		sz = sizeof temp;
		snprintf(buf, sizeof buf, "dev.cpu.%d.temperature", i);
		if (sysctlbyname(buf, &temp, &sz, NULL, 0) == -1)
			return;
		if (color_flag)
			printf(GREEN " -> " CEND);
		else
			printf(" -> ");
		snprintf(buf, sizeof buf, "Core [%d]", i + 1);
		cpr(buf, RED, "%.1f °C", (temp * 0.1) - CELSIUS);
	}

#elif defined(__OpenBSD__)
	struct sensor sensors;
	int mib[5];

	mib[0] = CTL_HW;
	mib[1] = HW_SENSORS;
	mib[2] = 0;
	mib[3] = SENSOR_TEMP;
	mib[4] = 0;

	sz = sizeof sensors;
	if (sysctl(mib, 5, &sensors, &sz, NULL, 0) == -1)
		return;
	cpr("CPU Temp", RED, "%d °C", (int)((float)(sensors.value - 273150000) / 1E6));

#elif defined(__NetBSD__)
	const char *const cmd = "/usr/sbin/envstat |"
							" awk '/ cpu[0-9]+ temperature: / { print $3 }'";
	FILE *f = popen(cmd, "r");
	if (f == NULL)
		err(1, "popen(%s) failed", cmd);
	int i = 0;

	while (fgets(buf, sizeof buf, f) != NULL) {
		float temp;

		if (sscanf(buf, "%f", &temp) != 1)
			break;
		if (color_flag)
			printf(GREEN " -> " CEND);
		else
			printf(" -> ");
		snprintf(buf, sizeof buf, "Core [%d]", ++i);
		cpr(buf, RED, "%.1f °C", temp);
	}

	if (pclose(f) != 0)
		err(1, "pclose(%s) failed", cmd);
#endif
}

static void get_loadavg(void) {
	double lavg[3] = {0.0};

	if (getloadavg(lavg, 3) != 3)
		err(1, "getloadavg() failed");
	cpr("Loadavg", RED, "%.2lf %.2lf %.2lf", lavg[0], lavg[1], lavg[2]);
}

static void get_packages(void) {

#if defined(__OpenBSD__) || defined(__NetBSD__)
	const char *const cmd = "/usr/sbin/pkg_info";

#elif defined(__FreeBSD__) || defined(__DragonFly__)
	const char *const cmd = "/usr/sbin/pkg info";

#else
#error Unsupported BSD variant
#endif

	FILE *f = popen(cmd, "r");
	if (f == NULL)
		err(1, "popen(%s) failed", cmd);

	/* No. of packages == simple line count */
	size_t npkg = 0;
	while (fgets(buf, sizeof buf, f) != NULL)
		if (strchr(buf, '\n') != NULL)
			npkg++;

	if (pclose(f) != 0)
		err(1, "pclose(%s) failed", cmd);

	cpr("Packages", RED, "%zu", npkg);
}

static void get_uptime(void) {
	long up, days, hours, mins;
	struct timeval t;
	size_t tsz = sizeof t;

	if (sysctlbyname("kern.boottime", &t, &tsz, NULL, 0) == -1)
		err(1, "failed to get kern.boottime");

	up = (long)(time(NULL) - t.tv_sec + 30);
	days = up / 86400;
	up %= 86400;
	hours = up / 3600;
	up %= 3600;
	mins = up / 60;

	cpr("Uptime", RED, "%ldd %ldh %ldm", days, hours, mins);
}

static void get_memory(void) {
	unsigned long long ramsz;
	long pagesz, npages;

	if ((pagesz = sysconf(_SC_PAGESIZE)) == -1)
		err(1, "error getting system page-size");
	if ((npages = sysconf(_SC_PHYS_PAGES)) == -1)
		err(1, "error getting no. of system pages");

	ramsz = (unsigned long long)(pagesz * npages) / (1024 * 1024);
	cpr("RAM", RED, "%llu MB", ramsz);
}

static void get_hostname(void) {
	if (gethostname(buf, sizeof buf) == -1)
		err(1, "gethostname() failed");
	cpr("Host", RED, "%s", buf);
}

static void get_sysinfo(void) {
	struct utsname un;
	char *p;

	if (uname(&un))
		err(1, "uname() failed");
	cpr("OS", RED, un.sysname);
	cpr("Release", RED, un.release);
	if ((p = strchr(un.version, ':')) != NULL)
		*p = '\0'; /* NetBSD: lop-off build-strings */
	cpr("Version", RED, un.version);
	cpr("Arch", RED, "%s", un.machine);
}

_Noreturn static void version(void) {
	printf("%s - version %s (%s)\n",
		   getprogname(),
		   VERSION,
		   __DATE__);
	exit(EXIT_SUCCESS);
}

_Noreturn static void usage(void) {
	printf("USAGE: %s [-h|-n|-v]\n"
		   "   -h  Show this help text\n"
		   "   -n  Turn off colors\n"
		   "   -v  Show version\n",
		   getprogname());
	exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
	color_flag = isatty(1);
	if (argc == 2) {
		if (strcmp(argv[1], "-h") == 0)
			usage();
		else if (strcmp(argv[1], "-n") == 0)
			color_flag = 0;
		else if (strcmp(argv[1], "-v") == 0)
			version();
	}
	detect_and_print_logo();
	get_sysinfo();
	get_hostname();
	get_shell();
	get_user();
	get_packages();
	get_uptime();
	get_memory();
	get_loadavg();
	get_cpu();

	return EXIT_SUCCESS;
}
