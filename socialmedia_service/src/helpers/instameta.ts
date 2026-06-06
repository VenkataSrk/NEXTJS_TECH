import axios from 'axios';
import { getconnectDataFB } from '../dao/facebook';
import { getconnectData } from '../dao/instagram';
import { updateMessageFlowAppoinment } from '../modules/facebook/handler';
import { updateMessageFlowAppoinmentInsta } from '../modules/instagram/handler';
import { logger } from '../plugins/log';
import { appoinment, messageValue } from './constants';
import {  chunkArray, contextMatch } from './utils';

// send Message Customer
/* tslint:disable */
export async function sendMessageCustomerInstagram(data: any, message: any) {
  try {
    const cleanMessage = typeof message === 'string' ? message : message?.message;
    let msg = cleanMessage;
    msg = msg.replace(/<p[^>]*>/g, '')?.replace(/<\/p>/g, '\n\n').replace(/\s+/g, ' ').trim()?.replace(/<br\s*\/?>/gi, '\n');
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
      customerId: data?.customerInstagramId,
      pageId: data?.agentInstagramId,
      type: 'text',
      message: msg
    };
    const response = await axios.post(process?.env?.InstagramOut, body);
    return response;
  } catch (error) {
    logger.error('sendMessageCustomerInstagram err', error);
  }
}

// send Message Customer
export async function sendMessageCustomerInstagramFlow(data: any, message: any) {
  try {
    const cleanMessage = typeof message === 'string' ? message.replace(/<[^>]+>/g, '') : message?.message;
    let msg = cleanMessage;
    const context = contextMatch(msg);
    if (context?.length > 0) {
      const connect: any = await getconnectData(data);
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
      customerId: data?.customerInstagramId,
      pageId: data?.agentInstagramId,
      type: 'text',
      message: msg
    };
    await axios.post(process?.env?.InstagramOut, body);
    return body;
  } catch (error) {
    logger.error('sendMessageCustomerInstagramFlow err', error);
  }
}

// customer satification
export async function sendCustomerSatificationFb(data: any) {
  try {
    let rating = [];
    if (data?.ratingDesignType === 1 || data?.ratingDesignType === 2 || data?.ratingDesignType === 3) {
      rating = ['Very Bad', 'Bad', 'Satisfied', 'Good', 'Very Good'];
    } else if (data?.ratingDesignType === 4 || data?.ratingDesignType === 5) {
      rating = ['Very Bad', 'Satisfied', 'Very Good'];
    } else {
      rating = ['Bad', 'Good'];
    }
    const interactive_list: any = [];
    rating.forEach((item: any, index: any) => {
      interactive_list.push({
        content_type: 'text',
        title: item,
        payload: `${index + 1}-${data?.HistoryId}-${data?.domainId}-${data?.AgentId}-${data?.ratingDesignType}`
      });
    });
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerContact,
      pageId: data?.domainContact,
      bottonTitle: data?.caption,
      buttons: interactive_list
    };
    await axios.post(process?.env?.sendFeedbackForm, body);
    return body;
  } catch (error) {
    logger.error('sendCustomerSatificationFb err', error);
  }
}

// customer satification facebook
export async function sendCustomerSatificationFbDropdown(data: any, dropDownMessage: any, dropDownMultipleData: any) {
  try {
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerFacebookId,
      pageId: data?.agentFacebookId,
      bottonTitle: dropDownMessage?.replace(/<\/*s>/g, '~').replace(/<\/*strong>/g, '*').replace(/<\/*em>/g, '_').replace(/<div>/g, '').replace(/<\/div>|<br>/g, '\n').replace(/<[^>]*>/g, '').replace(/&nbsp;/g, ' '),
      buttons: dropDownMultipleData
    };
    await axios.post(process?.env?.sendFeedbackForm, body);
    return body;
  } catch (error) {
    logger.error('sendCustomerSatificationFbDropdown err', error);
  }
}

/**
 *
 * @param data
 * @param carouselData
 * @returns
 */
export async function sendCarouselDataInsta(data: any, carouselData: any) {
  try {
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerInstagramId,
      pageId: data?.agentInstagramId,
      elements: carouselData
    };
    return await axios.post(process.env.sendCarousel, body);
  } catch (error) {
    logger.error('sendCarouselDataInsta err:', error);
  }
}

// send Button
export async function sendPaymentInsta(data: any, appointmentUrl: any) {
  try {
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerInstagramId,
      pageId: data?.agentInstagramId,
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
    return await axios.post(process.env.postbackButtonInsta, body);
  } catch (error) {
    logger.error('sendPaymentInsta err:', error);
  }
}

// send Button
export async function sendButtonLinkInsta(req: any, buttonData: any) {
  try {
    const bodyContants = buttonData?.message;
    const bodyContantsData = bodyContants?.replace(/<\/*s>/g, '~').replace(/<\/*strong>/g, '*').replace(/<\/*em>/g, '_').replace(/<div>/g, '').replace(/<\/div>|<br>/g, '\n').replace(/<[^>]*>/g, '').replace(/&nbsp;/g, ' ');
    const body = {
      domainId: req?.domainId,
      customerId: req?.customerInstagramId,
      pageId: req?.agentInstagramId,
      postbackText: bodyContantsData,
      buttons: []
    };
    buttonData?.getInputData?.map((val: any) => {
      body?.buttons.push(
        {
          type: 'web_url',
          url: val?.link,
          title: val?.value,
          webview_height_ratio: 'full'
        }
      );
    });
    return await axios.post(process.env.postbackButtonInsta, body);
  } catch (error) {
    logger.error('sendButtonLinkInsta err:', error);
  }
}

// customer satification
export async function sendCustomerSatificationInsta(data: any) {
  try {
    let rating = [];
    if (data?.ratingDesignType === 1 || data?.ratingDesignType === 2 || data?.ratingDesignType === 3) {
      rating = ['Very Bad', 'Bad', 'Satisfied', 'Good', 'Very Good'];
    } else if (data?.ratingDesignType === 4 || data?.ratingDesignType === 5) {
      rating = ['Very Bad', 'Satisfied', 'Very Good'];
    } else {
      rating = ['Bad', 'Good'];
    }
    const interactive_list: any = [];
    rating.forEach((item: any, index: any) => {
      interactive_list.push({
        content_type: 'text',
        title: item,
        payload: `${index + 1}-${data?.HistoryId}-${data?.domainId}-${data?.AgentId}-${data?.ratingDesignType}`
      });
    });
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerContact,
      pageId: data?.domainContact,
      bottonTitle: data?.caption,
      buttons: interactive_list
    };
    await axios.post(process?.env?.sendFeedbackFormInsta, body);
    return body;
  } catch (error) {
    logger.error('sendCustomerSatificationInsta err', error);
  }
}

// customer satification
export async function sendCustomerSatificationInstaDropdown(data: any, dropDownMessage: any, dropDownMultipleData: any) {
  try {
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerInstagramId,
      pageId: data?.agentInstagramId,
      bottonTitle: dropDownMessage?.replace(/<\/*s>/g, '~').replace(/<\/*strong>/g, '*').replace(/<\/*em>/g, '_').replace(/<div>/g, '').replace(/<\/div>|<br>/g, '\n').replace(/<[^>]*>/g, '').replace(/&nbsp;/g, ' '),
      buttons: dropDownMultipleData
    };
    await axios.post(process?.env?.sendFeedbackFormInsta, body);
    return body;
  } catch (error) {
    logger.error('sendCustomerSatificationInstaDropdown err', error);
  }
}

// send customer satisfications
export async function sendMessageSatisficationInsta(data: any, message: any, domainId: any) {
  try {
    const body = {
      domainId,
      customerId: data?.customerId,
      pageId: data?.pageId,
      type: 'text',
      message: message[0]?.thanksNote
    };
    await axios.post(process?.env?.InstagramOut, body);
    return body;
  } catch (error) {
    logger.error('sendMessageSatisficationInsta err', error);
  }
}

// send Image Customer
export async function sendImageCustomerInsta(req: any, filesData: any, fileType: any) {
  try {
    const body = {
      domainId: req?.domainId,
      customerId: req?.customerInstagramId,
      pageId: req?.agentInstagramId,
      mediaUrl: filesData?.url,
      mediaType: fileType
    };
    await axios.post(process?.env?.sendMediaInsta, body);
    return body;
  } catch (error) {
    logger.error('sendImageCustomerInsta err:', error);
  }
}

// send Image Customer bot
export async function sendImageCustomerInstaBot(req: any, filesData: any) {
  try {
    let mediaType = '';
    if (filesData?.mimeType?.startsWith('image/')) {
      mediaType = 'IMAGE';
    }  else {
      throw new Error(`Unsupported media type: ${filesData?.mimeType}`);
    }
    const body = {
      domainId: req?.domainId,
      customerId: req?.customerInstagramId,
      pageId: req?.agentInstagramId,
      mediaUrl: filesData?.mediaUri,
      mediaType
    };
    const response = await axios.post(process.env.sendMediaInsta as string, body);
    return response;
  } catch (error) {
    logger.error('sendImageCustomerInsta err:', error);
    throw error;
  }
}


// send Button
export async function sendCustomerInputInsta(req: any, buttonData: any) {
  try {
    const bodyContants = buttonData?.message;
    const bodyContantsData = bodyContants?.replace(/<\/*s>/g, '~').replace(/<\/*strong>/g, '*').replace(/<\/*em>/g, '_').replace(/<div>/g, '').replace(/<\/div>|<br>/g, '\n').replace(/<[^>]*>/g, '').replace(/&nbsp;/g, ' ');
    const body = {
      domainId: req?.domainId,
      customerId: req?.customerInstagramId,
      pageId: req?.agentInstagramId,
      postbackText: bodyContantsData,
      buttons: []
    };
    buttonData?.getInputData?.map((val: any) => {
      body?.buttons.push(
        {
          type: 'postback',
          title: val?.value,
          payload: val?.value,
        }
      );
    });
    return await axios.post(process.env.postbackButtonInsta, body);
  } catch (error) {
    logger.error('sendImageCustomerInsta err:', error);
  }
}

// send Button
export async function sendFlowBreakInpuRemaindertInsta(req: any) {
  try {
    const body = {
      domainId: req?.domainId,
      customerId: req?.customerInstagramId,
      pageId: req?.agentInstagramId,
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
    return await axios.post(process.env.postbackButtonInsta, body);
  } catch (error) {
    logger.error('sendFlowBreakInputInsta err:', error);
  }
}

// send Button
export async function sendFlowBreakInputInsta(req: any) {
  try {
    const body = {
      domainId: req?.domainId,
      customerId: req?.customerInstagramId,
      pageId: req?.agentInstagramId,
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
    return await axios.post(process.env.postbackButtonInsta, body);
  } catch (error) {
    logger.error('sendFlowBreakInputInsta err:', error);
  }
}

// send Button
export async function sendCallbackRequestInsta(req: any, buttonLink: any) {
  try {
    const body = {
      domainId: req?.domainId,
      customerId: req?.customerInstagramId,
      pageId: req?.agentInstagramId,
      postbackText: messageValue?.agentBusy,
      buttons: [
        {
          type: 'web_url',
          url: `${process.env.API_URL}/${buttonLink}`,
          title: 'callback',
          webview_height_ratio: 'full'
        }
      ]
    };
    return await axios.post(process.env.postbackButtonInsta, body);
  } catch (error) {
    logger.error('sendCallbackRequestInsta err:', error);
  }
}

// send end chat
export async function getDispositinDisconnectStatusInsta(data: any) {
  try {
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerInstagramId,
      pageId: data?.agentInstagramId,
      type: 'text',
      message: messageValue?.endChat
    };
    await axios.post(process.env.InstagramOut, body);
    return body;
  } catch (error) {
    logger.error('getDispositinDisconnectStatusInsta err:', error);
  }
}

// send message connect live agent
export async function sendConnectLiveAgentInsta(data: any) {
  try {
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerInstagramId,
      pageId: data?.agentInstagramId,
      type: 'text',
      message: messageValue?.liveAgent
    };
    await axios.post(process?.env?.InstagramOut, body);
    return body;
  } catch (error) {
    logger.error('sendConnectLiveAgentInsta err:', error);
  }
}

// send Transfer Agent Insta
export async function sendTransferAgentInsta(data: any) {
  try {
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerInstagramId,
      pageId: data?.agentInstagramId,
      type: 'text',
      message: messageValue?.anotherAgent
    };
    await axios.post(process?.env?.InstagramOut, body);
    return body;
  } catch (error) {
    logger.error('sendTransferAgentInsta err:', error);
  }
}

// send message
export async function sendMessageInsta(req: any) {
  try {
    const body = {
      domainId: req?.domainId,
      customerId: req?.customerInstagramId,
      pageId: req?.agentInstagramId,
      message: messageValue?.agentBusy
    };
    await axios.post(process?.env?.InstagramOut, body);
    return body;
  } catch (error) {
    logger.error('sendMessageInsta err:', error);
  }
}

// send booking details
export async function sendAppoinmentPaymentInsta(data: any) {
  try {
    const booking = JSON.parse(data?.p_book_form);
    const formattedLastName = booking[1]['Last name'] ? `${booking[1]['Last name']}` : '-';
    const formatNumber = booking[2]['Phone number'] !== null ? `${booking[2]['Phone number']}` : '-';
    let content: any = '';
    if (data?.p_app_type === 4) {
      content = `Thank You! Your appointment has been booked successfully.\n\n Appointment ID\n ${data?.app_booked_id}\n\n Booking date\n ${data?.p_book_date}\n\n Timing\n ${data?.p_book_starttime} - ${data?.p_book_endtime}\n\n Service\n ${data?.service_name}\n\n Name\n ${booking[0]['First name']} ${formattedLastName}\n\n Email\n ${booking[3]?.Email}\n\n Phone\n ${formatNumber}\n\n`;
    } else {
      content = `Thank You! Your appointment has been booked successfully.\n\n Appointment ID\n ${data?.app_booked_id}\n\n Booking date\n ${data?.p_book_date}\n\n Timing\n ${data?.p_book_starttime} - ${data?.p_book_endtime}\n\n Name\n ${booking[0]['First name']} ${formattedLastName}\n\n Email\n ${booking[3]?.Email}\n\n Phone\n ${formatNumber}\n\n`;
    }
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerNumber,
      pageId: data?.calledNumber,
      type: 'text',
      message: content
    };
    await axios.post(process?.env?.InstagramOut, body);
    return body;
  } catch (error) {
    logger.error('sendAppoinmentPaymentInsta err:', error);
  }
}

// send booking details
export async function sendBookingDetailsInsta(data: any, getBooking: any) {
  try {
    const booking = JSON.parse(getBooking?.p_book_form);
    const formattedLastName = booking[1]['Last name'] ? `${booking[1]['Last name']}` : '';
    const formatNumber = booking[2]['Phone number'] !== null ? `${booking[2]['Phone number']}` : '-';
    const context = `Thank You! Your appointment has been booked successfully.\n\n Appointment ID\n ${getBooking?.app_booked_id}\n\n Booking date\n ${getBooking?.p_book_date}\n\n Timing\n ${getBooking?.p_book_starttime} - ${getBooking?.p_book_endtime}\n\n Service\n ${getBooking?.service}\n\n Name\n ${booking[0]['First name']} ${formattedLastName}\n\n Email\n ${booking[3]?.Email}\n\n Phone\n ${formatNumber}\n\n`;
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerInstagramId,
      pageId: data?.agentInstagramId,
      type: 'text',
      message:context
    };
    await axios.post(process?.env?.InstagramOut, body);
    updateMessageFlowAppoinmentInsta(data, context);
    return body;
  } catch (error) {
    logger.error('sendBookingDetailsInsta err', error);
  }
}

// send booking details
export async function sendBookingDetailsOneToOneInsta(data: any, getBooking: any) {
  try {
    const booking = JSON.parse(getBooking?.p_book_form);
    const formattedLastName = booking[1]['Last name'] ? `${booking[1]['Last name']}` : '';
    const formatNumber = booking[2]['Phone number'] !== null ? `${booking[2]['Phone number']}` : '-';
    const context = `Thank You! Your appointment has been booked successfully.\n\n Appointment ID\n ${getBooking?.app_booked_id}\n\n Booking date\n ${getBooking?.p_book_date}\n\n Timing\n ${getBooking?.p_book_starttime} - ${getBooking?.p_book_endtime}\n\n Name\n ${booking[0]['First name']} ${formattedLastName}\n\n Email\n ${booking[3]?.Email}\n\n Phone\n ${formatNumber}\n\n`;
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerInstagramId,
      pageId: data?.agentInstagramId,
      type: 'text',
      message: context
    };
    await axios.post(process?.env?.InstagramOut, body);
    updateMessageFlowAppoinment(data, context);
    return body;
  } catch (error) {
    logger.error('sendBookingDetailsOneToOneInsta err', error);
  }
}

/**
 *
 * @param data
 * @param dateObjects
 * @returns
 */
export async function sendAppointmentDateInsta(data: any, dateObjects: any) {
  try {
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerInstagramId,
      pageId: data?.agentInstagramId,
      bottonTitle: appoinment?.SelectDate,
      buttons: dateObjects
    };
    await axios.post(process?.env?.sendFeedbackFormInsta, body);
    updateMessageFlowAppoinmentInsta(data, body?.bottonTitle);
    return body;
  } catch (error) {
    logger.error('sendAppointmentDateInsta err', error);
  }
}

// send categories
export async function sendCategoryNameInsta(data: any, categoryName: any) {
  try {
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerInstagramId,
      pageId: data?.agentInstagramId,
      bottonTitle: appoinment?.SelectCategory,
      buttons: categoryName
    };
    await axios.post(process?.env?.sendFeedbackFormInsta, body);
    updateMessageFlowAppoinmentInsta(data, body?.bottonTitle);
    return body;
  } catch (error) {
    logger.error('sendCategoryNameInsta err', error);
  }
}

// send service
export async function sendAppointmentResInsta(data: any, serviceNames: any, lable: any) {
  try {
    const interactive_list: any = [];
    serviceNames.forEach((item: any) => {
      interactive_list.push({ content_type: 'text', title: item?.service_name, payload: item?.serviceid });
    });
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerInstagramId,
      pageId: data?.agentInstagramId,
      bottonTitle: lable?.label_name1?.split('_')?.join(' '),
      buttons: interactive_list
    };
    await axios.post(process?.env?.sendFeedbackFormInsta, body);
    updateMessageFlowAppoinmentInsta(data, body?.bottonTitle);
    return body;
  } catch (error) {
    logger.error('sendAppointmentResInsta err', error);
  }
}

// send all userName
export async function sendAllUserNameInsta(data: any, getUser: any) {
  logger.info('Select Consultant', getUser);
  try {
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerInstagramId,
      pageId: data?.agentInstagramId,
      bottonTitle: appoinment?.SelectConsultant,
      buttons: getUser
    };
    await axios.post(process?.env?.sendFeedbackFormInsta, body);
    updateMessageFlowAppoinmentInsta(data, body?.bottonTitle);
    return body;
  } catch (error) {
    logger.error('sendAllUserNameInsta err', error);
  }
}

// send Interactive one to one
export async function sendAppointmentOneToOneInsta(data: any, dateObjects: any, lable: any) {
  logger.info('Select mins', dateObjects);
  try {
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerInstagramId,
      pageId: data?.agentInstagramId,
      bottonTitle: lable?.label_name9?.split('_')?.join(' '),
      buttons: dateObjects
    };
    await axios.post(process?.env?.sendFeedbackFormInsta, body);
    updateMessageFlowAppoinmentInsta(data, body?.bottonTitle);
    return body;
  } catch (error) {
    logger.error('sendAppointmentOneToOneInsta err', error);
  }
}

// send Interactive duration
export async function sendAppointmentDurationInsta(data: any, dateObjects: any, lable: any) {
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
      customerId: data?.customerInstagramId,
      pageId: data?.agentInstagramId,
      bottonTitle: lable?.label_name9?.split('_')?.join(' '),
      buttons: result
    };
    await axios.post(process?.env?.sendFeedbackFormInsta, body);
    updateMessageFlowAppoinmentInsta(data, body?.bottonTitle);
    return body;
  } catch (error) {
    logger.error('sendAppointmentDurationInsta err', error);
  }
}

// bookingDuration
export async function sendbookingDurationInsta(data: any, getDuration: any, lable: any) {
  try {
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerInstagramId,
      pageId: data?.agentInstagramId,
      bottonTitle: lable?.label_name11?.split('_')?.join(' '),
      buttons: getDuration
    };
    await axios.post(process?.env?.sendFeedbackFormInsta, body);
    updateMessageFlowAppoinmentInsta(data, body?.bottonTitle);
    return body;
  } catch (error) {
    logger.error('sendbookingDurationInsta err', error);
  }
}

export async function sendButtonInsta(req: any,links: any, plainText: string) {
  try {
    if (!Array.isArray(links) || links?.length === 0) return;
    const linkChunks = chunkArray(links, 3);
    for (let i = 0; i < linkChunks?.length; i++) {
      const chunk = linkChunks[i];
      const buttons = chunk.map((item) => ({
        type: 'web_url',
        url: item?.href,
        title: item?.text?.substring(0, 20),
        webview_height_ratio: 'full'
      }));
      const body = {
        domainId: req?.domainId,
        customerId: req?.customerInstagramId,
        pageId: req?.agentInstagramId,
        postbackText: i === 0 ? plainText : '',
        buttons
      };
      await axios.post(process.env.postbackButtonInsta!, body);
    }
  } catch (error) {
    logger.error('sendButtonInsta err:', error);
  }
}

// send instagram 
export async function sendInstaPdf(data: any, doc: any,message:any) {
  try {
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerInstagramId,
      pageId: data?.agentInstagramId,
      postbackText: message,
      buttons: [
        {
          type: 'web_url',
          url: doc?.mediaUri,
          title: 'Cake Menu',
          webview_height_ratio: 'full'
        }
      ]
    };
    const response = await axios.post(process.env.postbackButtonInsta, body);
    return response;
  } catch (error) {
    logger.error('sendPaymentInsta err:', error);
  }
}