/* $Header: /xGate/interpret.cpp 36    3/08/04 7:09p Bennylp $
 *   
 * CTENGINE
 * (C) 2001 Switchlab, Ltd
 *
 * PCODE Interpreter.
 *
 * $Log: /xGate/interpret.cpp $
 * 
 * 36    3/08/04 7:09p Bennylp
 * Halt the task if ExecuteAPI returns NOT_OK
 * 
 * 35    10/21/02 6:38p Bennylp
 * Remove try-catch in interpreter Execute().
 * 
 * 34    23/05/02 9:25 Junanto
 * 
 * 33    22/05/02 19:29 Junanto
 * ***TBD** check IP is still within range before 
 * fetching next instruction
 * 
 * 32    22/05/02 18:54 Junanto
 * Fixed bug when image is corrupted and pcode can be negative
 * 
 * 31    4/04/02 3:35p Bennylp
 * Changed High_Res_Timer to highest_Res_Timer, which uses
 * CPU_Timestamp_Clock_Timer.
 * 
 * 30    1/30/02 12:05p Bennylp
 * Fixed miscelanous bugs related to interpreter.
 * 
 * 29    24/10/01 15:33 Junanto
 * 
 * 28    24/10/01 15:31 Junanto
 * Fixed leaked memory
 * 
 * 27    10/19/01 2:07p Bennylp
 * Fixed bug in m_MaxStat initialization
 * 
 * 26    10/19/01 2:04p Bennylp
 * A litte optimization
 * 
 * 25    10/17/01 8:16p Bennylp
 * Hide some Task_Exec_Context member vars from interpreter
 * 
 * 24    10/10/01 17:36 Sonny
 * check stack before put string index
 * 
 * 23    7/08/01 13:14 Sonny
 * check string index for debugging
 * 
 * 22    8/06/01 7:36p Bennylp
 * Fixed bug: hHALT shouldl immediately return from function.
 * 
 * 21    7/25/01 4:26p Bennylp
 * Calculate how many instructions before exec time is checked.
 * 
 * 20    7/25/01 10:29a Bennylp
 * Limit task's execution time.
 * 
 * 19    17/07/01 12:01 Sonny
 * Fixed bug in hDUPS
 * 
 * 18    7/16/01 12:12p Bennylp
 * Check for string index in hADDS/hSTOS
 * 
 * 17    28/06/01 12:49 Sonny
 * 
 * 16    6/01/01 5:29a Sonny
 * supports function call in switch statement
 * 
* 15    5/26/01 8:04p Bennylp
* Record each call to AcquireSystemLock in DEBUG mode.
* 
* 14    10/05/01 19:31 Junanto
* Added handler for invalid p-code
* 
* 13    5/10/01 6:15a Sonny
* shortcut boolean expression evaluation
* 
* 12    10/05/01 12:40 Junanto
* Replaced "\r\n" sequence to "\n". Telnet translates
* an orphaned "\n" into "\r\n" during output
* 
* 11    10/05/01 11:24 Junanto
* Replaced all "\r\n" with a constant
* 
* 10    5/09/01 3:25a Sonny
* string comparisons added
* 
* 9     5/03/01 1:09p Bennylp
* Changed profiling to 'show times', and more improvements.
* 
* 8     5/01/01 2:39a Sonny
* applicaton statistics
* 
* 7     4/23/01 10:42p Bennylp
* Fixed std namespace conflicts.
* 
* 6     4/21/01 4:58a Sonny
* 
* 5     4/19/01 11:38p Sonny
* 
* 4     4/12/01 8:00p Bennylp
* Integration.
*
*/
#include "stdafx.h"
#include <limits.h>
#include "interpret.h"
#include "api.h"
#include "task.h"
#include "types.h"
#include "log.h"
#include "kernel.h"
#include "term.h"	// Term_Session


#define INTERPRETER  "SYS"
#define NEWLINE      "\n"

#define stricmp strcasecmp
#define strnicmp strncasecmp
#define strcmpi strcasecmp

// default stack size in bytes
#define DEF_STACK_SIZE	230000

Code_Interpreter *Code_Interpreter::interpreterInstance;

//#define I_TRACE(s)	CLog::Detail s
#define I_TRACE(s)

//-----------------------------------------------------------------------------
//extern char *MNEMONICS[];
const char *MNEMONICS[]={  
  "iPUSH",
  "iPLOC",
  "iPOP",
  "iPUTS",
  "iPUSHS",
  "iPOPS",
  "iRPUSH",
  "iDUP",
  "iAPUSH",
  "iAPLOC",
  "iSTOR",
  "iLOAD",
  "iSTOS",
  "iDUPS",
  "iMCPY", //ipoeng: SLC 1.6
  "iMCPYS", //ipoeng: SLC 1.6
  "iMCPYSR", //ipoeng: SLC 1.6
  "iPUTSR", //ipoeng: SLC 1.6
  "",
  "",
  "iNEGI",
  "iNEGR",
  "iMULI",
  "iMULR",
  "iDIVI",
  "iDIVR",
  "iADDI",
  "iADDR",
  "iSUBI",
  "iSUBR",
  "iMOD",
  "iINC",
  "iDEC",
  "iADDS",
  "",
  "",
  "",
  "",
  "",
  "",
  "iEQI",
  "iNEQI",
  "iLTI",
  "iLTEI",
  "iGTI",
  "iGTEI",
  "iEQR",
  "iNEQR",
  "iLTR",
  "iLTER",
  "iGTR",
  "iGTER",
  "iEQS",
  "iNEQS",
  "iLTS",
  "iLTES",
  "iGTS",
  "iGTES",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "iJMP",
  "iJNZ",
  "iJZ",
  "iAND",
  "iOR",
  "iNOT",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "iCALL",
  "iCAPI",
  "iRET",
  "iPUSHBP",
  "iPOPBP",
  "iMVSPBP",
  "iMVBPSP",
  "iADDSP",
  "iSUBSP",
  "iCLE",
  "iSTE",
  "iNOP",
  "iSAVEHD",
  "iRESTHD",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "iHALT"
};

static unsigned long GetTickCount()
{
  struct timeval tv;
  if( gettimeofday(&tv, NULL) != 0 )
    return 0;

  return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

//
// Scoring.
// Every OPCODE has a score value, which serves as approximation on
// how much time is needed to execute the OPCODE compared to the
// SCORE_DEFAULT.
//
enum {
  SCORE_NOP = 0,	    // NOP opcode, the score is relatively very low
  SCORE_DEFAULT = 1,  // simple opcodes
  SCORE_STRING = 64,  // string operations
  SCORE_API = 512     // API calls
};


// Interpreter -------------------------------------------------------------
/** Create singleton instance of this class.
*/
Code_Interpreter * Code_Interpreter::Create()
{
  if (interpreterInstance == NULL)
    interpreterInstance = new Code_Interpreter;
  return interpreterInstance;
}


Code_Interpreter::Code_Interpreter() 
  : ServiceProvider ("interpreter", false, false), m_ExecCheckInterval(0),
  m_ExecQuota(50), m_MaxStack(0)
{
  // first initialize with invalid p-code handler
  int i;
  for (i=0;i<MAXPCODE;i++) 
    m_IVT[i] = &Code_Interpreter::hInvalidPCode;

  m_IVT[iPUSH]=&Code_Interpreter::hPUSH;
  m_IVT[iPLOC]=&Code_Interpreter::hPLOC;
  m_IVT[iPOP]=&Code_Interpreter::hPOP;
  m_IVT[iPUTS]=&Code_Interpreter::hPUTS;
  m_IVT[iPUSHS]=&Code_Interpreter::hPUSHS;
  m_IVT[iPOPS]=&Code_Interpreter::hPOPS;
  m_IVT[iRPUSH]=&Code_Interpreter::hRPUSH;
  m_IVT[iDUP]=&Code_Interpreter::hDUP;
  m_IVT[iAPUSH]=&Code_Interpreter::hAPUSH;
  m_IVT[iAPLOC]=&Code_Interpreter::hAPLOC;
  m_IVT[iSTOR]=&Code_Interpreter::hSTOR;
  m_IVT[iLOAD]=&Code_Interpreter::hLOAD;
  m_IVT[iSTOS]=&Code_Interpreter::hSTOS;
  m_IVT[iDUPS]=&Code_Interpreter::hDUPS;
  m_IVT[iMCPY]=&Code_Interpreter::hMCPY; //ipoeng: SLC 1.6
  m_IVT[iMCPYS]=&Code_Interpreter::hMCPYS; //ipoeng: SLC 1.6
  m_IVT[iMCPYSR]=&Code_Interpreter::hMCPYSR; //ipoeng: SLC 1.6
  m_IVT[iPUTSR]=&Code_Interpreter::hPUTSR; //ipoeng: SLC 1.6
  m_IVT[iNEGI]=&Code_Interpreter::hNEGI;
  m_IVT[iNEGR]=&Code_Interpreter::hNEGR;
  m_IVT[iMULI]=&Code_Interpreter::hMULI;
  m_IVT[iMULR]=&Code_Interpreter::hMULR;
  m_IVT[iDIVI]=&Code_Interpreter::hDIVI;
  m_IVT[iDIVR]=&Code_Interpreter::hDIVR;
  m_IVT[iADDI]=&Code_Interpreter::hADDI;
  m_IVT[iADDR]=&Code_Interpreter::hADDR;
  m_IVT[iSUBI]=&Code_Interpreter::hSUBI;
  m_IVT[iSUBR]=&Code_Interpreter::hSUBR;
  m_IVT[iMOD]=&Code_Interpreter::hMOD;
  m_IVT[iINC]=&Code_Interpreter::hINC;
  m_IVT[iDEC]=&Code_Interpreter::hDEC;
  m_IVT[iADDS]=&Code_Interpreter::hADDS;
  m_IVT[iEQI]=&Code_Interpreter::hEQI;
  m_IVT[iNEQI]=&Code_Interpreter::hNEQI;
  m_IVT[iLTI]=&Code_Interpreter::hLTI;
  m_IVT[iLTEI]=&Code_Interpreter::hLTEI;
  m_IVT[iGTI]=&Code_Interpreter::hGTI;
  m_IVT[iGTEI]=&Code_Interpreter::hGTEI;
  m_IVT[iEQR]=&Code_Interpreter::hEQR;
  m_IVT[iNEQR]=&Code_Interpreter::hNEQR;
  m_IVT[iLTR]=&Code_Interpreter::hLTR;
  m_IVT[iLTER]=&Code_Interpreter::hLTER;
  m_IVT[iGTR]=&Code_Interpreter::hGTR;
  m_IVT[iGTER]=&Code_Interpreter::hGTER;
  m_IVT[iEQS]=&Code_Interpreter::hEQS;
  m_IVT[iNEQS]=&Code_Interpreter::hNEQS;
  m_IVT[iLTS]=&Code_Interpreter::hLTS;
  m_IVT[iLTES]=&Code_Interpreter::hLTES;
  m_IVT[iGTS]=&Code_Interpreter::hGTS;
  m_IVT[iGTES]=&Code_Interpreter::hGTES;
  m_IVT[iJMP]=&Code_Interpreter::hJMP;
  m_IVT[iJNZ]=&Code_Interpreter::hJNZ;
  m_IVT[iJZ]=&Code_Interpreter::hJZ;
  m_IVT[iAND]=&Code_Interpreter::hAND;
  m_IVT[iOR]=&Code_Interpreter::hOR;
  m_IVT[iNOT]=&Code_Interpreter::hNOT;
  m_IVT[iCALL]=&Code_Interpreter::hCALL;
  m_IVT[iCAPI]=&Code_Interpreter::hCAPI;
  m_IVT[iRET]=&Code_Interpreter::hRET;
  m_IVT[iPUSHBP]=&Code_Interpreter::hPUSHBP;
  m_IVT[iPOPBP]=&Code_Interpreter::hPOPBP;
  m_IVT[iMVSPBP]=&Code_Interpreter::hMVSPBP;
  m_IVT[iMVBPSP]=&Code_Interpreter::hMVBPSP;
  m_IVT[iADDSP]=&Code_Interpreter::hADDSP;
  m_IVT[iSUBSP]=&Code_Interpreter::hSUBSP;
  m_IVT[iCLE]=&Code_Interpreter::hCLE;
  m_IVT[iSTE]=&Code_Interpreter::hSTE;
  m_IVT[iNOP]=&Code_Interpreter::hNOP;
  m_IVT[iSAVEHD]=&Code_Interpreter::hSAVEHD;
  m_IVT[iRESTHD]=&Code_Interpreter::hRESTHD;
  m_IVT[iHALT]=&Code_Interpreter::hHALT;

  m_apptask = NULL;
  m_context = NULL;

  m_DefStackSize = DEF_STACK_SIZE;

  // Initialize execution score with default values.
  for (i=0;i<MAXPCODE;i++) 
    m_ExecScore[i] = SCORE_DEFAULT;

  // Initialize some OPCODE with special value.
  m_ExecScore[iNOP] = SCORE_NOP;
  m_ExecScore[iPOPS] = SCORE_STRING;
  m_ExecScore[iSTOS] = SCORE_STRING;
  m_ExecScore[iDUPS] = SCORE_STRING;
  m_ExecScore[iMCPYS] = SCORE_STRING; //ipoeng: SLC 1.6
  m_ExecScore[iMCPYSR] = SCORE_STRING; //ipoeng: SLC 1.6
  m_ExecScore[iPUTSR] = SCORE_STRING; //ipoeng: SLC 1.6
  m_ExecScore[iADDS] = SCORE_STRING;
  m_ExecScore[iEQS] = SCORE_STRING;
  m_ExecScore[iNEQS] = SCORE_STRING;
  m_ExecScore[iLTS] = SCORE_STRING;
  m_ExecScore[iLTES] = SCORE_STRING;
  m_ExecScore[iGTS] = SCORE_STRING;
  m_ExecScore[iGTES] = SCORE_STRING;

  m_ExecScore[iCAPI] = SCORE_API;

  CalculateCheckInterval();
}


//-----------------------------------------------------------------------------
Code_Interpreter::~Code_Interpreter() 
{
  m_apptask = NULL;
  m_context = NULL;
  interpreterInstance = NULL;
}


//-----------------------------------------------------------------------------
bool Code_Interpreter::Load() 
{
  m_DefStackSize = DEF_STACK_SIZE;

  if (interpreterTimer.init() == false) {
    CLog::Error ( INTERPRETER, "Unable to initialize high resolution timer");
    return false;
  }
  /*
     CLog::Info (INTERPRETER, "Interpreter quota check interval is %u msec, which approx. equal to: "
     "%u integer operations, %u string ops, or %u API calls", 
     Msec_Check_Interval,
     m_ExecCheckInterval/m_ExecScore[iMULI],
     m_ExecCheckInterval/m_ExecScore[iADDS],
     m_ExecCheckInterval/m_ExecScore[iCAPI]);
     */
  return true;
}


//-----------------------------------------------------------------------------
bool Code_Interpreter::Unload() 
{
  return true;
}


//-----------------------------------------------------------------------------
inline TMEM Code_Interpreter::Fetch() 
{
  return m_context->m_code[m_context->m_IP++];
}


//-----------------------------------------------------------------------------
/* Update task's maximum stack usage.
*/
inline void Code_Interpreter::UpdateLocalMaxStat() 
{
  // The value of m_MaxStack is in TMEM, not in bytes.
  unsigned stack_usage = m_context->m_SS.max - m_context->m_SP;
  if (stack_usage > m_MaxStack) m_MaxStack = stack_usage;
}

//-----------------------------------------------------------------------------
/* When the task's finished executing, update the application's maximum
 * stack usage statistic
 */
void Code_Interpreter::UpdateAppStackStat()
{
  // Convert the value to bytes.
  m_MaxStack *= 4;

  // Update application's statistic
  if (m_context->GetApplication() && m_MaxStack > m_context->GetApplication()->GetStatMaxStack()) 
  {
    m_context->GetApplication()->SetStatMaxStack(m_MaxStack);
  }

  // Reset
  m_MaxStack = 0;
}


//-----------------------------------------------------------------------------
inline void Code_Interpreter::Push(int i) {
  register Task_Exec_Context *p_context = m_context;
  if (p_context->m_SP > p_context->m_SS.min) {
    --p_context->m_SP;
    p_context->m_data[p_context->m_SP].i = i;
    UpdateLocalMaxStat();
  } else {
    CLog::Error( INTERPRETER, m_apptask, "STACK OVERFLOW");
    hHALT();
    return;
  }
}


//-----------------------------------------------------------------------------
inline void Code_Interpreter::Push(float r) {
  register Task_Exec_Context *p_context = m_context;
  if (p_context->m_SP > p_context->m_SS.min) {
    --p_context->m_SP;
    p_context->m_data[p_context->m_SP].r = r;
    UpdateLocalMaxStat();
  } else {
    CLog::Error( INTERPRETER, m_apptask, "STACK OVERFLOW");
    hHALT();
    return;
  }
}


//-----------------------------------------------------------------------------
inline void Code_Interpreter::Push(TMEM m) 
{
  register Task_Exec_Context *p_context = m_context;
  if ( p_context->m_SP > p_context->m_SS.min) {
    --p_context->m_SP;
    p_context->m_data[p_context->m_SP] = m;
    UpdateLocalMaxStat();
  } else {
    CLog::Error( INTERPRETER, m_apptask, "STACK OVERFLOW");
    hHALT();
    return;
  }
}


//-----------------------------------------------------------------------------
inline void Code_Interpreter::PushUnchecked(int i) {
  register Task_Exec_Context *p_context = m_context;
  p_context->m_data[--p_context->m_SP].i = i;
}


//-----------------------------------------------------------------------------
inline void Code_Interpreter::PushUnchecked(float r) {
  register Task_Exec_Context *p_context = m_context;
  p_context->m_data[--p_context->m_SP].r = r;
}


//-----------------------------------------------------------------------------
inline void Code_Interpreter::PushUnchecked(TMEM m) 
{
  register Task_Exec_Context *p_context = m_context;
  p_context->m_data[--p_context->m_SP] = m;
}


//-----------------------------------------------------------------------------
inline TMEM Code_Interpreter::Pop() {

  /*** jnt ***
    register Task_Exec_Context *p_context = m_context;
    if (p_context->m_SP >= p_context->m_SS.max) {
    CLog::Error( INTERPRETER, m_apptask, "STACK UNDERFLOW");
    hHALT();
    TMEM m;
    m.i = -1;
    return m;
    } else {
    TMEM m(p_context->m_data[p_context->m_SP]);
    p_context->m_SP++;

    return m;
    }
   ***/
  TMEM m(m_context->m_data[m_context->m_SP++]);
  return m;

}


//-----------------------------------------------------------------------------
void Code_Interpreter::CalculateCheckInterval()
{
  // Create a temporary context
  Task_Exec_Context context(NULL);
  context.m_stacksize = 10;
  context.m_code = new TMEM[2];
  TMEM *p_code = (TMEM*) &context.m_code[0];
  p_code->i = iMULI;
  context.m_data = (TMEM*)malloc((context.m_stacksize+1)*sizeof(TMEM));
  context.m_BP = context.m_SP  = 10;
  context.m_SS.min = 0;
  context.m_SS.max = context.m_BP;
  this->m_context = &context;

  // Check execution time after how many instructions
  // The value here is calculated by the loop below.
  Highest_Res_Timer timer;
  timer.init();

  // prevent compiler optimization with volatile
  volatile unsigned long count = 5000;
  volatile int imult = iMULI; 
  DWORD msec;
  TMEM m;
  m.i = 1;

  // Calculate time to execute iMULI opcode
  m_ExecCheckInterval = INT_MAX;
  volatile DWORD dummy_last = 0;
  do {
    unsigned op_exec_cnt = 0;
    count *= 10;

    // Push one integer for iMULI operands
    Push(m);

    timer.start();

    volatile vi;
    TMEM icode;
    for (vi=0; (unsigned)vi<count ; ++vi) {
      // Push one integer for iMULI operands
      Push(m);

      icode = Fetch();
      m_context->m_IP = 0;

      op_exec_cnt += m_ExecScore[icode.i%MAXPCODE];
      (this->*m_IVT[icode.i])();

      if (op_exec_cnt >= m_ExecCheckInterval) {
        dummy_last += GetTickCount();
        //dummy_last += GetTickCount();
      }
    }
    timer.stop();

    Pop();
    msec += timer.get_elapsed_msec();

  } while (msec < 500);

  // Calculate the score required before an execution check is performed.
  m_ExecCheckInterval = (unsigned int)(count * 1.0 * Msec_Check_Interval / msec);

  this->m_context = NULL;
  delete [] (TMEM*)context.m_code;
  context.m_code = NULL;
}


//-----------------------------------------------------------------------------
void Code_Interpreter::Execute(CTask* apptask) {
  TMEM m;

  // start timer.
  interpreterTimer.start();

  m_apptask = apptask;
  m_context = apptask->GetExecContext();
  m_MaxStack = m_context->GetApplication()->GetStatMaxStack() / 4;

  if (m_apptask->HasException() && m_apptask->GetPrevState()==CTask::BLOCKED &&
      m_apptask->CanProcessException()) 
  {
    CatchException();
  }

  unsigned op_exec_cnt = 0;
  DWORD t_exec_start = GetTickCount(), now;

  while (m_apptask->GetState()==CTask::READY && !m_apptask->IsQuitting()) {

    // if anything goes wrong, kill this task
    //try {
    m = Fetch();

    unsigned instr = ((unsigned)m.i)%MAXPCODE;

    op_exec_cnt += m_ExecScore[instr];

    I_TRACE( (INTERPRETER, m_apptask, "%s", MNEMONICS[instr]) );
    //*** jnt *** (this->*m_IVT[instr])();
    if (instr != iNOP) (this->*m_IVT[instr])();

    //} catch (...) {
    //    hInvalidPCode();
    //}

    // Check execution quota
    if (op_exec_cnt >= m_ExecCheckInterval) {
      op_exec_cnt = 0;
      now = GetTickCount();

      // Allow 10% difference
      //if (now - t_exec_start + m_ExecQuota/10 >= m_ExecQuota)
      if (now - t_exec_start >= m_ExecQuota)
        break;
    }
  }

  // end timer.
  interpreterTimer.stop();
  interpreterTime += interpreterTimer.get_elapsed();

  // Update application's task statistic
  UpdateAppStackStat();
}



// ==============================================================================
// p-CODE implementation
// =============================================================================+
void Code_Interpreter::hInvalidPCode() 
{
  CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR: invalid p-code encountered");
  hHALT();
  return;
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hPUSH() 
{
  TMEM m(Fetch());

  Push(m);
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hPLOC() 
{
  TMEM m(Fetch());

  Push(m.i+m_context->m_BP);
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hPOP() 
{
  Pop();
}

//-----------------------------------------------------------------------------
// Allocate m2 number of strings in the string table at BP+m1
void Code_Interpreter::hPUTS() 
{
  TMEM m1(Fetch());
  TMEM m2(Fetch());
  if (m_context->m_SP-m2.i<m_context->m_SS.min) {
    CLog::Error( INTERPRETER, m_apptask, "STACK OVERFLOW");
    hHALT();
    return;
  }
  /** jnt **
    int data_index;
    while (m2.i > 0) {	
    m1.i--;
    if (m_context->m_stringidx == m_context->GetStringTabSize()) {
    CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR [hPUTS]: string stack overflow");
    hHALT();
    return;
    }
    data_index = m_context->m_BP + m1.i;
    if (!m_context->CheckDataIndex(m_apptask, data_index)) {
    CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR [hPUTS]: array out of bound");
    hHALT();
    return;
    }

    m_context->m_data[data_index].i = m_context->m_stringidx++;
    m_context->m_sstack++;
    m2.i--;
    }
    if (m_context->GetApplication()->GetStatMaxSStack() < m_context->m_sstack)
    m_context->GetApplication()->SetStatMaxSStack(m_context->m_sstack);
   ***/
  if (m_context->m_stringidx+m2.i >= m_context->GetStringTabSize()) {
    CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR [hPUTS]: string stack overflow");
    hHALT();
    return;
  }
  while (m2.i--) {	
    m1.i--;
    m_context->m_data[m_context->m_BP + m1.i].i = m_context->m_stringidx++;
    m_context->m_sstack++;
  }
  if (m_context->GetApplication()->GetStatMaxSStack() < m_context->m_sstack)
    m_context->GetApplication()->SetStatMaxSStack(m_context->m_sstack);
}

//-----------------------------------------------------------------------------
// Allocate a new string and push it's address on the stack
void Code_Interpreter::hPUSHS() {
  if (m_context->m_stringidx == m_context->GetStringTabSize()) {
    CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR: string stack overflow");
    hHALT();
    return;
  }
  //    m_context->m_StatAllString += m_context->GetStringTab(m_context->m_stringidx).size();
  Push(m_context->m_stringidx++);
  m_context->m_sstack++;
  if ((int)m_context->GetApplication()->GetStatMaxSStack() < m_context->m_sstack)
    m_context->GetApplication()->SetStatMaxSStack(m_context->m_sstack);
}

//-----------------------------------------------------------------------------
// Deallocate N number of strings
void Code_Interpreter::hPOPS() 
{
  /** jnt ***
    TMEM m(Fetch());

    while (m.i > 0) {
    if (m_context->m_stringidx == m_context->m_minstridx) {
    CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR: string stack underflow");
    hHALT();
    return;
    }
    m_context->m_stringidx--;
    m_context->m_sstack--;
    if (!m_context->CheckStringIndex(m_apptask, m_context->m_stringidx)) {
    CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR: [hPOPS] string index is out of range");
    hHALT();
    return;
    } else {
    m_context->m_StatAllString -= m_context->GetStringTab(m_context->m_stringidx).size();
    m_context->GetStringTab(m_context->m_stringidx).erase();
    m.i--;
    }
    }
   **/
  TMEM m(Fetch());

  if (m_context->m_stringidx-m.i < m_context->m_minstridx) {
    CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR: string stack underflow");
    hHALT();
    return;
  }

  // check and clean all popped strings
  for (int i=0; i<m.i; i++) {
    if (!m_context->CheckStringIndex(m_apptask, m_context->m_stringidx-i)) {
      CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR: [hPOPS] string index is out of range");
      hHALT();
      return;
    }
    m_context->GetStringTab(m_context->m_stringidx-i).erase();
  }

  m_context->m_stringidx -= m.i;
  m_context->m_sstack -= m.i;
}

//-----------------------------------------------------------------------------
// RPUSH?
void Code_Interpreter::hRPUSH() 
{
  TMEM m1(Fetch());
  TMEM m2(Fetch());
  int data_index = m_context->m_BP + m1.i;
  if (!m_context->CheckDataIndex(m_apptask, data_index)) {
    CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR [hRPUSH]: array out of bound");
    hHALT();
    return;
  }

  Push(m_context->m_data[data_index].i + m2.i);
}

//-----------------------------------------------------------------------------
// duplicate what's on the stack
void Code_Interpreter::hDUP() 
{
  /*** jnt ***
    TMEM m(Pop());
    Push(m);
    Push(m);
   ***/
  register Task_Exec_Context *p_context = m_context;
  if ( p_context->m_SP > p_context->m_SS.min) {
    --p_context->m_SP;
    p_context->m_data[p_context->m_SP] = p_context->m_data[p_context->m_SP+1];
    UpdateLocalMaxStat();
  } else {
    CLog::Error( INTERPRETER, m_apptask, "STACK OVERFLOW");
    hHALT();
    return;
  }
}

//-----------------------------------------------------------------------------
//** duplicate a string located on the top of the stack and push the duplicate on the stack
void Code_Interpreter::hDUPS() 
{
  /*** jnt ***
    TMEM m(Pop());

    if (m_context->m_stringidx == m_context->GetStringTabSize()) {
    CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR: string stack overflow");
    hHALT();
    return;
    }
    Push(m);
    if (!m_context->CheckStringIndex(m_apptask, m_context->m_stringidx)) {
    CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR: [hDUPS] string index is out of range");
    hHALT();
    return;
    }

    if (!m_context->CheckStringIndex(m_apptask, m.i)) {
    CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR: [hDUPS] string index is out of range");
    hHALT();
    return;
    }
    m_context->GetStringTab(m_context->m_stringidx) = m_context->GetStringTab(m.i);
    m_context->m_StatAllString += m_context->GetStringTab(m_context->m_stringidx).size();
    Push(m_context->m_stringidx++);
    m_context->m_sstack++;
    if ((int)m_context->GetApplication()->GetStatMaxSStack() < m_context->m_sstack)
    m_context->GetApplication()->SetStatMaxSStack(m_context->m_sstack);
    if (m_context->m_Application->GetStatMaxAllString() < m_context->m_StatAllString)
    m_context->m_Application->SetStatMaxAllString(m_context->m_StatAllString);
   ***/

  if (m_context->m_stringidx == m_context->GetStringTabSize()) {
    CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR: string stack overflow");
    hHALT();
    return;
  }

  register Task_Exec_Context *p_context = m_context;
  p_context->GetStringTab(m_context->m_stringidx) = p_context->GetStringTab(p_context->m_data[p_context->m_SP].i);

  Push(m_context->m_stringidx++);

  m_context->m_sstack++;
  if (m_context->GetApplication()->GetStatMaxSStack() < m_context->m_sstack)
    m_context->GetApplication()->SetStatMaxSStack(m_context->m_sstack);
}

void Code_Interpreter::hMCPY() {
  TMEM len(Fetch());
  TMEM src(Pop());
  TMEM des(Pop());
  int i;

  if (!m_context->CheckDataIndex(m_apptask, src.i)||
      !m_context->CheckDataIndex(m_apptask, src.i+len.i)||
      !m_context->CheckDataIndex(m_apptask, des.i)||
      !m_context->CheckDataIndex(m_apptask, des.i+len.i)) {
    CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR [hMCPY]: array out of bound");
    hHALT();
    return;
  }
  for (i=0;i<len.i;i++) {
    m_context->m_data[des.i+i]=m_context->m_data[src.i+i];
  }
}

void Code_Interpreter::hMCPYS() {
  TMEM len(Fetch());
  TMEM src(Pop());
  TMEM des(Pop());
  int i;
  if (src.i!=des.i) {
    for (i=0;i<len.i;i++) {
      if (!m_context->CheckStringIndex(m_apptask, src.i+i)) {
        CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR: [hMCPYS] string index is out of range");
        hHALT();
        return;
      }
      if (!m_context->CheckStringIndex(m_apptask, des.i+i)) {
        m_context->m_StatAllString -= m_context->GetStringTab(des.i+i).size();
#ifdef STRINGTAB_CHECK
        m_context->Check_StringTab("hMCPYS-Got_Size(3)", __FILE__, __LINE__, false);
#endif
        m_context->GetStringTab(des.i+i).erase();
#ifdef STRINGTAB_CHECK
        m_context->Check_StringTab("hMCPYS-Erased(2)", __FILE__, __LINE__, true);
#endif
      }
      m_context->GetStringTab(des.i+i) = m_context->GetStringTab(src.i+i);
#ifdef STRINGTAB_CHECK
      m_context->Check_StringTab("hMCPYS-Copied(1)", __FILE__, __LINE__, true);
#endif
      if (m_context->m_Application->GetStatMaxAllString() < m_context->m_StatAllString) {
        m_context->m_Application->SetStatMaxAllString(m_context->m_StatAllString);
      }
    }
  }
}

void Code_Interpreter::hMCPYSR() {
  TMEM len(Fetch());
  TMEM des(Pop());
  TMEM src(Pop());
  int i;
  if (src.i!=des.i) {
    for (i=0;i<len.i;i++) {
      if (!m_context->CheckStringIndex(m_apptask, src.i+i)) {
        CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR: [hMCPYSR] string index is out of range");
        hHALT();
        return;
      }
      if (!m_context->CheckStringIndex(m_apptask, des.i+i)) {
        m_context->m_StatAllString -= m_context->GetStringTab(des.i+i).size();
#ifdef STRINGTAB_CHECK
        m_context->Check_StringTab("hMCPYSR-Got_Size(3)", __FILE__, __LINE__, false);
#endif
        m_context->GetStringTab(des.i+i).erase();
#ifdef STRINGTAB_CHECK
        m_context->Check_StringTab("hMCPYSR-Erased(2)", __FILE__, __LINE__, true);
#endif
      }
      m_context->GetStringTab(des.i+i) = m_context->GetStringTab(src.i+i);
#ifdef STRINGTAB_CHECK
      m_context->Check_StringTab("hMCPYSR-Copied(1)", __FILE__, __LINE__, true);
#endif
      if (m_context->m_Application->GetStatMaxAllString() < m_context->m_StatAllString) {
        m_context->m_Application->SetStatMaxAllString(m_context->m_StatAllString);
      }
    }
  }
}

void Code_Interpreter::hPUTSR() {
  TMEM m1(Fetch());
  TMEM m2(Fetch());
  if (m_context->m_SP-m2.i<m_context->m_SS.min) {
    CLog::Error( INTERPRETER, m_apptask, "STACK OVERFLOW");
    hHALT();
    return;
  }
  int data_index;
  while (m2.i > 0) {	
    if (m_context->m_stringidx == m_context->GetStringTabSize()) {
      CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR [hPUTSR]: string stack overflow");
      hHALT();
      return;
    }
    data_index = m_context->m_BP + m1.i;
    if (!m_context->CheckDataIndex(m_apptask, data_index)) {
      CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR [hPUTSR]: array out of bound");
      hHALT();
      return;
    }

    m_context->m_data[data_index].i = m_context->m_stringidx++;
    m_context->m_sstack++;
    m1.i++;
    m2.i--;
  }
  if ((int)m_context->GetApplication()->GetStatMaxSStack() < m_context->m_sstack)
    m_context->GetApplication()->SetStatMaxSStack(m_context->m_sstack);
}

//-----------------------------------------------------------------------------
// Array operation?
void Code_Interpreter::hAPUSH() 
{
  TMEM base(Fetch());
  TMEM index(Pop());

  if (index.i >= 0 && base.i+index.i<m_context->m_SS.max) Push(base.i+index.i);
  else {
    CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR: bad array index %d in hAPUSH",
        index.i);
    hHALT();
    return;
  }
}

//-----------------------------------------------------------------------------
// Array operation?
void Code_Interpreter::hAPLOC() 
{

  TMEM addr(Fetch());
  TMEM idx(Pop());

  int data_index = m_context->m_BP+addr.i+idx.i;
  if (idx.i >= 0 && data_index < m_context->m_SS.max) {
    PushUnchecked(data_index);
  } else {
    CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR: bad array index %d in hAPLOC",
        idx.i);
    hHALT();
    return;
  }
}

//-----------------------------------------------------------------------------
// store value to the location at the specified address
void Code_Interpreter::hSTOR() 
{
  TMEM val(Pop());
  TMEM addr(Pop());

  if (!m_context->CheckDataIndex(m_apptask, addr.i)) {
    CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR [hSTOR]: array out of bound");
    hHALT();
    return;
  }

  m_context->m_data[addr.i]=val;
}

//-----------------------------------------------------------------------------
// initially the top of the stack contains an address, replace the top of the stack with the value located at this address
void Code_Interpreter::hLOAD() 
{
  /*** jnt ***
    TMEM addr(Pop());
    I_TRACE( (INTERPRETER, m_apptask, "hLOAD %d", addr.i) );

    if (!m_context->CheckDataIndex(m_apptask, addr.i)) {
    CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR [hLOAD]: array out of bound");
    hHALT();
    return;
    }

    Push(m_context->m_data[addr.i]);
   ***/
  register Task_Exec_Context *p_context = m_context;
  int addr = p_context->m_data[p_context->m_SP].i;
  if (!m_context->CheckDataIndex(m_apptask, addr)) {
    CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR [hLOAD]: array out of bound");
    hHALT();
    return;
  }
  p_context->m_data[p_context->m_SP] = p_context->m_data[addr];
}

//-----------------------------------------------------------------------------
// copy string src to string dst
void Code_Interpreter::hSTOS() 
{
  /*** jnt ***
    TMEM src(Pop());
    TMEM dst(Pop());

    if (src.i >= (int)m_context->GetStringTabSize()) {
    CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR: [hSTOS] string index is out of range (idx:%d, max allowed:%d)", 
    src.i, m_context->GetStringTabSize());
    hHALT();
    return;
    }

    if (!m_context->CheckStringIndex(m_apptask, src.i)) {
    CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR: [hSTOS] string index is out of range");
    hHALT();
    return;
    }

    int slen = m_context->GetStringTab(src.i).size();
    if (slen >= MAXAPPSTRLEN) {
    CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR: [hSTOS] string too long");
    hHALT();
    return;
    }
    else {
    if (!m_context->CheckStringIndex(m_apptask, dst.i)) {
    CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR: [hSTOS] string index is out of range (idx:%d)", 
    dst.i);
    hHALT();
    return;
    }

    m_context->m_StatAllString -= m_context->GetStringTab(dst.i).size();
    m_context->GetStringTab(dst.i) = m_context->GetStringTab(src.i);
    m_context->m_StatAllString += slen;
    if (m_context->m_Application->GetStatMaxString() < (size_t)slen)
    m_context->m_Application->SetStatMaxString(slen);
    if (m_context->m_Application->GetStatMaxAllString() < m_context->m_StatAllString)
    m_context->m_Application->SetStatMaxAllString(m_context->m_StatAllString);
    }
   ***/

  TMEM src(Pop());
  TMEM dst(Pop());

  App_String srcstr = m_context->GetStringTab(src.i);
  int slen = srcstr.size();
  if (slen >= MAXAPPSTRLEN) {
    CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR: [hSTOS] string too long");
    hHALT();
    return;
  }

  m_context->GetStringTab(dst.i) = srcstr;
  if (m_context->m_Application->GetStatMaxString() < slen)
    m_context->m_Application->SetStatMaxString(slen);
}

//-----------------------------------------------------------------------------
// negate an integer on top of the stack
void Code_Interpreter::hNEGI() 
{
  /*** jnt ***
    TMEM m(Pop());
    Push(-m.i);
   ***/
  register Task_Exec_Context *p_context = m_context;
  p_context->m_data[p_context->m_SP].i = -p_context->m_data[p_context->m_SP].i;
}

//-----------------------------------------------------------------------------
// negate a real on top of the stack
void Code_Interpreter::hNEGR() 
{
  /*** jnt ***
    TMEM m(Pop());
    Push(-m.r);
   ***/
  register Task_Exec_Context *p_context = m_context;
  p_context->m_data[p_context->m_SP].r = -p_context->m_data[p_context->m_SP].r;
}

//-----------------------------------------------------------------------------
// multiply two integers on the stack
void Code_Interpreter::hMULI() 
{
  TMEM op2(Pop());
  TMEM op1(Pop());
  PushUnchecked(op1.i*op2.i);
}

//-----------------------------------------------------------------------------
// multiply two reals on the stack
void Code_Interpreter::hMULR() 
{

  TMEM op2(Pop());
  TMEM op1(Pop());

  PushUnchecked(op1.r*op2.r);
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hDIVI() 
{
  TMEM op2(Pop());
  TMEM op1(Pop());

  if (op2.i==0) {
    CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR: division by zero");
    hHALT();
    return;
  } else PushUnchecked(op1.i/op2.i);
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hDIVR() 
{
  TMEM op2(Pop());
  TMEM op1(Pop());

  if (op2.r==0.0) {
    CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR: division by zero");
    hHALT();
    return;
  }
  else
    PushUnchecked(op1.r/op2.r);
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hADDI() 
{

  TMEM op2(Pop());
  TMEM op1(Pop());

  PushUnchecked(op1.i+op2.i);
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hADDR() 
{

  TMEM op2(Pop());
  TMEM op1(Pop());

  PushUnchecked(op1.r+op2.r);
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hSUBI() 
{

  TMEM op2(Pop());
  TMEM op1(Pop());

  PushUnchecked(op1.i-op2.i);
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hSUBR() 
{

  TMEM op2(Pop());
  TMEM op1(Pop());

  PushUnchecked(op1.r-op2.r);
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hMOD() 
{

  TMEM op2(Pop());
  TMEM op1(Pop());

  if (op2.i==0) {
    CLog::Warning(INTERPRETER, m_apptask, "Operator MOD is applied with zero");
    PushUnchecked(op1.i);
  } else {
    PushUnchecked(op1.i%op2.i);
  }
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hINC() 
{
  /*** jnt ***
    TMEM m(Pop());
    PushUnchecked(++m.i);
   ***/

  register Task_Exec_Context *p_context = m_context;
  p_context->m_data[p_context->m_SP].i++;
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hDEC() 
{
  /*** jnt ***
    TMEM m(Pop());
    PushUnchecked(--m.i);
   ***/

  register Task_Exec_Context *p_context = m_context;
  p_context->m_data[p_context->m_SP].i--;
}

//-----------------------------------------------------------------------------
// add two strings 
void Code_Interpreter::hADDS() 
{
  /*** jnt ***
    TMEM dst(Pop());
    TMEM s2(Pop());
    TMEM s1(Pop());

    if (!m_context->CheckStringIndex(m_apptask, s1.i) ||
    !m_context->CheckStringIndex(m_apptask, s2.i) ||
    !m_context->CheckStringIndex(m_apptask, dst.i))
    {
    CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR: [hADDS] string index is out of range");
    hHALT();
    return;
    }

    if (s1.i >= (int)m_context->GetStringTabSize()) {
    CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR: [hADDS] string index is out of range (idx:%d, max allowed:%d)", 
    s1.i, m_context->GetStringTabSize());
    hHALT();
    return;
    }
    if (s2.i >= (int)m_context->GetStringTabSize()) {
    CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR: [hADDS] string index is out of range (idx:%d, max allowed:%d)", 
    s2.i, m_context->GetStringTabSize());
    hHALT();
    return;
    }

    int tot_len = m_context->GetStringTab(s1.i).size() + m_context->GetStringTab(s2.i).size();
    if (tot_len >= MAXAPPSTRLEN) {
    CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR: [hADDS] string too long");
    hHALT();
    return;
    }
    else {
    m_context->m_StatAllString -= m_context->GetStringTab(dst.i).size();

    if (m_context->GetStringTab(dst.i).capacity() < tot_len)
    m_context->GetStringTab(dst.i).reserve(tot_len);

    m_context->GetStringTab(dst.i) = m_context->GetStringTab(s1.i);
    m_context->GetStringTab(dst.i) += m_context->GetStringTab(s2.i);

    Push(dst);

    m_context->m_StatAllString += tot_len;
    if (m_context->m_Application->GetStatMaxString() < (size_t)tot_len)
    m_context->m_Application->SetStatMaxString(tot_len);
    if (m_context->m_Application->GetStatMaxAllString() < m_context->m_StatAllString)
    m_context->m_Application->SetStatMaxAllString(m_context->m_StatAllString);

    }
   ***/
  TMEM dst(Pop());
  TMEM s2(Pop());
  TMEM s1(Pop());

  App_String str1 = m_context->GetStringTab(s1.i);
  App_String str2 = m_context->GetStringTab(s2.i);
  int tot_len = str1.size() + str2.size();
  if (tot_len >= MAXAPPSTRLEN) {
    CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR: [hADDS] string too long");
    hHALT();
    return;
  }

  m_context->GetStringTab(dst.i) = str1 + str2;

  PushUnchecked(dst);

  if (m_context->m_Application->GetStatMaxString() < tot_len)
    m_context->m_Application->SetStatMaxString(tot_len);
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hEQI() 
{
  TMEM e2(Pop());
  TMEM e1(Pop());

  I_TRACE( (INTERPRETER, m_apptask, "hEQI %d %d", e1.i, e2.i) );
  PushUnchecked(e1.i==e2.i);
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hNEQI() 
{

  TMEM e2(Pop());
  TMEM e1(Pop());

  PushUnchecked(e1.i!=e2.i);
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hLTI() 
{

  TMEM e2(Pop());
  TMEM e1(Pop());

  PushUnchecked(e1.i<e2.i);
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hLTEI() 
{
  TMEM e2(Pop());
  TMEM e1(Pop());
  PushUnchecked(e1.i<=e2.i);
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hGTI() 
{

  TMEM e2(Pop());
  TMEM e1(Pop());

  PushUnchecked(e1.i>e2.i);
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hGTEI() 
{
  TMEM e2(Pop());
  TMEM e1(Pop());

  PushUnchecked(e1.i>=e2.i);
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hEQR() 
{
  TMEM e2(Pop());
  TMEM e1(Pop());

  PushUnchecked(e1.r==e2.r);
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hNEQR() 
{
  TMEM e2(Pop());
  TMEM e1(Pop());

  PushUnchecked(e1.r!=e2.r);
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hLTR() 
{

  TMEM e2(Pop());
  TMEM e1(Pop());

  PushUnchecked(e1.r<e2.r);
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hLTER() 
{

  TMEM e2(Pop());
  TMEM e1(Pop());

  PushUnchecked(e1.r<=e2.r);
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hGTR() 
{

  TMEM e2(Pop());
  TMEM e1(Pop());

  PushUnchecked(e1.r>e2.r);
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hGTER() 
{

  TMEM e2(Pop());
  TMEM e1(Pop());

  PushUnchecked(e1.r>=e2.r);
}

//-----------------------------------------------------------------------------
// check if two strings are equal
void Code_Interpreter::hEQS() 
{
  int  r;

  TMEM s2(Pop());
  TMEM s1(Pop());
  if (!m_context->CheckStringIndex(m_apptask, s1.i) ||
      !m_context->CheckStringIndex(m_apptask, s2.i))
  {
    CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR: [hEQS] string index is out of range");
    hHALT();
    return;
  }

  r = (stricmp(m_context->GetStringTab(s1.i).c_str(),m_context->GetStringTab(s2.i).c_str()) == 0);
  PushUnchecked(r);
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hNEQS() 
{
  int  r;

  TMEM s2(Pop());
  TMEM s1(Pop());
  if (!m_context->CheckStringIndex(m_apptask, s1.i) ||
      !m_context->CheckStringIndex(m_apptask, s2.i))
  {
    CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR: [hNEQS] string index is out of range");
    hHALT();
    return;
  }

  r = (stricmp(m_context->GetStringTab(s1.i).c_str(),m_context->GetStringTab(s2.i).c_str()) != 0);
  PushUnchecked(r);
}

void Code_Interpreter::hLTS() 
{
  int  r;

  TMEM s2(Pop());
  TMEM s1(Pop());
  if (!m_context->CheckStringIndex(m_apptask, s1.i) ||
      !m_context->CheckStringIndex(m_apptask, s2.i))
  {
    CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR: [hLTS] string index is out of range");
    hHALT();
    return;
  }

  r = (stricmp(m_context->GetStringTab(s1.i).c_str(),m_context->GetStringTab(s2.i).c_str()) < 0);
  PushUnchecked(r);
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hLTES() 
{
  int  r;

  TMEM s2(Pop());
  TMEM s1(Pop());
  if (!m_context->CheckStringIndex(m_apptask, s1.i) ||
      !m_context->CheckStringIndex(m_apptask, s2.i))
  {
    CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR: [hLTES] string index is out of range");
    hHALT();
    return;
  }

  r = (stricmp(m_context->GetStringTab(s1.i).c_str(),m_context->GetStringTab(s2.i).c_str()) <= 0);
  PushUnchecked(r);
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hGTS() 
{
  int  r;

  TMEM s2(Pop());
  TMEM s1(Pop());
  if (!m_context->CheckStringIndex(m_apptask, s1.i) ||
      !m_context->CheckStringIndex(m_apptask, s2.i))
  {
    CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR: [hGTS] string index is out of range");
    hHALT();
    return;
  }

  r = (stricmp(m_context->GetStringTab(s1.i).c_str(),m_context->GetStringTab(s2.i).c_str()) > 0);
  PushUnchecked(r);
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hGTES() 
{
  int  r;

  TMEM s2(Pop());
  TMEM s1(Pop());
  if (!m_context->CheckStringIndex(m_apptask, s1.i) ||
      !m_context->CheckStringIndex(m_apptask, s2.i))
  {
    CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR: [hGTES] string index is out of range");
    hHALT();
    return;
  }

  r = (stricmp(m_context->GetStringTab(s1.i).c_str(),m_context->GetStringTab(s2.i).c_str()) >= 0);
  PushUnchecked(r);
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hJMP() 
{
  TMEM addr(Fetch());
  m_context->m_IP = addr.i;
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hJNZ() 
{
  TMEM addr(Fetch());
  TMEM cond(Pop());

  if (cond.i!=0) m_context->m_IP = addr.i;
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hJZ() 
{
  TMEM addr(Fetch());
  TMEM cond(Pop());

  if (cond.i==0) m_context->m_IP = addr.i;
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hAND() 
{

  TMEM op2(Pop());
  TMEM op1(Pop());

  PushUnchecked(op1.i && op2.i);
}

void Code_Interpreter::hOR() 
{

  TMEM op2(Pop());
  TMEM op1(Pop());

  PushUnchecked(op1.i || op2.i);
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hNOT() 
{
  /*** jnt ***
    TMEM op(Pop());
    PushUnchecked(!op.i);
   **/
  register Task_Exec_Context *p_context = m_context;
  p_context->m_data[p_context->m_SP].i = !p_context->m_data[p_context->m_SP].i;
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hCALL() 
{
  TMEM addr(Fetch());

  Push(m_context->m_IP);
  m_context->m_IP = addr.i;
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hCAPI() 
{
  CAPIDef *api;

  TMEM idapi(Fetch());

  api = API_Provider_Manager::Instance()->FindAPI(idapi.i);
  if (!api) {
    CLog::Error( INTERPRETER, m_apptask, "API with UID 0x%X (%d) not found", idapi.i, idapi.i);
    m_apptask->SetLastError(-1);
    return;
  }

  if (api->m_Async) {

    if (m_apptask->HasException() && m_apptask->CanProcessException()) {
      CatchException();
      return;
    }
  }

  Push(m_context->m_IP);
  hPUSHBP();
  hMVSPBP();

  char location[256];
  sprintf(location, "Interpreter::API[%d,%d]", idapi.i >> 16, idapi.i & 0xffff);
  atomic_set (&Kernel::Instance()->currentService, location);

  STATUS status;
  status = API_Provider_Manager::Instance()->ExecuteAPI( api, m_apptask, (int*)m_context->m_data, m_context->m_SP);
  if (status != OK && m_apptask) {
    hHALT();
    return;
  }

  hMVBPSP();
  hPOPBP();

  TMEM retaddr(Pop());
  m_context->m_IP = retaddr.i;
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hRET() 
{
  TMEM retaddr(Pop());

  m_context->m_IP = retaddr.i;
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hPUSHBP() 
{
  Push(m_context->m_BP);
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hPOPBP() 
{
  TMEM m(Pop());
  m_context->m_BP = m.i;
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hMVSPBP() 
{
  m_context->m_BP = m_context->m_SP;
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hMVBPSP() 
{
  m_context->m_SP = m_context->m_BP;
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hADDSP() 
{
  /*** jnt ***
    TMEM m(Fetch());

    if (m_context->m_SP+m.i>m_context->m_SS.max) {
    CLog::Error( INTERPRETER, m_apptask, "STACK UNDERFLOW");
    hHALT();
    return;
    } else {
    m_context->m_SP += m.i;
    }
   ***/
  TMEM m(Fetch());
  m_context->m_SP += m.i;
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hSUBSP() 
{
  TMEM m(Fetch());

  if (m_context->m_SP-m.i<m_context->m_SS.min) {
    CLog::Error( INTERPRETER, m_apptask, "STACK OVERFLOW");
    hHALT();
    return;
  } else {
    m_context->m_SP -= m.i;
  }
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hCLE() 
{
  if (m_apptask->HasException() && m_apptask->CanProcessException()) {
    CatchException();
  } else {
    m_apptask->DisableException();
  }
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hSTE() 
{
  m_apptask->EnableException();

  if (m_apptask->HasException() && m_apptask->CanProcessException()) {
    CatchException();
  }
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hNOP() 
{
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hSAVEHD() 
{

  TMEM handler(Fetch());

  Push(m_context->m_BP);
  Push(m_context->m_stringidx);
  Push(m_context->m_sstack);
  Push(handler);
  Push(m_context->m_EP);

  m_context->m_EP = m_context->m_SP;
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hRESTHD() 
{

  m_context->m_SP = m_context->m_EP;

  TMEM ep(Pop());
  TMEM handler(Pop());
  TMEM sstack(Pop());
  TMEM stringidx(Pop());
  TMEM bp(Pop());

  for (int i=stringidx.i; i<m_context->m_stringidx; i++) {
    if (!m_context->CheckStringIndex(m_apptask, i)) {
      CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR: [hRESTHD] string index is out of range");
      hHALT();
      return;
    }
    App_String str = m_context->GetStringTab(i);
    m_context->m_StatAllString -= str.size();
    str.erase();

  }
  m_context->m_stringidx = stringidx.i;
  m_context->m_sstack = sstack.i;
  m_context->m_BP = bp.i;
  m_context->m_EP = ep.i;
}

//-----------------------------------------------------------------------------
void Code_Interpreter::hHALT()
{
  CLog::Debug( INTERPRETER, m_apptask, "Task halting");
  m_apptask->Quit();
}

// ==============================================================================
// end of p-CODE implementation
// =============================================================================+


//-----------------------------------------------------------------------------
void Code_Interpreter::CatchException() 
{


  CLog::Debug( INTERPRETER, m_apptask, "Processing exception");

  m_context->m_SP = m_context->m_EP;
  if (m_context->m_EP==0) {
    CLog::Debug( INTERPRETER, m_apptask, "Task doesn't have exception handler. It will quit");
    hHALT();
    return;
  } else {

    TMEM ep(Pop());
    TMEM handler(Pop());
    TMEM sstack(Pop());
    TMEM stringidx(Pop());
    TMEM bp(Pop());

    m_apptask->DisableException();

    for (int i=stringidx.i; i<m_context->m_stringidx; i++) {
      if (!m_context->CheckStringIndex(m_apptask, i)) {
        CLog::Error( INTERPRETER, m_apptask, "RUNTIME ERROR: [hRESTHD] string index is out of range");
        hHALT();
        return;
      }
      m_context->m_StatAllString -= m_context->GetStringTab(i).size();
      m_context->GetStringTab(i).erase();
    }
    m_context->m_stringidx = stringidx.i;
    m_context->m_sstack = sstack.i;
    m_context->m_BP = bp.i;
    m_context->m_IP = handler.i+1; // skip the CLE bytecode
    m_context->m_EP = ep.i;

    m_apptask->PopException();
  }

}


//-----------------------------------------------------------------------------
/** Show execution time.
*/
void Code_Interpreter::ShowProfile ( Term_Session * session )
{
  ACQUIRE_SYSTEM_LOCK();
  Time_Value total = interpreterTime;
  double msec_kernel_time = Kernel::Instance()->GetKernelBusyTime().to_msec();
  RELEASE_SYSTEM_LOCK();

  if (msec_kernel_time == 0)
    msec_kernel_time = 1;
  session->Sendf ("Total interpreter execution time: %u sec:%06u usec (%5.2f %% system)" NEWLINE, 
      total.sec(),
      total.usec(),
      total.to_msec() * 100 / msec_kernel_time);
}

