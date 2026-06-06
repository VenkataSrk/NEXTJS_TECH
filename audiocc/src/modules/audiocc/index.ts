
import { logger } from '../../plugin/log';
import { agentAssistStream, audioClientStream, ioredisChat } from '../../plugin/db';
import { v4 as uuidv4 } from 'uuid';

export async function audiocc(data: any) {
  try {
    console.log('Audiocc Data::::::::', data);
    if (data !== null) {
      let userList: any = {};
      const domainId = data.domain_id;
      const agentList = data?.agent.map((list: any) => {
        if (list?.ext === data?.speaker) {
          userList = list;
        }
        const id = `${domainId}_${list.ext}`;
        return id;
      });

      if (data?.text?.trim()?.length > 0) {
        let resultMessage: any;
        if (data.is_bot === 1 && data.speaker === 'bot') {
          resultMessage = {
            id: uuidv4(),
            from: `${domainId}_${userList?.ext ?? ''}`,
            to: data?.session_id,
            sessionId: data?.session_id,
            message: `<p>${data?.text}</p>`,
            sender: {
              Name: 'Bot',
            },
            isBot: true,
            isCustomer: false,
            messageType: 'Text',
            sendAt: Math.floor(Date.now() / 1000),
            chatIconLogo: '',
            timestamp: Math.floor(Date.now() / 1000),
            transcriptTm: data.transcriptTm,
            emotion: data?.emotion || '',
          };
        } else if (data?.speaker === userList?.ext) {
          resultMessage = {
            id: uuidv4(),
            from: `${domainId}_${userList?.ext}`,
            to: data?.session_id,
            sessionId: data?.session_id,
            message: `<p>${data?.text}</p>`,
            sender: {
              uuid: userList?.userId ?? '',
              Name: userList?.name ?? '',
              Email: userList?.email ?? '',
            },
            isBot: false,
            isCustomer: false,
            messageType: 'Text',
            sendAt: Math.floor(Date.now() / 1000),
            chatIconLogo: '',
            timestamp: Math.floor(Date.now() / 1000),
            transcriptTm: data.transcriptTm,
            emotion: data?.emotion || '',
          };
        } else {
          resultMessage = {
            messageID: uuidv4(),
            from: data?.session_id,
            to: `${domainId}_${data?.agent?.[0]?.ext}`,
            sessionId: data?.session_id,
            message: `<p>${data?.text}</p>`,
            sendAt: Math.floor(Date.now() / 1000),
            messageType: 'Text',
            pdfurl: null,
            isBot: false,
            isCustomer: true,
            sender: {
              'Full Name': `${data.customer_first_name || 'New Customer'} ${data.customer_last_name || ''}`,
              Email: data.customerDetails?.email ?? '',
              'Phone Number': data.customer_number,
            },
            transcriptTm: data.transcriptTm,
            emotion: data?.emotion || '',
          };
        }
        console.log(data?.speaker, ':', data?.text);
        sendMsgToClient(data, resultMessage, userList, agentList);
      }
    }
  } catch (error) {
    logger.error('sendMsgToClient', data, error);
  }
}

async function sendMsgToClient(data: any, resultMessage: any, userList: any, agentList: any) {
  try {
    if (!data?.is_bot && agentList.length > 0) {
      console.log('agentList:', agentList);
      ioredisChat?.to(agentList).emit('OneToOneCallReceive', resultMessage);
    }
    console.log(data?.speaker === data?.customer_number, data?.is_bot);
    if (data?.speaker === data?.customer_number && !data?.is_bot) {
      const message: any = {
        sessionId: data.session_id,
        message: data.text,
        messageType: 'TEXT',
        domainId: data.domain_id,
        timeStamp: Math.floor(Date.now() / 1000),
        to: agentList,
        channelType: 'Call',
        contactNumber: data.ccaas_number,
        customer_number: data?.customer_number,
        messageID: resultMessage?.messageID
      };
      agentAssistStream.xadd(process.env.AGENT_ASSIST_REDIS_CHANNEL_REQ, '*', 'agent_assist_data', JSON.stringify(message));
    }

    if (!data?.is_bot) {
      for (const i of data?.agent) {
        if (i?.ext.length === 3) {
          const updateMsg: any = { session_id: data?.session_id, message: resultMessage, domain_id: data.domain_id, ext: i?.ext, bot: null };
          console.log('Update agent message:', updateMsg);
          audioClientStream.xadd(process.env.CALL_STREAM_MESSAGE_NAME, '*', 'transcript_data', JSON.stringify(updateMsg));
        }
      }
    } else if (data?.is_bot) {
      const updateMsg: any = { session_id: data?.session_id, message: resultMessage, domain_id: data.domain_id, ext: null, bot: 1 };
      console.log('Update bot message:', updateMsg);
      audioClientStream.xadd(process.env.CALL_STREAM_MESSAGE_NAME, '*', 'transcript_data', JSON.stringify(updateMsg));
    }

  } catch (error) {
    logger.error('sendMsgToClient', JSON.stringify({ data, resultMessage, agentList }), error);
  }
}

export async function agentAssistMsg(data: any) {
  try {
    data.message = data.text;
    ioredisChat.to(data.to).emit('agentassist', data);
  } catch (error) {
    logger.error('agentAssistMsg', data, error);
  }
}
