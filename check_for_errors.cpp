/* ************************************************************************** */
/*                                                                            */
/*                                                                            */
/*   Filename: check_for_errors.cpp                                           */
/*   Author:   Peru Riezu <riezumunozperu@gmail.com>                          */
/*   github:   https://github.com/priezu-m                                    */
/*   Licence:  GPLv3                                                          */
/*   Created:  2024/03/21 12:18:59                                            */
/*   Updated:  2024/03/21 12:53:25                                            */
/*                                                                            */
/* ************************************************************************** */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "helpers.hpp"
#include "start_server.hpp"

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

void check_for_errors(char const *str)
{
	if (errno != 0)
	{
		perror(str);
		exit(EXIT_FAILURE); // NOLINT
	}
}

void check_for_errors_worker(char const *str, t_s_worker_id worker_id)
{
	if (errno != 0)
	{
		fprintf(stderr, "on worker nº%d, of port %hd: %s: ", worker_id.worker_num, worker_id.port, str);
		perror(nullptr);
		exit(EXIT_FAILURE); // NOLINT
	}
}

#pragma GCC diagnostic pop
