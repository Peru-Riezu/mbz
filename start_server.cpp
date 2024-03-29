/* ************************************************************************** */
/*                                                                            */
/*                                                                            */
/*   Filename: start_server.cpp                                               */
/*   Author:   Peru Riezu <riezumunozperu@gmail.com>                          */
/*   github:   https://github.com/priezu-m                                    */
/*   Licence:  GPLv3                                                          */
/*   Created:  2024/03/21 10:55:24                                            */
/*   Updated:  2024/03/27 12:19:08                                            */
/*                                                                            */
/* ************************************************************************** */

#include "botan/rng.h"
#include "botan/tls_callbacks.h"
#include "botan/tls_policy.h"
#include "botan/tls_server.h"
#include "botan/tls_session_manager.h"
#include "botan/tls_session_manager_memory.h"
#include "botan/tls_session_manager_noop.h"
#include "tls.hpp"
#include <cerrno>
#include <cstdint>
#include <cstdlib>
#include <liburing.h>
#include <liburing/io_uring.h>
#include <limits.h>
#include <memory>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

#include "connection_accepted.hpp"
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
;

uint8_t     g_buffer[REGISTERED_BUFFER_SIZE];

static void issue_accept_if_needed(struct io_uring *ring, int listening_fd, t_s_worker_id worker_id,
								   t_s_server_config_list server_config_list)
{
	struct io_uring_sqe                *sqe;
	static socklen_t                    addr_size = sizeof(sockaddr_in6);
	unsigned int const                  internal_index = get_next_connection_socket_index();
	static t_s_connection_accepted_data connection_accepted_data = {.server_config_list = server_config_list,
																	.buffer = g_buffer,
																	.buffer_size = REGISTERED_BUFFER_SIZE,
																	.ring = ring,
																	.socket_fd = internal_index,
																	.worker_id = worker_id};

	static t_s_identifyer identifyer = {.callback = connection_accepted, .callback_params = &connection_accepted_data};

	sqe = io_uring_get_sqe(ring);
	identifyer.fd_index = internal_index;
	io_uring_sqe_set_data(sqe, &identifyer);
	io_uring_prep_accept_direct(sqe, listening_fd,
								reinterpret_cast<struct sockaddr *>(&connection_accepted_data.connection_addres),
								&addr_size, 0, internal_index);
	errno = io_uring_submit(ring);
	if (errno < 1)
	{
		errno = -errno;
		fprintf(stderr,
				"on worker nº%d, of port %hd: failed to submit accept to io_uring, this should never happen and it "
				"is a bug: ",
				worker_id.worker_num, worker_id.port);
		perror(nullptr);
		free_connection_socket_index(internal_index);
	}
}

static void check_for_completions_and_make_callbacks(struct io_uring *ring, t_s_worker_id worker_id)
{
	struct io_uring_cqe *cqe;
	t_s_identifyer      *id_ptr;

	errno = -io_uring_wait_cqe(ring, &cqe);
	if (errno != 0)
	{
		fprintf(stderr,
				"on worker nº%d, of port %hd: failed to wait for completion queque entry: ", worker_id.worker_num,
				worker_id.port);
		perror(nullptr);
		return;
	}
	id_ptr = reinterpret_cast<t_s_identifyer *>(io_uring_cqe_get_data(cqe));
	id_ptr->callback(id_ptr->callback_params, cqe->res);
	io_uring_cqe_seen(ring, cqe);
}

static void enter_loop(t_s_worker_id worker_id, t_s_server_config_list server_config_list, struct io_uring *ring,
					   int listening_fd) __attribute__((noreturn));

static void enter_loop(t_s_worker_id worker_id, t_s_server_config_list server_config_list, struct io_uring *ring,
					   int listening_fd)
{
	while (true)
	{
		issue_accept_if_needed(ring, listening_fd, worker_id, server_config_list);
		check_for_completions_and_make_callbacks(ring, worker_id);
	}
}

void start_server(t_s_worker_id worker_id, t_s_server_config_list server_config_list, int listening_fd)
{
	struct io_uring ring;
	struct iovec    vector;

	vector.iov_base = g_buffer;
	vector.iov_len = REGISTERED_BUFFER_SIZE;
	errno = -io_uring_queue_init(INT_MAX, &ring,
								 IORING_SETUP_CLAMP | IORING_SETUP_COOP_TASKRUN | IORING_SETUP_SINGLE_ISSUER);
	check_for_errors_worker("Error: failed to create io_uring instance", worker_id);
	errno = -io_uring_register_files_sparse(&ring, FD_TABLE_SIZE);
	check_for_errors_worker("Error: failed to create io_uring fd table", worker_id);
	init_tracking_table();
	errno = -io_uring_register_buffers(&ring, &vector, 1);
	check_for_errors_worker("Error: failed to register registered buffer", worker_id);
	enter_loop(worker_id, server_config_list, &ring, listening_fd);
}

#pragma GCC diagnostic pop
