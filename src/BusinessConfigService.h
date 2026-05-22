#ifndef CA_BUSINESS_CONFIG_SERVICE_H
#define CA_BUSINESS_CONFIG_SERVICE_H

#include "BusinessConfigHandler.h"
//#include "BusinessConfigObject.h"
#include "BSOperations.h"
#include "CAFileNotifier.h"
class BusinessConfigService : public CA::CAThread
{
	private:
	BusinessConfigHandler m_configHandler;
	CAFileNotifier* cafNote;

	public:
		BusinessConfigService();
		virtual ~BusinessConfigService();

		bool m_run;
		bool init();
		virtual bool run();
		bool handle_msg(CA::CAMessageBlock* pConfigObj);
		bool loadGlobalConf();
		bool loadCallConf(CA::CAMessageBlock* pMsg);
		bool loadDBList(CA::CAMessageBlock* pMsg);
		bool loadAppList();
		bool postMessage(CA::CAMessageBlock*);
		bool StartBusinessServices(int bsService);
		bool reLoadConfig();
		bool LoadMVNOService();
		bool LoadBusinessProcessor();
		bool LoadLocalDBService();
		

		
};
#endif
