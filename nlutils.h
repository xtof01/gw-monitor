#ifndef _NLUTILS_H_
#define _NLUTILS_H_

const char *nl_family2str(sa_family_t nl_family);
const char *nlmsg_type2str(__u16 nlmsg_type);
const char *nlmsg_flags2str(__u16 nlmsg_flags, __u16 nlmsg_type);
const char *rtm_family2str(unsigned char rtm_family);
const char *rtm_table2str(unsigned char rtm_table);
const char *rtm_protocol2str(unsigned char rtm_protocol);
const char *rtm_scope2str(unsigned char rtm_scope);
const char *rtm_type2str(unsigned char rtm_type);
const char *rtm_flags2str(unsigned int rtm_flags);
const char *rtm_rta_type2str(unsigned short rta_type);
const char *rta_pref2str(unsigned char pref);
const char *ifi_family2str(unsigned char ifi_family);
const char *ifla_rta_type2str(unsigned short rta_type);
const char *ifla_addr2str(const unsigned char *addr, size_t len);
const char *ifa_family2str(unsigned char ifa_family);
const char *ifa_rta_type2str(unsigned short rta_type);
const char *ifa_flags2str(unsigned int ifa_flags);

#endif
