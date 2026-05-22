#include "CAFile.h"
#include "LogHandler.h"
using namespace std;
CAFileHandle::CAFileHandle()
{    
}

CAFileHandle::~CAFileHandle()
{
}

CAFileHandle::CAFileHandle(std::string FileName,std::string path,std::ios::openmode mode)
{
}
/* Method Name: FileCreate
Description: This function will create the file on given path
Parameters:  Name of the file, Path of the file , Mode of the file
return: Returns as boolean value as per the file craetion
*/

bool CAFileHandle::FileCreate(std::string FileName,std::string path,std::ios::openmode mode)
{
   filepath = path +"/"+FileName;
   fhndl.open(filepath,mode);
   if(!fhndl)
   {
      loginfo("","\n File creation was failed");
      return false;
   }else{
	   loginfo("","\n File creation was successfull");
   }
   return true;
}

/* Method Name: WriteData
Description: This function will write the data on the given opened file
Parameters:  string:: data
return: Returns as boolean value as per the file writing
*/

bool CAFileHandle::WriteData(std::string data)
{

   if(!fhndl.is_open())
   {
	loginfo("","File data written  was failed");
      return false;
   }else{
      fhndl<<data;
      loginfo("","File data written  was success");
      fhndl.close();
   }
   return true;
}
/* Method Name: ReadData
Description: This function will read the data on the given  file
Parameters:  NA
return: Returns as boolean value as per the file Reading
*/
bool CAFileHandle::ReadData()
{
   std::ifstream myfile;
   myfile.open(filepath);
   if(!myfile)
   {
      return false;
   }
   std::string mystring;
   while (getline (myfile,rdata))
   {

      cout<<rdata<<endl;
   }
   return true;
}
/* Method Name: ReadData
Description: This function will close the data on the given  file
Parameters:  NA
return: Returns as boolean value as per the file close
*/
bool CAFileHandle::CloseFile()
{
   if(fhndl.is_open())
   {
      fhndl.close();

   }else
   {
      return false;
   }
   return true;
}
/* Method Name: SearchFile
Description: This function will search the file is exist or not given file path
Parameters:  filename and path of the file as string datatype
return: Returns as boolean value as per the file exist or not
*/

bool CAFileHandle::SearchFile(std::string FileName,std::string path)
{
  std::ifstream myfile;
  std::string mpath = path +"/"+FileName;
  myfile.open(mpath);
  if(!myfile)
  {
      cout<<"file was not exist on given path"<<endl;
      loginfo("","file %s was not exist on given path %s",FileName.c_str(),path.c_str());
      return false;
  }else{
      cout<<"file was exist on given path"<<endl;
      loginfo("","file %s was exist on given path %s",FileName.c_str(),path.c_str());
  }
  return true;
}

int CAFileHandle::GetFileSize(std::string FileName,std::string path)
{
   std::string mpath = path +"/"+FileName;
   ifstream in_file(mpath, ios::binary);
   in_file.seekg(0, ios::end);
   int file_size = in_file.tellg();
   cout<<"Size of the file is"<<" "<< file_size<<" "<<"bytes";
   loginfo("","Size of the file is %s and size is %d bytes",FileName.c_str(),file_size);
   return 0;
}

/* Method Name: readFile
Description: To open given file and store the data in buffer
Parameters:  filepath
return: Returns as boolean value as per the file close
*/
std::vector<char> CAFileHandle::readFile(STR filepath)
{
   std::ifstream theFile;
   theFile.open(filepath, ios::in);
   if(!theFile)
	{
      cout<<"404 - config file (" << filepath.c_str() << ") not found"<<endl;
      loginfo("","404 - config file (%s)not found",filepath.c_str());
      return {};
	}
   std::vector<char>buffer((std::istreambuf_iterator<char>(theFile)), std::istreambuf_iterator<char>());
   buffer.push_back ('\0');
   if(theFile.is_open())
   {
      theFile.close();
   }
   return buffer;
}
