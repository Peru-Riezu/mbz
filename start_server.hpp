/* ************************************************************************** */
/*                                                                            */
/*                                                                            */
/*   Filename: start_server.hpp                                               */
/*   Author:   Peru Riezu <riezumunozperu@gmail.com>                          */
/*   github:   https://github.com/priezu-m                                    */
/*   Licence:  GPLv3                                                          */
/*   Created:  2024/03/21 10:54:41                                            */
/*   Updated:  2024/03/22 13:29:39                                            */
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
#define FD_TABLE_SIZE          500000
#define REGISTERED_BUFFER_SIZE 100000000

typedef enum class e_optype
{
	usser_issued_operation,
	operation_accept,
	socket_read,
	socket_write,
	timeout
} t_e_optype;

typedef struct s_worker_id
{
		unsigned int worker_num;
		uint16_t     port;
} t_s_worker_id;

typedef struct s_identifyer
{
		t_e_optype   operation;
		unsigned int fd_index;

		union
		{
				void (*internal_callback)(void *, int32_t, t_s_worker_id); // do not give this deffinition to the user
				void (*callback)(void *, int32_t);
		};

		void *callback_params;
} t_s_identifyer;

typedef struct s_server_cofig
{
		char           *host_name;
		struct in6_addr ipv6;
		struct in_addr  ipv4;
		char           *ctr_path;
		char           *key_path;
		bool            move_www_host_name;
		bool            move_port_80_to_servers_port;
} t_s_server_config;

typedef struct s_server_cofig_list
{
		t_s_server_config *server_configs;
		size_t             server_number;
} t_s_server_config_list;

void start_server(t_s_worker_id worker_id, t_s_server_config_list server_config_list, int listening_fd)
	__attribute__((noreturn));
void                   free_connection_socket_index(unsigned int internal_index);
unsigned int           get_next_connection_socket_index(void);
void                   init_tracking_table(void);

#pragma GCC diagnostic pop
