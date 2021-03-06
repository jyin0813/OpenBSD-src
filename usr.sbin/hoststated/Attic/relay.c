/*	$OpenBSD: relay.c,v 1.74 2007/11/28 16:25:12 reyk Exp $	*/

/*
 * Copyright (c) 2006, 2007 Reyk Floeter <reyk@openbsd.org>
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
#include <sys/queue.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/tree.h>
#include <sys/hash.h>
#include <sys/resource.h>

#include <net/if.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <err.h>
#include <pwd.h>
#include <event.h>
#include <fnmatch.h>

#include <openssl/ssl.h>

#include "hoststated.h"

void		 relay_sig_handler(int sig, short, void *);
void		 relay_statistics(int, short, void *);
void		 relay_dispatch_pfe(int, short, void *);
void		 relay_dispatch_parent(int, short, void *);
void		 relay_shutdown(void);

void		 relay_privinit(void);
void		 relay_nodedebug(const char *, struct protonode *);
void		 relay_protodebug(struct relay *);
void		 relay_init(void);
void		 relay_launch(void);
int		 relay_socket_af(struct sockaddr_storage *, in_port_t);
int		 relay_socket(struct sockaddr_storage *, in_port_t,
		    struct protocol *);
int		 relay_socket_listen(struct sockaddr_storage *, in_port_t,
		    struct protocol *);
int		 relay_socket_connect(struct sockaddr_storage *, in_port_t,
		    struct protocol *);

void		 relay_accept(int, short, void *);
void		 relay_input(struct session *);
void		 relay_close(struct session *, const char *);
void		 relay_session(struct session *);
void		 relay_natlook(int, short, void *);

int		 relay_connect(struct session *);
void		 relay_connected(int, short, void *);

u_int32_t	 relay_hash_addr(struct sockaddr_storage *, u_int32_t);
int		 relay_from_table(struct session *);

void		 relay_write(struct bufferevent *, void *);
void		 relay_read(struct bufferevent *, void *);
void		 relay_error(struct bufferevent *, short, void *);
void		 relay_dump(struct ctl_relay_event *, const void *, size_t);

int		 relay_resolve(struct ctl_relay_event *,
		    struct protonode *, struct protonode *);
int		 relay_handle_http(struct ctl_relay_event *,
		    struct protonode *, struct protonode *,
		    struct protonode *, int);
void		 relay_read_http(struct bufferevent *, void *);
static int	_relay_lookup_url(struct ctl_relay_event *, char *, char *,
		    char *, enum digest_type);
int		 relay_lookup_url(struct ctl_relay_event *,
		    const char *, enum digest_type);
int		 relay_lookup_query(struct ctl_relay_event *);
int		 relay_lookup_cookie(struct ctl_relay_event *, const char *);
void		 relay_read_httpcontent(struct bufferevent *, void *);
void		 relay_read_httpchunks(struct bufferevent *, void *);
char		*relay_expand_http(struct ctl_relay_event *, char *,
		    char *, size_t);
void		 relay_close_http(struct session *, u_int, const char *,
		    u_int16_t);

SSL_CTX		*relay_ssl_ctx_create(struct relay *);
void		 relay_ssl_transaction(struct session *);
void		 relay_ssl_accept(int, short, void *);
void		 relay_ssl_connected(struct ctl_relay_event *);
void		 relay_ssl_readcb(int, short, void *);
void		 relay_ssl_writecb(int, short, void *);

int		 relay_bufferevent_add(struct event *, int);
#ifdef notyet
int		 relay_bufferevent_printf(struct ctl_relay_event *,
		    const char *, ...);
#endif
int		 relay_bufferevent_print(struct ctl_relay_event *, char *);
int		 relay_bufferevent_write_buffer(struct ctl_relay_event *,
		    struct evbuffer *);
int		 relay_bufferevent_write_chunk(struct ctl_relay_event *,
		    struct evbuffer *, size_t);
int		 relay_bufferevent_write(struct ctl_relay_event *,
		    void *, size_t);
int		 relay_cmp_af(struct sockaddr_storage *,
		    struct sockaddr_storage *);
char		*relay_load_file(const char *, off_t *);
static __inline int
		 relay_proto_cmp(struct protonode *, struct protonode *);
extern void	 bufferevent_read_pressure_cb(struct evbuffer *, size_t,
		    size_t, void *);

volatile sig_atomic_t relay_sessions;
objid_t relay_conid;

static struct hoststated	*env = NULL;
struct imsgbuf			*ibuf_pfe;
struct imsgbuf			*ibuf_main;
int				 proc_id;

#if DEBUG > 1
#define DPRINTF		log_debug
#else
#define DPRINTF(x...)	do { } while(0)
#endif

void
relay_sig_handler(int sig, short event, void *arg)
{
	switch (sig) {
	case SIGTERM:
	case SIGINT:
		(void)event_loopexit(NULL);
	}
}

pid_t
relay(struct hoststated *x_env, int pipe_parent2pfe[2], int pipe_parent2hce[2],
    int pipe_parent2relay[RELAY_MAXPROC][2], int pipe_pfe2hce[2],
    int pipe_pfe2relay[RELAY_MAXPROC][2])
{
	pid_t		 pid;
	struct passwd	*pw;
	struct event	 ev_sigint;
	struct event	 ev_sigterm;
	int		 i;

	switch (pid = fork()) {
	case -1:
		fatal("relay: cannot fork");
	case 0:
		break;
	default:
		return (pid);
	}

	env = x_env;
	purge_config(env, PURGE_SERVICES);

	/* Need root privileges for relay initialization */
	relay_privinit();

	if ((pw = getpwnam(HOSTSTATED_USER)) == NULL)
		fatal("relay: getpwnam");

#ifndef DEBUG
	if (chroot(pw->pw_dir) == -1)
		fatal("relay: chroot");
	if (chdir("/") == -1)
		fatal("relay: chdir(\"/\")");

#else
#warning disabling privilege revocation and chroot in DEBUG mode
#endif

	setproctitle("socket relay engine");
	hoststated_process = PROC_RELAY;

#ifndef DEBUG
	if (setgroups(1, &pw->pw_gid) ||
	    setresgid(pw->pw_gid, pw->pw_gid, pw->pw_gid) ||
	    setresuid(pw->pw_uid, pw->pw_uid, pw->pw_uid))
		fatal("relay: can't drop privileges");
#endif

	/* Fork child handlers */
	for (i = 1; i < env->prefork_relay; i++) {
		if (fork() == 0) {
			proc_id = i;
			break;
		}
	}

	event_init();

	/* Per-child initialization */
	relay_init();

	signal_set(&ev_sigint, SIGINT, relay_sig_handler, NULL);
	signal_set(&ev_sigterm, SIGTERM, relay_sig_handler, NULL);
	signal_add(&ev_sigint, NULL);
	signal_add(&ev_sigterm, NULL);
	signal(SIGHUP, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);

	/* setup pipes */
	close(pipe_pfe2hce[0]);
	close(pipe_pfe2hce[1]);
	close(pipe_parent2hce[0]);
	close(pipe_parent2hce[1]);
	close(pipe_parent2pfe[0]);
	close(pipe_parent2pfe[1]);
	for (i = 0; i < env->prefork_relay; i++) {
		if (i == proc_id)
			continue;
		close(pipe_parent2relay[i][0]);
		close(pipe_parent2relay[i][1]);
		close(pipe_pfe2relay[i][0]);
		close(pipe_pfe2relay[i][1]);
	}
	close(pipe_parent2relay[proc_id][1]);
	close(pipe_pfe2relay[proc_id][1]);

	if ((ibuf_pfe = calloc(1, sizeof(struct imsgbuf))) == NULL ||
	    (ibuf_main = calloc(1, sizeof(struct imsgbuf))) == NULL)
		fatal("relay");
	imsg_init(ibuf_main, pipe_parent2relay[proc_id][0],
	    relay_dispatch_parent);
	imsg_init(ibuf_pfe, pipe_pfe2relay[proc_id][0], relay_dispatch_pfe);

	ibuf_pfe->events = EV_READ;
	event_set(&ibuf_pfe->ev, ibuf_pfe->fd, ibuf_pfe->events,
	    ibuf_pfe->handler, ibuf_pfe);
	event_add(&ibuf_pfe->ev, NULL);

	ibuf_main->events = EV_READ;
	event_set(&ibuf_main->ev, ibuf_main->fd, ibuf_main->events,
	    ibuf_main->handler, ibuf_main);
	event_add(&ibuf_main->ev, NULL);

	relay_launch();

	event_dispatch();
	relay_shutdown();

	return (0);
}

void
relay_shutdown(void)
{
	struct session	*con;

	struct relay	*rlay;
	TAILQ_FOREACH(rlay, env->relays, entry) {
		if (rlay->conf.flags & F_DISABLE)
			continue;
		close(rlay->s);
		while ((con = SPLAY_ROOT(&rlay->sessions)) != NULL)
			relay_close(con, "shutdown");
	}
	usleep(200);	/* XXX relay needs to shutdown last */
	log_info("socket relay engine exiting");
	_exit(0);
}

void
relay_nodedebug(const char *name, struct protonode *pn)
{
	const char	*s;
	int		 digest;

	if (pn->action == NODE_ACTION_NONE)
		return;

	fprintf(stderr, "\t\t");
	fprintf(stderr, "%s ", name);

	switch (pn->type) {
	case NODE_TYPE_HEADER:
		break;
	case NODE_TYPE_QUERY:
		fprintf(stderr, "query ");
		break;
	case NODE_TYPE_COOKIE:
		fprintf(stderr, "cookie ");
		break;
	case NODE_TYPE_PATH:
		fprintf(stderr, "path ");
		break;
	case NODE_TYPE_URL:
		fprintf(stderr, "url ");
		break;
	}

	switch (pn->action) {
	case NODE_ACTION_APPEND:
		fprintf(stderr, "append \"%s\" to \"%s\"",
		    pn->value, pn->key);
		break;
	case NODE_ACTION_CHANGE:
		fprintf(stderr, "change \"%s\" to \"%s\"",
		    pn->key, pn->value);
		break;
	case NODE_ACTION_REMOVE:
		fprintf(stderr, "remove \"%s\"",
		    pn->key);
		break;
	case NODE_ACTION_EXPECT:
	case NODE_ACTION_FILTER:
		s = pn->action == NODE_ACTION_EXPECT ? "expect" : "filter";
		digest = pn->flags & PNFLAG_LOOKUP_URL_DIGEST;
		if (strcmp(pn->value, "*") == 0)
			fprintf(stderr, "%s %s\"%s\"", s,
			    digest ? "digest " : "", pn->key);
		else
			fprintf(stderr, "%s \"%s\" from \"%s\"", s,
			    pn->value, pn->key);
		break;
	case NODE_ACTION_HASH:
		fprintf(stderr, "hash \"%s\"", pn->key);
		break;
	case NODE_ACTION_LOG:
		fprintf(stderr, "log \"%s\"", pn->key);
		break;
	case NODE_ACTION_MARK:
		if (strcmp(pn->value, "*") == 0)
			fprintf(stderr, "mark \"%s\"", pn->key);
		else
			fprintf(stderr, "mark \"%s\" from \"%s\"",
			    pn->value, pn->key);
		break;
	case NODE_ACTION_NONE:
		break;
	}
	fprintf(stderr, "\n");
}

void
relay_protodebug(struct relay *rlay)
{
	struct protocol		*proto = rlay->proto;
	struct protonode	*proot, *pn;
	struct proto_tree	*tree;
	const char		*name;
	int			 i;

	fprintf(stderr, "protocol %d: name %s\n", proto->id, proto->name);
	fprintf(stderr, "\tflags: 0x%04x\n", proto->flags);
	if (proto->cache != -1)
		fprintf(stderr, "\tssl session cache: %d\n", proto->cache);
	fprintf(stderr, "\ttype: ");
	switch (proto->type) {
	case RELAY_PROTO_TCP:
		fprintf(stderr, "tcp\n");
		break;
	case RELAY_PROTO_HTTP:
		fprintf(stderr, "http\n");
		break;
	case RELAY_PROTO_DNS:
		fprintf(stderr, "dns\n");
		break;
	}

	name = "request";
	tree = &proto->request_tree;
 show:
	i = 0;
	RB_FOREACH(proot, proto_tree, tree) {
		PROTONODE_FOREACH(pn, proot, entry) {
#ifndef DEBUG
			if (++i > 100)
				break;
#endif
			relay_nodedebug(name, pn);
		}
#ifndef DEBUG
		/* Limit the number of displayed lines */
		if (++i > 100) {
			fprintf(stderr, "\t\t...\n");
			break;
		}
#endif
	}
	if (tree == &proto->request_tree) {
		name = "response";
		tree = &proto->response_tree;
		goto show;
	}
}

void
relay_privinit(void)
{
	struct relay	*rlay;
	extern int	 debug;

	if (env->flags & F_SSL)
		ssl_init(env);

	TAILQ_FOREACH(rlay, env->relays, entry) {
		log_debug("relay_privinit: adding relay %s", rlay->conf.name);

		if (debug)
			relay_protodebug(rlay);

		switch (rlay->proto->type) {
		case RELAY_PROTO_DNS:
			relay_udp_privinit(env, rlay);
			break;
		case RELAY_PROTO_TCP:
		case RELAY_PROTO_HTTP:
			/* Use defaults */
			break;
		}

		if (rlay->conf.flags & F_UDP)
			rlay->s = relay_udp_bind(&rlay->conf.ss,
			    rlay->conf.port, rlay->proto);
		else
			rlay->s = relay_socket_listen(&rlay->conf.ss,
			    rlay->conf.port, rlay->proto);
		if (rlay->s == -1)
			fatal("relay_privinit: failed to listen");
	}
}

void
relay_init(void)
{
	struct relay	*rlay;
	struct host	*host;
	struct timeval	 tv;
	struct rlimit	 rl;

	if (getrlimit(RLIMIT_NOFILE, &rl) == -1)
		fatal("relay_init: failed to get resource limit");
	log_debug("relay_init: max open files %d", rl.rlim_max);

	/*
	 * Allow the maximum number of open file descriptors for this
	 * login class (which should be the class "daemon" by default).
	 */
	rl.rlim_cur = rl.rlim_max;
	if (setrlimit(RLIMIT_NOFILE, &rl) == -1)
		fatal("relay_init: failed to set resource limit");

	TAILQ_FOREACH(rlay, env->relays, entry) {
		if ((rlay->conf.flags & F_SSL) &&
		    (rlay->ssl_ctx = relay_ssl_ctx_create(rlay)) == NULL)
			fatal("relay_init: failed to create SSL context");

		if (rlay->dsttable != NULL) {
			switch (rlay->conf.dstmode) {
			case RELAY_DSTMODE_ROUNDROBIN:
				rlay->dstkey = 0;
				break;
			case RELAY_DSTMODE_LOADBALANCE:
			case RELAY_DSTMODE_HASH:
				rlay->dstkey =
				    hash32_str(rlay->conf.name, HASHINIT);
				rlay->dstkey =
				    hash32_str(rlay->dsttable->conf.name,
				    rlay->dstkey);
				break;
			}
			rlay->dstnhosts = 0;
			TAILQ_FOREACH(host, &rlay->dsttable->hosts, entry) {
				if (rlay->dstnhosts >= RELAY_MAXHOSTS)
					fatal("relay_init: "
					    "too many hosts in table");
				host->idx = rlay->dstnhosts;
				rlay->dsthost[rlay->dstnhosts++] = host;
			}
			log_info("adding %d hosts from table %s%s",
			    rlay->dstnhosts, rlay->dsttable->conf.name,
			    rlay->conf.dstcheck ? "" : " (no check)");
		}
	}

	/* Schedule statistics timer */
	evtimer_set(&env->statev, relay_statistics, NULL);
	bcopy(&env->statinterval, &tv, sizeof(tv));
	evtimer_add(&env->statev, &tv);
}

void
relay_statistics(int fd, short events, void *arg)
{
	struct relay		*rlay;
	struct ctl_stats	 crs, *cur;
	struct timeval		 tv, tv_now;
	int			 resethour = 0, resetday = 0;
	struct session		*con, *next_con;

	/*
	 * This is a hack to calculate some average statistics.
	 * It doesn't try to be very accurate, but could be improved...
	 */

	timerclear(&tv);
	if (gettimeofday(&tv_now, NULL))
		fatal("relay_init: gettimeofday");

	TAILQ_FOREACH(rlay, env->relays, entry) {
		bzero(&crs, sizeof(crs));
		resethour = resetday = 0;

		cur = &rlay->stats[proc_id];
		cur->cnt += cur->last;
		cur->tick++;
		cur->avg = (cur->last + cur->avg) / 2;
		cur->last_hour += cur->last;
		if ((cur->tick % (3600 / env->statinterval.tv_sec)) == 0) {
			cur->avg_hour = (cur->last_hour + cur->avg_hour) / 2;
			resethour++;
		}
		cur->last_day += cur->last;
		if ((cur->tick % (86400 / env->statinterval.tv_sec)) == 0) {
			cur->avg_day = (cur->last_day + cur->avg_day) / 2;
			resethour++;
		}
		bcopy(cur, &crs, sizeof(crs));

		cur->last = 0;
		if (resethour)
			cur->last_hour = 0;
		if (resetday)
			cur->last_day = 0;

		crs.id = rlay->conf.id;
		crs.proc = proc_id;
		imsg_compose(ibuf_pfe, IMSG_STATISTICS, 0, 0, -1,
		    &crs, sizeof(crs));

		for (con = SPLAY_ROOT(&rlay->sessions);
		    con != NULL; con = next_con) {
			next_con = SPLAY_NEXT(session_tree,
			    &rlay->sessions, con);
			timersub(&tv_now, &con->tv_last, &tv);
			if (timercmp(&tv, &rlay->conf.timeout, >=))
				relay_close(con, "hard timeout");
		}
	}

	/* Schedule statistics timer */
	evtimer_set(&env->statev, relay_statistics, NULL);
	bcopy(&env->statinterval, &tv, sizeof(tv));
	evtimer_add(&env->statev, &tv);
}

void
relay_launch(void)
{
	struct relay	*rlay;
	void		(*callback)(int, short, void *);

	TAILQ_FOREACH(rlay, env->relays, entry) {
		log_debug("relay_launch: running relay %s", rlay->conf.name);

		rlay->up = HOST_UP;

		if (rlay->conf.flags & F_UDP)
			callback = relay_udp_server;
		else
			callback = relay_accept;

		event_set(&rlay->ev, rlay->s, EV_READ|EV_PERSIST,
		    callback, rlay);
		event_add(&rlay->ev, NULL);
	}
}

int
relay_socket_af(struct sockaddr_storage *ss, in_port_t port)
{
	switch (ss->ss_family) {
	case AF_INET:
		((struct sockaddr_in *)ss)->sin_port = port;
		((struct sockaddr_in *)ss)->sin_len =
		    sizeof(struct sockaddr_in);
		break;
	case AF_INET6:
		((struct sockaddr_in6 *)ss)->sin6_port = port;
		((struct sockaddr_in6 *)ss)->sin6_len =
		    sizeof(struct sockaddr_in6);
		break;
	default:
		return (-1);
	}

	return (0);
}

int
relay_socket(struct sockaddr_storage *ss, in_port_t port,
    struct protocol *proto)
{
	int s = -1, val;
	struct linger lng;

	if (relay_socket_af(ss, port) == -1)
		goto bad;

	if ((s = socket(ss->ss_family, SOCK_STREAM, IPPROTO_TCP)) == -1)
		goto bad;

	/*
	 * Socket options
	 */
	bzero(&lng, sizeof(lng));
	if (setsockopt(s, SOL_SOCKET, SO_LINGER, &lng, sizeof(lng)) == -1)
		goto bad;
	val = 1;
	if (setsockopt(s, SOL_SOCKET, SO_REUSEPORT, &val, sizeof(int)) == -1)
		goto bad;
	if (fcntl(s, F_SETFL, O_NONBLOCK) == -1)
		goto bad;
	if (proto->tcpflags & TCPFLAG_BUFSIZ) {
		val = proto->tcpbufsiz;
		if (setsockopt(s, SOL_SOCKET, SO_RCVBUF,
		    &val, sizeof(val)) == -1)
			goto bad;
		val = proto->tcpbufsiz;
		if (setsockopt(s, SOL_SOCKET, SO_SNDBUF,
		    &val, sizeof(val)) == -1)
			goto bad;
	}

	/*
	 * IP options
	 */
	if (proto->tcpflags & TCPFLAG_IPTTL) {
		val = (int)proto->tcpipttl;
		if (setsockopt(s, IPPROTO_IP, IP_TTL,
		    &val, sizeof(val)) == -1)
			goto bad;
	}
	if (proto->tcpflags & TCPFLAG_IPMINTTL) {
		val = (int)proto->tcpipminttl;
		if (setsockopt(s, IPPROTO_IP, IP_MINTTL,
		    &val, sizeof(val)) == -1)
			goto bad;
	}

	/*
	 * TCP options
	 */
	if (proto->tcpflags & (TCPFLAG_NODELAY|TCPFLAG_NNODELAY)) {
		if (proto->tcpflags & TCPFLAG_NNODELAY)
			val = 0;
		else
			val = 1;
		if (setsockopt(s, IPPROTO_TCP, TCP_NODELAY,
		    &val, sizeof(val)) == -1)
			goto bad;
	}
	if (proto->tcpflags & (TCPFLAG_SACK|TCPFLAG_NSACK)) {
		if (proto->tcpflags & TCPFLAG_NSACK)
			val = 0;
		else
			val = 1;
		if (setsockopt(s, IPPROTO_TCP, TCP_SACK_ENABLE,
		    &val, sizeof(val)) == -1)
			goto bad;
	}

	return (s);

 bad:
	if (s != -1)
		close(s);
	return (-1);
}

int
relay_socket_connect(struct sockaddr_storage *ss, in_port_t port,
    struct protocol *proto)
{
	int s;

	if ((s = relay_socket(ss, port, proto)) == -1)
		return (-1);

	if (connect(s, (struct sockaddr *)ss, ss->ss_len) == -1) {
		if (errno != EINPROGRESS)
			goto bad;
	}

	return (s);

 bad:
	close(s);
	return (-1);
}

int
relay_socket_listen(struct sockaddr_storage *ss, in_port_t port,
    struct protocol *proto)
{
	int s;

	if ((s = relay_socket(ss, port, proto)) == -1)
		return (-1);

	if (bind(s, (struct sockaddr *)ss, ss->ss_len) == -1)
		goto bad;
	if (listen(s, proto->tcpbacklog) == -1)
		goto bad;

	return (s);

 bad:
	close(s);
	return (-1);
}

void
relay_connected(int fd, short sig, void *arg)
{
	struct session		*con = (struct session *)arg;
	struct relay		*rlay = (struct relay *)con->relay;
	struct protocol		*proto = rlay->proto;
	evbuffercb		 outrd = relay_read;
	evbuffercb		 outwr = relay_write;
	struct bufferevent	*bev;

	if (sig == EV_TIMEOUT) {
		relay_close_http(con, 504, "connect timeout", 0);
		return;
	}

	DPRINTF("relay_connected: session %d: %ssuccessful",
	    con->id, rlay->proto->lateconnect ? "late connect " : "");

	switch (rlay->proto->type) {
	case RELAY_PROTO_HTTP:
		/* Check the servers's HTTP response */
		if (!RB_EMPTY(&rlay->proto->response_tree)) {
			outrd = relay_read_http;
			if ((con->out.nodes = calloc(proto->response_nodes,
			    sizeof(u_int8_t))) == NULL) {
				relay_close_http(con, 500,
				    "failed to allocate nodes", 0);
				return;
			}
		}
		break;
	case RELAY_PROTO_TCP:
		/* Use defaults */
		break;
	default:
		fatalx("relay_input: unknown protocol");
	}

	/*
	 * Relay <-> Server
	 */
	bev = bufferevent_new(fd, outrd, outwr, relay_error, &con->out);
	if (bev == NULL) {
		relay_close_http(con, 500,
		    "failed to allocate output buffer event", 0);
		return;
	}
	evbuffer_free(bev->output);
	bev->output = con->out.output;
	if (bev->output == NULL)
		fatal("relay_connected: invalid output buffer");

	con->out.bev = bev;
	bufferevent_settimeout(bev,
	    rlay->conf.timeout.tv_sec, rlay->conf.timeout.tv_sec);
	bufferevent_enable(bev, EV_READ|EV_WRITE);
}

void
relay_input(struct session *con)
{
	struct relay	*rlay = (struct relay *)con->relay;
	struct protocol *proto = rlay->proto;
	evbuffercb	 inrd = relay_read;
	evbuffercb	 inwr = relay_write;

	switch (rlay->proto->type) {
	case RELAY_PROTO_HTTP:
		/* Check the client's HTTP request */
		if (!RB_EMPTY(&rlay->proto->request_tree) ||
		    proto->lateconnect) {
			inrd = relay_read_http;
			if ((con->in.nodes = calloc(proto->request_nodes,
			    sizeof(u_int8_t))) == NULL) {
				relay_close(con, "failed to allocate nodes");
				return;
			}
		}
		break;
	case RELAY_PROTO_TCP:
		/* Use defaults */
		break;
	default:
		fatalx("relay_input: unknown protocol");
	}

	/*
	 * Client <-> Relay
	 */
	con->in.bev = bufferevent_new(con->in.s, inrd, inwr,
	    relay_error, &con->in);
	if (con->in.bev == NULL) {
		relay_close(con, "failed to allocate input buffer event");
		return;
	}

	/* Initialize the SSL wrapper */
	if ((rlay->conf.flags & F_SSL) && con->in.ssl != NULL)
		relay_ssl_connected(&con->in);

	bufferevent_settimeout(con->in.bev,
	    rlay->conf.timeout.tv_sec, rlay->conf.timeout.tv_sec);
	bufferevent_enable(con->in.bev, EV_READ|EV_WRITE);
}

void
relay_write(struct bufferevent *bev, void *arg)
{
	struct ctl_relay_event	*cre = (struct ctl_relay_event *)arg;
	struct session		*con = (struct session *)cre->con;
	if (gettimeofday(&con->tv_last, NULL))
		con->done = 1;
	if (con->done)
		relay_close(con, "last write (done)");
}

void
relay_dump(struct ctl_relay_event *cre, const void *buf, size_t len)
{
	/*
	 * This function will dump the specified message directly
	 * to the underlying session, without waiting for success
	 * of non-blocking events etc. This is useful to print an
	 * error message before gracefully closing the session.
	 */
	if (cre->ssl != NULL)
		(void)SSL_write(cre->ssl, buf, len);
	else
		(void)write(cre->s, buf, len);
}

void
relay_read(struct bufferevent *bev, void *arg)
{
	struct ctl_relay_event	*cre = (struct ctl_relay_event *)arg;
	struct session		*con = (struct session *)cre->con;
	struct evbuffer		*src = EVBUFFER_INPUT(bev);

	if (gettimeofday(&con->tv_last, NULL))
		goto done;
	if (!EVBUFFER_LENGTH(src))
		return;
	if (relay_bufferevent_write_buffer(cre->dst, src) == -1)
		goto fail;
	if (con->done)
		goto done;
	bufferevent_enable(con->in.bev, EV_READ);
	return;
 done:
	relay_close(con, "last read (done)");
	return;
 fail:
	relay_close(con, strerror(errno));
}

int
relay_resolve(struct ctl_relay_event *cre,
    struct protonode *proot, struct protonode *pn)
{
	struct session		*con = (struct session *)cre->con;
	char			 buf[READ_BUF_SIZE], *ptr;
	int			 id;

	if (pn->mark && (pn->mark != con->mark))
		return (0);

	switch (pn->action) {
	case NODE_ACTION_FILTER:
		id = cre->nodes[proot->id];
		if (SIMPLEQ_NEXT(pn, entry) == NULL)
			cre->nodes[proot->id] = 0;
		if (id <= 1)
			return (0);
		break;
	case NODE_ACTION_EXPECT:
		id = cre->nodes[proot->id];
		if (SIMPLEQ_NEXT(pn, entry) == NULL)
			cre->nodes[proot->id] = 0;
		if (id > 1)
			return (0);
		break;
	default:
		if (cre->nodes[pn->id]) {
			cre->nodes[pn->id] = 0;
			return (0);
		}
		break;
	}
	switch (pn->action) {
	case NODE_ACTION_APPEND:
	case NODE_ACTION_CHANGE:
		ptr = pn->value;
		if ((pn->flags & PNFLAG_MACRO) &&
		    (ptr = relay_expand_http(cre, pn->value,
		    buf, sizeof(buf))) == NULL)
			break;
		if (relay_bufferevent_print(cre->dst, pn->key) == -1 ||
		    relay_bufferevent_print(cre->dst, ": ") == -1 ||
		    relay_bufferevent_print(cre->dst, ptr) == -1 ||
		    relay_bufferevent_print(cre->dst, "\r\n") == -1) {
			relay_close_http(con, 500,
			    "failed to modify header", 0);
			return (-1);
		}
		DPRINTF("relay_resolve: add '%s: %s'",
		    pn->key, ptr);
		break;
	case NODE_ACTION_EXPECT:
		DPRINTF("relay_resolve: missing '%s: %s'",
		    pn->key, pn->value);
		relay_close_http(con, 403, "incomplete request", pn->label);
		return (-1);
	case NODE_ACTION_FILTER:
		DPRINTF("relay_resolve: filtered '%s: %s'",
		    pn->key, pn->value);
		relay_close_http(con, 403, "rejecting request", pn->label);
		return (-1);
	default:
		break;
	}
	return (0);
}

char *
relay_expand_http(struct ctl_relay_event *cre, char *val, char *buf, size_t len)
{
	struct session	*con = (struct session *)cre->con;
	struct relay	*rlay = (struct relay *)con->relay;
	char		 ibuf[128];

	(void)strlcpy(buf, val, len);

	if (strstr(val, "$REMOTE_") != NULL) {
		if (strstr(val, "$REMOTE_ADDR") != NULL) {
			if (print_host(&cre->ss, ibuf, sizeof(ibuf)) == NULL)
				return (NULL);
			if (expand_string(buf, len,
			    "$REMOTE_ADDR", ibuf) != 0)
				return (NULL);
		}
		if (strstr(val, "$REMOTE_PORT") != NULL) {
			snprintf(ibuf, sizeof(ibuf), "%u", ntohs(cre->port));
			if (expand_string(buf, len,
			    "$REMOTE_PORT", ibuf) != 0)
				return (NULL);
		}
	}
	if (strstr(val, "$SERVER_") != NULL) {
		if (strstr(val, "$SERVER_ADDR") != NULL) {
			if (print_host(&rlay->conf.ss,
			    ibuf, sizeof(ibuf)) == NULL)
				return (NULL);
			if (expand_string(buf, len,
			    "$SERVER_ADDR", ibuf) != 0)
				return (NULL);
		}
		if (strstr(val, "$SERVER_PORT") != NULL) {
			snprintf(ibuf, sizeof(ibuf), "%u",
			    ntohs(rlay->conf.port));
			if (expand_string(buf, len,
			    "$SERVER_PORT", ibuf) != 0)
				return (NULL);
		}
	}
	if (strstr(val, "$TIMEOUT") != NULL) {
		snprintf(ibuf, sizeof(ibuf), "%lu", rlay->conf.timeout.tv_sec);
		if (expand_string(buf, len, "$TIMEOUT", ibuf) != 0)
			return (NULL);
	}

	return (buf);
}

int
relay_handle_http(struct ctl_relay_event *cre, struct protonode *proot,
    struct protonode *pn, struct protonode *pk, int header)
{
	struct session		*con = (struct session *)cre->con;
	char			 buf[READ_BUF_SIZE], *ptr;
	int			 ret = PN_DROP, mark = 0;
	struct protonode	*next;

	/* Check if this action depends on a marked session */
	if (pn->mark != 0)
		mark = pn->mark == con->mark ? 1 : -1;

	switch (pn->action) {
	case NODE_ACTION_EXPECT:
	case NODE_ACTION_FILTER:
	case NODE_ACTION_MARK:
		break;
	default:
		if (mark == -1)
			return (PN_PASS);
		break;
	}

	switch (pn->action) {
	case NODE_ACTION_APPEND:
		if (!header)
			return (PN_PASS);
		ptr = pn->value;
		if ((pn->flags & PNFLAG_MACRO) &&
		    (ptr = relay_expand_http(cre, pn->value,
		    buf, sizeof(buf))) == NULL)
			break;
		if (relay_bufferevent_print(cre->dst, pn->key) == -1 ||
		    relay_bufferevent_print(cre->dst, ": ") == -1 ||
		    relay_bufferevent_print(cre->dst, pk->value) == -1 ||
		    relay_bufferevent_print(cre->dst, ", ") == -1 ||
		    relay_bufferevent_print(cre->dst, ptr) == -1 ||
		    relay_bufferevent_print(cre->dst, "\r\n") == -1)
			goto fail;
		cre->nodes[pn->id] = 1;
		DPRINTF("relay_handle_http: append '%s: %s, %s'",
		    pk->key, pk->value, ptr);
		break;
	case NODE_ACTION_CHANGE:
	case NODE_ACTION_REMOVE:
		if (!header)
			return (PN_PASS);
		DPRINTF("relay_handle_http: change/remove '%s: %s'",
		    pk->key, pk->value);
		break;
	case NODE_ACTION_EXPECT:
		/*
		 * A client may specify the header line for multiple times
		 * trying to circumvent the filter.
		 */
		if (cre->nodes[proot->id] > 1) {
			relay_close_http(con, 400, "repeated header line", 0);
			return (PN_FAIL);
		}
		/* FALLTHROUGH */
	case NODE_ACTION_FILTER:
		DPRINTF("relay_handle_http: %s '%s: %s'",
		    (pn->action == NODE_ACTION_EXPECT) ? "expect" : "filter",
		    pn->key, pn->value);

		/* Do not drop the entity */
		ret = PN_PASS;

		if (mark != -1 &&
		    fnmatch(pn->value, pk->value, FNM_CASEFOLD) == 0) {
			cre->nodes[proot->id] = 1;

			/* Fail instantly */
			if (pn->action == NODE_ACTION_FILTER) {
				relay_close_http(con, 403,
				    "rejecting request", pn->label);
				return (PN_FAIL);
			}
		}
		next = SIMPLEQ_NEXT(pn, entry);
		if (next == NULL || next->action != pn->action)
			cre->nodes[proot->id]++;
		break;
	case NODE_ACTION_HASH:
		DPRINTF("relay_handle_http: hash '%s: %s'",
		    pn->key, pk->value);
		con->outkey = hash32_str(pk->value, con->outkey);
		ret = PN_PASS;
		break;
	case NODE_ACTION_LOG:
		DPRINTF("relay_handle_http: log '%s: %s'",
		    pn->key, pk->value);
		ret = PN_PASS;
		break;
	case NODE_ACTION_MARK:
		DPRINTF("relay_handle_http: mark '%s: %s'",
		    pn->key, pk->value);
		if (fnmatch(pn->value, pk->value, FNM_CASEFOLD) == 0)
			con->mark = pn->mark;
		ret = PN_PASS;
		break;
	case NODE_ACTION_NONE:
		return (PN_PASS);
	}
	if (mark != -1 && pn->flags & PNFLAG_LOG) {
		bzero(buf, sizeof(buf));
		if (snprintf(buf, sizeof(buf), " [%s: %s]",
		    pk->key, pk->value) == -1 ||
		    evbuffer_add(con->log, buf, strlen(buf)) == -1)
			goto fail;
	}

	return (ret);
 fail:
	relay_close_http(con, 500, strerror(errno), 0);
	return (PN_FAIL);
}

void
relay_read_httpcontent(struct bufferevent *bev, void *arg)
{
	struct ctl_relay_event	*cre = (struct ctl_relay_event *)arg;
	struct session		*con = (struct session *)cre->con;
	struct evbuffer		*src = EVBUFFER_INPUT(bev);
	size_t			 size;

	if (gettimeofday(&con->tv_last, NULL))
		goto done;
	size = EVBUFFER_LENGTH(src);
	DPRINTF("relay_read_httpcontent: size %d, to read %d",
	    size, cre->toread);
	if (!size)
		return;
	if (relay_bufferevent_write_buffer(cre->dst, src) == -1)
		goto fail;
	if (size >= cre->toread)
		bev->readcb = relay_read_http;
	cre->toread -= size;
	DPRINTF("relay_read_httpcontent: done, size %d, to read %d",
	    size, cre->toread);
	if (con->done)
		goto done;
	if (EVBUFFER_LENGTH(src) && bev->readcb != relay_read_httpcontent)
		bev->readcb(bev, arg);
	bufferevent_enable(bev, EV_READ);
	return;
 done:
	relay_close(con, "last http content read");
	return;
 fail:
	relay_close(con, strerror(errno));
}

void
relay_read_httpchunks(struct bufferevent *bev, void *arg)
{
	struct ctl_relay_event	*cre = (struct ctl_relay_event *)arg;
	struct session		*con = (struct session *)cre->con;
	struct evbuffer		*src = EVBUFFER_INPUT(bev);
	char			*line, *ep;
	long			 lval;
	size_t			 size;

	if (gettimeofday(&con->tv_last, NULL))
		goto done;
	size = EVBUFFER_LENGTH(src);
	DPRINTF("relay_read_httpchunks: size %d, to read %d",
	    size, cre->toread);
	if (!size)
		return;

	if (!cre->toread) {
		line = evbuffer_readline(src);
		if (line == NULL) {
			relay_close(con, "invalid chunk");
			return;
		}

		/* Read prepended chunk size in hex */
		errno = 0;
		lval = strtol(line, &ep, 16);
		if (line[0] == '\0' || *ep != '\0') {
			free(line);
			relay_close(con, "invalid chunk size");
			return;
		}
		if (errno == ERANGE &&
		    (lval == LONG_MAX || lval == LONG_MIN)) {
			free(line);
			relay_close(con, "chunk size out of range");
			return;
		}
		if (relay_bufferevent_print(cre->dst, line) == -1 ||
		    relay_bufferevent_print(cre->dst, "\r\n") == -1)
			goto fail;
		free(line);

		/* Last chunk is 0 bytes followed by an empty newline */
		if ((cre->toread = lval) == 0) {
			line = evbuffer_readline(src);
			if (line == NULL) {
				relay_close(con, "invalid chunk");
				return;
			}
			free(line);
			if (relay_bufferevent_print(cre->dst, "\r\n") == -1)
				goto fail;

			/* Switch to HTTP header mode */
			bev->readcb = relay_read_http;
		}
	} else {
		/* Read chunk data */
		if (size > cre->toread)
			size = cre->toread;
		if (relay_bufferevent_write_chunk(cre->dst, src, size) == -1)
			goto fail;
		cre->toread -= size;
		DPRINTF("relay_read_httpchunks: done, size %d, to read %d",
		    size, cre->toread);

		if (cre->toread == 0) {
			/* Chunk is terminated by an empty newline */
			line = evbuffer_readline(src);
			if (line == NULL || strlen(line)) {
				if (line != NULL)
					free(line);
				relay_close(con, "invalid chunk");
				return;
			}
			free(line);
			if (relay_bufferevent_print(cre->dst, "\r\n\r\n") == -1)
				goto fail;
		}
	}

	if (con->done)
		goto done;
	if (EVBUFFER_LENGTH(src) && bev->readcb != relay_read_httpchunks)
		bev->readcb(bev, arg);
	bufferevent_enable(bev, EV_READ);
	return;

 done:
	relay_close(con, "last http chunk read (done)");
	return;
 fail:
	relay_close(con, strerror(errno));
}

void
relay_read_http(struct bufferevent *bev, void *arg)
{
	struct ctl_relay_event	*cre = (struct ctl_relay_event *)arg;
	struct session		*con = (struct session *)cre->con;
	struct relay		*rlay = (struct relay *)con->relay;
	struct protocol		*proto = rlay->proto;
	struct evbuffer		*src = EVBUFFER_INPUT(bev);
	struct protonode	*pn, pk, *proot, *pnv = NULL, pkv;
	char			*line;
	int			 header = 0, ret, pass = 0;
	const char		*errstr;
	size_t			 size;

	if (gettimeofday(&con->tv_last, NULL))
		goto done;
	size = EVBUFFER_LENGTH(src);
	DPRINTF("relay_read_http: size %d, to read %d", size, cre->toread);
	if (!size)
		return;

	pk.type = NODE_TYPE_HEADER;

	while (!cre->done && (line = evbuffer_readline(src)) != NULL) {
		/*
		 * An empty line indicates the end of the request.
		 * libevent already stripped the \r\n for us.
		 */
		if (!strlen(line)) {
			cre->done = 1;
			free(line);
			break;
		}
		pk.key = line;

		/*
		 * The first line is the GET/POST/PUT/... request,
		 * subsequent lines are HTTP headers.
		 */
		if (++cre->line == 1) {
			pk.value = strchr(pk.key, ' ');
		} else
			pk.value = strchr(pk.key, ':');
		if (pk.value == NULL || strlen(pk.value) < 3) {
			if (cre->line == 1) {
				free(line);
				relay_close_http(con, 400, "malformed", 0);
				return;
			}

			DPRINTF("relay_read_http: request '%s'", line);
			/* Append line to the output buffer */
			if (relay_bufferevent_print(cre->dst, line) == -1 ||
			    relay_bufferevent_print(cre->dst, "\r\n") == -1) {
				free(line);
				goto fail;
			}
			free(line);
			continue;
		}
		if (*pk.value == ':') {
			*pk.value++ = '\0';
			pk.value++;
			header = 1;
		} else {
			*pk.value++ = '\0';
			header = 0;
		}

		DPRINTF("relay_read_http: header '%s: %s'", pk.key, pk.value);

		/*
		 * Identify and handle specific HTTP request methods
		 */
		if (cre->line == 1) {
			if (cre->dir == RELAY_DIR_RESPONSE) {
				cre->method = HTTP_METHOD_RESPONSE;
				goto lookup;
			} else if (strcmp("GET", pk.key) == 0)
				cre->method = HTTP_METHOD_GET;
			else if (strcmp("HEAD", pk.key) == 0)
				cre->method = HTTP_METHOD_HEAD;
			else if (strcmp("POST", pk.key) == 0)
				cre->method = HTTP_METHOD_POST;
			else if (strcmp("PUT", pk.key) == 0)
				cre->method = HTTP_METHOD_PUT;
			else if (strcmp("DELETE", pk.key) == 0)
				cre->method = HTTP_METHOD_DELETE;
			else if (strcmp("OPTIONS", pk.key) == 0)
				cre->method = HTTP_METHOD_OPTIONS;
			else if (strcmp("TRACE", pk.key) == 0)
				cre->method = HTTP_METHOD_TRACE;
			else if (strcmp("CONNECT", pk.key) == 0)
				cre->method = HTTP_METHOD_CONNECT;

			/*
			 * Decode the path and query
			 */
			cre->path = strdup(pk.value);
			if (cre->path == NULL) {
				free(line);
				goto fail;
			}
			cre->version = strchr(cre->path, ' ');
			if (cre->version != NULL)
				*cre->version++ = '\0';
			cre->args = strchr(cre->path, '?');
			if (cre->args != NULL)
				*cre->args++ = '\0';
#ifdef DEBUG
			char	 buf[BUFSIZ];
			if (snprintf(buf, sizeof(buf), " \"%s\"",
			    cre->path) == -1 ||
			    evbuffer_add(con->log, buf, strlen(buf)) == -1) {
				free(line);
				goto fail;
			}
#endif

			/*
			 * Lookup protocol handlers in the URL path
			 */
			if ((proto->flags & F_LOOKUP_PATH) == 0)
				goto lookup;

			pkv.key = cre->path;
			pkv.type = NODE_TYPE_PATH;
			pkv.value = cre->args == NULL ? "" : cre->args;

			DPRINTF("relay_read_http: "
			    "lookup path '%s: %s'", pkv.key, pkv.value);

			if ((proot = RB_FIND(proto_tree,
			    cre->tree, &pkv)) == NULL)
				goto lookup;

			PROTONODE_FOREACH(pnv, proot, entry) {
				ret = relay_handle_http(cre, proot,
				    pnv, &pkv, 0);
				if (ret == PN_FAIL)
					goto abort;
			}
		} else if ((cre->method == HTTP_METHOD_POST ||
		    cre->method == HTTP_METHOD_PUT ||
		    cre->method == HTTP_METHOD_RESPONSE) &&
		    strcasecmp("Content-Length", pk.key) == 0) {
			/*
			 * Need to read data from the client after the
			 * HTTP header.
			 * XXX What about non-standard clients not using
			 * the carriage return? And some browsers seem to
			 * include the line length in the content-length.
			 */
			cre->toread = strtonum(pk.value, 1, INT_MAX, &errstr);
			if (errstr) {
				relay_close_http(con, 500, errstr, 0);
				goto abort;
			}
		}
 lookup:
		if (strcasecmp("Transfer-Encoding", pk.key) == 0 &&
		    strcasecmp("chunked", pk.value) == 0)
			cre->chunked = 1;

		/* Match the HTTP header */
		if ((pn = RB_FIND(proto_tree, cre->tree, &pk)) == NULL)
			goto next;

		if (cre->dir == RELAY_DIR_RESPONSE)
			goto handle;

		if (pn->flags & PNFLAG_LOOKUP_URL) {
			/*
			 * Lookup the URL of type example.com/path?args.
			 * Either as a plain string or SHA1/MD5 digest.
			 */
			if ((pn->flags & PNFLAG_LOOKUP_DIGEST(0)) &&
			    relay_lookup_url(cre, pk.value,
			    DIGEST_NONE) == PN_FAIL)
				goto abort;
			if ((pn->flags & PNFLAG_LOOKUP_DIGEST(DIGEST_SHA1)) &&
			    relay_lookup_url(cre, pk.value,
			    DIGEST_SHA1) == PN_FAIL)
				goto abort;
			if ((pn->flags & PNFLAG_LOOKUP_DIGEST(DIGEST_MD5)) &&
			    relay_lookup_url(cre, pk.value,
			    DIGEST_MD5) == PN_FAIL)
				goto abort;
		} else if (pn->flags & PNFLAG_LOOKUP_QUERY) {
			/* Lookup the HTTP query arguments */
			if (relay_lookup_query(cre) == PN_FAIL)
				goto abort;
		} else if (pn->flags & PNFLAG_LOOKUP_COOKIE) {
			/* Lookup the HTTP cookie */
			if (relay_lookup_cookie(cre, pk.value) == PN_FAIL)
				goto abort;
		}

 handle:
		pass = 0;
		PROTONODE_FOREACH(pnv, pn, entry) {
			ret = relay_handle_http(cre, pn, pnv, &pk, header);
			if (ret == PN_PASS)
				pass = 1;
			else if (ret == PN_FAIL)
				goto abort;
		}

		if (pass) {
 next:
			if (relay_bufferevent_print(cre->dst, pk.key) == -1 ||
			    relay_bufferevent_print(cre->dst,
			    header ? ": " : " ") == -1 ||
			    relay_bufferevent_print(cre->dst, pk.value) == -1 ||
			    relay_bufferevent_print(cre->dst, "\r\n") == -1) {
				free(line);
				goto fail;
			}
		}
		free(line);
	}
	if (cre->done) {
		RB_FOREACH(proot, proto_tree, cre->tree) {
			PROTONODE_FOREACH(pn, proot, entry)
				if (relay_resolve(cre, proot, pn) != 0)
					return;
		}

		switch (cre->method) {
		case HTTP_METHOD_CONNECT:
			/* Data stream */
			bev->readcb = relay_read;
			break;
		case HTTP_METHOD_POST:
		case HTTP_METHOD_PUT:
		case HTTP_METHOD_RESPONSE:
			/* HTTP request payload */
			if (cre->toread) {
				bev->readcb = relay_read_httpcontent;
				break;
			}
			/* FALLTHROUGH */
		default:
			/* HTTP handler */
			bev->readcb = relay_read_http;
			break;
		}
		if (cre->chunked) {
			/* Chunked transfer encoding */
			cre->toread = 0;
			bev->readcb = relay_read_httpchunks;
		}

		/* Write empty newline and switch to relay mode */
		if (relay_bufferevent_print(cre->dst, "\r\n") == -1)
			goto fail;
		cre->line = 0;
		cre->method = 0;
		cre->done = 0;
		cre->chunked = 0;

		if (cre->dir == RELAY_DIR_REQUEST &&
		    proto->lateconnect && cre->dst->bev == NULL &&
		    relay_connect(con) == -1) {
			relay_close_http(con, 502, "session failed", 0);
			return;
		}
	}
	if (con->done)
		goto done;
	if (EVBUFFER_LENGTH(src) && bev->readcb != relay_read_http)
		bev->readcb(bev, arg);
	bufferevent_enable(bev, EV_READ);
	return;
 done:
	relay_close(con, "last http read (done)");
	return;
 fail:
	relay_close_http(con, 500, strerror(errno), 0);
	return;
 abort:
	free(line);
}

static int
_relay_lookup_url(struct ctl_relay_event *cre, char *host, char *path,
    char *query, enum digest_type type)
{
	struct session		*con = (struct session *)cre->con;
	struct protonode	*proot, *pnv, pkv;
	char			*val, *md = NULL;
	int			 ret = PN_FAIL;

	if (asprintf(&val, "%s%s%s%s",
	    host, path,
	    query == NULL ? "" : "?",
	    query == NULL ? "" : query) == -1) {
		relay_close_http(con, 500, "failed to allocate URL", 0);
		return (PN_FAIL);
	}

	DPRINTF("_relay_lookup_url: %s", val);

	switch (type) {
	case DIGEST_SHA1:
	case DIGEST_MD5:
		if ((md = digeststr(type, val, strlen(val), NULL)) == NULL) {
			relay_close_http(con, 500,
			    "failed to allocate digest", 0);
			goto fail;
		}
		pkv.key = md;
		break;
	case DIGEST_NONE:
		pkv.key = val;
		break;
	}
	pkv.type = NODE_TYPE_URL;
	pkv.value = "";

	if ((proot = RB_FIND(proto_tree, cre->tree, &pkv)) == NULL)
		goto done;

	PROTONODE_FOREACH(pnv, proot, entry) {
		ret = relay_handle_http(cre, proot, pnv, &pkv, 0);
		if (ret == PN_FAIL)
			goto fail;
	}

 done:
	ret = PN_PASS;
 fail:
	if (md != NULL)
		free(md);
	free(val);
	return (ret);
}

int
relay_lookup_url(struct ctl_relay_event *cre, const char *str,
    enum digest_type type)
{
	struct session	*con = (struct session *)cre->con;
	int		 i, j, dots;
	char		*hi[RELAY_MAXLOOKUPLEVELS], *p, *pp, *c, ch;
	char		 ph[MAXHOSTNAMELEN];
	int		 ret;

	if (cre->path == NULL)
		return (PN_PASS);

	/*
	 * This is an URL lookup algorithm inspired by
	 * http://code.google.com/apis/safebrowsing/
	 *     developers_guide.html#PerformingLookups
	 */

	DPRINTF("relay_lookup_url: host: '%s', path: '%s', query: '%s'",
	    str, cre->path, cre->args == NULL ? "" : cre->args);

	if (canonicalize_host(str, ph, sizeof(ph)) == NULL) {
		relay_close_http(con, 400, "invalid host name", 0);
		return (PN_FAIL);
	}

	bzero(hi, sizeof(hi));
	for (dots = -1, i = strlen(ph) - 1; i > 0; i--) {
		if (ph[i] == '.' && ++dots)
			hi[dots - 1] = &ph[i + 1];
		if (dots > (RELAY_MAXLOOKUPLEVELS - 2))
			break;
	}
	if (dots == -1)
		dots = 0;
	hi[dots] = ph;

	if ((pp = strdup(cre->path)) == NULL) {
		relay_close_http(con, 500, "failed to allocate path", 0);
		return (PN_FAIL);
	}
	for (i = (RELAY_MAXLOOKUPLEVELS - 1); i >= 0; i--) {
		if (hi[i] == NULL)
			continue;

		/* 1. complete path with query */
		if (cre->args != NULL)
			if ((ret = _relay_lookup_url(cre, hi[i],
			    pp, cre->args, type)) != PN_PASS)
				goto done;

		/* 2. complete path without query */
		if ((ret = _relay_lookup_url(cre, hi[i],
		    pp, NULL, type)) != PN_PASS)
			goto done;

		/* 3. traverse path */
		for (j = 0, p = strchr(pp, '/');
		    p != NULL; p = strchr(p, '/'), j++) {
			if (j > (RELAY_MAXLOOKUPLEVELS - 2) || ++p == '\0')
				break;
			c = &pp[p - pp];
			ch = *c;
			*c = '\0';
			if ((ret = _relay_lookup_url(cre, hi[i],
			    pp, NULL, type)) != PN_PASS)
				goto done;
			*c = ch;
		}
	}

	ret = PN_PASS;
 done:
	free(pp);
	return (ret);
}

int
relay_lookup_query(struct ctl_relay_event *cre)
{
	struct session		*con = (struct session *)cre->con;
	struct protonode	*proot, *pnv, pkv;
	char			*val, *ptr;
	int			 ret;

	if (cre->path == NULL || cre->args == NULL || strlen(cre->args) < 2)
		return (PN_PASS);
	if ((val = strdup(cre->args)) == NULL) {
		relay_close_http(con, 500, "failed to allocate query", 0);
		return (PN_FAIL);
	}

	ptr = val;
	while (ptr != NULL && strlen(ptr)) {
		pkv.key = ptr;
		pkv.type = NODE_TYPE_QUERY;
		if ((ptr = strchr(ptr, '&')) != NULL)
			*ptr++ = '\0';
		if ((pkv.value =
		    strchr(pkv.key, '=')) == NULL ||
		    strlen(pkv.value) < 1)
			continue;
		*pkv.value++ = '\0';

		if ((proot = RB_FIND(proto_tree, cre->tree, &pkv)) == NULL)
			continue;
		PROTONODE_FOREACH(pnv, proot, entry) {
			ret = relay_handle_http(cre, proot,
			    pnv, &pkv, 0);
			if (ret == PN_FAIL)
				goto done;
		}
	}

	ret = PN_PASS;
 done:
	free(val);
	return (ret);
}

int
relay_lookup_cookie(struct ctl_relay_event *cre, const char *str)
{
	struct session		*con = (struct session *)cre->con;
	struct protonode	*proot, *pnv, pkv;
	char			*val, *ptr;
	int			 ret;

	if ((val = strdup(str)) == NULL) {
		relay_close_http(con, 500, "failed to allocate cookie", 0);
		return (PN_FAIL);
	}

	for (ptr = val; ptr != NULL && strlen(ptr);) {
		if (*ptr == ' ')
			*ptr++ = '\0';
		pkv.key = ptr;
		pkv.type = NODE_TYPE_COOKIE;
		if ((ptr = strchr(ptr, ';')) != NULL)
			*ptr++ = '\0';
		/*
		 * XXX We do not handle attributes
		 * ($Path, $Domain, or $Port)
		 */
		if (*pkv.key == '$')
			continue;

		if ((pkv.value =
		    strchr(pkv.key, '=')) == NULL ||
		    strlen(pkv.value) < 1)
			continue;
		*pkv.value++ = '\0';
		if (*pkv.value == '"')
			*pkv.value++ = '\0';
		if (pkv.value[strlen(pkv.value) - 1] == '"')
			pkv.value[strlen(pkv.value) - 1] = '\0';
		if ((proot = RB_FIND(proto_tree, cre->tree, &pkv)) == NULL)
			continue;
		PROTONODE_FOREACH(pnv, proot, entry) {
			ret = relay_handle_http(cre, proot, pnv, &pkv, 0);
			if (ret == PN_FAIL)
				goto done;
		}
	}

	ret = PN_PASS;
 done:
	free(val);
	return (ret);
}

void
relay_close_http(struct session *con, u_int code, const char *msg,
    u_int16_t labelid)
{
	struct relay		*rlay = (struct relay *)con->relay;
	struct bufferevent	*bev = con->in.bev;
	const char		*httperr = print_httperror(code), *text = "";
	char			*httpmsg;
	time_t			 t;
	struct tm		*lt;
	char			 tmbuf[32], hbuf[128];
	const char		*style, *label = NULL;

	/* In some cases this function may be called from generic places */
	if (rlay->proto->type != RELAY_PROTO_HTTP ||
	    (rlay->proto->flags & F_RETURN) == 0) {
		relay_close(con, msg);
		return;
	}

	if (bev == NULL)
		goto done;

	/* Some system information */
	if (print_host(&rlay->conf.ss, hbuf, sizeof(hbuf)) == NULL)
		goto done;

	/* RFC 2616 "tolerates" asctime() */
	time(&t);
	lt = localtime(&t);
	tmbuf[0] = '\0';
	if (asctime_r(lt, tmbuf) != NULL)
		tmbuf[strlen(tmbuf) - 1] = '\0';	/* skip final '\n' */

	/* Do not send details of the Internal Server Error */
	if (code != 500)
		text = msg;
	if (labelid != 0)
		label = pn_id2name(labelid);

	/* A CSS stylesheet allows minimal customization by the user */
	if ((style = rlay->proto->style) == NULL)
		style = "body { background-color: #a00000; color: white; }";

	/* Generate simple HTTP+HTML error document */
	if (asprintf(&httpmsg,
	    "HTTP/1.x %03d %s\r\n"
	    "Date: %s\r\n"
	    "Server: %s\r\n"
	    "Connection: close\r\n"
	    "Content-Type: text/html\r\n"
	    "\r\n"
	    "<!DOCTYPE HTML PUBLIC "
	    "\"-//W3C//DTD HTML 4.01 Transitional//EN\">\n"
	    "<html>\n"
	    "<head>\n"
	    "<title>%03d %s</title>\n"
	    "<style type=\"text/css\"><!--\n%s\n--></style>\n"
	    "</head>\n"
	    "<body>\n"
	    "<h1>%s</h1>\n"
	    "<div id='m'>%s</div>\n"
	    "<div id='l'>%s</div>\n"
	    "<hr><address>%s at %s port %d</address>\n"
	    "</body>\n"
	    "</html>\n",
	    code, httperr, tmbuf, HOSTSTATED_SERVERNAME,
	    code, httperr, style, httperr, text,
	    label == NULL ? "" : label,
	    HOSTSTATED_SERVERNAME, hbuf, ntohs(rlay->conf.port)) == -1)
		goto done;

	/* Dump the message without checking for success */
	relay_dump(&con->in, httpmsg, strlen(httpmsg));
	free(httpmsg);

 done:
	if (asprintf(&httpmsg, "%s (%03d %s)", msg, code, httperr) == -1)
		relay_close(con, msg);
	else {
		relay_close(con, httpmsg);
		free(httpmsg);
	}
}

void
relay_error(struct bufferevent *bev, short error, void *arg)
{
	struct ctl_relay_event *cre = (struct ctl_relay_event *)arg;
	struct session *con = (struct session *)cre->con;
	struct evbuffer *dst;

	if (error & EVBUFFER_TIMEOUT) {
		relay_close(con, "buffer event timeout");
		return;
	}
	if (error & (EVBUFFER_READ|EVBUFFER_WRITE|EVBUFFER_EOF)) {
		bufferevent_disable(bev, EV_READ|EV_WRITE);

		con->done = 1;
		if (cre->dst->bev != NULL) {
			dst = EVBUFFER_OUTPUT(cre->dst->bev);
			if (EVBUFFER_LENGTH(dst))
				return;
		}

		relay_close(con, "done");
		return;
	}
	relay_close(con, "buffer event error");
}

void
relay_accept(int fd, short sig, void *arg)
{
	struct relay *rlay = (struct relay *)arg;
	struct protocol *proto = rlay->proto;
	struct session *con = NULL;
	struct ctl_natlook *cnl = NULL;
	socklen_t slen;
	struct timeval tv;
	struct sockaddr_storage ss;
	int s = -1;

	slen = sizeof(ss);
	if ((s = accept(fd, (struct sockaddr *)&ss, (socklen_t *)&slen)) == -1)
		return;

	if (relay_sessions >= RELAY_MAX_SESSIONS ||
	    rlay->conf.flags & F_DISABLE)
		goto err;

	if ((con = (struct session *)
	    calloc(1, sizeof(struct session))) == NULL)
		goto err;

	con->in.s = s;
	con->in.ssl = NULL;
	con->out.s = -1;
	con->out.ssl = NULL;
	con->in.dst = &con->out;
	con->out.dst = &con->in;
	con->in.con = con;
	con->out.con = con;
	con->relay = rlay;
	con->id = ++relay_conid;
	con->relayid = rlay->conf.id;
	con->outkey = rlay->dstkey;
	con->in.tree = &proto->request_tree;
	con->out.tree = &proto->response_tree;
	con->in.dir = RELAY_DIR_REQUEST;
	con->out.dir = RELAY_DIR_RESPONSE;
	con->retry = rlay->conf.dstretry;
	if (gettimeofday(&con->tv_start, NULL))
		goto err;
	bcopy(&con->tv_start, &con->tv_last, sizeof(con->tv_last));
	bcopy(&ss, &con->in.ss, sizeof(con->in.ss));
	con->out.port = rlay->conf.dstport;
	switch (ss.ss_family) {
	case AF_INET:
		con->in.port = ((struct sockaddr_in *)&ss)->sin_port;
		break;
	case AF_INET6:
		con->in.port = ((struct sockaddr_in6 *)&ss)->sin6_port;
		break;
	}

	relay_sessions++;
	SPLAY_INSERT(session_tree, &rlay->sessions, con);

	/* Increment the per-relay session counter */
	rlay->stats[proc_id].last++;

	/* Pre-allocate output buffer */
	con->out.output = evbuffer_new();
	if (con->out.output == NULL) {
		relay_close(con, "failed to allocate output buffer");
		return;
	}

	/* Pre-allocate log buffer */
	con->log = evbuffer_new();
	if (con->log == NULL) {
		relay_close(con, "failed to allocate log buffer");
		return;
	}

	if (rlay->conf.flags & F_NATLOOK) {
		if ((cnl = (struct ctl_natlook *)
		    calloc(1, sizeof(struct ctl_natlook))) == NULL) {
			relay_close(con, "failed to allocate nat lookup");
			return;
		}
	}

	if (rlay->conf.flags & F_NATLOOK && cnl != NULL) {
		con->cnl = cnl;;
		bzero(cnl, sizeof(*cnl));
		cnl->in = -1;
		cnl->id = con->id;
		cnl->proc = proc_id;
		bcopy(&con->in.ss, &cnl->src, sizeof(cnl->src));
		bcopy(&rlay->conf.ss, &cnl->dst, sizeof(cnl->dst));
		imsg_compose(ibuf_pfe, IMSG_NATLOOK, 0, 0, -1, cnl,
		    sizeof(*cnl));

		/* Schedule timeout */
		evtimer_set(&con->ev, relay_natlook, con);
		bcopy(&rlay->conf.timeout, &tv, sizeof(tv));
		evtimer_add(&con->ev, &tv);
		return;
	}

	relay_session(con);
	return;
 err:
	if (s != -1) {
		close(s);
		if (con != NULL)
			free(con);
	}
}

u_int32_t
relay_hash_addr(struct sockaddr_storage *ss, u_int32_t p)
{
	struct sockaddr_in	*sin4;
	struct sockaddr_in6	*sin6;

	if (ss->ss_family == AF_INET) {
		sin4 = (struct sockaddr_in *)ss;
		p = hash32_buf(&sin4->sin_addr,
		    sizeof(struct in_addr), p);
	} else {
		sin6 = (struct sockaddr_in6 *)ss;
		p = hash32_buf(&sin6->sin6_addr,
		    sizeof(struct in6_addr), p);
	}

	return (p);
}

int
relay_from_table(struct session *con)
{
	struct relay		*rlay = (struct relay *)con->relay;
	struct host		*host;
	struct table		*table = rlay->dsttable;
	u_int32_t		 p = con->outkey;
	int			 idx = 0;

	if (rlay->conf.dstcheck && !table->up) {
		log_debug("relay_from_table: no active hosts");
		return (-1);
	}

	switch (rlay->conf.dstmode) {
	case RELAY_DSTMODE_ROUNDROBIN:
		if ((int)rlay->dstkey >= rlay->dstnhosts)
			rlay->dstkey = 0;
		idx = (int)rlay->dstkey;
		break;
	case RELAY_DSTMODE_LOADBALANCE:
		p = relay_hash_addr(&con->in.ss, p);
		/* FALLTHROUGH */
	case RELAY_DSTMODE_HASH:
		p = relay_hash_addr(&rlay->conf.ss, p);
		p = hash32_buf(&rlay->conf.port, sizeof(rlay->conf.port), p);
		if ((idx = p % rlay->dstnhosts) >= RELAY_MAXHOSTS)
			return (-1);
	}
	host = rlay->dsthost[idx];
	DPRINTF("relay_from_table: host %s, p 0x%08x, idx %d",
	    host->conf.name, p, idx);
	while (host != NULL) {
		DPRINTF("relay_from_table: host %s", host->conf.name);
		if (!rlay->conf.dstcheck || host->up == HOST_UP)
			goto found;
		host = TAILQ_NEXT(host, entry);
	}
	TAILQ_FOREACH(host, &rlay->dsttable->hosts, entry) {
		DPRINTF("relay_from_table: next host %s", host->conf.name);
		if (!rlay->conf.dstcheck || host->up == HOST_UP)
			goto found;
	}

	/* Should not happen */
	fatalx("relay_from_table: no active hosts, desynchronized");

 found:
	if (rlay->conf.dstmode == RELAY_DSTMODE_ROUNDROBIN)
		rlay->dstkey = host->idx + 1;
	con->retry = host->conf.retry;
	con->out.port = table->conf.port;
	bcopy(&host->conf.ss, &con->out.ss, sizeof(con->out.ss));

	return (0);
}

void
relay_natlook(int fd, short event, void *arg)
{
	struct session		*con = (struct session *)arg;
	struct relay		*rlay = (struct relay *)con->relay;
	struct ctl_natlook	*cnl = con->cnl;

	if (cnl == NULL)
		fatalx("invalid NAT lookup");

	if (con->out.ss.ss_family == AF_UNSPEC && cnl->in == -1 &&
	    rlay->conf.dstss.ss_family == AF_UNSPEC && rlay->dsttable == NULL) {
		relay_close(con, "session NAT lookup failed");
		return;
	}
	if (cnl->in != -1) {
		bcopy(&cnl->rdst, &con->out.ss, sizeof(con->out.ss));
		con->out.port = cnl->rdport;
	}
	free(con->cnl);
	con->cnl = NULL;

	relay_session(con);
}

void
relay_session(struct session *con)
{
	struct relay		*rlay = (struct relay *)con->relay;
	struct ctl_relay_event	*in = &con->in, *out = &con->out;

	if (bcmp(&rlay->conf.ss, &out->ss, sizeof(out->ss)) == 0 &&
	    out->port == rlay->conf.port) {
		log_debug("relay_session: session %d: looping",
		    con->id);
		relay_close(con, "session aborted");
		return;
	}

	if (rlay->conf.flags & F_UDP) {
		/*
		 * Call the UDP protocol-specific handler
		 */
		if (rlay->proto->request == NULL)
			fatalx("invalide UDP session");
		if ((*rlay->proto->request)(con) == -1)
			relay_close(con, "session failed");
		return;
	}

	if ((rlay->conf.flags & F_SSL) && (in->ssl == NULL)) {
		relay_ssl_transaction(con);
		return;
	}

	if (!rlay->proto->lateconnect && relay_connect(con) == -1) {
		relay_close(con, "session failed");
		return;
	}

	relay_input(con);
}

int
relay_connect(struct session *con)
{
	struct relay	*rlay = (struct relay *)con->relay;

	if (gettimeofday(&con->tv_start, NULL))
		return (-1);

	if (rlay->dsttable != NULL) {
		if (relay_from_table(con) != 0)
			return (-1);
	} else if (con->out.ss.ss_family == AF_UNSPEC) {
		bcopy(&rlay->conf.dstss, &con->out.ss, sizeof(con->out.ss));
		con->out.port = rlay->conf.dstport;
	}

 retry:
	if ((con->out.s = relay_socket_connect(&con->out.ss, con->out.port,
	    rlay->proto)) == -1) {
		if (con->retry) {
			con->retry--;
			log_debug("relay_connect: session %d: "
			    "forward failed: %s, %s",
			    con->id, strerror(errno),
			    con->retry ? "next retry" : "last retry");
			goto retry;
		}
		log_debug("relay_connect: session %d: forward failed: %s",
		    con->id, strerror(errno));
		return (-1);
	}

	if (errno == EINPROGRESS)
		event_again(&con->ev, con->out.s, EV_WRITE|EV_TIMEOUT,
		    relay_connected, &con->tv_start, &env->timeout, con);
	else
		relay_connected(con->out.s, EV_WRITE, con);

	return (0);
}

void
relay_close(struct session *con, const char *msg)
{
	struct relay	*rlay = (struct relay *)con->relay;
	char		 ibuf[128], obuf[128], *ptr = NULL;

	SPLAY_REMOVE(session_tree, &rlay->sessions, con);

	event_del(&con->ev);
	if (con->in.bev != NULL)
		bufferevent_disable(con->in.bev, EV_READ|EV_WRITE);
	if (con->out.bev != NULL)
		bufferevent_disable(con->out.bev, EV_READ|EV_WRITE);

	if (env->opts & HOSTSTATED_OPT_LOGUPDATE) {
		bzero(&ibuf, sizeof(ibuf));
		bzero(&obuf, sizeof(obuf));
		(void)print_host(&con->in.ss, ibuf, sizeof(ibuf));
		(void)print_host(&con->out.ss, obuf, sizeof(obuf));
		if (EVBUFFER_LENGTH(con->log) &&
		    evbuffer_add_printf(con->log, "\r\n") != -1)
			ptr = evbuffer_readline(con->log);
		log_info("relay %s, session %d (%d active), %d, %s -> %s:%d, "
		    "%s%s%s", rlay->conf.name, con->id, relay_sessions,
		    con->mark, ibuf, obuf, ntohs(con->out.port), msg,
		    ptr == NULL ? "" : ",", ptr == NULL ? "" : ptr);
		if (ptr != NULL)
			free(ptr);
	}

	if (con->in.bev != NULL)
		bufferevent_free(con->in.bev);
	else if (con->in.output != NULL)
		evbuffer_free(con->in.output);
	if (con->in.ssl != NULL) {
		/* XXX handle non-blocking shutdown */
		if (SSL_shutdown(con->in.ssl) == 0)
			SSL_shutdown(con->in.ssl);
		SSL_free(con->in.ssl);
	}
	if (con->in.s != -1)
		close(con->in.s);
	if (con->in.path != NULL)
		free(con->in.path);
	if (con->in.buf != NULL)
		free(con->in.buf);
	if (con->in.nodes != NULL)
		free(con->in.nodes);

	if (con->out.bev != NULL)
		bufferevent_free(con->out.bev);
	else if (con->out.output != NULL)
		evbuffer_free(con->out.output);
	if (con->out.s != -1)
		close(con->out.s);
	if (con->out.path != NULL)
		free(con->out.path);
	if (con->out.buf != NULL)
		free(con->out.buf);
	if (con->out.nodes != NULL)
		free(con->out.nodes);

	if (con->log != NULL)
		evbuffer_free(con->log);

	if (con->cnl != NULL) {
#if 0
		imsg_compose(ibuf_pfe, IMSG_KILLSTATES, 0, 0, -1,
		    cnl, sizeof(*cnl));
#endif
		free(con->cnl);
	}

	free(con);
	relay_sessions--;
}

void
relay_dispatch_pfe(int fd, short event, void *ptr)
{
	struct imsgbuf		*ibuf;
	struct imsg		 imsg;
	ssize_t			 n;
	struct relay		*rlay;
	struct session		*con;
	struct ctl_natlook	 cnl;
	struct timeval		 tv;
	struct host		*host;
	struct table		*table;
	struct ctl_status	 st;
	objid_t			 id;

	ibuf = ptr;
	switch (event) {
	case EV_READ:
		if ((n = imsg_read(ibuf)) == -1)
			fatal("relay_dispatch_pfe: imsg_read_error");
		if (n == 0) {
			/* this pipe is dead, so remove the event handler */
			event_del(&ibuf->ev);
			event_loopexit(NULL);
			return;
		}
		break;
	case EV_WRITE:
		if (msgbuf_write(&ibuf->w) == -1)
			fatal("relay_dispatch_pfe: msgbuf_write");
		imsg_event_add(ibuf);
		return;
	default:
		fatalx("relay_dispatch_pfe: unknown event");
	}

	for (;;) {
		if ((n = imsg_get(ibuf, &imsg)) == -1)
			fatal("relay_dispatch_pfe: imsg_read error");
		if (n == 0)
			break;

		switch (imsg.hdr.type) {
		case IMSG_HOST_DISABLE:
			memcpy(&id, imsg.data, sizeof(id));
			if ((host = host_find(env, id)) == NULL)
				fatalx("relay_dispatch_pfe: desynchronized");
			if ((table = table_find(env, host->conf.tableid)) ==
			    NULL)
				fatalx("relay_dispatch_pfe: invalid table id");
			if (host->up == HOST_UP)
				table->up--;
			host->flags |= F_DISABLE;
			host->up = HOST_UNKNOWN;
			break;
		case IMSG_HOST_ENABLE:
			memcpy(&id, imsg.data, sizeof(id));
			if ((host = host_find(env, id)) == NULL)
				fatalx("relay_dispatch_pfe: desynchronized");
			host->flags &= ~(F_DISABLE);
			host->up = HOST_UNKNOWN;
			break;
		case IMSG_HOST_STATUS:
			if (imsg.hdr.len - IMSG_HEADER_SIZE != sizeof(st))
				fatalx("relay_dispatch_pfe: invalid request");
			memcpy(&st, imsg.data, sizeof(st));
			if ((host = host_find(env, st.id)) == NULL)
				fatalx("relay_dispatch_pfe: invalid host id");
			if (host->flags & F_DISABLE)
				break;
			if (host->up == st.up) {
				log_debug("relay_dispatch_pfe: host %d => %d",
				    host->conf.id, host->up);
				fatalx("relay_dispatch_pfe: desynchronized");
			}

			if ((table = table_find(env, host->conf.tableid))
			    == NULL)
				fatalx("relay_dispatch_pfe: invalid table id");

			DPRINTF("relay_dispatch_pfe: [%d] state %d for "
			    "host %u %s", proc_id, st.up,
			    host->conf.id, host->conf.name);

			if ((st.up == HOST_UNKNOWN && host->up == HOST_DOWN) ||
			    (st.up == HOST_DOWN && host->up == HOST_UNKNOWN)) {
				host->up = st.up;
				break;
			}
			if (st.up == HOST_UP)
				table->up++;
			else
				table->up--;
			host->up = st.up;
			break;
		case IMSG_NATLOOK:
			bcopy(imsg.data, &cnl, sizeof(cnl));
			if ((con = session_find(env, cnl.id)) == NULL ||
			    con->cnl == NULL) {
				log_debug("relay_dispatch_pfe: "
				    "session expired");
				break;
			}
			bcopy(&cnl, con->cnl, sizeof(*con->cnl));
			evtimer_del(&con->ev);
			evtimer_set(&con->ev, relay_natlook, con);
			bzero(&tv, sizeof(tv));
			evtimer_add(&con->ev, &tv);
			break;
		case IMSG_CTL_SESSION:
			TAILQ_FOREACH(rlay, env->relays, entry)
				SPLAY_FOREACH(con, session_tree,
				    &rlay->sessions)
					imsg_compose(ibuf, IMSG_CTL_SESSION,
					    0, 0, -1, con, sizeof(*con));
			imsg_compose(ibuf, IMSG_CTL_END, 0, 0, -1, NULL, 0);
			break;
		default:
			log_debug("relay_dispatch_msg: unexpected imsg %d",
			    imsg.hdr.type);
			break;
		}
		imsg_free(&imsg);
	}
	imsg_event_add(ibuf);
}

void
relay_dispatch_parent(int fd, short event, void * ptr)
{
	struct imsgbuf	*ibuf;
	struct imsg	 imsg;
	ssize_t		 n;

	ibuf = ptr;
	switch (event) {
	case EV_READ:
		if ((n = imsg_read(ibuf)) == -1)
			fatal("relay_dispatch_parent: imsg_read error");
		if (n == 0) {
			/* this pipe is dead, so remove the event handler */
			event_del(&ibuf->ev);
			event_loopexit(NULL);
			return;
		}
		break;
	case EV_WRITE:
		if (msgbuf_write(&ibuf->w) == -1)
			fatal("relay_dispatch_parent: msgbuf_write");
		imsg_event_add(ibuf);
		return;
	default:
		fatalx("relay_dispatch_parent: unknown event");
	}

	for (;;) {
		if ((n = imsg_get(ibuf, &imsg)) == -1)
			fatal("relay_dispatch_parent: imsg_read error");
		if (n == 0)
			break;

		switch (imsg.hdr.type) {
		default:
			log_debug("relay_dispatch_parent: unexpected imsg %d",
			    imsg.hdr.type);
			break;
		}
		imsg_free(&imsg);
	}
	imsg_event_add(ibuf);
}

SSL_CTX *
relay_ssl_ctx_create(struct relay *rlay)
{
	struct protocol *proto = rlay->proto;
	SSL_CTX *ctx;

	ctx = SSL_CTX_new(SSLv23_method());
	if (ctx == NULL)
		goto err;

	/* Modify session timeout and cache size*/
	SSL_CTX_set_timeout(ctx, rlay->conf.timeout.tv_sec);
	if (proto->cache < -1) {
		SSL_CTX_set_session_cache_mode(ctx, SSL_SESS_CACHE_OFF);
	} else if (proto->cache >= -1) {
		SSL_CTX_set_session_cache_mode(ctx, SSL_SESS_CACHE_SERVER);
		if (proto->cache >= 0)
			SSL_CTX_sess_set_cache_size(ctx, proto->cache);
	}

	/* Enable all workarounds and set SSL options */
	SSL_CTX_set_options(ctx, SSL_OP_ALL);
	SSL_CTX_set_options(ctx,
	    SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION);

	/* Set the allowed SSL protocols */
	if ((proto->sslflags & SSLFLAG_SSLV2) == 0)
		SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2);
	if ((proto->sslflags & SSLFLAG_SSLV3) == 0)
		SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv3);
	if ((proto->sslflags & SSLFLAG_TLSV1) == 0)
		SSL_CTX_set_options(ctx, SSL_OP_NO_TLSv1);

	if (!SSL_CTX_set_cipher_list(ctx, proto->sslciphers))
		goto err;

	log_debug("relay_ssl_ctx_create: loading certificate");
	if (!ssl_ctx_use_certificate_chain(ctx,
	    rlay->ssl_cert, rlay->ssl_cert_len))
		goto err;

	log_debug("relay_ssl_ctx_create: loading private key");
	if (!ssl_ctx_use_private_key(ctx, rlay->ssl_key, rlay->ssl_key_len))
		goto err;
	if (!SSL_CTX_check_private_key(ctx))
		goto err;

	/* Set session context to the local relay name */
	if (!SSL_CTX_set_session_id_context(ctx, rlay->conf.name,
	    strlen(rlay->conf.name)))
		goto err;

	return (ctx);

 err:
	if (ctx != NULL)
		SSL_CTX_free(ctx);
	ssl_error(rlay->conf.name, "relay_ssl_ctx_create");
	return (NULL);
}

void
relay_ssl_transaction(struct session *con)
{
	struct relay	*rlay = (struct relay *)con->relay;
	SSL		*ssl;

	ssl = SSL_new(rlay->ssl_ctx);
	if (ssl == NULL)
		goto err;

	if (!SSL_set_ssl_method(ssl, SSLv23_server_method()))
		goto err;
	if (!SSL_set_fd(ssl, con->in.s))
		goto err;
	SSL_set_accept_state(ssl);

	con->in.ssl = ssl;

	event_again(&con->ev, con->in.s, EV_TIMEOUT|EV_READ,
	    relay_ssl_accept, &con->tv_start, &env->timeout, con);
	return;

 err:
	if (ssl != NULL)
		SSL_free(ssl);
	ssl_error(rlay->conf.name, "relay_ssl_transaction");
}

void
relay_ssl_accept(int fd, short event, void *arg)
{
	struct session	*con = (struct session *)arg;
	struct relay	*rlay = (struct relay *)con->relay;
	int		 ret;
	int		 ssl_err;
	int		 retry_flag;

	if (event == EV_TIMEOUT) {
		relay_close(con, "SSL accept timeout");
		return;
	}

	retry_flag = ssl_err = 0;

	ret = SSL_accept(con->in.ssl);
	if (ret <= 0) {
		ssl_err = SSL_get_error(con->in.ssl, ret);

		switch (ssl_err) {
		case SSL_ERROR_WANT_READ:
			retry_flag = EV_READ;
			goto retry;
		case SSL_ERROR_WANT_WRITE:
			retry_flag = EV_WRITE;
			goto retry;
		case SSL_ERROR_ZERO_RETURN:
		case SSL_ERROR_SYSCALL:
			if (ret == 0) {
				relay_close(con, "closed");
				return;
			}
			/* FALLTHROUGH */
		default:
			ssl_error(rlay->conf.name, "relay_ssl_accept");
			relay_close(con, "SSL accept error");
			return;
		}
	}


#ifdef DEBUG
	log_info("relay %s, session %d established (%d active)",
	    rlay->conf.name, con->id, relay_sessions);
#else
	log_debug("relay %s, session %d established (%d active)",
	    rlay->conf.name, con->id, relay_sessions);
#endif
	relay_session(con);
	return;

retry:
	DPRINTF("relay_ssl_accept: session %d: scheduling on %s", con->id,
	    (retry_flag == EV_READ) ? "EV_READ" : "EV_WRITE");
	event_again(&con->ev, fd, EV_TIMEOUT|retry_flag, relay_ssl_accept,
	    &con->tv_start, &env->timeout, con);
}

void
relay_ssl_connected(struct ctl_relay_event *cre)
{
	/*
	 * Hack libevent - we overwrite the internal bufferevent I/O
	 * functions to handle the SSL abstraction.
	 */
	event_set(&cre->bev->ev_read, cre->s, EV_READ,
	    relay_ssl_readcb, cre->bev);
	event_set(&cre->bev->ev_write, cre->s, EV_WRITE,
	    relay_ssl_writecb, cre->bev);
}

void
relay_ssl_readcb(int fd, short event, void *arg)
{
	struct bufferevent *bufev = arg;
	struct ctl_relay_event *cre = (struct ctl_relay_event *)bufev->cbarg;
	struct session *con = (struct session *)cre->con;
	struct relay *rlay = (struct relay *)con->relay;
	int ret = 0, ssl_err = 0;
	short what = EVBUFFER_READ;
	size_t len;
	char rbuf[READ_BUF_SIZE];
	int howmuch = READ_BUF_SIZE;

	if (event == EV_TIMEOUT) {
		what |= EVBUFFER_TIMEOUT;
		goto err;
	}

	if (bufev->wm_read.high != 0)
		howmuch = MIN(sizeof(rbuf), bufev->wm_read.high);

	ret = SSL_read(cre->ssl, rbuf, howmuch);
	if (ret <= 0) {
		ssl_err = SSL_get_error(cre->ssl, ret);

		switch (ssl_err) {
		case SSL_ERROR_WANT_READ:
			DPRINTF("relay_ssl_readcb: session %d: "
			    "want read", con->id);
			goto retry;
		case SSL_ERROR_WANT_WRITE:
			DPRINTF("relay_ssl_readcb: session %d: "
			    "want write", con->id);
			goto retry;
		default:
			if (ret == 0)
				what |= EVBUFFER_EOF;
			else {
				ssl_error(rlay->conf.name, "relay_ssl_readcb");
				what |= EVBUFFER_ERROR;
			}
			goto err;
		}
	}

	if (evbuffer_add(bufev->input, rbuf, ret) == -1) {
		what |= EVBUFFER_ERROR;
		goto err;
	}

	relay_bufferevent_add(&bufev->ev_read, bufev->timeout_read);

	len = EVBUFFER_LENGTH(bufev->input);
	if (bufev->wm_read.low != 0 && len < bufev->wm_read.low)
		return;
	if (bufev->wm_read.high != 0 && len > bufev->wm_read.high) {
		struct evbuffer *buf = bufev->input;
		event_del(&bufev->ev_read);
		evbuffer_setcb(buf, bufferevent_read_pressure_cb, bufev);
		return;
	}

	if (bufev->readcb != NULL)
		(*bufev->readcb)(bufev, bufev->cbarg);
	return;

 retry:
	relay_bufferevent_add(&bufev->ev_read, bufev->timeout_read);
	return;

 err:
	(*bufev->errorcb)(bufev, what, bufev->cbarg);
}

void
relay_ssl_writecb(int fd, short event, void *arg)
{
	struct bufferevent *bufev = arg;
	struct ctl_relay_event *cre = (struct ctl_relay_event *)bufev->cbarg;
	struct session *con = (struct session *)cre->con;
	struct relay *rlay = (struct relay *)con->relay;
	int ret = 0, ssl_err;
	short what = EVBUFFER_WRITE;

	if (event == EV_TIMEOUT) {
		what |= EVBUFFER_TIMEOUT;
		goto err;
	}

	if (EVBUFFER_LENGTH(bufev->output)) {
		if (cre->buf == NULL) {
			cre->buflen = EVBUFFER_LENGTH(bufev->output);
			if ((cre->buf = malloc(cre->buflen)) == NULL) {
				what |= EVBUFFER_ERROR;
				goto err;
			}
			bcopy(EVBUFFER_DATA(bufev->output),
			    cre->buf, cre->buflen);
		}

		ret = SSL_write(cre->ssl, cre->buf, cre->buflen);
		if (ret <= 0) {
			ssl_err = SSL_get_error(cre->ssl, ret);

			switch (ssl_err) {
			case SSL_ERROR_WANT_READ:
				DPRINTF("relay_ssl_writecb: session %d: "
				    "want read", con->id);
				goto retry;
			case SSL_ERROR_WANT_WRITE:
				DPRINTF("relay_ssl_writecb: session %d: "
				    "want write", con->id);
				goto retry;
			default:
				if (ret == 0)
					what |= EVBUFFER_EOF;
				else {
					ssl_error(rlay->conf.name,
					    "relay_ssl_writecb");
					what |= EVBUFFER_ERROR;
				}
				goto err;
			}
		}
		evbuffer_drain(bufev->output, ret);
	}
	if (cre->buf != NULL) {
		free(cre->buf);
		cre->buf = NULL;
		cre->buflen = 0;
	}

	if (EVBUFFER_LENGTH(bufev->output) != 0)
		relay_bufferevent_add(&bufev->ev_write, bufev->timeout_write);

	if (bufev->writecb != NULL &&
	    EVBUFFER_LENGTH(bufev->output) <= bufev->wm_write.low)
		(*bufev->writecb)(bufev, bufev->cbarg);
	return;

 retry:
	if (cre->buflen != 0)
		relay_bufferevent_add(&bufev->ev_write, bufev->timeout_write);
	return;

 err:
	if (cre->buf != NULL) {
		free(cre->buf);
		cre->buf = NULL;
		cre->buflen = 0;
	}
	(*bufev->errorcb)(bufev, what, bufev->cbarg);
}

int
relay_bufferevent_add(struct event *ev, int timeout)
{
	struct timeval tv, *ptv = NULL;

	if (timeout) {
		timerclear(&tv);
		tv.tv_sec = timeout;
		ptv = &tv;
	}

	return (event_add(ev, ptv));
}

#ifdef notyet
int
relay_bufferevent_printf(struct ctl_relay_event *cre, const char *fmt, ...)
{
	int ret;
	va_list ap;

	va_start(ap, fmt);
	ret = evbuffer_add_vprintf(cre->output, fmt, ap);
	va_end(ap);

	if (cre->bev != NULL &&
	    ret != -1 && EVBUFFER_LENGTH(cre->output) > 0 &&
	    (cre->bev->enabled & EV_WRITE))
		bufferevent_enable(cre->bev, EV_WRITE);

	return (ret);
}
#endif

int
relay_bufferevent_print(struct ctl_relay_event *cre, char *str)
{
	if (cre->bev == NULL)
		return (evbuffer_add(cre->output, str, strlen(str)));
	return (bufferevent_write(cre->bev, str, strlen(str)));
}

int
relay_bufferevent_write_buffer(struct ctl_relay_event *cre,
    struct evbuffer *buf)
{
	if (cre->bev == NULL)
		return (evbuffer_add_buffer(cre->output, buf));
	return (bufferevent_write_buffer(cre->bev, buf));
}

int
relay_bufferevent_write_chunk(struct ctl_relay_event *cre,
    struct evbuffer *buf, size_t size)
{
	int ret;
	ret = relay_bufferevent_write(cre, buf->buffer, size);
	if (ret != -1)
		evbuffer_drain(buf, size);
	return (ret);
}

int
relay_bufferevent_write(struct ctl_relay_event *cre, void *data, size_t size)
{
	if (cre->bev == NULL)
		return (evbuffer_add(cre->output, data, size));
	return (bufferevent_write(cre->bev, data, size));
}

int
relay_cmp_af(struct sockaddr_storage *a, struct sockaddr_storage *b)
{
	struct sockaddr_in ia, ib;
	struct sockaddr_in6 ia6, ib6;

	switch (a->ss_family) {
	case AF_INET:
		bcopy(a, &ia, sizeof(struct sockaddr_in));
		bcopy(b, &ib, sizeof(struct sockaddr_in));

		return (memcmp(&ia.sin_addr, &ib.sin_addr,
		    sizeof(ia.sin_addr)) +
		    memcmp(&ia.sin_port, &ib.sin_port,
		    sizeof(ia.sin_port)));
		break;
	case AF_INET6:
		bcopy(a, &ia6, sizeof(struct sockaddr_in6));
		bcopy(b, &ib6, sizeof(struct sockaddr_in6));

		return (memcmp(&ia6.sin6_addr, &ib6.sin6_addr,
		    sizeof(ia6.sin6_addr)) +
		    memcmp(&ia6.sin6_port, &ib6.sin6_port,
		    sizeof(ia6.sin6_port)));
		break;
	default:
		return (-1);
	}
}

char *
relay_load_file(const char *name, off_t *len)
{
	struct stat	 st;
	off_t		 size;
	u_int8_t	*buf = NULL;
	int		 fd;

	if ((fd = open(name, O_RDONLY)) == -1)
		return (NULL);
	if (fstat(fd, &st) != 0)
		goto fail;
	size = st.st_size;
	if ((buf = (char *)calloc(1, size + 1)) == NULL)
		goto fail;
	if (read(fd, buf, size) != size)
		goto fail;

	close(fd);

	*len = size + 1;
	return (buf);

 fail:
	if (buf != NULL)
		free(buf);
	close(fd);
	return (NULL);
}

int
relay_load_certfiles(struct relay *rlay)
{
	char	 certfile[PATH_MAX];
	char	 hbuf[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")];

	if ((rlay->conf.flags & F_SSL) == 0)
		return (0);

	if (print_host(&rlay->conf.ss, hbuf, sizeof(hbuf)) == NULL)
		return (-1);

	if (snprintf(certfile, sizeof(certfile),
	    "/etc/ssl/%s.crt", hbuf) == -1)
		return (-1);
	if ((rlay->ssl_cert = relay_load_file(certfile,
	    &rlay->ssl_cert_len)) == NULL)
		return (-1);
	log_debug("relay_load_certfile: using certificate %s", certfile);

	if (snprintf(certfile, sizeof(certfile),
	    "/etc/ssl/private/%s.key", hbuf) == -1)
		return -1;
	if ((rlay->ssl_key = relay_load_file(certfile,
	    &rlay->ssl_key_len)) == NULL)
		return (-1);
	log_debug("relay_load_certfile: using private key %s", certfile);

	return (0);
}

static __inline int
relay_proto_cmp(struct protonode *a, struct protonode *b)
{
	int ret;
	ret = strcasecmp(a->key, b->key);
	if (ret == 0)
		ret = (int)a->type - b->type;
	return (ret);
}

RB_GENERATE(proto_tree, protonode, nodes, relay_proto_cmp);

int
relay_session_cmp(struct session *a, struct session *b)
{
	struct relay	*rlay = (struct relay *)b->relay;
	struct protocol	*proto = rlay->proto;

	if (proto != NULL && proto->cmp != NULL)
		return ((*proto->cmp)(a, b));

	return ((int)a->id - b->id);
}

SPLAY_GENERATE(session_tree, session, nodes, relay_session_cmp);
