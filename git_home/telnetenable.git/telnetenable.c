/*
  This program is a re-implementation of the telnet console enabler utility
  for use with Netgear wireless routers.
  
  The original Netgear Windows binary version of this tool is available here:
  http://www.netgear.co.kr/Support/Product/FileInfo.asp?IDXNo=155
  
  Per DMCA 17 U.S.C. �1201(f)(1)-(2), the original Netgear executable was
  reverse engineered to enable interoperability with other operating systems
  not supported by the original windows-only tool (MacOS, Linux, etc).

  Currently his program implements the only the signing and encryption parts
  of Netgear telnet-enable algorithm, it does not provide the network socket
  support, but can trivially be used with 'netcat' or other tools capable of
  sending the output of this program to telnet port 23 on the router.
  

	Netgear Router - Console Telnet Enable Utility 
	Release 0.1 : 25th June 2006
	Copyright (C) 2006, yoshac @ member.fsf.org

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along
	with this program; if not, write to the Free Software Foundation, Inc.,
	51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.


  The RSA MD5 and Blowfish implementations are provided under LGPL from
  http://www.opentom.org/Mkttimage 
 
  Added a socket layer by haiyue @ Delta Networks Inc. 2008-02-25
  Hope yoshac NOT mind the stupid modification :)
*/
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "md5.h"
#include "blowfish.h"

struct PAYLOAD
{
	char signature[0x10];
	char mac[0x10];
	char username[0x10];
	char password[0x10];
	char reserved[0x40];
};

#define __BIG_ENDIAN__	1

#define LAN_IFNAME	"br0"

#define TELNET_CMD	"/usr/sbin/utelnetd -d -i " LAN_IFNAME

/*******************************************************************/
static void ether_etoa(char *p, unsigned char *e)
{	
	int i;
	static const char hex[] = "0123456789ABCDEF";

	for (i = 0; i < 6; i++) {
		p[2*i] = hex[e[i] >> 4];
		p[2*i + 1] = hex[e[i] & 0x0F];
	}

	p[2*i] = '\0';
}

static void get_mac(char *p, char *ifname)
{
	int s;
	struct ifreq ifr;
	
	s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
	if (s < 0) {
err:		printf("Can't get the MAC address of %s.\n", ifname);
		exit(-1);
	}
 	
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(s, SIOCGIFHWADDR, &ifr) == 0)
		ether_etoa(p, (unsigned char *)ifr.ifr_hwaddr.sa_data);
	else
		goto err;
	close(s);
}

static int open_telnet(char *ifname)
{
	int fd, on = 1;
	struct ifreq ifr;
	struct sockaddr_in sa;
	
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
		return -1;

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(fd, SIOCGIFADDR, &ifr) < 0)
		return -1;
	
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(23);
	sa.sin_addr = ((struct sockaddr_in *)(&ifr.ifr_addr))->sin_addr;
	
	if (bind(fd, (struct sockaddr *) &sa, sizeof(sa)) < 0)
		return -1;

	if (listen(fd, 1) < 0)
		return -1;

	return fd;
}

/****************************************************************/

int GetOutputLength(unsigned long lInputLong)
{
	unsigned long lVal = lInputLong % 8;

	if (lVal!=0)
		return lInputLong+8-lVal;
	else
		return lInputLong;
}

#define GET32U(p, x) do { \
			x = p[3]; \
			x = (x << 8) |p[2]; \
			x = (x << 8) |p[1]; \
			x = (x << 8) |p[0]; \
		} while(0)
	
#define PUT32U(p, x) do { \
			p[0] = (x) & 0xFF; \
			p[1] = (x >> 8) & 0xFF; \
			p[2] = (x >> 16) & 0xFF; \
			p[3] = (x >> 24) & 0xFF; \
		} while(0)
	
int EncodeString(void *ctx, char *pInput,char *pOutput, int lSize)
{
	int lCount;
	int lOutSize;

#if __BIG_ENDIAN__
	unsigned char *pi = (unsigned char *)pInput;
	unsigned char *po = (unsigned char *)pOutput;
#else
	int i = 0;
#endif

	lOutSize = GetOutputLength(lSize);
	lCount=0;
	while (lCount<lOutSize)
	{
	#if __BIG_ENDIAN__	
		uint32 xl, xr;

		GET32U(pi, xl); pi +=4;
		GET32U(pi, xr); pi +=4;
		Blowfish_Encrypt(ctx, &xl, &xr);
		PUT32U(po, xl); po += 4;
		PUT32U(po, xr); po += 4;
		
		lCount += 8;
	#else
		char *pi=pInput;
		char *po=pOutput;
		for (i=0; i<8; i++)
			*po++=*pi++;
		Blowfish_Encrypt(ctx, (uint32 *)pOutput, (uint32 *)(pOutput+4));
		pInput+=8;
		pOutput+=8;
		lCount+=8;
	#endif
	}

	return lCount;
}

int fill_payload(char *p)
{
	int secret_len;
	int encoded_len;
	MD5_CTX MD;
	BLOWFISH_CTX BF;
	struct PAYLOAD payload;
	char *username = "Gearguy";
	char *password = "Geardog";
	char mac[0x10], MD5_key[0x11];
	char secret_key[0x40];

	get_mac(mac, LAN_IFNAME);
	
	memset(&payload, 0, sizeof(payload));
	strcpy(payload.mac, mac);
	strcpy(payload.username, username);
	strcpy(payload.password, password);

	MD5Init(&MD);
	MD5Update(&MD, (unsigned char *)payload.mac, 0x70);
	MD5Final((unsigned char *)MD5_key, &MD);

#if 1
	memcpy(payload.signature, MD5_key, 0x10);
#else
	MD5_key[0x10] = '\0'; /* ?? */
	strcpy(payload.signature, MD5_key);
	strcat(payload.signature, mac);
#endif

	secret_len = sprintf(secret_key, "AMBIT_TELNET_ENABLE+%s", password);
	Blowfish_Init(&BF, (unsigned char *)secret_key, secret_len);	
	encoded_len = EncodeString(&BF, (char*)&payload, p, 0x80);

	return encoded_len;
}

int main(int argc, char * argv[])
{
	int listen_fd;
	int conn_fd;
	int r, datasize;
	char rbuf[512], output_buf[512];

	datasize = fill_payload(output_buf);
		
	listen_fd = open_telnet(LAN_IFNAME);
	if (listen_fd < 0) {
		printf("Can't open the socket!\n");
		return -1;
	}

	daemon(1,1);
	printf("The telnetenable is running ...\n");

	for (;;) {
		conn_fd = accept(listen_fd, NULL, NULL);
		if (conn_fd < 0)
			continue;
		
		r = read(conn_fd, rbuf, sizeof(rbuf));
		if (r == datasize && memcmp(rbuf, output_buf, r) == 0) {
			printf("The telnet server is enabled now!!!\n");
			close(conn_fd);
			shutdown(listen_fd, 2);
			close(listen_fd);
			system(TELNET_CMD);
			exit(0);
		}
		
		close(conn_fd);
	}
	
	return 0;
}

