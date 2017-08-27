#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

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


void timeout_cb(EV_P_ ev_timer *w, int revents)
{
    printf("timeout\n");
    ev_break(EV_A_ EVBREAK_ALL);
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


int parse_addr_attr_cb(const struct nlattr *attr, void *data)
{
    if (mnl_attr_type_valid(attr, RTA_MAX) < 0)
        return MNL_CB_OK;

    char addr[INET6_ADDRSTRLEN];
    unsigned char ifa_family = (unsigned char)(uintptr_t)data;
    size_t addrsize = af_addr_size(ifa_family);
    int type = mnl_attr_get_type(attr);

    printf("      rta_type: %s\n", ifa_rta_type2str(type));

    switch (type) {
    case IFA_ADDRESS:
    case IFA_LOCAL:
    case IFA_BROADCAST:
    case IFA_ANYCAST:
        if (mnl_attr_validate2(attr, MNL_TYPE_BINARY, addrsize) < 0) {
            perror("mnl_attr_validate2");
            return MNL_CB_ERROR;
        }
        break;

    case IFA_CACHEINFO:
        if (mnl_attr_validate2(attr, MNL_TYPE_BINARY, sizeof (struct ifa_cacheinfo)) < 0) {
            perror("mnl_attr_validate2");
            return MNL_CB_ERROR;
        }
        break;

    case IFA_LABEL:
        if (mnl_attr_validate(attr, MNL_TYPE_NUL_STRING) < 0) {
            perror("mnl_attr_validate");
            return MNL_CB_ERROR;
        }
        break;

    case IFA_FLAGS:
        if (mnl_attr_validate(attr, MNL_TYPE_U32) < 0) {
            perror("mnl_attr_validate");
            return MNL_CB_ERROR;
        }
        break;
    }

    if (type == IFA_ADDRESS) {
        inet_ntop(ifa_family, mnl_attr_get_payload(attr), addr, sizeof addr);
        printf("        address: %s\n", addr);
    }
    if (type == IFA_LOCAL) {
        inet_ntop(ifa_family, mnl_attr_get_payload(attr), addr, sizeof addr);
        printf("        local:   %s\n", addr);
    }
    if (type == IFA_LABEL) {
        printf("        label:   %s\n", mnl_attr_get_str(attr));
    }
    if (type == IFA_BROADCAST) {
        inet_ntop(ifa_family, mnl_attr_get_payload(attr), addr, sizeof addr);
        printf("        brcst:   %s\n", addr);
    }
    if (type == IFA_ANYCAST) {
        inet_ntop(ifa_family, mnl_attr_get_payload(attr), addr, sizeof addr);
        printf("        anycast: %s\n", addr);
    }
    if (type == IFA_CACHEINFO) {
        struct ifa_cacheinfo *ci = mnl_attr_get_payload(attr);

        printf("        ifa_prefered: %u\n", ci->ifa_prefered);
        printf("        ifa_valid:    %u\n", ci->ifa_valid);
        printf("        cstamp:       %u\n", ci->cstamp);
        printf("        tstamp:       %u\n", ci->tstamp);
    }
    if (type == IFA_FLAGS) {
        unsigned int flags = mnl_attr_get_u32(attr);

        printf("        flags:        %s\n", ifa_flags2str(flags));
    }

    return MNL_CB_OK;
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


void parse_addr_msg(const struct nlmsghdr *nlh)
{
    char ifname[IF_NAMESIZE];
    const struct ifaddrmsg *ifa = mnl_nlmsg_get_payload(nlh);

    printf("  nlmsg_type:  %s\n", nlmsg_type2str(nlh->nlmsg_type));
    printf("    ifa_family:    %s\n", ifa_family2str(ifa->ifa_family));
    printf("    ifa_prefixlen: %u\n", ifa->ifa_prefixlen);
    printf("    ifa_flags:     %s\n", ifa_flags2str(ifa->ifa_flags));
    printf("    ifa_scope:     %s\n", rtm_scope2str(ifa->ifa_scope));
    printf("    ifa_index:     %s (%d)\n", if_indextoname(ifa->ifa_index, ifname), ifa->ifa_index);

    mnl_attr_parse(nlh, sizeof *ifa, parse_addr_attr_cb, (void *)(uintptr_t)ifa->ifa_family);
}


int nl_msg_cb(const struct nlmsghdr *nlh, void *data)
{
    switch (nlh->nlmsg_type) {
    case RTM_NEWROUTE:
    case RTM_DELROUTE:
        parse_route_msg(nlh);
        break;
    case RTM_NEWADDR:
    case RTM_DELADDR:
        parse_addr_msg(nlh);
        break;
    }

    return MNL_CB_OK;
}


void nl_cb(EV_P_ ev_io *w, int revents)
{
    int len;
    char buf[MNL_SOCKET_BUFFER_SIZE];
    struct mnl_socket *nl = w->data;

    printf("### nl_cb ###\n");
    len = mnl_socket_recvfrom(nl, buf, sizeof buf);

    if (len > 0) {
        mnl_cb_run(buf, len, 0, 0, nl_msg_cb, NULL);
    }
}


int join_mcast_groups(struct mnl_socket *nl)
{
    int groups[] = {
        //RTNLGRP_LINK,
        RTNLGRP_IPV4_IFADDR,
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


int main(int argc, char *argv[])
{
    int ret = EXIT_FAILURE;
    struct mnl_socket *nl;

    struct ev_loop *loop = EV_DEFAULT;
    ev_io nl_watcher;
    ev_timer timeout_watcher;

    // open netlink
    nl = mnl_socket_open2(NETLINK_ROUTE, SOCK_NONBLOCK);
    if (nl != NULL) {
        if (mnl_socket_bind(nl, 0, MNL_SOCKET_AUTOPID) == 0) {
            if (join_mcast_groups(nl) == 0) {
                // init event loop
                ev_io_init(&nl_watcher, nl_cb, mnl_socket_get_fd(nl), EV_READ);
                nl_watcher.data = nl;
                ev_io_start(loop, &nl_watcher);

                ev_timer_init(&timeout_watcher, timeout_cb, 50.0, 0.0);
                ev_timer_start(loop, &timeout_watcher);

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
