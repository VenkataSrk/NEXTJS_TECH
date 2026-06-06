import { getFiFOQueue, getQueueSettings } from './dao/callflow.dao';
import { deleteQueueWait } from './dao/queueWaitTime.dao';
import { loggerError } from './plugins/logger';

export const InvalidCallsHandler = async (channel: any, adaptor: any) => {
  try {
    if (channel.call_state === 'invalid' && channel?.data?.queueSettings && channel?.data?.agentDetials) {
      const deleteData: any = {
        sessionId: channel.session_id,
        qid: channel?.data?.queueSettings?.qid,
      };
      deleteQueueWait(deleteData);
      if (channel?.data?.agentDetials) {
        const queueDataInfo: any = await getFiFOQueue(channel?.data?.agentDetials?.ext, channel?.data?.agentDetials?.domainId);
        if (queueDataInfo[0]?.queueList?.length > 0) {
          setInterval(async () => {
            const currentqueueSettings: any = await getQueueSettings(queueDataInfo[0]?.qid);
            if (channel?.data?.agentDetials) {
              const nextCustomerData: any = {
                queueDataInfo,
                session_id: queueDataInfo[0]?.queueList[0]?.sessionId,
                req_type: 'QUEUE_CONNECT',
                data: {
                  queueSettings: currentqueueSettings,
                  agentDetials: channel?.data?.agentDetials,
                },
              };
              adaptor.publishMessage(JSON.stringify(nextCustomerData));
            }
          },          10000);
        }
      }
    }
  } catch (error) {
    loggerError('InvalidCallsHandler', channel, error);
  }
};
