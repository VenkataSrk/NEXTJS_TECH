/*----------------------------------------------------------------* 
 * $Archive: /xGate/vocab.h $
 * $Date: 21/04/01 23:08 $
 * $Revision: 1 $
 *
 * $History: vocab.h $
 * 
 * *****************  Version 1  *****************
 * User: Junanto      Date: 21/04/01   Time: 23:08
 * Created in $/xGate
 * Integrated
 * 
 *----------------------------------------------------------------*/

/*----------------------------------------------------------------* 
 * Description:
 * This module converts a number into a list of number components
 * in order for the number to be read out
 *----------------------------------------------------------------*/

#if !defined(_VOCAB_H__INCLUDED_)
#define _VOCAB_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/** Vocabulary
 */
class Vocabulary  
{
protected:
    //blp:
    //CStringArray m_elts;
    Vector <String> m_elts;

    virtual void BuildNumberLT100(unsigned number);
    virtual void BuildNumberLT1K(unsigned number);
    virtual void BuildNumberLT10K(unsigned number);
    virtual void BuildNumberLT1M(unsigned number);
    virtual void BuildNumberLT1B(unsigned number);
    virtual void BuildNumber(unsigned number);

    void AddToList(const char *eltname) {
	//blp:
	//if (eltname) m_elts.Add(eltname);
	if (eltname) m_elts.push_back(eltname);
    }

public:
    Vocabulary(unsigned number) { BuildNumber(number); } 
    ~Vocabulary();

    unsigned GetNumOfElts() { /*blp: return m_elts.GetUpperBound()+1;*/
			      return m_elts.size(); }
    const char *GetElement(unsigned index) { return m_elts[index].c_str(); } 
};

#endif // !defined(_VOCAB_H__INCLUDED_)
