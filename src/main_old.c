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

#define D_CMP_CHAR(a,b) (((a)==(b))||(((a)>='A')&&((a)<='Z')&&((a)^(b))==(0x20)))

BOOL rCmpStr ( LPCSTR pSrc, LPCSTR pDst )
{
    while ( D_CMP_CHAR ( *pSrc, *pDst ) ) { ++pSrc; ++pDst; }
    if ( *pDst == 0 ) return TRUE;
    return FALSE;
}

BYTE* rLoadFile ( CONST LPCSTR szFileName, UINT * CONST pLen )
{
    FILE *pF = fopen ( szFileName, "rb" );
    fseek ( pF, 0, SEEK_END );
    UINT nFileLenght = ftell ( pF );
    fseek ( pF, 0, SEEK_SET );
    BYTE *pMem = (BYTE*) malloc ( nFileLenght+1 );
    UINT nReaded = 0;
    while ( nReaded < nFileLenght ) {
        nReaded += fread ( pMem+nReaded, 1, nFileLenght-nReaded, pF );
    }
    fclose ( pF );
    if ( pLen ) *pLen = nFileLenght;
    pMem [ nFileLenght ] = 0;
    return pMem;
}

struct html_node {
    LPCSTR pBegin;
    UINT nSize;
    struct html_node *pParent, *pChild, *pNext, *pPrev;
};

struct html_node_comment
{

};

struct html_document
{
    LPVOID pBuffer;
    LPCSTR pDocType;
    void (*rParserState)( struct html_document * pThis );
    PBYTE pParserStream;
};

void rHtmlParser_Initial ( struct html_document * pThis )
{
    pThis -> p;

}



struct html_node * rHtmlNodeCreate ( LPCSTR pBegin )
{
    struct html_node * p = (struct html_node*) malloc ( sizeof(struct html_node) );
    memset ( p, 0, sizeof(struct html_node) );
    p -> pBegin = pBegin;
    if ( rCmpStr ( pBegin, "<!--" ) )
    {
        pBegin += 4;
        p -> nSize += 4;
        while ( !rCmpStr ( pBegin, "-->" ) ) { ++pBegin; ++(p -> nSize); }
        pBegin += 3;
        p -> nSize += 3;
    }
    else
    if ( *pBegin == '<' )
    {
        while ( *pBegin != '>' ) { ++pBegin; ++(p -> nSize); }
        ++pBegin; ++(p -> nSize);
    }
    else
    {
        while ( *pBegin )
        {
            if ( *pBegin == '<' )
            {
                return p;
            }
            if ( *pBegin == '\'' )
            {
                ++pBegin; ++(p -> nSize);
                while ( *pBegin != '\'' ) { ++pBegin; ++(p -> nSize); }
                ++pBegin; ++(p -> nSize);
                continue;
            }
            if ( *pBegin == '\"' )
            {
                ++pBegin; ++(p -> nSize);
                while ( *pBegin != '\"' ) { ++pBegin; ++(p -> nSize); }
                ++pBegin; ++(p -> nSize);
                continue;
            }
            ++pBegin; ++(p -> nSize);
        }

    }
    return p;
}

void rHtmlNodeSaveTo ( struct html_node *p, FILE *pF )
{
    static UINT z = 0;
    ++z;
    while ( p )
    {
        fwrite (
        "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"
        "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"
        "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"
        "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"
        "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"
        "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"
        "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"
        "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"
        "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"
        "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"
        "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"
        "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"
        "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"
        "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"
        "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"
        "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"
        "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t", 1, z, pF );
        fwrite ( p -> pBegin, 1, p -> nSize, pF );
        fwrite ( "\n", 1, 1, pF );
        rHtmlNodeSaveTo ( p -> pChild, pF );
        p = p -> pNext;
    }
    --z;
}

void rHtmlNodeFilter ( struct html_node *p )
{
    struct html_node *pLast = p;
    p = p -> pNext;
    p -> pPrev = pLast;
    while ( p )
    {
        /* if closed */
        if ( rCmpStr ( p -> pBegin, "<meta" ) ||
            rCmpStr ( p -> pBegin, "<link" ) ||
            rCmpStr ( p -> pBegin, "</script" ) )
        {
            p = p -> pNext;
            continue;
        }
        /* if can contain text */
        if ( rCmpStr ( p -> pBegin, "<script" ) )
        {
            p = p -> pNext;
            if ( *p -> pBegin != '<' ) { p = p -> pNext; }
            continue;
        }
        pLast -> pNext = p;
        p -> pPrev = pLast;
        pLast = p;
        p = p -> pNext;
    }
    pLast -> pNext = p;
    if ( p ) p -> pPrev = pLast;
}

void rHtmlNodeReSetIerarhy ( struct html_node *p )
{
    while ( p )
    {
        struct html_node * k = p -> pNext;
        if ( p -> pNext ) p -> pNext = p -> pPrev;
        p -> pPrev = k;
        p = k;
    }
}

void parse_file(LPCSTR szFileName)
{
    LPCSTR pMem = (LPCSTR)rLoadFile ( szFileName, NULL );
    LPCSTR p = pMem;

    if( ! rCmpStr ( p, "HTTP/1.1 200 OK" ) ) { assert(0); }
    p += 15;
    while ( ! rCmpStr ( p, "<!DOCTYPE html>" ) ) { ++p; }

    struct html_node root = {};
    root.pBegin = p;
    root.nSize = 15;
    struct html_node *pLast;
    pLast = &root;
    p += 15;
    while ( *p )
    {
        /* Start of tag */
        if ( *p == '<' )
        {
            struct html_node *pNew = rHtmlNodeCreate ( p );
            if ( pLast -> pBegin [ 0 ] == '<' && pLast -> pBegin [ 1 ] == '/' )
            {
                pLast = pLast -> pParent;
                pLast -> pNext = pNew;
                pLast -> pNext -> pPrev = pLast;
                pLast = pLast -> pNext;
            }
            else
            if ( pLast -> pBegin [ pLast -> nSize - 2 ] == '/' )
            {
            }
            else
            if ( isalpha ( p[1] ) )
            {
                if ( pLast -> pBegin [ 0 ] == '<' && isalpha ( pLast -> pBegin [ 1 ] ) )
                {
                    pLast -> pChild = pNew;
                    pLast -> pChild -> pParent = pLast;
                    pLast = pLast -> pChild;
                }
                else
                {
                    pLast -> pNext = pNew;
                    pLast -> pNext -> pPrev = pLast;
                    pLast = pLast -> pNext;
                }
            }
            p += pNew -> nSize;
            continue;

        }
        if ( *p == '\'' )
        {
            ++p;
            while ( *p != '\'' ) { ++p; }
            ++p;
            continue;
        }
        if ( *p == '\"' )
        {
            ++p;
            while ( *p != '\"' ) { ++p; }
            ++p;
            continue;
        }
        if ( ! isspace ( *p ) )
        {
            pLast -> pNext = rHtmlNodeCreate ( p );
            pLast = pLast -> pNext;
            p += pLast -> nSize;
            continue;
        }
        ++p;
    }

    // rHtmlNodeFilter ( &root );
    // rHtmlNodeReSetIerarhy ( &root );

    snprintf ( _buf, sizeof(_buf), "%s.html", szFileName);
    FILE *pF = fopen ( _buf, "wb" );
    rHtmlNodeSaveTo ( &root, pF );
    fclose ( pF );
    free ( (VOID*)pMem );
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

    get_https("docs.microsoft.com", "/en-us/windows/win32/apiindex/windows-api-list", "windows-api-list.log");

    get_https("docs.microsoft.com", "/en-us/windows/win32/winmsg/windowing", "windowing.log");



    SSL_CTX_free(ctx_client);

    return 0;
}




