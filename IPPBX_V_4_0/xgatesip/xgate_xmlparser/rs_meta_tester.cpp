/*
 * ******************************************************************************
 * Vectone Mobile
 * @file rs_meta_tester.cpp
 *
 * @brief This is RS Metadata XML Parser/Encoder
 *
 * @author Narrain prithvi Dharuman
 * ******************************************************************************
 */
#include "rs_meta_tester.h"
using namespace std;

xmltester::xmltester(){
}

xmltester::~xmltester(){

}

/*
 * Tester XML class takes XML string to parse,
 * Parsed elements are stored in rsMData
 */
void xmltester::starttest(const char* pl){
    SipRec_RsMetadata      rsMData;

    xGateRSXMLParser xgxml;
    xgxml.loadParser(pl);
    xgxml.parseXMLInfo(rsMData);
    xgxml.printParsedXML(rsMData);

    xGateRSXMLParser toxml;
    toxml.encodeRSMetadata(rsMData);
}

int main(int argc, char *argv[]){
    cout << "XML Parser Testing sample" <<endl;
    //const char* payload = "<?xml version=\"1.0\" encoding=\"utf-8\"?><conference-info confid=\"b7956f34-108e-4e2b-a56d-2dda1e673f2b\" state=\"start\"><host-info entity=\"100@node1.vectone.com\" confdisplayname=\"morning meeting\"></host-info><users count=\"1\"><user entity=\"100@node1.vectone.com\" type=\"host\" index=\"0\" callcount=\"2\"><call-info index=\"0\"><call_status status=\"joining\" /><display-text value=\"\" /><call-id id=\"1c0b82d4-5a9c-4ee2-b204-9b429dc6242d\" /><to sipuri=\"102@node1.vectone.com\" tagid=\"9b429dc6242d\" /><from sipuri=\"100@node1.vectone.com\"  tagid=\"b2041c0b82d4-5a9c-4ee2\" /></call-info><call-info index=\"1\"><call_status status=\"joining\" /><display-text value=\"\" /><call-id id=\"3c0b82d4-5a9c-4ee2-b204-9b429dc6242d\" /><to sipuri=\"101@node1.vectone.com\" tagid=\"20b429dc6242d\" /><from sipuri=\"100@node1.vectone.com\" tagid=\"21041c0b82d4-5a9c-4ee2\" /></call-info></user></users></conference-info>";
    // const char* payload ="<?xml version=\"1.0\" encoding=\"UTF-8\"?> <recording xmlns=\'urn:ietf:params:xml:ns:recording:1\'>    <datamode>complete</datamode>    <group group_id=\"7+OTCyoxTmqmqyA/1weDAg==\">       <associate-time>2010-12-16T23:41:07Z</associate-time>    </group>   <session session_id=\"hVpd7YQgRW2nD22h7q60JQ==\">       <sipSessionID>ab30317f1a784dc48ff824d0d3715d86;          remote=47755a9de7794ba387653f2099600ef2</sipSessionID>            <urcalldata xmlns='http://unifiedrign.com/rec/recxml'>              <callgroupid>67755a9de7794ba387653f2099600ea3</callgroupid>           </urcalldata>   </session> <participant participant_id=\"srfBElmCRp2QB23b7Mpk0w==\">        <nameID aor=\"sip:bob@biloxi.com\">           <name xml:lang=\"it\">Bob</name>        </nameID>   </participant>   <participant participant_id=\"zSfPoSvdSDCmU3A3TRDxAw==\">        <nameID aor=\"sip:Paul@biloxi.com\">           <name xml:lang=\"it\">Paul</name>        </nameID>   </participant>   <stream stream_id=\"UAAMm5GRQKSCMVvLyl4rFw==\"          session_id=\"hVpd7YQgRW2nD22h7q60JQ==\">      <label>96</label>   </stream>   <stream stream_id=\"i1Pz3to5hGk8fuXl+PbwCw==\"           session_id=\"hVpd7YQgRW2nD22h7q60JQ==\">       <label>97</label>    </stream></recording>";

    if(argc != 2) {
        cerr << "Usage: " << argv[0]
                << " <xmlFile> " << std::endl;
        return -1;
    }

    std::ifstream file(argv[1]);
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    std::string payload(buffer.str());

    xmltester tester;
    tester.starttest(payload.c_str());

    return 0;
}
