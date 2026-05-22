#ifndef ROUTING_PARAM_OPERATION
#define ROUTING_PARAM_OPERATION
#include "CAUtilityInternal.h"
class MRouteSharedData {
  public:
	  std::string outGoingTrunk;
    std::string outGoingGroup;
    std::string outGoingInterface;
    std::string terminationGetwayIP;
    std::string prefix;
    std::string curr_code;
    double callcost;
};

class Es3Xlat{
  public:
    std::string xlatpfx;
    std::string prefix;

};

class TdmIpAddress{
  public:
    std::string ipaddress;
    std::string interface;
    
};

class PrefixRefData{
  public:
    std::string prefixRef;
	  

};
class CodecDetails{
  public:
    std::string trunkcodec;
   

};

class CRouteCodecData{
  public:
	 
    bool is_codec_enabled;
    std::string trunkCodec;
};



class R25GetRoute{  
  public:
    double costprice;
    std::string access;
    int capability;
    std::string clsOrg;
    float cost;
    std::string domain;
    int exception;
    int ext;
    int flag;
    float grade;
    std::string group;
    std::string hint;
    int id;
    int intcls;
    std::string interface;
    int isactive;
    int oprtype;
    std::string pdomain;
    std::string prefixcode;
    int priority;
    int rating;
    int reason;
    int redlist;
    int routecls;
    std::string routeset;
    std::string timecls;
    std::string universe;
    std::string userinfo;
};
// Swiching Paramater
class XLatCallOut{
   public:
        std::string  xlatpfx;
        std::string prefix;
        int numbertype;
        int complaw;
        int nbplan;
        std::string signaddress;
        std::string callingnb;
        int callingcat;
        int callingnbtype;
        int didlen;
        int callingscreening;
        int presentation;
        int mindidlen;
        int conntype;

};


#endif

