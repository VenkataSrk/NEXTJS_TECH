import axios from 'axios';
import CryptoJS from 'crypto-js';
import { logger } from '../plugins/log';
import { sendPayment } from './fbmeta';
import { sendPaymentInsta } from './instameta';

// get appointment
export async function getAppointmentInfo(appointmentId: any, companyId: any) {
  try {
    const body = {
      p_app_id: appointmentId,
      p_company_id: companyId,
      p_bookingId: 0
    };
    const appointmentData: any = await axios.post(process?.env?.getAppointment, body);
    logger.info('appointmentData', appointmentData);
    if (appointmentData?.data?.status_code === 200) {
      return appointmentData?.data?.finalresponse[1];
    }
    return [];
  } catch (error) {
    logger.error('getAppointmentInfo err', error);
  }
}

// get appointment details
export async function AppointmentInfo(data: any) {
  try {
    const body = {
      p_app_id: data?.appId,
      p_company_id: data?.companyId
    };
    const appointmentData: any = await axios.post(process?.env?.getAppointment, body);
    logger.info('appointmentData', appointmentData);
    if (appointmentData?.data?.status_code === 200) {
      return appointmentData?.data?.finalresponse[0];
    }
    return [];

  } catch (error) {
    logger.error('getAppointmentInfo err', error);
  }
}

// get lable
export async function getAppointmentLable(companyId: any) {
  try {
    const body = {
      p_company_id: companyId
    };
    const appointmentLable: any = await axios.post(process?.env?.getLable, body);
    logger.info('getAppointmentLable', appointmentLable);
    if (appointmentLable?.data?.status_code === 200) {
      return appointmentLable?.data?.finalresponse[0];
    }
    return [];
  } catch (error) {
    logger.error('getAppointmentLable err', error);
  }
}

// get all user name
export async function getAppointmentUserName(appointment: any, id: any) {
  try {
    const body = {
      p_companyId: appointment?.companyId,
      p_serviceid: id?.serviceId
    };
    const appointmentUser: any = await axios.post(process?.env?.getUserName, body);
    logger.info('getAppointmentUserName', appointmentUser);
    if (appointmentUser?.data?.status_code === 200) {
      return appointmentUser?.data?.finalresponse;
    }
    return [];

  } catch (error) {
    logger.error('getAppointmentLable err', error);
  }
}

// get Consultant selection
export async function getConsultantData(appointment: any, id: any, getAppDetails: any) {
  try {
    const body = {
      p_company_id: appointment?.companyId,
      p_app_id: appointment?.appId,
      p_serviceid: id?.serviceId,
      p_consl_selection_id: getAppDetails?.consl_selection_id,
      p_consl_selection_logic_id: getAppDetails?.consl_selection_logic_id,
    };
    const constantsData: any = await axios.post(process?.env?.getConsultant, body);
    logger.info('constantsData', constantsData);
    if (constantsData?.data?.status_code === 200) {
      return constantsData?.data?.finalresponse;
    }
    return [];

  } catch (error) {
    logger.error('getConsultantData', error);
  }
}

// get appointment
export async function getdurationDetails(getAppointmentFlow: any) {
  try {
    const body = {
      p_app_id: getAppointmentFlow?.appId,
      p_company_id: getAppointmentFlow?.companyId
    };
    const appointmentData: any = await axios.post(process?.env?.getAppointment, body);
    logger.info('appointmentData', appointmentData);
    if (appointmentData?.data?.status_code === 200) {
      return appointmentData?.data?.finalresponse[1];
    }
    return [];

  } catch (error) {
    logger.error('getdurationDetails', error);
  }
}

// get calendar view
export async function getCalenderViewDetails(appointment: any, getAppDetails: any, DurationType: any, getAppointmentFlow: any) {
  try {
    let typeValue: any;
    if (
      getAppDetails[0]?.app_typeid === 1 ||
      getAppDetails[0]?.app_typeid === 2 ||
      getAppDetails[0]?.app_typeid === 3
    ) {
      const value = JSON.parse(getAppDetails[0]?.serv_dur_pricing || '[]');
      const filterValue = value?.filter((type: any) => {
        if (type?.scheduleHours === 1) {
          return +(type?.service) === +(appointment?.processDuration);
        }
        return +(type?.service) === +(appointment?.processDuration) * 60;

      });
      typeValue = filterValue?.[0]?.service ? +(filterValue[0]?.service) : null;
    } else {
      const duration = DurationType.filter((data: any) => data?.serviceid === +(getAppointmentFlow?.serviceId));
      const serviceIndex = duration.findIndex((item: any) => item.serviceid === +(getAppointmentFlow?.serviceId));
      const selectDuration = duration[0].amount_collected.split('|')[serviceIndex];
      const parseData = JSON.parse(selectDuration);
      const filterValue = parseData?.filter((type: any) => {
        if (type?.type === 1) {
          return +(type?.duration) === +(appointment?.processDuration);
        }
        return +(type?.duration) === +(appointment?.processDuration) * 60;

      });
      typeValue = filterValue[0]?.duration ? +(filterValue[0]?.duration) : null;
    }
    try {
      const body = {
        p_company_id: appointment?.companyId,
        p_user_id: appointment?.consultant,
        p_date: appointment?.processDate,
        p_duration: typeValue,
        p_booking_allow: getAppDetails?.[0]?.no_of_booking_slot,
        p_bufferTime: getAppDetails?.[0]?.buffer_time,
        p_rot_sch_typeid: getAppDetails[0]?.app_typeid === 3 ? getAppDetails[0]?.rot_sch_typeid : 0,
        p_app_id:getAppDetails[0]?.app_typeid !== 2 ? getAppDetails[0]?.app_id :''
        // p_app_id: getAppDetails[0]?.app_typeid === 3 ? getAppDetails[0]?.app_id : ''
      };
      let appointmentCal: any = [];
      const appTypeId = getAppDetails?.[0]?.app_typeid;
      let apiUrl = '';
      if (appTypeId === 1 || appTypeId === 4) {
        apiUrl = process?.env?.getCalenderView;
      } else if (appTypeId === 2) {
        apiUrl = process?.env?.getGroupCalenderView;
      } else if (appTypeId === 3) {
        apiUrl = process?.env?.getRotationalCalenderView;
      }
      appointmentCal = await axios.post(apiUrl, body);
      if (appointmentCal?.data?.status_code === 200) {
        if (appTypeId === 1 || appTypeId === 4) {
          return appointmentCal?.data?.finalresponse?.[1];
        } if (appTypeId === 2) {
          return appointmentCal?.data?.finalresponse;
        } if (appTypeId === 3) {
          return appointmentCal?.data?.finalresponse?.[2];
        }
      }
      return [];
    } catch (error) {
      logger.error('Error occurred while fetching appointment calendar:', error);
      return [];
    }
  } catch (err) {
    console.log(err);
  }
}

// get end time view
export async function getEndTimeData(appointment: any, getAppDetails: any, DurationType: any, getAppointmentFlow: any) {
  try {
    let typeValue: any;
    if (
      getAppDetails[0]?.app_typeid === 1 ||
      getAppDetails[0]?.app_typeid === 2 ||
      getAppDetails[0]?.app_typeid === 3
    ) {
      const value = JSON.parse(getAppDetails[0]?.serv_dur_pricing || '[]');
      const filterValue = value?.filter((type: any) => {
        if (type?.scheduleHours === 1) {
          return +(type?.service) === +(appointment?.processDuration);
        }
        return +(type?.service) === +(appointment?.processDuration) * 60;

      });
      typeValue = filterValue?.[0]?.service ? +(filterValue[0]?.service) : null;
    } else {
      const duration = DurationType.filter((data: any) => data?.serviceid === +(getAppointmentFlow?.serviceId));
      const serviceIndex = duration.findIndex((item: any) => item.serviceid === +(getAppointmentFlow?.serviceId));
      const selectDuration = duration[0].amount_collected.split('|')[serviceIndex];
      const parseData = JSON.parse(selectDuration);
      const filterValue = parseData?.filter((type: any) => {
        if (type?.type === 1) {
          return +(type?.duration) === +(appointment?.processDuration);
        }
        return +(type?.duration) === +(appointment?.processDuration) * 60;

      });
      typeValue = filterValue[0]?.duration ? +(filterValue[0]?.duration) : null;
    }
    const body = {
      startTime: appointment?.processTime,
      duration: typeValue,
      bufferTime: getAppDetails[0]?.buffer_time
    };
    const appointmentCal: any = await axios.post(process?.env?.slectEndTime, body);
    logger.info('appointmentCal', appointmentCal);
    if (appointmentCal?.data?.status_code === 200) {
      return appointmentCal?.data?.finalresponse[0];
    }
    return [];

  } catch (error) {
    logger.error('getEndTimeData err', error);
  }
}

// get appointment leave date
export async function getLeaveData(getAppointmentFlow: any) {
  try {
    const body = {
      p_companyid: getAppointmentFlow?.companyId,
      p_user_id: getAppointmentFlow?.userId
    };
    const appointmentData: any = await axios.post(process?.env?.getUserLeaveData, body);
    if (appointmentData?.data?.statusCode === 200) {
      return appointmentData?.data?.result[1];
    }
    return [];

  } catch (error) {
    logger.error('getLeaveData', error);
  }
}

// get appointment
export async function createBooking(jsonString: any, getAppointmentFlow: any, getAppDetails: any, EndTime: any, typeValue: any) {
  const jdata: any = [];
  jsonString.forEach((item: any) => {
    const key = Object.keys(item)[0]?.replace(/\s+/g, '_');
    jdata.push(key);
  });
  const keys = jdata?.join(', ');
  const jsrc: any = [];
  jsonString.forEach((item: any) => { jsrc.push(`"${(Object.values(item)[0])}"`); });
  const value = jsrc?.join(', ');
  try {
    const body = {
      p_company_id: getAppointmentFlow?.companyId,
      p_app_id: getAppointmentFlow?.appId,
      p_user_id: +(getAppointmentFlow?.consultant),
      p_book_date: getAppointmentFlow?.processDate,
      p_book_starttime: getAppointmentFlow?.processTime,
      p_book_endtime: EndTime?.substring(0, 5),
      p_book_duration: +(getAppointmentFlow?.processDuration),
      p_book_duration_type: typeValue === 1 ? 'mins' : 'hours',
      p_book_form: JSON.stringify(jsonString).replace(/name/g, 'name'),
      p_ispayment: 0,
      p_pay_amount: 0,
      p_payment_status: 'success',
      p_createdby: getAppointmentFlow?.userId,
      p_book_form_field: keys,
      p_book_form_value: value,
      p_payment_type: '',
      p_service_id: getAppointmentFlow?.serviceId,
      p_app_type: getAppDetails[0]?.app_typeid
    };
    const config: any = {
      method: 'post',
      url: getAppDetails[0]?.app_typeid === 2 ? process?.env?.createGroupBooking : process?.env?.createBooking,
      headers: { 'Content-Type': 'application/json' },
      data: body
    };
    const createBookingRes: any = await axios.request(config);
    if (createBookingRes?.data?.status_code === 200) {
      return createBookingRes?.data?.finalresponse;
    }
    return [];

  } catch (error) {
    logger.error('createBooking err', error);
  }
}

// get appointment
export async function bookingPayment(jsonString: any, getAppointmentFlow: any, getAppDetails: any, EndTime: any, typeValue: any, facebookCcaasTable: any, FilterPayment: any) {
  const jdata: any = [];
  jsonString.forEach((item: any) => {
    const key = Object.keys(item)[0]?.replace(/\s+/g, '_');
    jdata.push(key);
  });
  const keys = jdata?.join(', ');
  const jsrc: any = [];
  jsonString.forEach((item: any) => { jsrc.push(`"${(Object.values(item)[0])}"`); });
  const value = jsrc?.join(', ');
  try {
    const body = {
      p_company_id: getAppointmentFlow?.companyId,
      p_app_id: getAppointmentFlow?.appId,
      p_user_id: +(getAppointmentFlow?.consultant),
      p_book_date: getAppointmentFlow?.processDate,
      p_book_starttime: getAppointmentFlow?.processTime,
      p_book_endtime: EndTime?.substring(0, 5),
      p_book_duration: +(getAppointmentFlow?.processDuration),
      p_book_duration_type: typeValue === 1 ? 'mins' : 'hours',
      p_book_form: JSON.stringify(jsonString).replace(/name/g, 'name'),
      p_ispayment: getAppDetails[0]?.app_typeid === 4 ? FilterPayment?.is_enable_price : getAppDetails[0]?.is_enable_pricing,
      p_pay_amount: 0,
      p_payment_status: 'success',
      p_createdby: getAppointmentFlow?.userId,
      p_book_form_field: keys,
      p_book_form_value: value,
      p_payment_type: getAppDetails[0]?.paymentMethods,
      p_service_id: getAppointmentFlow?.serviceId,
      service_name: FilterPayment?.service_name ?? null,
      p_app_type: getAppDetails[0]?.app_typeid,
      app_org_name: getAppDetails[0]?.app_org_name,
      app_name: getAppDetails[0]?.app_name,
      currency: getAppDetails[0]?.app_typeid === 4 ? FilterPayment?.deposit_currency : getAppDetails[0]?.dep_currency,
      calledNumber: facebookCcaasTable?.agentFacebookId,
      customerNumber: facebookCcaasTable?.customerFacebookId,
      domainId: facebookCcaasTable?.domainId,
      channelType: 'Facebook',
      serv_dur_pricing: getAppDetails[0]?.serv_dur_pricing ?? null,
      amount_collected: getAppDetails[0]?.app_typeid === 4 ? FilterPayment?.amount_collected : getAppDetails[0]?.amount_collected
    };
    const encryptedId = CryptoJS.AES.encrypt(JSON.stringify(body), process.env.REACT_APP_ENCDEC_KEY).toString();
    const appointmentUrl = `${process.env.bookingUrl}/socialMediaPayment/?id=${encryptedId}`;
    const PayNowUrl = `Pay Now ${appointmentUrl}`;
    logger.info(PayNowUrl);
    await sendPayment(facebookCcaasTable, appointmentUrl);
  } catch (error) {
    logger.error('bookingPayment err', error);
  }
}

// get appointment instagram
export async function bookingPaymentInsta(jsonString: any, getAppointmentFlow: any, getAppDetails: any, EndTime: any, typeValue: any, instagramCcaasTable: any, FilterPayment: any) {
  const jdata: any = [];
  jsonString.forEach((item: any) => {
    const key = Object.keys(item)[0]?.replace(/\s+/g, '_');
    jdata.push(key);
  });
  const keys = jdata?.join(', ');
  const jsrc: any = [];
  jsonString.forEach((item: any) => { jsrc.push(`"${(Object.values(item)[0])}"`); });
  const value = jsrc?.join(', ');
  try {
    const body = {
      p_company_id: getAppointmentFlow?.companyId,
      p_app_id: getAppointmentFlow?.appId,
      p_user_id: +(getAppointmentFlow?.consultant),
      p_book_date: getAppointmentFlow?.processDate,
      p_book_starttime: getAppointmentFlow?.processTime,
      p_book_endtime: EndTime?.substring(0, 5),
      p_book_duration: +(getAppointmentFlow?.processDuration),
      p_book_duration_type: typeValue === 1 ? 'mins' : 'hours',
      p_book_form: JSON.stringify(jsonString).replace(/name/g, 'name'),
      p_ispayment: getAppDetails[0]?.app_typeid === 4 ? FilterPayment?.is_enable_price : getAppDetails[0]?.is_enable_pricing,
      p_pay_amount: 0,
      p_payment_status: 'success',
      p_createdby: getAppointmentFlow?.userId,
      p_book_form_field: keys,
      p_book_form_value: value,
      p_payment_type: getAppDetails[0]?.paymentMethods,
      p_service_id: getAppointmentFlow?.serviceId,
      service_name: FilterPayment?.service_name ?? null,
      p_app_type: getAppDetails[0]?.app_typeid,
      app_org_name: getAppDetails[0]?.app_org_name,
      app_name: getAppDetails[0]?.app_name,
      currency: getAppDetails[0]?.app_typeid === 4 ? FilterPayment?.deposit_currency : getAppDetails[0]?.dep_currency,
      calledNumber: instagramCcaasTable?.agentInstagramId,
      customerNumber: instagramCcaasTable?.customerInstagramId,
      domainId: instagramCcaasTable?.domainId,
      channelType: 'Instagram',
      serv_dur_pricing: getAppDetails[0]?.serv_dur_pricing ?? null,
      amount_collected: getAppDetails[0]?.app_typeid === 4 ? FilterPayment?.amount_collected : getAppDetails[0]?.amount_collected
    };
    const encryptedId = CryptoJS.AES.encrypt(JSON.stringify(body), process.env.REACT_APP_ENCDEC_KEY).toString();
    const appointmentUrl = `${process.env.bookingUrl}/socialMediaPayment/?id=${encryptedId}`;
    const PayNowUrl = `Pay Now ${appointmentUrl}`;
    logger.info(PayNowUrl);
    await sendPaymentInsta(instagramCcaasTable, appointmentUrl);
  } catch (error) {
    logger.error('bookingPaymentInsta err', error);
  }
}

// get appointment details
export async function AppointmentInfoPaymentCheckInsta(data: any) {
  try {
    const body = {
      p_app_id: data?.appId,
      p_company_id: data?.companyId
    };
    const appointmentData: any = await axios.post(process?.env?.getAppointment, body);
    if (appointmentData?.data?.status_code === 200) {
      return appointmentData?.data?.finalresponse;
    }
    return [];

  } catch (error) {
    logger.error('AppointmentInfoPaymentCheckInsta err:', error);
  }
}
