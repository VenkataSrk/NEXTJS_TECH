#ifndef _XGATE_READ_CONFIG_FROM_PROXY_CONF_FILE_H
#define _XGATE_READ_CONFIG_FROM_PROXY_CONF_FILE_H

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>



class GlobalClass
{
	static GlobalClass *s_instance;
    GlobalClass()
    {
		m_PBX_A="";
		m_PBX_B="";
		m_DEC_RESAMPLER=0;
		m_DEC_CONVERTER=0;
		m_G729_DEC_AUDIORATE=0;
		m_G729_DEC_AUDIORESAMPLER=0;
		m_G729_DEC_AUDIOCONVERTER=0;
                m_AUDIT_MAX_RETRY=0;
		m_AUDIT_START_TIME=0L;
		m_DTMF_LOW_DURATION= 0L;
    }
	
	GlobalClass(const GlobalClass &glc);
	GlobalClass &operator=(const GlobalClass &rhs); 
	
  public:

	        std::string m_PBX_A;
                std::string m_PBX_B;
                int m_DEC_RESAMPLER;
                int m_DEC_CONVERTER;
                int m_G729_DEC_AUDIORATE;
                int m_G729_DEC_AUDIORESAMPLER;
                int m_G729_DEC_AUDIOCONVERTER;
                int m_AUDIT_MAX_RETRY;
                unsigned long m_AUDIT_START_TIME;
                unsigned long m_DTMF_LOW_DURATION;
                std::vector<std::string> m_PBXIpAddressList;
 
    static GlobalClass *instance()
    {
        if (!s_instance)
          s_instance = new GlobalClass;
        return s_instance;
    }

	int loadConfig();
};

extern GlobalClass *ConfigLoad;

#endif
