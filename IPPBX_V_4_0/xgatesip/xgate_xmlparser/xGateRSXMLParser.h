/*
 * ******************************************************************************
 * Vectone Mobile
 * @file xGateRSXMLParser.h
 *
 * @brief This is RS Metadata XML Parser/Encoder header file.
 *
 * @author Narrain prithvi Dharuman
 * ******************************************************************************
 */ 
#ifndef XGATE_RS_XML_PARSER
#define XGATE_RS_XML_PARSER
#include <string>
#include <iostream>
#include "xGateRapidXML.h"
#include "xGateRSXMLParserUtil.h"

class xGateRSXMLParser : public xGateRapidXML {
public:
      xGateRSXMLParser();
      ~xGateRSXMLParser();
      bool loadParser(std::string payload);
      bool parseXMLInfo(SipRec_RsMetadata& rsMData);
      void serializeXMLInfo();
      bool decodeRecordingInfo(rapidxml::xml_node<>*node, SipRec_RsMetadata& rsMData);
      /*
      void decodeCSGroup(rapidxml::xml_node<>*node);
      void decodeCallingParty(rapidxml::xml_node<>*node);
      void decodeCalledParty(rapidxml::xml_node<>*node);
      */
      bool decodeDataMode(rapidxml::xml_node<>*node, SipRec_RsMetadata& rsMData);
      bool decodeCallType(rapidxml::xml_node<>*node, SipRec_RsMetadata& rsMData);
      bool decodeFromNumber(rapidxml::xml_node<>*node, SipRec_RsMetadata& rsMData);
      bool decodeFromCallType(rapidxml::xml_attribute<>*attribute,SipRec_RsMetadata& rsMData);
      bool decodeToCallType(rapidxml::xml_attribute<>*attribute,SipRec_RsMetadata& rsMData);
      bool decodeToNumber(rapidxml::xml_node<>*node, SipRec_RsMetadata& rsMData);
      bool decodeDomainName(rapidxml::xml_node<>*node, SipRec_RsMetadata& rsMData);
      bool decodeDomainId(rapidxml::xml_node<>*node, SipRec_RsMetadata& rsMData);
      bool decodeQExtn(rapidxml::xml_node<>*node, SipRec_RsMetadata& rsMData);
      bool decodeCcaasCallType(rapidxml::xml_node<>*node, SipRec_RsMetadata& rsMData);
      bool decodeCcaasRecFileName(rapidxml::xml_node<>*node, SipRec_RsMetadata& rsMData);
      bool decodeCcaasRecUrl(rapidxml::xml_node<>*node, SipRec_RsMetadata& rsMData);
      bool decodeSession(rapidxml::xml_node<>*node, SipRec_Session& sessionInfo);
      bool decodeParticipants(rapidxml::xml_node<>*node, SipRec_Participant& partiInfo);
      bool decodeStreams(rapidxml::xml_node<>*node, SipRec_Stream& streamInfo);
      bool decodeSessionRecordAssoc(rapidxml::xml_node<>*node,
                      SipRec_SessionRecAssoc& sessRecAssocInfo);
      bool decodePartiSessionAssoc(rapidxml::xml_node<>*node,
                      SipRec_PartiSessionAssoc& partiSessAssoacInfo);
      bool decodePartiStreamAssoc(rapidxml::xml_node<>*node,
                      SipRec_PartiStreamAssoc& partiStreamAssocInfo);
      void encodeRSMetadata( SipRec_RsMetadata rsMData );
      void encodeSessionInfo(rapidxml::xml_document<>& doc, rapidxml::xml_node<> *node,
                      SipRec_Session &sessionInfo);
      void encodeParticipantInfo(rapidxml::xml_document<>& doc, rapidxml::xml_node<> *node,
                      SipRec_Participant &partiInfo);
      void encodeStreamInfo(rapidxml::xml_document<>& doc, rapidxml::xml_node<> *node,
                      SipRec_Stream &streamInfo);
      void encodeSessionRecordAssoc(rapidxml::xml_document<>& doc, rapidxml::xml_node<> *node,
                      SipRec_SessionRecAssoc &sessRecAssocInfo);
      void encodePartiSessionAssoc(rapidxml::xml_document<>& doc, rapidxml::xml_node<> *node,
                      SipRec_PartiSessionAssoc &partiSessAssocInfo);
      void encodePartiStreamAssoc(rapidxml::xml_document<>& doc, rapidxml::xml_node<> *node,
                      SipRec_PartiStreamAssoc &partiStreamAssocInfo);
      void printParsedXML(SipRec_RsMetadata rsMData);
};

#endif
