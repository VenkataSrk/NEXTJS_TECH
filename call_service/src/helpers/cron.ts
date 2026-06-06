import moment from 'moment';
import { getCallblastInfo } from '../dao/callBlast.dao';
import { insertNotificationDao } from '../dao/notification.dao';
import { call, campaignClient, ioredisChat } from '../plugins/db';
const schedule = require('node-schedule');
import { v4 as uuidv4 } from 'uuid';

function scheduleRun() {
  try {
    const job = schedule.scheduleJob('0 */1 * * * *', async () => {
      console.log('Job executed at:', new Date());
      const res = await getCallblastInfo();
      console.log('getCallblastInfo', JSON.stringify(res));
      if (res && Array.isArray(res) && res.length > 0) {
        for (const i of res) {
          const contact = JSON.parse(i?.contact ?? '[]');
          const importContactFile = JSON.parse(i?.importContactFile ?? '[]');
          const addContactFile = JSON.parse(i?.addContact ?? '[]');
          const campaignMessage = JSON.parse(i.campaignMessage ?? 'null');
          const textSpeechFile = JSON.parse(i.textSpeechFile ?? 'null');
          const promptMessageUrl = JSON.parse(i.promptMessage ?? 'null');
          const promptMessage = promptMessageUrl ? promptMessageUrl?.path : '';
          const connectingAudioUrl = JSON.parse(i.connectingAudio ?? 'null');
          const connectingAudio = connectingAudioUrl ? connectingAudioUrl?.path : '';
          const agentUnavailableUrl = JSON.parse(i.agentUnavailable ?? 'null');
          const agentUnavailable = agentUnavailableUrl ? agentUnavailableUrl?.path : '';
          const agentRejectUrl = JSON.parse(i.agentReject ?? 'null');
          const agentReject = agentRejectUrl ? agentRejectUrl?.path : '';

          const url = campaignMessage?.[0]?.url;
          let path = '';
          if (url?.trim()) {
            path = url.replace('https://ccaas-storage.worktual.co.uk', '');
          }
          const Texturl = textSpeechFile?.url;
          let textpath = '';
          if (Texturl?.trim()) {
            textpath = Texturl.replace('https://ccaas-storage.worktual.co.uk', '');
          }
          const endDateCon = i?.endDate
            ? moment(i?.endDate).format('YYYY-MM-DD')
            : null;
          console.log('endDateConk', endDateCon);
          // const endDateCon = i?.endDate ? i?.endDate?.split('T')[0] : null;
          // console.log("endDateConk",endDateConk,endDateCon)

          const data = {
            campaign_action: 1,
            blastName: i?.blastName,
            trigger_type: i?.triggerType,
            pd_id: i?.blastId,
            endDate: endDateCon,
            session_id: uuidv4(),
            triggered_id: i?.triggeredId ?? 0,
            req_type: i.campaignType === 1 ? 'CALL_BLAST' : i.campaignType === 2 ? 'PREDICTIVE_DIALER' : 'AGENT_SPECIFIC_CAMPAIGN',
            frequency: JSON.parse(i?.frequency ?? '[]'),
            domain_name: i?.companyName ?? '',
            domain_id: JSON.stringify(i?.domainId),
            caller_id: i?.outboundNumber ?? '',
            play_file:
                i.campaignMessageType === 1 ? (path ? `/volume/nfsshare/ccaas${path}` : '')
                : i.campaignMessageType === 2 ? (textpath ? `/volume/nfsshare/ccaas${textpath}` : '')
                : promptMessage,
            did_list: i?.contactType === 1 ? contact ?? [] : (i?.contactType === 2) ? importContactFile ?? [] : (i?.contactType === 3) ? addContactFile ?? [] : [],
            Working_BusinessHours: JSON.parse(i?.Working_businessHours ?? '[]'),
            connecting_audio: connectingAudio,
            agentunavailable_audio: agentUnavailable,
            agentreject_audio: agentReject,
            agent_list: JSON.parse(i?.agent_list ?? '[]')
          };
          console.log('publish data to umn', data);
          /* if (data?.domain_id) {
            const domainId = data?.domain_id.toString().split('').map(Number).reduce((acc, digit) => acc + digit, 0);
            if ((domainId % 2) === 0 && process.env.EVEN === 'EVEN') {
              call.publish(process.env.CAMPAIGN, JSON.stringify(data));
            } {
              call.publish(process.env.CAMPAIGN, JSON.stringify(data));
            }
          } */
          const channelName = await getRedisChannel(i?.domainId);
          console.log('publish data to umn channelName', channelName);
          // call.publish(process.env.CAMPAIGN, JSON.stringify(data));
          call.publish(channelName, JSON.stringify(data));

          insertNotificationDao({
            uuid: uuidv4(),
            domainId: i?.domainId,
            toExt: null,
            notificationMsg: `Outbound campaign <b>${i?.blastName}</b> has been started successfully.`,
            type: 'Outbound campaign',
            channelType: 'Call',
            markAsRead: 0,
            timeStamp: new Date().getTime(),
          });
          ioredisChat.to(`${i?.domainId}`).emit('refreshNotification');
        }

      }
    });
  } catch (error) {
    console.log('scheduleRun error:', error);
  }
}

scheduleRun();

export const getRedisChannel = async (domainId: any) => {
  // let channelOne = "CAMPAIGN_ACD_QA_66";
  // let channelTwo = "CAMPAIGN_ACD_QA_125";
  let channel = '';
  const data = await campaignClient.get(`CAMPAIGN_ACD_CHANNEL_${domainId}`);
  const domain = typeof domainId === 'number' ? domainId : parseInt(domainId, 10);
  console.log(' getRedisChannel ', data);
  if (data !== 'null' && data) {
    channel = data;
  } else if (domain % 2 === 0) {
    channel = process.env.CAMPAIGN_CHANNEL_ONE;
  } else {
    channel = process.env.CAMPAIGN_CHANNEL_TWO;
  }
  return channel;
};
