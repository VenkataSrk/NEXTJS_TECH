

//xgate/branches/Sofia/full/pwlib/src/ptclib/pdns.cxx
// This is related to pdns.cxx file function "DnsRecordListFree"


#include <ptlib.h>
#include <ptclib/pdns.h>
#include <ptclib/url.h>
#include <ptlib/ipsock.h>

void DnsRecordListFree(PDNS_RECORD rec, int /* FreeType */)
{
  while (rec != NULL) {
    PDNS_RECORD next = rec->pNext;
    free(rec);
    rec = next;
  }
}


