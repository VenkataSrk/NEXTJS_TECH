/* $Header: /xGate/filedrv.cpp 10    5/03/05 3:45p Bennylp $
 *   
 * XGATE PROJECT
 * (C) 2001 Switchlab, Ltd
 *
 * XGATE FILE SERVICE/API.
 *
 * $Log: /xGate/filedrv.cpp $
 * 
 * 10    5/03/05 3:45p Bennylp
 * Changed CreateThread to CREATE_THREAD
 * 
 * 9     10/18/04 4:26p Bennylp
 * Fixed crash bug if invalid handle is given by app.
 * 
 * 8     9/06/04 4:41p Bennylp
 * Changed to asynchronous
 * 
 * 7     7/13/04 2:48p Bennylp
 * Added more info when file driver behaving badly
 * 
 * 6     2/20/04 10:26a Bennylp
 * Added project configuration Hoard and Hoard Debug, to use STLPORT and
 * Hoard memory allocator.
 * 
 * 5     4/23/03 5:34p Bennylp
 * Administrivia: tidy-up the usage of API provider ID. Now the
 * enumeration in api.h should be used instead of bare constants.
 * 
 * 4     3/07/03 5:41p Bennylp
 * Added FileFind API
 * 
 * 3     4/30/01 8:14a Bennylp
 * File driver integration.
 */

//#include "stdafx.h"
//#include "filedrv.h"
#include <poll.h>
#include "kernel.h"
#include "task.h"
#include "log.h"
#include "apperror.h"
#include "sl.h"
#include <errno.h>
//#include <ace/Synch.h>
#include "thread_process.h"
#include <sys/types.h>
#include <sys/stat.h>	//linux porting
//#include "Synch.h"
#include "filedrv.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <libgen.h>
#include <limits.h>
#include <dirent.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include "findfirst.h"
#include "spec.h"


#define FILEDRV	"filedrv"


File_Service *File_Service::fileSvcInstance;


void Sleep( unsigned long ms )
{
#ifndef __linux__
    ::Sleep(ms);
#else
    poll(0,0,ms); // __linux__
#endif
};


//-----------------------------------------------------------------------------
// This code for linux porting
#define DOTDOT_HANDLE    0L
#define INVALID_HANDLE  -1L

typedef struct fhandle_t {
    DIR* dstream;
    short dironly;
    char* spec;
} fhandle_t;

static void fill_finddata(struct stat* st, const char* name,
        struct _finddata_t* fileinfo);

static intptr_t findfirst_dotdot(const char* filespec,
        struct _finddata_t* fileinfo);

static intptr_t findfirst_in_directory(const char* dirpath,
        const char* spec, struct _finddata_t* fileinfo);

static void findfirst_set_errno();

intptr_t _findfirst(const char* filespec, struct _finddata_t* fileinfo) {

    char* rmslash;      /* Rightmost forward slash in filespec. */
    const char* spec;   /* Specification string. */

    if (!fileinfo || !filespec) {
        errno = EINVAL;
        return INVALID_HANDLE;
    }

    if (filespec[0] == '\0') {
        errno = ENOENT;
        return INVALID_HANDLE;
    }

    rmslash = strrchr(filespec, '/');

    if (rmslash != NULL) {
        /*
         * At least one forward slash was found in the filespec
         * string, and rmslash points to the rightmost one. The
         * specification part, if any, begins right after it.
         */
        spec = rmslash + 1;
    } else {
        /*
         * Since no slash were found in the filespec string, its
         * entire content can be used as our spec string.
         */
        spec = filespec;
    }

    if (strcmp(spec, ".") == 0 || strcmp(spec, "..") == 0) {
        /* On Windows, . and .. must return canonicalized names. */
        return findfirst_dotdot(filespec, fileinfo);
    } else if (rmslash == filespec) {
        /*
         * Since the rightmost slash is the first character, we're
         * looking for something located at the file system's root.
         */
        return findfirst_in_directory("/", spec, fileinfo);
    } else if (rmslash != NULL) {
        /*
         * Since the rightmost slash isn't the first one, we're
         * looking for something located in a specific folder. In
         * order to open this folder, we split the folder path from
         * the specification part by overwriting the rightmost
         * forward slash.
         */
        size_t pathlen = strlen(filespec) +1;
        char* dirpath = alloca(pathlen);
        memcpy(dirpath, filespec, pathlen);
        dirpath[rmslash - filespec] = '\0';
        return findfirst_in_directory(dirpath, spec, fileinfo);
    } else {
        /*
         * Since the filespec doesn't contain any forward slash,
         * we're looking for something located in the current
         * directory.
         */
        return findfirst_in_directory(".", spec, fileinfo);
    }
}

/* Perfom a scan in the directory identified by dirpath. */
static intptr_t findfirst_in_directory(const char* dirpath,
        const char* spec, struct _finddata_t* fileinfo) {

    DIR* dstream;
    fhandle_t* ffhandle;

    if (spec[0] == '\0') {
        errno = ENOENT;
        return INVALID_HANDLE;
    }

    if ((dstream = opendir(dirpath)) == NULL) {
        findfirst_set_errno();
        return INVALID_HANDLE;
    }

    if ((ffhandle = malloc(sizeof(fhandle_t))) == NULL) {
        closedir(dstream);
        errno = ENOMEM;
        return INVALID_HANDLE;
    }

    /* On Windows, *. returns only directories. */
    ffhandle->dironly = strcmp(spec, "*.") == 0 ? 1 : 0;
    ffhandle->dstream = dstream;
    ffhandle->spec = strdup(spec);

    if (_findnext((intptr_t) ffhandle, fileinfo) != 0) {
        _findclose((intptr_t) ffhandle);
        errno = ENOENT;
        return INVALID_HANDLE;
    }

    return (intptr_t) ffhandle;
}

/* On Windows, . and .. return canonicalized directory names. */
static intptr_t findfirst_dotdot(const char* filespec,
        struct _finddata_t* fileinfo) {

    char* dirname;
    char* canonicalized;
    struct stat st;

    if (stat(filespec, &st) != 0) {
        findfirst_set_errno();
        return INVALID_HANDLE;
    }

    /* Resolve filespec to an absolute path. */
    if ((canonicalized = realpath(filespec, NULL)) == NULL) {
        findfirst_set_errno();
        return INVALID_HANDLE;
    }

    /* Retrieve the basename from it. */
    dirname = basename(canonicalized);

    /* Make sure that we actually have a basename. */
    if (dirname[0] == '\0') {
        free(canonicalized);
        errno = ENOENT;
        return INVALID_HANDLE;
    }

    /* Make sure that we won't overflow finddata_t::name. */
    if (strlen(dirname) > 259) {
        free(canonicalized);
        errno = ENOMEM;
        return INVALID_HANDLE;
    }

    fill_finddata(&st, dirname, fileinfo);

    free(canonicalized);

    /*
     * Return a special handle since we can't return
     * NULL. The findnext and findclose functions know
     * about this custom handle.
     */
    return DOTDOT_HANDLE;
}

/*
 * Windows implementation of _findfirst either returns EINVAL,
 * ENOENT or ENOMEM. This function makes sure that the above
 * implementation doesn't return anything else when an error
 * condition is encountered.
 */
static void findfirst_set_errno() {
    if (errno != ENOENT &&
        errno != ENOMEM &&
        errno != EINVAL) {
        errno = EINVAL;
    }
}

static void fill_finddata(struct stat* st, const char* name,
        struct _finddata_t* fileinfo) {

    fileinfo->attrib = S_ISDIR(st->st_mode) ? _A_SUBDIR : _A_NORMAL;
    fileinfo->size = st->st_size;
    fileinfo->time_create = st->st_ctime;
    fileinfo->time_access = st->st_atime;
    fileinfo->time_write = st->st_mtime;
    strcpy(fileinfo->name, name);
}

/* int _findnext(intptr_t fhandle, struct _finddata_t* fileinfo) {

    struct dirent entry, *result;
    struct fhandle_t* handle;
    struct stat st;

    if (fhandle == DOTDOT_HANDLE) {
        errno = ENOENT;
        return -1;
    }

    if (fhandle == INVALID_HANDLE || !fileinfo) {
        errno = EINVAL;
        return -1;
    }

    handle = (struct fhandle_t*) fhandle;

    while (readdir_r(handle->dstream, &entry, &result) == 0 && result != NULL) {

        if (!handle->dironly && !match_spec(handle->spec, entry.d_name))
            continue;

        if (fstatat(dirfd(handle->dstream), entry.d_name, &st, 0) == -1)
            return -1;

        if (handle->dironly && !S_ISDIR(st.st_mode))
            continue;

        fill_finddata(&st, entry.d_name, fileinfo);

        return 0;
    }

    errno = ENOENT;
    return -1;
} */

int _findclose(intptr_t fhandle) {

    struct fhandle_t* handle;

    if (fhandle == DOTDOT_HANDLE) {
        return 0;
    }

    if (fhandle == INVALID_HANDLE) {
        errno = ENOENT;
        return -1;
    }

    handle = (struct fhandle_t*) fhandle;

    closedir(handle->dstream);
    free(handle->spec);
    free(handle);

    return 0;
}

//-----------------------------------------------------------------------------
// File_Job
//
class File_Job
{
public:
    const char *name_;
    CTask *task_;
    int task_pid_;

    // HANDLE hCompletionEvent_; still buggy

    File_Job( const char *name, CTask *task ) : name_(name), task_(task) 
    {
	task_pid_ = task_->GetPID();
    }
    virtual ~File_Job() {}

    int  execute();
    void wait_job();
    bool is_job_valid() const;

private:
    virtual int do_job()  = 0;
};

bool File_Job::is_job_valid() const
{
    return File_Service::Instance()->HasPendingTask(task_pid_);
}

int File_Job::execute()
{
    return do_job();
}

class File_OpenForRead_Job : public File_Job
{
public:
    String filename_;
    int *ret_hnd_;
    int slot_idx_;
    FILE **pfile_hnd_;

    File_OpenForRead_Job (CTask *task, const char *filename, 
			  int *ret_hnd, int slot_idx, FILE **pfile_hnd)
	: File_Job("FileOpenForRead", task), filename_(filename), ret_hnd_(ret_hnd), slot_idx_(slot_idx), pfile_hnd_(pfile_hnd)
    {
	*ret_hnd_ = -1;
	*pfile_hnd_ = NULL;
    }

    virtual int do_job() {
	if (!is_job_valid())
	    return 0;

	FILE *fhnd = fopen(filename_.c_str(), "rt+");
	if (!fhnd) {
	    CLog::Warning(FILEDRV, task_, "FileOpenForRead: unable to open file '%s'", filename_.c_str());
	    return -1;
	}

	File_Service::Instance()->LockPendingTask();
	if (File_Service::Instance()->HasPendingTask_NoLock(task_pid_)) {
	    *pfile_hnd_ = fhnd;
	    *ret_hnd_ = slot_idx_;
	} else {
	    fclose(fhnd);
	}
	File_Service::Instance()->UnlockPendingTask();
	return 0;
    }
};

class File_OpenForWrite_Job : public File_Job
{
public:
    String filename_;
    int *ret_hnd_;
    int  slot_idx_;
    FILE **pfile_hnd_;

    File_OpenForWrite_Job (CTask *task, const char *filename, 
			   int *ret_hnd, int slot_idx, FILE **pfile_hnd)
	: File_Job("FileOpenForWrite", task), filename_(filename), ret_hnd_(ret_hnd), slot_idx_(slot_idx), pfile_hnd_(pfile_hnd)
    {
	*ret_hnd_ = -1;
	*pfile_hnd_ = NULL;
    }

    virtual int do_job() {
	if (!is_job_valid())
	    return 0;

	FILE *fhnd = fopen(filename_.c_str(), "at+");
	if (!fhnd) {
	    CLog::Warning(FILEDRV, task_, "FileOpenForWrite: unable to open file '%s'", filename_.c_str());
	    return -1;
	}

	File_Service::Instance()->LockPendingTask();
	if (File_Service::Instance()->HasPendingTask_NoLock(task_pid_)) {
	    *pfile_hnd_ = fhnd;
	    *ret_hnd_ = slot_idx_;
	} else {
	    fclose(fhnd);
	}
	File_Service::Instance()->UnlockPendingTask();

	return 0;
    }
};

class File_WriteLine_Job : public File_Job
{
public:
    FILE *fhnd_;
    String line_;

    File_WriteLine_Job (CTask *task, FILE *fhnd, const char *line)
	: File_Job("FileWriteLine", task), fhnd_(fhnd), line_(line)
    {}

    virtual int do_job() {

	File_Service::Instance()->LockPendingTask();
	if (File_Service::Instance()->HasPendingTask_NoLock(task_pid_)) {
	    fprintf(fhnd_, "%s\n", line_.c_str());
	    fflush(fhnd_);
	}

	File_Service::Instance()->UnlockPendingTask();
	return 0;
    }
};

class File_ReadLine_Job : public File_Job
{
public:
    FILE *fhnd_;
    char *line_;

    File_ReadLine_Job (CTask *task, FILE *fhnd, char *line)
	: File_Job("FileReadLine", task), fhnd_(fhnd), line_(line)
    {
	*line_ = '\0';
    }

    virtual int do_job() {

	File_Service::Instance()->LockPendingTask();
	if (File_Service::Instance()->HasPendingTask_NoLock(task_pid_)) {
	    if (fgets(line_, MAXAPPSTRLEN, fhnd_) != NULL) {

		char *p = line_ + strlen(line_) -1;
		while (p >= line_ && (*p == '\r' || *p == '\n')) {
		    *p = '\0';
		    --p;
		}
  	    }
	}
	File_Service::Instance()->UnlockPendingTask();

	return 0;
    }
};

class File_Close_Job : public File_Job
{
public:
    FILE *fhnd_;

    File_Close_Job (CTask *task, FILE *fhnd)
	: File_Job("FileClose", task), fhnd_(fhnd)
    {}

    virtual int do_job() {
	int ret;

	File_Service::Instance()->LockPendingTask();
	if (File_Service::Instance()->HasPendingTask_NoLock(task_pid_)) {
	    ret = fclose(fhnd_);
	} else {
	    ret = 0;
	}
	File_Service::Instance()->UnlockPendingTask();
	return 0;
    }
};

class File_IsEOF_Job : public File_Job
{
public:
    FILE *fhnd_;
    int *ret_;

    File_IsEOF_Job (CTask *task, FILE *fhnd, int *ret)
	: File_Job("FileIsEOF", task), fhnd_(fhnd), ret_(ret)
    {}

    virtual int do_job() {
	File_Service::Instance()->LockPendingTask();
	if (File_Service::Instance()->HasPendingTask_NoLock(task_pid_)) {
	    *ret_ = feof(fhnd_) ? APPTRUE : APPFALSE;
	}
	File_Service::Instance()->UnlockPendingTask();
	return 0;
    }
};

class File_Exist_Job : public File_Job
{
public:
    String filename_;
    int *ret_;

    File_Exist_Job (CTask *task, const char *filename, int *ret)
	: File_Job("FileExist", task), filename_(filename), ret_(ret)
    {}

    virtual int do_job() {
	File_Service::Instance()->LockPendingTask();
	if (File_Service::Instance()->HasPendingTask_NoLock(task_pid_)) {
	    *ret_ = (access( filename_.c_str(), 0)==-1) ? APPFALSE : APPTRUE;
	}
	File_Service::Instance()->UnlockPendingTask();
	return 0;
    }
};

class File_Delete_Job : public File_Job
{
public:
    String filename_;

    File_Delete_Job (CTask *task, const char *filename)
	: File_Job("FileDelete", task), filename_(filename)
    {}

    virtual int do_job() {
	if (!is_job_valid())
	    return 0;

	if (remove(filename_.c_str()) != 0) {
	    CLog::Warning( FILEDRV, task_, "FileDelete: unable to delete file %s", filename_.c_str());
	    return -1;
	}
	
	return 0;
    }
};

class File_Rename_Job : public File_Job
{
public:
    String src_;
    String dst_;

    File_Rename_Job (CTask *task, const char *src, const char *dst)
	: File_Job("FileRename", task), src_(src), dst_(dst)
    {}

    virtual int do_job() {
	if (!is_job_valid())
	    return 0;

	if (rename(src_.c_str(), dst_.c_str()) != 0) {
	    CLog::Warning( FILEDRV, task_, "Unable to rename file '%s' to '%s'", 
			   src_.c_str(), dst_.c_str());
	}
	return 0;
    }
};

class File_GetSize_Job : public File_Job
{
public:
    String filename_;
    int *ret_;

    File_GetSize_Job (CTask *task, const char *filename, int *ret)
	: File_Job("FileGetSize", task), filename_(filename), ret_(ret)
    {
	*ret_ = 0;
    }

    virtual int do_job() {
	if (!is_job_valid())
	    return 0;

	// get the filesize
	struct stat stat_buf;
	
	if (stat( filename_.c_str(), &stat_buf) != 0) {
	    CLog::Warning( FILEDRV, task_, "FileGetSize: stat error for '%s'", filename_.c_str());
	    return -1;
	}
	
	File_Service::Instance()->LockPendingTask();
	if (File_Service::Instance()->HasPendingTask_NoLock(task_pid_)) {
	    *ret_ = stat_buf.st_size;
	}
	File_Service::Instance()->UnlockPendingTask();
	return 0;
    }
};


class File_Copy_Job : public File_Job
{
public:
    String src_;
    String dst_;

    File_Copy_Job (CTask *task, const char *src, const char *dst)
	: File_Job("FileCopy", task), src_(src), dst_(dst)
    {}

    virtual int do_job() {
	if (!is_job_valid())
	    return 0;

	if (sl::FileCopy( src_.c_str(), dst_.c_str(), true) == NOT_OK) {
	    CLog::Warning( FILEDRV, task_, "FileCopy: unable to copy '%s' to '%s'", src_.c_str(), dst_.c_str());
	    return -1;
	} 
	return 0;
    }
};


//-----------------------------------------------------------------------------
// File_Worker
//
class File_Worker
{
public:
    enum { MAX_WORKER_THREADS = 4 };

    File_Worker();
    ~File_Worker();

    bool initialize();
    void shutdown();

    void submit_job( File_Job *job );

private:
    // Set worker count to 1 to avoid overlapping lines when writing file (e.g. CDR file).

#ifndef __linux__
    HANDLE worker_threads_[MAX_WORKER_THREADS];
#else
    pthread_t worker_threads_[MAX_WORKER_THREADS];
#endif

    bool quit_;
    ACE_Thread_Mutex job_mutex_;
    //ACE_Event job_event_;
    List <File_Job*> jobs_;
    //linux porting
    pthread_cond_t   filedrv_cond;
    pthread_mutex_t  filedrv_mutex;


    static DWORD __stdcall thread_starter(void *arg);
    void thread_main();
};


File_Worker::File_Worker()
: quit_(false)
{
    for (int i=0; i<MAX_WORKER_THREADS; ++i)
	worker_threads_[i] = NULL;

    filedrv_cond  = PTHREAD_COND_INITIALIZER;
    filedrv_mutex = PTHREAD_MUTEX_INITIALIZER;
}

File_Worker::~File_Worker()
{
    shutdown();
}


void* dummyThread(void *){

	while(1){

		Sleep(1);
	}
}

bool File_Worker::initialize()
{
    //pthread_t my_thread; //linux porting
        pthread_t             threadId;
        pthread_attr_t        pthread_attr;
    pthread_t my_thread;


    // Spawn worker threads
    /* for (int i=0; i<MAX_WORKER_THREADS; ++i) {
	//DWORD id;
	//worker_threads_[i] = CREATE_THREAD(NULL, 0, &thread_starter, this, 0, &id);
	int rc = pthread_create(&worker_threads_[i], NULL, &thread_starter, (void *) this);	
	if (rc != 0){
	    CLog::Fatal(FILEDRV, "Fatal: unable to create file worker thread \n");
	    return false;
	}
	Sleep(1);
    } */

	int rc = pthread_create(&my_thread, NULL, &thread_starter, (void *) this);	
	if (rc != 0){
	    printf("Fatal: unable to create file worker thread\n"); //rkv testing
	    CLog::Fatal(FILEDRV, "Fatal: unable to create file worker thread \n");
	    return false;
	}

    return true;
}

void File_Worker::shutdown()
{
    // Clear all jobs
    job_mutex_.acquire();
    jobs_.clear();
    job_mutex_.release();

    // Signal individual threads to quit.
    int i;
    quit_ = true;

    /* for (i=0; i<MAX_WORKER_THREADS; ++i) {
	if (worker_threads_[i] != 0) {
	    //job_event_.signal();
            //pthread_mutex_lock(&filedrv_mutex);

	    pthread_cond_signal( &filedrv_cond );	//linux porting

            //pthread_mutex_unlock(&filedrv_mutex);
	    pthread_mutex_destroy(worker_threads_[i]); //linux porting
	    worker_threads_[i] = NULL;
	    CLog::Detail(FILEDRV, "Detail: File worker thread is shutdown =%d\n",i);
	} 
    }*/
  //pthread_mutex_destroy(&filedrv_mutex);
  //pthread_cond_destroy(&filedrv_cond);
}

void File_Worker::submit_job( File_Job *job )
{
    job_mutex_.acquire();
    jobs_.push_back(job);
    
    job->task_->Sleep( File_Service::Instance() );
    File_Service::Instance()->AddPendingTask( job->task_pid_ );

    job_mutex_.release();
    //job_event_.signal();
    //pthread_mutex_lock(&filedrv_mutex);

    //pthread_cond_broadcast(&filedrv_cond ); //linux porting rkv
    pthread_cond_signal(&filedrv_cond ); //linux porting

    //pthread_mutex_unlock(&filedrv_mutex); //rkv
}

//DWORD __stdcall File_Worker::thread_starter(void *arg)
DWORD File_Worker::thread_starter(void *arg)
{
    //printf("calling File_Worker::thread_starter function\n");
    File_Worker *worker = (File_Worker *)arg;
    worker->thread_main();
    return 0;
}


void File_Worker::thread_main()
{
    //printf("calling File_Worker::thread_main function\n");
    File_Job *current_job;
    //HANDLE hCompletionEvent;

    /*
    hCompletionEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!hCompletionEvent) {
	CLog::Fatal(FILEDRV, "Fatal: unable to create event!");
    }
    */

    while (!quit_) {

	// Get one job.
	// Just let threads compete to get jobs...
	job_mutex_.acquire();
	while (jobs_.empty() && !quit_) {
	    job_mutex_.release();
	    //job_event_.wait();
	    /* submit_job signals to this below */
	    pthread_cond_wait(&filedrv_cond,&filedrv_mutex);
	    //job_mutex_.release();
	    if (quit_) {
                CLog::Warning(FILEDRV, "filedrv: thread_main is quit due to some issue");
		goto on_quit;
	    }
	    job_mutex_.acquire();
	}

        //job_mutex_.acquire();
	current_job = jobs_.front();
	//current_job_->hCompletionEvent_ = hCompletionEvent;
	//ResetEvent(hCompletionEvent);
	jobs_.pop_front();
	job_mutex_.release();
	

	if (!current_job->is_job_valid()) {
	    CLog::Warning(FILEDRV, "Job %s cancelled because task %d doesn't exist", current_job->name_, current_job->task_pid_);

	} else {
	    CLog::Detail(FILEDRV, current_job->task_, "Executing asynchronous file job %s", current_job->name_);

	    int result = current_job->execute();

	    File_Service::Instance()->LockPendingTask();
	    if (File_Service::Instance()->HasPendingTask_NoLock(current_job->task_pid_)) {
		current_job->task_->SetLastError(result);
		File_Service::Instance()->RemovePendingTask_NoLock(current_job->task_pid_);
		File_Service::Instance()->AddCompletedTask_NoLock(current_job->task_);
	    }
	    File_Service::Instance()->UnlockPendingTask();
	}

	//SetEvent(hCompletionEvent);

	job_mutex_.acquire();
	//current_job->hCompletionEvent_ = NULL;
	delete current_job;
	job_mutex_.release();
    }

on_quit:
    ;
}


//-----------------------------------------------------------------------------

File_Service * File_Service::Create()
{
    if (fileSvcInstance == NULL)
	fileSvcInstance = new File_Service;
    return fileSvcInstance;
}


//-----------------------------------------------------------------------------
// Constructor
File_Service::File_Service()
: ServiceProvider(FILEDRV, true, true, Lower_Priority, "File I/O API driver")
{
    m_Worker = new File_Worker;
}


File_Service::~File_Service()
{
    fileSvcInstance = NULL;
    delete m_Worker;
}

//-----------------------------------------------------------------------------
// Initialize the file driver
bool File_Service::Load()
{
    m_TLS = Kernel::Instance()->AllocTLS();

    CLog::Detail(FILEDRV, "Creating and starting file API"); 

    int provider_id = API_PROVIDER_ID_FILE;
    m_API = new File_API(provider_id, this);
    if (m_API->Init() != OK) {
	delete m_API;
	m_API = NULL;
	return false;
    }

    CLog::Detail(FILEDRV, "Starting file worker"); 
    if (!m_Worker->initialize()) {
	CLog::Error(FILEDRV, "Unable to initialize file worker"); 
	//return false;
    }

    CLog::Detail(FILEDRV, "Initialization success!"); 
    return true;
}


//-----------------------------------------------------------------------------
// Shutdown the filedriver
bool File_Service::Unload()
{
    Kernel::Instance()->FreeTLS( m_TLS );

    CLog::Detail(FILEDRV, "Shutting down API"); 
    m_API->Shutdown();
    delete m_API;
    m_API = NULL;

    CLog::Detail(FILEDRV, "Shutting down worker"); 
    m_Worker->shutdown();

    CLog::Detail(FILEDRV, "Shutdown complete"); 
    return true;
}


void File_Service::AddPendingTask(unsigned pid)
{
    ACE_Guard <ACE_Thread_Mutex> guard(m_TaskMutex_);
    m_PendingTasks_.insert(pid);
}

void File_Service::AddCompletedTask_NoLock(CTask *task)
{
    m_CompletedTasks_.insert(task);
}

void File_Service::RemovePendingTask_NoLock(unsigned pid)
{
    m_PendingTasks_.erase(pid);
}

void File_Service::RemovePendingTask(unsigned pid)
{
    ACE_Guard <ACE_Thread_Mutex> guard(m_TaskMutex_);
    RemovePendingTask_NoLock(pid);
}

bool File_Service::HasPendingTask_NoLock(unsigned pid)
{
    return m_PendingTasks_.find(pid) != m_PendingTasks_.end();
}

bool File_Service::HasPendingTask(unsigned pid)
{
    ACE_Guard <ACE_Thread_Mutex> guard(m_TaskMutex_);
    return HasPendingTask_NoLock(pid);
}

void File_Service::LockPendingTask()
{
    m_TaskMutex_.acquire();
}

void File_Service::UnlockPendingTask()
{
    m_TaskMutex_.release();
}

//-----------------------------------------------------------------------------
/** HandleEvents: Descendent shall implement this abstract method.
    It is called by Service Provider Manager
 */
void File_Service::HandleEvents()
{
    ACE_Guard <ACE_Thread_Mutex> guard(m_TaskMutex_);
    std::set<CTask*>::iterator it, end = m_CompletedTasks_.end();

    for (it = m_CompletedTasks_.begin(); it!=end; ++it) {
	CTask *task = *it;
	task->Wakeup();
    }

    m_CompletedTasks_.clear();
}

//-----------------------------------------------------------------------------
// Internal:
// Set task's last error based on system's errno
void File_Service::SetTaskLastError( CTask *task)
{
    switch (errno) {
    case EACCES:
	task->SetLastError(Err::FILE_ACCESS);
	break;
    case ENOENT:
	task->SetLastError(Err::FILE_NOENT);
	break;
    case EINVAL:
	task->SetLastError(Err::FILE_INVAL);
	break;
    case EBADF:
	task->SetLastError(Err::FILE_BADNUMBER);
	break;
    case EEXIST:
	task->SetLastError(Err::FILE_EXIST);
	break;
    case EMFILE:
	task->SetLastError(Err::FILE_SYSTOOMANY);
	break;
    case ENOSPC:
	task->SetLastError(Err::FILE_NOSPACE);
	break;
    default:
	task->SetLastError(Err::FILE_UNKNOWN);
	break;
    }
}


//-----------------------------------------------------------------------------
// Internal: get file handle based on task's handle
FILE* File_Service::GetFileObject( CTask *task, int hnd, const char *operation)
{
    if (hnd < 0 || hnd >= MAXOPENFHANDLE) {
	 CLog::Warning( FILEDRV, task, "%s: invalid handle %d", operation, hnd);
	task->SetLastError(Err::FILE_INVALIDHANDLE);
	return NULL;
    }

    CTaskState *ts = (CTaskState*)task->GetTLSValue( m_TLS );
    if (!ts) {
	 CLog::Warning( FILEDRV, task, "%s: task does not have file handles!", operation);
	task->SetLastError(Err::FILE_INVALIDHANDLE);
	return NULL;
    }

    FILE *fhnd = ts->m_Handle[hnd];
    if (!fhnd) {
	 CLog::Warning( FILEDRV, task, "%s: invalid handle %d", operation, hnd);
 
	task->SetLastError(Err::FILE_INVALIDHANDLE);
	return NULL;
    }

    return fhnd;
}


//-----------------------------------------------------------------------------
// Called  by kernel when new task created
//
bool File_Service::AttachTask(CTask *pTask)
{
    CTaskState *ts;
    NEW_RETURN_ (ts, CTaskState);
    if (ts == NULL)
	return false;

    pTask->SetTLSValue( m_TLS, (int)ts);
    return true;
}


//-----------------------------------------------------------------------------
// Called  by kernel when task is about to be destroyed
// When detached, close all opened file handles
//
void File_Service::DetachTask( CTask *task)
{
    CTaskState *ts = (CTaskState*)task->GetTLSValue( m_TLS );
    if (!ts)
	return;

    for (int i=0; i<MAXOPENFHANDLE; i++) {
	if (ts->m_Handle[i]) {
	    fclose(ts->m_Handle[i]);
	    ts->m_Handle[i] = NULL;
	}
    }

    if (ts->m_FindHandle != -1) {
	_findclose(ts->m_FindHandle);
	ts->m_FindHandle = -1;
    }
    delete ts;


    LockPendingTask();
    RemovePendingTask_NoLock( task->GetPID() );
    m_CompletedTasks_.erase(task);
    UnlockPendingTask();
}


//-----------------------------------------------------------------------------
bool File_Service::CancelJob(CTask *task)
{
    unsigned pid = task->GetPID();

    ACE_Guard <ACE_Thread_Mutex> guard(ACE_Thread_Mutex);

    // If the job for the task has already completed, then do nothing, the task
    // will be woken up immediately by HandleEvents.
    // Otherwise cancel the job.
    //
    std::set<CTask*>::iterator it = m_CompletedTasks_.find(task);
    if (it == m_CompletedTasks_.end()) {
	if (HasPendingTask_NoLock(pid)) {
	    task->Wakeup();
	    RemovePendingTask_NoLock(pid);
	}
    }

    return true;
}

//-----------------------------------------------------------------------------
// Show status
//
bool File_Service::OnShowStatus(String & output)
{
    char s[128];

    ACE_Guard <ACE_Thread_Mutex> guard(ACE_Thread_Mutex);

    sprintf(s, SHOW_STATUS_INDENT "Number of worker threads: %d\n", File_Worker::MAX_WORKER_THREADS);
    output += s;
    sprintf(s, SHOW_STATUS_INDENT "Number of pending tasks: %d task(s)\n", m_PendingTasks_.size());
    output += s;
    return true;
}

//-----------------------------------------------------------------------------
// Open file for text reading
//
STATUS  File_Service::API_FileOpenForRead( CTask *task, int *rethnd, const char *fname)
{
    CLog::Detail(FILEDRV, task, "FileOpenForRead file='%s'", fname); 

    *rethnd = -1;

    // validate TLS
    CTaskState *ts = (CTaskState*)task->GetTLSValue( m_TLS );
    if (!ts) {
	task->SetLastError( -1 );
	 CLog::Error( FILEDRV, "Task %u does not have TLS", task->GetPID()); 
	return OK;
    }

    // normalize filename
    String filename;
    sl::MakeFullPath( &filename, task->GetAppDirectory(), fname);


    // allocate handle in the TLSfor the opened file
    int slot = ts->FindSlot();
    if (slot < 0) {
	 CLog::Error( FILEDRV, task, "FileOpenForRead: Too many opened files");

	task->SetLastError(Err::FILE_TOOMANY);
	return OK;
    }

    CLog::Detail(FILEDRV, task, "...handle %d allocated", slot); 

    File_OpenForRead_Job *job;
    NEW_RETURN_ (job, File_OpenForRead_Job( task, filename.c_str(), rethnd, slot, &ts->m_Handle[slot]));
    if (!job) {
	task->SetLastError(-1);
	return OK;
    }

    m_Worker->submit_job (job);
    return OK;
}


//-----------------------------------------------------------------------------
// Open file for text writing
//
STATUS  File_Service::API_FileOpenForWrite( CTask *task, int *rethnd, const char *fname)
{
    CLog::Detail(FILEDRV, task, "FileOpenForWrite file='%s'", fname); 

    *rethnd = -1;

    // validate TLS
    CTaskState *ts = (CTaskState*)task->GetTLSValue( m_TLS );
    if (!ts) {
	task->SetLastError( -1 );
	 CLog::Error( FILEDRV, task, "Task does not have TLS"); 
	return OK;
    }

    // normalize file name
    String filename;
    sl::MakeFullPath( &filename, task->GetAppDirectory(), fname);


    
    // allocate handle in the TLS

    int slot = ts->FindSlot();
    if (slot < 0) {
	 CLog::Warning( FILEDRV, task, "FileOpenForWrite: too many opened files");
	task->SetLastError(Err::FILE_TOOMANY);
	return OK;
    }

    CLog::Detail(FILEDRV, task, "...handle %d allocated", slot); 

    File_OpenForWrite_Job *job;
    NEW_RETURN_ (job, File_OpenForWrite_Job (task, filename.c_str(), rethnd, slot, &ts->m_Handle[slot]));
    if (!job) {
	task->SetLastError(-1);
	return OK;
    }
    m_Worker->submit_job (job);
    return OK;
}


//-----------------------------------------------------------------------------
// Write a line to a file
//
STATUS  File_Service::API_FileWriteLine( CTask *task, int hnd, const char *line)
{
    CLog::Detail(FILEDRV, task, "FileWriteLine hnd=%d", hnd); 

    FILE *fhnd = GetFileObject( task, hnd, "FileWriteLine");
    if (!fhnd) {
	return OK;
    }
  

    File_WriteLine_Job *job;
    NEW_RETURN_ (job, File_WriteLine_Job(task, fhnd, line));
    if (!job) {
	task->SetLastError(-1);
	return OK;
    }

    m_Worker->submit_job (job);

    return OK;
}


//-----------------------------------------------------------------------------
// Read a line from a file
//
STATUS  File_Service::API_FileReadLine( CTask *task, int hnd, char *line)
{
    CLog::Detail(FILEDRV, task, "FileReadLine hnd=%d", hnd);

    *line = '\0';

    FILE *fhnd = GetFileObject( task, hnd, "FileReadLine");
    if (!fhnd) {
	return OK;
    }

    File_ReadLine_Job *job;
    NEW_RETURN_ (job, File_ReadLine_Job(task, fhnd, line));
    if (!job) {
	task->SetLastError(-1);
	return OK;
    }

    m_Worker->submit_job (job);
    return OK;
}


//-----------------------------------------------------------------------------
// Close a file
//
STATUS  File_Service::API_FileClose( CTask *task, int hnd)
{
    CLog::Detail(FILEDRV, task, "FileClose hnd=%d", hnd); 

    // get file handle
    FILE *fhnd = GetFileObject( task, hnd, "FileClose");
    if (!fhnd)
	return OK;

    
    // free handle slot.
    CTaskState *ts = (CTaskState*)task->GetTLSValue( m_TLS );
    ts->ReleaseHandle(fhnd);

    // For closing, we execute immediately.
    // We don't want job to be cancelled when executing close().
    /*
    // submit job
    File_Close_Job *job;
    NEW_RETURN_ (job, File_Close_Job (task, fhnd));
    if (!job) {
	task->SetLastError(-1);
	return OK;
    }

    m_Worker->submit_job(job);
    */
    fclose(fhnd);

    return OK;
}


//-----------------------------------------------------------------------------
// Check if file is EOF
//
STATUS  File_Service::API_FileIsEOF( CTask *task, int *ret, int hnd)
{
    CLog::Detail(FILEDRV, task, "FileIsEOF hnd=%d", hnd); 

    // get file handle
    FILE *fhnd = GetFileObject( task, hnd, "FileIsEOF");
    if (!fhnd)
	return OK;

    // Submit job
    File_IsEOF_Job *job;
    NEW_RETURN_ (job, File_IsEOF_Job (task, fhnd, ret));
    if (!job) {
	task->SetLastError(-1);
	return OK;
    }

    m_Worker->submit_job (job);

    return OK;
}


//-----------------------------------------------------------------------------
// Determine whether a file exists
// Return:
//  APPTRUE : exists
//  APPFALSE: doesn't exist
//
STATUS  File_Service::API_FileExist( CTask *task, int *ret, const char *fname)
{
    CLog::Detail(FILEDRV, task, "FileExist filename='%s'", fname); 

    // normalize filename
    String filename;
    sl::MakeFullPath( &filename, task->GetAppDirectory(), fname);

    // Submit job
    File_Exist_Job *job;
    NEW_RETURN_ (job, File_Exist_Job (task, filename.c_str(), ret));
    if (!job) {
	task->SetLastError(-1);
	return OK;
    }

    m_Worker->submit_job(job);
    return OK;
}


//-----------------------------------------------------------------------------
// Delete a file
//
STATUS  File_Service::API_FileDelete( CTask *task, const char *fname)
{
    CLog::Detail(FILEDRV, task, "FileDelete filename='%s'", fname); 

    // normalize filename
    String filename;
    sl::MakeFullPath( &filename, task->GetAppDirectory(), fname);

    // Submit job
    File_Delete_Job *job;
    NEW_RETURN_ (job, File_Delete_Job (task, filename.c_str()));
    if (!job) {
	task->SetLastError(-1);
	return OK;
    }

    m_Worker->submit_job (job);
    return OK;
}


//-----------------------------------------------------------------------------
// Rename a file
//
STATUS  File_Service::API_FileRename( CTask *task, const char *fold, const char *fnew)
{
    String oldname, newname;

    // normalize oldname file
    sl::MakeFullPath( &oldname, task->GetAppDirectory(), fold);


    // normalize new name
    sl::MakeFullPath( &newname, task->GetAppDirectory(), fnew);


    // Submit job.
    File_Rename_Job *job;
    NEW_RETURN_ (job, File_Rename_Job (task, oldname.c_str(), newname.c_str()));
    if (!job) {
	task->SetLastError(-1);
	return OK;
    }

    m_Worker->submit_job (job);
    return OK;
}


//-----------------------------------------------------------------------------
// Retrieve the size of a file.
// Returns:
//  0-positive	: the actual size
//  -1		: error (probably file doesn't exists, etc)
//
STATUS  File_Service::API_FileGetSize( CTask *task, int *ret, const char *fname)
{

    String filename;

    // normalize filename
    sl::MakeFullPath( &filename, task->GetAppDirectory(), fname);


    File_GetSize_Job *job = new File_GetSize_Job (task, filename.c_str(), ret);
    m_Worker->submit_job (job);

    return OK;
}


//-----------------------------------------------------------------------------
// Copy a file
//
STATUS  File_Service::API_FileCopy( CTask *task, const char *fsrc, const char *fdest)
{

    String srcname, destname;

    // normalize src file
    sl::MakeFullPath( &srcname, task->GetAppDirectory(), fsrc);


    // normalize dest name
    sl::MakeFullPath( &destname, task->GetAppDirectory(), fdest);


    File_Copy_Job *job = new File_Copy_Job (task, srcname.c_str(), destname.c_str());
    m_Worker->submit_job (job);


    return OK;
}

int _findnext(intptr_t fhandle, struct _finddata_t* fileinfo) {

    struct dirent entry, *result;
    struct fhandle_t* handle;
    struct stat st;

    if (fhandle == DOTDOT_HANDLE) {
        errno = ENOENT;
        return -1;
    }

    if (fhandle == INVALID_HANDLE || !fileinfo) {
        errno = EINVAL;
        return -1;
    }

    handle = (struct fhandle_t*) fhandle;

    while (readdir_r(handle->dstream, &entry, &result) == 0 && result != NULL) {

        if (!handle->dironly && !match_spec(handle->spec, entry.d_name))
            continue;

        if (fstatat(dirfd(handle->dstream), entry.d_name, &st, 0) == -1)
            return -1;

        if (handle->dironly && !S_ISDIR(st.st_mode))
            continue;

        fill_finddata(&st, entry.d_name, fileinfo);

        return 0;
    }

    errno = ENOENT;
    return -1;
}

static int find_next_file(long handle, _finddata_t *finddata)
{
    int status = _findnext(handle, finddata);

    while (status==0 && (finddata->attrib & _A_SUBDIR))
	status = _findnext(handle, finddata);

    return status;
}

STATUS File_Service::API_FileFindFirst( CTask *task, char *filename, char *spec )
{
    *filename = '\0';
    CTaskState *ts = (CTaskState*)task->GetTLSValue( m_TLS );
    if (!ts) {
	CLog::Error( FILEDRV, task, "Task has no TLS in FileFindFirst(%s)", spec);
	task->SetLastError(Err::FILE_INVALIDHANDLE);
	return OK;
    }

    if (ts->m_FindHandle != -1) {
	_findclose(ts->m_FindHandle);
    }

    String spec_normalized;
    //char spec_normalized[520];

    // normalize filename
    sl::MakeFullPath( &spec_normalized, task->GetAppDirectory(), spec);

    // Convert '/' to '\\'
    unsigned pos;
    while ((pos=spec_normalized.find('/')) != spec_normalized.npos) {
	spec_normalized.replace(pos, 1, 1, '\\');
    }

    _finddata_t finddata;
    ts->m_FindHandle = _findfirst(spec_normalized.c_str(), &finddata);
    //ts->m_FindHandle = _findfirst(spec_normalized, &finddata);

    if (ts->m_FindHandle == -1) {
	CLog::Error( FILEDRV, task, "Error in FileFindFirst (spec=%s)", spec);
	task->SetLastError(Err::FILE_INVALIDHANDLE);
	return OK;
    }

    // Omit directories.
    if (finddata.attrib & _A_SUBDIR) {
	int status = find_next_file(ts->m_FindHandle, &finddata);
	if (status == -1) {
	    _findclose(ts->m_FindHandle);
	    ts->m_FindHandle = -1;
	    return OK;
	}
    }

    safe_strcpy(filename, finddata.name, MAXAPPSTRLEN);
    return OK;
}

STATUS File_Service::API_FileFindNext( CTask *task, char *filename )
{
    *filename = '\0';

    CTaskState *ts = (CTaskState*)task->GetTLSValue( m_TLS );
    if (!ts) {
	CLog::Error( FILEDRV, task, "Task has no TLS in FileFindNext");
	task->SetLastError(Err::FILE_INVALIDHANDLE);
	return OK;
    }

    if (ts->m_FindHandle == -1) {
	// Either no more match or task didn't call FileFindFirst.
	return OK;
    }

    _finddata_t finddata;
    int status = find_next_file(ts->m_FindHandle, &finddata);
    if (status == -1) {
	_findclose(ts->m_FindHandle);
	ts->m_FindHandle = -1;
    } else {
	safe_strcpy(filename, finddata.name, MAXAPPSTRLEN);
    }

    return OK;
}

#undef FILEDRV

//-----------------------------------------------------------------------------
// FILE DRIVER API
//-----------------------------------------------------------------------------

#define FILEAPI	"fileapi"

File_API::File_API( int provider_id, File_Service *d)
: CAPIProvider(provider_id, FILEAPI)
{
    m_Driver = d;
}

STATUS File_API::Init()
{
    CLog::Detail( FILEAPI, "Registering API"); 

    if (API_Provider_Manager::Instance()->RegisterProvider(this) == NOT_OK) {
	CLog::Error( FILEAPI, "Unable to register provider");
	return NOT_OK;
    }

    enum {
	FileOpenForRead,
	FileOpenForWrite,
	FileWriteLine,
	FileReadLine,
	FileClose,
	FileExist,
	FileDelete,
	FileRename,
	FileGetSize,
	FileCopy,
	FileIsEOF,
	FileFindFirst,
	FileFindNext,
    };

    // STATUS  API_FileOpenForRead( CTask *task, int *rethnd, const char *filename);
    if (RegisterAPI( "FileOpenForRead", FileOpenForRead, true,
		     CVariant::VT_INT,
		     (CAPIProvider::APIPROC) &File_API::API_FileOpenForRead,
		     CAPIDef::CDECL_TYPE, 3,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "ret_hnd", BYREF, CVariant::VT_RET),
		     CArgType( "filename", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_FileOpenForWrite( CTask *task, int *rethnd, const char *filename);
    if (RegisterAPI( "FileOpenForWrite", FileOpenForWrite, true,
		     CVariant::VT_INT,
		     (CAPIProvider::APIPROC) &File_API::API_FileOpenForWrite,
		     CAPIDef::CDECL_TYPE, 3,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "ret_hnd", BYREF, CVariant::VT_RET),
		     CArgType( "filename", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_FileWriteLine( CTask *, int hnd, const char *line);
    if (RegisterAPI( "FileWriteLine", FileWriteLine, true,
		     CVariant::VT_VOID,
		     (CAPIProvider::APIPROC) &File_API::API_FileWriteLine,
		     CAPIDef::CDECL_TYPE, 3,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "hnd", BYVAL, CVariant::VT_INT),
		     CArgType( "line", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_FileReadLine( CTask *, int hnd, char *line);
    if (RegisterAPI( "FileReadLine", FileReadLine, true,
		     CVariant::VT_VOID,
		     (CAPIProvider::APIPROC) &File_API::API_FileReadLine,
		     CAPIDef::CDECL_TYPE, 3,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "hnd", BYVAL, CVariant::VT_INT),
		     CArgType( "line", BYREF, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_FileIsEOF( CTask *, int *ret, int hnd);
    if (RegisterAPI( "FileIsEOF", FileIsEOF, true,
		     CVariant::VT_INT,
		     (CAPIProvider::APIPROC) &File_API::API_FileIsEOF,
		     CAPIDef::CDECL_TYPE, 3,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "ret", BYREF, CVariant::VT_RET),
		     CArgType( "hnd", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }


    // STATUS  API_FileClose( CTask *, int hnd);
    if (RegisterAPI( "FileClose", FileClose, true,
		     CVariant::VT_VOID,
		     (CAPIProvider::APIPROC) &File_API::API_FileClose,
		     CAPIDef::CDECL_TYPE, 2,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "hnd", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }


    // STATUS API_FileExist( CTask *task, int *bret, const char *filename);
    if (RegisterAPI( "FileExist", FileExist, true,
		     CVariant::VT_INT,
		     (CAPIProvider::APIPROC) &File_API::API_FileExist,
		     CAPIDef::CDECL_TYPE, 3,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "ret_val", BYREF, CVariant::VT_RET),
		     CArgType( "filename", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }


    // STATUS API_FileDelete( CTask *task, const char *filename);
    if (RegisterAPI( "FileDelete", FileDelete, true,
		     CVariant::VT_VOID,
		     (CAPIProvider::APIPROC) &File_API::API_FileDelete,
		     CAPIDef::CDECL_TYPE, 2,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "filename", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS API_FileRename( CTask *task, const char *oldname, const char *newname);
    if (RegisterAPI( "FileRename", FileRename, true,
		     CVariant::VT_VOID,
		     (CAPIProvider::APIPROC) &File_API::API_FileRename,
		     CAPIDef::CDECL_TYPE, 3,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "oldname", BYVAL, CVariant::VT_STRING),
		     CArgType( "newname", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }


    // STATUS API_FileGetSize( CTask *task, int *ret, const char *filename);
    if (RegisterAPI( "FileGetSize", FileGetSize, true,
		     CVariant::VT_INT,
		     (CAPIProvider::APIPROC) &File_API::API_FileGetSize,
		     CAPIDef::CDECL_TYPE, 3,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "ret_val", BYREF, CVariant::VT_RET),
		     CArgType( "filename", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS API_FileCopy( CTask *task, const char *src, const char *dest);
    if (RegisterAPI( "FileCopy", FileCopy, true,
		     CVariant::VT_VOID,
		     (CAPIProvider::APIPROC) &File_API::API_FileCopy,
		     CAPIDef::CDECL_TYPE, 3,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "src_name", BYVAL, CVariant::VT_STRING),
		     CArgType( "dest_name", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }

    //STATUS API_FileFindFirst( CTask *task, char *filename, char *spec );
    if (RegisterAPI( "FileFindFirst", FileFindFirst, false,
		     CVariant::VT_STRING,
		     (CAPIProvider::APIPROC) &File_API::API_FileFindFirst,
		     CAPIDef::CDECL_TYPE, 3,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "file", BYREF, CVariant::VT_RET),
		     CArgType( "spec", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }

    //STATUS API_FileFindNext( CTask *task, char *filename );
    if (RegisterAPI( "FileFindNext", FileFindNext, false,
		     CVariant::VT_STRING,
		     (CAPIProvider::APIPROC) &File_API::API_FileFindNext,
		     CAPIDef::CDECL_TYPE, 2,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "file", BYREF, CVariant::VT_RET)) == NOT_OK) {
	return NOT_OK;
    }

    CLog::Debug( FILEAPI, "Initialized");
    return OK;
}

//-----------------------------------------------------------------------------
STATUS File_API::Shutdown()
{
    API_Provider_Manager::Instance()->UnregisterProvider(this);
    return OK;
}



//-----------------------------------------------------------------------------
// File_Service::CTaskState
//
File_Service::CTaskState::CTaskState() 
{ 
    memset( m_Handle, 0, MAXOPENFHANDLE*sizeof(FILE*)); 
    m_FindHandle = -1;
}


int File_Service::CTaskState::FindSlot()
{
    int i;
    for (i=0; i<MAXOPENFHANDLE; ++i) {
	if (m_Handle[i] == NULL)
	    return i;
    }
    return -1;
}

void File_Service::CTaskState::ReleaseHandle (FILE *fhnd)
{
    int i;
    for (i=0; i<MAXOPENFHANDLE; ++i) {
	if (m_Handle[i] == fhnd) {
	    m_Handle[i] = NULL;
	}
    }
}


//-----------------------------------------------------------------------------
// File_API 
//
STATUS File_API::API_FileOpenForRead( CTask *task, int *rethnd, const char *filename) 
{
    return m_Driver->API_FileOpenForRead(task, rethnd, filename);
}

//-----------------------------------------------------------------------------
STATUS File_API::API_FileOpenForWrite( CTask *task, int *rethnd, const char *filename) 
{
    return m_Driver->API_FileOpenForWrite( task, rethnd, filename);
}

//-----------------------------------------------------------------------------
STATUS File_API::API_FileWriteLine( CTask *task, int hnd, const char *line) 
{
    return m_Driver->API_FileWriteLine( task, hnd, line);
}

//-----------------------------------------------------------------------------
STATUS File_API::API_FileReadLine( CTask *task, int hnd, char *line) 
{
    return m_Driver->API_FileReadLine( task, hnd, line);
}

//-----------------------------------------------------------------------------
STATUS File_API::API_FileClose( CTask *task, int hnd) 
{
    return m_Driver->API_FileClose( task, hnd);
}

//-----------------------------------------------------------------------------
STATUS File_API::API_FileIsEOF( CTask *task, int *ret, int hnd) 
{
    return m_Driver->API_FileIsEOF(task, ret, hnd);
}

//-----------------------------------------------------------------------------
STATUS File_API::API_FileExist( CTask *task, int *bret, const char *filename) 
{
    return m_Driver->API_FileExist( task, bret, filename);
}

//-----------------------------------------------------------------------------
STATUS File_API::API_FileDelete( CTask *task, const char *filename) 
{
    return m_Driver->API_FileDelete( task, filename);
}

//-----------------------------------------------------------------------------
STATUS File_API::API_FileRename( CTask *task, const char *oldname, const char *newname) 
{
    return m_Driver->API_FileRename( task, oldname, newname);
}

//-----------------------------------------------------------------------------
STATUS File_API::API_FileGetSize( CTask *task, int *ret, const char *filename) 
{
    return m_Driver->API_FileGetSize( task, ret, filename);
}

//-----------------------------------------------------------------------------
STATUS File_API::API_FileCopy( CTask *task, const char *src, const char *dest) 
{
    return m_Driver->API_FileCopy( task, src, dest);
}

STATUS File_API::API_FileFindFirst( CTask *task, char *filename, char *spec )
{
    return m_Driver->API_FileFindFirst(task, filename, spec);
}

STATUS File_API::API_FileFindNext( CTask *task, char *filename )
{
    return m_Driver->API_FileFindNext(task, filename);
}

/* STATUS _findclose(long h)
{
   
   if (fileInfo.Size()>h)
   {
      _findinfo_t* fi = fileInfo[h];
      fileInfo.RemoveAtIndex(h);
      delete fi;
      return 0;   
   }
   else
   {
      return -1;
   }
   
} */ 

#undef FILEAPI
