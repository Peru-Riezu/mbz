/* ************************************************************************** */
/*                                                                            */
/*                                                                            */
/*   Filename: main.cpp                                                       */
/*   Author:   Peru Riezu <riezumunozperu@gmail.com>                          */
/*   github:   https://github.com/priezu-m                                    */
/*   Licence:  GPLv3                                                          */
/*   Created:  2024/02/26 17:37:34                                            */
/*   Updated:  2024/02/26 18:04:02                                            */
/*                                                                            */
/* ************************************************************************** */

#include <asm-generic/socket.h>
#include <cstdio>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>

;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic warning "-Weverything"
#pragma GCC diagnostic ignored "-Wempty-translation-unit"
#pragma GCC diagnostic ignored "-Wunused-macros"
#pragma GCC diagnostic ignored "-Wextra-semi"
#pragma GCC diagnostic ignored "-Wc99-extensions"
#pragma GCC diagnostic ignored "-Wc++98-compat"
;

int main(void)
{
	int const	listening_ipv6 = socket(AF_INET6, SOCK_RAW, IPPROTO_TCP);
	int const	listening_ipv4 = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);

	perror(nullptr);
}

#pragma GCC diagnostic pop
