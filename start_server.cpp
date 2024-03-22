/* ************************************************************************** */
/*                                                                            */
/*                                                                            */
/*   Filename: start_server.cpp                                               */
/*   Author:   Peru Riezu <riezumunozperu@gmail.com>                          */
/*   github:   https://github.com/priezu-m                                    */
/*   Licence:  GPLv3                                                          */
/*   Created:  2024/03/21 10:55:24                                            */
/*   Updated:  2024/03/22 14:03:19                                            */
/*                                                                            */
/* ************************************************************************** */

#include <cerrno>
#include <cstdint>
#include <cstdlib>
#include <liburing.h>
#include <liburing/io_uring.h>
#include <limits.h>
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

static void issue_accept_if_needed(struct io_uring *ring, int listening_fd, t_s_worker_id worker_id,
								   t_s_server_config_list server_config_list)
{
	struct io_uring_sqe                *sqe;
	socklen_t                           addr_size;
	static t_s_connection_accepted_data connection_accepted_data = {.server_config_list = server_config_list};
	static t_s_identifyer               identifyer = {.operation = e_optype::operation_accept,
													  .internal_callback = connection_accepted,
													  .callback_params = &connection_accepted_data};
	unsigned int const                  internal_index = get_next_connection_socket_index();

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
	id_ptr->internal_callback(id_ptr->callback_params, cqe->res, worker_id);
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
	static uint8_t  buffer[REGISTERED_BUFFER_SIZE];

	vector.iov_base = buffer;
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
