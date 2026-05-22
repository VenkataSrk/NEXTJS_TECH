/*----------------------------------------------------------------* 
 * $Archive: /xGate/ConsoleMgr.h $
 * $Date: 27/04/01 13:34 $
 * $Revision: 1 $
 *
 * $History: ConsoleMgr.h $
 * 
 * *****************  Version 1  *****************
 * User: Junanto      Date: 27/04/01   Time: 13:34
 * Created in $/xGate
 * added Display support to xGate
 * 
 *----------------------------------------------------------------*/

/*----------------------------------------------------------------* 
 * Description:
 * Console Server is the module which is responsible to maintain
 * application output display and to distribute each update to each of
 * connected clients (GUI display).
 *----------------------------------------------------------------*/

#if !defined(CT2_CONSOLEMGR_H__INCLUDED_)
#define CT2_CONSOLEMGR_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "serviceprovider.h"
#include <queue>	//linux porting

//The below using statement for linux porting
using std::priority_queue;
using std::vector;
using std::greater;

class ConsoleAPI;	//linux porting

/** Forward declaration
 */
class ConsoleServer;


/** ConsoleManager: Exported class from Console module
 */
class ConsoleManager : public ServiceProvider
{
    friend class ConsoleAPI;

    // typedefs
    typedef greater<int>	GreaterInt;
    typedef vector<int>	VectorInt;
    typedef std::priority_queue<int,VectorInt,GreaterInt> OrderedInt;

public:
    /** ConsoleManager's destructor
     */
    virtual ~ConsoleManager();

    /** Create a singleton instance of AculabSignalling
     */
    static ConsoleManager *Create();

    /** Instance: Returns a pointer to this singleton instance
     */
    static ConsoleManager *Instance();

    /** This function will be called by the service framework after 
        instantiation of this class, to allow the instance to initialize
	itself.
     */
    virtual bool Load();

    /** This function will be called by the service framework before it is
        destroyed/deleted, to allow the instance to cleanup itself.
     */
    virtual bool Unload();

    /** This function will be called whenever a new task is created.
     */
    virtual bool AttachTask(CTask *task);

    /** This function will be called whenever a task is about to be deleted.
     */
    virtual void DetachTask(CTask *task);

    /** HandleEvents: It is called continuously by Service Provider Manager
     */
    virtual void HandleEvents();

private:
    /** Private constructor
     */
    ConsoleManager();

    /** AllocRow: Allocate an empty row for display purpose. If there is no
	available free row in the pool, create a new one
     */
    int AllocRow();

    /** FreeRow: Once a row is not used, it is put back in the pool
     */
    void FreeRow(int row);

    /** GetTaskRow: Returns the row number for a particular task
     */
    int GetTaskRow(CTask *task);

    /** SetTaskRow: Set the row number for a particular task
     */
    void SetTaskRow(CTask *task,int row);

    /** Display: API entry point
     */
    void Display(CTask *task,int attr,const char *msg);


    static ConsoleManager *theConsoleManager;

    ConsoleAPI		*_api;
    int			_tlsId;
    int			_nextRow;
    OrderedInt		_freeRows;
    ConsoleServer	*_conServer;
};


#endif // !defined(CT2_CONSOLEMGR_H__INCLUDED_)
