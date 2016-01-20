#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#ifdef	WIN32

#define WIN32_LEAN_AND_MEAN
#include <winsock.h>
#pragma	comment(lib, "Wsock32.lib")

#else

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

#endif

#define ICMP_ECHO 8 
#define ICMP_ECHOREPLY 0 

//#define ICMP_MIN 8	// minimum 8 byte icmp packet (just header) 
#define ICMP_MIN (8 + 4)	// minimum 8 byte icmp packet (just header + timestamp)

// IP header
typedef struct _tagX_iphdr
{
	unsigned char	h_len:4;          // length of the header 
	unsigned char	version:4;        // Version of IP 
	unsigned char	tos;             // Type of service 
	unsigned short	total_len;      // total length of the packet 

	unsigned short	ident;          // unique identifier 
	unsigned short	frag_and_flags; // flags 
	
	unsigned char	ttl;				// ttl
	unsigned char	proto;           // protocol (TCP, UDP etc) 
	unsigned short	checksum;       // IP checksum 
	
	unsigned int	sourceIP; 
	unsigned int	destIP; 
}XIpHeader;

// ICMP header
typedef struct _tagX_icmphdr
{
	unsigned char	i_type;
	unsigned char	i_code;
	unsigned short	i_cksum;
	unsigned short	i_id;
	unsigned short	i_seq;
	unsigned long	i_timestamp;
}XIcmpHeader;



#define MAXLEN 526

class CPing
{
public:

    CPing();
    ~CPing();


    
    DWORD IniICMP();
	DWORD PingHost(char *ip);
	DWORD Cleanup();
	unsigned short in_cksum(unsigned short* addr, int len);
	void fill_IpHeader(char *buf);
	void fill_IcmpData(char *buf, int datasize);
	unsigned short fill_IcmpHeader(char *buf, int datasize);
	DWORD decode_IpIcmp(char *buf, int size);
	DWORD checkicmpid(char *buf, int size,unsigned short id);
	//DWORD PingHostList(std::vector<char *> *iplist);



public:



private:


    

};
