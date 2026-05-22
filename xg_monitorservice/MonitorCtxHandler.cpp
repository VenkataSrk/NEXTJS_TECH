#include "MonitorCtxHandler.h"

/*
 * Insert context data into map
 */
bool MonitorCtxHandler::insertMonitorCtx(std::string &ip, int &trunk_id, MonitorContext *mnctx)
{
    if (!mnctx)
    {
        XGLOG_ERROR("MonitorCtxHandler::insertMonitorCtx failed to insert data due to mnctx is nullptr!");
        return false;
    }
    std::lock_guard<std::mutex> lock(_ctx_mutex);
    auto it = m_mntrHctx.find(make_pair(ip, trunk_id));
    if (it != m_mntrHctx.end())
    {
        XGLOG_ERROR("Inserting Monitor Ctx failed ,context pair (%s,%d) already exists in the map!", ip.c_str(), trunk_id);
        return false;
    }
    m_mntrHctx.insert(make_pair(make_pair(ip, trunk_id), mnctx));
    XGLOG_INFO("Inserting the Monitor context pair(%s,%d) in the map success", ip.c_str(), trunk_id);

    return true;
}

/*
 * Delete context data from map
 */
bool MonitorCtxHandler::deleteMonitorCtx(std::string &ip, int &trunk_id)
{
    std::lock_guard<std::mutex> lock(_ctx_mutex);
    auto it = m_mntrHctx.find(make_pair(ip, trunk_id));
    if (it != m_mntrHctx.end())
    {
        MonitorContext *pMntrctr = it->second;
        delete pMntrctr;
        pMntrctr = nullptr;
        m_mntrHctx.erase(make_pair(ip, trunk_id));
        XGLOG_INFO("Deleting the Monitor context pair(%s,%d) in the map success", ip.c_str(), trunk_id);
        return true;
    }

    XGLOG_ERROR("Deleting Monitor Ctx failed ,context pair (%s,%d) not found in the map!", ip.c_str(), trunk_id);
    return false;
}

/*
 * Get context data as a reference from map 
 */
bool MonitorCtxHandler::getMonitorCtx(std::string &ip, int &trunk_id, MonitorContext *&mnctx)
{
    std::lock_guard<std::mutex> lock(_ctx_mutex);
    auto it = m_mntrHctx.find(make_pair(ip, trunk_id));
    if (it != m_mntrHctx.end())
    {
        XGLOG_INFO("Accessing Monitor context pair (%s,%d) in the map ", ip.c_str(), trunk_id);
        mnctx = it->second;
        if (!mnctx)
        {
            XGLOG_ERROR("Pair (%s,%d) .Monitor context is nullptr !",ip,trunk_id);
            return false;
        }
        return true;
    }

    XGLOG_ERROR("Retriving Monitor Ctx failed ,context pair (%s,%d) not found in map!", ip.c_str(), trunk_id);
    return false;
}