/*
 * Copyright (c) 2015 CloudGuard Software AG. All rights reserved.
 */

#include "tester.h"

#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

static void srvcb(struct tester *t, int fd)
{
	tester_read_eventreg(fd, "nosuchevent");
	tester_write_eventunknown(fd);
}

static void eventcb(struct davici_conn *c, int err, const char *name,
					struct davici_response *res, void *user)
{
	struct tester *t = user;

	assert(strcmp(name, "nosuchevent") == 0);
	assert(err == -EBADSLT);
	assert(res == NULL);

	tester_complete(t);
}

int main(int argc, char *argv[])
{
	struct tester *t;
	struct davici_conn *c;

	t = tester_create(srvcb);
	assert(davici_connect_unix(tester_getpath(t),
							   tester_davici_iocb, t, &c) >= 0);
	assert(davici_register(c, "nosuchevent", eventcb, t) >= 0);

	tester_runio(t, c);
	davici_disconnect(c);
	tester_cleanup(t);
	return 0;
}