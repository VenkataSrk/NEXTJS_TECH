import { updateAgentSessionId } from './dao/callflow.dao';
import { ioredisChat } from './plugins/db';
import { loggerError } from './plugins/logger';

export const OutboundCallsHandler = async (channel: any, ioHandler: any) => {
  try {
    if (channel.req_type === 'UPDATE' && channel.call_type === 'outbound') {
      ioredisChat.to(`${channel.domain_id}_${channel?.cli}`).emit('outboundcallSession', channel);

      // AGENT STATE: WHEN THE AGENT PICKS THE CALL
      if (channel.agentState) {
        const agentStatus: any = { ...channel, ext: channel?.ext, domainId: channel?.domain_id, currentSessionId: channel?.session_id };
        await updateAgentSessionId(agentStatus);
        ioHandler.to(channel.ext.toString()).emit('acceptCall', channel.session_id);
      }
      if (channel.status === 'disconnected') {
        const agentStatusUpdate: any = { ext: channel?.ext, domainId: channel?.domain_id, currentSessionId: channel?.session_id };
        await updateAgentSessionId(agentStatusUpdate);
      }
    } else if (channel.req_type === 'UPDATE' || channel.callTransfered === 1 || channel.callMerge === 1) {
      if (channel.callTransfered === 1) {
        ioredisChat.to(`${channel.domain_id}_${channel.transferee}`).emit('callTransferedSuccess', channel);
      } else if (channel.callMerge === 1) {
        if (channel?.Agent?.length > 0) {
          const list = [];
          channel.Agent.map((ext: any) => {
            list.push(`${channel.domain_id}_${ext}`);
          });
          console.log('CALL MERGED SUCCESS :::::::', list);
          ioredisChat.to(list).emit('callMergeedSuccess', channel);
        }
      }
    }
  } catch (error) {
    loggerError('OutboundCallsHandler', channel, error);
  }
};
