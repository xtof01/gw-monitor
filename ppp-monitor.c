#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <time.h>
#include <libmnl/libmnl.h>
#include <linux/rtnetlink.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>  // for if_indextoname
#include <ev.h>

#include "nlutils.h"


void syntax(void)
{
    printf("Usage: ppp-monitor <ifname>\n"
           "\n"
           "Monitor interface and give audible and visual alerts.\n"
           "\n"
           "Arguments:\n"
           "  ifname      The interface to be monitored.\n"
           "\n"
           "Options:\n"
           "  -h, --help  Show this help message and exit.\n");
}


size_t af_addr_size(unsigned char family)
{
    switch (family) {
    case AF_INET:
        return sizeof(struct in_addr);
    case AF_INET6:
        return sizeof(struct in6_addr);
    default:
        return UINT_MAX;
    }
}


int parse_route_attr_cb(const struct nlattr *attr, void *data)
{
    if (mnl_attr_type_valid(attr, RTA_MAX) < 0)
		return MNL_CB_OK;

    char ifname[IF_NAMESIZE];
    char addr[INET6_ADDRSTRLEN];
    unsigned char rtm_family = (unsigned char)(uintptr_t)data;
    size_t addrsize = af_addr_size(rtm_family);
    int type = mnl_attr_get_type(attr);

    printf("      rta_type: %s\n", rtm_rta_type2str(type));

    switch (type) {
    case RTA_DST:
    case RTA_GATEWAY:
        if (mnl_attr_validate2(attr, MNL_TYPE_BINARY, addrsize) < 0) {
            perror("mnl_attr_validate2");
            return MNL_CB_ERROR;
        }
        break;

    case RTA_OIF:
    case RTA_PRIORITY:
    case RTA_TABLE:
        if (mnl_attr_validate(attr, MNL_TYPE_U32) < 0) {
            perror("mnl_attr_validate");
            return MNL_CB_ERROR;
        }
        break;

    case RTA_PREF:
        if (mnl_attr_validate(attr, MNL_TYPE_U8) < 0) {
            perror("mnl_attr_validate");
            return MNL_CB_ERROR;
        }
        break;
    }

    if (type == RTA_DST) {
        inet_ntop(rtm_family, mnl_attr_get_payload(attr), addr, sizeof addr);
        printf("        dst:   %s\n", addr);
    }
    if (type == RTA_GATEWAY) {
        inet_ntop(rtm_family, mnl_attr_get_payload(attr), addr, sizeof addr);
        printf("        gw:    %s\n", addr);
    }
    if (type == RTA_OIF) {
        int idx = mnl_attr_get_u32(attr);

        printf("        oif:   %s (%d)\n", if_indextoname(idx, ifname), idx);
    }
    if (type == RTA_PRIORITY) {
        int prio = mnl_attr_get_u32(attr);

        printf("        prio:  %d\n", prio);
    }
    if (type == RTA_TABLE) {
        unsigned int table = mnl_attr_get_u32(attr);

        printf("        table: %s\n", rtm_table2str(table));
    }
    if (type == RTA_PREF) {
        unsigned char pref = mnl_attr_get_u8(attr);

        printf("        pref:  %s\n", rta_pref2str(pref));
    }

    return MNL_CB_OK;
}


void parse_route_msg(const struct nlmsghdr *nlh)
{
    const struct rtmsg *rtm = mnl_nlmsg_get_payload(nlh);

    printf("  nlmsg_type:  %s\n", nlmsg_type2str(nlh->nlmsg_type));
    printf("    rtm_family:   %s\n", rtm_family2str(rtm->rtm_family));
    printf("    rtm_dst_len:  %3u\n", rtm->rtm_dst_len);
    printf("    rtm_src_len:  %3u\n", rtm->rtm_src_len);
    printf("    rtm_tos:      %3u\n", rtm->rtm_tos);
    printf("    rtm_table:    %s\n", rtm_table2str(rtm->rtm_table));
    printf("    rtm_protocol: %s\n", rtm_protocol2str(rtm->rtm_protocol));
    printf("    rtm_scope:    %s\n", rtm_scope2str(rtm->rtm_scope));
    printf("    rtm_type:     %s\n", rtm_type2str(rtm->rtm_type));
    printf("    rtm_flags:    %08X: %s\n", rtm->rtm_flags, rtm_flags2str(rtm->rtm_flags));

    mnl_attr_parse(nlh, sizeof *rtm, parse_route_attr_cb, (void *)(uintptr_t)rtm->rtm_family);
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


int join_mcast_groups(struct mnl_socket *nl)
{
    int groups[] = {
        //RTNLGRP_LINK,
        //RTNLGRP_IPV4_IFADDR,
        //RTNLGRP_IPV6_IFADDR,
        RTNLGRP_IPV4_ROUTE,
        //RTNLGRP_IPV6_ROUTE,
    };
    int ret = 0;
    size_t i;

    for (i = 0; i < MNL_ARRAY_SIZE(groups); i++) {
        if (mnl_socket_setsockopt(nl, NETLINK_ADD_MEMBERSHIP,
                                  &groups[i], sizeof groups[i]) != 0) {
            ret = 1;
            break;
        }
    }
    return ret;
}


unsigned int seq, portid;


void receive_nl_msg(struct mnl_socket *nl)
{
    char buf[MNL_SOCKET_BUFFER_SIZE];
    int len;

    len = mnl_socket_recvfrom(nl, buf, sizeof buf);

    if (len > 0) {
        mnl_cb_run(buf, len, seq, portid, nl_msg_cb, NULL);
    }
}

void request_route_dump(struct mnl_socket *nl)
{
    char buf[MNL_SOCKET_BUFFER_SIZE];
    struct nlmsghdr *nlh;
    struct rtmsg *rtm;

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


void nl_cb(EV_P_ ev_io *w, int revents)
{
    struct mnl_socket *nl = w->data;

    printf("### nl_cb ###\n");
    receive_nl_msg(nl);
}


void timeout_cb(EV_P_ ev_timer *w, int revents)
{
    struct mnl_socket *nl = w->data;

    printf("### timeout ###\n");
    ev_timer_stop(EV_A_ w);
    request_route_dump(nl);
}


int main(int argc, char *argv[])
{
    int ret = EXIT_FAILURE;
    struct mnl_socket *nl;

    struct ev_loop *loop = EV_DEFAULT;
    ev_io nl_watcher;
    ev_timer route_timeout_watcher;

    // open netlink
    nl = mnl_socket_open2(NETLINK_ROUTE, SOCK_NONBLOCK);
    if (nl != NULL) {
        if (mnl_socket_bind(nl, 0, MNL_SOCKET_AUTOPID) == 0) {
            portid = mnl_socket_get_portid(nl);
            seq = time(NULL);

            if (join_mcast_groups(nl) == 0) {
                // init event loop
                ev_io_init(&nl_watcher, nl_cb, mnl_socket_get_fd(nl), EV_READ);
                nl_watcher.data = nl;
                ev_io_start(loop, &nl_watcher);

                ev_timer_init(&route_timeout_watcher, timeout_cb, 0.0, 1.0);
                route_timeout_watcher.data = nl;
                ev_timer_again(loop, &route_timeout_watcher);

                ev_run(loop, 0);
                ret = EXIT_SUCCESS;
            }
            else {
                perror("mnl_socket_setsockopt");
            }
        }
        else {
            perror("mnl_socket_bind");
        }

        mnl_socket_close(nl);
    }
    else {
        perror("mnl_socket_open");
    }

    return ret;
}
