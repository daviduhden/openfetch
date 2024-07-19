/*
 * Copyright (c) 2019-2021 Brian Callahan <bcallah@openbsd.org>
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

#include <errno.h>
#include <string.h>

#include "openbsd_internal.h"
#include "sysctlbyname.h"

// Function to get or set a sysctl value by its name
int sysctlbyname(const char *name, void *oldp, size_t *oldlenp,
	void *newp, size_t newlen)
{
	int i, mib[2];

	// Loop through the sysctlnames array to find the matching name
	for (i = 0; sysctlnames[i].name != NULL; i++) {
		// If the name matches, set the MIB (Management Information Base) values
		if (!strcmp(name, sysctlnames[i].name)) {
			mib[0] = sysctlnames[i].mib0;
			mib[1] = sysctlnames[i].mib1;

			// Call sysctl with the MIB values to get or set the value
			return sysctl(mib, 2, oldp, oldlenp, newp, newlen);
		}
	}

	// If the name is not found, set errno to ENOENT (No such file or directory)
	errno = ENOENT;

	// Return -1 to indicate failure
	return (-1);
}

