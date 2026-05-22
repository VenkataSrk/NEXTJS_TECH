#ifndef __CA_FILE_HANDLE__H__
#define __CA_FILE_HANDLE__H__

#include "CAUtilites.h"

class CAFileHandle
{
    private:
    std::string filepath;
	  std::fstream fhndl;
      
    public:
        std::string rdata;
        CAFileHandle();
        CAFileHandle(std::string FileName,std::string path,std::ios::openmode mode);
        virtual ~CAFileHandle();
        bool FileCreate(std::string FileName,std::string path,std::ios::openmode mode);
        bool WriteData(std::string data);
        bool ReadData();
        bool CloseFile();
        int GetFileSize(std::string FileName,std::string path);
	    bool SearchFile(std::string FileName,std::string path);
        std::vector<char> readFile(STR filepath);
};

#endif
