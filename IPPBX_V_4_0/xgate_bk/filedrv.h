/* $Header: /xGate/filedrv.h 6     9/06/04 4:41p Bennylp $
 *   
 * XGATE PROJECT
 * (C) 2001 Switchlab, Ltd
 *
 * XGATE FILE SERVICE/API.
 *
 * $Log: /xGate/filedrv.h $
 * 
 * 6     9/06/04 4:41p Bennylp
 * Changed to asynchronous
 * 
 * 5     7/13/04 2:48p Bennylp
 * Added more info when file driver behaving badly
 * 
 * 4     3/07/03 5:41p Bennylp
 * Added FileFind API
 * 
 * 3     4/30/01 8:14a Bennylp
 * File driver integration.
 */

#ifndef __CFILEDRIVER_H__
#define __CFILEDRIVER_H__

#include "api.h"
#include "serviceprovider.h"
#include <set>
#include <ace/Synch.h>
#include <ace/Thread_Mutex.h>
#include <ace/Guard_T.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>   //linux porting

extern "C" {
//# include <jam.h>
//# include "filesys.h"
//# include "pathsys.h"
//# include "strings.h"
//# include "newstr.h"

#define _finddata_t ffblk
}

//ACE_Thnead_Mutex;
//typedef ACE_Token ACE_Thread_Mutex;        //linux porting

//-----------------------------------------------------------------------------
// Maximum opened file handles by a task
// TODO: make it configurable
//
const int MAXOPENFHANDLE=32;


/* Forward decl.
 */
class CTask;
class File_API;
class File_Worker;


///////////////////////////////////////////////////////////////////////////////
/** XGATE service that provides file manipulation API for the application.
 */
class File_Service : public ServiceProvider {
public:
    /** Create the singleton instance of this class.
     */
    static File_Service * Create();

    /** Get the singleton instance of this class.
     */
    static File_Service * Instance() {
	return fileSvcInstance;
    }

    /** Initialize the file service.
     */
    virtual bool Load();

    /** Deinitialize the file service.
     */
    virtual bool Unload();

    /** Called when a new task has just been created.
     */
    virtual bool AttachTask(CTask *);

    /** Called when the task is about to be destroyed.
     */
    virtual void DetachTask(CTask *);

    /** Cancelling function. */
    virtual bool CancelJob(CTask *);

    /** Show status. */
    virtual bool OnShowStatus(String & output);

    /** HandleEvents: Descendent shall implement this abstract method.
	It is called by Service Provider Manager
     */
    virtual void HandleEvents();

    /** Open a text file for reading.
     */
    STATUS  API_FileOpenForRead( CTask *task, int *rethnd, const char *filename);

    /** Open a text file for writing.
     */
    STATUS  API_FileOpenForWrite( CTask *task, int *rethnd, const char *filename);

    /** Write a line to the text file.
     */
    STATUS  API_FileWriteLine( CTask *, int hnd, const char *line);

    /** Read a line from the text file.
     */
    STATUS  API_FileReadLine( CTask *, int hnd, char *line);

    /** Close the file.
     */
    STATUS  API_FileClose( CTask *, int hnd);

    /** Determine whether the EOF condition has been reached for the file.
     */
    STATUS  API_FileIsEOF( CTask *, int *ret, int hnd);

    /** Determine whether a file exist.
     */
    STATUS API_FileExist( CTask *task, int *bret, const char *filename);

    /** Delete a file.
     */
    STATUS API_FileDelete( CTask *task, const char *filename);

    /** Rename a file.
     */
    STATUS API_FileRename( CTask *task, const char *oldname, const char *newname);

    /** Get the size of a file.
     */
    STATUS API_FileGetSize( CTask *task, int *ret, const char *filename);

    /** Copy a file.
     */
    STATUS API_FileCopy( CTask *task, const char *src, const char *dest);

    /** Find first file.
     */
    STATUS API_FileFindFirst( CTask *task, char *filename, char *spec );

    /** Find next file.
     */
    STATUS API_FileFindNext( CTask *task, char *filename );

    void AddPendingTask(unsigned pid);
    void RemovePendingTask(unsigned pid);
    void RemovePendingTask_NoLock(unsigned pid);
    bool HasPendingTask(unsigned pid);
    bool HasPendingTask_NoLock(unsigned pid);
    void AddCompletedTask_NoLock(CTask *task);

    void LockPendingTask();
    void UnlockPendingTask();

private:
    static File_Service *fileSvcInstance;
    int		m_TLS;
    File_API	*m_API;
    File_Worker *m_Worker;

    ACE_Thread_Mutex m_TaskMutex_;
    std::set<unsigned> m_PendingTasks_;
    std::set<CTask*> m_CompletedTasks_;

    File_Service();
    ~File_Service();
    static void    SetTaskLastError(CTask *);


    // Internal representation of task's opened file handles
    class CTaskState {
	CTaskState();

	int     FindSlot();
	void    ReleaseHandle (FILE *fhnd);
    	FILE*	m_Handle[MAXOPENFHANDLE];
	long    m_FindHandle;

	friend class File_Service;
    };


    // get file object/handle based on task's handle
    FILE* GetFileObject( CTask *task, int hnd, const char *operation);

    // internal: get file size
    static unsigned long GetFileSize(FILE *fhnd);

    // display
private:
    friend class CTaskState;
};


/** File API provider class.
 */
class File_API : public CAPIProvider {

public:
    STATUS API_CALL API_FileOpenForRead( CTask *task, int *rethnd, const char *filename);
    STATUS API_CALL API_FileOpenForWrite( CTask *task, int *rethnd, const char *filename);
    STATUS API_CALL API_FileWriteLine( CTask *, int hnd, const char *line);
    STATUS API_CALL API_FileReadLine( CTask *, int hnd, char *line);
    STATUS API_CALL API_FileClose( CTask *, int hnd);
    STATUS API_CALL API_FileIsEOF( CTask *, int *ret, int hnd);

    STATUS API_CALL API_FileExist( CTask *task, int *bret, const char *filename);
    STATUS API_CALL API_FileDelete( CTask *task, const char *filename);
    STATUS API_CALL API_FileRename( CTask *task, const char *oldname, const char *newname);
    STATUS API_CALL API_FileGetSize( CTask *task, int *ret, const char *filename);
    STATUS API_CALL API_FileCopy( CTask *task, const char *src, const char *dest);

    STATUS API_CALL API_FileFindFirst( CTask *task, char *filename, char *spec );
    STATUS API_CALL API_FileFindNext( CTask *task, char *filename );

private:
    File_Service	*m_Driver;

    File_API( int provider_id, File_Service *d);

    STATUS Init();
    STATUS  Shutdown();

    friend class File_Service;
};





#endif
