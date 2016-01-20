#include "stdafx.h"
#include "Ping.h"


CPing::CPing()
{
   
}

CPing::~CPing()
{
}

DWORD CPing::IniICMP()
{

	
    return RET_SUCCESS;
}

DWORD CPing::PingHost(char *ip)
{
	int	ret = 99999;

#ifdef	WIN32
	WSADATA ws;
	WSAStartup(0x0101,&ws);
//#else
//	;
#endif

	int iIcmpDataSize = 0;
	struct sockaddr_in dest,from;
	unsigned int addr = 0;
	struct hostent *hp;
	
	char buffer[1024];
	char recv_buffer[1024];


	memset(&dest, 0, sizeof dest);
	dest.sin_family = AF_INET;
	hp = gethostbyname(ip);
	if (!hp)
		addr = inet_addr(ip);
	if ((!hp) && (addr == INADDR_NONE))
	{
		fprintf(stderr,"Unable to resolve %s\r\n",ip);
		return ret;
	}
	if (hp != NULL)
		memcpy(&(dest.sin_addr), hp->h_addr,hp->h_length);
	else
		dest.sin_addr.s_addr = addr;

#ifdef	WIN32
//	;
#else
	setuid(getuid());
//	setuid(0);
#endif
	int sockfd;
	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

    int TimeOut=1000;//设置接收超时6秒
	if(::setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,(char *)&TimeOut,sizeof(TimeOut))==SOCKET_ERROR)
	{
		return ret;
	}

	for (int i = 0; i < 3; i++)
	{
		memset(buffer, 0, 1024);
		fill_IcmpData(buffer, iIcmpDataSize);
		unsigned short id = fill_IcmpHeader(buffer, iIcmpDataSize);
		XIcmpHeader *icmp_hdr = (XIcmpHeader *)buffer;
		int iSendSize = sendto(sockfd, buffer, sizeof(XIcmpHeader) + iIcmpDataSize, 0, (struct sockaddr*)&dest, sizeof(dest));

		memset(&from, 0, sizeof from);
		memset(recv_buffer, 0, 1024);
		int Num = 0;
#ifdef	WIN32
		int fromlen = sizeof(from);
		int iRecvSize = recvfrom(sockfd, recv_buffer, 1024, 0, (struct sockaddr*)&from,  &fromlen);
		//printf("来自IP[%s]\n",inet_ntoa(from.sin_addr));
		if (iRecvSize > 0)
		{
			while(checkicmpid(recv_buffer, iRecvSize, id) < 1)
			{
				memset(&from, 0, sizeof from);
		        memset(recv_buffer, 0, 1024);
				iRecvSize = recvfrom(sockfd, recv_buffer, 1024, 0, (struct sockaddr*)&from,  &fromlen);
				Num++;
				if(Num > 3)
				{
					iRecvSize = 0;
					break;
				}
				if(iRecvSize <= 0)
				{
					break;
				}
			}
		}
#else
		socklen_t fromlen = sizeof(from);
		int iRecvSize = recvfrom(sockfd, recv_buffer, 1024, 0, (struct sockaddr*)&from,  &fromlen);

		if (iRecvSize > 0)
		{
			while(checkicmpid(recv_buffer, iRecvSize, id) < 1)
			{
				memset(&from, 0, sizeof from);
		        memset(recv_buffer, 0, 1024);
				iRecvSize = recvfrom(sockfd, recv_buffer, 1024, 0, (struct sockaddr*)&from,  &fromlen);
				Num++;
				if(Num > 3)
				{
					iRecvSize = 0;
					break;
				}
				if(iRecvSize <= 0)
				{
					break;
				}
			}
		}
#endif
		if (iRecvSize > 0)
		{
			DWORD ttime = decode_IpIcmp(recv_buffer, iRecvSize);
			if((ret > ttime) && (ttime >= 0))
			{
				ret = ttime;
			}
		}
	}

	closesocket(sockfd);
#ifdef	WIN32
	WSACleanup();
//#else
//	;
#endif
	return ret;
}
DWORD CPing::Cleanup()
{

	return 1;
}
//puclic code
//网际校验和生产算法
//网际校验和是被校验数据16位值的反码和(ones-complement sum)
unsigned short CPing::in_cksum(unsigned short* addr, int len)
{
	int		nleft = len;
	int		sum = 0;
	unsigned short* w = addr;
	unsigned short answer = 0;
	
	while(nleft > 1) {
		sum += *w++;
		nleft -= 2;
	}

	if(nleft == 1) {
		*(unsigned char*)(&answer) = *(unsigned char*)w;
		sum += answer;
	}

	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	answer = ~sum;

	return (answer);
}

void CPing::fill_IpHeader(char *buf)
{
//	XIpHeader *ip_hdr = (XIpHeader *)buf;
}

void CPing::fill_IcmpData(char *buf, int datasize)
{
	if (buf)
	{
		char ch = 0;
		char* icmpdata = buf + sizeof(XIcmpHeader);
		for (int i = 0; i < datasize; i++)
		{
			ch = 'A' + i%('z' - 'A');
			*(icmpdata + i) = ch;
		}
	}
}

unsigned short CPing::fill_IcmpHeader(char *buf, int datasize)
{
	static unsigned short seq_no = 0;
	XIcmpHeader *icmp_hdr = (XIcmpHeader *)buf;
	if (icmp_hdr)
	{
		icmp_hdr->i_type = ICMP_ECHO;
		icmp_hdr->i_code = 0;
		icmp_hdr->i_cksum = 0;

#ifdef	WIN32
		icmp_hdr->i_id = (unsigned short)GetCurrentProcessId(); 
#else
		icmp_hdr->i_id = (unsigned short)getpid(); 
#endif
		icmp_hdr->i_seq = seq_no++;

#ifdef	WIN32
		icmp_hdr->i_timestamp = (unsigned long)::GetTickCount();
#else
		icmp_hdr->i_timestamp = (unsigned long)time(NULL);
#endif

		icmp_hdr->i_cksum = in_cksum((unsigned short*)buf, sizeof(XIcmpHeader) + datasize);

		/*fprintf(stdout, "(IcmpHeader)\r\n");
		fprintf(stdout, "%02X%02X%04X\r\n", icmp_hdr->i_type, icmp_hdr->i_code, icmp_hdr->i_cksum);
		fprintf(stdout, "%04X%04X\r\n", icmp_hdr->i_id, icmp_hdr->i_seq);
		fprintf(stdout, "%08X\r\n", icmp_hdr->i_timestamp);*/
	}
	return icmp_hdr->i_seq;
}

DWORD CPing::checkicmpid(char *buf, int size,unsigned short id)
{
	unsigned short iphdrlen = sizeof(XIpHeader);
	if(size > iphdrlen)
	{
		XIpHeader *ip_hdr = (XIpHeader *)buf;
		iphdrlen = ip_hdr->h_len << 2;
		if (size < iphdrlen + ICMP_MIN)
		{
			return 0;
		}
		else
		{
			XIcmpHeader *icmp_hdr = (XIcmpHeader *)(buf + iphdrlen);
			if(icmp_hdr->i_seq == id)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}
	else
	{
		return 0;
	}
}
// decode
DWORD CPing::decode_IpIcmp(char *buf, int size)
{
	DWORD STime = 255;
	XIpHeader *ip_hdr = (XIpHeader *)buf;
	unsigned short iphdrlen;
	if (ip_hdr)
	{

		iphdrlen = ip_hdr->h_len << 2; // number of 32-bit words *4 = bytes
		if (size < iphdrlen + ICMP_MIN)
		{
			fprintf(stdout, "Reply %d bytes Too few\r\n", size);
		}
		else
		{
			XIcmpHeader *icmp_hdr = (XIcmpHeader *)(buf + iphdrlen);

			unsigned long timestamp = 0;
#ifdef	WIN32
			timestamp = (unsigned long)::GetTickCount();
#else
			timestamp = (unsigned long)time(NULL);;
#endif
			timestamp -= icmp_hdr->i_timestamp;

			struct sockaddr_in from;
			from.sin_addr.s_addr = ip_hdr->sourceIP;

			fprintf(stdout, "Reply %d bytes from: %s time<%d TTL=%d icmp_seq=%d\r\n",
				size,
				inet_ntoa(from.sin_addr),
				timestamp,
				ip_hdr->ttl,
				icmp_hdr->i_seq
				);
			STime = timestamp;
		}
	}
	return STime;
}