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


struct html_node {
    LPCSTR type;
    LPSTR pBegin;
    UINT nSize;
    struct html_node *pNext;
};

struct html_node * html_node_add(struct html_node *p, LPCSTR type, LPCSTR begin)
{
    p -> pNext = (struct html_node*)malloc(sizeof(struct html_node));
    p = p -> pNext;
    p -> type = type;
    p -> pBegin = begin;
    p -> nSize = 1;
    p -> pNext = NULL;
    return p;
}

void parse_file(LPCSTR szFileName)
{
    FILE *pF = fopen(szFileName,"rb");
    fseek(pF, 0, SEEK_END);
    UINT nFileLenght = ftell(pF);
    fseek(pF, 0, SEEK_SET);
    BYTE *pMem = (BYTE*)malloc(nFileLenght+1);
    UINT nReaded = 0;
    while ( nReaded < nFileLenght ) {
        nReaded += fread(pMem+nReaded, 1, nFileLenght-nReaded, pF);
    }
    fclose(pF);

    pMem[nFileLenght] = 0;

    LPSTR p = pMem;

    if(memcmp(p, "HTTP/1.1 200 OK", 15) != 0)
    {
        assert(0);
    }
    while(memcmp(p, "<!DOCTYPE html>", 15) != 0)
    {
        assert(p[15]!=0);
        ++p;
    }
    struct html_node root = {};
    root.type = "<!DOCTYPE html>";
    root.pBegin = p;
    root.nSize = 15;
    struct html_node *pLast;
    pLast = &root;
    p += 15;
    static LPCSTR _typeText = "#TEXT";
    BOOL bInTag = FALSE;
    while(*p)
    {
        if ( bInTag == FALSE )
        {
            if ( *p != '<' )
            {
                if ( pLast -> type != _typeText && isalnum(*p) )
                {
                    pLast = html_node_add(pLast, _typeText, p);
                    ++p;
                    continue;
                }
                if ( pLast -> type == _typeText )
                {
                    ++ pLast -> nSize;
                    ++p;
                    continue;
                }
                ++p;
                continue;
            }
            else
            {
                bInTag = TRUE;
                pLast = html_node_add(pLast, p+1, p);
                ++p;
                continue;
            }
        }
        else
        {
            if ( *p != '>' )
            {
                ++pLast -> nSize;
                ++p;
                continue;
            }
            else
            {
                bInTag = FALSE;
                ++pLast -> nSize;
                ++p;
                continue;
            }
        }
    }

    pF = fopen(szFileName,"wb");

    pLast = &root;
    while(pLast!=NULL)
    {
        fwrite(pLast -> pBegin, 1, pLast -> nSize, pF);
        pLast = pLast -> pNext;
    }
    fclose(pF);


    free(pMem);
}

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

    parse_file(szFileName);
}




int main(int argc, char const *argv[])
{
    SSL_library_init();
    SSL_load_error_strings();

    ctx_client = SSL_CTX_new(TLS_client_method());
    assert(ctx_client!=NULL);
    SSL_CTX_set_options(ctx_client, SSL_OP_NO_SSLv2|SSL_OP_NO_SSLv3);

    get_https("docs.microsoft.com", "/en-us/windows/win32/apiindex/windows-api-list", "windows-api-list.html");

    get_https("docs.microsoft.com", "/en-us/windows/win32/winmsg/windowing", "windowing.html");



    SSL_CTX_free(ctx_client);

    return 0;
}




