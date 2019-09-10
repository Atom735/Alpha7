
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "windows.h"

typedef struct DOM_Node         DOM_Node;
typedef struct DOM_Document     DOM_Document;
typedef struct DOM_Element      DOM_Element;
typedef struct DOM_Node         DOM_Node;
typedef struct DOM_Text         DOM_Text;
typedef struct DOM_Comment      DOM_Comment;
typedef struct DOM_CDATASection DOM_CDATASection;
typedef struct DOM_Attr         DOM_Attr;
typedef struct HTML_Parser      HTML_Parser;
typedef struct HTML_Writer      HTML_Writer;

///DOM_Node https://www.w3.org/TR/DOM-Level-3-Core/core.html#ID-1950641247

  enum
  {
    kDOM_NodeType_NULL = 0,
    kDOM_NodeType_ELEMENT,
    kDOM_NodeType_ATTRIBUTE,
    kDOM_NodeType_TEXT,
    kDOM_NodeType_CDATA_SECTION,
    kDOM_NodeType_ENTITY_REFERENCE,
    kDOM_NodeType_ENTITY,
    kDOM_NodeType_PROCESSING_INSTRUCTION,
    kDOM_NodeType_COMMENT,
    kDOM_NodeType_DOCUMENT,
    kDOM_NodeType_DOCUMENT_TYPE,
    kDOM_NodeType_DOCUMENT_FRAGMENT,
    kDOM_NodeType_NOTATION,
  };

  struct DOM_Node
  {
    UINT16            iType;
    UINT16            iName;
    PBYTE             pValue;
    DOM_Document     *pOwner;
    DOM_Node
         *pParent,       *pAttributes,
         *pChildFirst,   *pChildLast,
         *pSiblingPrev,  *pSiblingNext;
  };

  DOM_Node * rDOM_Node_insertBefore ( DOM_Node * const pThis,
      DOM_Node * const pNew, DOM_Node * const pRef );
  DOM_Node * rDOM_Node_replaceChild ( DOM_Node * const pThis,
      DOM_Node * const pNew, DOM_Node * const pOld );
  DOM_Node * rDOM_Node_removeChild ( DOM_Node * const pThis,
      DOM_Node * const pOld );
  DOM_Node * rDOM_Node_appendChild ( DOM_Node * const pThis,
      DOM_Node * const pNew );
  DOM_Node * rDOM_Node_cloneNode ( DOM_Node * const pThis,
      const BOOL bDeep );
  void       rDOM_Node_normalize ( DOM_Node * const pThis );

///DOM_Document https://www.w3.org/TR/DOM-Level-3-Core/core.html#i-Document

  enum
  {
    kDOM_Doctype_NULL = 0,
    kDOM_Doctype_HTML5,
  }

  struct DOM_Document
  {
    DOM_Node          Node;
    UINT16            iDoctype;
  };

  DOM_Document *     rDOM_Document_new ( );
  void               rDOM_Document_free ( DOM_Document * const pThis );

  DOM_Element *      rDOM_Document_createElement (
    DOM_Document * const pThis, const UINT16 iTagName );
  DOM_Node *         rDOM_Document_createDocumentFragment (
    DOM_Document * const pThis );
  DOM_Text *         rDOM_Document_createTextNode (
    DOM_Document * const pThis, const PBYTE pData );
  DOM_Comment *      rDOM_Document_createComment (
    DOM_Document * const pThis, const PBYTE pData );
  DOM_CDATASection * rDOM_Document_createCDATASection (
    DOM_Document * const pThis, const PBYTE pData );
  DOM_Attr *         rDOM_Document_createAttribute (
    DOM_Document * const pThis, const UINT16 iAttrName );
  void               rDOM_Document_normalizeDocument (
    DOM_Document * const pThis );

///DOM_Attr https://www.w3.org/TR/DOM-Level-3-Core/core.html#ID-637646024

  struct DOM_Attr
  {
    DOM_Node          Node;
    DOM_Element      *pOwner;
  };

///DOM_Element https://www.w3.org/TR/DOM-Level-3-Core/core.html#ID-745549614

  struct DOM_Element
  {
    DOM_Node          Node;
    DOM_Attr         *pId;
  };

  PBYTE         rDOM_Element_getAttribute (
    DOM_Element * const pThis, const UINT16 iName );
  void          rDOM_Element_setAttribute (
    DOM_Element * const pThis, const UINT16 iName, const PBYTE pValue );
  void          rDOM_Element_removeAttribute (
    DOM_Element * const pThis, const UINT16 iName );
  DOM_Attr *    rDOM_Element_getAttributeNode (
    DOM_Element * const pThis, const UINT16 iName );
  DOM_Attr *    rDOM_Element_setAttributeNode (
    DOM_Element * const pThis, DOM_Attr * const pNew );
  DOM_Attr *    rDOM_Element_removeAttributeNode(
    DOM_Element * const pThis, DOM_Attr * const pOld );

///DOM_Text https://www.w3.org/TR/DOM-Level-3-Core/core.html#ID-1312295772

  struct DOM_Text
  {
    DOM_Node          Node;
  };

///DOM_Comment https://www.w3.org/TR/DOM-Level-3-Core/core.html#ID-1728279322

  struct DOM_Comment
  {
    DOM_Node          Node;
  };

///DOM_CDATASection https://www.w3.org/TR/DOM-Level-3-Core/core.html#ID-667469212

  struct DOM_CDATASection
  {
    DOM_Node          Node;
  };

///HTML_Parser https://www.w3.org/TR/html52/syntax.html#parsing-html-documents

  struct HTML_Parser
  {
    DOM_Document     *pDoc;
    DOM_Node         *pStack;
    void             *pReturnState;
    void             *pActualState;
    void ( *rInsert )( HTML_Parser * const pThis );
    BYTE              _locBuf[0xf00];
  };

  void rHTML_Parser_init (
    HTML_Parser * const pThis );
  void rHTML_Parser_sendBytes (
    HTML_Parser * const pThis, PBYTE pData, UINT nSize );

///HTML_Writer https://www.w3.org/TR/html52/syntax.html#writing-html-documents

  void rHTML_Writer_writeToFile (
    DOM_Document * const pThis, LPCSTR szFileName );

  void rHTML_Writer_writeNodeToFD (
    DOM_Node * const pThis, FILE * const pF );

int main ( int argc, char const *argv[] )
{
  DOM_Document * const pDoc = (DOM_Document*) malloc ( sizeof(DOM_Document*) );
  rHTML_Writer_writeToFile ( pDoc, argv[2] );
  free ( pDoc );
  return 0;
}

void rHTML_Parser_init (
    HTML_Parser * const pThis )
{
  assert ( pThis != NULL );
  pThis -> pDoc = NULL;
}

void rHTML_Writer_writeToFile (
    DOM_Document * const pThis, LPCSTR szFileName )
{
  FILE * pF = fopen ( szFileName, "wb" );
  if ( pThis -> iDoctype == kDOM_Doctype_HTML5 )
  {
    fprintf ( pF, "<!DOCTYPE html>\n" );
  }
  fclose ( pF );
}


void rHTML_Writer_writeNodeToFD (
    DOM_Node * const pThis, FILE * const pF )
{
  switch ( pThis -> iType )
  {

  }
}
