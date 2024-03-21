/* ************************************************************************** */
/*                                                                            */
/*                                                                            */
/*   Filename: main.cpp                                                       */
/*   Author:   Peru Riezu <riezumunozperu@gmail.com>                          */
/*   github:   https://github.com/priezu-m                                    */
/*   Licence:  GPLv3                                                          */
/*   Created:  2024/03/21 02:10:13                                            */
/*   Updated:  2024/03/21 13:38:34                                            */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <cstdint>
#include <errno.h>
#include <limits.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include "helpers.hpp"
#include "start_server.hpp"

;
#pragma GCC diagnostic push
#pragma GCC diagnostic warning "-Weverything"
#pragma GCC diagnostic ignored "-Wunsafe-buffer-usage"
#pragma GCC diagnostic ignored "-Wc++98-compat"
#pragma GCC diagnostic ignored "-Wwritable-strings"
#pragma GCC diagnostic ignored "-Wlanguage-extension-token"
#pragma GCC diagnostic ignored "-Wpre-c++20-compat-pedantic"
#pragma GCC diagnostic ignored "-Wc++20-designator"
#pragma GCC diagnostic ignored "-Wc++98-compat-extra-semi"
#pragma GCC diagnostic ignored "-Wc99-designator"
;

static void lauch_workers_for_port(uint16_t port, int worker_number, t_s_server_config *servers, size_t server_number)
{
	int                 lisening_fd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
	struct sockaddr_in6 bind_addres = {
		.sin6_family = AF_INET6, .sin6_port = htons(port), .sin6_addr = IN6ADDR_ANY_INIT};
	int           false_val = 0;
	t_s_worker_id worker_id = {.port = port};

	check_for_errors("Error: could not create listening socket");
	setsockopt(lisening_fd, IPPROTO_IPV6, IPV6_V6ONLY, &false_val, sizeof(int));
	check_for_errors("Error: could not set ipv6_only to false in listening socket");
	setsockopt(lisening_fd, SOL_SOCKET, SO_REUSEADDR, &false_val, sizeof(int));
	check_for_errors("Error: could not set so_reuseaddr in listening socket");
	(void)bind(lisening_fd, reinterpret_cast<struct sockaddr *>(&bind_addres), sizeof(bind_addres));
	check_for_errors("Error: could not bind listening socket to its port");
	listen(lisening_fd, INT_MAX);
	check_for_errors("Error: could make listening socket to start listening");
	for (int i = 0; i < worker_number; i++)
	{
		if (fork() == 0)
		{
			worker_id.worker_num = i + 1;
			start_server(worker_id, servers, server_number);
			fprintf(stderr, "Something went wrong in the worker nº%d of port %hu: ", i + 2, worker_id.port);
			perror(nullptr);
			exit(EXIT_FAILURE); // NOLINT
		}
		check_for_errors("Error: could not launch one of the workers");
	}
}

int main(void)
{
	t_s_server_config servers[] = {[0] = {.host_name = "*"}};

	lauch_workers_for_port(443, 6, servers, sizeof(servers) / sizeof(servers[0]));
	if (isatty(STDOUT_FILENO) == 1 || isatty(STDERR_FILENO) == 1 || isatty(STDIN_FILENO) == 1)
	{
		sleep(UINT_MAX); // NOLINT
	}
	return (EXIT_FAILURE);
}

#pragma GCC diagnostic pop
