/*
 * Copyright (c) 2019 Brian Callahan <bcallah@openbsd.org>
 * Copyright (c) 2024-2025 David Uhden Collado <david@uhden.dev>
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

#include <sys/sysctl.h>
#include <sys/types.h>

/*
 * Declaration of the sysctlbyname function.
 *
 * Parameters:
 * - const char *name: The name of the system control variable.
 * - void *oldp: A pointer to a buffer where the value of the system control
 *   variable will be stored.
 * - size_t *oldlenp: A pointer to a variable that specifies the size of the
 *   buffer pointed to by oldp.
 * - void *newp: A pointer to a buffer containing the new value to be set for
 *   the system control variable.
 * - size_t newlen: The size of the buffer pointed to by newp.
 *
 * Return value:
 * - Returns 0 on success, or -1 on error with errno set to indicate the error.
 */
extern int sysctlbyname(const char *, void *, size_t *, void *, size_t);
