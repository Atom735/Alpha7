#define HOST_NAME "example.com"
#define HOST_PORT "443"
#define HOST_RESOURCE "/"

#include <openssl/conf.h>
#include <openssl/ssl.h>


int main(int argc, char const *argv[])
{
    SSL_library_init();
    SSL_load_error_strings();
    SSL_CTX * ctx = NULL;
    BIO *web = NULL, *out = NULL;
    SSL *ssl = NULL;
    const SSL_METHOD * method = SSLv23_client_method();
    if(method == NULL) return __LINE__;
    ctx = SSL_CTX_new(method);
    if(ctx == NULL) return __LINE__;
    /* Cannot fail ??? */
    // SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, verify_callback);
    /* Cannot fail ??? */
    // SSL_CTX_set_verify_depth(ctx, 4);
    /* Cannot fail ??? */
    const long flags = SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_COMPRESSION;
    SSL_CTX_set_options(ctx, flags);

    // res = SSL_CTX_load_verify_locations(ctx, "random-org-chain.pem", NULL);
    // if(!(1 == res)) return __LINE__;

    web = BIO_new_ssl_connect(ctx);
    if(!(web != NULL)) return __LINE__;

    int res = BIO_set_conn_hostname(web, HOST_NAME ":" HOST_PORT);
    if(!(1 == res)) return __LINE__;

    BIO_get_ssl(web, &ssl);
    if(!(ssl != NULL)) return __LINE__;

    // const char* const PREFERRED_CIPHERS = "HIGH:!aNULL:!kRSA:!PSK:!SRP:!MD5:!RC4";
    // res = SSL_set_cipher_list(ssl, PREFERRED_CIPHERS);
    // if(!(1 == res)) handleFailure();

    res = SSL_set_tlsext_host_name(ssl, HOST_NAME);
    if(!(1 == res)) return __LINE__;

    out = BIO_new_fp(stdout, BIO_NOCLOSE);
    if(!(NULL != out)) return __LINE__;

    res = BIO_do_connect(web);
    if(!(1 == res)) return __LINE__;

    res = BIO_do_handshake(web);
    if(!(1 == res)) return __LINE__;

    /* Step 1: verify a server certificate was presented during the negotiation */
    // X509* cert = SSL_get_peer_certificate(ssl);
    // if(cert) { X509_free(cert); } /* Free immediately */
    // if(NULL == cert) handleFailure();

    /* Step 2: verify the result of chain verification */
    /* Verification performed according to RFC 4158    */
    // res = SSL_get_verify_result(ssl);
    // if(!(X509_V_OK == res)) handleFailure();

    /* Step 3: hostname verification */
    /* An exercise left to the reader */

    BIO_puts(web, "GET " HOST_RESOURCE " HTTP/1.1\r\n"
                  "Host: " HOST_NAME "\r\n"
                  "Connection: close\r\n\r\n");
    BIO_puts(out, "\n");

    int len = 0;
    do
    {
        char buff[1536] = {};
        len = BIO_read(web, buff, sizeof(buff));

        if(len > 0)
            BIO_write(out, buff, len);

    } while (len > 0 || BIO_should_retry(web));

    if(out)
        BIO_free(out);

    if(web != NULL)
        BIO_free_all(web);

    if(NULL != ctx)
        SSL_CTX_free(ctx);
    return 0;
}
