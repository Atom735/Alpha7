/* [https://www.w3.org/TR/html52/syntax.html] */

#define D_CMP_CHAR(a,b) (((a)==(b))||(((a)>='A')&&((a)<='Z')&&((a)^(b))==(0x20)))

BOOL rCmpStr ( PCSTR pSrc, PCSTR pDst )
{
    while ( D_CMP_CHAR ( *pSrc, *pDst ) ) { ++pSrc; ++pDst; }
    if ( *pDst == 0 ) return TRUE;
    return FALSE;
}

UINT rCmpStrN ( PCSTR pSrc, PCSTR pDst )
{
    UINT n = 0;
    while ( D_CMP_CHAR ( *pSrc, *pDst ) ) { ++pSrc; ++pDst; ++n; }
    if ( *pDst == 0 ) return n;
    return 0;
}

PVOID rLoadFile ( const PCSTR szFileName, const PUINT pLen )
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

struct html_document;

struct html_node {
    UINT iType;
    struct html_node
        *pChildFirst, *pChildLast,
        *pSibilingNext, *pSibilingPrev,
        *pParent;
    struct html_document *pOwner;
};

struct html_node * rHtmlNode_AppendChild ( struct html_node * const pThis,
    struct html_node * const pChildNew )
{
    assert ( pThis != NULL );
    if ( pThis -> pChildLast == NULL ) {

        printf ( "HTML [%p] add first Child [%p]\n", pThis, pChildNew );
        assert ( pThis -> pChildFirst == NULL );
        pThis -> pChildFirst = pThis -> pChildLast = pChildNew;
        assert ( pChildNew -> pParent == NULL );
        pChildNew -> pParent = pThis;
        assert ( pChildNew -> pSibilingNext == NULL );
        assert ( pChildNew -> pSibilingPrev == NULL );
        pChildNew -> pSibilingPrev = pChildNew -> pSibilingNext = NULL;
    }
    else
    {
        printf ( "HTML [%p] add Child [%p]\n", pThis, pChildNew );
        assert ( pThis -> pChildFirst != NULL );
        struct html_node *pChildLast = pThis -> pChildLast;
        pThis -> pChildLast = pChildNew;
        assert ( pChildNew -> pParent == NULL );
        pChildNew -> pParent = pThis;
        assert ( pChildNew -> pSibilingNext == NULL );
        pChildNew -> pSibilingNext = NULL;
        assert ( pChildNew -> pSibilingPrev == NULL );
        pChildNew -> pSibilingPrev = pChildLast;
        assert ( pChildLast -> pSibilingNext == NULL );
        pChildLast -> pSibilingNext = pChildNew;
    }
    return pChildNew;
}

struct html_node * rHtmlNode_RemoveChild ( struct html_node * const pThis,
    struct html_node * const pChildOld )
{
    return pChildOld;
};

struct html_node * rHtmlNode_ReplaceChild ( struct html_node * const pThis,
    struct html_node * const pChildNew, struct html_node * const pChildOld )
{
    return pChildOld;
};

struct html_node * rHtmlNode_InsertBefore ( struct html_node * const pThis,
    struct html_node * const pChildNew, struct html_node * const pChildRef )
{
    return pChildNew;
}

enum
{
    DNT_NUL = 0,
    DNT_COMMENT,
    DNT_DOCUMENT,
    DNT_HTML,
    DNT_HEAD,
    DNT_META,
};

struct html_document
{
    struct html_node Node;
    PVOID pBuffer;
    PCSTR pDocType;
    void (*rParserState)( struct html_document * const pThis );
    PSTR pParserStream;
    struct html_node *pParserLastNode;
};

struct html_comment
{
    struct html_node Node;
    PCSTR pBegin;
    UINT nSize;
};


struct html_comment * rHtmlNode_NewComment ( struct html_document * pThis )
{
    struct html_comment * const pOut = (struct html_comment*) malloc ( sizeof(struct html_comment) );
    memset ( pOut, 0, sizeof(struct html_comment) );
    pOut -> Node.iType = DNT_COMMENT;
    pOut -> Node.pOwner = pThis;
    return pOut;
}

struct html_element
{
    struct html_node Node;
    PCSTR pAttr;
};

struct html_element * rHtmlNode_NewElement ( struct html_document * pThis, UINT iType )
{
    struct html_element * const pOut = (struct html_element*) malloc ( sizeof(struct html_element) );
    memset ( pOut, 0, sizeof(struct html_element) );
    pOut -> Node.iType = iType;
    pOut -> Node.pOwner = pThis;
    return pOut;
}

UINT rHtmlToken_Comment ( PCSTR p )
{
    if ( rCmpStr ( p, "<!--" ) )
    {
        UINT o = 4;
        p += 4;
        while ( *p )
        {
            if ( rCmpStr ( p, "-->" ) ) { return o+3; }
            ++p; ++o;
        }
        assert ( 0 );
    }
    return 0;
}

UINT rHtmlToken_Doctype ( PCSTR p )
{
    if ( rCmpStr ( p, "<!DOCTYPE" ) )
    {
        UINT o = 9;
        p += 9;
        while ( *p )
        {
            if ( rCmpStr ( p, ">" ) ) { return o+1; }
            ++p; ++o;
        }
        assert ( 0 );
    }
    return 0;
}

UINT rHtmlToken_Attr ( PCSTR p )
{
    if ( *p == ' ' )
    {
        ++p;
        UINT o = 1;
        while ( *p != '>' ) { ++p; ++o; }
        return o;
    }
    return 0;
}


void rHtmlParserState_InHead ( struct html_document * const pDocument )
{
    PSTR p = pDocument -> pParserStream;
    UINT o;
    while ( *p )
    {
        if ( *p == '<' )
        {
            if ( ( o = rHtmlToken_Comment ( p ) ) )
            {
                printf ( "HTML [%p] searched html_comment (%d)\n", pDocument, o );
                struct html_comment * const pE = rHtmlNode_NewComment ( pDocument );
                rHtmlNode_AppendChild ( pDocument -> pParserLastNode, &(pE -> Node) );
                pE -> pBegin = p;
                pE -> nSize = o;
                p += o;
                continue;
            }
            if ( ( o = rCmpStrN ( p+1, "meta" ) ) && ( !isalpha ( p[o+1] ) ) )
            {
                printf ( "HTML [%p] searched html_element [META]\n", pDocument );
                p += o + 1;
                struct html_element * const pE = rHtmlNode_NewElement ( pDocument, DNT_META );
                rHtmlNode_AppendChild ( pDocument -> pParserLastNode, &(pE -> Node) );
                if ( ( o = rHtmlToken_Attr ( p ) ) )
                {
                    pE -> pAttr = p+1;
                    p [ o ] = 0;
                    p += o + 1;
                }
                continue;
            }
            printf ( "HTML [%p] ERROR can't search html_element [/HEAD]\n", pDocument );
            pDocument -> rParserState = NULL;
            pDocument -> pParserStream = NULL;
            return;
        }
        ++p;
    }
}

void rHtmlParserState_BeforeHead ( struct html_document * const pDocument )
{
    PSTR p = pDocument -> pParserStream;
    UINT o;
    while ( *p )
    {
        if ( *p == '<' )
        {
            if ( ( o = rHtmlToken_Comment ( p ) ) )
            {
                printf ( "HTML [%p] searched html_comment (%d)\n", pDocument, o );
                struct html_comment * const pE = rHtmlNode_NewComment ( pDocument );
                rHtmlNode_AppendChild ( pDocument -> pParserLastNode, &(pE -> Node) );
                pE -> pBegin = p;
                pE -> nSize = o;
                p += o;
                continue;
            }
            if ( ( o = rCmpStrN ( p+1, "head" ) ) && ( !isalpha ( p[o+1] ) ) )
            {
                printf ( "HTML [%p] searched html_element [HEAD]\n", pDocument );
                p += o + 1;
                struct html_element * const pE = rHtmlNode_NewElement ( pDocument, DNT_HEAD );
                rHtmlNode_AppendChild ( pDocument -> pParserLastNode, &(pE -> Node) );
                pDocument -> pParserLastNode = &(pE -> Node);
                if ( ( o = rHtmlToken_Attr ( p ) ) )
                {
                    pE -> pAttr = p+1;
                    p [ o ] = 0;
                    p += o + 1;
                }
                pDocument -> rParserState = rHtmlParserState_InHead;
                pDocument -> pParserStream = p;
                return;
            }
            printf ( "HTML [%p] ERROR can't search html_element [HEAD]\n", pDocument );
            pDocument -> rParserState = NULL;
            pDocument -> pParserStream = NULL;
            return;
        }
        ++p;
    }
}

void rHtmlParserState_BeforeHtml ( struct html_document * const pDocument )
{
    PSTR p = pDocument -> pParserStream;
    UINT o;
    while ( *p )
    {
        if ( *p == '<' )
        {
            if ( ( o = rHtmlToken_Comment ( p ) ) )
            {
                printf ( "HTML [%p] searched html_comment (%d)\n", pDocument, o );
                struct html_comment * const pE = rHtmlNode_NewComment ( pDocument );
                rHtmlNode_AppendChild ( &(pDocument -> Node), &(pE -> Node) );
                pE -> pBegin = p;
                pE -> nSize = o;
                p += o;
                continue;
            }
            if ( ( o = rCmpStrN ( p+1, "html" ) ) && ( !isalpha ( p[o+1] ) ) )
            {
                printf ( "HTML [%p] searched html_element [HTML]\n", pDocument );
                p += o + 1;
                struct html_element * const pE = rHtmlNode_NewElement ( pDocument, DNT_HTML );
                rHtmlNode_AppendChild ( &(pDocument -> Node), &(pE -> Node) );
                pDocument -> pParserLastNode = &(pE -> Node);
                if ( ( o = rHtmlToken_Attr ( p ) ) )
                {
                    pE -> pAttr = p+1;
                    p [ o ] = 0;
                    p += o + 1;
                }
                pDocument -> rParserState = rHtmlParserState_BeforeHead;
                pDocument -> pParserStream = p;
                return;
            }
            printf ( "HTML [%p] ERROR can't search html_element [HTML]\n", pDocument );
            pDocument -> rParserState = NULL;
            pDocument -> pParserStream = NULL;
            return;
        }
        ++p;
    }
}


struct html_document * rHtmlParser_OpenFile ( const PCSTR szFileName )
{
    struct html_document * const pDocument = (struct html_document*) malloc ( sizeof(struct html_document) );
    memset ( pDocument, 0, sizeof(struct html_document) );
    pDocument -> Node.iType = DNT_DOCUMENT;
    PSTR p = pDocument -> pParserStream = (PSTR) ( pDocument -> pBuffer = rLoadFile ( szFileName, NULL ) );

    printf ( "HTML [%p] Created html_document and parse by file \"%s\"\n", pDocument, szFileName );

    UINT o;
    while ( *p )
    {
        if ( *p == '<' )
        {
            if ( ( o = rHtmlToken_Comment ( p ) ) )
            {
                printf ( "HTML [%p] searched html_comment (%d)\n", pDocument, o );
                struct html_comment * const pE = rHtmlNode_NewComment ( pDocument );
                rHtmlNode_AppendChild ( &(pDocument -> Node), &(pE -> Node) );
                pE -> pBegin = p;
                pE -> nSize = o;
                p += o;
                continue;
            }
            if ( ( o = rHtmlToken_Doctype ( p ) ) )
            {
                printf ( "HTML [%p] searched [!DOCTYPE] (%d)\n", pDocument, o );
                pDocument -> pDocType = p+2;
                p [ o-1 ] = 0;
                pDocument -> rParserState = rHtmlParserState_BeforeHtml;
                pDocument -> pParserStream = p + o;
                break;
            }
            printf ( "HTML [%p] ERROR can't search [!DOCTYPE]\n", pDocument );
            pDocument -> rParserState = NULL;
            pDocument -> pParserStream = NULL;
            break;
        }
        ++p;
    }

    while ( pDocument -> rParserState ) { pDocument -> rParserState ( pDocument ); }

    return pDocument;
}

void rHtmlNode_SaveToFD ( struct html_node const * const pThis, FILE * const pF )
{
    static UINT z = 0;
    void rZZ()
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
    }
    printf ( "HTML_PRINT (depth = %d) [%p] type (%d)\n", z, pThis, pThis -> iType );
    switch ( pThis -> iType )
    {
        case DNT_COMMENT:
        {
            struct html_comment const * const pE = (struct html_comment const * const) pThis;
            rZZ();
            fwrite ( pE -> pBegin, 1, pE -> nSize, pF );
            fwrite ( "\n", 1, 1, pF );
            break;
        }
        case DNT_DOCUMENT:
        {
            struct html_document const * const pE = (struct html_document const * const) pThis;
            rZZ();
            fprintf ( pF, "<!%s>\n", pE -> pDocType );
            break;
        }
        case DNT_HTML:
        {
            struct html_element const * const pE = (struct html_element const * const) pThis;
            rZZ();
            fprintf ( pF, pE -> pAttr ? "<html %s>\n" : "<html>\n", pE -> pAttr );
            break;
        }
        case DNT_HEAD:
        {
            struct html_element const * const pE = (struct html_element const * const) pThis;
            rZZ();
            fprintf ( pF, pE -> pAttr ? "<head %s>\n" : "<head>\n", pE -> pAttr );
            break;
        }
        case DNT_META:
        {
            struct html_element const * const pE = (struct html_element const * const) pThis;
            rZZ();
            fprintf ( pF, pE -> pAttr ? "<meta %s>\n" : "<meta>\n", pE -> pAttr );
            break;
        }
    }
    if ( pThis -> pChildFirst )
    {
        ++z;
        rHtmlNode_SaveToFD ( pThis -> pChildFirst, pF );
        --z;
    }
    if ( pThis -> pSibilingNext )
    {
        rHtmlNode_SaveToFD ( pThis -> pSibilingNext, pF );
    }
}

void rHtmlNode_SaveToFile ( struct html_document const * const pThis, const PCSTR szFileName )
{
    FILE * const pF = fopen ( szFileName, "wb" );
    rHtmlNode_SaveToFD ( &(pThis -> Node), pF );
    fclose ( pF );
}

