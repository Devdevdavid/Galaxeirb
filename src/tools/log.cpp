/*
 * Copyright (c) 2017 rxi
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "log.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>


static struct {
	void *udata;
	log_LockFn lock;
	char path[150];
	int level;
	int quiet;
} L;


static const char *level_names[] = {
		"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
};

#ifdef LOG_USE_COLOR
static const char *level_colors[] = {
		"\x1b[94m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m"
};
#endif


static void lock(void)   {
	if (L.lock) {
		L.lock(L.udata, 1);
	}
}


static void unlock(void) {
	if (L.lock) {
		L.lock(L.udata, 0);
	}
}


void log_set_udata(void *udata) {
	L.udata = udata;
}

void log_set_lock(log_LockFn fn) {
	L.lock = fn;
}

/**
 * Set the new file path and check if it is not too big
 * If so, the existing file is removed
 * @param path
 */
void log_set_fp(const char * path) {
	int fd, retValue;
	struct stat fileStat;

	// Update path
	strcpy(L.path, path);

	// Open the file
	fd = open(L.path, O_RDONLY);
	if (fd != -1) {
		// Get file stats
		retValue = fstat(fd, &fileStat);
		close(fd);

		if (retValue != -1) {
			// Check size
			if (fileStat.st_size > LOG_MAX_FILE_SIZE) {
				// Remove file if too big (will be re-created at the first log_log())
				remove(L.path);
			}
		}
	}
}

void log_set_level(int level) {
	L.level = level;
}

void log_log(int level, const char *file, int line, const char *fmt, ...) {
	if (level <= L.level) {
		//return;
	}
	va_list args;

	/* Get current time */
	time_t t = time(NULL);
	struct tm *lt = localtime(&t);

	/* Log to file */
	if (level != LOG_DEBUG) {
		/* Open file */
		FILE * fp = NULL;
		if ((fp = fopen(L.path, "a")) == NULL) {
			return;
		}

		/* Acquire lock */
		lock();

		if (fp) {
			char buf[32];
			buf[strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", lt)] = '\0';
#ifdef LOG_USE_COLOR
			fprintf(fp, "%s %s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m ",
					buf, level_colors[level], level_names[level], file, line);
#else
			fprintf(fp, "%s %-5s %s:%d: ", buf, level_names[level], file, line);
#endif
			va_start(args, fmt);
			vfprintf(fp, fmt, args);
			va_end(args);
			fprintf(fp, "\n");
			fclose(fp);
		}
	}

	/* Log to console */
	printf("%-5s %s:%d: ", level_names[level], file, line);
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
	printf("\n");
	fflush(stdout);

	/* Release lock */
	unlock();
}
