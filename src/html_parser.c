#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <windows.h>






enum
{
  kNodeType_NULL            = 0,
  kNodeType_document    = 0xff000000,
  kNodeType_doctype,
  kNodeType_text,
  kNodeType_comment,
  kNodeType_htmlElement     = 0xff010000,
};

struct htmlNode
{
  UINT32 iType;
  struct htmlNode
    *pParent, *pNext, *pPrev, *pFirst, *pLast;
  struct htmlDocument *pOwner;
};

void htmlNode_append ( struct htmlNode * const pThis,
    struct htmlNode * const pNew )
{
    if ( pThis -> pLast == NULL ) {
        pThis -> pFirst = pThis -> pLast = pNew;
        pNew -> pParent = pThis;
    }
    else
    {
        struct htmlNode *pLast = pThis -> pLast;
        pThis -> pLast = pNew;
        pNew -> pParent = pThis;
        pNew -> pPrev = pLast;
        pLast -> pNext = pNew;
    }
}

struct htmlDocument
{
  struct htmlNode node;
};

struct htmlString
{
  struct htmlNode node;
  PBYTE pData;
  UINT nSize, nSizeMax;
};

void htmlString_append ( struct htmlString * const pString,
      const PBYTE pData, const UINT nDataSize )
{
  if ( pString -> nSizeMax < pString -> nSize + nDataSize )
  {
    while ( pString -> nSizeMax < pString -> nSize + nDataSize )
    {
      pString -> nSizeMax <<= 1;
    }
    const PBYTE buf = (PBYTE) malloc ( pString -> nSizeMax );
    memcpy ( buf, pString -> pData, pString -> nSize );
    free ( pString -> pData );
    pString -> pData = buf;
  }
  memcpy ( pString -> pData + pString -> nSize, pData, nDataSize );
  pString -> nSize += nDataSize;
}

struct htmlString* htmlDocument_newString (
      struct htmlDocument * const pDoc, const UINT iType )
{
  struct htmlString * const pOut = (struct htmlString*)
        malloc ( sizeof(struct htmlString) );
  memset ( pOut, 0, sizeof(struct htmlString) );
  pOut -> node.iType = iType;
  pOut -> node.pOwner = pDoc;
  pOut -> pData = (PBYTE) malloc ( pOut -> nSizeMax = 0x10 );
  return pOut;
}

#define htmlDocument_newText(pDoc)\
      htmlDocument_newString(pDoc,kNodeType_text)
#define htmlDocument_newComment(pDoc)\
      htmlDocument_newString(pDoc,kNodeType_comment)


struct htmlDocument* htmlDocument_new ( )
{
  struct htmlDocument * const pOut = (struct htmlDocument*)
        malloc ( sizeof(struct htmlDocument) );
  memset ( pOut, 0, sizeof(struct htmlDocument) );
  pOut -> node.iType = kNodeType_document;
  pOut -> node.pOwner = pOut;
  return pOut;
}

void htmlDocument_saveToFile ( struct htmlDocument * const pDoc,
      const LPCSTR szFileName )
{
  FILE * const pF = fopen ( szFileName, "wb" );
  fwrite ( "\xEF\xBB\xBF", 1, 3, pF );
  struct htmlNode *pNode = pDoc -> node.pFirst;
  while ( pNode )
  {
    switch ( pNode -> iType )
    {
      case kNodeType_comment:
        fwrite ( "<!--", 1, 4, pF );
        fwrite ( ((struct htmlString*)(pNode)) -> pData, 1,
              ((struct htmlString*)(pNode)) -> nSize, pF );
        fwrite ( "-->\n", 1, 4, pF );
        pNode = pNode -> pNext;
        continue;
    }
  }
  fclose ( pF );
}

struct htmlParser
{
  struct htmlDocument *pDoc;
};

struct htmlParser* htmlParser_new ( struct htmlDocument * const pDoc )
{
  struct htmlParser * const pOut = (struct htmlParser*)
        malloc ( sizeof(struct htmlParser) );
  memset ( pOut, 0, sizeof(struct htmlParser) );
  pOut -> pDoc = pDoc;
  return pOut;

}

void htmlParser_parse ( struct htmlParser * const pParser,
      const PBYTE pData, const UINT nDataSize )
{

}

int main ( int argc, char const *argv[] )
{
  struct htmlDocument * const pDoc = htmlDocument_new ( );
  struct htmlParser * const pParser = htmlParser_new ( pDoc );
  FILE * const pF = fopen ( argv [ 1 ], "rb" );
  BYTE _buf [ 0x10 ];
  while ( !feof ( pF ) )
  {
    htmlParser_parse ( pParser, _buf, fread ( _buf, 1, 0x10, pF ) );
  }
  fclose ( pF );

  {
    struct htmlString * const pStr = htmlDocument_newComment ( pDoc );
    htmlNode_append ( &( pDoc -> node ), &( pStr -> node ) );
    htmlString_append ( pStr, (PBYTE)(" Hello World! "), 14 );
  }
  {
    struct htmlString * const pStr = htmlDocument_newComment ( pDoc );
    htmlNode_append ( &( pDoc -> node ), &( pStr -> node ) );
    htmlString_append ( pStr, (PBYTE)("What are you need???"), 20 );
    htmlString_append ( pStr, (PBYTE)(" Or not you?"), 12 );
  }

  htmlDocument_saveToFile ( pDoc, argv [ 2 ] );
  return 0;
}

