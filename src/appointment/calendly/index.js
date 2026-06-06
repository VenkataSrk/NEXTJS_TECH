import React from 'react'
import { useCalendlyEventListener, InlineWidget } from "react-calendly";
import { messageActions } from "../../store/slice/message_slice";
import { useSelector, useDispatch } from "react-redux";
import { getCalendlySchedulePoint } from "../../chatbot/utils"

const CalendlyBooking = () => {
	const messageList = useSelector((state) => state.message);
	const dispatch = useDispatch();
	useCalendlyEventListener({
		onProfilePageViewed: () => console.log("onProfilePageViewed"),
		onDateAndTimeSelected: () => console.log("onDateAndTimeSelected"),
		onEventTypeViewed: () => console.log("onEventTypeViewed"),
		onEventScheduled: (e) => bookingConfirmation(e.data.payload),
	});
	const bookingConfirmation = (data) => {
		getCalendlySchedulePoint(data)
		dispatch(messageActions.setIntegration({ ...data,showIframe:false, isCalendlyShow: false, }))
	}
	return (
		<InlineWidget url={messageList?.integration?.url} />
	)
}

export default CalendlyBooking