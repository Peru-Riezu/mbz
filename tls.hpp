/* ************************************************************************** */
/*                                                                            */
/*                                                                            */
/*   Filename: tls.hpp                                                        */
/*   Author:   Peru Riezu <riezumunozperu@gmail.com>                          */
/*   github:   https://github.com/priezu-m                                    */
/*   Licence:  GPLv3                                                          */
/*   Created:  2024/03/26 21:32:51                                            */
/*   Updated:  2024/03/27 12:26:36                                            */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <botan/auto_rng.h>
#include <botan/certstor.h>
#include <botan/pk_keys.h>
#include <botan/pkcs8.h>
#include <botan/tls.h>
#include <cstddef>
#include <cstdint>
#include <memory>

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

/**
 * @brief Callbacks invoked by TLS::Channel.
 *
 * Botan::TLS::Callbacks is an abstract class.
 * For improved readability, only the functions that are mandatory
 * to implement are listed here. See src/lib/tls/tls_callbacks.h.
 */

class Callbacks : public Botan::TLS::Callbacks // NOLINT
{
	private:
		size_t           buffer_size;
		uint8_t         *buffer;
		struct io_uring *ring;
		int              socket_fd;

	public:
		Callbacks(uint8_t *buffer, size_t buffer_size, struct io_uring *arg_ring, int socket_fd);

		void tls_emit_data(std::span<uint8_t const> data) override;
		void tls_record_received(uint64_t seq_no, std::span<uint8_t const> data) override;
		void tls_alert(Botan::TLS::Alert alert) override;
};

/**
 * @brief Credentials storage for the tls server.
 *
 * It returns a certificate and the associated private key to
 * authenticate the tls server to the client.
 * TLS client authentication is not requested.
 * See src/lib/tls/credentials_manager.h.
 */

class Server_Credentials : public Botan::Credentials_Manager
{
	private:
		std::shared_ptr<Botan::Private_Key> m_key;
		Botan::X509_Certificate             m_cert;

	public:
		Server_Credentials();

		// clang-format off
		std::vector<Botan::Certificate_Store *> trusted_certificate_authorities(std::string const &type,
																				std::string const &context) override;
		std::vector<Botan::X509_Certificate>    cert_chain(std::vector<std::string> const &cert_key_types,
											std::vector<Botan::AlgorithmIdentifier> const &cert_signature_schemes,
											std::string const                             &type,
											std::string const                             &context) override;
		std::shared_ptr<Botan::Private_Key> private_key_for(Botan::X509_Certificate const &cert,
															std::string const             &type,
															std::string const             &context) override;
		// clang-format on
};

typedef struct s_tls_engine_params
{
		std::shared_ptr<Botan::RNG>                       rng;
		std::shared_ptr<Botan::TLS::Session_Manager_Noop> session_mgr;
		std::shared_ptr<Server_Credentials>      		  *volatile creds;
		std::shared_ptr<Botan::TLS::Strict_Policy>        policy;
} t_s_tls_engine_params;

t_s_tls_engine_params *get_tls_engine_params(void);
Botan::TLS::Server    *init_tls_engine(t_s_tls_engine_params const *&params, uint8_t arg_buffer, size_t buffer_size,
									   struct io_uring *arg_ring, int socket_fd);

#pragma GCC diagnostic pop
