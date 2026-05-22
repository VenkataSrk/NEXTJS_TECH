/*
********************************************************************************
*
*           Copyright  2020 by VECTONE MOBILE.
*           All rights reserved
*
* Description: This contains the class declaration for the SipLogger method used to
*           log messages of different severity levels supported by ACE.
*
* Author:      Narrain Prithvi Dharuman
*
* Change History:
* Problem#         Fixed by                        Old Ver#
*
*******************************************************************************
*/


#ifndef _SIP_ACE_LOGGER_H
#define _SIP_ACE_LOGGER_H
#include "ace/Log_Msg.h"
#include "ace/streams.h"
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include <iostream>

#define DEBUG_PREFIX        ACE_TEXT ("DB%I")
#define INFO_PREFIX         ACE_TEXT ("IN%I")
#define NOTICE_PREFIX       ACE_TEXT ("NO%I")
#define WARNING_PREFIX      ACE_TEXT ("WA%I")
#define ERROR_PREFIX        ACE_TEXT ("ER%I")
#define CRITICAL_PREFIX     ACE_TEXT ("CR%I")
#define ALERT_PREFIX        ACE_TEXT ("AL%I")
#define EMERGENCY_PREFIX    ACE_TEXT ("EM%I")
#define SIP_DEBUG     LM_DEBUG,     ACE_TEXT ("%D | " DEBUG_PREFIX " | %P | " "%N:%l | ")
#define SIP_INFO      LM_INFO,      ACE_TEXT ("%D | " INFO_PREFIX " | %P | " "%N:%l | ")
#define SIP_NOTICE    LM_NOTICE,    ACE_TEXT ("%D | " NOTICE_PREFIX " | %P )| " "%N:%l | ")
#define SIP_WARNING   LM_WARNING,   ACE_TEXT ("%D | " WARNING_PREFIX " | %P | " "%N:%l | ")
#define SIP_ERROR     LM_ERROR,     ACE_TEXT ("%D | " ERROR_PREFIX " | %P | " " %N:%l | ")
#define SIP_CRITICAL  LM_CRITICAL,  ACE_TEXT ("%D | " CRITICAL_PREFIX "| %P | " " %N:%l | ")
#define SIP_ALERT     LM_ALERT,     ACE_TEXT ("%D | " ALERT_PREFIX " | %P | " "%N:%l | ")
#define SIP_EMERGENCY LM_EMERGENCY, ACE_TEXT ("%D | " EMERGENCY_PREFIX " | %P | " "%N:%l | ")

using namespace std;
class SipLog:public ACE_Log_Msg
{
        private:

                static unsigned int  lineCount_;
                static SipLog *myLog_;
                int logLevel_;
                unsigned int maxLineCount_;
                string logFilePath_;
                bool debug_, info_, error_, warn_, fatal_, console_,syslog_ ;
                string logFile_;
                ACE_OSTREAM_TYPE *output_;
                int fileCount_ ;
                struct stat buf;

                /* constructor */
                SipLog ()
                {
                        logFile_ = "diaLog";
                        console_ = false;
                        maxLineCount_ = 10000;
                        lineCount_ = 0;
                        fileCount_ =0;
                        output_ = new std::ofstream (logFile_.c_str());
                        ACE_LOG_MSG->msg_ostream (output_);
                        ACE_LOG_MSG -> set_flags (ACE_Log_Msg :: OSTREAM);
                        //if (console_)
                        //ACE_LOG_MSG -> clr_flags (ACE_Log_Msg :: STDERR);
                }
                /**
                 * Initializes logger object to log to a file.
                 * @@param logFile log file name.
                 */
                SipLog (string logFile)
                {
                        logFile_ = logFile;
                        console_ = false;
                        maxLineCount_ = 10000;
                        lineCount_ = 0;
                        fileCount_ =0;
                        output_ = new std::ofstream (logFile_.c_str());
                        ACE_LOG_MSG->msg_ostream (output_);
                        ACE_LOG_MSG -> set_flags (ACE_Log_Msg :: OSTREAM);
                }

                /**
                 *      Enforces the log levels.
                 */
                void applyLogLevels()
                {
                        u_long mask = this -> priority_mask(ACE_Log_Msg::PROCESS);
                        mask &= ~LM_DEBUG;
                        mask &= ~LM_INFO;
                        mask &= ~LM_ERROR;
                        mask &= ~LM_WARNING;
                        mask &= ~LM_EMERGENCY;

                        mask |= debug_ ? LM_DEBUG : 0;
                        mask |= info_ ? LM_INFO : 0;
                        mask |= error_ ? LM_ERROR : 0;
                        mask |= warn_ ? LM_WARNING : 0;
                        mask |= fatal_ ? LM_EMERGENCY : 0;
                        this -> priority_mask(mask, ACE_Log_Msg::PROCESS);
                }
                /**
                 *      Enforces the log targets.
                 */
                void applyTargets()
                {
                        if (!console_)
                        ACE_LOG_MSG -> clr_flags (ACE_Log_Msg :: STDERR);
                }

        public:
                ~SipLog()
                {
                        delete output_;
                }

                /**
                 *      This functon sets the max number of lines in log file.
                 *      @@param maxLineCount    The maximum lne count of the file.
                 */
                void setLogLineCount( unsigned int maxLineCount)
                {
                        maxLineCount_ = maxLineCount;
                }

                /**
                 * This function returns the max line count of the log file, beyond 
                 *      which the file wraps.
                 *      @@return                unsigned int    The maximum line count.
                 */
                unsigned int getLogLineCount (){return maxLineCount_;}

                /**
                 *      This function sets the log targets.
                 *      @@param maxLineCount            Maximum line count.
                 *      @@param console                         to enable/disable console logs.
                 *      @@param syslog                          to enable/disable syslog.
                 */
                void setLogTargets(unsigned int maxLineCount, bool console, bool syslog)
                {
                        maxLineCount_ = maxLineCount;
                        console_ = console;
                        syslog_ = syslog;
                        applyTargets();
                }

                void setLogLevels (bool debug,bool info,bool warn,bool error,bool fatal)
                {
                        debug_ = debug;
                        info_ = info;
                        warn_ = warn;
                        error_ = error;
                        fatal_ = fatal;
                        applyLogLevels();
                }

                string& getLogFile()
                {
                        return logFile_;
                }

                static void setLogFile(string logFile)
                {
                        delete myLog_;
                        myLog_ = NULL;
                        myLog_ = new SipLog(logFile);
                }

                bool isDebug() {return debug_;}
                bool isInfo() {return info_;}
                bool isWarn() {return warn_;}
                bool isError() {return error_;}
                bool isFatal() {return fatal_;}

                void setDebug () {debug_ = 1;applyLogLevels();}
                void setInfo () {info_ = 1;applyLogLevels();}
                void setWarn () {warn_ = 1;applyLogLevels();}
                void setError () {error_ = 1;applyLogLevels();}
                void setFatal () {fatal_ = 1;applyLogLevels();}


                void clearDebug () {debug_ = 0;applyLogLevels();}
                void clearInfo () {info_ = 0;applyLogLevels();}
                void clearWarn () {warn_ = 0;applyLogLevels();}
                void clearError () {error_ = 0;applyLogLevels();}
                void clearFatal () {fatal_ = 0;applyLogLevels();}

                /* returns log object*/
                static SipLog *getLogInstance(void)
                {
                        if (myLog_ ==NULL)
                        {
                                myLog_ = new SipLog();
                        }
                        return myLog_;
                }

                /**
                 *
                 */
                static SipLog *getLogInstance(string logFile)
                {
                        if (myLog_ == NULL)
                        {
                                myLog_ = new SipLog(logFile);
                        }
                        return myLog_;
                }
                /* Used to log message*/
                void logCount (void);

};


#define SIP_LOG(X) \
do { \
        if  (1 == (ACE_LOG_MSG -> log_priority_enabled X)) {\
                SipLog::getLogInstance() -> logCount(); \
                ACE_DEBUG(X); }\
} while (0)

#endif
