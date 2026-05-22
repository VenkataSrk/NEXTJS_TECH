#include "xGateServerConfig.h"
#include "xglog.h"
#define THISMODULE "LOADCONFPROXY"



GlobalClass *GlobalClass::s_instance = 0;


//Copy Constructor
GlobalClass::GlobalClass(const GlobalClass &glc){
	
  
	        m_PBX_A=glc.m_PBX_A;
                m_PBX_B=glc.m_PBX_B;
                m_DTMF_LOW_DURATION= glc.m_DTMF_LOW_DURATION;
                m_DEC_RESAMPLER=glc.m_DEC_RESAMPLER;
                m_DEC_CONVERTER=glc.m_DEC_CONVERTER;
                m_G729_DEC_AUDIORATE=glc.m_G729_DEC_AUDIORATE;
                m_G729_DEC_AUDIORESAMPLER=glc.m_G729_DEC_AUDIORESAMPLER;
                m_G729_DEC_AUDIOCONVERTER=glc.m_G729_DEC_AUDIOCONVERTER;
                m_AUDIT_START_TIME=glc.m_AUDIT_START_TIME;
                m_AUDIT_MAX_RETRY=glc.m_AUDIT_MAX_RETRY;	
		
}

//Assignment Operator
GlobalClass &GlobalClass::operator=(const GlobalClass &glc){

	        m_PBX_A=glc.m_PBX_A;
                m_PBX_B=glc.m_PBX_B;
                m_DTMF_LOW_DURATION= glc.m_DTMF_LOW_DURATION;
                m_DEC_RESAMPLER=glc.m_DEC_RESAMPLER;
                m_DEC_CONVERTER=glc.m_DEC_CONVERTER;
                m_G729_DEC_AUDIORATE=glc.m_G729_DEC_AUDIORATE;
                m_G729_DEC_AUDIORESAMPLER=glc.m_G729_DEC_AUDIORESAMPLER;
                m_G729_DEC_AUDIOCONVERTER=glc.m_G729_DEC_AUDIOCONVERTER;
                m_AUDIT_START_TIME=glc.m_AUDIT_START_TIME;
                m_AUDIT_MAX_RETRY=glc.m_AUDIT_MAX_RETRY;	
		
		
}


int GlobalClass::loadConfig(){

   std::ifstream cFile ("/etc/xgate/Server.conf");
		if (cFile.is_open())
		{
			XGLOG_INFO("LoadConfigForMG::LoadConfig() Going to Load Data From Server.conf file");
				std::string line;
			while(getline(cFile, line))
			{	
				line.erase(std::remove_if(line.begin(), line.end(), isspace),line.end());
				
				if(line[0] == '#' || line.empty())
					continue;
				
				   auto delimiterPos = line.find(":");
				   auto name = line.substr(0, delimiterPos);
			   
				if(name.compare("PBX")==0){
						std::string pbx_ip  = line.substr(delimiterPos + 1);
                                                m_PBXIpAddressList.push_back(pbx_ip);    
				}

					

					
				else if(name.compare("dtmf_low_duration")==0){
						std::string m_dtmf_low_duration = line.substr(delimiterPos + 1);
                                                m_DTMF_LOW_DURATION = atoi(m_dtmf_low_duration.c_str());
						
						
				} 


				else if(name.compare("enable_dec_resampler")==0){
                                                std::string enable_dec_resampler = line.substr(delimiterPos + 1);
						m_DEC_RESAMPLER = atoi(enable_dec_resampler.c_str());
						
						
				}
					
				else if(name.compare("enable_dec_converter")==0){
                                                std::string enable_dec_converter = line.substr(delimiterPos + 1);
                                                m_DEC_CONVERTER = atoi(enable_dec_converter.c_str());
						
						
				}
				
				else if(name.compare("enable_g729dec_audiorate")==0){
                                                std::string enable_g729dec_audiorate = line.substr(delimiterPos + 1);
                                                m_G729_DEC_AUDIORATE = atoi(enable_g729dec_audiorate.c_str());
						
						
				}

					
				else if(name.compare("enable_g729dec_audioresampler")==0){
                                                std::string enable_g729dec_audioresampler = line.substr(delimiterPos + 1);
						m_G729_DEC_AUDIORESAMPLER = atoi(enable_g729dec_audioresampler.c_str());
						
						
				 }

					
				else if(name.compare("enable_g729dec_audioconverter")==0){
                                                std::string enable_g729dec_audioconverter = line.substr(delimiterPos + 1);
						m_G729_DEC_AUDIOCONVERTER = atoi(enable_g729dec_audioconverter.c_str());
						
						
				 }
				
				else if(name.compare("audit_start_time")==0){
                                                std::string audit_start_time = line.substr(delimiterPos + 1);
						m_AUDIT_START_TIME = atoi(audit_start_time.c_str());


				}
                                else if(name.compare("audit_max_retry")==0){
                                                 std::string audit_max_retry = line.substr(delimiterPos + 1);
                                                m_AUDIT_MAX_RETRY = atoi(audit_max_retry.c_str());


                                }

		
			}
								   
		}
		
		else {
			std::cerr << "Couldn't open config file for reading.\n";
			XGLOG_ERROR("Server.conf File Not Loaded Successfully exiting !");
			exit (1);
		}
		
	
   return 0;
}

 
   
   
   
   

