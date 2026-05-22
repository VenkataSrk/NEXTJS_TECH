#include "BSBaseReport.h"
#define max_CDR_file_size 10000

STR BSBaseReport::TmStrFormat(const char *format)
{
  tm * timeptr;
  time_t timeval = time(NULL);
  char time_string[50];
  timeptr = localtime(&timeval);
  memset(time_string, 0, sizeof(time_string));
  strftime(time_string, sizeof(time_string), format, timeptr);
  return time_string;
}

bool BSBaseReport::WriteToFile(STR data){

        if(data.empty()){
        return false;
        }
        m_strFileName = createFilename();
        m_fileHandler = new CAFileHandle;
	      m_fileHandler->FileCreate(m_strFileName, m_strReportFilePath, std::ios_base::app);
        if(!m_fileHandler->WriteData(data)){
          return false;
        }
        m_fileHandler->CloseFile();
        delete m_fileHandler;
        return true;
}



STR BSBaseReport::createFilename()
{
 STR CDRFileName("");
 STR time = TmStrFormat("%m%d%Y");
 CDRFileName.assign(m_strSwitchCode);
 CDRFileName.append(time);
 CDRFileName.append(m_strFileFormat);
 return CDRFileName;
}






