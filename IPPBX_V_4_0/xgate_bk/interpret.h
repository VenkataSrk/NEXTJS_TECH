/* $Header: /xGate/interpret.h 16    4/04/02 3:35p Bennylp $
 *   
 * CTENGINE
 * (C) 2001 Switchlab, Ltd
 *
 * PCODE Interpreter.
 *
 * $Log: /xGate/interpret.h $
 * 
 * 16    4/04/02 3:35p Bennylp
 * Changed High_Res_Timer to highest_Res_Timer, which uses
 * CPU_Timestamp_Clock_Timer.
 * 
 * 15    10/19/01 2:04p Bennylp
 * A litte optimization
 * 
 * 14    10/09/01 6:55p Bennylp
 * Reset times counters on clear counter command
 * 
 * 13    7/25/01 10:29a Bennylp
 * Limit task's execution time.
 * 
 * 12    6/01/01 5:29a Sonny
 * supports function call in switch statement
 * 
 * 11    10/05/01 19:31 Junanto
 * Added handler for invalid p-code
 * 
 * 10    5/10/01 6:15a Sonny
 * shortcut boolean expression evaluation
 * 
 * 9     5/09/01 3:25a Sonny
 * string comparisons added
 * 
 * 8     5/03/01 1:09p Bennylp
 * Changed profiling to 'show times', and more improvements.
 * 
 * 7     4/23/01 10:42p Bennylp
 * Fixed std namespace conflicts.
 * 
 * 6     4/21/01 4:58a Sonny
 * 
 * 5     4/17/01 12:31p Bennylp
 * Integrate icommon.h to interpret.h.
 * 
 * 4     4/12/01 8:59p Bennylp
 * Integration and some bug fixes.
 * 
 * 3     4/12/01 8:00p Bennylp
 * Integration.
 *
 */
#ifndef __INTERPRET_H__
#define __INTERPRET_H__

#include "types.h"
#include "serviceprovider.h"

class CTask;
class Task_Exec_Context;


//-----------------------------------------------------------------------------
/** This class describes basic PCODE type.
 */
union TMEM {
public:
    TMEM() : i(-1) {}
    TMEM(int val) : i(val) {}
    TMEM(float val) : r(val) {}
    TMEM(const TMEM &t) : i(t.i) {}

    int   i;
    float r;
};


//-----------------------------------------------------------------------------
/** This class describes the interpreter for *.slc codes.
 */
class Code_Interpreter : public ServiceProvider {
public:
    /** Create singleton instance of this class.
     */
    static Code_Interpreter * Create();

    /** Get the singleton instance of this class.
     */
    static Code_Interpreter * Instance() {
	return interpreterInstance;
    }

    /** This function will be called by the service framework after 
        instantiation of this class, to allow the instance to initialize
	itself.
     */
    virtual bool Load();

    /** This function will be called by the service framework before it is
        destroyed/deleted, to allow the instance to cleanup itself.
     */
    virtual bool Unload();
    
    /** Execute the current execution pointer for this task.
     */
    void  Execute(CTask*);
    
    /** Get interpreter's total execution time.
     */
    const Time_Value & GetExecutionTime() const {
	return interpreterTime;
    }

    /** Get task's execution quota (in miliseconds)
     */
    unsigned GetExecQuota() const {
	return m_ExecQuota;
    }

    /** Set task's execution quota (in miliseconds)
     */
    void SetExecQuota(unsigned ms) {
	m_ExecQuota = ms;
    }

    /** Show execution time.
     */
    void ShowProfile ( Term_Session * session );

    /** OnClearCounter is called by the framework to clear any statistic values
	maintained by this service provider.
     */
    virtual void OnClearCounter() {
	interpreterTime.set(0,0);
    }


private:
     Code_Interpreter();
    ~Code_Interpreter();

    // the singleton instance.
    static Code_Interpreter *interpreterInstance;

    // configuration parameter
    int	    m_DefStackSize;

    // execution time.
    Time_Value interpreterTime;

    // timer for execution time.
    Highest_Res_Timer interpreterTimer;

    // handler type definition
    typedef void (Code_Interpreter::*HANDLER)();

    // opcode id
    typedef enum {
	  iPUSH=0,
	  iPLOC,
	  iPOP,
	  iPUTS,
	  iPUSHS,
	  iPOPS,
	  iRPUSH,
	  iDUP,
	  iAPUSH,
	  iAPLOC,
	  iSTOR,
	  iLOAD,
	  iSTOS,
	  iDUPS,
	  iMCPY, //ipoeng: SLC 1.6
	  iMCPYS, //ipoeng: SLC 1.6
	  iMCPYSR, //ipoeng: SLC 1.6
	  iPUTSR, //ipoeng: SLC 1.6
	  iNEGI=20,
	  iNEGR,
	  iMULI,
	  iMULR,
	  iDIVI,
	  iDIVR,
	  iADDI,
	  iADDR,
	  iSUBI,
	  iSUBR,
	  iMOD,
	  iINC,
	  iDEC,
	  iADDS,
	  iEQI=40,
	  iNEQI,
	  iLTI,
	  iLTEI,
	  iGTI,
	  iGTEI,
	  iEQR,
	  iNEQR,
	  iLTR,
	  iLTER,
	  iGTR,
	  iGTER,
	  iEQS,
	  iNEQS,
	  iLTS,
	  iLTES,
	  iGTS,
	  iGTES,
	  iJMP=70,
	  iJNZ,
	  iJZ,
	  iAND,
	  iOR,
	  iNOT,
	  iCALL=85,
	  iCAPI,
	  iRET,
	  iPUSHBP,
	  iPOPBP,
	  iMVSPBP,
	  iMVBPSP,
	  iADDSP,
	  iSUBSP,
	  iCLE,
	  iSTE,
	  iNOP,
	  iSAVEHD,
	  iRESTHD,
	  iHALT = 110
    } TOPCODEID;

private:
    enum { MAXPCODE=512 };
    ///List <CAppImage *> m_appimage;

    CTask        *m_apptask;
    Task_Exec_Context *m_context;
    HANDLER       m_IVT[MAXPCODE];
    int		  m_ExecScore[MAXPCODE];
    unsigned	  m_ExecCheckInterval,	// interval when exec time is checked
		  m_ExecQuota;		// max task execution time

    enum {
	Msec_Check_Interval = 5,    // interval (msec) when exec time is checked
    };
    void CalculateCheckInterval();


    /*
     * Keep record of maximum stack usage statistics
     * For every PushX(), check whether the stack usage exceeds the value of m_MaxStack.
     */
    unsigned	  m_MaxStack;	// The maximum task stack usage for current task
				// being executed.

    /* Update task's maximum stack usage.
     */
    void UpdateLocalMaxStat();

    /* When the task's finished executing, update the application's maximum
     * stack usage statistic
     */
    void UpdateAppStackStat();


    TMEM Fetch();
    TMEM Pop();
    void Push(int);
    void Push(float);
    void Push(TMEM);
    void PushUnchecked(int);
    void PushUnchecked(float);
    void PushUnchecked(TMEM);

    void CatchException();

//p-code prototype
    void hInvalidPCode();
    void hPUSH();
    void hPLOC();
    void hPOP();
    void hPUTS();
    void hPUSHS();
    void hPOPS();
    void hRPUSH();
    void hDUP();
    void hAPUSH();
    void hAPLOC();
    void hSTOR();
    void hLOAD();
    void hSTOS();
    void hDUPS();
    void hMCPY(); //ipoeng: SLC 1.6
    void hMCPYS(); //ipoeng: SLC 1.6
    void hMCPYSR(); //ipoeng: SLC 1.6
    void hPUTSR(); //ipoeng: SLC 1.6
    void hNEGI();
    void hNEGR();
    void hMULI();
    void hMULR();
    void hDIVI();
    void hDIVR();
    void hADDI();
    void hADDR();
    void hSUBI();
    void hSUBR();
    void hMOD();
    void hINC();
    void hDEC();
    void hADDS();
    void hEQI();
    void hNEQI();
    void hLTI();
    void hLTEI();
    void hGTI();
    void hGTEI();
    void hEQR();
    void hNEQR();
    void hLTR();
    void hLTER();
    void hGTR();
    void hGTER();
    void hEQS();
    void hNEQS();
    void hLTS();
    void hLTES();
    void hGTS();
    void hGTES();
    void hJMP();
    void hJNZ();
    void hJZ();
    void hAND();
    void hOR();
    void hNOT();
    void hCALL();
    void hCAPI();
    void hRET();
    void hPUSHBP();
    void hPOPBP();
    void hMVSPBP();
    void hMVBPSP();
    void hADDSP();
    void hSUBSP();
    void hCLE();
    void hSTE();
    void hNOP();
    void hSAVEHD();
    void hRESTHD();
    void hHALT();


};

#endif
