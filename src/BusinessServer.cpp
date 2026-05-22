#include "BusinessServer.h"
MVNOBusinessService* BusinessServer::m_businessService =  nullptr;
 BusinessConfigService* BusinessServer::m_configService = nullptr;
 BSRequestProcessor* BusinessServer::m_bsrProcessor =  nullptr;
 DBService* BusinessServer::m_dbService =  nullptr;
 ChargingService* BusinessServer::m_chargingService =  nullptr;
 RoutingService* BusinessServer::m_routingService =  nullptr;
 BSmomtGatewayService* BusinessServer::m_momtService =  nullptr;
 BSLocalDBService* BusinessServer::m_localDBService =  nullptr;
 BSReportingService* BusinessServer::m_reportService = nullptr;
 BSHttpService* BusinessServer::m_httpService =  nullptr;
 GmscService* BusinessServer::m_gmscService = nullptr;
