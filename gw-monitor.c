#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <net/if.h>

#include <linux/rtnetlink.h>
#include <libmnl/libmnl.h>
#include <ev.h>

#include "outputs.h"


const int up_seq[] = { 440, 554, 659, 0 };
const int down_seq[] = { 659, 554, 440, 0 };

char *interface;
bool route_dump_in_progress;
bool def_route_on_interface;
bool def_route_on_interface_prev;

ev_io nl_watcher;
ev_timer route_timeout_watcher;
ev_signal stop_watcher;


void syntax(void)
{
    printf("Usage: gw-monitor <ifname>\n"
           "\n"
           "Monitor default gateway on interface and give audible and visual alerts.\n"
           "\n"
           "Arguments:\n"
           "  ifname      The interface to be monitored.\n"
           "\n"
           "Options:\n"
           "  -h, --help  Show this help message and exit.\n");
}


void parse_route_msg(const struct nlmsghdr *nlh)
{
    char ifname[IF_NAMESIZE];
    const struct nlattr *attr;
    const struct rtmsg *rtm = mnl_nlmsg_get_payload(nlh);

    ifname[0] = '\0';
    mnl_attr_for_each(attr, nlh, sizeof *rtm) {
        if (mnl_attr_type_valid(attr, RTA_MAX) > 0) {
            int type = mnl_attr_get_type(attr);

            if (type == RTA_OIF) {
                if (mnl_attr_validate(attr, MNL_TYPE_U32) >= 0) {
                    int idx = mnl_attr_get_u32(attr);

                    if (if_indextoname(idx, ifname) == NULL) {
                        ifname[0] = '\0';
                    }
                }
            }
        }
    }

    if (strncmp(interface, ifname, IF_NAMESIZE) == 0) {
        if (!route_dump_in_progress) {
            if (!ev_is_active(&route_timeout_watcher)) {
                // route change on monitored interface detected:
                // start timer
                printf("route change detected\n");
                fflush(stdout);
                ev_timer_again(EV_DEFAULT_ &route_timeout_watcher);
            }
        }
        else {
            if (nlh->nlmsg_type == RTM_NEWROUTE &&
                rtm->rtm_family == AF_INET &&
                rtm->rtm_dst_len == 0 &&
                rtm->rtm_src_len == 0 &&
                rtm->rtm_table == RT_TABLE_MAIN) {
                // default route to monitored interface found
                printf("default route found\n");
                fflush(stdout);
                def_route_on_interface = true;
            }
        }
    }
}


int nl_msg_cb(const struct nlmsghdr *nlh, void *data)
{
    switch (nlh->nlmsg_type) {
    case RTM_NEWROUTE:
    case RTM_DELROUTE:
        parse_route_msg(nlh);
        break;
    }

    return MNL_CB_OK;
}


int nl_dump_complete_cb(const struct nlmsghdr *nlh, void *data)
{
    route_dump_in_progress = false;

    if (def_route_on_interface != def_route_on_interface_prev) {
        def_route_on_interface_prev = def_route_on_interface;
        printf("state change detected: %s\n",
                def_route_on_interface ? "ON" : "OFF");
        fflush(stdout);
        if (def_route_on_interface) {
            set_led(true);
            play_sequence(up_seq);
        }
        else {
            set_led(false);
            play_sequence(down_seq);
        }
    }
    return MNL_CB_STOP;
}


mnl_cb_t nlmsg_cb_array[NLMSG_MIN_TYPE] = {
	[NLMSG_DONE]	= nl_dump_complete_cb,
};

unsigned int seq, portid;


void receive_nl_msg(struct mnl_socket *nl)
{
    char buf[MNL_SOCKET_BUFFER_SIZE];
    int len;

    len = mnl_socket_recvfrom(nl, buf, sizeof buf);

    if (len > 0) {
        mnl_cb_run2(buf, len, seq, portid, nl_msg_cb, NULL,
                    nlmsg_cb_array, NLMSG_MIN_TYPE);
    }
}


void request_route_dump(struct mnl_socket *nl)
{
    char buf[MNL_SOCKET_BUFFER_SIZE];
    struct nlmsghdr *nlh;
    struct rtmsg *rtm;

    def_route_on_interface = false;
    route_dump_in_progress = true;

    memset(buf, 0, sizeof buf);
    nlh = mnl_nlmsg_put_header(buf);
    nlh->nlmsg_type = RTM_GETROUTE;
	nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
	nlh->nlmsg_seq = ++seq;
    nlh->nlmsg_pid = portid;
    rtm = mnl_nlmsg_put_extra_header(nlh, sizeof(struct rtmsg));
    rtm->rtm_family = AF_INET;

    if (mnl_socket_sendto(nl, buf, nlh->nlmsg_len) < 0) {
        perror("mnl_socket_sendto");
    }
}


struct mnl_socket *nl_open()
{
    int group = RTNLGRP_IPV4_ROUTE;
    struct mnl_socket *nl;

    nl = mnl_socket_open(NETLINK_ROUTE);
    if (nl != NULL) {
        int fd = mnl_socket_get_fd(nl);

        if (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK) == 0) {

            if (mnl_socket_bind(nl, 0, MNL_SOCKET_AUTOPID) == 0) {
                portid = mnl_socket_get_portid(nl);
                seq = time(NULL);

                if (mnl_socket_setsockopt(nl, NETLINK_ADD_MEMBERSHIP,
                                          &group, sizeof group) == 0) {
                    return nl;
                }
                else {
                    perror("mnl_socket_setsockopt");
                }
            }
            else {
                perror("mnl_socket_bind");
            }
        }
        else {
            perror("fcntl");
        }

        mnl_socket_close(nl);
    }
    else {
        perror("mnl_socket_open");
    }

    return NULL;
}


void nl_cb(EV_P_ ev_io *w, int revents)
{
    struct mnl_socket *nl = w->data;

    receive_nl_msg(nl);
}


void timeout_cb(EV_P_ ev_timer *w, int revents)
{
    struct mnl_socket *nl = w->data;

    ev_timer_stop(EV_A_ w);
    request_route_dump(nl);
}


void stop_cb(EV_P_ ev_signal *w, int revents)
{
    ev_break(EV_A_ EVBREAK_ALL);
}


int main(int argc, char *argv[])
{
    int ret = EXIT_FAILURE;
    struct mnl_socket *nl;
    struct ev_loop *loop = EV_DEFAULT;

    if (argc != 2) {
        syntax();
        return ret;
    }
    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        syntax();
        return EXIT_SUCCESS;
    }

    interface = argv[1];
    def_route_on_interface = def_route_on_interface_prev = false;
    route_dump_in_progress = false;

    // prepare output devices
    if (init_outputs()) {
        set_led(false);

        // open netlink
        if ((nl = nl_open()) != NULL) {
            // init event loop
            ev_io_init(&nl_watcher, nl_cb, mnl_socket_get_fd(nl), EV_READ);
            nl_watcher.data = nl;
            ev_io_start(loop, &nl_watcher);

            ev_timer_init(&route_timeout_watcher, timeout_cb, 0.0, 2.0);
            route_timeout_watcher.data = nl;
            ev_timer_start(loop, &route_timeout_watcher);

            ev_signal_init(&stop_watcher, stop_cb, SIGTERM);
            ev_signal_start(loop, &stop_watcher);

            ev_run(loop, 0);
            ret = EXIT_SUCCESS;

            set_led(false);
            mnl_socket_close(nl);
        }
    }

    return ret;
}
