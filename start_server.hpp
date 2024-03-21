/* ************************************************************************** */
/*                                                                            */
/*                                                                            */
/*   Filename: start_server.hpp                                               */
/*   Author:   Peru Riezu <riezumunozperu@gmail.com>                          */
/*   github:   https://github.com/priezu-m                                    */
/*   Licence:  GPLv3                                                          */
/*   Created:  2024/03/21 10:54:41                                            */
/*   Updated:  2024/03/21 12:55:26                                            */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstddef>
#include <cstdint>
#include <netinet/in.h>
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
#pragma GCC diagnostic ignored "-Wunused-macros"
#pragma GCC diagnostic ignored "-Wpadded"
;

#define FD_TABLE_SIZE 500000
#define REGISTERED_BUFFER_SIZE 1e8

typedef enum class e_optype
{
	usser_issued_operation,
	operation_accept,
	socket_read,
	socket_write,
	timeout
} t_e_optype;

typedef struct s_identifyer
{
		t_e_optype operation;
		int        id;
		void (*callback)(void *);
		void *callback_params;
} t_s_identifyer;

typedef struct s_worker_id
{
		int      worker_num;
		uint16_t port;
} t_s_worker_id;

typedef struct s_server_cofig
{
		char           *host_name;
		struct in6_addr ip;
} t_s_server_config;

void                   start_server(t_s_worker_id worker_id, t_s_server_config *server_configs, size_t server_number);

#pragma GCC diagnostic pop
