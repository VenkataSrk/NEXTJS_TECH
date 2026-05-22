#ifndef XGATE_XMLDOM_PARSER
#define XGATEXMLDOM_PARSER
#include  "xGateRapidXML.h"
class DOMParser : public xGateRapidXML{
public:
 DOMParser();
~DOMParser();
// load the document 
int  LoadDocument(const char* xmlString);

};
#endif
