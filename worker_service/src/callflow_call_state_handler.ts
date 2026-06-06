import { v4 as uuidv4 } from 'uuid';
import { getCustomerDetails, getQueueSettings, updateAgentSessionId } from './dao/callflow.dao';
import { insertNotificationDao } from './dao/notification.dao';
import { nlpSummary } from './fetch_api';
import { ioredisChat } from './plugins/db';
import { loggerError } from './plugins/logger';

export async function CallflowCallStateHandler(channel: any, adaptor: any) {
  try {
    if (channel?.req_type === 'INVITE') {
      ioredisChat.to(`${channel?.domain_id}_${channel?.ext}`).emit('incomingcall_queue_name', channel);
    }

    if (channel?.holdState) {
      const agentStatus: any = { ...channel, domainId: channel?.domain_id, currentSessionId: channel?.session_id, ext: channel?.ext };
      await updateAgentSessionId(agentStatus);
    }

    if (channel.disconnectedBy) {
      console.log(`DISCONNECTED BY ${channel.disconnectedBy}`);
      try {
        if (adaptor?.convoRedis?.isOpen) {
          adaptor?.convoRedis?.unsubscribe();
          adaptor?.convoRedis?.quit();
        }
      } catch (error) {
        console.error('CLIENT IS ALREADY CLOSED');
      }
    }

    if (channel?.status === 'cancelled' && channel?.ext && channel?.domain_id) {
      const customerDetails: any = await getCustomerDetails(channel?.cli);
      const incomingCallData = {
        id: uuidv4(),
        type: 'missed',
        domainId: channel?.domain_id,
        message: `Missed call from ${!customerDetails?.[0]?.firstName ? channel?.cli : `${customerDetails?.[0]?.firstName ?? ''} ${customerDetails?.[0]?.lastName ?? ''}`}`,
      };

      await insertNotificationDao({
        uuid: incomingCallData.id,
        domainId: channel.domain_id,
        type: incomingCallData.type,
        toExt: channel.ext,
        notificationMsg: incomingCallData?.message,
        channelType: 'Call',
        markAsRead: 1,
        timeStamp: new Date().getTime(),
        coid: customerDetails?.[0]?.coid ?? null,
        customerValue: channel?.cli ?? null
      });

      ioredisChat.to(`${channel?.domain_id}_${channel?.ext}`).emit('notification', incomingCallData);
    }

    if (channel?.status === 'connected' && channel.ext) {
      if (channel?.pd === 1) {
        ioredisChat.to(`${channel.domain_id}_${channel.ext}`).emit('CAMPAIGN_DIALOUT_SESSION', channel);
      }
      if (channel.agentState) {
        const agentStatus: any = { ...channel, domainId: channel?.domain_id, currentSessionId: channel?.session_id, ext: channel?.ext };
        const agentUpdateCurrentSessionId = await updateAgentSessionId(agentStatus);
        console.log('AGENT UPDATE CURRENT SESSION ID:::::::::::', agentUpdateCurrentSessionId);
        adaptor.emitMessage(`${channel?.domain_id}_${channel?.ext}`, 'acceptCall', channel.session_id);
        adaptor.emitMessage(`${channel?.domain_id}_${channel?.ext}`, 'currentCaller', channel.ddi);

        if (channel.qid) {
          await getQueueSettings(channel.qid);
        }
      }

      if (channel.status === 'disconnected') {
        const agentStatusUpdate: any = { ...channel, domainId: channel?.domain_id, currentSessionId: '', ext: channel?.ext };
        await updateAgentSessionId(agentStatusUpdate);
      }
      console.log('CALL CONNECTED');
    }

    if (channel?.acd_event === 47) {
      if (channel?.Agent?.length > 0) {
        const list = [];
        channel.Agent?.map((ext: any) => {
          list.push(`${channel?.domain_id}_${ext}`);
        });
        ioredisChat.to(list).emit('pstnleavecall', channel);
      }
    }

    if (channel?.req_type === 'AGENT_REGISTER') {
      ioredisChat.to(`${channel?.domain_id}_${channel?.ext}`).emit('AGENT_REGISTER', channel);
    }

    if ((channel?.req_type === 'UPDATE' && channel?.call_type === 'inbound' && channel?.acd_event === 10) && (channel?.auto_attendant || channel?.virtual_agent)) {
      await nlpSummary(channel);
    }
  } catch (error) {
    loggerError('CallflowCallStateHandler', channel, error);
  }
}
