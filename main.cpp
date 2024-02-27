/* ************************************************************************** */
/*                                                                            */
/*                                                                            */
/*   Filename: main.cpp                                                       */
/*   Author:   Peru Riezu <riezumunozperu@gmail.com>                          */
/*   github:   https://github.com/priezu-m                                    */
/*   Licence:  GPLv3                                                          */
/*   Created:  2024/02/26 17:37:34                                            */
/*   Updated:  2024/02/27 15:30:04                                            */
/*                                                                            */
/* ************************************************************************** */

#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MTU 1464 // Orange poska is the isp whit the lowest maximum transfer unit size that we knew of when writing this
				 // code clients whit an isp whit a lower MTU will probably not recive the response packages

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

static void parse_tcp(uint8_t const *const segment, ssize_t segment_size)
{
	if ((segment_size < 20) || (segment_size > MTU))
	{
		return;
	}
	printf("Source port:%u\n", (segment[0] << 8U) + segment[1]);
	printf("Destination port:%u\n", (segment[2] << 8U) + segment[3]);
	printf("Sequence number:%u\n", (segment[4] << 24U) + (segment[5] << 16U) + (segment[6] << 8U) + segment[7]);
	printf("Acknowledgment number:%u\n",
		   (segment[8] << 24U) + (segment[9] << 16U) + (segment[10] << 8U) + segment[11]);
	printf("Data offset:%u\n", segment[12] >> 4U);
	printf("CWR flag:%u\n", static_cast<uint8_t>(segment[13] >> 7U) & 1U);
	printf("ECE flag:%u\n", static_cast<uint8_t>(segment[13] >> 6U) & 1U);
	printf("URG flag:%u\n", static_cast<uint8_t>(segment[13] >> 5U) & 1U);
	printf("ACK flag:%u\n", static_cast<uint8_t>(segment[13] >> 4U) & 1U);
	printf("PSH flag:%u\n", static_cast<uint8_t>(segment[13] >> 3U) & 1U);
	printf("RST flag:%u\n", static_cast<uint8_t>(segment[13] >> 2U) & 1U);
	printf("SYN flag:%u\n", static_cast<uint8_t>(segment[13] >> 1U) & 1U);
	printf("FIN flag:%u\n", static_cast<uint8_t>(segment[13] >> 0U) & 1U);
	printf("Window Size:%u\n", (segment[14] << 8U) + segment[15]);
	printf("Checksum:%u\n", (segment[16] << 8U) + segment[17]);
	printf("Urgent pointer:%u\n\n\n", (segment[18] << 8U) + segment[19]);
}

static ssize_t ipv4_header_size(uint8_t const *const segment, ssize_t segment_size)
{
	ssize_t	header_size;

	if (segment_size < 20)
	{
		return (-(MTU + 2));
	}
	header_size = segment[0] & 0x0fU;
	if (header_size < 5 || header_size > 15)
	{
		return (-(MTU + 2));
	}
	return (header_size * 4);
}

int main(void)
{
	int const		   listening_ipv6 = socket(AF_INET6, SOCK_RAW, IPPROTO_TCP);
	int const		   listening_ipv4 = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
	int const		   epoll_instance = epoll_create(static_cast<int>(true));
	struct epoll_event epoll_event;
	uint8_t			   segment[MTU]; 
	ssize_t			   segment_size;

	// NOLINTNEXTLINE
	system("iptables -A OUTPUT -p tcp --tcp-flags RST RST -j DROP");
	// NOLINTNEXTLINE
	system("ip6tables -A OUTPUT -p tcp --tcp-flags RST RST -j DROP");
	fcntl(listening_ipv6, F_SETFL,
		  static_cast<unsigned>(fcntl(listening_ipv6, F_GETFL, 0)) | static_cast<unsigned>(O_NONBLOCK));
	fcntl(listening_ipv4, F_SETFL,
		  static_cast<unsigned>(fcntl(listening_ipv4, F_GETFL, 0)) | static_cast<unsigned>(O_NONBLOCK));
	epoll_ctl(epoll_instance, EPOLL_CTL_ADD, listening_ipv4,
			  (struct epoll_event[]){
				  {.events = EPOLLET | EPOLLIN, .data.fd = listening_ipv4}
	  });
	epoll_ctl(epoll_instance, EPOLL_CTL_ADD, listening_ipv6,
			  (struct epoll_event[]){
				  {.events = EPOLLET | EPOLLIN, .data.fd = listening_ipv6}
	  });
	if (errno != 0)
	{
		printf("server could not be started, are you a root?\n");
		return (EXIT_FAILURE);
	}
	while (true)
	{
		while (epoll_wait(epoll_instance, &epoll_event, 1, -1) == -1)
		{
		}
		if (epoll_event.data.fd == listening_ipv4)
		{
			segment_size = recvfrom(listening_ipv4, segment, MTU, 0, nullptr, nullptr);
			while (segment_size != -1)
			{
				// ipv4 raw sockets include the ipv4 header usually of size 20 but the header must be parsed to know
				parse_tcp(segment + ipv4_header_size(segment, segment_size),
						  segment_size - ipv4_header_size(segment, segment_size));
				segment_size = recvfrom(listening_ipv4, segment, MTU, 0, nullptr, nullptr);
			}
		}
		else
		{
			segment_size = recvfrom(listening_ipv6, segment, MTU, 0, nullptr, nullptr);
			while (segment_size != -1)
			{
				// ipv6 raw sockets do not include the ipv6 header
				parse_tcp(segment, segment_size);
				segment_size = recvfrom(listening_ipv6, segment, MTU, 0, nullptr, nullptr);
			}
		}
	}
	return (EXIT_FAILURE); //this line of code should never be reached
}

#pragma GCC diagnostic pop
