	/***********************************************************************************************
	  ******ReadConfigFromProxyConfFile.cpp Version 1.0*********************************************
	  *******Author @Abdinav Kumar Singh this File Basical Read Data from Proxy.conf File and load***
	  *******into Global Variable. so that any .cpp file having scop to read Variable=value******
	  ************************************************************************************************/


#include "ReadConfigFromProxyConfFile.h"
#include "xglog.h"

#define THISMODULE "LOADCONFPROXY"



GlobalClass *GlobalClass::s_instance = 0;



int GlobalClass::loadConfig(){

   std::ifstream cFile ("/etc/xgate/Proxy.conf");
		if (cFile.is_open())
		{
			XGLOG_INFO( THISMODULE,"LoadConfigForPBXMBC::LoadConfig() Going to Load Data From Proxy.conf file");
				std::string line;
			while(getline(cFile, line))
			{	
				line.erase(std::remove_if(line.begin(), line.end(), isspace),line.end());
				
				if(line[0] == '#' || line.empty())
					continue;
				
				   auto delimiterPos = line.find(":");
				   auto name = line.substr(0, delimiterPos);
			   
				if(name.compare("REGTAG")==0){
						m_REGTAG = line.substr(delimiterPos + 1);
						
						
				}

					
				else if(name.compare("PUBTAG")==0){
						m_PUBTAG = line.substr(delimiterPos + 1);
						
						
				}

                                else if(name.compare("URCBPUBTAG")==0){
                                                m_URCBPUBTAG = line.substr(delimiterPos + 1);


                                }
					
				else if(name.compare("MBSERVERIP")==0){
						m_MBSERVERIP = line.substr(delimiterPos + 1);
						
						
				} 

                                else if(name.compare("CONMBSERVERIP")==0){
                                                m_CONMBSERVERIP = line.substr(delimiterPos + 1);


                                }

                                else if(name.compare("PRODMBSERVERIP")==0){
                                                m_PRODMBSERVERIP = line.substr(delimiterPos + 1);


                                }
					
				else if(name.compare("TOPICB")==0){
						m_TOPICB = line.substr(delimiterPos + 1);
						
						
				}
				
				else if(name.compare("TOPICA")==0){
						m_TOPICA = line.substr(delimiterPos + 1);
						
						
				}

				else if(name.compare("TOPICC")==0){
                                                m_TOPICC = line.substr(delimiterPos + 1);


                                }
					
				else if(name.compare("GROUPA")==0){
						m_GROUPA = line.substr(delimiterPos + 1);
						
						
				}

					
				else if(name.compare("GROUPB")==0){
						m_GROUPB = line.substr(delimiterPos + 1);
						
						
				}
		
				else if(name.compare("GROUPC")==0){
                                                m_GROUPC = line.substr(delimiterPos + 1);


                                }
			
			}
			XGLOG_INFO( THISMODULE,"LoadConfigForPBXMBC::LoadConfig() Successfully Done!");
			XGLOG_INFO( THISMODULE,"REGTAG(%s),PUBTAG(%s),\
								   MBSERVERIP(%s),TOPICA(%s),TOPICB(%s),GROUPA(%s),\
								   GROUPB(%s), GROUPC(%s)",m_REGTAG.c_str(),m_PUBTAG.c_str(),\
								   m_MBSERVERIP.c_str(),m_TOPICA.c_str(),m_TOPICB.c_str(),m_GROUPA.c_str(), m_GROUPC.c_str());
		}
		
		else {
			std::cerr << "Couldn't open config file for reading.\n";
			XGLOG_ERROR( THISMODULE,"Proxy.conf File Not Loaded Successfully exiting ");
			exit (1);
		}
		
	
   return 0;
}

 
   
   
   
   
