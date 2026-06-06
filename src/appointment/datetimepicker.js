
import React, { useState, useEffect } from 'react';
import { v4 as uuidv4 } from "uuid";
import { messageActions } from "../store/slice/message_slice";
import { useSelector, useDispatch } from "react-redux";
import moment from 'moment';
import { chatMessageUpdate } from '../service/socketservice'
import { continusBlock, newChatSendMsg } from "../chatbot/utils"
import store from "../store/redux-store/root_store";
import Calendar from 'antd/es/calendar';


const DateTimePicker = (props) => {
  const messageList = useSelector((state) => state.message);
  const [selectedDate, setSelectedDate] = useState(null)
  const [selectedTime, setSelectedTime] = useState(null)
  const [showTime, setShowTime] = useState(false)
  const [timeSlot, setTimeSlot] = useState([])
  const [rangeStart, setRangeStart] = useState(null);
  const [rangeEnd, setRangeEnd] = useState(null);
  const [isSelecting, setIsSelecting] = useState(false);
  const [selectedDates, setSelectedDates] = useState([]);
  const [currentPage, setCurrentPage] = useState(1);
  

  let pickerNode = null
  if (Object.keys(messageList?.datetimePickerNode[0]?.data?.information).length > 0)
    pickerNode = JSON.parse(messageList?.datetimePickerNode[0]?.data?.information?.datepicker_data)
  const today = moment().startOf('day'); 
  const yearRange = [pickerNode?.dateMode=="2"?today:moment().subtract(200, 'years'), moment().add(200, 'years')];

  useEffect(() => {
    console.log("pickerNode",pickerNode?.widgetType)

    setSelectedDate(getCurrentFormattedDate())
    generateTimeSlots(messageList?.integration?.meeting_duration ?? 60, new Date())
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
  const onConfirmTime = () => {
    setShowTime(true)
  }

  const onConfirmDate = () => {
    let findAgent1 = messageList?.datetimePickerNode[0]?.data?.children?.find(
      (o) => o?.type === 'success'
    );
    let err_array = messageList?.currentNodeFlow?.data?.children?.find(
      (o) => o.type === "default" || o.type === "success"
    );
    const caroType = [];
    if (err_array?.childId?.includes('carousel')) {
      messageList?.currentNodeFlow?.data?.children?.map((list) => {
        let data = {
          childId: list?.childId,
          type: 'carousel'
        }
        caroType.push(data);
      })
    }
    let data = {
      session_id: messageList.uuid,
      domain_id: messageList.personalisationAll?.domainId,
      channelType: 'Chat',
      type: 'error',
      response: "success",
      call_type: caroType?.length > 0 ? "carousel" : "",
      btnValue: findAgent1?.childId,
      callFlowId: messageList.personalisationAll?.chatFlowId,
      carousal_type: caroType?.length > 0 ? caroType : [],
      messageID: uuidv4(),
      ChatHistoryId: parseInt(sessionStorage.ChatHistoryId),
      ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
      inputParams: messageList?.inputEntity,
      targetLang: messageList?.targetLang,
    }
    //continusBlock(data);
    chatMessageUpdate(data);

    let msgdate = "";

    if (pickerNode?.widgetType === "2" && rangeStart) {
      const start = rangeStart?.toDate()?.toDateString();
      const end = rangeEnd ? rangeEnd?.toDate()?.toDateString() : '';
      msgdate = end ? `${start} to ${end}` : start;
    } else if (pickerNode?.widgetType === "5" && selectedDates?.length > 0 && selectedTime !== null) {
      msgdate = selectedDates?.map(item => item?.date?.toDate()?.toDateString())?.join(", ") + " " + selectedTime;
    } else {
      msgdate = selectedTime !== null ? pickerNode?.widgetType !== "6" ? `${selectedDate} ${selectedTime}` : '' + " " + selectedTime : selectedDate;
    }
    // let msgdate = selectedTime !== null ? selectedDate + " " + selectedTime : selectedDate
    newChatSendMsg("Text", msgdate, "IN", 0);
    store.dispatch(messageActions.datetimePickerNode([]))
    let arr = [...messageList?.inputEntity, { date: msgdate }]
    store.dispatch(messageActions.datetimePickerNode([]))
    store.dispatch(messageActions.inputEntity(arr));
  }

  const generateTimeSlots = (intervalInMinutes, formattedDate) => {
    const timeSlots = [];
    const hoursInDay = 24;
    const minutesInHour = 60;
    let hhour = 0;
    var date = new Date();
    var date1 = pickerNode?.dateMode=="2"?new Date(formattedDate):new Date('');
    if (date1.getDate() === date.getDate()) {
      hhour = date.getHours();
      console.log("hourss")
    }
    const now = new Date();
    const selectedDate = new Date(formattedDate);
    const isToday =selectedDate.toDateString() === now.toDateString(); 
    for (let i = hhour; i < hoursInDay; i++) {
      for (let j = 0; j < minutesInHour; j += intervalInMinutes) {
        const startDateTime = new Date(selectedDate);
        startDateTime.setHours(i, j, 0, 0);

        if (isToday && startDateTime <= now && pickerNode?.dateMode=="2") continue;

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

  const convertToTimestamp = (time) => {
    const inputDateString = `${selectedDate} ${time.trim()}:00 GMT+0530`;
    console.log("inputDateString", inputDateString)
    const timestamp = Date.parse(inputDateString);
    return timestamp;
  }
  const disabledDate = (current) => {
    return current && current < moment().startOf('day');
  };

  // const handleRangeSelect = (date) => {
  //   if (!isSelecting) {
  //     setRangeStart(date);
  //     setRangeEnd(null);
  //     setIsSelecting(true);
  //   } else {
  //     if (date.isBefore(rangeStart)) {
  //       setRangeEnd(rangeStart);
  //       setRangeStart(date);
  //     } else {
  //       setRangeEnd(date);
  //     }
  //     setIsSelecting(false);
  //   }
  // };
  const customCellRender = (current) => {
    return current.date();
  };

  // const toggleDate = (date) => {
  //   const existsIndex = selectedDates?.findIndex(d => d.date.isSame(date, 'day'));
  //   if (existsIndex !== -1) {
  //     const updated = [...selectedDates];
  //     updated.splice(existsIndex, 1);
  //     setSelectedDates(updated);
  //   } else {
  //     setSelectedDates([...selectedDates, { date, time: null }]);
  //   }
  // };

  const toggleDate = (date, info) => {
  if (info.source !== "date") {
    return; // prevent capturing when switching year/month
  }

  const existsIndex = selectedDates?.findIndex(d => d.date.isSame(date, "day"));
  if (existsIndex !== -1) {
    const updated = [...selectedDates];
    updated.splice(existsIndex, 1);
    setSelectedDates(updated);
  } else {
    setSelectedDates([...selectedDates, { date, time: null }]);
  }
};

const handleRangeSelect = (date, info) => {
  if (info.source !== "date") {
    // Ignore year/month changes
    return;
  }

  if (!isSelecting) {
    setRangeStart(date);
    setRangeEnd(null);
    setIsSelecting(true);
  } else {
    const start = date.isBefore(rangeStart) ? date : rangeStart;
    const end = date.isBefore(rangeStart) ? rangeStart : date;

    setRangeStart(start);
    setRangeEnd(end);
    setIsSelecting(false);
  }
};
  
  return (
    <div className="worktualmaincalendar">
      {(pickerNode?.widgetType === "1" || pickerNode?.widgetType === "2" || pickerNode?.widgetType === "3" || pickerNode?.widgetType === "4" || pickerNode?.widgetType === "5") && !showTime && <div className="worktualcalendarinner">

        <p>Select Date</p>
        {(pickerNode?.widgetType === "1" || pickerNode?.widgetType === "4") &&
          <div>
            <Calendar onChange={onSelectDate}  validRange={yearRange}  disabledDate={pickerNode?.dateMode=="2"?disabledDate:null} />
          </div>}
        {pickerNode?.widgetType === "2" && (
          <div>
            <Calendar
              onSelect={handleRangeSelect}
              validRange={yearRange}
              disabledDate={pickerNode?.dateMode=="2"?disabledDate:null}
              cellRender={customCellRender}
            />
            {rangeStart && (
              <p>
                Selected Range: {`${rangeStart.format("DD-MM-YYYY")} to `}{rangeEnd ? rangeEnd.format("DD-MM-YYYY") : ''}
              </p>
            )}
          </div>
        )}
        {(pickerNode?.widgetType === "5") &&
          <div>
            <Calendar
              cellRender={customCellRender}
              onSelect={toggleDate}
              validRange={yearRange}
              disabledDate={pickerNode?.dateMode=="2"?disabledDate:null}
            />

            <div style={{ marginTop: 20 }}>
              {selectedDates.map((item, index) => (
                <div key={index} style={{ marginBottom: 10 }}>
                  <strong>{item.date.format('DD-MM-YYYY')}</strong>
                </div>
              ))}
            </div>
          </div>}
        {pickerNode?.widgetType === "3" &&
          <div>
            <Calendar onChange={onSelectDate} mode="year" validRange={yearRange} disabledDate={pickerNode?.dateMode=="2"?disabledDate:null}/>
          </div>}

        <div>
          {pickerNode?.widgetType === "4" || pickerNode?.widgetType === "5" ? <p className="worktualbtns" onClick={() => onConfirmTime()}>Confirm</p> :
            (pickerNode?.widgetType === "2" && rangeStart) ? <p className="worktualbtns" onClick={() => onConfirmDate()}>Confirm</p> : pickerNode?.widgetType !== "2" ? <p className="worktualbtns" onClick={() => onConfirmDate()}>Confirm</p> : ''}

        </div>
      </div>}

      {((pickerNode?.widgetType === "5" || pickerNode?.widgetType === "4") && showTime) && <div className="worktualtimerinner">

        <p>Select Time</p>
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

        {selectedTime ? <p className="worktualbtns" onClick={() => onConfirmDate()} >Confirm</p> : ''}
      </div>}

      {(pickerNode?.widgetType !== "5" && pickerNode?.widgetType !== "4" && (pickerNode?.widgetType === "6" || showTime)) && <div className="worktualtimerinner">

        <p>Select Time</p>
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

        {selectedTime !== null ? <p className="worktualbtns" onClick={() => onConfirmDate()} >Confirm</p> : ''}
      </div>}


    </div>
  )
}
export default DateTimePicker;