#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/icmpv6.h>

#include "nlutils.h"


const char *nl_family2str(sa_family_t nl_family)
{
    switch (nl_family) {
    case AF_NETLINK:
        return "AF_NETLINK";
    default:
        return "unknown";
    }
}

const char *nlmsg_type2str(__u16 nlmsg_type)
{
    switch (nlmsg_type) {
    case NLMSG_NOOP:
        return "NLMSG_NOOP";
    case NLMSG_ERROR:
        return "NLMSG_ERROR";
    case NLMSG_DONE:
        return "NLMSG_DONE";
    case NLMSG_OVERRUN:
        return "NLMSG_OVERRUN";
    case RTM_NEWLINK:
        return "RTM_NEWLINK";
    case RTM_DELLINK:
        return "RTM_DELLINK";
    case RTM_GETLINK:
        return "RTM_GETLINK";
    case RTM_SETLINK:
        return "RTM_SETLINK";
    case RTM_NEWADDR:
        return "RTM_NEWADDR";
    case RTM_DELADDR:
        return "RTM_DELADDR";
    case RTM_GETADDR:
        return "RTM_GETADDR";
    case RTM_NEWROUTE:
        return "RTM_NEWROUTE";
    case RTM_DELROUTE:
        return "RTM_DELROUTE";
    case RTM_GETROUTE:
        return "RTM_GETROUTE";
    default:
        return "unknown";
    }
}

const char *nlmsg_flags2str(__u16 nlmsg_flags, __u16 nlmsg_type)
{
    static char buf[1024];
    buf[0] = 0;

    if (nlmsg_flags & NLM_F_REQUEST) {
        strcat(buf, "NLM_F_REQUEST ");
    }
    if (nlmsg_flags & NLM_F_MULTI) {
        strcat(buf, "NLM_F_MULTI ");
    }
    if (nlmsg_flags & NLM_F_ACK) {
        strcat(buf, "NLM_F_ACK ");
    }
    if (nlmsg_flags & NLM_F_ECHO) {
        strcat(buf, "NLM_F_ECHO ");
    }
    if (nlmsg_flags & NLM_F_DUMP_INTR) {
        strcat(buf, "NLM_F_DUMP_INTR ");
    }
    if (nlmsg_flags & NLM_F_DUMP_FILTERED) {
        strcat(buf, "NLM_F_DUMP_FILTERED ");
    }
    if (nlmsg_type == RTM_GETLINK || nlmsg_type == RTM_GETADDR || nlmsg_type == RTM_GETROUTE) {
        if (nlmsg_flags & NLM_F_ROOT) {
            strcat(buf, "NLM_F_ROOT ");
        }
        if (nlmsg_flags & NLM_F_MATCH) {
            strcat(buf, "NLM_F_MATCH ");
        }
        if (nlmsg_flags & NLM_F_ATOMIC) {
            strcat(buf, "NLM_F_ATOMIC ");
        }
    }
    else if (nlmsg_type == RTM_NEWLINK || nlmsg_type == RTM_NEWADDR || nlmsg_type == RTM_NEWROUTE) {
        if (nlmsg_flags & NLM_F_REPLACE) {
            strcat(buf, "NLM_F_REPLACE ");
        }
        if (nlmsg_flags & NLM_F_EXCL) {
            strcat(buf, "NLM_F_EXCL ");
        }
        if (nlmsg_flags & NLM_F_CREATE) {
            strcat(buf, "NLM_F_CREATE ");
        }
        if (nlmsg_flags & NLM_F_APPEND) {
            strcat(buf, "NLM_F_APPEND ");
        }
    }

    return buf;
}

const char *rtm_family2str(unsigned char rtm_family)
{
    switch (rtm_family) {
    case AF_INET:
        return "AF_INET";
    case AF_INET6:
        return "AF_INET6";
    default:
        return "unknown";
    }
}

const char *rtm_table2str(unsigned char rtm_table)
{
    switch (rtm_table) {
    case RT_TABLE_UNSPEC:
        return "RT_TABLE_UNSPEC";
    case RT_TABLE_COMPAT:
        return "RT_TABLE_COMPAT";
    case RT_TABLE_DEFAULT:
        return "RT_TABLE_DEFAULT";
    case RT_TABLE_MAIN:
        return "RT_TABLE_MAIN";
    case RT_TABLE_LOCAL:
        return "RT_TABLE_LOCAL";
    default:
        return "unknown";
    }
}

const char *rtm_protocol2str(unsigned char rtm_protocol)
{
    switch (rtm_protocol) {
    case RTPROT_UNSPEC:
        return "RTPROT_UNSPEC";
    case RTPROT_REDIRECT:
        return "RTPROT_REDIRECT";
    case RTPROT_KERNEL:
        return "RTPROT_KERNEL";
    case RTPROT_BOOT:
        return "RTPROT_BOOT";
    case RTPROT_STATIC:
        return "RTPROT_STATIC";
    case RTPROT_GATED:
        return "RTPROT_GATED";
    case RTPROT_RA:
        return "RTPROT_RA";
    case RTPROT_MRT:
        return "RTPROT_MRT";
    case RTPROT_ZEBRA:
        return "RTPROT_ZEBRA";
    case RTPROT_BIRD:
        return "RTPROT_BIRD";
    case RTPROT_DNROUTED:
        return "RTPROT_DNROUTED";
    case RTPROT_XORP:
        return "RTPROT_XORP";
    case RTPROT_NTK:
        return "RTPROT_NTK";
    case RTPROT_DHCP:
        return "RTPROT_DHCP";
    case RTPROT_MROUTED:
        return "RTPROT_MROUTED";
    case RTPROT_BABEL:
        return "RTPROT_BABEL";
    default:
        return "unknown";
    }
}

const char *rtm_scope2str(unsigned char rtm_scope)
{
    switch (rtm_scope) {
    case RT_SCOPE_UNIVERSE:
        return "RT_SCOPE_UNIVERSE";
    case RT_SCOPE_SITE:
        return "RT_SCOPE_SITE";
    case RT_SCOPE_LINK:
        return "RT_SCOPE_LINK";
    case RT_SCOPE_HOST:
        return "RT_SCOPE_HOST";
    case RT_SCOPE_NOWHERE:
        return "RT_SCOPE_NOWHERE";
    default:
        return "unknown";
    }
}

const char *rtm_type2str(unsigned char rtm_type)
{
    switch (rtm_type) {
    case RTN_UNSPEC:
        return "RTN_UNSPEC";
    case RTN_UNICAST:
        return "RTN_UNICAST";
    case RTN_LOCAL:
        return "RTN_LOCAL";
    case RTN_BROADCAST:
        return "RTN_BROADCAST";
    case RTN_ANYCAST:
        return "RTN_ANYCAST";
    case RTN_MULTICAST:
        return "RTN_MULTICAST";
    case RTN_BLACKHOLE:
        return "RTN_BLACKHOLE";
    case RTN_UNREACHABLE:
        return "RTN_UNREACHABLE";
    case RTN_PROHIBIT:
        return "RTN_PROHIBIT";
    case RTN_THROW:
        return "RTN_THROW";
    case RTN_NAT:
        return "RTN_NAT";
    case RTN_XRESOLVE:
        return "RTN_XRESOLVE";
    default:
        return "unknown";
    }
}

const char *rtm_flags2str(unsigned int rtm_flags)
{
    static char buf[1024];
    buf[0] = 0;

    if (rtm_flags & RTM_F_NOTIFY) {
        strcat(buf, "RTM_F_NOTIFY ");
    }
    if (rtm_flags & RTM_F_CLONED) {
        strcat(buf, "RTM_F_CLONED ");
    }
    if (rtm_flags & RTM_F_EQUALIZE) {
        strcat(buf, "RTM_F_EQUALIZE ");
    }

    return buf;
}

const char *rtm_rta_type2str(unsigned short rta_type)
{
    switch (rta_type) {
    case RTA_UNSPEC:
        return "RTA_UNSPEC";
    case RTA_DST:
        return "RTA_DST";
    case RTA_SRC:
        return "RTA_SRC";
    case RTA_IIF:
        return "RTA_IIF";
    case RTA_OIF:
        return "RTA_OIF";
    case RTA_GATEWAY:
        return "RTA_GATEWAY";
    case RTA_PRIORITY:
        return "RTA_PRIORITY";
    case RTA_PREFSRC:
        return "RTA_PREFSRC";
    case RTA_METRICS:
        return "RTA_METRICS";
    case RTA_MULTIPATH:
        return "RTA_MULTIPATH";
    case RTA_PROTOINFO:
        return "RTA_PROTOINFO";
    case RTA_FLOW:
        return "RTA_FLOW";
    case RTA_CACHEINFO:
        return "RTA_CACHEINFO";
    case RTA_SESSION:
        return "RTA_SESSION";
    case RTA_MP_ALGO:
        return "RTA_MP_ALGO";
    case RTA_TABLE:
        return "RTA_TABLE";
    case RTA_MARK:
        return "RTA_MARK";
    case RTA_MFC_STATS:
        return "RTA_MFC_STATS";
    case RTA_VIA:
        return "RTA_VIA";
    case RTA_NEWDST:
        return "RTA_NEWDST";
    case RTA_PREF:
        return "RTA_PREF";
    case RTA_ENCAP_TYPE:
        return "RTA_ENCAP_TYPE";
    case RTA_ENCAP:
        return "RTA_ENCAP";
    case RTA_EXPIRES:
        return "RTA_EXPIRES";
    case RTA_PAD:
        return "RTA_PAD";
    case RTA_UID:
        return "RTA_UID";
    default:
        return "unknown";
    }
}

const char *rta_pref2str(unsigned char pref)
{
    switch (pref) {
    case ICMPV6_ROUTER_PREF_LOW:
        return "ICMPV6_ROUTER_PREF_LOW";
    case ICMPV6_ROUTER_PREF_MEDIUM:
        return "ICMPV6_ROUTER_PREF_MEDIUM";
    case ICMPV6_ROUTER_PREF_HIGH:
        return "ICMPV6_ROUTER_PREF_HIGH";
    case ICMPV6_ROUTER_PREF_INVALID:
        return "ICMPV6_ROUTER_PREF_INVALID";
    default:
        return "unknown";
    }
}

const char *ifi_family2str(unsigned char ifi_family)
{
    switch (ifi_family) {
    case AF_UNSPEC:
        return "AF_UNSPEC";
    default:
        return "unknown";
    }
}

const char *ifla_rta_type2str(unsigned short rta_type)
{
    switch (rta_type) {
    case IFLA_UNSPEC:
        return "IFLA_UNSPEC";
    case IFLA_ADDRESS:
        return "IFLA_ADDRESS";
    case IFLA_BROADCAST:
        return "IFLA_BROADCAST";
    case IFLA_IFNAME:
        return "IFLA_IFNAME";
    case IFLA_MTU:
        return "IFLA_MTU";
    case IFLA_LINK:
        return "IFLA_LINK";
    case IFLA_QDISC:
        return "IFLA_QDISC";
    case IFLA_STATS:
        return "IFLA_STATS";
    case IFLA_COST:
        return "IFLA_COST";
    case IFLA_PRIORITY:
        return "IFLA_PRIORITY";
    case IFLA_MASTER:
        return "IFLA_MASTER";
    case IFLA_WIRELESS:
        return "IFLA_WIRELESS";
    case IFLA_PROTINFO:
        return "IFLA_PROTINFO";
    case IFLA_TXQLEN:
        return "IFLA_TXQLEN";
    case IFLA_MAP:
        return "IFLA_MAP";
    case IFLA_WEIGHT:
        return "IFLA_WEIGHT";
    case IFLA_OPERSTATE:
        return "IFLA_OPERSTATE";
    case IFLA_LINKMODE:
        return "IFLA_LINKMODE";
    case IFLA_LINKINFO:
        return "IFLA_LINKINFO";
    case IFLA_NET_NS_PID:
        return "IFLA_NET_NS_PID";
    case IFLA_IFALIAS:
        return "IFLA_IFALIAS";
    case IFLA_NUM_VF:
        return "IFLA_NUM_VF";
    case IFLA_VFINFO_LIST:
        return "IFLA_VFINFO_LIST";
    case IFLA_STATS64:
        return "IFLA_STATS64";
    case IFLA_VF_PORTS:
        return "IFLA_VF_PORTS";
    case IFLA_PORT_SELF:
        return "IFLA_PORT_SELF";
    case IFLA_AF_SPEC:
        return "IFLA_AF_SPEC";
    case IFLA_GROUP:
        return "IFLA_GROUP";
    case IFLA_NET_NS_FD:
        return "IFLA_NET_NS_FD";
    case IFLA_EXT_MASK:
        return "IFLA_EXT_MASK";
    case IFLA_PROMISCUITY:
        return "IFLA_PROMISCUITY";
    case IFLA_NUM_TX_QUEUES:
        return "IFLA_NUM_TX_QUEUES";
    case IFLA_NUM_RX_QUEUES:
        return "IFLA_NUM_RX_QUEUES";
    case IFLA_CARRIER:
        return "IFLA_CARRIER";
    case IFLA_PHYS_PORT_ID:
        return "IFLA_PHYS_PORT_ID";
    case IFLA_CARRIER_CHANGES:
        return "IFLA_CARRIER_CHANGES";
    case IFLA_PHYS_SWITCH_ID:
        return "IFLA_PHYS_SWITCH_ID";
    case IFLA_LINK_NETNSID:
        return "IFLA_LINK_NETNSID";
    case IFLA_PHYS_PORT_NAME:
        return "IFLA_PHYS_PORT_NAME";
    case IFLA_PROTO_DOWN:
        return "IFLA_PROTO_DOWN";
    case IFLA_GSO_MAX_SEGS:
        return "IFLA_GSO_MAX_SEGS";
    case IFLA_GSO_MAX_SIZE:
        return "IFLA_GSO_MAX_SIZE";
    case IFLA_PAD:
        return "IFLA_PAD";
    case IFLA_XDP:
        return "IFLA_XDP";
    default:
        return "unknown";
    }
}

const char *ifla_addr2str(const unsigned char *addr, size_t len)
{
    static char buf[1024];
    size_t i;

    sprintf(buf, "%02x", addr[0]);
    for (i = 1; i < len; i++) {
        sprintf(&buf[strlen(buf)], ":%02x", addr[i]);
    }
    return buf;
}

const char *ifa_family2str(unsigned char ifa_family)
{
    switch (ifa_family) {
    case AF_INET:
        return "AF_INET";
    case AF_INET6:
        return "AF_INET6";
    default:
        return "unknown";
    }
}

const char *ifa_rta_type2str(unsigned short rta_type)
{
    switch (rta_type) {
    case IFA_UNSPEC:
        return "IFA_UNSPEC";
    case IFA_ADDRESS:
        return "IFA_ADDRESS";
    case IFA_LOCAL:
        return "IFA_LOCAL";
    case IFA_LABEL:
        return "IFA_LABEL";
    case IFA_BROADCAST:
        return "IFA_BROADCAST";
    case IFA_ANYCAST:
        return "IFA_ANYCAST";
    case IFA_CACHEINFO:
        return "IFA_CACHEINFO";
    case IFA_MULTICAST:
        return "IFA_MULTICAST";
    case IFA_FLAGS:
        return "IFA_FLAGS";
    default:
        return "unknown";
    }
}

const char *ifa_flags2str(unsigned int ifa_flags)
{
    static char buf[1024];
    buf[0] = 0;

    if (ifa_flags & IFA_F_TEMPORARY) {
        strcat(buf, "IFA_F_TEMPORARY ");
    }
    if (ifa_flags & IFA_F_NODAD) {
        strcat(buf, "IFA_F_NODAD ");
    }
    if (ifa_flags & IFA_F_OPTIMISTIC) {
        strcat(buf, "IFA_F_OPTIMISTIC ");
    }
    if (ifa_flags & IFA_F_DADFAILED) {
        strcat(buf, "IFA_F_DADFAILED ");
    }
    if (ifa_flags & IFA_F_HOMEADDRESS) {
        strcat(buf, "IFA_F_HOMEADDRESS ");
    }
    if (ifa_flags & IFA_F_DEPRECATED) {
        strcat(buf, "IFA_F_DEPRECATED ");
    }
    if (ifa_flags & IFA_F_TENTATIVE) {
        strcat(buf, "IFA_F_TENTATIVE ");
    }
    if (ifa_flags & IFA_F_PERMANENT) {
        strcat(buf, "IFA_F_PERMANENT ");
    }
    if (ifa_flags & IFA_F_MANAGETEMPADDR) {
        strcat(buf, "IFA_F_MANAGETEMPADDR ");
    }
    if (ifa_flags & IFA_F_NOPREFIXROUTE) {
        strcat(buf, "IFA_F_NOPREFIXROUTE ");
    }
    if (ifa_flags & IFA_F_MCAUTOJOIN) {
        strcat(buf, "IFA_F_MCAUTOJOIN ");
    }
    if (ifa_flags & IFA_F_STABLE_PRIVACY) {
        strcat(buf, "IFA_F_STABLE_PRIVACY ");
    }

    return buf;
}
