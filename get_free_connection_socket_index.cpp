/* ************************************************************************** */
/*                                                                            */
/*                                                                            */
/*   Filename: get_free_connection_socket_index.cpp                           */
/*   Author:   Peru Riezu <riezumunozperu@gmail.com>                          */
/*   github:   https://github.com/priezu-m                                    */
/*   Licence:  GPLv3                                                          */
/*   Created:  2024/03/22 12:12:06                                            */
/*   Updated:  2024/03/22 13:56:40                                            */
/*                                                                            */
/* ************************************************************************** */

#include "start_server.hpp"
#include <cstdint>

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

static unsigned int tracking_table[FD_TABLE_SIZE / 2];
static unsigned int available;
static unsigned int given;

void                init_tracking_table(void)
{
	for (unsigned int i = 1; i < FD_TABLE_SIZE / 2; i++)
	{
		tracking_table[i] = i;
	}
}

void free_connection_socket_index(unsigned int internal_index)
{
	tracking_table[available] = internal_index;
	available++;
}

unsigned int get_next_connection_socket_index(void)
{
	unsigned int ret = tracking_table[available];

	if (((available + 1) % FD_TABLE_SIZE) == given)
	{
		return (UINT32_MAX);
	}
	available++;
	return (ret);
}

#pragma GCC diagnostic pop
