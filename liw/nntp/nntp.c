/*
 * nntp.c -- some routines for NNTP clients
 *
 * Lars Wirzenius
 * Part of Publib, see README and publib(3) for more info.
 * "@(#)colawatcher:$Id: nntp.c,v 1.2 1995/08/05 21:40:52 liw Exp $"
 */


#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

extern int h_errno;

#include "publib/errormsg.h"
#include "publib/strutil.h"
#include "publib/log.h"
#include "publib/nntp.h"


/*
 * The port number of the NNTP service.
 */
#define NNTP_PORT 119



/*
 * Should we log I/O with the NNTP server?  If we do, the log level is
 * log_level_chat.
 */
static int log_input = 0;
static int log_output = 0;



/*
 * Local functions.
 */
static int send_cmd(int, const char *, ...);
static int recv_response(int);
static int read_line(int fd, char *buf);
static int nread(int fd, void *buf, int need);
int write_all(int fd, const void *s);




/*
 * Set log_input and log_output.
 */
void nntp_do_logs(int input, int output) {
	assert(input == 0 || input == 1);
	assert(output == 0 || output == 1);

	log_input = input;
	log_output = output;
}



/*
 * Open a connection to the NNTP server.  Set *fd to be a descriptor for
 * the connection (the client mustn't use it).  Return -1 for error,
 * 0 for OK (posting not allowed), or 1 for OK (posting allowed);
 */
int nntp_open(int *fd, const char *server, int use_mode_reader) {
	int s, tries, resp;
	struct in_addr addr;
	struct sockaddr_in serv;
	struct hostent *he;
	const int max = 16;	/* max # of retries for gethostby*-functions */

	assert(server != NULL);

	if (isdigit(*server)) {
		addr.s_addr = inet_addr(server);
		tries = 0;
		do {
			he = gethostbyaddr((void*)&addr, sizeof(addr), AF_INET);
		} while (he == NULL && h_errno == TRY_AGAIN && ++tries < max);
		if (he == NULL) {
			__publib_error("gethostbyaddr failed");
			return -1;
		}
	} else {
		tries = 0;
		do {
			he = gethostbyname(server);
		} while (he == NULL && h_errno == TRY_AGAIN && ++tries < max);
		if (he == NULL) {
			__publib_error("gethostbyname failed");
			return -1;
		}
	}

	assert(he->h_addrtype == AF_INET);

	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == -1) {
		__publib_error("couldn't create socket");
		return -1;
	}

	memset(&serv, 0, sizeof(serv));
	serv.sin_family = AF_INET;
	serv.sin_port = htons(NNTP_PORT);
	memcpy(&serv.sin_addr, he->h_addr_list[0], he->h_length);

	if (connect(s, (struct sockaddr *) &serv, sizeof(serv)) == -1) {
		__publib_error("couldn't connect to server");
		(void) close(s);
		return -1;
	}

	while ((resp = recv_response(s)) != -1 && resp != 200 && resp != 201)
		__publib_error("received unexpected response at startup");

	if (use_mode_reader) {
		if (send_cmd(s, "mode reader") == -1) {
			(void) close(s);
			return -1;
		}
		while ((resp = recv_response(s)) != -1
			&& resp != 200 && resp != 201) {
				__publib_error("received unexpected response "
					"after `mode reader'");
		}
	}

	*fd = s;
	return (resp == 200) ? 1 : 0;
}



/*
 * Close an NNTP connection.  Return -1 for error, 0 for OK.
 */
int nntp_close(int fd) {
	if (close(fd) == -1) {
		__publib_error("error closing NNTP connection");
		return -1;
	}
	return 0;
}



/*
 * Send the NEWNEWS command.  Return -1 for error, 0 for OK.  The message
 * identifiers must be read separately, using nntp_next_line.
 */
int nntp_newnews(int fd, const char *grp, const char *date, const char *time,
		const char *tz) {
	if (send_cmd(fd, "NEWNEWS %s %s %s%s%s", grp, date, time,
			*tz == '\0' ? "" : " ", tz) == -1)
		return -1;

	for (;;) {
		switch (recv_response(fd)) {
		case 230: return 0;
		case 400:
		case 500:
		case 501:
		case 502:
		case 503: return -1;
		}
	}
	/*NOTREACHED*/
}



/*
 * Start posting an article.  Return -1 for error, 0 for OK.  If OK, then
 * caller must use nntp_write_next_line() and nntp_write_end_line() to
 * send the actual article.
 */
int nntp_post(int fd) {
	if (send_cmd(fd, "POST") == -1)
		return -1;

	if (recv_response(fd) == 340)
		return 0;
	return -1;
}



/*
 * Select a given group.  Return -1 for error, 0 for OK.
 */
int nntp_group(int fd, const char *group) {
	if (send_cmd(fd, "GROUP %s", group) == -1)
		return -1;
	if (recv_response(fd) == 211)
		return 0;
	return -1;
}



/*
 * Ask for the head and body of the current article.  Return 0 for OK
 * (read the text with nntp_next_line), or -1 for error.
 */
int nntp_get_current_article(int fd) {
	if (send_cmd(fd, "ARTICLE") == -1)
		return -1;
	if (recv_response(fd) == 220)
		return 0;
	return -1;
}



/*
 * Select next article.  Return -1 for error, or 0 for OK (request text
 * of article with nntp_get_current_article).
 */
int nntp_goto_next_article(int fd) {
	if (send_cmd(fd, "NEXT") == -1)
		return -1;
	switch (recv_response(fd)) {
	case 223:
		return 1;
	case 421:
		return 0;
	}
	return -1;
}



/*
 * Read next line from the NNTP connection; for users, this should always
 * be a data line, not a numeric+textual response from the server.
 * Return -1 for error, 0 for EOF (no more lines) and 1 for OK.  Set
 * *line to point to the correct line; an internal buffer that will be
 * changed by the next call.
 */
int nntp_next_line(int fd, char **line) {
	static char buf[1024];	/* should be long enough */

	if (read_line(fd, buf) == -1)
		return -1;
	if (strcmp(buf, ".") == 0)
		return 0;
	if (buf[0] == '.')
		*line = buf+1;
	else
		*line = buf;
	return 1;
}



/*
 * Write next line of data to the server.  If line begins with a dot,
 * prefix it with another dot.  Return -1 for error, 0 for OK.
 * Note that the line must be at most 10000 characters long.
 */
int nntp_write_next_line(int fd, const char *line) {
	size_t n;
	char buf[10240];

	assert(line != NULL);

	n = strlen(line);
	assert(n+1 < sizeof(buf));

	*buf = '\0';
	if (*line == '.')
		strcat(buf, ".");
	strcat(buf, line);
	strendzap(buf, "\n");
	strendzap(buf, "\r");

	if (send_cmd(fd, "%s", buf) == -1)
		return -1;
	return 0;
}



/*
 * Write the end line for a data section (i.e., a solitary dot).
 * Return -1 for error, 0 for ok.
 */
int nntp_write_end_line(int fd) {
	if (send_cmd(fd, ".") == -1)
		return -1;
	if (recv_response(fd) != 240)
		return -1;
	return 0;
}




/*
 * Send a command to the server.
 */
static int send_cmd(int fd, const char *fmt, ...) {
	va_list args;
	char buf[10240];

	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);

	if (strlen(buf) > 510) {
		__publib_error("output line too long!\n");
		return -1;
	}

	if (log_output)
		log_chat("nntp: sending: `%s'\n", buf);
	strcat(buf, "\r\n");

	if (write_all(fd, buf) == -1)
		return -1;
	return 0;
}



/*
 * Read a response from the server (a line beginning with a numeric code).
 * Bypass all informative messages.  Return the numeric code.
 */
static int recv_response(int fd) {
	char buf[10240];
	int code;

	if (read_line(fd, buf) == -1)
		return -1;

	code = atoi(buf);
	if (code < 100 || code > 599)
		return -1;
	return code;
}



/*
 * Read a whole line (at most 512 bytes).
 */
static int read_line(int fd, char *buf) {
	int ret;
	char *start;

	start = buf;
	while (buf-start<512 && (ret = nread(fd, buf, 1)) == 0 && *buf != '\n')
		++buf;
	if (ret != 0)
		return -1;
	assert(*buf == '\n');
	if (buf > start && buf[-1] == '\r')
		--buf;
	*buf = '\0';

	if (log_input)
		log_chat("nntp: from server: `%s'\n", start);
	return 0;
}


/*
 * Read `need' bytes from file/socket descriptor
 */
static int nread(int fd, void *buf, int need) {
        char *p;
        int n;

        p = buf;
        while (need > 0) {
                n = read(fd, p, need);
                if (n < 0) {
                        __publib_error("read failed");
                        return -1;
                }
                if (n == 0) {
                        __publib_error("got 0 bytes when expecting more");
                        return -1;
                }
                need -= n;
                p += n;
        }
        return 0;
}




/*
 * Write the whole string, even if there are signals.
 */
int write_all(int fd, const void *s) {
        size_t len;
        int n;

        len = strlen(s);
        while (len > 0) {
                n = write(fd, s, len);
                if (n < 0) {
                        __publib_error("write failed");
                        return -1;
                }
                len -= n;
        }
        return 0;
}
