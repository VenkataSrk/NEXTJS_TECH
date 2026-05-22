#ifndef _XGATE_READ_CONFIG_FROM_PROXY_CONF_FILE_H
#define _XGATE_READ_CONFIG_FROM_PROXY_CONF_FILE_H

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>


class GlobalClass
{

	
	
    static GlobalClass *s_instance;
    GlobalClass()
    {
		m_REGTAG="";
		m_PUBTAG="";
		m_URCBPUBTAG="";
		m_MBSERVERIP="";
		m_CONMBSERVERIP="";
		m_PRODMBSERVERIP="";
		m_TOPICB="";
		m_TOPICA="";
		m_TOPICC="";
		m_GROUPA="";
		m_GROUPB="";
		m_GROUPC="";
    }
	
  public:

	std::string  m_REGTAG;
	std::string  m_PUBTAG;
	std::string  m_URCBPUBTAG;
	std::string  m_MBSERVERIP;
	std::string  m_CONMBSERVERIP;
	std::string  m_PRODMBSERVERIP;
	std::string  m_TOPICB;
	std::string  m_TOPICA;
	std::string  m_TOPICC;
	std::string  m_GROUPA;
	std::string  m_GROUPB;
	std::string  m_GROUPC;
	
 
 
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
