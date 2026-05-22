#include "xglog.h"
#include "URCBCallContextHandler.h"

bool URCBCallContextHandler::insertURCBCallContext(std::string &sID, URCBCallContext* ptCtx)
{
        IT_URCB_CALL_CONTEXT_MAP it = m_tCallContext.find(sID.c_str());
        if (it != m_tCallContext.end()){
                XGLOG_ERROR(" Inserting the URCB call context is failed. Context Id (%s) already existis in the map!",sID.c_str());
                return false;
        }
        m_tCallContext.insert(URCB_CALL_CONTEXT_MAP::value_type(sID.c_str(), ptCtx));
        XGLOG_DEBUG(" Inserting the URCB call context Id (%s) in the map!", sID.c_str());
        return true;
}
bool URCBCallContextHandler::DeleteURCBCallContext(std::string &sID)
{
        IT_URCB_CALL_CONTEXT_MAP it = m_tCallContext.find(sID.c_str());
        if (it == m_tCallContext.end()) {
                XGLOG_ERROR(" Deleting the URCB call context is failed. Context Id (%s) does NOT existis in the map!", sID.c_str());
                return false;
        }
        URCBCallContext* ptCtx = it->second;
        delete ptCtx;
        ptCtx = NULL;
        m_tCallContext.erase(it->first);

        XGLOG_DEBUG(" Deleteing the URCB call context Id (%s) from the map!", sID.c_str());
        return true;
}
bool URCBCallContextHandler::getURCBCallContext(std::string &sID, URCBCallContext *& ptCtx)
{
        IT_URCB_CALL_CONTEXT_MAP it = m_tCallContext.find(sID.c_str());
        if (it == m_tCallContext.end()) {
                XGLOG_ERROR(" retriving the URCB call context is failed. Context Id (%s) does NOT existis in the map!", sID.c_str());
                return false;
        }
        ptCtx = it->second;

        if (!ptCtx) {
                XGLOG_ERROR("Call Id (%s). URCB call context is NULL", sID.c_str());
                return false;
        }

        XGLOG_DEBUG("accessing the URCB call context Id (%s) from the map!", sID.c_str());
        return true;
}
    

