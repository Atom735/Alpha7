#define HOST_NAME "docs.microsoft.com"
#define HOST_PORT "443"
// #define HOST_RESOURCE "/en-us/windows/desktop/apiindex/windows-api-list"
#define HOST_RESOURCE "/en-us/windows/win32/apiindex/windows-api-list"

#include <openssl/conf.h>
#include <openssl/ssl.h>

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include <windows.h>

SSL_CTX *ctx_client;
int _i = 0;
CHAR _buf[0x1000];

#include "html.c"



void get_https(LPCSTR szHost, LPCSTR szResource, LPCSTR szFileName)
{
    BIO *bio;
    bio = BIO_new_ssl_connect(ctx_client);
    assert(bio!=NULL);
    snprintf(_buf, sizeof(_buf), "%s:443", szHost);
    _i = BIO_set_conn_hostname(bio, _buf);
    assert(_i==1);
    _i = BIO_do_connect(bio);
    assert(_i==1);
    _i = BIO_do_handshake(bio);
    assert(_i==1);
    snprintf(_buf, sizeof(_buf),
        "GET %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Connection: close\r\n\r\n"
        ,szResource, szHost);
    BIO_puts(bio, _buf);

    BIO *out;
    out = BIO_new_file(szFileName, "wb");
    assert(out!=NULL);
    int len = 0;
    do
    {
        len = BIO_read(bio, _buf, sizeof(_buf));
        if(len > 0)
            BIO_write(out, _buf, len);
    } while (len > 0 || BIO_should_retry(bio));

    BIO_free_all(out);
    BIO_free_all(bio);

}




int main(int argc, char const *argv[])
{
    SSL_library_init();
    SSL_load_error_strings();

    ctx_client = SSL_CTX_new(TLS_client_method());
    assert(ctx_client!=NULL);
    SSL_CTX_set_options(ctx_client, SSL_OP_NO_SSLv2|SSL_OP_NO_SSLv3);

    // get_https("docs.microsoft.com", "/en-us/windows/win32/apiindex/windows-api-list", "windows-api-list.log");
    rHtmlNode_SaveToFile ( rHtmlParser_OpenFile ( "windows-api-list.log" ), "windows-api-list.html" );

    // get_https("docs.microsoft.com", "/en-us/windows/win32/winmsg/windowing", "windowing.log");

    rHtmlNode_SaveToFile ( rHtmlParser_OpenFile ( "windowing.log" ), "windowing.html" );


    SSL_CTX_free(ctx_client);

    return 0;
}




