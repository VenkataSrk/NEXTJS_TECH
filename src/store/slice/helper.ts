export const calculateNotiUnreadCount = (notification: any) => {
    let count = 0;
    notification.forEach((ele :any) => {
        if(ele.is_read === 0){
            count++;
        }
    });
    return count;
};

export const filterInboxData = (inboxData :any, payload :any) => {
    return inboxData.filter((ele :any) => {
        if(payload?.email_history_ids && ele?.EmailHistoryId && `${payload?.email_history_ids}`?.includes(ele?.EmailHistoryId)){
            return false;
        }else if(payload?.sms_history_ids && ele?.SMSHistoryId && `${payload?.sms_history_ids}`?.includes(ele?.SMSHistoryId)){
            return false;
        }else if(payload?.call_history_ids && ele?.CallHistoryId && `${payload?.call_history_ids}`?.includes(ele?.CallHistoryId)){
            return false;
        }else if(payload?.draft_ids && ele?.draft_id && `${payload?.draft_ids}`?.includes(ele?.draft_id)){
            return false;
        }
        return true
    });
};