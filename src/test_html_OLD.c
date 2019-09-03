
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include <windows.h>
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
  pMem [ nFileLenght ] = EOF;
  return pMem;
}

static FILE * pF_HtmlDebug = NULL;
#define HTML_PRINT(...) ( (VOID)( pF_HtmlDebug ? fprintf ( pF_HtmlDebug, __VA_ARGS__ ) : 0 ) )


struct html__node;
struct html__document;
struct html__doctype;
struct html__comment;
struct html__text;
struct html_html;
struct html_head;
struct html_body;

/// @html__node

  struct html__node
  {
    UINT iType;
    struct html__node
      *pChildFirst, *pChildLast,
      *pSibilingNext, *pSibilingPrev,
      *pParent;
    struct html__document *pOwner;
  };

  struct html__node * rHtmlNode_AppendChild ( struct html__node * const pThis,
    struct html__node * const pChildNew )
  {
    assert ( pThis != NULL );
    if ( pThis -> pChildLast == NULL ) {
      HTML_PRINT ( "[%p] add child [%p] first\n",  pThis, pChildNew );
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
      HTML_PRINT ( "[%p] add child [%p] first\n",  pThis, pChildNew );
      assert ( pThis -> pChildFirst != NULL );
      struct html__node *pChildLast = pThis -> pChildLast;
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

  struct html__node * rHtmlNode_RemoveChild ( struct html__node * const pThis,
    struct html__node * const pChildOld )
  {
    return pChildOld;
  };

  struct html__node * rHtmlNode_ReplaceChild ( struct html__node * const pThis,
    struct html__node * const pChildNew, struct html__node * const pChildOld )
  {
    return pChildOld;
  };

  struct html__node * rHtmlNode_InsertBefore ( struct html__node * const pThis,
    struct html__node * const pChildNew, struct html__node * const pChildRef )
  {
    return pChildNew;
  }

/// [https://www.w3.org/TR/html52/syntax.html#writing-html-documents-elements]

  enum
  {
    DNT__NULL = 0,
    DNT__DOCUMENT,
    DNT__DOCTYPE,
    DNT__COMMENT,
    DNT__TEXT,
    DNT__CDATA,

    DNT_HTML,
    DNT_HEAD,
    DNT_BODY,

    DNT___LAST,

    // DNT_HTML,
    // DNT_HEAD,
    // DNT_META,
    // DNT_BASE,
    // DNT_BASEFONT,
    // DNT_BGSOUND,
    // DNT_LINK,
    // DNT_TITLE,
    // DNT_SCRIPT,

    #if 0
      /* Void elements */
      DNT_AREA,
      DNT_BASE,
      DNT_BR,
      DNT_COL,
      DNT_EMBED,
      DNT_HR,
      DNT_IMG,
      DNT_INPUT,
      DNT_LINK,
      DNT_META,
      DNT_PARAM,
      DNT_SOURCE,
      DNT_TRACK,
      DNT_WBR,
      /* <template> */
      DNT_TEMPLATE,
      /* Raw text elements */
      DNT_SCRIPT,
      DNT_STYLE,
      /* Escapable raw text elements */
      DNT_TEXTAREA,
      DNT_TITLE,
      /* Normal elements */
    #endif
  };

  /// @html__proto_element
  struct html__proto_element
  {
    UINT iType;
    UINT nSize;
    PCSTR pName;
    UINT iFlags;
  };

  #define HTML_PROTO_SPECIAL(a,b) [iT] = { \
    .iType = a,\
    .pName = b,\
    .iFlags = 0,\
  }

  #define HTML_PROTO(a,b,c) [iT] = { \
    .iType = a,\
    .pName = b,\
    .iFlags = c,\
  }

  const struct html__proto_element _html_proto[] =
  {
    HTML_PROTO_SPECIAL ( DNT__NULL,     "#null" ),
    HTML_PROTO_SPECIAL ( DNT__DOCUMENT, "#document" ),
    HTML_PROTO_SPECIAL ( DNT__DOCTYPE,  "#doctype" ),
    HTML_PROTO_SPECIAL ( DNT__COMMENT,  "#comment" ),
    HTML_PROTO_SPECIAL ( DNT__TEXT,     "#text" ),
    HTML_PROTO_SPECIAL ( DNT__CDATA,    "#cdata" ),

    HTML_PROTO ( DNT_HTML, "html", 0 ),
    HTML_PROTO ( DNT_HEAD, "head", 0 ),
    HTML_PROTO ( DNT_BODY, "body", 0 ),
  };



/// @html__document

  struct html__document
  {
    struct html__node Node;
    PVOID pBuffer;
    PCSTR pDocType;

    struct html__doctype *pNodeDoctype;
    struct html_node *pNodeHead;
    struct html_node *pNodeBody;


    void (*rParserState)( struct html__document * const pThis );
    PSTR pParserStream;
    struct html__node *pParserLastNode;
  };

  struct html__node * rHtmlNode_NewNodeSz ( struct html__document * const pThis, const UINT nSize )
  {
    struct html__node * const pOut = (struct html__node*) malloc ( nSize );
    memset ( pOut, 0, nSize );
    pOut -> pOwner = pThis;
    HTML_PRINT ( "[%p] new node [%p] with size %u bytes\n", pThis, pOut, nSize );
    return pOut;
    // TODO: Link node mem to document
  }

  struct html__node * rHtmlNode_NewNode ( struct html__document * const pThis, const UINT iType )
  {
    assert ( iType < DNT___LAST );
    struct html__node * const pOut = rHtmlNode_NewNodeSz ( pThis, _html_proto[iType].nSize );
    pOut -> iType = iType;
    return pOut;
  };

  struct html__document * rHmtlDocument_New ( )
  {
    struct html__document * const pDocument = (struct html__document*) malloc ( sizeof(struct html__document) );
    memset ( pDocument, 0, sizeof(struct html__document) );
    pDocument -> Node.iType = DNT__DOCUMENT;
    return pDocument;
  }

/// @html__doctype

  struct html__doctype
  {
    struct html__node Node;
    PCSTR pBegin;
    UINT nSize;
  };

  struct html__doctype * rHtmlNode_NewDoctype ( struct html__document * pThis )
  {
    struct html__doctype * const pOut = (struct html__doctype*) rHtmlNode_NewNodeSz ( pThis, sizeof(struct html__doctype) );
    pOut -> Node.iType = DNT__DOCTYPE;
    return pOut;
  }

/// @html__comment

  struct html__comment
  {
    struct html__node Node;
    PCSTR pBegin;
    UINT nSize;
  };

  struct html__comment * rHtmlNode_NewComment ( struct html__document * pThis )
  {
    struct html__comment * const pOut = (struct html__comment*) rHtmlNode_NewNodeSz ( pThis, sizeof(struct html__comment) );
    pOut -> Node.iType = DNT__COMMENT;
    return pOut;
  }

/// @html__text

  struct html__text
  {
    struct html__node Node;
    PCSTR pBegin;
    UINT nSize;
  };

  struct html__text * rHtmlNode_NewText ( struct html__document * pThis )
  {
    struct html__text * const pOut = (struct html__text*) rHtmlNode_NewNodeSz ( pThis, sizeof(struct html__text) );
    pOut -> Node.iType = DNT__TEXT;
    return pOut;
  }

/// @html__cdata

  struct html__cdata
  {
    struct html__node Node;
    PCSTR pBegin;
    UINT nSize;
  };

  struct html__cdata * rHtmlNode_NewText ( struct html__document * pThis )
  {
    struct html__cdata * const pOut = (struct html__cdata*) rHtmlNode_NewNodeSz ( pThis, sizeof(struct html__cdata) );
    pOut -> Node.iType = DNT__CDATA;
    return pOut;
  }

/// https://www.w3.org/TR/html52/syntax.html#tokenization


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
  if ( rCmpStr ( p, "<!DOCTYPE " ) )
  {
    UINT o = 10;
    p += 10;
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
    while ( *p != '>' ) {
      if ( *p == '\"' )
      {
        while ( *p != '\"' )
        {
          ++p; ++o;
        }
      }
      else
      if ( *p == '\'' )
      {
        while ( *p != '\'' )
        {
          ++p; ++o;
        }
      }
      ++p; ++o;
    }
    return o;
  }
  return 0;
}


#define __HTML_TOKEN_COMMENT(o,p)\
  if ( ( o = rHtmlToken_Comment ( p ) ) )\
  {\
    struct html__comment * const pE = rHtmlNode_NewComment ( pDocument );\
    rHtmlNode_AppendChild ( pDocument -> pParserLastNode, &(pE -> Node) );\
    pE -> pBegin = p;\
    pE -> nSize = o;\
    p += o;\
    continue;\
  }
#define __HTML_TOKEN_ELEMENT_VOID(o,p,name,dnt)\
  if ( ( o = rCmpStrN ( p+1, name ) ) && ( !isalpha ( p[o+1] ) ) )\
  {\
    p += o + 1;\
    struct html_element * const pE = rHtmlNode_NewElement ( pDocument, dnt );\
    rHtmlNode_AppendChild ( pDocument -> pParserLastNode, &(pE -> Node) );\
    if ( ( o = rHtmlToken_Attr ( p ) ) )\
    {\
      pE -> pAttr = p+1;\
      p [ o ] = 0;\
      p += o + 1;\
    }\
    continue;\
  }
#define __HTML_TOKEN_ELEMENT_CONTAINER(o,p,name,dnt,next)\
  if ( ( o = rCmpStrN ( p+1, name ) ) && ( !isalpha ( p[o+1] ) ) )\
  {\
    p += o + 1;\
    struct html_element * const pE = rHtmlNode_NewElement ( pDocument, dnt );\
    rHtmlNode_AppendChild ( pDocument -> pParserLastNode, &(pE -> Node) );\
    pDocument -> pParserLastNode = &(pE -> Node);\
    if ( ( o = rHtmlToken_Attr ( p ) ) )\
    {\
      pE -> pAttr = p+1;\
      p [ o ] = 0;\
      p += o + 1;\
    }\
    pDocument -> rParserState = next;\
    pDocument -> pParserStream = p;\
    return;\
  }

void rHtmlParserState_InHead ( struct html__document * const pDocument )
{
  PSTR p = pDocument -> pParserStream;
  UINT o;
  while ( *p )
  {
    if ( *p == '<' )
    {
      __HTML_TOKEN_COMMENT(o,p)
      // "meta", "base", "basefont", "bgsound", "link"
      __HTML_TOKEN_ELEMENT_VOID(o,p,"meta",DNT_META)
      __HTML_TOKEN_ELEMENT_VOID(o,p,"base",DNT_BASE)
      __HTML_TOKEN_ELEMENT_VOID(o,p,"basefont",DNT_BASEFONT)
      __HTML_TOKEN_ELEMENT_VOID(o,p,"bgsound",DNT_BGSOUND)
      __HTML_TOKEN_ELEMENT_VOID(o,p,"link",DNT_LINK)

      if ( ( o = rCmpStrN ( p+1, "title" ) ) && ( !isalpha ( p[o+1] ) ) )
      {
        p += o + 1;
        struct html_element * const pE = rHtmlNode_NewElement ( pDocument, DNT_TITLE );
        rHtmlNode_AppendChild ( pDocument -> pParserLastNode, &(pE -> Node) );
        if ( ( o = rHtmlToken_Attr ( p ) ) )
        {
          pE -> pAttr = p+1;
          p [ o ] = 0;
          p += o + 1;
        }
        else
        {
          *p = 0;
          ++p;
        }
        struct html__comment * const pT = rHtmlNode_NewComment ( pDocument );
        pT -> Node.iType = DNT__TEXT;
        rHtmlNode_AppendChild ( &(pE -> Node), &(pT -> Node) );
        pT -> pBegin = p;
        o = 0;
        while ( *p !='<' ) { ++o; ++p; }
        pT -> nSize = o;
        if ( ( o = rCmpStrN ( p+1, "/title" ) ) && ( !isalpha ( p[o+1] ) ) )
        {
          *p = 0;
          p += o + 1;
        }
        continue;
      }

      if ( ( o = rCmpStrN ( p+1, "script" ) ) && ( !isalpha ( p[o+1] ) ) )
      {
        p += o + 1;
        struct html__comment * const pE = rHtmlNode_NewComment ( pDocument );
        rHtmlNode_AppendChild ( pDocument -> pParserLastNode, &(pE -> Node) );
        if ( ( o = rHtmlToken_Attr ( p ) ) )
        {
          // pE -> pAttr = p+1;
          p [ o ] = 0;
          p += o;
        }
        ++p;
        pE -> Node.iType = DNT_SCRIPT;
        pE -> pBegin = p;
        o = 0;
        while ( *p !='<' )
        {
          if ( *p == '\'' )
          {
            ++o; ++p;
            while ( *p != '\'' ) { ++o; ++p; }
          }
          if ( *p == '\"' )
          {
            ++o; ++p;
            while ( *p != '\"' ) { ++o; ++p; }
          }
          ++o; ++p;
        }
        pE -> nSize = o;
        if ( ( o = rCmpStrN ( p+1, "/script" ) ) && ( !isalpha ( p[o+1] ) ) )
        {
          *p = 0;
          p += o + 1;
        }
        continue;
      }


      if ( ( o = rCmpStrN ( p+1, "/head" ) ) && ( !isalpha ( p[o+1] ) ) )
      {
        *p = 0;
        p += o+1;
        pDocument -> pParserLastNode = pDocument -> pParserLastNode -> pParent;
        pDocument -> rParserState = NULL;
        pDocument -> pParserStream = p;
        return;
      }

      printf ( "ERROR can't search [/HEAD] %.16s\n", p );
      pDocument -> rParserState = NULL;
      pDocument -> pParserStream = NULL;
      return;
    }
    ++p;
  }
}

void rHtmlParserState_BeforeHead ( struct html__document * const pDocument )
{
  PSTR p = pDocument -> pParserStream;
  UINT o;
  while ( *p )
  {
    if ( *p == '<' )
    {
      __HTML_TOKEN_COMMENT(o,p)
      __HTML_TOKEN_ELEMENT_CONTAINER(o,p,"head",DNT_HEAD,rHtmlParserState_InHead)
      printf ( "ERROR can't search [HEAD] %.16s\n", p );
      pDocument -> rParserState = NULL;
      pDocument -> pParserStream = NULL;
      return;
    }
    ++p;
  }
}

void rHtmlParserState_BeforeHtml ( struct html__document * const pDocument )
{
  PSTR p = pDocument -> pParserStream;
  UINT o;
  while ( *p )
  {
    if ( *p == '<' )
    {
      __HTML_TOKEN_COMMENT(o,p)
      __HTML_TOKEN_ELEMENT_CONTAINER(o,p,"html",DNT_HTML,rHtmlParserState_BeforeHead)
      printf ( "ERROR can't search [HTML] %.16s\n", p );
      pDocument -> rParserState = NULL;
      pDocument -> pParserStream = NULL;
      return;
    }
    ++p;
  }
}


struct html__document * rHtmlParser_OpenFile ( const PCSTR szFileName )
{
  struct html__document * const pDocument = (struct html__document*) malloc ( sizeof(struct html__document) );
  memset ( pDocument, 0, sizeof(struct html__document) );
  pDocument -> Node.iType = DNT__DOCUMENT;
  PSTR p = pDocument -> pParserStream = (PSTR) ( pDocument -> pBuffer = rLoadFile ( szFileName, NULL ) );

  printf ( "Created html__document and parse by file \"%s\"\n", szFileName );

  UINT o;
  pDocument -> pParserLastNode = &(pDocument -> Node);
  while ( *p )
  {
    if ( *p == '<' )
    {
      __HTML_TOKEN_COMMENT(o,p)
      if ( ( o = rHtmlToken_Doctype ( p ) ) )
      {
        pDocument -> pDocType = p+2;
        p [ o-1 ] = 0;
        pDocument -> rParserState = rHtmlParserState_BeforeHtml;
        pDocument -> pParserStream = p + o;
        break;
      }
      printf ( "ERROR can't search [!DOCTYPE]\n" );
      pDocument -> rParserState = NULL;
      pDocument -> pParserStream = NULL;
      break;
    }
    ++p;
  }

  while ( pDocument -> rParserState ) { pDocument -> rParserState ( pDocument ); }

  return pDocument;
}

void rHtmlNode_SaveToFD ( struct html__node const * const pThis, FILE * const pF )
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
  // printf ( "HTML_PRINT (depth = %d) [%p] type (%d)\n", z, pThis, pThis -> iType );
  switch ( pThis -> iType )
  {
    case DNT__COMMENT:
    {
      struct html__comment const * const pE = (struct html__comment const * const) pThis;
      rZZ();
      fwrite ( pE -> pBegin, 1, pE -> nSize, pF );
      fwrite ( "\n", 1, 1, pF );
      break;
    }
    case DNT__DOCUMENT:
    {
      struct html__document const * const pE = (struct html__document const * const) pThis;
      rZZ();
      fprintf ( pF, "<!%s>\n", pE -> pDocType );
      break;
    }
    case DNT__TEXT:
    {
      struct html__comment const * const pE = (struct html__comment const * const) pThis;
      rZZ();
      fprintf ( pF, "#TEXT: " );
      fwrite ( pE -> pBegin, 1, pE -> nSize, pF );
      fwrite ( "\n", 1, 1, pF );
      break;
    }
    case DNT_SCRIPT:
    {
      struct html__comment const * const pE = (struct html__comment const * const) pThis;
      rZZ();
      fprintf ( pF, "<script></script>\n" );
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
    case DNT_TITLE:
    {
      struct html_element const * const pE = (struct html_element const * const) pThis;
      rZZ();
      fprintf ( pF, pE -> pAttr ? "<title %s>\n" : "<title>\n", pE -> pAttr );
      break;
    }
      #define __HTML_PRINT_ELEMENT_VOID(name,dnt)\
        case dnt:\
        {\
          struct html_element const * const pE = (struct html_element const * const) pThis;\
          rZZ();\
          fprintf ( pF, pE -> pAttr ? "<" name " %s>\n" : "<" name ">\n", pE -> pAttr );\
          break;\
        }
    __HTML_PRINT_ELEMENT_VOID("meta",DNT_META)
    __HTML_PRINT_ELEMENT_VOID("base",DNT_BASE)
    __HTML_PRINT_ELEMENT_VOID("basefont",DNT_BASEFONT)
    __HTML_PRINT_ELEMENT_VOID("bgsound",DNT_BGSOUND)
    __HTML_PRINT_ELEMENT_VOID("link",DNT_LINK)

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

void rHtmlNode_SaveToFile ( struct html__document const * const pThis, const PCSTR szFileName )
{
  FILE * const pF = fopen ( szFileName, "wb" );
  rHtmlNode_SaveToFD ( &(pThis -> Node), pF );
  fclose ( pF );
}




int main ( int argc, char const * argv [] )
{
  rHtmlNode_SaveToFile ( rHtmlParser_OpenFile ( argv[1] ), argv[2] );
  return 0;
}
