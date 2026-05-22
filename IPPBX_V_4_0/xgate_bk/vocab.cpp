/*----------------------------------------------------------------* 
 * $Archive: /xGate/vocab.cpp $
 * $Date: 21/04/01 23:08 $
 * $Revision: 1 $
 *
 * $History: vocab.cpp $
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


#include "stdafx.h"

#include "vocab.h"


/*-------------------------------------------------------------------*
 *  Elements for vocabulary:
 *  - number:
 *	0,1,...,99
 *	100,200,...,900
 *	1000,2000,...,9000
 *	thousand
 *	million(s)
 *	billion(s)
 *  - date(*):
 *	1st,2nd,...31st
 *	January,February,...,December
 *  - money(*):
 *-------------------------------------------------------------------*/



/*-------------------------------------------------------------------*
 * Constructor/destructor
 *-------------------------------------------------------------------*/

//-------------------------------------------------------------------
Vocabulary::~Vocabulary()
{
}



/*-------------------------------------------------------------------*
 * Implementation
 *-------------------------------------------------------------------*/


//-------------------------------------------------------------------
void Vocabulary::BuildNumberLT100(unsigned number)
{
    static char* eltnames[] = {
	NULL,  "nb1", "nb2", "nb3", "nb4",
	"nb5", "nb6", "nb7", "nb8", "nb9",
	"nb10","nb11","nb12","nb13","nb14",
	"nb15","nb16","nb17","nb18","nb19",
	"nb20","nb21","nb22","nb23","nb24",
	"nb25","nb26","nb27","nb28","nb29",
	"nb30","nb31","nb32","nb33","nb34",
	"nb35","nb36","nb37","nb38","nb39",
	"nb40","nb41","nb42","nb43","nb44",
	"nb45","nb46","nb47","nb48","nb49",
	"nb50","nb51","nb52","nb53","nb54",
	"nb55","nb56","nb57","nb58","nb59",
	"nb60","nb61","nb62","nb63","nb64",
	"nb65","nb66","nb67","nb68","nb69",
	"nb70","nb71","nb72","nb73","nb74",
	"nb75","nb76","nb77","nb78","nb79",
	"nb80","nb81","nb82","nb83","nb84",
	"nb85","nb86","nb87","nb88","nb89",
	"nb90","nb91","nb92","nb93","nb94",
	"nb95","nb96","nb97","nb98","nb99",
    };

    if (number > 99) return;
    AddToList(eltnames[number]);
}


//-------------------------------------------------------------------
void Vocabulary::BuildNumberLT1K(unsigned number)
{
    static char* eltnames[] = {
	NULL,   "nb100","nb200","nb300","nb400",
	"nb500","nb600","nb700","nb800","nb900"
    };

    if (number > 999) return;
    AddToList(eltnames[number/100]);
    if ((number > 99) && (number%100 != 0)) AddToList("and");
    BuildNumberLT100(number%100);
}


//-------------------------------------------------------------------
void Vocabulary::BuildNumberLT10K(unsigned number)
{
    static char* eltnames[] = {
	NULL,    "nb1000","nb2000","nb3000","nb4000",
	"nb5000","nb6000","nb7000","nb8000","nb9000"
    };

    if (number > 9999) return;
    AddToList(eltnames[number/1000]);
    BuildNumberLT1K(number%1000);
}


//-------------------------------------------------------------------
void Vocabulary::BuildNumberLT1M(unsigned number)
{
    if (number > 999999) return;
    if (number < 10000) {
	BuildNumberLT10K(number);
    } else {
	BuildNumberLT1K(number/1000);
	AddToList("thousand");
	BuildNumberLT1K(number%1000);
    }
}


//-------------------------------------------------------------------
void Vocabulary::BuildNumberLT1B(unsigned number)
{
    if (number > 999999999) return;
    if (number < 1000000) {
	BuildNumberLT1M(number);
    } else {
	BuildNumberLT1K(number/1000000);
	if (number/1000000 == 1) AddToList("million");
	else AddToList("millions");
	BuildNumberLT1M(number%1000000);
    }
}


//-------------------------------------------------------------------
void Vocabulary::BuildNumber(unsigned number)
{
    if (number == 0) {
	AddToList("nb0");
    } else if (number < 1000000000) {
	BuildNumberLT1B(number);
    } else {
	BuildNumberLT1K(number/1000000000);
	if (number/1000000 == 1) AddToList("billion");
	else AddToList("billions");
	BuildNumberLT1B(number%1000000000);
    }
}
