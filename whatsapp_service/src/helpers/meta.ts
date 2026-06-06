import axios from 'axios';
import fs from 'fs';
import { getBotDetail, getBotDetailWhatsapp, getCompanyName, getconnectData, Satification } from '../dao/whatsapp';
import { sendMessageCustomerAppointment } from '../modules/whatsapp/handler';
import { logger } from '../plugins/log';
import { appoinment, messageValue } from './constants';
import { contextMatch } from './utils';

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
    const getId = await getBotDetail(data);
    const body = {
      content,
      id: getId[0]?.purchasedat,
      from: data?.calledNumber,
      to: `+${data?.customerNumber}`,
    };
    await axios.post(process?.env?.whatsappOut, body);
    return body;
  } catch (error) {
    logger.error('sendAppoinmentPayment err:', error);
  }
}

// send message
export async function sendMessage(req: any) {
  try {
    const getId = await getBotDetail(req);
    const body = {
      id: getId[0]?.purchasedat,
      from: req?.calledNumber,
      to: `+${req?.customerNumber}`,
      content: messageValue?.agentBusy
    };
    logger.info('send RouteMobile', body);
    const response = await axios.post(process.env.whatsappOut, body);
    return body;
  } catch (error) {
    logger.error('Error sending message:', error);
  }
}

// send Message Customer
export async function sendMessageCustomerFlow(req: any, message: any) {
  try {
    let msg = message;
    const context = contextMatch(msg);
    if (context?.length > 0) {
      const connect: any = await getconnectData(req);
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
    const getId = await getBotDetail(req);
    const body = {
      id: getId[0]?.purchasedat,
      from: req?.calledNumber,
      to: `+${req?.customerNumber}`,
      content: msg?.replace(/<[^>]+>/g, '')
    };
    logger.info('send RouteMobile', body);
    await axios.post(process?.env?.whatsappOut, body);
    return body;
  } catch (error) {
    logger.error('sendMessageCustomerFlow err:', error);
  }
}

/* tslint:disable */
// send Message Customer
export async function sendMessageCustomer(req: any, message: any) {
  try {
    let msg = message;
    msg = msg.replace(/<p[^>]*>/g, '').replace(/<\/p>/g, '\n\n').replace(/\s+/g, ' ').trim();
    if (msg.includes('<ul>') && msg.includes('</ul>')) {
      msg = msg.replace(/<ul>/g, '\n').replace(/<\/ul>/g, '');
      msg = msg.replace(/<li>\s*/g, '• ').replace(/<\/li>/g, '\n');
    }
    let counter = 1;
    msg = msg.replace(/<ol>/g, '\n').replace(/<\/ol>/g, '');
    msg = msg.replace(/<li>\s*/g, () => `${counter++}. `).replace(/<\/li>/g, '\n');
    msg = msg.replace(/<b>/g, '').replace(/<\/b>/g, '');
    msg = msg.replace(/<strong>/g, '').replace(/<\/strong>/g, '');
    msg = msg.replace(/<em>/g, '_').replace(/<\/em>/g, '_');
    msg = msg.replace(/<s>/g, '~').replace(/<\/s>/g, '~');
    msg = msg.replace(/<a[^>]*>(.*?)<\/a>/g, '$1');
    msg = msg.replace(/\n\s*\n/g, '\n').trim();
    const context = contextMatch(msg);
    if (context?.length > 0) {
      const connect: any = await getconnectData(req);
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
    const getId = await getBotDetail(req);
    const body = {
      id: getId[0]?.purchasedat,
      from: req?.calledNumber,
      to: `+${req?.customerNumber}`,
      content: msg?.replace(/<[^>]+>/g, '')
    };
    logger.info('send RouteMobile', body);
    await axios.post(process?.env?.whatsappOut, body);
    return body;
  } catch (error) {
    logger.error('sendMessageCustomer err:', error);
  }
}

/* tslint:disable */
// send Message Customer CAI
export async function sendMessageCustomerCai(req: any, message: any) {
  try {
    let msg = message;
    // --- Handle <br> → newline
    msg = msg.replace(/<br\s*\/?>/gi, '\n');
    // --- Replace bold first
    msg = msg.replace(/<b>/gi, '*').replace(/<\/b>/gi, '*');
    msg = msg.replace(/<strong>/gi, '*').replace(/<\/strong>/gi, '*');
    // --- Paragraphs
    msg = msg.replace(/<p[^>]*>/gi, '').replace(/<\/p>/gi, '\n\n').replace(/\s+/g, ' ').trim();
    // --- Unordered lists
    if (msg.includes('<ul>') && msg.includes('</ul>')) {
      msg = msg.replace(/<ul>/gi, '\n').replace(/<\/ul>/gi, '');
      msg = msg.replace(/<li>\s*/gi, '• ').replace(/<\/li>/gi, '\n');
    }
    // --- Ordered lists
    let counter = 1;
    msg = msg.replace(/<ol>/gi, '\n').replace(/<\/ol>/gi, '');
    msg = msg.replace(/<li>\s*/gi, () => `${counter++}. `).replace(/<\/li>/gi, '\n');
    // --- Italic
    msg = msg.replace(/<em>/gi, '_').replace(/<\/em>/gi, '_');
    // --- Strikethrough
    msg = msg.replace(/<s>/gi, '~').replace(/<\/s>/gi, '~');
    // --- Links: keep text only
    msg = msg.replace(/<a[^>]*>(.*?)<\/a>/gi, '$1');
    // --- Clean remaining newlines/spaces
    msg = msg.replace(/\n\s*\n/g, '\n').trim();
    const getId = await getBotDetail(req);
    const body = {
      id: getId[0]?.purchasedat,
      from: req?.calledNumber,
      to: `+${req?.customerNumber}`,
      content: msg
    };
    logger.info('send RouteMobile', body);
    const response = await axios.post(process?.env?.whatsappOut, body);
    logger.info('response RouteMobile', response?.data);
    return response;
  } catch (error) {
    logger.error('sendMessageCustomerCai err:', error);
  }
}

// send customer url
export async function sendMessageCustomerUrl(data: any, message: any) {
  try {
    const getId = await getBotDetailWhatsapp(data);
    const body = {
      id: getId[0]?.purchasedat,
      from: data?.agentNumber,
      to: `+${data?.from}`,
      type: "text",
      content: message
    };
    logger.info('send RouteMobile', body);
    const response = await axios.post(process?.env?.WHATSAPPSENDURL, body);
    return response;
  } catch (error) {
    logger.error('sendMessageSatisfication err:', error);
  }
}


// send customer satisfications
export async function sendMessageSatisfication(data: any, message: any) {
  try {
    const getId = await getBotDetailWhatsapp(data);
    const body = {
      id: getId[0]?.purchasedat,
      from: data?.agentNumber,
      to: `+${data?.from}`,
      content: message[0]?.thanksNote
    };
    logger.info('send RouteMobile', body);
    const response = await axios.post(process?.env?.whatsappOut, body);
    return body;
  } catch (error) {
    logger.error('sendMessageSatisfication err:', error);
  }
}

// send messsage callbacks
export async function sendMessageCustomerCallbacks(req: any) {
  try {
    const getId = await getBotDetail(req);
    const body = {
      id: getId[0]?.purchasedat,
      from: req?.calledNumber,
      to: `+${req?.customerNumber}`,
      content: messageValue?.looksLike
    };
    logger.info('send RouteMobile', body);
    await axios.post(process?.env?.whatsappOut, body);
    return body;
  } catch (error) {
    logger.error('sendMessageCustomerCallbacks err:', error);
  }
}

// send Image Customer
export async function sendImageCustomer(req: any, message: any, filesData: any) {
  try {
    const getId = await getBotDetail(req);
    const body = {
      id: getId[0].purchasedat,
      from: `${req?.calledNumber}`,
      to: `+${req?.customerNumber}`,
      content: message ? message?.replace(/<[^>]+>/g, '') : '',
      mediaName: filesData?.mediaName ?? '',
      mediaUri: filesData?.mediaUri,
      mimeType: filesData?.mimeType,
    };
    logger.info("send meta", body);
    const response = await axios.post(process?.env?.sendMedia, body);
    return response;
  } catch (error) {
    logger.error("sendImageCustomer err:", error);
    throw error;
  }
}

// send Image Customer
export async function sendMediaCustomer(req: any, message: any, filesData: any, mimeData: any) {
  try {
    const getId = await getBotDetail(req);
    const body = {
      id: getId[0]?.purchasedat,
      from: (req?.calledNumber).toString(),
      to: `+${req?.customerNumber}`,
      content: message?.replace(/<[^>]+>/g, ''),
      mediaName: filesData?.name,
      mediaUri: filesData?.url,
      mimeType: mimeData
    };
    logger.info('send RouteMobile', body);
    await axios.post(process?.env?.sendMedia, body);
    return body;
  } catch (error) {
    logger.error('sendMediaCustomer err:', error);
  }
}

// send Button
export async function sendCustomerInput(req: any, buttonData: any) {
  try {
    const bodyContants = buttonData?.message;
    const bodyContantsData = bodyContants?.replace(/<\/*s>/g, '~').replace(/<\/*strong>/g, '*').replace(/<\/*em>/g, '_').replace(/<div>/g, '').replace(/<\/div>|<br>/g, '\n').replace(/<[^>]*>/g, '').replace(/&nbsp;/g, ' ');
    const getId = await getBotDetail(req);
    const body = {
      id: getId[0]?.purchasedat,
      from: req?.calledNumber,
      to: `+${req?.customerNumber}`,
      header: { text: buttonData?.header },
      content: bodyContantsData,
      footer: buttonData?.footer,
      button1: buttonData?.getInputData[0]?.value,
      button2: buttonData?.getInputData[1]?.value,
      button3: buttonData?.getInputData[2]?.value,
      type: 'quickReply'
    };
    logger.info('send RouteMobile', body);
    return await axios.post(process.env.whatsappOutQuickReplyBot, body);
  } catch (error) {
    logger.error('sendCustomerInput err:', error);
  }
}

// send Button
export async function sendFlowBreakInput(req: any) {
  try {
    const getId = await getBotDetail(req);
    const body = {
      id: getId[0]?.purchasedat,
      from: req?.calledNumber,
      to: `+${req?.customerNumber}`,
      header: { text: 'confirmations' },
      content: messageValue?.disconnectFlow,
      footer: '',
      button1: 'Yes',
      button2: 'No',
      id1: '100',
      id2: '200',
      type: 'quickReply'
    };
    logger.info('send RouteMobile', body);
    return await axios.post(process.env.whatsappOutQuickReplyBot, body);
  } catch (error) {
    logger.error('sendFlowBreakInput err:', error);
  }
}

// send Button
export async function sendFlowBreakInputRemainder(req: any) {
  try {
    const getId = await getBotDetail(req);
    const body = {
      id: getId[0]?.purchasedat,
      from: req?.calledNumber,
      to: `+${req?.customerNumber}`,
      header: { text: 'Remainder confirmations' },
      content: messageValue?.continueFlow,
      footer: '',
      button1: 'Yes',
      button2: 'No',
      id1: '10001',
      id2: '20002',
      type: 'quickReply'
    };
    logger.info('send RouteMobile', body);
    return await axios.post(process.env.whatsappOutQuickReplyBot, body);
  } catch (error) {
    logger.error('sendFlowBreakInputRemainder err:', error);
  }
}

// send Button Link
export async function sendButtonLink(req: any, link: any) {
  try {
    const getId = await getBotDetail(req);
    const body = {
      id: getId[0]?.purchasedat,
      from: req?.calledNumber,
      to: `+${req?.customerNumber}`,
      templateName: link?.templateName,
      type: 'call_to_action_static',
      buttonUrl: '/',
    };
    logger.info('send RouteMobile', body);
    await axios.post(process?.env?.sendTemplate, body);
    return body;
  } catch (error) {
    logger.error('sendButtonLink err:', error);
  }
}

// send message connect live agent
export async function sendConnectLiveAgent(req: any) {
  try {
    const getId = await getBotDetail(req);
    const body = {
      id: getId[0]?.purchasedat,
      from: req?.calledNumber,
      to: `+${req?.customerNumber}`,
      content: messageValue?.liveAgent
    };
    const response = await axios.post(process.env.whatsappOut, body);
    logger.info('response:', response);
    return body;
  } catch (error) {
    logger.error('sendConnectLiveAgent err:', error);
  }
}

// send message Transfer agent
export async function sendTransferAgent(req: any) {
  try {
    const getId = await getBotDetail(req);
    const body = {
      id: getId[0]?.purchasedat,
      from: req?.calledNumber,
      to: `+${req?.customerNumber}`,
      content: messageValue?.transferred
    };
    const response = await axios.post(process.env.whatsappOut, body);
    logger.info('response:', response);
    return body;
  } catch (error) {
    logger.error('sendTransferAgent err:', error);
  }
}

// send end chat
export async function getDispositinDisconnectStatus(req: any) {
  try {
    const getId = await getBotDetail(req);
    const body = {
      id: getId[0]?.purchasedat,
      from: req?.calledNumber,
      to: `+${req?.customerNumber}`,
      content: messageValue?.endChat
    };
    const response = await axios.post(process.env.whatsappOut, body);
    logger.info('response:', response);
    return body;
  } catch (error) {
    logger.error('getDispositinDisconnectStatus err:', error);
  }
}

// send Message Customer
export async function sendMessageNumber(req: any, message: any) {
  try {
    const bodyContantsData = message?.replace(/<\/*s>/g, '~').replace(/<\/*strong>/g, '*').replace(/<\/*em>/g, '_').replace(/<div>/g, '').replace(/<\/div>|<br>/g, '\n').replace(/<[^>]*>/g, '').replace(/&nbsp;/g, ' ');
    const getId = await getBotDetail(req);
    const body = {
      id: getId[0]?.purchasedat,
      from: req?.calledNumber,
      to: `+${req?.customerNumber}`,
      content: bodyContantsData
    };
    const response = await axios.post(process?.env?.whatsappOut, body);
    logger.info('response:', response);
    return body;
  } catch (error) {
    logger.error('sendMessageNumber err:', error);
  }
}

// send Catalogues
export async function sendCatalogues(req: any, getNlpData: any, customerName: any) {
  try {
    const getId = await getBotDetail(req);
    const getName: any = await getCompanyName(req);
    const getProduct: any = Object.entries(getNlpData?.ecommerce_product);
    const updatedProductItems = getProduct[0][1].map((Items: any) => ({
      product_retailer_id: Items
    }));
    const mergedProductItems = [...updatedProductItems];
    const getRecommended: any = Object.values(getNlpData?.recomended_product).reduce((acc: any, uuidArray: any) => acc.concat(uuidArray), []);
    const updatedRecommendedItems = getRecommended.map((Items: any) => ({
      product_retailer_id: Items
    }));
    const mergedRecommendedItems = [...updatedRecommendedItems];
    const body = {
      id: getId[0]?.purchasedat,
      from: req?.calledNumber,
      to: `+${req?.customerNumber}`,
      header: `Tap into the ${getName?.companyName} shop Catalog on WhatsApp`,
      body: `Hi ${customerName}.Welcome to ${getName?.companyName}. Begin your shopping experience by exploring our catalog and adding your desired items to your cart.`,
      sections: [
        {
          title: 'Products',
          product_items: mergedProductItems
        },
        {
          title: 'Recommended',
          product_items: mergedRecommendedItems
        }
      ]
    };
    await axios.post(process?.env?.sendCatalog, body);
    return body;
  } catch (error) {
    logger.error('sendCatalogues err:', error);
  }
}

// send Button Link
export async function sendButtonLinkPayNow(req: any, PayNowUrl: any) {
  try {
    const getId = await getBotDetail(req);
    const body = {
      id: getId[0]?.purchasedat,
      from: req?.calledNumber,
      to: `+${req?.customerNumber}`,
      type: 'sendPaymentTemplate',
      buttonUrl: PayNowUrl,
    };
    logger.info('sendButtonLinkPayNow err:', body);
    await axios.post(process?.env?.sendTemplate, body);
    return body;
  } catch (error) {
    logger.error('sendButtonLinkPayNow err:', error);
  }
}

// send Button Link for callback
export async function sendCallbackRequest(req: any, callbackUrl: any) {
  try {
    const getId = await getBotDetail(req);
    const body = {
      id: getId[0]?.purchasedat,
      from: req?.calledNumber,
      to: `+${req?.customerNumber}`,
      templateName: process.env.templateNameCallBack,
      type: 'cta_template_body_buttons',
      buttonUrl: callbackUrl,
    };
    await axios.post(process?.env?.sendCallbackRequest, body);
    return body;
  } catch (error) {
    logger.error('sendCallbackRequest err:', error);
  }
}

// send Interactive List
export async function sendAppointmentRes(req: any, serviceNames: any, lable: any) {
  try {
    let interactive_list: any = [];
    serviceNames.forEach((item: any) => {
      interactive_list.push({ id: item?.serviceid, title: item?.service_name })
    });
    const body = {
      type: "interactive_list",
      from: req?.calledNumber.toString(),
      to: `+${req?.customerNumber}`,
      bodyContent: messageValue?.interested,
      buttonText: lable?.label_name1?.split('_')?.join(' '),
      button: [
        {
          section_title: lable?.label_name1?.split('_')?.join(' '),
          row: interactive_list
        }
      ]
    };
    const getId = await getBotDetail(req);
    if (getId[0]?.purchasedat === 1) {
      await axios.post(process?.env?.sendInteractive, body);
    } else {
      await axios.post(process?.env?.sendInteractiveMeta, body);
    }
    sendMessageCustomerAppointment(req, body?.bodyContent);
    return body;
  } catch (error) {
    logger.error('sendAppointmentRes err', error);
  }
}

// send categories
export async function sendCategoryName(req: any, categoryName: any) {
  try {
    const body = {
      type: "interactive_list",
      from: req?.calledNumber.toString(),
      to: `+${req?.customerNumber}`,
      bodyContent: appoinment?.category,
      buttonText: appoinment?.SelectCategory,
      button: [
        {
          section_title: appoinment?.SelectCategory,
          row: categoryName
        }
      ]
    };
    const getId = await getBotDetail(req);
    if (getId[0]?.purchasedat === 1) {
      await axios.post(process?.env?.sendInteractive, body);
    } else {
      await axios.post(process?.env?.sendInteractiveMeta, body);
    }
    sendMessageCustomerAppointment(req, body?.bodyContent);
    return body;
  } catch (error) {
    logger.error('sendCategoryName err:', error);
  }
}

// send Interactive date
export async function sendAppointmentDate(req: any, dateObjects: any) {
  try {
    const body = {
      type: "interactive_list",
      from: req?.calledNumber.toString(),
      to: `+${req?.customerNumber}`,
      bodyContent: appoinment?.date,
      buttonText: appoinment?.SelectDate,
      button: [
        {
          section_title: appoinment?.SelectDate,
          row: dateObjects
        }
      ]
    };
    const getId = await getBotDetail(req);
    if (getId[0]?.purchasedat === 1) {
      await axios.post(process?.env?.sendInteractive, body);
    } else {
      await axios.post(process?.env?.sendInteractiveMeta, body);
    }
    sendMessageCustomerAppointment(req, body?.bodyContent);
    return body;
  } catch (error) {
    logger.error('sendAppointmentDate err:', error);
  }
}

// send Interactive duration
export async function sendAppointmentDuration(req: any, dateObjects: any, lable: any) {
  const durationCollected = JSON.parse(dateObjects);
  const result = durationCollected.map((item: any, index: any) => ({
    id: index,
    title: item.type === 1 ? `${+(item.duration)} mins` : `${+(item.duration / 60)} hours`
  }));
  logger.info("Select mins", result);
  try {
    const body = {
      type: "interactive_list",
      from: req?.calledNumber.toString(),
      to: `+${req?.customerNumber}`,
      bodyContent: appoinment?.duration,
      buttonText: lable?.label_name9?.split('_')?.join(' '),
      button: [
        {
          section_title: lable?.label_name9?.split('_')?.join(' '),
          row: result
        }
      ]
    };
    const getId = await getBotDetail(req);
    if (getId[0]?.purchasedat === 1) {
      await axios.post(process?.env?.sendInteractive, body);
    } else {
      await axios.post(process?.env?.sendInteractiveMeta, body);
    }
    sendMessageCustomerAppointment(req, body?.bodyContent);
    return body;
  } catch (error) {
    console.error('sendAppointmentDuration err:', error);
  }
}

// send Interactive one to one
export async function sendAppointmentOneToOne(req: any, dateObjects: any, lable: any) {
  logger.info("Select mins", dateObjects);
  try {
    const body = {
      type: "interactive_list",
      from: req?.calledNumber.toString(),
      to: `+${req?.customerNumber}`,
      bodyContent: appoinment?.duration,
      buttonText: lable?.label_name9?.split('_')?.join(' '),
      button: [
        {
          section_title: lable?.label_name9?.split('_')?.join(' '),
          row: dateObjects
        }
      ]
    };
    const getId = await getBotDetail(req);
    if (getId[0]?.purchasedat === 1) {
      await axios.post(process?.env?.sendInteractive, body);
    } else {
      await axios.post(process?.env?.sendInteractiveMeta, body);
    }
    sendMessageCustomerAppointment(req, body?.bodyContent);
    return body;
  } catch (error) {
    console.error('sendAppointmentOneToOne err:', error);
  }
}

// bookingDuration
export async function sendbookingDuration(req: any, getDuration: any, lable: any) {
  try {
    const body = {
      type: "interactive_list",
      from: req?.calledNumber.toString(),
      to: `+${req?.customerNumber}`,
      bodyContent: appoinment?.booking,
      buttonText: lable?.label_name11?.split('_')?.join(' '),
      button: [
        {
          section_title: lable?.label_name11?.split('_')?.join(' '),
          row: getDuration
        }
      ]
    };
    const getId = await getBotDetail(req);
    if (getId[0]?.purchasedat === 1) {
      await axios.post(process?.env?.sendInteractive, body);
    } else {
      await axios.post(process?.env?.sendInteractiveMeta, body);
    }
    sendMessageCustomerAppointment(req, body?.bodyContent);
    return body;
  } catch (error) {
    logger.error('sendbookingDuration err:', error);
  }
}

// send booking details
export async function sendBookingDetails(req: any, getBooking: any) {
  try {
    const booking = JSON.parse(getBooking?.p_book_form);
    const formattedLastName = booking[1]['Last name'] ? `*${booking[1]['Last name']}*` : '';
    const formatNumber = booking[2]["Phone number"] !== null ? `*${booking[2]["Phone number"]}*` : '-'
    const getId = await getBotDetail(req);
    let confirmMsg = `Thank You! Your appointment has been booked successfully.\n\n Appointment ID\n *${getBooking?.app_booked_id}*\n\n Booking date\n *${getBooking?.p_book_date}*\n\n Timing\n *${getBooking?.p_book_starttime}* - *${getBooking?.p_book_endtime}*\n\n Name\n *${booking[0]['First name']}* ${formattedLastName}\n\n Email\n *${booking[3]?.Email ?? '-'}*\n\n Phone\n ${formatNumber}\n\n`
    const body = {
      id: getId[0]?.purchasedat,
      from: req?.calledNumber,
      to: `+${req?.customerNumber}`,
      content: confirmMsg
    };
    await axios.post(process?.env?.whatsappOut, body);
    sendMessageCustomerAppointment(req, confirmMsg);
    return body;
  } catch (error) {
    logger.error('sendBookingDetails err:', error);
  }
}

// send booking details
export async function sendBookingDetailsOneToOne(req: any, getBooking: any) {
  try {
    const booking = JSON.parse(getBooking?.p_book_form);
    const formattedLastName = booking[1]['Last name'] ? `*${booking[1]['Last name']}*` : '';
    const formatNumber = booking[2]["Phone number"] !== null ? `*${booking[2]["Phone number"]}*` : '-'
    const getId = await getBotDetail(req);
    let confirmMsg = `Thank You! Your appointment has been booked successfully.\n\n Appointment ID\n *${getBooking?.app_booked_id}*\n\n Booking date\n *${getBooking?.p_book_date}*\n\n Timing\n *${getBooking?.p_book_starttime}* - *${getBooking?.p_book_endtime}*\n\n Name\n *${booking[0]['First name']}* ${formattedLastName}\n\n Email\n *${booking[3]?.Email ?? '-'}*\n\n Phone\n ${formatNumber}\n\n`
    const body = {
      id: getId[0]?.purchasedat,
      from: req?.calledNumber,
      to: `+${req?.customerNumber}`,
      content: confirmMsg
    };
    await axios.post(process?.env?.whatsappOut, body);
    sendMessageCustomerAppointment(req, confirmMsg);
    return body;
  } catch (error) {
    logger.error('sendBookingDetailsOneToOne err:', error);
  }
}

// send all userName
export async function sendAllUserName(req: any, getUser: any) {
  logger.info("Select service", getUser);

  try {
    const body = {
      type: "interactive_list",
      from: req?.calledNumber.toString(),
      to: `+${req?.customerNumber}`,
      bodyContent: appoinment?.consultant,
      buttonText: appoinment?.SelectConsultant,
      button: [
        {
          section_title: appoinment?.SelectConsultant,
          row: getUser
        }
      ]
    };
    const getId = await getBotDetail(req);
    if (getId[0]?.purchasedat === 1) {
      await axios.post(process?.env?.sendInteractive, body);
    } else {
      await axios.post(process?.env?.sendInteractiveMeta, body);
    }
    sendMessageCustomerAppointment(req, body?.bodyContent);
    return body;
  } catch (error) {
    logger.error('sendAllUserName err:', error);
  }
}

// send dropdown message
export async function sendDropdownDetails(req: any, dropDownMessage: any, dropDownButton: any, dropDownMultipleData: any) {
  try {
    const body: any = {
      type: "interactive_list",
      from: req?.calledNumber.toString(),
      to: `+${req?.customerNumber}`,
      bodyContent: dropDownMessage?.replace(/<\/*s>/g, '~').replace(/<\/*strong>/g, '*').replace(/<\/*em>/g, '_').replace(/<div>/g, '').replace(/<\/div>|<br>/g, '\n').replace(/<[^>]*>/g, '').replace(/&nbsp;/g, ' '),
      buttonText: dropDownButton,
    };
    body.button = [{ "section_title": 1, "row": dropDownMultipleData }];
    const getId = await getBotDetail(req);
    if (getId[0]?.purchasedat === 1) {
      await axios.post(process?.env?.sendInteractive, body);
    } else {
      await axios.post(process?.env?.sendInteractiveMeta, body);
    } return body;
  } catch (error) {
    logger.error('sendDropdownDetails err:', error);
  }
}

//customer satification
export async function sendCustomerSatification(data: any) {
  try {
    let rating = []
    if (data?.ratingDesignType === 1 || data?.ratingDesignType === 2 || data?.ratingDesignType === 3) {
      rating = ["Very Bad", "Bad", "Satisfied", "Good", "Very Good"]
    } else if (data?.ratingDesignType === 4 || data?.ratingDesignType === 5) {
      rating = ["Very Bad", "Satisfied", "Very Good"]
    } else {
      rating = ["Bad", "Good"]
    }
    let interactive_list: any = [];
    rating.forEach((item: any, index: any) => {
      interactive_list.push({
        id: `${index}-${data?.HistoryId}-${data?.AgentId}-${data?.ratingDesignType}`,
        title: item,
      })
    });
    const body = {
      type: "interactive_list",
      from: data?.domainContact,
      to: `+${data?.customerContact}`,
      bodyContent: data?.caption,
      buttonText: appoinment?.SelectAnswer,
      button: [
        {
          section_title: appoinment?.SelectAnswer,
          row: interactive_list
        }
      ]
    };
    const getId = await Satification(data);
    if (getId[0]?.purchasedat === 1) {
      await axios.post(process?.env?.sendInteractive, body);
    } else {
      await axios.post(process?.env?.sendInteractiveMeta, body);
    }
    return body;
  } catch (error) {
    logger.error('sendCustomerSatification err:', error);
  }
}

export async function sendTyping(data: any) {
  try {
    const botDetails = await getBotDetailWhatsapp(data);
    const body = {
      id: botDetails[0]?.purchasedat,
      from: data.agentNumber,
      to: `+${data.from}`,
      message_id: data?.id
    };
    console.log("send typing meta", body)
    const response = await axios.post(process.env.typingIndicator, body);
    console.log("response typing mete", response.data)
    return response.data;
  } catch (error: any) {
    logger.error("sendTyping error:", error?.response?.data || error.message);
  }
}

export async function getWhatsAppMediaUrl(mediaId: any, data: any) {
  try {
    const body = {
      from: data?.agentNumber ?? data?.calledNumber,
      media_id: mediaId
    };
    console.log("send media url", body);
    const response: any = await axios.post(process.env.mediaDetails, body);
    console.log("response media url", response?.data?.response)
    return response?.data?.response || null;
  } catch (error: any) {
    logger.error("media error:", error?.response?.data || error.message);
    return null;
  }
}