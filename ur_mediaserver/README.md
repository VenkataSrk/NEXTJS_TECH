Authors: Jais, Yoga, Pradeep, Gaurav, Sangamesh.h
Repo created date: 22-01-2021
Description: this build system base taken from VMS project build system. The project MBC (Media Broadcast Controller) to support SFU as be business and high level docuements
git repo - git clone https://YoganathanV@bitbucket.org/vectonerepo/umn_audiovideo_engine.git
git branch - feature/new_mbc_build_system
  1. the above said "new_mbc_build_system" branch has been created to enhance the build system for mbc project.
  2. this build system sample brought from VMS project from repo "UR_VoiceMailServer"
  3. main tasks planned in this branch are as follows
    3.1 remove unwanted below said components from existing VMS build system
      3.1.1 sofia-sip-1.12.11
      3.1.2 xg_sipservice
      3.1.3 xg_ivrservice
    3.2 keep and utilize exisiting below said components
      3.2.1 xg_reactor - contain the logics to utilize the ace framework and supply required base class
      3.2.2 xg_mediaservice - contain send and read json messages to and fro between other entities like (pbx, mgc)
      3.2.3 xg_iurmodule - contain the logics to interact between difference components in this mbc build system 
      3.2.4 xg_logger - provides log feature using ACE_Logger
      3.2.5 xg_parser - provides xml parser
      3.4.6 xg_httpservice - provides http service using curl library
      3.4.7 xg_hmp - host media processor contains the logics to interact with gstreamer framework
      3.4.8 src - which contains the source files of mbc to support sfu and mcu. main thread from this src folder will initiate above said components
===============================================================================================

Date: 11-08-2021
	git repo - git clone http://mpradeepan@bitbucket.org/vectonerepo/umn_audiovideo_engine.git
	git branch - feature/mbc_conference
		1. Added the below configuration to the mbc.conf, contains the stun details
		    "stun_config":{
		      "username": "admin",
    		  "password": "system123",
   	 	    "stun_domain": "stun02.unifiedring.co.uk",
    		  "stun_port": 3478
  		  },
		2. Added the mbc.conf file to this commit refer that.
		3. The below values are hardcoded as default vales in inc/xGateUtil.h
		      "username": "admin",
    		  "password": "system123",
   	 	    "stun_domain": "vturn01.unifiedring.co.uk",
    		  "stun_port": 3478

===============================================================================================
