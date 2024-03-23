/* ************************************************************************** */
/*                                                                            */
/*                                                                            */
/*   Filename: connection_accepted.cpp                                        */
/*   Author:   Peru Riezu <riezumunozperu@gmail.com>                          */
/*   github:   https://github.com/priezu-m                                    */
/*   Licence:  GPLv3                                                          */
/*   Created:  2024/03/22 12:45:42                                            */
/*   Updated:  2024/03/23 09:47:41                                            */
/*                                                                            */
/* ************************************************************************** */

#include "connection_accepted.hpp"
#include <arpa/inet.h>
#include <cstdio>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>

;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic warning "-Weverything"
#pragma GCC diagnostic ignored "-Wempty-translation-unit"
#pragma GCC diagnostic ignored "-Wunused-macros"
#pragma GCC diagnostic ignored "-Wextra-semi"
#pragma GCC diagnostic ignored "-Wunsafe-buffer-usage"
#pragma GCC diagnostic ignored "-Wc++98-compat"
#pragma GCC diagnostic ignored "-Wwritable-strings"
#pragma GCC diagnostic ignored "-Wlanguage-extension-token"
#pragma GCC diagnostic ignored "-Wpre-c++20-compat-pedantic"
#pragma GCC diagnostic ignored "-Wc++20-designator"
#pragma GCC diagnostic ignored "-Wc++98-compat-extra-semi"
;

void connection_accepted(void *data, int32_t res, t_s_worker_id worker_id)
{
	t_s_connection_accepted_data connection_accepted_data = *reinterpret_cast<t_s_connection_accepted_data *>(data);
	in_addr                      ipv4;
	char                         str[8 * 4 + 7 + 1];

	fprintf(stderr, "worker nº%u of port %hu: ", worker_id.worker_num, worker_id.port);
	if (res == 0)
	{
		if ((connection_accepted_data.connection_addres.sin6_addr.s6_addr[0] == 0) &&
			(connection_accepted_data.connection_addres.sin6_addr.s6_addr[1] == 0) &&
			(connection_accepted_data.connection_addres.sin6_addr.s6_addr[2] == 0) &&
			(connection_accepted_data.connection_addres.sin6_addr.s6_addr[3] == 0) &&
			(connection_accepted_data.connection_addres.sin6_addr.s6_addr[4] == 0) &&
			(connection_accepted_data.connection_addres.sin6_addr.s6_addr[5] == 0) &&
			(connection_accepted_data.connection_addres.sin6_addr.s6_addr[6] == 0) &&
			(connection_accepted_data.connection_addres.sin6_addr.s6_addr[7] == 0) &&
			(connection_accepted_data.connection_addres.sin6_addr.s6_addr[8] == 0) &&
			(connection_accepted_data.connection_addres.sin6_addr.s6_addr[9] == 0) &&
			(connection_accepted_data.connection_addres.sin6_addr.s6_addr[10] == 0xff) &&
			(connection_accepted_data.connection_addres.sin6_addr.s6_addr[11] == 0xff))
		{
			ipv4.s_addr =
				static_cast<unsigned>(connection_accepted_data.connection_addres.sin6_addr.s6_addr[15] << 8U * 3U) |
				static_cast<unsigned>(connection_accepted_data.connection_addres.sin6_addr.s6_addr[14] << 8U * 2U) |
				static_cast<unsigned>(connection_accepted_data.connection_addres.sin6_addr.s6_addr[13] << 8U * 1U) |
				connection_accepted_data.connection_addres.sin6_addr.s6_addr[12];

			inet_ntop(AF_INET, &ipv4, str, sizeof(str));
		}
		else
		{
			inet_ntop(AF_INET6, &connection_accepted_data.connection_addres.sin6_addr, str, sizeof(str));
		}
		fprintf(stderr, "connection accepted from ip: %s\n", str);
	}
	else
	{
		errno = -res;
		fprintf(stderr, "accept failed: ");
		perror(nullptr);
	}
}

#pragma GCC diagnostic pop
