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

#define VERSION "1.0"

#define LOGO_PATH1 "./logo/"
#define LOGO_PATH2 "/usr/local/share/doc/logo/"
#define MAX_LOGO_LINES 30
#define MAX_LINE_LENGTH 256

typedef struct {
	const char *name;
	char lines[MAX_LOGO_LINES][MAX_LINE_LENGTH];
} Logo;

static char buf[BUFSIZ]; /* buffer large enough for everything */

void read_logo(Logo *logo, const char *filename) {
	FILE *file;
	char filepath1[MAX_LINE_LENGTH], filepath2[MAX_LINE_LENGTH];
	snprintf(filepath1, sizeof(filepath1), "%s%s", LOGO_PATH1, filename);
	snprintf(filepath2, sizeof(filepath2), "%s%s", LOGO_PATH2, filename);

	if ((file = fopen(filepath1, "r")) == NULL) {
		if ((file = fopen(filepath2, "r")) == NULL) {
			fprintf(stderr, "Error: Unable to open logo file from either path.\n");
			exit(1);
		}
	}

	int i = 0;
	while (fgets(logo->lines[i], MAX_LINE_LENGTH, file) && i < MAX_LOGO_LINES) {
		logo->lines[i][strcspn(logo->lines[i], "\n")] = 0;  // Remove newline character
		i++;
	}
	fclose(file);
}

void append_info(char info[MAX_LOGO_LINES][MAX_LINE_LENGTH], int *info_lines, const char *fmt, ...) {
	if (*info_lines >= MAX_LOGO_LINES) {
		return;
	}
	va_list args;
	va_start(args, fmt);
	vsnprintf(info[*info_lines], MAX_LINE_LENGTH, fmt, args);
	va_end(args);
	(*info_lines)++;
}

void get_sysinfo(char info[MAX_LOGO_LINES][MAX_LINE_LENGTH], int *info_lines) {
	struct utsname un;
	char *p;

	if (uname(&un)) {
		err(1, "uname() failed");
	}
	append_info(info, info_lines, "OS: %s", un.sysname);
	append_info(info, info_lines, "Release: %s", un.release);
	if ((p = strchr(un.version, ':')) != NULL) {
		*p = '\0'; /* NetBSD: lop-off build-strings */
	}
	append_info(info, info_lines, "Version: %s", un.version);
	append_info(info, info_lines, "Arch: %s", un.machine);
}

void get_hostname(char info[MAX_LOGO_LINES][MAX_LINE_LENGTH], int *info_lines) {
	if (gethostname(buf, sizeof buf) == -1) {
		err(1, "gethostname() failed");
	}
	append_info(info, info_lines, "Host: %s", buf);
}

void get_shell(char info[MAX_LOGO_LINES][MAX_LINE_LENGTH], int *info_lines) {
	struct passwd *pw;
	char *sh, *p;

	if ((sh = getenv("SHELL")) == NULL || *sh == '\0') {
		if ((pw = getpwuid(getuid())) == NULL) {
			err(1, "getpwuid() failed");
		}
		sh = pw->pw_shell;
	}
	if ((p = strrchr(sh, '/')) != NULL && *(p + 1) != '\0') {
		sh = ++p;
	}
	append_info(info, info_lines, "Shell: %s", sh);
}

void get_user(char info[MAX_LOGO_LINES][MAX_LINE_LENGTH], int *info_lines) {
	struct passwd *pw;
	char *p;

	if ((p = getenv("USER")) == NULL || *p == '\0') {
		if ((pw = getpwuid(getuid())) == NULL) {
			err(1, "getpwuid() failed");
		}
		p = pw->pw_name;
	}
	append_info(info, info_lines, "User: %s", p);
}

void get_packages(char info[MAX_LOGO_LINES][MAX_LINE_LENGTH], int *info_lines) {
	const char *cmd;

#if defined(__OpenBSD__) || defined(__NetBSD__)
	cmd = "/usr/sbin/pkg_info";
#elif defined(__FreeBSD__) || defined(__DragonFly__)
	cmd = "/usr/sbin/pkg info";
#else
	#error Unsupported BSD variant
#endif

	FILE *f = popen(cmd, "r");
	if (f == NULL) {
		err(1, "popen(%s) failed", cmd);
	}

	/* No. of packages == simple line count */
	size_t npkg = 0;
	while (fgets(buf, sizeof buf, f) != NULL) {
		if (strchr(buf, '\n') != NULL) {
			npkg++;
		}
	}

	if (pclose(f) != 0) {
		err(1, "pclose(%s) failed", cmd);
	}

	append_info(info, info_lines, "Packages: %zu", npkg);
}

void get_uptime(char info[MAX_LOGO_LINES][MAX_LINE_LENGTH], int *info_lines) {
	long up, days, hours, mins;
	struct timeval t;
	size_t tsz = sizeof t;

	if (sysctlbyname("kern.boottime", &t, &tsz, NULL, 0) == -1) {
		err(1, "failed to get kern.boottime");
	}

	up = (long)(time(NULL) - t.tv_sec + 30);
	days = up / 86400;
	up %= 86400;
	hours = up / 3600;
	up %= 3600;
	mins = up / 60;

	append_info(info, info_lines, "Uptime: %ldd %ldh %ldm", days, hours, mins);
}

void get_memory(char info[MAX_LOGO_LINES][MAX_LINE_LENGTH], int *info_lines) {
	unsigned long long ramsz;
	long pagesz, npages;

	if ((pagesz = sysconf(_SC_PAGESIZE)) == -1) {
		err(1, "error getting system page-size");
	}
	if ((npages = sysconf(_SC_PHYS_PAGES)) == -1) {
		err(1, "error getting no. of system pages");
	}

	ramsz = (unsigned long long)(pagesz * npages) / (1024 * 1024);
	append_info(info, info_lines, "RAM: %llu MB", ramsz);
}

void get_loadavg(char info[MAX_LOGO_LINES][MAX_LINE_LENGTH], int *info_lines) {
	double lavg[3] = {0.0};

	if (getloadavg(lavg, 3) != 3) {
		err(1, "getloadavg() failed");
	}
	append_info(info, info_lines, "Loadavg: %.2lf %.2lf %.2lf", lavg[0], lavg[1], lavg[2]);
}

void get_cpu(char info[MAX_LOGO_LINES][MAX_LINE_LENGTH], int *info_lines) {
	long ncpu, nmax;
	size_t sz;

	if ((ncpu = sysconf(_SC_NPROCESSORS_ONLN)) == -1) {
		err(1, "sysconf(_SC_NPROCESSORS_ONLN) failed");
	}
	if ((nmax = sysconf(_SC_NPROCESSORS_CONF)) == -1) {
		err(1, "sysconf(_SC_NPROCESSORS_CONF) failed");
	}

	sz = sizeof buf;
	if (sysctlbyname("machdep.cpu_brand", buf, &sz, NULL, 0) == -1) {
		if (sysctlbyname("hw.model", buf, &sz, NULL, 0) == -1) {
			err(1, "error getting CPU info.");
		}
	}

	buf[sz] = '\0';
	append_info(info, info_lines, "CPU: %s", buf);
	append_info(info, info_lines, "Cores: %ld of %ld processors online", ncpu, nmax);

#if defined(__FreeBSD__) || defined(__DragonFly__)
	#define CELSIUS 273.15
	for (int i = 0; i < (int)ncpu; i++) {
		int temp = 0;

		sz = sizeof temp;
		snprintf(buf, sizeof buf, "dev.cpu.%d.temperature", i);
		if (sysctlbyname(buf, &temp, &sz, NULL, 0) == -1) {
			return;
		}
		snprintf(buf, sizeof buf, "Core [%d] Temp", i + 1);
		append_info(info, info_lines, "%s: %.1f °C", buf, (temp * 0.1) - CELSIUS);
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
	if (sysctl(mib, 5, &sensors, &sz, NULL, 0) == -1) {
		return;
	}
	append_info(info, info_lines, "CPU Temp: %d °C", (int)((float)(sensors.value - 273150000) / 1E6));

#elif defined(__NetBSD__)
	const char *const cmd = "/usr/sbin/envstat | awk '/ cpu[0-9]+ temperature: / { print $3 }'";
	FILE *f = popen(cmd, "r");
	if (f == NULL) {
		err(1, "popen(%s) failed", cmd);
	}
	int i = 0;

	while (fgets(buf, sizeof buf, f) != NULL) {
		float temp;

		if (sscanf(buf, "%f", &temp) != 1) {
			break;
		}
		snprintf(buf, sizeof buf, "Core [%d] Temp", ++i);
		append_info(info, info_lines, "%s: %.1f °C", buf, temp);
	}

	if (pclose(f) != 0) {
		err(1, "pclose(%s) failed", cmd);
	}
#endif
}

void print_logo_and_info(Logo *logo, char info[MAX_LOGO_LINES][MAX_LINE_LENGTH], int info_lines) {
	int logo_lines = 0;
	while (logo->lines[logo_lines][0] != '\0' && logo_lines < MAX_LOGO_LINES) {
		logo_lines++;
	}

	int max_lines = logo_lines > info_lines ? logo_lines : info_lines;

	for (int i = 0; i < max_lines; i++) {
		if (i < logo_lines) {
			printf("%-40s", logo->lines[i]);
		} else {
			printf("%-40s", "");
		}

		if (i < info_lines) {
			printf("  %s", info[i]);
		}
		printf("\n");
	}
}

void detect_and_print_logo(void) {
	Logo logo = {0};
	char info[MAX_LOGO_LINES][MAX_LINE_LENGTH] = {{0}};
	int info_lines = 0;

#if defined(__FreeBSD__)
	read_logo(&logo, "freebsd.txt");
#elif defined(__OpenBSD__)
	read_logo(&logo, "openbsd.txt");
#elif defined(__NetBSD__)
	read_logo(&logo, "netbsd.txt");
#elif defined(__DragonFly__)
	read_logo(&logo, "dragonfly.txt");
#else
	#error Unsupported BSD variant
#endif

	get_sysinfo(info, &info_lines);
	get_hostname(info, &info_lines);
	get_shell(info, &info_lines);
	get_user(info, &info_lines);
	get_packages(info, &info_lines);
	get_uptime(info, &info_lines);
	get_memory(info, &info_lines);
	get_loadavg(info, &info_lines);
	get_cpu(info, &info_lines);

	print_logo_and_info(&logo, info, info_lines);
}

_Noreturn static void version(void) {
	printf("%s - version %s (%s)\n",
		   getprogname(),
		   VERSION,
		   __DATE__);
	exit(EXIT_SUCCESS);
}

_Noreturn static void usage(void) {
	printf("USAGE: %s [-h|-v]\n"
		   "   -h  Show this help text\n"
		   "   -v  Show version\n",
		   getprogname());
	exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
	if (argc == 2) {
		if (strcmp(argv[1], "-h") == 0) {
			usage();
		} else if (strcmp(argv[1], "-v") == 0) {
			version();
		}
	}
	detect_and_print_logo();

	return EXIT_SUCCESS;
}