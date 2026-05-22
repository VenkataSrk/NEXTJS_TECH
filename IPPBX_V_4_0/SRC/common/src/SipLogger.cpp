/*
********************************************************************************
*
*           Copyright  2020 by VECTON MOBILE.
*           All rights reserved
*
* Description: This contains the class definition for the SipLogger methods used to
*           log messages of different severity levels supported by ACE.
*
* Author:      Narrain Prithi Dharuman
*
* Change History:
*
* Problem#         Fixed by                        Old Ver#
*
*******************************************************************************
*/


#include "SipLogger.h"
#include <time.h>



using namespace std;

SipLog* SipLog :: myLog_ = NULL;
unsigned int SipLog :: lineCount_ = 0;

/*
***************************************************************************
* method Name: logCount
* Arguments: log level, file generating log,line number,log message.
* description: logs message based on severity level passed.
***************************************************************************
*/

void SipLog :: logCount ()
{
    lineCount_ ++;
    if (lineCount_ >=  maxLineCount_)
    {
        lineCount_ = 0;
        char bkpfile[20];
        int i;
        for (i = fileCount_;(sprintf(bkpfile, "%s.%d",logFile_.c_str(),i) &&
            0 == stat(bkpfile,&buf));i++){}
        fileCount_ = i;
        if (0 == link (logFile_.c_str(),bkpfile) )
        {
            unlink(logFile_.c_str());
            output_ =new std::ofstream (logFile_.c_str());
            ACE_LOG_MSG -> msg_ostream (output_);
            ACE_LOG_MSG -> set_flags (ACE_Log_Msg :: OSTREAM);
            if (!console_)
            ACE_LOG_MSG -> clr_flags (ACE_Log_Msg :: STDERR);
        }
        else
        {
            perror("link error =");
        }
    }
}

#if 0
int main()
{
        SipLog *_log = SipLog::getLogInstance("SipLog");
    int i = 100;
        _log -> setLogTargets( 10000, true, false);
        _log -> setLogLevels(1,1,1,1,1);
        //_log -> setWarn();
        _log -> setLogLineCount (5);
//      SIP_LOG ((SIP_DEBUG" debug log = %d\n",i));
        SIP_LOG ((SIP_DEBUG" debug log = %d\n",i));
        //SIP_LOG ((SIP_DEBUG" debug log = %d\n",i));
        //SIP_LOG ((SIP_DEBUG" debug log = %d\n",i));
//      SIP_LOG ((SIP_DEBUG" debug log = %d\n",i));
//      SIP_LOG ((SIP_DEBUG" debug log = %d\n",i));
//      SIP_LOG ((SIP_DEBUG" debug log = %d\n",i));
        return 0;
}
#endif
