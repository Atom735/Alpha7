
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
  PBYTE pMem = (PBYTE) malloc ( nFileLenght+1 );
  UINT nReaded = 0;
  while ( nReaded < nFileLenght ) {
    nReaded += fread ( pMem+nReaded, 1, nFileLenght-nReaded, pF );
  }
  fclose ( pF );
  if ( pLen ) *pLen = nFileLenght;
  pMem [ nFileLenght ] = EOF;
  return pMem;
}

FILE * pF_HtmlDebug = NULL;
BOOL gb_HtmlVerb = FALSE;
#define HTML_PRINT(...) ( (VOID)( pF_HtmlDebug ? fprintf ( pF_HtmlDebug, __VA_ARGS__ ) : 0 ) )
#define HTML_PVERB(...) ( (VOID)( (pF_HtmlDebug) && (gb_HtmlVerb) ? fprintf ( pF_HtmlDebug, __VA_ARGS__ ) : 0 ) )


struct html__node;
struct html__document;
struct html__doctype;
struct html__comment;
struct html__text;

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

    DNT_BASEFONT,
    DNT_BGSOUND,
    DNT_TITLE,
    DNT_NOSCRIPT,
    DNT_NOFRAMES,
    DNT_STYLE,
    DNT_SCRIPT,

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

/// @html__node

  struct html__node
  {
    UINT iType;
    struct html__node
      *pChildFirst, *pChildLast,
      *pSibilingNext, *pSibilingPrev,
      *pParent;
    struct html__document *pOwner;
    PBYTE pAttribs;
  };

/// @html__proto_element
  struct html__proto_element
  {
    PCSTR pName;
    UINT nNameSize;
    UINT nSize;
  };

  #define HTML_PROTO_SPECIAL(a,b) [a] = { \
    .pName = b,\
    .nNameSize = sizeof(b),\
    .nSize = sizeof(struct html__node),\
  }

  #define HTML_PROTO(a,b) [a] = { \
    .pName = b,\
    .nNameSize = sizeof(b),\
    .nSize = sizeof(struct html__node),\
  }

  const struct html__proto_element _html_proto[] =
  {
    HTML_PROTO_SPECIAL ( DNT__NULL,     "#null" ),
    HTML_PROTO_SPECIAL ( DNT__DOCUMENT, "#document" ),
    HTML_PROTO_SPECIAL ( DNT__DOCTYPE,  "#doctype" ),
    HTML_PROTO_SPECIAL ( DNT__COMMENT,  "#comment" ),
    HTML_PROTO_SPECIAL ( DNT__TEXT,     "#text" ),
    HTML_PROTO_SPECIAL ( DNT__CDATA,    "#cdata" ),

    HTML_PROTO ( DNT_HTML, "html" ),
    HTML_PROTO ( DNT_HEAD, "head" ),
    HTML_PROTO ( DNT_BODY, "body" ),
    HTML_PROTO ( DNT_BR, "br" ),

    HTML_PROTO ( DNT_AREA, "area" ),
    HTML_PROTO ( DNT_BASE, "base" ),
    HTML_PROTO ( DNT_BR, "br" ),
    HTML_PROTO ( DNT_COL, "col" ),
    HTML_PROTO ( DNT_EMBED, "embed" ),
    HTML_PROTO ( DNT_HR, "hr" ),
    HTML_PROTO ( DNT_IMG, "img" ),
    HTML_PROTO ( DNT_INPUT, "input" ),
    HTML_PROTO ( DNT_LINK, "link" ),
    HTML_PROTO ( DNT_META, "meta" ),
    HTML_PROTO ( DNT_PARAM, "param" ),
    HTML_PROTO ( DNT_SOURCE, "source" ),
    HTML_PROTO ( DNT_TRACK, "track" ),
    HTML_PROTO ( DNT_WBR, "wbr" ),

    HTML_PROTO ( DNT_BASEFONT, "basefont" ),
    HTML_PROTO ( DNT_BGSOUND, "bgsound" ),
    HTML_PROTO ( DNT_TITLE, "title" ),
    HTML_PROTO ( DNT_NOSCRIPT, "noscript" ),
    HTML_PROTO ( DNT_NOFRAMES, "noframes" ),
    HTML_PROTO ( DNT_STYLE, "style" ),
    HTML_PROTO ( DNT_SCRIPT, "script" ),
  };

  UINT rHtmlNode_GetTypeByName ( const void *pSrc )
  {
    for ( UINT i = 0; i < DNT___LAST; ++i )
    {
      if ( memcmp ( pSrc, _html_proto[i].pName, _html_proto[i].nNameSize ) == 0 )
      {
        return i;
      }
    }
    return 0;
  }


  struct html__node * rHtmlNode_AppendChild ( struct html__node * const pThis,
    struct html__node * const pChildNew )
  {
    assert ( pThis != NULL );
    if ( pThis -> pChildLast == NULL ) {
      HTML_PRINT ( "[%p] add child [%p] first\n",  pThis, pChildNew );
      HTML_PRINT ( "[%p]<%-9.9s>[%p]<%-9.9s>\n",
        pThis, _html_proto[pThis->iType].pName,
        pChildNew, _html_proto[pChildNew->iType].pName );
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
      HTML_PRINT ( "[%p] add child [%p]\n",  pThis, pChildNew );
      HTML_PRINT ( "[%p]<%-9.9s>[%p]<%-9.9s>\n",
        pThis, _html_proto[pThis->iType].pName,
        pChildNew, _html_proto[pChildNew->iType].pName );
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




/// @html__document

  struct html__document
  {
    struct html__node Node;
    PVOID pBuffer;
    PCSTR pDocType;
    BOOL bNoScript;

    struct html__doctype *pNodeDoctype;
    struct html__node *pNodeHead;
    struct html__node *pNodeBody;


    void (*rParserMode)( struct html__document * const pThis, const UINT iTokenType, const PBYTE pData );
    void (*rParserModeOriginal)( struct html__document * const pThis, const UINT iTokenType, const PBYTE pData );
    BOOL bParserError;
    PBYTE pParserStream;
    struct html__node *pParserLastNode;
  };

  struct html__node * rHtmlNode_NewNodeSz ( struct html__document * const pThis, const UINT nSize )
  {
    struct html__node * const pOut = (struct html__node*) malloc ( nSize );
    memset ( pOut, 0, nSize );
    pOut -> pOwner = pThis;
    HTML_PRINT ( "[%p] new node  [%p] with size %u bytes\n", pThis, pOut, nSize );
    return pOut;
    // TODO: Link node mem to document
  }

  struct html__node * rHtmlNode_NewNode ( struct html__document * const pThis, const UINT iType )
  {
    assert ( iType < DNT___LAST );
    struct html__node * const pOut = rHtmlNode_NewNodeSz ( pThis, _html_proto[iType].nSize );
    HTML_PRINT ( "[%p]<%-9.9s>[%p]<%-9.9s>\n", pThis, _html_proto[pThis->Node.iType].pName, pOut, _html_proto[iType].pName );
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
    PBYTE pBegin;
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
    PBYTE pBegin;
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
    PBYTE pBegin;
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
    PBYTE pBegin;
  };

  struct html__cdata * rHtmlNode_NewCdata ( struct html__document * pThis )
  {
    struct html__cdata * const pOut = (struct html__cdata*) rHtmlNode_NewNodeSz ( pThis, sizeof(struct html__cdata) );
    pOut -> Node.iType = DNT__CDATA;
    return pOut;
  }

/// https://www.w3.org/TR/html52/syntax.html#tokenization

  #define _EOF 0x80

  enum
  {
    HTT_DOCTYPE = 0x80000000,
    HTT_start_tag,
    HTT_end_tag,
    HTT_comment,
    HTT_end_of_file,
  };

  struct html__token_tag
  {
    PBYTE pName;
    BOOL bSelfClosing;
    PBYTE pAttribs;
  };

  void rHtmlParserMode_initial ( struct html__document * const pThis, const UINT iTokenType, const PBYTE pData );
  void rHtmlParserMode_before_html ( struct html__document * const pThis, const UINT iTokenType, const PBYTE pData );
  void rHtmlParserMode_before_head ( struct html__document * const pThis, const UINT iTokenType, const PBYTE pData );
  void rHtmlParserMode_in_head ( struct html__document * const pThis, const UINT iTokenType, const PBYTE pData );
  void rHtmlParserMode_in_head_noscript ( struct html__document * const pThis, const UINT iTokenType, const PBYTE pData );
  void rHtmlParserMode_after_head ( struct html__document * const pThis, const UINT iTokenType, const PBYTE pData );
  void rHtmlParserMode_in_body ( struct html__document * const pThis, const UINT iTokenType, const PBYTE pData );
  void rHtmlParserMode_text ( struct html__document * const pThis, const UINT iTokenType, const PBYTE pData );
  void rHtmlParserMode_in_table ( struct html__document * const pThis, const UINT iTokenType, const PBYTE pData );
  void rHtmlParserMode_in_table_text ( struct html__document * const pThis, const UINT iTokenType, const PBYTE pData );
  void rHtmlParserMode_in_caption ( struct html__document * const pThis, const UINT iTokenType, const PBYTE pData );
  void rHtmlParserMode_in_column_group ( struct html__document * const pThis, const UINT iTokenType, const PBYTE pData );
  void rHtmlParserMode_in_table_body ( struct html__document * const pThis, const UINT iTokenType, const PBYTE pData );
  void rHtmlParserMode_in_row ( struct html__document * const pThis, const UINT iTokenType, const PBYTE pData );
  void rHtmlParserMode_in_cell ( struct html__document * const pThis, const UINT iTokenType, const PBYTE pData );
  void rHtmlParserMode_in_select ( struct html__document * const pThis, const UINT iTokenType, const PBYTE pData );
  void rHtmlParserMode_in_select_in_table ( struct html__document * const pThis, const UINT iTokenType, const PBYTE pData );
  void rHtmlParserMode_in_template ( struct html__document * const pThis, const UINT iTokenType, const PBYTE pData );
  void rHtmlParserMode_after_body ( struct html__document * const pThis, const UINT iTokenType, const PBYTE pData );
  void rHtmlParserMode_in_frameset ( struct html__document * const pThis, const UINT iTokenType, const PBYTE pData );
  void rHtmlParserMode_after_frameset ( struct html__document * const pThis, const UINT iTokenType, const PBYTE pData );
  void rHtmlParserMode_after_after_body ( struct html__document * const pThis, const UINT iTokenType, const PBYTE pData );
  void rHtmlParserMode_after_after_frameset ( struct html__document * const pThis, const UINT iTokenType, const PBYTE pData );

  void rHtmlParserMode_initial ( struct html__document * const pThis, const UINT iTokenType, const PBYTE pData )
  {
    switch ( iTokenType )
    {
      case 0x09 : /* TAB */
      case 0x0A : /* LF */
      case 0X0C : /* FF */
      case 0x0D : /* CR */
      case 0X20 : /* SPACE */
        return;
      case HTT_comment :
      {
        struct html__comment *pE = rHtmlNode_NewComment ( pThis );
        pE -> pBegin = pData;
        rHtmlNode_AppendChild ( &(pThis -> Node), &(pE -> Node) );
        return;
      }
      case HTT_DOCTYPE :
      {
        struct html__doctype *pE = rHtmlNode_NewDoctype ( pThis );
        pE -> pBegin = pData;
        rHtmlNode_AppendChild ( &(pThis -> Node), &(pE -> Node) );
        pThis -> rParserMode = rHtmlParserMode_before_html;
        return;
      }
      default :
        pThis -> bParserError = __LINE__;
        return;
    }
  }

  void rHtmlParserMode_before_html ( struct html__document * const pThis, const UINT iTokenType, const PBYTE pData )
  {
    switch ( iTokenType )
    {
      case 0x09 : /* TAB */
      case 0x0A : /* LF */
      case 0X0C : /* FF */
      case 0x0D : /* CR */
      case 0X20 : /* SPACE */
        return;
      case HTT_comment :
      {
        struct html__comment *pE = rHtmlNode_NewComment ( pThis );
        pE -> pBegin = pData;
        rHtmlNode_AppendChild ( &(pThis -> Node), &(pE -> Node) );
        return;
      }
      case HTT_DOCTYPE :
        pThis -> bParserError = __LINE__;
        return;
      case HTT_start_tag :
      {
        const struct html__token_tag *pToken = (const struct html__token_tag *)pData;
        UINT iT = rHtmlNode_GetTypeByName ( pToken -> pName );
        switch ( iT )
        {
          case DNT_HTML :
          {
            struct html__node *pE = rHtmlNode_NewNode ( pThis, iT );
            rHtmlNode_AppendChild ( &(pThis -> Node), pE );
            pE -> pAttribs = pToken -> pAttribs;
            pThis -> pParserLastNode = pE;
            pThis -> rParserMode = rHtmlParserMode_before_head;
            return;
          }
          default :
            goto P_Anything;
        }
      }
      case HTT_end_tag :
      {
        const struct html__token_tag *pToken = (const struct html__token_tag *)pData;
        UINT iT = rHtmlNode_GetTypeByName ( pToken -> pName );
        switch ( iT )
        {
          case DNT_HEAD :
          case DNT_BODY :
          case DNT_HTML :
          case DNT_BR :
            goto P_Anything;
          default :
            pThis -> bParserError = __LINE__;
            return;
        }
      }
      default :
      {
        P_Anything :
        {
          struct html__node *pE = rHtmlNode_NewNode ( pThis, DNT_HTML );
          rHtmlNode_AppendChild ( &(pThis -> Node), pE );
          pThis -> pParserLastNode = pE;
          pThis -> rParserMode = rHtmlParserMode_before_head;
          pThis -> rParserMode ( pThis, iTokenType, pData );
          return;
        }
      }
    }
  }

  void rHtmlParserMode_before_head ( struct html__document * const pThis, const UINT iTokenType, const PBYTE pData )
  {
    switch ( iTokenType )
    {
      case 0x09 : /* TAB */
      case 0x0A : /* LF */
      case 0X0C : /* FF */
      case 0x0D : /* CR */
      case 0X20 : /* SPACE */
        return;
      case HTT_comment :
      {
        struct html__comment *pE = rHtmlNode_NewComment ( pThis );
        pE -> pBegin = pData;
        rHtmlNode_AppendChild ( pThis -> pParserLastNode, &(pE -> Node) );
        return;
      }
      case HTT_DOCTYPE :
        pThis -> bParserError = __LINE__;
        return;
      case HTT_start_tag :
      {
        const struct html__token_tag *pToken = (const struct html__token_tag *)pData;
        UINT iT = rHtmlNode_GetTypeByName ( pToken -> pName );
        switch ( iT )
        {
          case DNT_HTML :
          {
            // TODO: Process the token using the rules for the "in body" insertion mode.
            pThis -> bParserError = __LINE__;
            return;
          }
          case DNT_HEAD :
          {
            struct html__node *pE = rHtmlNode_NewNode ( pThis, iT );
            rHtmlNode_AppendChild ( pThis -> pParserLastNode, pE );
            pE -> pAttribs = pToken -> pAttribs;
            pThis -> pParserLastNode = pE;
            pThis -> rParserMode = rHtmlParserMode_before_head;
            pThis -> pNodeHead = pE;
            return;
          }
          default :
            goto P_Anything;
        }
      }
      case HTT_end_tag :
      {
        const struct html__token_tag *pToken = (const struct html__token_tag *)pData;
        UINT iT = rHtmlNode_GetTypeByName ( pToken -> pName );
        switch ( iT )
        {
          case DNT_HEAD :
          case DNT_BODY :
          case DNT_HTML :
          case DNT_BR :
            goto P_Anything;
          default :
            pThis -> bParserError = __LINE__;
            return;
        }
      }
      default :
      {
        P_Anything :
        {
          struct html__node *pE = rHtmlNode_NewNode ( pThis, DNT_HEAD );
          rHtmlNode_AppendChild ( pThis -> pParserLastNode, pE );
          pThis -> pParserLastNode = pE;
          pThis -> rParserMode = rHtmlParserMode_in_head;
          pThis -> pNodeHead = pE;
          pThis -> rParserMode ( pThis, iTokenType, pData );
          return;
        }
      }
    }
  }

  void rHtmlParserMode_in_head ( struct html__document * const pThis, const UINT iTokenType, const PBYTE pData )
  {
    switch ( iTokenType )
    {
      case 0x09 : /* TAB */
      case 0x0A : /* LF */
      case 0X0C : /* FF */
      case 0x0D : /* CR */
      case 0X20 : /* SPACE */
        // TODO: Insert Char
        return;
      case HTT_comment :
      {
        struct html__comment *pE = rHtmlNode_NewComment ( pThis );
        pE -> pBegin = pData;
        rHtmlNode_AppendChild ( pThis -> pParserLastNode, &(pE -> Node) );
        return;
      }
      case HTT_DOCTYPE :
        pThis -> bParserError = __LINE__;
        return;
      case HTT_start_tag :
      {
        const struct html__token_tag *pToken = (const struct html__token_tag *)pData;
        UINT iT = rHtmlNode_GetTypeByName ( pToken -> pName );
        switch ( iT )
        {
          case DNT_HTML :
          {
            // TODO: Process the token using the rules for the "in body" insertion mode.
            pThis -> bParserError = __LINE__;
            return;
          }
          case DNT_BASE :
          case DNT_BASEFONT :
          case DNT_BGSOUND :
          case DNT_LINK :
          case DNT_META :
          {
            struct html__node *pE = rHtmlNode_NewNode ( pThis, iT );
            rHtmlNode_AppendChild ( pThis -> pParserLastNode, pE );
            pE -> pAttribs = pToken -> pAttribs;
            return;
          }
          case DNT_TITLE :
          {
            struct html__node *pE = rHtmlNode_NewNode ( pThis, iT );
            rHtmlNode_AppendChild ( pThis -> pParserLastNode, pE );
            pE -> pAttribs = pToken -> pAttribs;
            // TODO: Follow the generic RCDATA element parsing algorithm.
            pThis -> bParserError = __LINE__;
            return;
          }
          case DNT_NOSCRIPT :
          {
            if ( pThis -> bNoScript )
            {
              // TODO: Insert an HTML element for the token.
              // Switch the insertion mode to "in head noscript".
              pThis -> bParserError = __LINE__;
              return;
            }
          }
          case DNT_NOFRAMES :
          case DNT_STYLE :
          {
            // TODO: Follow the generic raw text element parsing algorithm.
            pThis -> bParserError = __LINE__;
            return;
          }
          case DNT_SCRIPT :
          {
            // TODO: Many Steps
            pThis -> bParserError = __LINE__;
            return;
          }
          case DNT_HEAD :
            pThis -> bParserError = __LINE__;
            return;
          default :
            goto P_Anything;
        }
        return;
      }
      case HTT_end_tag :
      {
        const struct html__token_tag *pToken = (const struct html__token_tag *)pData;
        UINT iT = rHtmlNode_GetTypeByName ( pToken -> pName );
        switch ( iT )
        {
          case DNT_BODY :
          case DNT_HTML :
          case DNT_BR :
            goto P_Anything;
          default :
            pThis -> bParserError = __LINE__;
            return;
        }
      }
      default :
      {
        P_Anything :
        assert ( pThis -> pParserLastNode -> iType == DNT_HTML );
        pThis -> pParserLastNode = pThis -> pParserLastNode -> pParent;
        pThis -> rParserMode = rHtmlParserMode_after_head;
        pThis -> rParserMode ( pThis, iTokenType, pData );
        return;
      }
    }
  }

  void rHtmlParserMode_after_head ( struct html__document * const pThis, const UINT iTokenType, const PBYTE pData )
  {
    // TODO: https://www.w3.org/TR/html52/syntax.html#the-after-head-insertion-mode
    pThis -> bParserError = __LINE__;
    return;
  }

  struct html__document * rHtmlParser_OpenFile ( const PCSTR szFileName )
  {
    struct html__document * const pDocument = rHmtlDocument_New ( );
    UINT nSize = 0;
    PBYTE p = pDocument -> pParserStream = (PBYTE) ( pDocument -> pBuffer = rLoadFile ( szFileName, &nSize ) );
    pDocument -> rParserMode = rHtmlParserMode_initial;
    p [ nSize ] = _EOF;
    HTML_PRINT ( "Parsing file: \"%s\"\n", szFileName );
    HTML_PVERB ( "Buffer point %p with size %u\n", p, nSize );



    while ( !rCmpStr ( (PSTR)p, "<!DOCTYPE" ) ) { ++p; }

    #define HTML_STATE(a)\
      a : HTML_PVERB ( "T: "#a"\n   symbol in %p '%c' (%u 0x%x)\n", p, (CHAR)(*p), (UINT)(*p), (UINT)(*p) );

    #define HTML_STATE_ERROR() HTML_PRINT ( "T ERROR: on line %d" , __LINE__ )

    UINT iData = 0;
    struct html__token_tag token_tag;
    void token_tag_reset ( )
    {
      memset ( &token_tag, 0, sizeof ( token_tag ) );
    }
    PBYTE token_comment = NULL;

    HTML_STATE ( P_Data_state )
      switch ( *p )
      {
        case '<' :
          ++p;
          goto P_Tag_open_state;
        case 0x00 : /* NULL */
          HTML_STATE_ERROR();
          goto P___Finish;
        case _EOF :
          pDocument -> rParserMode ( pDocument, HTT_end_of_file, NULL );
          goto P___Finish;
        default :
          pDocument -> rParserMode ( pDocument, (UINT)(*p), NULL );
          ++p;
          goto P_Data_state;
      }
    // HTML_STATE ( P_RCDATA_state )
    // HTML_STATE ( P_RAWTEXT_state )
    // HTML_STATE ( P_Script_data_state )
    // HTML_STATE ( P_PLAINTEXT_state )
    HTML_STATE ( P_Tag_open_state )
      switch ( *p )
      {
        case '!' :
          ++p;
          goto P_Markup_declaration_open_state;
        case '/' :
          ++p;
          goto P_End_tag_open_state;
        case 'A' ... 'Z' :
        case 'a' ... 'z' :
          iData = HTT_start_tag;
          token_tag_reset ( );
          token_tag.pName = p;
          goto P_Tag_name_state;
        case '?' :
          HTML_STATE_ERROR();
          token_comment = p+1;
          goto P_Bogus_comment_state;
        default :
          HTML_STATE_ERROR();
          pDocument -> rParserMode ( pDocument, '<', NULL );
          goto P_Data_state;
      }
    HTML_STATE ( P_End_tag_open_state )
      switch ( *p )
      {
        case 'A' ... 'Z' :
        case 'a' ... 'z' :
          iData = HTT_end_tag;
          token_tag_reset ( );
          token_tag.pName = p;
          goto P_Tag_name_state;
        case '>' :
          HTML_STATE_ERROR();
          ++p;
          goto P_Data_state;
        default :
          HTML_STATE_ERROR();
          token_comment = p+1;
          goto P_Bogus_comment_state;
      }
    HTML_STATE ( P_Tag_name_state )
      switch ( *p )
      {
        case 0x09 : /* TAB */
        case 0x0A : /* LF */
        case 0X0C : /* FF */
        case 0X20 : /* SPACE */
          *p = 0;
          ++p;
          // goto P_Before_attribute_name_state;
          goto P___Finish;
        case '/' :
          // goto P_Self_closing_start_tag_state;
          goto P___Finish;
        case 'A' ... 'Z' :
          *p += 0x20;
        case 'a' ... 'z' :
          ++p;
          goto P_Tag_name_state;
        case 0x00 : /* NULL */
          HTML_STATE_ERROR();
          goto P___Finish;
        case _EOF :
          HTML_STATE_ERROR();
          pDocument -> rParserMode ( pDocument, HTT_end_of_file, NULL );
          goto P___Finish;
        default :
          ++p;
          goto P_Tag_name_state;
      }

    // HTML_STATE ( P_RCDATA_less_than_sign_state )
    // HTML_STATE ( P_RCDATA_end_tag_open_state )
    // HTML_STATE ( P_RCDATA_end_tag_name_state )
    // HTML_STATE ( P_RAWTEXT_less_than_sign_state )
    // HTML_STATE ( P_RAWTEXT_end_tag_open_state )
    // HTML_STATE ( P_RAWTEXT_end_tag_name_state )
    // HTML_STATE ( P_Script_data_less_than_sign_state )
    // HTML_STATE ( P_Script_data_end_tag_open_state )
    // HTML_STATE ( P_Script_data_end_tag_name_state )
    // HTML_STATE ( P_Script_data_escape_start_state )
    // HTML_STATE ( P_Script_data_escape_start_dash_state )
    // HTML_STATE ( P_Script_data_escaped_state )
    // HTML_STATE ( P_Script_data_escaped_dash_state )
    // HTML_STATE ( P_Script_data_escaped_dash_dash_state )
    // HTML_STATE ( P_Script_data_escaped_less_than_sign_state )
    // HTML_STATE ( P_Script_data_escaped_end_tag_open_state )
    // HTML_STATE ( P_Script_data_escaped_end_tag_name_state )
    // HTML_STATE ( P_Script_data_double_escape_start_state )
    // HTML_STATE ( P_Script_data_double_escaped_state )
    // HTML_STATE ( P_Script_data_double_escaped_dash_state )
    // HTML_STATE ( P_Script_data_double_escaped_dash_dash_state )
    // HTML_STATE ( P_Script_data_double_escaped_less_than_sign_state )
    // HTML_STATE ( P_Script_data_double_escape_end_state )
    // HTML_STATE ( P_Before_attribute_name_state )
    // HTML_STATE ( P_Attribute_name_state )
    // HTML_STATE ( P_After_attribute_name_state )
    // HTML_STATE ( P_Before_attribute_value_state )
    // HTML_STATE ( P_Attribute_value__double_quoted__state )
    // HTML_STATE ( P_Attribute_value__single_quoted__state )
    // HTML_STATE ( P_Attribute_value__unquoted__state )
    // HTML_STATE ( P_After_attribute_value__quoted__state )
    // HTML_STATE ( P_Self_closing_start_tag_state )
    HTML_STATE ( P_Bogus_comment_state )
      switch ( *p )
      {
        case '>' :
          pDocument -> rParserMode ( pDocument, HTT_comment, token_comment );
          *p = 0;
          ++p;
          goto P_Data_state;
        case _EOF :
          pDocument -> rParserMode ( pDocument, HTT_comment, token_comment );
          pDocument -> rParserMode ( pDocument, HTT_end_of_file, NULL );
          goto P___Finish;
        case 0x00 : /* NULL */
          HTML_STATE_ERROR();
          goto P___Finish;
        default :
          ++p;
          goto P_Bogus_comment_state;
      }
    HTML_STATE ( P_Markup_declaration_open_state )
      if ( ( *p == '-' ) && ( p[1] == '-' ) )
      {
        token_comment = p + 2;
        P__Loc0 :
          switch ( *p )
          {
            case '-' :
              if ( rCmpStr ( (PSTR)(p+1), "->" ) )
              {
                *p = 0;
                p += 3;
                pDocument -> rParserMode ( pDocument, HTT_comment, token_comment );
                goto P_Data_state;
              }
              goto P__Loc0;

            case _EOF :
              pDocument -> rParserMode ( pDocument, HTT_comment, token_comment );
              pDocument -> rParserMode ( pDocument, HTT_end_of_file, NULL );
              goto P___Finish;
            case 0x00 : /* NULL */
              HTML_STATE_ERROR();
              goto P___Finish;
            default :
              goto P__Loc0;
          }
      }
      if ( rCmpStr ( (PSTR)p, "DOCTYPE" ) )
      {
        p += 7;
        goto P_DOCTYPE_state;
      }
      HTML_STATE_ERROR();
      token_comment = p;
      goto P_Bogus_comment_state;
    // HTML_STATE ( P_Comment_start_state )
    // HTML_STATE ( P_Comment_start_dash_state )
    // HTML_STATE ( P_Comment_state )
    // HTML_STATE ( P_Comment_less_than_sign_state )
    // HTML_STATE ( P_Comment_less_than_sign_bang_state )
    // HTML_STATE ( P_Comment_less_than_sign_bang_dash_state )
    // HTML_STATE ( P_Comment_less_than_sign_bang_dash_dash_state )
    // HTML_STATE ( P_Comment_end_dash_state )
    // HTML_STATE ( P_Comment_end_state )
    // HTML_STATE ( P_Comment_end_bang_state )
    HTML_STATE ( P_DOCTYPE_state )
      switch ( *p )
      {
        case 0x09 : /* TAB */
        case 0x0A : /* LF */
        case 0X0C : /* FF */
        case 0X20 : /* SPACE */
          ++p;
          goto P_Before_DOCTYPE_name_state;
        case _EOF :
          pDocument -> rParserMode ( pDocument, HTT_DOCTYPE, token_comment );
          pDocument -> rParserMode ( pDocument, HTT_end_of_file, NULL );
          goto P___Finish;
        default :
          HTML_STATE_ERROR();
          goto P_Before_DOCTYPE_name_state;
      }
    HTML_STATE ( P_Before_DOCTYPE_name_state )
      switch ( *p )
      {
        case 0x09 : /* TAB */
        case 0x0A : /* LF */
        case 0X0C : /* FF */
        case 0X20 : /* SPACE */
          ++p;
          goto P_Before_DOCTYPE_name_state;
        case 0x00 : /* NULL */
          HTML_STATE_ERROR();
          goto P___Finish;
        case _EOF :
          pDocument -> rParserMode ( pDocument, HTT_DOCTYPE, token_comment );
          pDocument -> rParserMode ( pDocument, HTT_end_of_file, NULL );
          goto P___Finish;
        case 'A' ... 'Z' :
          *p += 0x20;
        default :
          token_comment = p;
          ++p;
          goto P_DOCTYPE_name_state;
      }
    HTML_STATE ( P_DOCTYPE_name_state )
    switch ( *p )
      {
        case 0x09 : /* TAB */
        case 0x0A : /* LF */
        case 0X0C : /* FF */
        case 0X20 : /* SPACE */
          ++p;
          // goto P_After_DOCTYPE_name_state;
          goto P_DOCTYPE_name_state;
        case '>' :
          pDocument -> rParserMode ( pDocument, HTT_DOCTYPE, token_comment );
          goto P_Data_state;
        case 0x00 : /* NULL */
          HTML_STATE_ERROR();
          goto P___Finish;
        case _EOF :
          pDocument -> rParserMode ( pDocument, HTT_DOCTYPE, token_comment );
          pDocument -> rParserMode ( pDocument, HTT_end_of_file, NULL );
          goto P___Finish;
        case 'A' ... 'Z' :
          *p += 0x20;
        default :
          ++p;
          goto P_DOCTYPE_name_state;
      }
    // HTML_STATE ( P_After_DOCTYPE_name_state )
    // HTML_STATE ( P_After_DOCTYPE_public_keyword_state )
    // HTML_STATE ( P_Before_DOCTYPE_public_identifier_state )
    // HTML_STATE ( P_DOCTYPE_public_identifier__double_quoted__state )
    // HTML_STATE ( P_DOCTYPE_public_identifier__single_quoted__state )
    // HTML_STATE ( P_After_DOCTYPE_public_identifier_state )
    // HTML_STATE ( P_Between_DOCTYPE_public_and_system_identifiers_state )
    // HTML_STATE ( P_After_DOCTYPE_system_keyword_state )
    // HTML_STATE ( P_Before_DOCTYPE_system_identifier_state )
    // HTML_STATE ( P_DOCTYPE_system_identifier__double_quoted__state )
    // HTML_STATE ( P_DOCTYPE_system_identifier__single_quoted__state )
    // HTML_STATE ( P_After_DOCTYPE_system_identifier_state )
    // HTML_STATE ( P_Bogus_DOCTYPE_state )
    // HTML_STATE ( P_CDATA_section_state )
    // HTML_STATE ( P_CDATA_section_bracket_state )
    // HTML_STATE ( P_CDATA_section_end_state )
    // HTML_STATE ( P_Character_reference_state )
    // HTML_STATE ( P_Numeric_character_reference_state )
    // HTML_STATE ( P_Hexadecimal_character_reference_start_state )
    // HTML_STATE ( P_Decimal_character_reference_start_state )
    // HTML_STATE ( P_Hexadecimal_character_reference_state )
    // HTML_STATE ( P_Decimal_character_reference_state )
    // HTML_STATE ( P_Numeric_character_reference_end_state )
    // HTML_STATE ( P_Character_reference_end_state )
    HTML_STATE ( P___Finish )
    return pDocument;
  }

/// Entery point

int main ( int argc, char const * argv [] )
{
  if ( argv[2] ) { pF_HtmlDebug = fopen ( argv[2], "wb" ); }
  if ( ( argc > 3 ) && ( argv[3][0] == '1' ) ) { gb_HtmlVerb = TRUE; }
  rHtmlParser_OpenFile ( argv[1] );
  if ( pF_HtmlDebug ) { fclose ( pF_HtmlDebug ); }
  return 0;
}
