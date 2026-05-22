#include "xmltester.h"
using namespace std;
xmltester::xmltester(){
}
xmltester::~xmltester(){
}
void xmltester::starttest(const char* pl){
xGateXMLParser xgxml;
xgxml.LoadParser(pl);
xgxml.parseXMLInfo();
//xgxml.decodeConferenceInfo();
//xgxml.decodeHostInfo();
//xgxml.decodeUserInfo();

}
void xmltester::testencoder(){
xGateXMLParser en;
CONF_INFO conf;
HOST_INFO host;
std::string userscount ;
CALL_INFO *call;
USER_INFO user;
conf.confid = "a3456f34-108e-4e2b-a56d-2dda1e673f2b";
conf.confState = "start";

host.entity = "590@office.vectone.com";
host.confDisplayName = "Office Meeting";

userscount =  "1";

user.entity = "590@office.vectone.com";
user.type = "host";
user.index = "0";
user.callCount = "2";

call = new CALL_INFO();
call->index = "0";
call->callStatus = "joining";
call->callId = "3c0b82d4-5a9c-4ee2-b204-9b429dc6242d";
call->confDisplayText = "xyz";
call->toHeader.sipuri = "380@office.vectone.com";
call->toHeader.tagid = "50bfgc234-098gfh";
call->fromHeader.sipuri = "590@office.vectone.com";
call->fromHeader.tagid = "25qwer323-890hjg";
user.callInfo.push_back(call);
call = new CALL_INFO();
call->index = "1";
call->callStatus = "joining";
call->callId = "2b1a7133-4b10e-3ee2-b204-9b429d334edrd";
call->confDisplayText = "abc";
call->toHeader.sipuri = "480@office.vectone.com";
call->toHeader.tagid = "2we4evi0p-789erf768";
call->fromHeader.sipuri = "590@office.vectone.com";
call->fromHeader.tagid = "34qrew323-000oiu";
user.callInfo.push_back(call);

en.setConferenceInfo(conf);
en.setHostInfo(host);
en.setUsersInfo(userscount);
en.setUserInfo(user);
en.serializeXMLInfo();
cout << "XML String--- " << en.getxmlAsString();
}
int main(void){
cout << "XML Parser Testing sample" <<endl;
const char* payload = "<?xml version=\"1.0\" encoding=\"utf-8\"?><conference-info confid=\"b7956f34-108e-4e2b-a56d-2dda1e673f2b\" state=\"start\"><host-info entity=\"100@node1.vectone.com\" confdisplayname=\"morning meeting\"></host-info><users count=\"1\"><user entity=\"100@node1.vectone.com\" type=\"host\" index=\"0\" callcount=\"2\"><call-info index=\"0\"><call_status status=\"joining\" /><display-text value=\"\" /><call-id id=\"1c0b82d4-5a9c-4ee2-b204-9b429dc6242d\" /><to sipuri=\"102@node1.vectone.com\" tagid=\"9b429dc6242d\" /><from sipuri=\"100@node1.vectone.com\"  tagid=\"b2041c0b82d4-5a9c-4ee2\" /></call-info><call-info index=\"1\"><call_status status=\"joining\" /><display-text value=\"\" /><call-id id=\"3c0b82d4-5a9c-4ee2-b204-9b429dc6242d\" /><to sipuri=\"101@node1.vectone.com\" tagid=\"20b429dc6242d\" /><from sipuri=\"100@node1.vectone.com\" tagid=\"21041c0b82d4-5a9c-4ee2\" /></call-info></user></users></conference-info>";
std::string ps = payload;
xmltester t;
//for(int i =1 ; i < 100 ; i++)
// t.starttest(payload);
t.testencoder();
return 1;
}
