/* ************************************************************************** */
/*                                                                            */
/*                                                                            */
/*   Filename: main.cpp                                                       */
/*   Author:   Peru Riezu <riezumunozperu@gmail.com>                          */
/*   github:   https://github.com/priezu-m                                    */
/*   Licence:  GPLv3                                                          */
/*   Created:  2024/02/26 17:37:34                                            */
/*   Updated:  2024/02/27 12:41:46                                            */
/*                                                                            */
/* ************************************************************************** */

#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_DATAGRAM_SIZE 1500

;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic warning "-Weverything"
#pragma GCC diagnostic ignored "-Wempty-translation-unit"
#pragma GCC diagnostic ignored "-Wunused-macros"
#pragma GCC diagnostic ignored "-Wextra-semi"
#pragma GCC diagnostic ignored "-Wc99-extensions"
#pragma GCC diagnostic ignored "-Wc++98-compat"
#pragma GCC diagnostic ignored "-Wpre-c++20-compat-pedantic"
;

static void parse_tcp(uint8_t *datagram, ssize_t datagram_size)
{
	if (datagram_size < 20)
	{
		printf("datagram does not include full tcp header (contained %ld bytes).\n", datagram_size);
	}
	printf("Source port:%u\n", (datagram[0] << 8U) + datagram[1]);
	printf("Destination port:%u\n", (datagram[2] << 8U) + datagram[3]);
	printf("Sequence number:%u\n", (datagram[4] << 24U) + (datagram[5] << 16U) + (datagram[6] << 8U) + datagram[7]);
	printf("Acknowledgment number:%u\n",
		   (datagram[8] << 24U) + (datagram[9] << 16U) + (datagram[10] << 8U) + datagram[11]);
	printf("Data offset:%u\n", datagram[12] >> 4U);
	printf("CWR flag:%u\n", static_cast<uint8_t>(datagram[13] >> 7U) & 1U);
	printf("ECE flag:%u\n", static_cast<uint8_t>(datagram[13] >> 6U) & 1U);
	printf("URG flag:%u\n", static_cast<uint8_t>(datagram[13] >> 5U) & 1U);
	printf("ACK flag:%u\n", static_cast<uint8_t>(datagram[13] >> 4U) & 1U);
	printf("PSH flag:%u\n", static_cast<uint8_t>(datagram[13] >> 3U) & 1U);
	printf("RST flag:%u\n", static_cast<uint8_t>(datagram[13] >> 2U) & 1U);
	printf("SYN flag:%u\n", static_cast<uint8_t>(datagram[13] >> 1U) & 1U);
	printf("FIN flag:%u\n", static_cast<uint8_t>(datagram[13] >> 0U) & 1U);
	printf("Window Size:%u\n", (datagram[14] << 8U) + datagram[15]);
	printf("Checksum:%u\n", (datagram[16] << 8U) + datagram[17]);
	printf("Urgent pointer:%u\n\n\n", (datagram[18] << 8U) + datagram[19]);
}

int main(void)
{
	int const		   listening_ipv6 = socket(AF_INET6, SOCK_RAW, IPPROTO_TCP);
	int const		   listening_ipv4 = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
	int const		   epoll_instance = epoll_create(static_cast<int>(true));
	struct epoll_event epoll_event;
	uint8_t			   datagram[MAX_DATAGRAM_SIZE];
	ssize_t			   datagram_size;

	// NOLINTNEXTLINE
	system("iptables -A OUTPUT -p tcp --tcp-flags RST RST -j DROP");
	// NOLINTNEXTLINE
	system("ip6tables -A OUTPUT -p tcp --tcp-flags RST RST -j DROP");
	epoll_ctl(epoll_instance, EPOLL_CTL_ADD, listening_ipv4,
			  (struct epoll_event[]){
				  {.events = EPOLLET | EPOLLIN, .data.fd = listening_ipv4}
	  });
	epoll_ctl(epoll_instance, EPOLL_CTL_ADD, listening_ipv6,
			  (struct epoll_event[]){
				  {.events = EPOLLET | EPOLLIN, .data.fd = listening_ipv6}
	  });
	fcntl(listening_ipv6, F_SETFL,
		  static_cast<unsigned>(fcntl(listening_ipv6, F_GETFL, 0)) | static_cast<unsigned>(O_NONBLOCK));
	fcntl(listening_ipv4, F_SETFL,
		  static_cast<unsigned>(fcntl(listening_ipv4, F_GETFL, 0)) | static_cast<unsigned>(O_NONBLOCK));
	if (errno != 0)
	{
		printf("server could not be started, are you a root?\n");
	}
	while (true)
	{
		while (epoll_wait(epoll_instance, &epoll_event, 1, -1) == -1)
		{
		}
		if (epoll_event.data.fd == listening_ipv4)
		{
			datagram_size = recvfrom(listening_ipv4, datagram, MAX_DATAGRAM_SIZE, 0, nullptr, nullptr);
			while (datagram_size != -1)
			{
				printf("ipv4 package of size %zu recived\n", datagram_size);
				// ipv4 raw sockets include the ipv4 header usually of size 20 but the header must be parsed to know
				parse_tcp(datagram + 20, datagram_size - 20);
				datagram_size = recvfrom(listening_ipv4, datagram, MAX_DATAGRAM_SIZE, 0, nullptr, nullptr);
			}
		}
		else
		{
			datagram_size = recvfrom(listening_ipv6, datagram, MAX_DATAGRAM_SIZE, 0, nullptr, nullptr);
			while (datagram_size != -1)
			{
				printf("ipv6 package of size %zu recived\n", datagram_size);
				// ipv6 raw sockets do not include the ipv6 header
				parse_tcp(datagram, datagram_size);
				datagram_size = recvfrom(listening_ipv6, datagram, MAX_DATAGRAM_SIZE, 0, nullptr, nullptr);
			}
		}
	}
}

#pragma GCC diagnostic pop
