/*	$OpenBSD: test-time.c,v 1.3 2003/03/10 04:02:50 david Exp $	*/

/*
 * Compile with:
 * cc -I/usr/local/include -o time-test time-test.c -L/usr/local/lib -levent
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <event.h>

int called = 0;

#define NEVENT	20000

struct event *ev[NEVENT];

void
time_cb(int fd, short event, void *arg)
{
	struct timeval tv;
	int i, j;

	called++;

	if (called < 10*NEVENT) {
		for (i = 0; i < 10; i++) {
			j = random() % NEVENT;
			tv.tv_sec = 0;
			tv.tv_usec = random() % 50000L;
			if (tv.tv_usec % 2)
				timeout_add(ev[j], &tv);
			else
				timeout_del(ev[j]);
		}
	}
}

int
main (int argc, char **argv)
{
	struct timeval tv;
	int i;

	/* Initialize the event library */
	event_init();

	for (i = 0; i < NEVENT; i++) {
		ev[i] = malloc(sizeof(struct event));

		/* Initialize one event */
		timeout_set(ev[i], time_cb, ev[i]);
		tv.tv_sec = 0;
		tv.tv_usec = random() % 50000L;
		timeout_add(ev[i], &tv);
	}

	event_dispatch();

	return (called < NEVENT);
}

