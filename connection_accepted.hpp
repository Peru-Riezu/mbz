/* ************************************************************************** */
/*                                                                            */
/*                                                                            */
/*   Filename: connection_accepted.hpp                                        */
/*   Author:   Peru Riezu <riezumunozperu@gmail.com>                          */
/*   github:   https://github.com/priezu-m                                    */
/*   Licence:  GPLv3                                                          */
/*   Created:  2024/03/22 13:19:44                                            */
/*   Updated:  2024/03/23 08:47:43                                            */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "start_server.hpp"
#include <cstdint>
#include <netinet/in.h>

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

typedef struct s_connection_accepted_data
{
		t_s_server_config_list server_config_list;
		sockaddr_in6           connection_addres;
} t_s_connection_accepted_data;

void                   connection_accepted(void *data, int32_t res, t_s_worker_id worker_id);

#pragma GCC diagnostic pop
