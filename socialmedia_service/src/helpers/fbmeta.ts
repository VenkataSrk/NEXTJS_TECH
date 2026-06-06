import axios from 'axios';
import { getconnectDataFB } from '../dao/facebook';
import { updateMessageFlowAppoinment } from '../modules/facebook/handler';
import { logger } from '../plugins/log';
import { appoinment, messageValue } from './constants';
import { contextMatch } from './utils';

// send Message Customer
/* tslint:disable */
export async function sendMessageCustomer(data: any, message: any) {
  try {
    const cleanMessage = typeof message === 'string' ? message : message?.message;
    let msg = cleanMessage;
    msg = msg.replace(/<p[^>]*>/g, '').replace(/<\/p>/g, '\n\n').replace(/\s+/g, ' ').trim();
    if (msg.includes('<ul>') && msg.includes('</ul>')) {
      msg = msg.replace(/<ul>/g, '\n').replace(/<\/ul>/g, '');
      msg = msg.replace(/<li>\s*/g, '• ').replace(/<\/li>/g, '\n');
    }
    let counter = 1;
    msg = msg.replace(/<ol>/g, '\n').replace(/<\/ol>/g, '');
    msg = msg.replace(/<li>\s*/g, () => `${counter++}. `).replace(/<\/li>/g, '\n');
    msg = msg.replace(/<b>/g, '').replace(/<\/b>/g, '');
    msg = msg.replace(/<strong>/g, '').replace(/<\/strong>/g, '').replace(/<em>/g, '_').replace(/<\/em>/g, '_').replace(/<s>/g, '~').replace(/<\/s>/g, '~');
    msg = msg.replace(/<a[^>]*>(.*?)<\/a>/g, '$1');
    msg = msg.replace(/\n\s*\n/g, '\n').trim();
    const context = contextMatch(msg);
    if (context?.length > 0) {
      const connect: any = await getconnectDataFB(data);
      const checkArray = Array.isArray(JSON.parse(connect?.connectorData)) ? JSON.parse(connect?.connectorData) : [JSON.parse(connect?.connectorData)];
      for (const context_data of context) {
        for (const temp_data of checkArray) {
          if (`context.${temp_data.key}` === context_data) {
            msg = msg.replace(`\${${context_data}}`, temp_data.value);
          }
        }
        msg = msg.replace(`\${${context_data}}`, '');
      }
    }
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerFacebookId,
      pageId: data?.agentFacebookId,
      type: 'text',
      message: msg
    };
    const response = await axios.post(process?.env?.FacebookOut, body);
    return body;
  } catch (error) {
    logger.error('sendMessageCustomer err', error);
  }
}

// send Message Customer
export async function sendMessageCustomerFbFlow(data: any, message: any) {
  try {
    const cleanMessage = typeof message === 'string' ? message.replace(/<[^>]+>/g, '') : message?.message;
    let msg = cleanMessage;
    const context = contextMatch(msg);
    if (context?.length > 0) {
      const connect: any = await getconnectDataFB(data);
      const checkArray = Array.isArray(JSON.parse(connect?.connectorData)) ? JSON.parse(connect?.connectorData) : [JSON.parse(connect?.connectorData)];
      for (const context_data of context) {
        for (const temp_data of checkArray) {
          if (`context.${temp_data.key}` === context_data) {
            msg = msg.replace(`\${${context_data}}`, temp_data.value);
          }
        }
        msg = msg.replace(`\${${context_data}}`, '');
      }
    }
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerFacebookId,
      pageId: data?.agentFacebookId,
      type: 'text',
      message: msg
    };
    const response = await axios.post(process?.env?.FacebookOut, body);
    return body;
  } catch (error) {
    logger.error('sendMessageCustomerFbFlow err', error);
  }
}

// send message connect live agent
export async function sendConnectLiveAgent(data: any) {
  try {
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerFacebookId,
      pageId: data?.agentFacebookId,
      type: 'text',
      message: messageValue?.liveAgent
    };
    const response = await axios.post(process.env.FacebookOut, body);
    return body;
  } catch (error) {
    logger.error('sendConnectLiveAgent err', error);
  }
}

// send message connect live agent
export async function sendTransferAgent(data: any) {
  try {
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerFacebookId,
      pageId: data?.agentFacebookId,
      type: 'text',
      message: messageValue?.anotherAgent
    };
    const response = await axios.post(process.env.FacebookOut, body);
    logger.info('sendTransferAgent', response);
    return body;
  } catch (error) {
    logger.error('sendTransferAgent err', error);
  }
}

// send message
export async function sendMessage(req: any) {
  try {
    const body = {
      domainId: req?.domainId,
      customerId: req?.customerFacebookId,
      pageId: req?.agentFacebookId,
      message: messageValue?.agentBusy
    };
    const response = await axios.post(process.env.FacebookOut, body);
    logger.info('sendMessage', response);
    return body;
  } catch (error) {
    logger.error('sendMessage err', error);
  }
}

// send customer satisfications
export async function sendMessageSatisfication(data: any, message: any, domainId: any) {
  try {
    const body = {
      domainId,
      customerId: data?.customerId,
      pageId: data?.pageId,
      type: 'text',
      message: message[0]?.thanksNote
    };
    const response = await axios.post(process?.env?.FacebookOut, body);
    logger.info('sendMessageSatisfication', response);
    return body;
  } catch (error) {
    logger.error('sendMessageSatisfication err', error);
  }
}
// send end chat
export async function getDispositinDisconnectStatus(data: any) {
  try {
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerFacebookId,
      pageId: data?.agentFacebookId,
      type: 'text',
      message: messageValue?.endChat
    };
    const response = await axios.post(process.env.FacebookOut, body);
    logger.error('getDispositinDisconnectStatus', response);
    return body;
  } catch (error) {
    logger.error('getDispositinDisconnectStatus err:', error);
  }
}

// send service
export async function sendAppointmentRes(data: any, serviceNames: any, lable: any) {
  try {
    const interactive_list: any = [];
    serviceNames.forEach((item: any) => {
      interactive_list.push({ content_type: 'text', title: item?.service_name, payload: item?.serviceid });
    });
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerFacebookId,
      pageId: data?.agentFacebookId,
      bottonTitle: lable?.label_name1?.split('_')?.join(' '),
      buttons: interactive_list
    };
    await axios.post(process?.env?.sendFeedbackForm, body);
    updateMessageFlowAppoinment(data, body?.bottonTitle);
    return body;
  } catch (error) {
    logger.error('sendAppointmentRes err', error);
  }
}

/**
 *
 * @param data
 * @param dateObjects
 * @returns
 */
export async function sendAppointmentDateFb(data: any, dateObjects: any) {
  try {
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerFacebookId,
      pageId: data?.agentFacebookId,
      bottonTitle: appoinment?.SelectDate,
      buttons: dateObjects
    };
    await axios.post(process?.env?.sendFeedbackForm, body);
    updateMessageFlowAppoinment(data, body?.bottonTitle);
    return body;
  } catch (error) {
    logger.error('sendAppointmentDateFb err', error);
  }
}

// send categories
export async function sendCategoryName(data: any, categoryName: any) {
  try {
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerFacebookId,
      pageId: data?.agentFacebookId,
      bottonTitle: appoinment?.SelectCategory,
      buttons: categoryName
    };
    await axios.post(process?.env?.sendFeedbackForm, body);
    updateMessageFlowAppoinment(data, body?.bottonTitle);
    return body;
  } catch (error) {
    logger.error('sendCategoryName err', error);
  }
}

// send all userName
export async function sendAllUserName(data: any, getUser: any) {
  logger.info('Select consultant', getUser);
  try {
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerFacebookId,
      pageId: data?.agentFacebookId,
      bottonTitle: appoinment?.SelectConsultant,
      buttons: getUser
    };
    await axios.post(process?.env?.sendFeedbackForm, body);
    updateMessageFlowAppoinment(data, body?.bottonTitle);
    return body;
  } catch (error) {
    logger.error('sendAllUserName err', error);
  }
}

// send Interactive one to one
export async function sendAppointmentOneToOne(data: any, dateObjects: any, lable: any) {
  logger.info('Select mins', dateObjects);
  try {
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerFacebookId,
      pageId: data?.agentFacebookId,
      bottonTitle: lable?.label_name9?.split('_')?.join(' '),
      buttons: dateObjects
    };
    await axios.post(process?.env?.sendFeedbackForm, body);
    updateMessageFlowAppoinment(data, body?.bottonTitle);
    return body;
  } catch (error) {
    logger.error('sendAppointmentOneToOne err', error);
  }
}

// send Interactive duration
export async function sendAppointmentDuration(data: any, dateObjects: any, lable: any) {
  const durationCollected = JSON.parse(dateObjects);
  const result = durationCollected.map((item: any, index: any) => ({
    content_type: 'text',
    title: item.type === 1 ? `${+(item.duration)} mins` : `${+(item.duration / 60)} hours`,
    payload: index,
  }));
  logger.info('Select mins', result);
  try {
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerFacebookId,
      pageId: data?.agentFacebookId,
      bottonTitle: lable?.label_name9?.split('_')?.join(' '),
      buttons: result
    };
    await axios.post(process?.env?.sendFeedbackForm, body);
    updateMessageFlowAppoinment(data, body?.bottonTitle);
    return body;
  } catch (error) {
    logger.error('sendAppointmentDuration err', error);
  }
}

// bookingDuration
export async function sendbookingDuration(data: any, getDuration: any, lable: any) {
  try {
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerFacebookId,
      pageId: data?.agentFacebookId,
      bottonTitle: lable?.label_name11?.split('_')?.join(' '),
      buttons: getDuration
    };
    await axios.post(process?.env?.sendFeedbackForm, body);
    updateMessageFlowAppoinment(data, body?.bottonTitle);
    return body;
  } catch (error) {
    logger.error('sendbookingDuration err', error);
  }
}

// send booking details
export async function sendBookingDetailsOneToOne(data: any, getBooking: any) {
  try {
    const booking = JSON.parse(getBooking?.p_book_form);
    const formattedLastName = booking[1]['Last name'] ? `${booking[1]['Last name']}` : '';
    const formatNumber = booking[2]['Phone number'] !== null ? `${booking[2]['Phone number']}` : '-';
    const context = `Thank You! Your appointment has been booked successfully.\n\n Appointment ID\n ${getBooking?.app_booked_id}\n\n Booking date\n ${getBooking?.p_book_date}\n\n Timing\n ${getBooking?.p_book_starttime} - ${getBooking?.p_book_endtime}\n\n Name\n ${booking[0]['First name']} ${formattedLastName}\n\n Email\n ${booking[3]?.Email}\n\n Phone\n ${formatNumber}\n\n`;
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerFacebookId,
      pageId: data?.agentFacebookId,
      type: 'text',
      message: context
    };
    logger.info('body>>>>>>>>>>>>>', body);
    await axios.post(process?.env?.FacebookOut, body);
    updateMessageFlowAppoinment(data, context);
    return body;
  } catch (error) {
    logger.error('sendBookingDetailsOneToOne err', error);
  }
}

// send booking details
export async function sendBookingDetails(data: any, getBooking: any, getProcessTime: any) {
  try {
    const booking = JSON.parse(getBooking?.p_book_form);
    const formattedLastName = booking[1]['Last name'] ? `*${booking[1]['Last name']}*` : '';
    const context = `Thank You! Your appointment has been booked successfully.\n\n Appointment ID\n ${getBooking?.app_booked_id}\n\n Booking date\n ${getBooking?.p_book_date}\n\n Timing\n ${getBooking?.p_book_starttime} - ${getBooking?.p_book_endtime}\n\n Service\n ${getBooking?.service}\n\n Name\n ${booking[0]['First name']} ${formattedLastName}\n\n Email\n ${getProcessTime?.emailId}\n\n Phone\n ${getProcessTime?.phoneNumber}\n\n`;
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerFacebookId,
      pageId: data?.agentFacebookId,
      type: 'text',
      message: context
    };
    await axios.post(process?.env?.FacebookOut, body);
    updateMessageFlowAppoinment(data, context);
    return body;
  } catch (error) {
    logger.error('sendBookingDetails err', error);
  }
}

// send Button
export async function sendFlowBreakInput(req: any) {
  try {
    const body = {
      domainId: req?.domainId,
      customerId: req?.customerFacebookId,
      pageId: req?.agentFacebookId,
      postbackText: messageValue?.disconnectFlow,
      buttons: [{
        type: 'postback',
        title: 'Yes',
        payload: '100'
      },
        {
          type: 'postback',
          title: 'No',
          payload: '200'
        }]
    };
    return await axios.post(process.env.postbackButton, body);
  } catch (error) {
    logger.error('sendFlowBreakInput err:', error);
  }
}

// send Button
export async function sendFlowBreakInpuRemaindertFb(req: any) {
  try {
    const body = {
      domainId: req?.domainId,
      customerId: req?.customerFacebookId,
      pageId: req?.agentFacebookId,
      postbackText: messageValue?.continueFlow,
      buttons: [{
        type: 'postback',
        title: 'Yes',
        payload: '10001'
      },
        {
          type: 'postback',
          title: 'No',
          payload: '20002'
        }]
    };
    return await axios.post(process.env.postbackButton, body);
  } catch (error) {
    logger.error('sendFlowBreakInpuRemaindertFb err:', error);
  }
}

// send booking details
export async function sendAppoinmentPayment(data: any) {
  try {
    const booking = JSON.parse(data?.p_book_form);
    const formattedLastName = booking[1]['Last name'] ? `*${booking[1]['Last name']}*` : '-';
    const formatNumber = booking[2]['Phone number'] !== null ? `*${booking[2]['Phone number']}*` : '-';
    let content: any = '';
    if (data?.p_app_type === 4) {
      content = `Thank You! Your appointment has been booked successfully.\n\n Appointment ID\n *${data?.app_booked_id}*\n\n Booking date\n *${data?.p_book_date}*\n\n Timing\n *${data?.p_book_starttime}* - *${data?.p_book_endtime}*\n\n Service\n *${data?.service_name}*\n\n Name\n *${booking[0]['First name']}* ${formattedLastName}\n\n Email\n *${booking[3]?.Email}*\n\n Phone\n ${formatNumber}\n\n`;
    } else {
      content = `Thank You! Your appointment has been booked successfully.\n\n Appointment ID\n *${data?.app_booked_id}*\n\n Booking date\n *${data?.p_book_date}*\n\n Timing\n *${data?.p_book_starttime}* - *${data?.p_book_endtime}*\n\n Name\n *${booking[0]['First name']}* ${formattedLastName}\n\n Email\n *${booking[3]?.Email}*\n\n Phone\n ${formatNumber}\n\n`;
    }
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerNumber,
      pageId: data?.calledNumber,
      type: 'text',
      message: content
    };
    await axios.post(process?.env?.FacebookOut, body);
    return body;
  } catch (error) {
    logger.error('sendAppoinmentPayment err:', error);
  }
}

// send Button
export async function sendPayment(data: any, appointmentUrl: any) {
  try {
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerFacebookId,
      pageId: data?.agentFacebookId,
      postbackText: messageValue?.checkHere,
      buttons: [
        {
          type: 'web_url',
          url: appointmentUrl,
          title: 'Pay Now',
          webview_height_ratio: 'full'
        }
      ]
    };
    return await axios.post(process.env.postbackButton, body);
  } catch (error) {
    logger.error('sendPayment err:', error);
  }
}


// send Typing Indicator
export async function sendTypingIndicator(data: any, typing: any): Promise<void> {
  return new Promise<void>(async (resolve, reject) => {
    try {
      const body = {
        domainId: data?.domainId,
        customerId: data?.customerFacebookId,
        pageId: data?.agentFacebookId,
        sender_action: typing
      };
      const result: any = await axios.post(process?.env?.sendTyping, body);
      logger.info('sendTypingIndicator', result);
      resolve(result);
    } catch (error) {
      logger.error('sendTypingIndicator err:', error);
      reject(error);
    }
  });
}

// send Button fb
export async function sendButtonFb(data: any, url:any, title:any, plainText:any) {
  try {
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerFacebookId,
      pageId: data?.agentFacebookId,
      postbackText: plainText,
      buttons: [
        {
          type: 'web_url',
          url,
          title,
          webview_height_ratio: 'full'
        }
      ]
    };
    console.log("body>>>>>>>>",body)
    return await axios.post(process.env.postbackButton, body);
  } catch (error) {
    logger.error('sendPayment err:', error);
  }
}