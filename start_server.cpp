/* ************************************************************************** */
/*                                                                            */
/*                                                                            */
/*   Filename: start_server.cpp                                               */
/*   Author:   Peru Riezu <riezumunozperu@gmail.com>                          */
/*   github:   https://github.com/priezu-m                                    */
/*   Licence:  GPLv3                                                          */
/*   Created:  2024/03/21 10:55:24                                            */
/*   Updated:  2024/03/21 12:53:56                                            */
/*                                                                            */
/* ************************************************************************** */

#include <cerrno>
#include <cstdlib>
#include <liburing.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

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

void start_server(t_s_worker_id worker_id, t_s_server_config *server_configs,
				  __attribute__((unused)) size_t server_number)
{
	struct io_uring ring;
	struct iovec	vector;

	vector.iov_base = malloc(REGISTERED_BUFFER_SIZE);
	check_for_errors_worker("Error: failed to alloc memory needed for the registered buffer", worker_id);
	vector.iov_len = REGISTERED_BUFFER_SIZE;
	errno = -io_uring_queue_init(INT_MAX, &ring,
								 IORING_SETUP_CLAMP | IORING_SETUP_COOP_TASKRUN | IORING_SETUP_SINGLE_ISSUER);
	check_for_errors_worker("Error: failed to create io_uring instance", worker_id);
	errno = -io_uring_register_files_sparse(&ring, FD_TABLE_SIZE);
	check_for_errors_worker("Error: failed to create io_uring fd table", worker_id);
	errno = -io_uring_register_buffers(&ring, &vector, 1);
	check_for_errors_worker("Error: failed register registered buffer", worker_id);
	sleep(1000); // NOLINT
}

#pragma GCC diagnostic pop
