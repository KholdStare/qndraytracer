#ifndef _DATA_XML_PARSER_H_
#define _DATA_XML_PARSER_H_

#define FOREACH_ELEMENT_IN( ElementName ) for ( TiXmlElement* pChild = ElementName->FirstChildElement(); \
          pChild != 0; pChild = pChild->NextSiblingElement())

#define IF_CHILD_IS( TagName ) if(pChild->ValueStr().compare(TagName) == 0)

#define FOREACH_ATTRIBUTE_OF( ElementName ) for ( TiXmlAttribute* pAttrib = ElementName->FirstAttribute(); \
          pAttrib != 0; pAttrib = pAttrib->Next())

class TiXmlElement;

template <class DataType>
struct DataXmlParser {

    static DataType parse(TiXmlElement* dataElement);

};

#endif // _DATA_XML_PARSER_H_
