#ifndef CHARING_PARAM_OPERATION
#define CHARING_PARAM_OPERATION
#include "CAUtilityInternal.h"

    enum class PremiumDest{
        eAccAnxBal = 1,
        eFreeMinPAvaiBal = 2,
        eFreeMinOnly = 3,
        eCallpackMin = 4,
        eAbsoluteFreeDest = 5,
        eNotDefined1 = 6,
        eNotDefined2 = 7,
        eFreeSampleCharge = 8,
        eInaTariff = 9,
    };
    enum class CALL_TYPE{
    //call_type
    CAT_CALLBACK = 1,   //normal callback
    CAT_CALLTROUGH = 2,   //normal call
    CAT_I_CALLBACK = 3,   //callback international (ask cli)
    CAT_ISP = 4, 	//isp access call
    CAT_X_CALLBACK = 5,   //callback (save pin automatically-no menu-noplayblc)
    CAT_GOTOTV = 6,	//call routed directly to tv
    CAT_ONESTAGE = 7,   //one stage, normal call
    CAT_CALLROUTE = 8,   //callroute, ignore unnecessary processes like account, tariff, etc
    CAT_CALLPRO = 9,	//receive call from esppro, go to pipecspro 
    CAT_CALLCRO = 10,  //receive call from croute, go to pipes or atm 
    CAT_DIRECT_ACS = 11,  //direct access to a number
    CAT_MEETMECALL = 12,	//call to a meetmecall number
};
    enum class ACCESS_MODE{//access_mode     : integer;   //read>>> 'authorization mode'
    ACSM_COMBI = 1,  	//Combination of 2 & 4 (1:CliAcc/DIDani/Langset)
    ACSM_DID_ANI = 2,  	//did ani
    ACSM_ACCOUNT = 3,  	//TCBS account
    ACSM_PIN = 4,  	 	//pin & langset
    ACSM_SIP = 5,      	//sip
    ACSM_RES_SVC = 6,  	//oldresidensial service (aka. cliacc)
    ACSM_GD = 7,       	//Gnanam direct (defunct)
    ACSM_RES_LOCAL = 8,     //Residential Svc 
    ACSM_RES_ROAMING = 9,     //Residential Svc - Roaming
    ACSM_MENU = 10,		//Mode menu - can't call, only check bal, menu pair did in custcode column (for callback?)
    ACSM_FWD = 11,		//recieve call from forwarder
    ACSM_RES_PIN6 = 12,     //Residential  6 pincode
    ACSM_RES_LOCAL_CV = 13, //Residential  cv
    ACSM_RES_PIN6_CV = 14,  //Residential  cv /fnf etalk
    ACSM_MVNO_CLI = 15, 	//MVNO by cli/ocn/rn
    ACSM_MVNO_DID = 16, 	//MVNO by did
    ACSM_MVNO_PIN = 17, 	//MVNO by pin
    ACSM_SRSIP = 18,		//Switchless Reseller
    ACSM_BBD_HO = 19, 	//Handover access thru Rendezvous number
    ACSM_BBD_CLI = 20,  	//Access thru mobile for BBD account
    ACSM_BBD_LOGIN = 21, 	//Access thru CISCO for BBD account
    ACSM_MVNO_CLI_FREEDIAL = 22, //MVNO by cli/ocn/rn ... freedial
    ACSM_RES_ZERO2 = 23, //Residential Zero2 .. by CLI, direct DTMF
    ACSM_SME = 24, // Access thru SME from privete info
    ACSM_MVNO_IVR = 26, // Access thru IVR app.
    ACSM_TEST = 27, // Access TESTING
    ACSM_DID = 28, // Access using DID
    ACSM_RES_OSID = 29, // Access using residential CLI and OSID + auto register
    };
    class Access{
        public:
        int calltype;
        int didtype;
        int mode;
        int charge;
        std::string did;
        std::string telco_code;
        std::string accessType;
        std::string siteCode;
        Access()=default;
        virtual ~Access()=default;
    };
    class AccountMvnoV2{
        public:
        int card_id;
        double sellprice;
        int cardused;
        int actdiff;
        int actstatus;
        double excrate;
        std::string custcode; 
	    int batchcode; 
	    int serialcode;
        AccountMvnoV2()
        {
        card_id = 0;
        sellprice  = 0.0;
        cardused = 0;
        actdiff = 0;
        actstatus = 0;
        excrate = 0.0;
        std::string custcode = ""; 
	    batchcode = 0; 
	    serialcode = 0; 
        }
        virtual ~AccountMvnoV2()=default;
    };
    class AccountByCbs{
        public:
            float balance  ;
            int monthlim  ;
            std::string  langset  ;
            std::string tariffClass;
            long firstusg;
            long expdat;
            int status;
            int cardId;
            int billingMode;
            AccountByCbs()=default;
            ~AccountByCbs()=default;
    };
    class MvnoSubscriberInfo{
        public:
            std::string vlr  ;
            std::string msc  ;
            int isRoaming  ;
    };
    class Xlat{
        public:
        std::string xlatpfx  ;
        std::string prefix  ;
        int xlatnbtype  ;	
    };
    class CARD_FEATURE{
        public:
        CARD_FEATURE()=default;
        ~CARD_FEATURE()=default;
        int cardId  ;
        int featuretype  ;
        int status  ;
        std::string  param1  ;
        std::string param2  ;
        std::string param3  ;
    };
     class TariffClass{
        public:
        int prefix_set  ;
        int tclass_prem_routecls  ;
        std::string route_grade  ;
        double onego_charge  ;
        int xlatset;
        TariffClass()=default;
        virtual ~TariffClass()=default;
    };
    class PackTariff{
        public:
        PackTariff() = default;
        virtual ~PackTariff() = default;
        std::string tariffClass;
    };
    class MnpDetails{
        public:
        MnpDetails()=default;
        virtual ~MnpDetails() =  default;
        std::string mnpNum;
        std::string Nrn;
        std::string destcode; 
        std::string isPrefixCodeBlocked;
        std::string blockedprefixdestcode;
    };



    class chargingSharedData {
        public:
        chargingSharedData()=default;
        ~chargingSharedData()=default;
        double samplingrate;
        std::string xlatedDidnum;
        std::string  siteCode  ;
        std::string tariffClassA  ;
        std::string tariffClassB  ;
        std::string destCode  ;
        std::string custCode  ;
        std::string espdb  ;
        int batchCode  ;
        int serialCode  ;
        std::string accessType  ;
        int operatorSetRule  ;
        int operatorSet  ;
        std::string gradecode  ;
        int espRouteCls  ;
        std::string mnpNum  ;
        std::string firstusg  ;    
    };


    

    class PackageByMaster {
        public:
        PackageByMaster()=default;
       virtual ~PackageByMaster()=default;
        std::string custcode  ; 
        int batchcode  ; 
        int serialcode  ;
        int packageid  ; 
        int cardid  ; 
        int uselimitMode  ; 
        int uselimitDelay  ; 
        double uselimitamount  ; 
        int uselimitCall  ; 
        int uselimitminutes  ; 
        int ispackcnxcharge  ; 
        int packfreeflag  ; 
        int callMaxTalkTime  ; 
        int callMaxTalkTimeMode  ; 
        int isSubPackage  ; 
        double promobalRatio  ; 
        int usagentnotifMode  ; 
        int usagentnotifType  ; 
        std::string usagentnotifText  ; 
        int checkDest  ; 
        std::string prefixDest  ; 
        int prefixDestMode  ; 
        int prefixCliMode  ; 
        int checkCli  ; 
        std::string usagentnotifThreshold  ; 
        double pricePerUnit  ; 
        float balance  ; 
        float balanceused  ; 
        int breakageChargeMode  ; 
        int allowRoam  ;   
    };
    

    
    class byBrandXlat{
        public:
        std::string prefix_old  ;
        std::string prefix_new  ;
        byBrandXlat()=default;
        ~byBrandXlat()=default;

    };
    class Activation{
        public:
        Activation() = default;
        virtual ~Activation() = default;
        int isFree;
    };

    

    
   

    
    class TariffCode{
        public:
        int tariff_code  ;
        TariffCode()=default;
        ~TariffCode()=default;
    };

    class TimeClass{
        public:
        int time_prd  ;
        TimeClass()=default;
    };

    class GetTariff{
        public:
        GetTariff()=default;
        ~GetTariff()=default;
        float cnxunit  ;
        int preDest  ;
        int sampledelay  ;
        float sampleunit  ;
        int cnxdelay  ;
        unsigned short no_package;
        bool nobalance;
        
        
    };

    class topupTariff {
        public:
        topupTariff()=default;
        virtual ~topupTariff()=default;
        std::string tp_tariffclass  ;
        int tp_createdate  ;
        
    };

    class GetAuthorizeDest{
        public:
        GetAuthorizeDest()=default;
        ~GetAuthorizeDest()=default;
        int action ;
        std::string prompt_file  ;
        std::string errmsg  ;
        
        
    };

    class UpdateBalanceInfront{
        public:
        UpdateBalanceInfront()=default;
        ~UpdateBalanceInfront()=default;
        float balance  ;
        float balanceused  ;
        float totalcons  ;
        int cnxcount  ;
        
        
    };
    class PackageLimitV2 {
        public:
        PackageLimitV2()=default;
        ~PackageLimitV2()=default;
        bool updateRequired;
        double avail_balance  ;
        int avail_min  ;
        int avail_call  ;
        
        

    };

    class Es2AniInfo{
        public:
            Es2AniInfo()=default;
            ~Es2AniInfo()=default;
            int isactive;
            int feature1;
            int feature2;
    };

    class Es1ClicatInfo{
        public:
            Es1ClicatInfo()=default;
            ~Es1ClicatInfo()=default;
            int category;
            int charge;
            double surcharge;
            std::string type;
    };

#endif
