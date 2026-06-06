
import React, { useState, useEffect } from 'react';

import { messageActions } from "../../store/slice/message_slice";
import { useSelector, useDispatch } from "react-redux";
import { config } from "../../base/constants";
import moment from 'moment';
import { encryptData, newChatSendMsg } from "../../chatbot/utils"
import store from "../../store/redux-store/root_store";
// const Calendar = lazy(() => import('antd/es/calendar'));
import Calendar from 'antd/es/calendar';
import { continusBlock } from '../../chatbot/utils'

const BookingCalendar = (props) => {
  const messageList = useSelector((state) => state.message);
  const [selectedDate, setSelectedDate] = useState(null)
  const [selectedTime, setSelectedTime] = useState(null)
  const [showTime, setShowTime] = useState(false)
  const [timeSlot, setTimeSlot] = useState([])
  const [currentPage, setCurrentPage] = useState(1);


  useEffect(() => {
    setSelectedDate(getCurrentFormattedDate())
    generateTimeSlots(messageList?.integration?.meeting_duration ?? 60, "")
  }, [])
  const getCurrentFormattedDate = () => {
    const options = { weekday: 'short', month: 'short', day: 'numeric', year: 'numeric' };
    const formattedDate = new Date().toLocaleDateString('en-US', options);
    return formattedDate;
  }

  const indexOfLastItem = currentPage * 8;
  const indexOfFirstItem = indexOfLastItem - 8;
  const currentItems = timeSlot.slice(indexOfFirstItem, indexOfLastItem);
  //console.log(timeSlot,indexOfFirstItem,indexOfLastItem,"indexOfFirstItem")

  const handlePageChange = (newPage) => {
    if (newPage > 0 && newPage <= 3 && timeSlot.length === 24)
      setCurrentPage(newPage);
    else if (newPage > 0 && newPage <= 6 && timeSlot.length === 48)
      setCurrentPage(newPage);
    else if (newPage > 0 && newPage <= 12 && timeSlot.length === 96)
      setCurrentPage(newPage);
    else if (newPage > 0 && ((newPage - 1) * 8) < timeSlot.length)
      setCurrentPage(newPage);
  };
  const onSelectDate = (e) => {
    try {
      const date = new Date(e["$d"]);
      const formattedDate = date.toDateString();
      setSelectedDate(formattedDate)
    } catch (error) {
      console.log(error)
    }
  }

  const onConfirmDate = () => {
    generateTimeSlots(messageList?.integration?.meeting_duration ?? 60, selectedDate);
    setShowTime(true)
  }

  const generateTimeSlots = (intervalInMinutes, formattedDate) => {
    const timeSlots = [];
    const hoursInDay = 24;
    const minutesInHour = 60;
    let hhour = 0;
    var date = new Date();
    var date1 = new Date(formattedDate);
    if (date1.getDate() === date.getDate()) {
      hhour = date.getHours();
      console.log("hourss")
    }

    for (let i = hhour; i < hoursInDay; i++) {
      for (let j = 0; j < minutesInHour; j += intervalInMinutes) {
        const startHour = i < 10 ? `0${i}` : `${i}`;
        const startMinute = j < 10 ? `0${j}` : `${j}`;

        let endHour, endMinute;
        if (j + intervalInMinutes < minutesInHour) {
          endHour = i < 10 ? `0${i}` : `${i}`;
          endMinute = j + intervalInMinutes < 10 ? `0${j + intervalInMinutes}` : `${j + intervalInMinutes}`;
        } else {
          endHour = i + 1 < 10 ? `0${i + 1}` : `${i + 1}`;
          endMinute = '00';
        }

        const startTime = `${startHour}:${startMinute}`;
        const endTime = `${endHour}:${endMinute}`;
        const timeSlot = `${startTime} - ${endTime}`;

        timeSlots.push(timeSlot);
      }
    }

    setTimeSlot(timeSlots);
  }
  const onSelectedTime = (e) => {
    try {
      setSelectedTime(e)
    } catch (error) {
      console.log(error)
    }
  }

  const beforeBooking = () => {
    try {
      let filtered_array = messageList?.inputEntity?.find((o) => o.email);
      console.log(filtered_array, "filtered_arrayfiltered_arrayfiltered_array")
      const data = {
        "start_Time": convertToTimestamp(selectedTime.split('-')[0]),
        "end_Time": convertToTimestamp(selectedTime.split('-')[1]),
        "event_Title": messageList?.integration?.connector === 8 ? "Outlook" : 'Google',
        "participants": [
          {
            "address": filtered_array?.email ?? "test@gmail.com",
            "name": messageList?.integration?.connector === 8 ? "Outlook" : 'Google'
          }
        ]
      }
      if (messageList?.integration?.is_payment) {
        store.dispatch(messageActions.setIntegration({ ...messageList.integration, payload: data, }))
        const paymentData = {
          domainId: messageList?.personalisationAll?.domainId,
          amount: messageList?.integration?.amount,
          currency: "INR"
        }
        const paymentEncrypt = encryptData(paymentData)
        sessionStorage.setItem("AMOUNT", messageList?.integration?.amount)
        store.dispatch(messageActions.paymentEnable(true))
        store.dispatch(messageActions.paymentSource(messageList?.integration?.paymentMethod.toLowerCase()))
        store.dispatch(messageActions.paymentId(paymentEncrypt))
      } else {
        onBookingConfirmation(data)
        store.dispatch(messageActions.setIntegration({ ...messageList.integration, isPaymentSuccess: false, isMsGoogleShow: false, showIframe: false }))
      }
    } catch (error) {
      console.log(error)
    }
  }

  useEffect(() => {
    const message_data = store.getState().message?.integration
    if (message_data.isPaymentSuccess) {
      onBookingConfirmation(message_data.payload)
      store.dispatch(messageActions.setIntegration({ ...messageList.integration, isPaymentSuccess: false, isMsGoogleShow: false, showIframe: false }))
    }
  }, [store.getState().message?.integration])

  const onBookingConfirmation = (data) => {
    try {
      if (messageList?.integration?.connector === 8) {
        outLookBooking(data)
      } else if (messageList?.integration?.connector === 9) {
        googleBooking(data)
      }
    } catch (error) {
      console.log(error)
    }
  }
  const outLookBooking = async (data) => {
    try {
      const token = messageList?.integration?.connecter_data[0].token;
      const end_point = messageList?.integration?.end_point;
      const response = await fetch(end_point, {
        method: 'POST',
        headers: {
          Authorization: token,
          'Content-Type': 'application/json',
        },
        body: JSON.stringify(data),
      });
      const eve_response = await response.json();
      if (eve_response?.statusCode === 200) {
        let email = eve_response?.result?.attendees[0]?.emailAddress?.address;
        let created_at = eve_response?.result?.start?.dateTime;
        let end_at = eve_response?.result?.end?.dateTime;
        let _timestamp = moment(new Date(created_at)).format("MMM DD, YYYY hh:mm a");
        let endtimestamp = moment(new Date(end_at)).format("hh:mm a");
        let msg = "All set! Your booking is scheduled on " + _timestamp + " to " + endtimestamp + "  I've sent an email confirmation to " + email;
        newChatSendMsg("Text", msg, "OUT", 0);
        store.dispatch(messageActions.setIntegration({ ...messageList.integration, showIframe: false, isMsGoogleShow: false, url: '' }));
      } else if (eve_response?.statusCode === 401 && eve_response?.result.message === 'Session expired or invalid') {
        // Refresh token
        const refreshResponse = await fetch(`${config().CONNECTOR_URL}/outlook/refresh_token/${messageList.personalisationAll.domainId}`, {
          method: 'GET',
          headers: {
            Authorization: token,
          },
        });
        const refreshData = await refreshResponse.json();
        if (refreshData.statusCode === 200) {
          // Retry the initial request with the new token
          const retryResponse = await fetch(end_point, {
            method: 'POST',
            headers: {
              Authorization: refreshData.results.token,
              'Content-Type': 'application/json',
            },
            body: JSON.stringify(data),
          });
          const sec_response = await retryResponse.json();
          if (sec_response?.statusCode === 200) {
            let email = sec_response?.result?.attendees[0]?.emailAddress?.address;
            let created_at = sec_response?.result?.start?.dateTime;
            let end_at = sec_response?.result?.end?.dateTime;
            let _timestamp = moment(new Date(created_at)).format("MMM DD, YYYY hh:mm a");
            let endtimestamp = moment(new Date(end_at)).format("hh:mm a");
            let msg = "All set! Your booking is scheduled on " + _timestamp + " to " + endtimestamp + "  I've sent an email confirmation to " + email;
            newChatSendMsg("Text", msg, "OUT", 0);
            store.dispatch(messageActions.setIntegration({ ...messageList.integration, showIframe: false, isMsGoogleShow: false, url: '' }));
          }
        }
      }
    } catch (error) {
      console.log(error);
    }

  }

  const googleBooking = async (data) => {
    try {
      console.log(data, "datadatadatadata");
      const token = messageList?.integration?.connecter_data[0].token;
      const end_point = messageList?.integration?.end_point;
      const dataRes = {
        "start_Time": data?.start_Time,
        "end_Time": data?.end_Time,
        "event_Title": data?.event_Title,
        "participants": [
          {
            "email": data?.participants[0].address
          }
        ]
      };
      const response = await fetch(end_point, {
        method: 'POST',
        headers: {
          Authorization: token,
          'Content-Type': 'application/json'
        },
        body: JSON.stringify(dataRes)
      });
      const eve_response = await response.json();
      if (eve_response?.statusCode === 200) {
        let email = eve_response?.result?.attendees[0]?.email;
        let created_at = eve_response?.result?.start?.dateTime;
        let end_at = eve_response?.result?.end?.dateTime;
        let _timestamp = moment(new Date(created_at)).format("MMM DD, YYYY hh:mm a");
        let endtimestamp = moment(new Date(end_at)).format("hh:mm a");
        let msg = "All set! Your booking is scheduled on " + _timestamp + " to " + endtimestamp + "  I've sent an email confirmation to " + email;
        newChatSendMsg("Text", msg, "OUT", 0);
        store.dispatch(messageActions.setIntegration({ ...messageList.integration, isMsGoogleShow: false, url: '' }));
        let filtered_array = messageList?.currentNodeFlow?.data?.children?.filter(o => o.type === "success");
        let data = {
          childId: filtered_array[0]?.childId,
          type: 'error'
        }
        setTimeout(() => {
          continusBlock(data);
        }, 3000)
      } else if (eve_response?.statusCode === 401 && eve_response?.result.message === 'Session expired or invalid') {
        // Refresh token
        const refreshResponse = await fetch(`${config().CONNECTOR_URL}/google/refresh_token/${messageList.personalisationAll.domainId}`, {
          method: 'GET',
          headers: {
            Authorization: token
          }
        });
        const refreshData = await refreshResponse.json();
        if (refreshData.statusCode === 200) {
          // Retry the POST request with the new token
          const retryResponse = await fetch(end_point, {
            method: 'POST',
            headers: {
              Authorization: refreshData.results.token,
              'Content-Type': 'application/json'
            },
            body: JSON.stringify(dataRes)
          });

          const sec_response = await retryResponse.json();
          console.log("second response ", sec_response);

          if (sec_response?.statusCode === 200) {
            let email = sec_response?.result?.attendees[0]?.email;
            let created_at = sec_response?.result?.start?.dateTime;
            let end_at = sec_response?.result?.end?.dateTime;
            let _timestamp = moment(new Date(created_at)).format("MMM DD, YYYY hh:mm a");
            let endtimestamp = moment(new Date(end_at)).format("hh:mm a");
            let msg = "All set! Your booking is scheduled on " + _timestamp + " to " + endtimestamp + "  I've sent an email confirmation to " + email;
            newChatSendMsg("Text", msg, "OUT", 0);
            store.dispatch(messageActions.setIntegration({ ...messageList.integration, isMsGoogleShow: false, url: '' }));
            let filtered_array = messageList?.currentNodeFlow?.data?.children?.filter(o => o.type === "success");
            let data = {
              childId: filtered_array[0]?.childId,
              type: 'error'
            }
            setTimeout(() => {
              continusBlock(data);
            }, 3000)
          }
        }
      }
    } catch (error) {
      console.log(error);
    }

  }

  const convertToTimestamp = (time) => {
    const inputDateString = `${selectedDate} ${time.trim()}:00 GMT+0530`;
    console.log("inputDateString", inputDateString)
    const timestamp = Date.parse(inputDateString);
    return timestamp;
  }
  const disabledDate = (current) => {
    return current && current < moment().startOf('day');
  };

  return (
    <div className="worktualmaincalendar">
      {!showTime ?
        <div className="worktualcalendarinner">

          {/* <p>Please use the date picker below to select a preferred date and time</p> */}
          <div>
            <Calendar onChange={onSelectDate} disabledDate={disabledDate} mode="month" />
          </div>
          <div>
            <p className="worktualbtns" onClick={() => onConfirmDate()}>Confirm</p>
          </div>
        </div>
        :
        <div className="worktualtimerinner">

          {/* <p>Please use the date picker below to select a preferred date and time</p> */}
          <div className="worktualtimer">
            <svg xmlns="http://www.w3.org/2000/svg" width="6" height="13" viewBox="0 0 6 13" fill="none" onClick={() => handlePageChange(currentPage - 1)}>
              <path d="M5.15326 12.1343C4.96186 12.1343 4.76996 12.0611 4.62346 11.9155L0.758756 8.06197C0.332956 7.63717 0.098157 7.07177 0.097657 6.46917C0.097157 5.86657 0.332056 5.30117 0.758756 4.87447L4.62396 1.02197C4.91696 0.729973 5.39206 0.729973 5.68446 1.02397C5.97696 1.31697 5.97596 1.79157 5.68246 2.08447L1.81776 5.93697C1.67566 6.07857 1.59756 6.26807 1.59756 6.46917C1.59756 6.66937 1.67566 6.85787 1.81776 6.99947L5.68296 10.853C5.97596 11.146 5.97686 11.6206 5.68446 11.9135C5.53796 12.061 5.34556 12.1343 5.15326 12.1343Z" fill="#757676" />
            </svg>
            <div className="worktualtimerinner">
              {currentItems.map((row, index) => {
                if (index < 8)
                  return (
                    <p className={`${row === selectedTime ? 'activewk' : ''}`} key={index} onClick={() => onSelectedTime(row)}>{row}</p>
                  )
              })}
            </div>
            <svg xmlns="http://www.w3.org/2000/svg" width="6" height="13" viewBox="0 0 6 13" fill="none" onClick={() => handlePageChange(currentPage + 1)}>
              <path d="M0.847607 12.1341C0.655207 12.1341 0.462808 12.0609 0.316408 11.9134C0.0239078 11.6204 0.0249076 11.1458 0.318408 10.8529L4.18271 7.0004C4.32481 6.8588 4.40291 6.6693 4.40291 6.4682C4.40291 6.268 4.32481 6.0795 4.18271 5.9379L0.318008 2.0844C0.025008 1.7914 0.0241079 1.3168 0.316508 1.0239C0.608508 0.729998 1.08361 0.729998 1.37751 1.0219L5.24181 4.8754C5.66761 5.3002 5.90241 5.8656 5.90291 6.4682C5.90341 7.0708 5.66851 7.6362 5.24181 8.0629L1.37711 11.9154C1.23111 12.0609 1.03901 12.1341 0.847607 12.1341Z" fill="#757676" />
            </svg>
          </div>

          <p className="worktualbtns" onClick={beforeBooking}>Confirm</p>
        </div>
      }
    </div>
  )
}
export default BookingCalendar;