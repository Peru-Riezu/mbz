/* ************************************************************************** */
/*                                                                            */
/*                                                                            */
/*   Filename: tls.cpp                                                        */
/*   Author:   Peru Riezu <riezumunozperu@gmail.com>                          */
/*   github:   https://github.com/priezu-m                                    */
/*   Licence:  GPLv3                                                          */
/*   Created:  2024/03/26 21:34:19                                            */
/*   Updated:  2024/03/27 12:33:19                                            */
/*                                                                            */
/* ************************************************************************** */

#include "tls.hpp"
#include "start_server.hpp"
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <exception>
#include <liburing.h>

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

Callbacks::Callbacks(uint8_t *arg_buffer, size_t arg_buffer_size, struct io_uring *arg_ring, int arg_socket_fd)
	: buffer_size(arg_buffer_size), buffer(arg_buffer), ring(arg_ring), socket_fd(arg_socket_fd)
{
}

void Callbacks::tls_emit_data(std::span<uint8_t const> data)
{
	fprintf(stderr, "tls_emit_data called\n");
	struct io_uring_sqe *sqe = io_uring_get_sqe(ring);

	sqe->flags |= IOSQE_FIXED_FILE;
	sqe->fd = socket_fd;

	// io_uring_sqe_set_data(sqe, &identifyer);
	io_uring_prep_send(sqe, socket_fd, data.data(), data.size(), MSG_NOSIGNAL);
	// send data to tls client, e.g., using BSD sockets or boost asio
	BOTAN_UNUSED(data);
}

void Callbacks::tls_record_received(uint64_t seq_no, std::span<uint8_t const> data)
{
	// process full TLS record received by tls client, e.g.,
	// by passing it to the application
	BOTAN_UNUSED(seq_no, data);
}

void Callbacks::tls_alert(Botan::TLS::Alert alert)
{
	// handle a tls alert received from the tls server
	BOTAN_UNUSED(alert);
}

/**
 * @brief Credentials storage for the tls server.
 *
 * It returns a certificate and the associated private key to
 * authenticate the tls server to the client.
 * TLS client authentication is not requested.
 * See src/lib/tls/credentials_manager.h.
 */

Server_Credentials::Server_Credentials()
{
	Botan::DataSource_Stream in("botan.randombit.net.key");
	m_key.reset(Botan::PKCS8::load_key(in).release());
	m_cert = Botan::X509_Certificate("botan.randombit.net.crt");
}

std::vector<Botan::Certificate_Store *> Server_Credentials::trusted_certificate_authorities(std::string const &type,
																							std::string const &context)
{
	BOTAN_UNUSED(type, context);
	// if client authentication is required, this function
	// shall return a list of certificates of CAs we trust
	// for tls client certificates, otherwise return an empty list
	return (std::vector<Botan::Certificate_Store *>{});
}

std::vector<Botan::X509_Certificate>
	Server_Credentials::cert_chain(std::vector<std::string> const                &cert_key_types,
								   std::vector<Botan::AlgorithmIdentifier> const &cert_signature_schemes,
								   std::string const &type, std::string const &context)
{
	BOTAN_UNUSED(cert_key_types, cert_signature_schemes, type, context);
	// return the certificate chain being sent to the tls client
	// e.g., the certificate file "botan.randombit.net.crt"
	return (std::vector<Botan::X509_Certificate>{m_cert});
}

std::shared_ptr<Botan::Private_Key> Server_Credentials::private_key_for(Botan::X509_Certificate const &cert,
																		std::string const             &type,
																		std::string const             &context)
{
	BOTAN_UNUSED(cert, type, context);
	// return the private key associated with the leaf certificate,
	// in this case the one associated with "botan.randombit.net.crt"
	return (Server_Credentials::m_key);
}

static t_s_tls_engine_params g_params; // NOLINT

t_s_tls_engine_params       *get_tls_engine_params(void)
{
	std::shared_ptr<Server_Credentials> *creds;

	try
	{
		std::shared_ptr<Botan::RNG>                rng = std::make_shared<Botan::AutoSeeded_RNG>();
		std::shared_ptr<Botan::TLS::Session_Manager_Noop> session_mgr =
			std::make_shared<Botan::TLS::Session_Manager_Noop>();
		creds = new (std::shared_ptr<Server_Credentials>);
		std::shared_ptr<Botan::TLS::Strict_Policy>   policy = std::make_shared<Botan::TLS::Strict_Policy>();

		g_params.rng = rng;
		g_params.session_mgr = session_mgr;
		g_params.policy = policy;
		g_params.creds = creds;

		return (&g_params);
	}
	catch (...)
	{
		delete (creds);
		return (nullptr);
	}
}

Botan::TLS::Server *init_tls_engine(t_s_tls_engine_params const *&params, uint8_t buffer, size_t buffer_size,
									struct io_uring *arg_ring, int socket_fd)
{
	try
	{
		std::shared_ptr<Callbacks> callbacks = std::make_shared<Callbacks>(buffer, buffer_size, arg_ring, socket_fd);

		return (new Botan::TLS::Server{callbacks, params->session_mgr, *(params->creds), params->policy, params->rng});
	}
	catch (...)
	{
		return (nullptr);
	}
}

#pragma GCC diagnostic pop
