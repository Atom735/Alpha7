# The "initial" insertion mode

* Ignore chars:
    [TAB, LF, FF, CR, SPACE]
* A __comment__ token:
    Insert a comment as the last child of the Document object.
* A __DOCTYPE__ token
    Append a DocumentType node to the Document node
    Then, switch the insertion mode to __"before html"__.
* Anything else
    Parse error.

# The "before html" insertion mode

* Ignore chars:
    [TAB, LF, FF, CR, SPACE]
* A __comment__ token
    Insert a comment as the last child of the Document object.
* A __DOCTYPE__ token
    Parse error.
* A start tag whose tag name is __"html"__
    Create an element for the token in the HTML namespace, with the Document as the intended parent. Append it to the Document object. Put this element in the stack of open elements.
    Switch the insertion mode to __"before head"__.
* Anything else
    Parse error.

# The "before head" insertion mode

* Ignore chars:
    [TAB, LF, FF, CR, SPACE]
* A __comment__ token
    Insert a comment.
* A __DOCTYPE__ token
    Parse error.
* A start tag whose tag name is __"head"__
    Insert an HTML element for the token.
    Set the head element pointer to the newly created head element.
    Switch the insertion mode to __"in head"__.
* Anything else
    Parse error.

# The "in head" insertion mode

* [TAB, LF, FF, CR, SPACE]
    Insert the character.
* A __comment__ token
    Insert a comment.
* A __DOCTYPE__ token
    Parse error.
*  A start tag whose tag name is one of: __"base", "basefont", "bgsound", "link"__
    Insert an HTML element for the token. Immediately pop the current node off the stack of open elements.
    Acknowledge the token’s *self-closing flag*, if it is set.
* A start tag whose tag name is __"meta"__
    Insert an HTML element for the token. Immediately pop the current node off the stack of open elements.
    Acknowledge the token’s *self-closing flag*, if it is set.
* A start tag whose tag name is __"title"__
    Follow the generic RCDATA element parsing algorithm.
* A start tag whose tag name is __"noscript"__, if the scripting flag is enabled
* A start tag whose tag name is one of: __"noframes", "style"__
    Follow the generic raw text element parsing algorithm.
* A start tag whose tag name is __"noscript"__, if the scripting flag is disabled
    Insert an HTML element for the token.
    Switch the insertion mode to "in head noscript".
* A start tag whose tag name is __"script"__
    Run these steps:
    Let the adjusted insertion location be the appropriate place for inserting a node.

Create an element for the token in the HTML namespace, with the intended parent being the element in which the adjusted insertion location finds itself.

Mark the element as being "parser-inserted" and unset the element’s "non-blocking" flag.

This ensures that, if the script is external, any document.write() calls in the script will execute in-line, instead of blowing the document away, as would happen in most other cases. It also prevents the script from executing until the end tag is seen.

If the parser was originally created for the HTML fragment parsing algorithm, then mark the script element as "already started". (fragment case)

Insert the newly created element at the adjusted insertion location.

Push the element onto the stack of open elements so that it is the new current node.

Switch the tokenizer to the script data state.

Let the original insertion mode be the current insertion mode.

Switch the insertion mode to "text".

An end tag whose tag name is "head"
Pop the current node (which will be the head element) off the stack of open elements.

Switch the insertion mode to "after head".

An end tag whose tag name is one of: "body", "html", "br"
Act as described in the "anything else" entry below.

A start tag whose tag name is "template"
Insert an HTML element for the token.

Insert a marker at the end of the list of active formatting elements.

Set the frameset-ok flag to "not ok".

Switch the insertion mode to "in template".

Push "in template" onto the stack of template insertion modes so that it is the new current template insertion mode.

An end tag whose tag name is "template"
If there is no template element on the stack of open elements, then this is a parse error; ignore the token.

Otherwise, run these steps:

Generate all implied end tags thoroughly.

If the current node is not a template element, then this is a parse error.

Pop elements from the stack of open elements until a template element has been popped from the stack.

Clear the list of active formatting elements up to the last marker.

Pop the current template insertion mode off the stack of template insertion modes.

Reset the insertion mode appropriately.

A start tag whose tag name is "head"
Any other end tag
Parse error.

Anything else
Pop the current node (which will be the head element) off the stack of open elements.

Switch the insertion mode to "after head".

Reprocess the token.

8.2.5.4.5. The "in head noscript" insertion mode
When the user agent is to apply the rules for the "in head noscript" insertion mode, the user agent must handle the token as follows:

A DOCTYPE token
Parse error.

A start tag whose tag name is "html"
Process the token using the rules for the "in body" insertion mode.

An end tag whose tag name is "noscript"
Pop the current node (which will be a noscript element) from the stack of open elements; the new current node will be a head element.

Switch the insertion mode to "in head".

A character token that is one of U+0009 CHARACTER TABULATION, U+000A LINE FEED (LF), U+000C FORM FEED (FF), U+000D CARRIAGE RETURN (CR), or U+0020 SPACE
A comment token
A start tag whose tag name is one of: "basefont", "bgsound", "link", "meta", "noframes", "style"
Process the token using the rules for the "in head" insertion mode.

An end tag whose tag name is "br"
Act as described in the "anything else" entry below.

A start tag whose tag name is one of: "head", "noscript"
Any other end tag
Parse error.

Anything else
Parse error.

Pop the current node (which will be a noscript element) from the stack of open elements; the new current node will be a head element.

Switch the insertion mode to "in head".

Reprocess the token.

8.2.5.4.6. The "after head" insertion mode
When the user agent is to apply the rules for the "after head" insertion mode, the user agent must handle the token as follows:

A character token that is one of U+0009 CHARACTER TABULATION, U+000A LINE FEED (LF), U+000C FORM FEED (FF), U+000D CARRIAGE RETURN (CR), or U+0020 SPACE
Insert the character.

A comment token
Insert a comment.

A DOCTYPE token
Parse error.

A start tag whose tag name is "html"
Process the token using the rules for the "in body" insertion mode.

A start tag whose tag name is "body"
Insert an HTML element for the token.

Set the frameset-ok flag to "not ok".

Switch the insertion mode to "in body".

A start tag whose tag name is "frameset"
Insert an HTML element for the token.

Switch the insertion mode to "in frameset".

A start tag whose tag name is one of: "base", "basefont", "bgsound", "link", "meta", "noframes", "script", "style", "template", "title"
Parse error.

Push the node pointed to by the head element pointer onto the stack of open elements.

Process the token using the rules for the "in head" insertion mode.

Remove the node pointed to by the head element pointer from the stack of open elements. (It might not be the current node at this point.)

The head element pointer cannot be null at this point.

An end tag whose tag name is "template"
Process the token using the rules for the "in head" insertion mode.

An end tag whose tag name is one of: "body", "html", "br"
Act as described in the "anything else" entry below.

A start tag whose tag name is "head"
Any other end tag
Parse error.

Anything else
Insert an HTML element for a "body" start tag token with no attributes.

Switch the insertion mode to "in body".

Reprocess the current token.

8.2.5.4.7. The "in body" insertion mode
When the user agent is to apply the rules for the "in body" insertion mode, the user agent must handle the token as follows:

A character token that is U+0000 NULL
Parse error.

A character token that is one of U+0009 CHARACTER TABULATION, U+000A LINE FEED (LF), U+000C FORM FEED (FF), U+000D CARRIAGE RETURN (CR), or U+0020 SPACE
Reconstruct the active formatting elements, if any.

Insert the token’s character.

Any other character token
Reconstruct the active formatting elements, if any.

Insert the token’s character.

Set the frameset-ok flag to "not ok".

A comment token
Insert a comment.

A DOCTYPE token
Parse error.

A start tag whose tag name is "html"
Parse error.

If there is a template element on the stack of open elements, then ignore the token.

Otherwise, for each attribute on the token, check to see if the attribute is already present on the top element of the stack of open elements. If it is not, add the attribute and its corresponding value to that element.

A start tag whose tag name is one of: "base", "basefont", "bgsound", "link", "meta", "noframes", "script", "style", "template", "title"
An end tag whose tag name is "template"
Process the token using the rules for the "in head" insertion mode.

A start tag whose tag name is "body"
Parse error.

If the second element on the stack of open elements is not a body element, if the stack of open elements has only one node on it, or if there is a template element on the stack of open elements, then ignore the token. (fragment case)

Otherwise, set the frameset-ok flag to "not ok"; then, for each attribute on the token, check to see if the attribute is already present on the body element (the second element) on the stack of open elements, and if it is not, add the attribute and its corresponding value to that element.

A start tag whose tag name is "frameset"
Parse error.

If the stack of open elements has only one node on it, or if the second element on the stack of open elements is not a body element, then ignore the token. (fragment case)

If the frameset-ok flag is set to "not ok", ignore the token.

Otherwise, run the following steps:

Remove the second element on the stack of open elements from its parent node, if it has one.

Pop all the nodes from the bottom of the stack of open elements, from the current node up to, but not including, the root html element.

Insert an HTML element for the token.

Switch the insertion mode to "in frameset".

An end-of-file token
If the stack of template insertion modes is not empty, then process the token using the rules for the "in template" insertion mode.

Otherwise, follow these steps:

If there is a node in the stack of open elements that is not either a dd element, a dt element, an li element, an optgroup element, an option element, a p element, an rb element, an rp element, an rt element, an rtc element, a tbody element, a td element, a tfoot element, a th element, a thead element, a tr element, the body element, or the html element, then this is a parse error.

Stop parsing.

An end tag whose tag name is "body"
If the stack of open elements does not have a body element in scope, this is a parse error; ignore the token.

Otherwise, if there is a node in the stack of open elements that is not either a dd element, a dt element, an li element, an optgroup element, an option element, a p element, an rb element, an rp element, an rt element, an rtc element, a tbody element, a td element, a tfoot element, a th element, a thead element, a tr element, the body element, or the html element, then this is a parse error.

Switch the insertion mode to "after body".

An end tag whose tag name is "html"
If the stack of open elements does not have a body element in scope, this is a parse error; ignore the token.

Otherwise, if there is a node in the stack of open elements that is not either a dd element, a dt element, an li element, an optgroup element, an option element, a p element, an rb element, an rp element, an rt element, an rtc element, a tbody element, a td element, a tfoot element, a th element, a thead element, a tr element, the body element, or the html element, then this is a parse error.

Switch the insertion mode to "after body".

Reprocess the token.

A start tag whose tag name is one of: "address", "article", "aside", "blockquote", "center", "details", "dialog", "dir", "div", "dl", "fieldset", "figcaption", "figure", "footer", "header", "main", "nav", "ol", "p", "section", "summary", "ul"
If the stack of open elements has a p element in button scope, then close a p element.

Insert an HTML element for the token.

A start tag whose tag name is one of: "h1", "h2", "h3", "h4", "h5", "h6"
If the stack of open elements has a p element in button scope, then close a p element.

If the current node is an HTML element whose tag name is one of "h1", "h2", "h3", "h4", "h5", or "h6", then this is a parse error; pop the current node off the stack of open elements.

Insert an HTML element for the token.

A start tag whose tag name is one of: "pre", "listing"
If the stack of open elements has a p element in button scope, then close a p element.

Insert an HTML element for the token.

If the next token is a U+000A LINE FEED (LF) character token, then ignore that token and move on to the next one. (Newlines at the start of pre blocks are ignored as an authoring convenience.)

Set the frameset-ok flag to "not ok".

A start tag whose tag name is "form"
If the form element pointer is not null, and there is no template element on the stack of open elements, then this is a parse error; ignore the token.

Otherwise:

If the stack of open elements has a p element in button scope, then close a p element.

Insert an HTML element for the token, and, if there is no template element on the stack of open elements, set the form element pointer to point to the element created.

A start tag whose tag name is "li"
Run these steps:

Set the frameset-ok flag to "not ok".

Initialize node to be the current node (the bottommost node of the stack).

Loop: If node is an li element, then run these substeps:

Generate implied end tags, except for li elements.

If the current node is not an li element, then this is a parse error.

Pop elements from the stack of open elements until an li element has been popped from the stack.

Jump to the step labeled Done below.

If node is in the special category, but is not an address, div, or p element, then jump to the step labeled Done below.

Otherwise, set node to the previous entry in the stack of open elements and return to the step labeled Loop.

Done: If the stack of open elements has a p element in button scope, then close a p element.

Finally, insert an HTML element for the token.

A start tag whose tag name is one of: "dd", "dt"
Run these steps:

Set the frameset-ok flag to "not ok".

Initialize node to be the current node (the bottommost node of the stack).

Loop: If node is a dd element, then run these substeps:

Generate implied end tags, except for dd elements.

If the current node is not a dd element, then this is a parse error.

Pop elements from the stack of open elements until a dd element has been popped from the stack.

Jump to the step labeled Done below.

If node is a dt element, then run these substeps:

Generate implied end tags, except for dt elements.

If the current node is not a dt element, then this is a parse error.

Pop elements from the stack of open elements until a dt element has been popped from the stack.

Jump to the step labeled Done below.

If node is in the special category, but is not an address, div, or p element, then jump to the step labeled Done below.

Otherwise, set node to the previous entry in the stack of open elements and return to the step labeled Loop.

Done: If the stack of open elements has a p element in button scope, then close a p element.

Finally, insert an HTML element for the token.

A start tag whose tag name is "plaintext"
If the stack of open elements has a p element in button scope, then close a p element.

Insert an HTML element for the token.

Switch the tokenizer to the §8.2.4.5 PLAINTEXT state.

Once a start tag with the tag name "plaintext" has been seen, that will be the last token ever seen other than character tokens (and the end-of-file token), because there is no way to switch out of the §8.2.4.5 PLAINTEXT state.

A start tag whose tag name is "button"
If the stack of open elements has a button element in scope, then run these substeps:

Parse error.

Generate implied end tags.

Pop elements from the stack of open elements until a button element has been popped from the stack.

Reconstruct the active formatting elements, if any.

Insert an HTML element for the token.

Set the frameset-ok flag to "not ok".

An end tag whose tag name is one of: "address", "article", "aside", "blockquote", "button", "center", "details", "dialog", "dir", "div", "dl", "fieldset", "figcaption", "figure", "footer", "header", "listing", "main", "nav", "ol", "pre", "section", "summary", "ul"
If the stack of open elements does not have an element in scope that is an HTML element with the same tag name as that of the token, then this is a parse error; ignore the token.

Otherwise, run these steps:

Generate implied end tags.

If the current node is not an HTML element with the same tag name as that of the token, then this is a parse error.

Pop elements from the stack of open elements until an HTML element with the same tag name as the token has been popped from the stack.

An end tag whose tag name is "form"
If there is no template element on the stack of open elements, then run these substeps:

Let node be the element that the form element pointer is set to, or null if it is not set to an element.

Set the form element pointer to null.

If node is null or if the stack of open elements does not have node in scope, then this is a parse error; abort these steps and ignore the token.

Generate implied end tags.

If the current node is not node, then this is a parse error.

Remove node from the stack of open elements.

If there is a template element on the stack of open elements, then run these substeps instead:

If the stack of open elements does not have a form element in scope, then this is a parse error; abort these steps and ignore the token.

Generate implied end tags.

If the current node is not a form element, then this is a parse error.

Pop elements from the stack of open elements until a form element has been popped from the stack.

An end tag whose tag name is "p"
If the stack of open elements does not have a p element in button scope, then this is a parse error; insert an HTML element for a "p" start tag token with no attributes.

Close a p element.

An end tag whose tag name is "li"
If the stack of open elements does not have an li element in list item scope, then this is a parse error; ignore the token.

Otherwise, run these steps:

Generate implied end tags, except for li elements.

If the current node is not an li element, then this is a parse error.

Pop elements from the stack of open elements until an li element has been popped from the stack.

An end tag whose tag name is one of: "dd", "dt"
If the stack of open elements does not have an element in scope that is an HTML element with the same tag name as that of the token, then this is a parse error; ignore the token.

Otherwise, run these steps:

Generate implied end tags, except for HTML elements with the same tag name as the token.

If the current node is not an HTML element with the same tag name as that of the token, then this is a parse error.

Pop elements from the stack of open elements until an HTML element with the same tag name as the token has been popped from the stack.

An end tag whose tag name is one of: "h1", "h2", "h3", "h4", "h5", "h6"
If the stack of open elements does not have an element in scope that is an HTML element and whose tag name is one of "h1", "h2", "h3", "h4", "h5", or "h6", then this is a parse error; ignore the token.

Otherwise, run these steps:

Generate implied end tags.

If the current node is not an HTML element with the same tag name as that of the token, then this is a parse error.

Pop elements from the stack of open elements until an HTML element whose tag name is one of "h1", "h2", "h3", "h4", "h5", or "h6" has been popped from the stack.

An end tag whose tag name is "sarcasm"
Take a deep breath, then act as described in the "any other end tag" entry below.

A start tag whose tag name is "a"
If the list of active formatting elements contains an a element between the end of the list and the last marker on the list (or the start of the list if there is no marker on the list), then this is a parse error; run the adoption agency algorithm for the token, then remove that element from the list of active formatting elements and the stack of open elements if the adoption agency algorithm didn’t already remove it (it might not have if the element is not in table scope).

In the non-conforming stream &lt;a&nbsp;href="a">a&lt;table>&lt;a&nbsp;href="b">b&lt;/table>x, the first a element would be closed upon seeing the second one, and the "x" character would be inside a link to "b", not to "a". This is despite the fact that the outer a element is not in table scope (meaning that a regular &lt;/a> end tag at the start of the table wouldn’t close the outer a element). The result is that the two a elements are indirectly nested inside each other — non-conforming markup will often result in non-conforming DOMs when parsed.

Reconstruct the active formatting elements, if any.

Insert an HTML element for the token. Push onto the list of active formatting elements that element.

A start tag whose tag name is one of: "b", "big", "code", "em", "font", "i", "s", "small", "strike", "strong", "tt", "u"
Reconstruct the active formatting elements, if any.

Insert an HTML element for the token. Push onto the list of active formatting elements that element.

A start tag whose tag name is "nobr"
Reconstruct the active formatting elements, if any.

If the stack of open elements has a nobr element in scope, then this is a parse error; run the adoption agency algorithm for the token, then once again Reconstruct the active formatting elements, if any.

Insert an HTML element for the token. Push onto the list of active formatting elements that element.

An end tag whose tag name is one of: "a", "b", "big", "code", "em", "font", "i", "nobr", "s", "small", "strike", "strong", "tt", "u"
Run the adoption agency algorithm for the token.

A start tag whose tag name is one of: "applet", "marquee", "object"
Reconstruct the active formatting elements, if any.

Insert an HTML element for the token.

Insert a marker at the end of the list of active formatting elements.

Set the frameset-ok flag to "not ok".

An end tag token whose tag name is one of: "applet", "marquee", "object"
If the stack of open elements does not have an element in scope that is an HTML element with the same tag name as that of the token, then this is a parse error; ignore the token.

Otherwise, run these steps:

Generate implied end tags.

If the current node is not an HTML element with the same tag name as that of the token, then this is a parse error.

Pop elements from the stack of open elements until an HTML element with the same tag name as the token has been popped from the stack.

Clear the list of active formatting elements up to the last marker.

A start tag whose tag name is "table"
If the Document is not set to quirks mode, and the stack of open elements has a p element in button scope, then close a p element.

Insert an HTML element for the token.

Set the frameset-ok flag to "not ok".

Switch the insertion mode to "in table".

An end tag whose tag name is "br"
Parse error. Drop the attributes from the token, and act as described in the next entry; i.e., act as if this was a "br" start tag token with no attributes, rather than the end tag token that it actually is.

A start tag whose tag name is one of: "area", "br", "embed", "img", "wbr"
Reconstruct the active formatting elements, if any.

Insert an HTML element for the token. Immediately pop the current node off the stack of open elements.

Acknowledge the token’s *self-closing flag*, if it is set.

Set the frameset-ok flag to "not ok".

A start tag whose tag name is "input"
Reconstruct the active formatting elements, if any.

Insert an HTML element for the token. Immediately pop the current node off the stack of open elements.

Acknowledge the token’s *self-closing flag*, if it is set.

If the token does not have an attribute with the name "type", or if it does, but that attribute’s value is not an ASCII case-insensitive match for the string "hidden", then: set the frameset-ok flag to "not ok".

A start tag whose tag name is one of: "param", "source", "track"
Insert an HTML element for the token. Immediately pop the current node off the stack of open elements.

Acknowledge the token’s *self-closing flag*, if it is set.

A start tag whose tag name is "hr"
If the stack of open elements has a p element in button scope, then close a p element.

Insert an HTML element for the token. Immediately pop the current node off the stack of open elements.

Acknowledge the token’s *self-closing flag*, if it is set.

Set the frameset-ok flag to "not ok".

A start tag whose tag name is "image"
Parse error. Change the token’s tag name to "img" and reprocess it. (Don’t ask.)

A start tag whose tag name is "textarea"
Run these steps:

Insert an HTML element for the token.

If the next token is a U+000A LINE FEED (LF) character token, then ignore that token and move on to the next one. (Newlines at the start of textarea elements are ignored as an authoring convenience.)

Switch the tokenizer to the RCDATA state.

Let the original insertion mode be the current insertion mode.

Set the frameset-ok flag to "not ok".

Switch the insertion mode to "text".

A start tag whose tag name is "xmp"
If the stack of open elements has a p element in button scope, then close a p element.

Reconstruct the active formatting elements, if any.

Set the frameset-ok flag to "not ok".

Follow the generic raw text element parsing algorithm.

A start tag whose tag name is "iframe"
Set the frameset-ok flag to "not ok".

Follow the generic raw text element parsing algorithm.

A start tag whose tag name is "noembed"
A start tag whose tag name is "noscript", if the scripting flag is enabled
Follow the generic raw text element parsing algorithm.

A start tag whose tag name is "select"
Reconstruct the active formatting elements, if any.

Insert an HTML element for the token.

Set the frameset-ok flag to "not ok".

If the insertion mode is one of "in table", "in caption", "in table body", "in row", or "in cell", then switch the insertion mode to "in select in table". Otherwise, switch the insertion mode to "in select".

A start tag whose tag name is one of: "optgroup", "option"
If the current node is an option element, then pop the current node off the stack of open elements.

Reconstruct the active formatting elements, if any.

Insert an HTML element for the token.

A start tag whose tag name is one of: "rb", "rtc"
If the stack of open elements has a ruby element in scope, then generate implied end tags. If the current node is not now a ruby element, this is a parse error.

Insert an HTML element for the token.

A start tag whose tag name is one of: "rp", "rt"
If the stack of open elements has a ruby element in scope, then generate implied end tags, except for rtc elements. If the current node is not now a rtc element or a ruby element, this is a parse error.

Insert an HTML element for the token.

A start tag whose tag name is "math"
Reconstruct the active formatting elements, if any.

Adjust MathML attributes for the token. (This fixes the case of MathML attributes that are not all lowercase.)

Adjust foreign attributes for the token. (This fixes the use of namespaced attributes, in particular XLink.)

Insert a foreign element for the token, in the MathML namespace.

If the token has its self-closing flag set, pop the current node off the stack of open elements and acknowledge the token’s *self-closing flag*.

A start tag whose tag name is "svg"
Reconstruct the active formatting elements, if any.

Adjust SVG attributes for the token. (This fixes the case of SVG attributes that are not all lowercase.)

Adjust foreign attributes for the token. (This fixes the use of namespaced attributes, in particular XLink in SVG.)

Insert a foreign element for the token, in the SVG namespace.

If the token has its self-closing flag set, pop the current node off the stack of open elements and acknowledge the token’s *self-closing flag*.

A start tag whose tag name is one of: "caption", "col", "colgroup", "frame", "head", "tbody", "td", "tfoot", "th", "thead", "tr"
Parse error.

Any other start tag
Reconstruct the active formatting elements, if any.

Insert an HTML element for the token.

This element will be an ordinaryelement.

Any other end tag
Run these steps:

Initialize node to be the current node (the bottommost node of the stack).

Loop: If node is an HTML element with the same tag name as the token, then:

Generate implied end tags, except for HTML elements with the same tag name as the token.

If node is not the current node, then this is a parse error.

Pop all the nodes from the current node up to node, including node, then stop these steps.

Otherwise, if node is in the special category, then this is a parse error; ignore the token, and abort these steps.

Set node to the previous entry in the stack of open elements.

Return to the step labeled Loop.

When the steps above say the UA is to close a p element, it means that the UA must run the following steps:

Generate implied end tags, except for p elements.

If the current node is not a p element, then this is a parse error.

Pop elements from the stack of open elements until a p element has been popped from the stack.

The adoption agency algorithm, which takes as its only argument a token token for which the algorithm is being run, consists of the following steps:

Let subject be token’s tag name.

If the current node is an HTML element whose tag name is subject, and the current node is not in the list of active formatting elements, then pop the current node off the stack of open elements, and abort these steps.

Let outer loop counter be zero.

Outer loop: If outer loop counter is greater than or equal to eight, then abort these steps.

Increment outer loop counter by one.

Let formatting element be the last element in the list of active formatting elements that:

is between the end of the list and the last marker in the list, if any, or the start of the list otherwise, and

has the tag name subject.

If there is no such element, then abort these steps and instead act as described in the "any other end tag" entry above.

If formatting element is not in the stack of open elements, then this is a parse error; remove the element from the list, and abort these steps.

If formatting element is in the stack of open elements, but the element is not in scope, then this is a parse error; abort these steps.

If formatting element is not the current node, this is a parse error. (But do not abort these steps.)

Let furthest block be the topmost node in the stack of open elements that is lower in the stack than formatting element, and is an element in the special category. There might not be one.

If there is no furthest block, then the UA must first pop all the nodes from the bottom of the stack of open elements, from the current node up to and including formatting element, then remove formatting element from the list of active formatting elements, and finally abort these steps.

Let common ancestor be the element immediately above formatting element in the stack of open elements.

Let a bookmark note the position of formatting element in the list of active formatting elements relative to the elements on either side of it in the list.

Let node and last node be furthest block. Follow these steps:

Let inner loop counter be zero.

Inner loop: Increment inner loop counter by one.

Let node be the element immediately above node in the stack of open elements, or if node is no longer in the stack of open elements (e.g., because it got removed by this algorithm), the element that was immediately above node in the stack of open elements before node was removed.

If node is formatting element, then go to the next step in the overall algorithm.

If inner loop counter is greater than three and node is in the list of active formatting elements, then remove node from the list of active formatting elements.

If node is not in the list of active formatting elements, then remove node from the stack of open elements and then go back to the step labeled Inner loop.

Create an element for the token for which the element node was created, in the HTML namespace, with common ancestor as the intended parent; replace the entry for node in the list of active formatting elements with an entry for the new element, replace the entry for node in the stack of open elements with an entry for the new element, and let node be the new element.

If last node is furthest block, then move the aforementioned bookmark to be immediately after the new node in the list of active formatting elements.

Insert last node into node, first removing it from its previous parent node if any.

Let last node be node.

Return to the step labeled Inner loop.

Insert whatever last node ended up being in the previous step at the appropriate place for inserting a node, but using common ancestor as the override target.

Create an element for the token for which formatting element was created, in the HTML namespace, with furthest block as the intended parent.

Take all of the child nodes of furthest block and append them to the element created in the last step.

Append that new element to furthest block.

Remove formatting element from the list of active formatting elements, and insert the new element into the list of active formatting elements at the position of the aforementioned bookmark.

Remove formatting element from the stack of open elements, and insert the new element into the stack of open elements immediately below the position of furthest block in that stack.

Jump back to the step labeled Outer loop.

This algorithm’s name, the "adoption agency algorithm", comes from the way it causes elements to change parents, and is in contrast with other possible algorithms for dealing with misnested content, which included the "incest algorithm", the "secret affair algorithm", and the "Heisenberg algorithm".

8.2.5.4.8. The "text" insertion mode
When the user agent is to apply the rules for the "text" insertion mode, the user agent must handle the token as follows:

A character token
Insert the token’s character.

This can never be a U+0000 NULL character; the tokenizer converts those to U+FFFD REPLACEMENT CHARACTER characters.

An end-of-file token
Parse error.

If the current node is a script element, mark the script element as "already started".

Pop the current node off the stack of open elements.

Switch the insertion mode to the original insertion mode and reprocess the token.

An end tag whose tag name is "script"
If the JavaScript execution context stack is empty, perform a microtask checkpoint.

Let script be the current node (which will be a script element).

Pop the current node off the stack of open elements.

Switch the insertion mode to the original insertion mode.

Let the old insertion point have the same value as the current insertion point. Let the insertion point be just before the next input character.

Increment the parser’s script nesting level by one.

Prepare the script. This might cause some script to execute, which might cause new characters to be inserted into the tokenizer, and might cause the tokenizer to output more tokens, resulting in a reentrant invocation of the parser.

Decrement the parser’s script nesting level by one. If the parser’s script nesting level is zero, then set the parser pause flag to false.

Let the insertion point have the value of the old insertion point. (In other words, restore the insertion point to its previous value. This value might be the "undefined" value.)

At this stage, if there is a pending parsing-blocking script, then:

If the script nesting level is not zero:
Set the parser pause flag to true, and abort the processing of any nested invocations of the tokenizer, yielding control back to the caller. (Tokenization will resume when the caller returns to the "outer" tree construction stage.)

The tree construction stage of this particular parser is being called reentrantly, say from a call to document.write().

Otherwise:
Run these steps:

Let the script be the pending parsing-blocking script. There is no longer a pending parsing-blocking script.

Block the tokenizer for this instance of the HTML parser, such that the event loop will not run tasks that invoke the tokenizer.

If the parser’s Document has a style sheet that is blocking scripts or the script’s "ready to be parser-executed" flag is not set: spin the event loop until the parser’s Document has no style sheet that is blocking scripts and the script’s "ready to be parser-executed" flag is set.

If this parser has been aborted in the meantime, abort these steps.

This could happen if, e.g., while the spin the event loop algorithm is running, the browsing context gets closed, or the document.open() method gets invoked on the Document.

Unblock the tokenizer for this instance of the HTML parser, such that tasks that invoke the tokenizer can again be run.

Let the insertion point be just before the next input character.

Increment the parser’s script nesting level by one (it should be zero before this step, so this sets it to one).

Execute the script.

Decrement the parser’s script nesting level by one. If the parser’s script nesting level is zero (which it always should be at this point), then set the parser pause flag to false.

Let the insertion point be undefined again.

If there is once again a pending parsing-blocking script, then repeat these steps from step 1.

Any other end tag
Pop the current node off the stack of open elements.

Switch the insertion mode to the original insertion mode.

8.2.5.4.9. The "in table" insertion mode
When the user agent is to apply the rules for the "in table" insertion mode, the user agent must handle the token as follows:

A character token, if the current node is table, tbody, tfoot, thead, or tr element
Let the pending table character tokens be an empty list of tokens.

Let the original insertion mode be the current insertion mode.

Switch the insertion mode to "in table text" and reprocess the token.

A comment token
Insert a comment.

A DOCTYPE token
Parse error.

A start tag whose tag name is "caption"
Clear the stack back to a table context. (See below.)

Insert a marker at the end of the list of active formatting elements.

Insert an HTML element for the token, then switch the insertion mode to "in caption".

A start tag whose tag name is "colgroup"
Clear the stack back to a table context. (See below.)

Insert an HTML element for the token, then switch the insertion mode to "in column group".

A start tag whose tag name is "col"
Clear the stack back to a table context. (See below.)

Insert an HTML element for a "colgroup" start tag token with no attributes, then switch the insertion mode to "in column group".

Reprocess the current token.

A start tag whose tag name is one of: "tbody", "tfoot", "thead"
Clear the stack back to a table context. (See below.)

Insert an HTML element for the token, then switch the insertion mode to "in table body".

A start tag whose tag name is one of: "td", "th", "tr"
Clear the stack back to a table context. (See below.)

Insert an HTML element for a "tbody" start tag token with no attributes, then switch the insertion mode to "in table body".

Reprocess the current token.

A start tag whose tag name is "table"
Parse error.

If the stack of open elements does not have a table element in table scope, ignore the token.

Otherwise:

Pop elements from this stack until a table element has been popped from the stack.

Reset the insertion mode appropriately.

Reprocess the token.

An end tag whose tag name is "table"
If the stack of open elements does not have a table element in table scope, this is a parse error; ignore the token.

Otherwise:

Pop elements from this stack until a table element has been popped from the stack.

Reset the insertion mode appropriately.

An end tag whose tag name is one of: "body", "caption", "col", "colgroup", "html", "tbody", "td", "tfoot", "th", "thead", "tr"
Parse error.

A start tag whose tag name is one of: "style", "script", "template"
An end tag whose tag name is "template"
Process the token using the rules for the "in head" insertion mode.

A start tag whose tag name is "input"
If the token does not have an attribute with the name "type", or if it does, but that attribute’s value is not an ASCII case-insensitive match for the string "hidden", then: act as described in the "anything else" entry below.

Otherwise:

Parse error.

Insert an HTML element for the token.

Pop that input element off the stack of open elements.

Acknowledge the token’s *self-closing flag*, if it is set.

A start tag whose tag name is "form"
Parse error.

If there is a template element on the stack of open elements, or if the form element pointer is not null, ignore the token.

Otherwise:

Insert an HTML element for the token, and set the form element pointer to point to the element created.

Pop that form element off the stack of open elements.

An end-of-file token
Process the token using the rules for the "in body" insertion mode.

Anything else
Parse error. Enable foster parenting, process the token using the rules for the "in body" insertion mode, and then disable foster parenting.

When the steps above require the UA to clear the stack back to a table context, it means that the UA must, while the current node is not a table, template, or html element, pop elements from the stack of open elements.

This is the same list of elements as used in the has an element in table scope steps.

The current node being an html element after this process is a fragment case.

8.2.5.4.10. The "in table text" insertion mode
When the user agent is to apply the rules for the "in table text" insertion mode, the user agent must handle the token as follows:

A character token that is U+0000 NULL
Parse error.

Any other character token
Append the character token to the pending table character tokens list.

Anything else
If any of the tokens in the pending table character tokens list are character tokens that are not space characters, then this is a parse error: reprocess the character tokens in the pending table character tokens list using the rules given in the "anything else" entry in the "in table" insertion mode.

Otherwise, insert the characters given by the pending table character tokens list.

Switch the insertion mode to the original insertion mode and reprocess the token.

8.2.5.4.11. The "in caption" insertion mode
When the user agent is to apply the rules for the "in caption" insertion mode, the user agent must handle the token as follows:

An end tag whose tag name is "caption"
If the stack of open elements does not have a caption element in table scope, this is a parse error; ignore the token. (fragment case)

Otherwise:

Generate implied end tags.

Now, if the current node is not a caption element, then this is a parse error.

Pop elements from this stack until a caption element has been popped from the stack.

Clear the list of active formatting elements up to the last marker.

Switch the insertion mode to "in table".

A start tag whose tag name is one of: "caption", "col", "colgroup", "tbody", "td", "tfoot", "th", "thead", "tr"
An end tag whose tag name is "table"
If the stack of open elements does not have a caption element in table scope, this is a parse error; ignore the token. (fragment case)

Otherwise:

Generate implied end tags.

Now, if the current node is not a caption element, then this is a parse error.

Pop elements from this stack until a caption element has been popped from the stack.

Clear the list of active formatting elements up to the last marker.

Switch the insertion mode to "in table".

Reprocess the token.

An end tag whose tag name is one of: "body", "col", "colgroup", "html", "tbody", "td", "tfoot", "th", "thead", "tr"
Parse error.

Anything else
Process the token using the rules for the "in body" insertion mode.

8.2.5.4.12. The "in column group" insertion mode
When the user agent is to apply the rules for the "in column group" insertion mode, the user agent must handle the token as follows:

A character token that is one of U+0009 CHARACTER TABULATION, U+000A LINE FEED (LF), U+000C FORM FEED (FF), U+000D CARRIAGE RETURN (CR), or U+0020 SPACE
Insert the character.

A comment token
Insert a comment.

A DOCTYPE token
Parse error.

A start tag whose tag name is "html"
Process the token using the rules for the "in body" insertion mode.

A start tag whose tag name is "col"
Insert an HTML element for the token. Immediately pop the current node off the stack of open elements.

Acknowledge the token’s *self-closing flag*, if it is set.

An end tag whose tag name is "colgroup"
If the current node is not a colgroup element, then this is a parse error; ignore the token.

Otherwise, pop the current node from the stack of open elements. Switch the insertion mode to "in table".

An end tag whose tag name is "col"
Parse error.

A start tag whose tag name is "template"
An end tag whose tag name is "template"
Process the token using the rules for the "in head" insertion mode.

An end-of-file token
Process the token using the rules for the "in body" insertion mode.

Anything else
If the current node is not a colgroup element, then this is a parse error; ignore the token.

Otherwise, pop the current node from the stack of open elements.

Switch the insertion mode to "in table".

Reprocess the token.

8.2.5.4.13. The "in table body" insertion mode
When the user agent is to apply the rules for the "in table body" insertion mode, the user agent must handle the token as follows:

A start tag whose tag name is "tr"
Clear the stack back to a table body context. (See below.)

Insert an HTML element for the token, then switch the insertion mode to "in row".

A start tag whose tag name is one of: "th", "td"
Parse error.

Clear the stack back to a table body context. (See below.)

Insert an HTML element for a "tr" start tag token with no attributes, then switch the insertion mode to "in row".

Reprocess the current token.

An end tag whose tag name is one of: "tbody", "tfoot", "thead"
If the stack of open elements does not have an element in table scope that is an HTML element with the same tag name as the token, this is a parse error; ignore the token.

Otherwise:

Clear the stack back to a table body context. (See below.)

Pop the current node from the stack of open elements. Switch the insertion mode to "in table".

A start tag whose tag name is one of: "caption", "col", "colgroup", "tbody", "tfoot", "thead"
An end tag whose tag name is "table"
If the stack of open elements does not have a tbody, thead, or tfoot element in table scope, this is a parse error; ignore the token.

Otherwise:

Clear the stack back to a table body context. (See below.)

Pop the current node from the stack of open elements. Switch the insertion mode to "in table".

Reprocess the token.

An end tag whose tag name is one of: "body", "caption", "col", "colgroup", "html", "td", "th", "tr"
Parse error.

Anything else
Process the token using the rules for the "in table" insertion mode.

When the steps above require the UA to clear the stack back to a table body context, it means that the UA must, while the current node is not a tbody, tfoot, thead, template, or html element, pop elements from the stack of open elements.

The current node being an html element after this process is a fragment case.

8.2.5.4.14. The "in row" insertion mode
When the user agent is to apply the rules for the "in row" insertion mode, the user agent must handle the token as follows:

A start tag whose tag name is one of: "th", "td"
Clear the stack back to a table row context. (See below.)

Insert an HTML element for the token, then switch the insertion mode to "in cell".

Insert a marker at the end of the list of active formatting elements.

An end tag whose tag name is "tr"
If the stack of open elements does not have a tr element in table scope, this is a parse error; ignore the token.

Otherwise:

Clear the stack back to a table row context. (See below.)

Pop the current node (which will be a tr element) from the stack of open elements. Switch the insertion mode to "in table body".

A start tag whose tag name is one of: "caption", "col", "colgroup", "tbody", "tfoot", "thead", "tr"
An end tag whose tag name is "table"
If the stack of open elements does not have a tr element in table scope, this is a parse error; ignore the token.

Otherwise:

Clear the stack back to a table row context. (See below.)

Pop the current node (which will be a tr element) from the stack of open elements. Switch the insertion mode to "in table body".

Reprocess the token.

An end tag whose tag name is one of: "tbody", "tfoot", "thead"
If the stack of open elements does not have an element in table scope that is an HTML element with the same tag name as the token, this is a parse error; ignore the token.

If the stack of open elements does not have a tr element in table scope, ignore the token.

Otherwise:

Clear the stack back to a table row context. (See below.)

Pop the current node (which will be a tr element) from the stack of open elements. Switch the insertion mode to "in table body".

Reprocess the token.

An end tag whose tag name is one of: "body", "caption", "col", "colgroup", "html", "td", "th"
Parse error.

Anything else
Process the token using the rules for the "in table" insertion mode.

When the steps above require the UA to clear the stack back to a table row context, it means that the UA must, while the current node is not a tr, template, or html element, pop elements from the stack of open elements.

The current node being an html element after this process is a fragment case.

8.2.5.4.15. The "in cell" insertion mode
When the user agent is to apply the rules for the "in cell" insertion mode, the user agent must handle the token as follows:

An end tag whose tag name is one of: "td", "th"
If the stack of open elements does not have an element in table scope that is an HTML element with the same tag name as that of the token, then this is a parse error; ignore the token.

Otherwise:

Generate implied end tags.

Now, if the current node is not an HTML element with the same tag name as the token, then this is a parse error.

Pop elements from the stack of open elements stack until an HTML element with the same tag name as the token has been popped from the stack.

Clear the list of active formatting elements up to the last marker.

Switch the insertion mode to "in row".

A start tag whose tag name is one of: "caption", "col", "colgroup", "tbody", "td", "tfoot", "th", "thead", "tr"
If the stack of open elements does not have a td or th element in table scope, then this is a parse error; ignore the token. (fragment case)

Otherwise, close the cell (see below) and reprocess the token.

An end tag whose tag name is one of: "body", "caption", "col", "colgroup", "html"
Parse error.

An end tag whose tag name is one of: "table", "tbody", "tfoot", "thead", "tr"
If the stack of open elements does not have an element in table scope that is an HTML element with the same tag name as that of the token, then this is a parse error; ignore the token.

Otherwise, close the cell (see below) and reprocess the token.

Anything else
Process the token using the rules for the "in body" insertion mode.

Where the steps above say to close the cell, they mean to run the following algorithm:

Generate implied end tags.

If the current node is not now a td element or a th element, then this is a parse error.

Pop elements from the stack of open elements stack until a td element or a th element has been popped from the stack.

Clear the list of active formatting elements up to the last marker.

Switch the insertion mode to "in row".

The stack of open elements cannot have both a td and a th element in table scope at the same time, nor can it have neither when the close the cell algorithm is invoked.

8.2.5.4.16. The "in select" insertion mode
When the user agent is to apply the rules for the "in select" insertion mode, the user agent must handle the token as follows:

A character token that is U+0000 NULL
Parse error.

Any other character token
Insert the token’s character.

A comment token
Insert a comment.

A DOCTYPE token
Parse error.

A start tag whose tag name is "html"
Process the token using the rules for the "in body" insertion mode.

A start tag whose tag name is "option"
If the current node is an option element, pop that node from the stack of open elements.

Insert an HTML element for the token.

A start tag whose tag name is "optgroup"
If the current node is an option element, pop that node from the stack of open elements.

If the current node is an optgroup element, pop that node from the stack of open elements.

Insert an HTML element for the token.

An end tag whose tag name is "optgroup"
First, if the current node is an option element, and the node immediately before it in the stack of open elements is an optgroup element, then pop the current node from the stack of open elements.

If the current node is an optgroup element, then pop that node from the stack of open elements. Otherwise, this is a parse error; ignore the token.

An end tag whose tag name is "option"
If the current node is an option element, then pop that node from the stack of open elements. Otherwise, this is a parse error; ignore the token.

An end tag whose tag name is "select"
If the stack of open elements does not have a select element in select scope, this is a parse error; ignore the token. (fragment case)

Otherwise:

Pop elements from the stack of open elements until a select element has been popped from the stack.

Reset the insertion mode appropriately.

A start tag whose tag name is "select"
Parse error.

If the stack of open elements does not have a select element in select scope, ignore the token. (fragment case)

Otherwise:

Pop elements from the stack of open elements until a select element has been popped from the stack.

Reset the insertion mode appropriately.

Note: It just gets treated like an end tag.

A start tag whose tag name is one of: "input", "textarea"
Parse error.

If the stack of open elements does not have a select element in select scope, ignore the token. (fragment case)

Otherwise:

Pop elements from the stack of open elements until a select element has been popped from the stack.

Reset the insertion mode appropriately.

Reprocess the token.

A start tag whose tag name is one of: "script", "template"
An end tag whose tag name is "template"
Process the token using the rules for the "in head" insertion mode.

An end-of-file token
Process the token using the rules for the "in body" insertion mode.

Anything else
Parse error.

8.2.5.4.17. The "in select in table" insertion mode
When the user agent is to apply the rules for the "in select in table" insertion mode, the user agent must handle the token as follows:

A start tag whose tag name is one of: "caption", "table", "tbody", "tfoot", "thead", "tr", "td", "th"
Parse error.

Pop elements from the stack of open elements until a select element has been popped from the stack.

Reset the insertion mode appropriately.

Reprocess the token.

An end tag whose tag name is one of: "caption", "table", "tbody", "tfoot", "thead", "tr", "td", "th"
Parse error.

If the stack of open elements does not have an element in table scope that is an HTML element with the same tag name as that of the token, then ignore the token.

Otherwise:

Pop elements from the stack of open elements until a select element has been popped from the stack.

Reset the insertion mode appropriately.

Reprocess the token.

Anything else
Process the token using the rules for the "in select" insertion mode.

8.2.5.4.18. The "in template" insertion mode
When the user agent is to apply the rules for the "in template" insertion mode, the user agent must handle the token as follows:

A character token
A comment token
A DOCTYPE token
Process the token using the rules for the "in body" insertion mode.

A start tag whose tag name is one of: "base", "basefont", "bgsound", "link", "meta", "noframes", "script", "style", "template", "title"
An end tag whose tag name is "template"
Process the token using the rules for the "in head" insertion mode.

A start tag whose tag name is one of: "caption", "colgroup", "tbody", "tfoot", "thead"
Pop the current template insertion mode off the stack of template insertion modes.

Push "in table" onto the stack of template insertion modes so that it is the new current template insertion mode.

Switch the insertion mode to "in table", and reprocess the token.

A start tag whose tag name is "col"
Pop the current template insertion mode off the stack of template insertion modes.

Push "in column group" onto the stack of template insertion modes so that it is the new current template insertion mode.

Switch the insertion mode to "in column group", and reprocess the token.

A start tag whose tag name is "tr"
Pop the current template insertion mode off the stack of template insertion modes.

Push "in table body" onto the stack of template insertion modes so that it is the new current template insertion mode.

Switch the insertion mode to "in table body", and reprocess the token.

A start tag whose tag name is one of: "td", "th"
Pop the current template insertion mode off the stack of template insertion modes.

Push "in row" onto the stack of template insertion modes so that it is the new current template insertion mode.

Switch the insertion mode to "in row", and reprocess the token.

Any other start tag
Pop the current template insertion mode off the stack of template insertion modes.

Push "in body" onto the stack of template insertion modes so that it is the new current template insertion mode.

Switch the insertion mode to "in body", and reprocess the token.

Any other end tag
Parse error.

An end-of-file token
If there is no template element on the stack of open elements, then stop parsing. (fragment case)

Otherwise, this is a parse error.

Pop elements from the stack of open elements until a template element has been popped from the stack.

Clear the list of active formatting elements up to the last marker.

Pop the current template insertion mode off the stack of template insertion modes.

Reset the insertion mode appropriately.

Reprocess the token.

8.2.5.4.19. The "after body" insertion mode
When the user agent is to apply the rules for the "after body" insertion mode, the user agent must handle the token as follows:

A character token that is one of U+0009 CHARACTER TABULATION, U+000A LINE FEED (LF), U+000C FORM FEED (FF), U+000D CARRIAGE RETURN (CR), or U+0020 SPACE
Process the token using the rules for the "in body" insertion mode.

A comment token
Insert a comment as the last child of the first element in the stack of open elements (the html element).

A DOCTYPE token
Parse error.

A start tag whose tag name is "html"
Process the token using the rules for the "in body" insertion mode.

An end tag whose tag name is "html"
If the parser was originally created as part of the HTML fragment parsing algorithm, this is a parse error; ignore the token. (fragment case)

Otherwise, switch the insertion mode to "after after body".

An end-of-file token
Stop parsing.

Anything else
Parse error. Switch the insertion mode to "in body" and reprocess the token.

8.2.5.4.20. The "in frameset" insertion mode
When the user agent is to apply the rules for the "in frameset" insertion mode, the user agent must handle the token as follows:

A character token that is one of U+0009 CHARACTER TABULATION, U+000A LINE FEED (LF), U+000C FORM FEED (FF), U+000D CARRIAGE RETURN (CR), or U+0020 SPACE
Insert the character.

A comment token
Insert a comment.

A DOCTYPE token
Parse error.

A start tag whose tag name is "html"
Process the token using the rules for the "in body" insertion mode.

A start tag whose tag name is "frameset"
Insert an HTML element for the token.

An end tag whose tag name is "frameset"
If the current node is the root html element, then this is a parse error; ignore the token. (fragment case)

Otherwise, pop the current node from the stack of open elements.

If the parser was not originally created as part of the HTML fragment parsing algorithm (fragment case), and the current node is no longer a frameset element, then switch the insertion mode to "after frameset".

A start tag whose tag name is "frame"
Insert an HTML element for the token. Immediately pop the current node off the stack of open elements.

Acknowledge the token’s *self-closing flag*, if it is set.

A start tag whose tag name is "noframes"
Process the token using the rules for the "in head" insertion mode.

An end-of-file token
If the current node is not the root html element, then this is a parse error.

The current node can only be the root html element in the fragment case.

Stop parsing.

Anything else
Parse error.

8.2.5.4.21. The "after frameset" insertion mode
When the user agent is to apply the rules for the "after frameset" insertion mode, the user agent must handle the token as follows:

A character token that is one of U+0009 CHARACTER TABULATION, U+000A LINE FEED (LF), U+000C FORM FEED (FF), U+000D CARRIAGE RETURN (CR), or U+0020 SPACE
Insert the character.

A comment token
Insert a comment.

A DOCTYPE token
Parse error.

A start tag whose tag name is "html"
Process the token using the rules for the "in body" insertion mode.

An end tag whose tag name is "html"
Switch the insertion mode to "after after frameset".

A start tag whose tag name is "noframes"
Process the token using the rules for the "in head" insertion mode.

An end-of-file token
Stop parsing.

Anything else
Parse error.

8.2.5.4.22. The "after after body" insertion mode
When the user agent is to apply the rules for the "after after body" insertion mode, the user agent must handle the token as follows:

A comment token
Insert a comment as the last child of the Document object.

A DOCTYPE token
A character token that is one of U+0009 CHARACTER TABULATION, U+000A LINE FEED (LF), U+000C FORM FEED (FF), U+000D CARRIAGE RETURN (CR), or U+0020 SPACE
A start tag whose tag name is "html"
Process the token using the rules for the "in body" insertion mode.

An end-of-file token
Stop parsing.

Anything else
Parse error. Switch the insertion mode to "in body" and reprocess the token.

8.2.5.4.23. The "after after frameset" insertion mode
When the user agent is to apply the rules for the "after after frameset" insertion mode, the user agent must handle the token as follows:

A comment token
Insert a comment as the last child of the Document object.

A DOCTYPE token
A character token that is one of U+0009 CHARACTER TABULATION, U+000A LINE FEED (LF), U+000C FORM FEED (FF), U+000D CARRIAGE RETURN (CR), or U+0020 SPACE
A start tag whose tag name is "html"
Process the token using the rules for the "in body" insertion mode.

An end-of-file token
Stop parsing.

A start tag whose tag name is "noframes"
Process the token using the rules for the "in head" insertion mode.

Anything else
Parse error.

8.2.5.5. The rules for parsing tokens in foreign content
When the user agent is to apply the rules for parsing tokens in foreign content, the user agent must handle the token as follows:

A character token that is U+0000 NULL
Parse error. Insert a U+FFFD REPLACEMENT CHARACTER character.

A character token that is one of U+0009 CHARACTER TABULATION, U+000A LINE FEED (LF), U+000C FORM FEED (FF), U+000D CARRIAGE RETURN (CR), or U+0020 SPACE
Insert the token’s character.

Any other character token
Insert the token’s character.

Set the frameset-ok flag to "not ok".

A comment token
Insert a comment.

A DOCTYPE token
Parse error.

A start tag whose tag name is one of: "b", "big", "blockquote", "body", "br", "center", "code", "dd", "div", "dl", "dt", "em", "embed", "h1", "h2", "h3", "h4", "h5", "h6", "head", "hr", "i", "img", "li", "listing", "meta", "nobr", "ol", "p", "pre", "ruby", "s", "small", "span", "strong", "strike", "sub", "sup", "table", "tt", "u", "ul", "var"
A start tag whose tag name is "font", if the token has any attributes named "color", "face", or "size"
Parse error.

If the parser was originally created for the HTML fragment parsing algorithm, then act as described in the "any other start tag" entry below. (fragment case)

Otherwise:

Pop an element from the stack of open elements, and then keep popping more elements from the stack of open elements until the current node is a MathML text integration point, an HTML integration point, or an element in the HTML namespace.

Then, reprocess the token.

Any other start tag
If the adjusted current node is an element in the MathML namespace, adjust MathML attributes for the token. (This fixes the case of MathML attributes that are not all lowercase.)

If the adjusted current node is an element in the SVG namespace, and the token’s tag name is one of the ones in the first column of the following table, change the tag name to the name given in the corresponding cell in the second column. (This fixes the case of SVG elements that are not all lowercase.)

Tag name    Element name
altglyph    altGlyph
altglyphdef altGlyphDef
altglyphitem    altGlyphItem
animatecolor    animateColor
animatemotion   animateMotion
animatetransform    animateTransform
clippath    clipPath
feblend feBlend
fecolormatrix   feColorMatrix
fecomponenttransfer feComponentTransfer
fecomposite feComposite
feconvolvematrix    feConvolveMatrix
fediffuselighting   feDiffuseLighting
fedisplacementmap   feDisplacementMap
fedistantlight  feDistantLight
fedropshadow    feDropShadow
feflood feFlood
fefunca feFuncA
fefuncb feFuncB
fefuncg feFuncG
fefuncr feFuncR
fegaussianblur  feGaussianBlur
feimage feImage
femerge feMerge
femergenode feMergeNode
femorphology    feMorphology
feoffset    feOffset
fepointlight    fePointLight
fespecularlighting  feSpecularLighting
fespotlight feSpotLight
fetile  feTile
feturbulence    feTurbulence
foreignobject   foreignObject
glyphref    glyphRef
lineargradient  linearGradient
radialgradient  radialGradient
textpath    textPath
If the adjusted current node is an element in the SVG namespace, adjust SVG attributes for the token. (This fixes the case of SVG attributes that are not all lowercase.)

Adjust foreign attributes for the token. (This fixes the use of namespaced attributes, in particular XLink in SVG.)

Insert a foreign element for the token, in the same namespace as the adjusted current node.

If the token has its self-closing flag set, then run the appropriate steps from the following list:

If the token’s tag name is "script", and the new current node is in the SVG namespace
Acknowledge the token’s *self-closing flag*, and then act as described in the steps for a "script" end tag below.

Otherwise
Pop the current node off the stack of open elements and acknowledge the token’s *self-closing flag*.

An end tag whose tag name is "script", if the current node is an SVG script element
Pop the current node off the stack of open elements.

Let the old insertion point have the same value as the current insertion point. Let the insertion point be just before the next input character.

Increment the parser’s script nesting level by one. Set the parser pause flag to true.

Process the SVG script element according to the SVG rules, if the user agent supports SVG.  [SVG11]

Even if this causes new characters to be inserted into the tokenizer, the parser will not be executed reentrantly, since the parser pause flag is true.

Decrement the parser’s script nesting level by one. If the parser’s script nesting level is zero, then set the parser pause flag to false.

Let the insertion point have the value of the old insertion point. (In other words, restore the insertion point to its previous value. This value might be the "undefined" value.)

Any other end tag
Run these steps:

Initialize node to be the current node (the bottommost node of the stack).

If node’s tag name, converted to ASCII lowercase, is not the same as the tag name of the token, then this is a parse error.

Loop: If node is the topmost element in the stack of open elements, abort these steps. (fragment case)

If node’s tag name, converted to ASCII lowercase, is the same as the tag name of the token, pop elements from the stack of open elements until node has been popped from the stack, and then abort these steps.

Set node to the previous entry in the stack of open elements.

If node is not an element in the HTML namespace, return to the step labeled Loop.

Otherwise, process the token according to the rules given in the section corresponding to the current insertion mode in HTML content.
