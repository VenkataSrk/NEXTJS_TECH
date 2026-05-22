/* $Header: /xGate/h248sig.h 
 *   
 * XGATE
 * (C) 2015 Mundio, Ltd
 * Initial coding.
 */
#ifndef __XGATE_H248RLS_H__
#define __XGATE_H248RLS_H__

/*
#ifdef BOOST_INSTALLED
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#endif
*/

/* Common Structure between MGC and MG */


struct RLS_RECORD
{
   public:
   char interfaceName[10];
   char groupName[10];
   char cardName[10];
   int sigType;
   int supportedCodec;
   int totalChanCount;
   RLS_RECORD();
   RLS_RECORD(const RLS_RECORD &rhs);

   friend ostream &operator<<( ostream &output, 
                                     const RLS_RECORD &R )
   { 
         output << R.interfaceName << "\t" << R.groupName << "\t" << R.cardName  << "\t" << R.totalChanCount
                << "\t" << R.sigType << "\t" << R.supportedCodec << endl;
         return output;            
   }

   RLS_RECORD &operator=(const RLS_RECORD &rhs)
   {
     printf("Assignment Operator\n");
     strcpy(interfaceName, rhs.interfaceName);
     strcpy(cardName , rhs.cardName);
     strcpy(groupName , rhs.groupName);
     sigType = rhs.sigType;
     supportedCodec=rhs.supportedCodec;
     totalChanCount=rhs.totalChanCount ;
     return *this;
   }

};



typedef vector<RLS_RECORD> GwRscVector;


#endif
