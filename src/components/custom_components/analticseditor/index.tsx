import React from "react";
import { useSelector } from 'react-redux';
import { assets } from '../../../styles/assets/assets'

const Analticsinput = ({ callBackMsg }: any) => {
  const [message, setMessage] = React.useState("");
  const analyticsData = useSelector((state: any) => state.analyticsData);

  const sendMessage = (message: any) => {
    if (!message.trim()) return;
    callBackMsg(message);
    setMessage("");
  }

  return (
    <>
      <div className="analytics-input">
        <textarea
          placeholder="e.g., Provide a summary of this month's campaigns across all channels; include open rates, CTR, and engagement scores"
          rows={3}
          value={message}
          onChange={(e) => setMessage(e.target.value)}
          onKeyDown={(e) => {
            if (e.key === "Enter" && !e.shiftKey) {
              e.preventDefault();
              if (message.trim()) {
                sendMessage(message);
                setMessage("");
              }
            }
          }}
        />
        <div className="icons">
          {/* <button className="icon-btn history" >
            <svg xmlns="http://www.w3.org/2000/svg" width="21" height="20" viewBox="0 0 21 20" fill="none">
              <path d="M20.4797 9.46286C20.1828 3.95696 15.469 -0.293043 9.95528 0.0136574C8.11248 0.113057 6.36668 0.730658 4.85678 1.76586L4.83228 1.06636C4.81368 0.526357 4.37038 0.101558 3.83428 0.101558C3.82258 0.101558 3.81088 0.101558 3.79818 0.102558C3.24638 0.122058 2.81478 0.584957 2.83428 1.13676L2.93678 4.08306C2.95538 4.62306 3.39868 5.04786 3.93478 5.04786C3.94648 5.04786 3.95818 5.04786 3.97088 5.04686L6.91818 4.94436C7.46998 4.92486 7.90158 4.46196 7.88208 3.91016C7.86348 3.35836 7.42308 2.91216 6.84788 2.94626L5.75228 2.98406C7.02368 2.12296 8.48948 1.60846 10.0373 1.52536C14.7111 1.27436 18.717 4.87006 18.968 9.54486C19.22 14.2178 15.6223 18.2246 10.9494 18.4756C8.69258 18.6084 6.50988 17.8311 4.82338 16.3164C3.13588 14.8018 2.14078 12.7207 2.01868 10.456C1.99528 10.039 1.65248 9.73526 1.22178 9.74116C0.804776 9.76366 0.484476 10.1201 0.506976 10.5381C0.650576 13.2051 1.82438 15.6572 3.81168 17.4424C5.66228 19.1035 8.01288 20.002 10.4806 20.002C10.6642 20.002 10.8468 19.9971 11.0314 19.9874C16.5373 19.6905 20.7756 14.9697 20.4797 9.46286Z" fill="black" />
              <path d="M10.125 5.11234C9.7109 5.11234 9.375 5.44824 9.375 5.86234V10.9209C9.375 11.1582 9.4873 11.3809 9.6777 11.5225L13.3095 14.2237C13.4433 14.3243 13.6005 14.3721 13.7558 14.3721C13.9853 14.3721 14.2119 14.2676 14.3583 14.0694C14.6054 13.7374 14.537 13.2676 14.204 13.0206L10.8749 10.544V5.86243C10.8749 5.44833 10.5391 5.11234 10.125 5.11234Z" fill="black" />
            </svg>
          </button> */}
          {!analyticsData.isTyping || !callBackMsg?.isChat ? <button className="icon-btn send" onClick={() => {
            if (message.trim()) {
              sendMessage(message);
              setMessage("");
            }
          }}>
            <svg width="33" height="32" viewBox="0 0 33 32" fill="none" xmlns="http://www.w3.org/2000/svg">
              <rect x="0.5" width="32" height="32" rx="16" fill="#00E2A4" />
              <g clip-path="url(#clip0_2430_251)">
                <path d="M21.6828 17.6595C23.341 16.0167 23.3186 16.0167 21.6828 14.3398L17.4422 10.1001C17.1074 9.76527 16.565 9.76527 16.2302 10.1001C15.8953 10.4349 15.8954 10.9773 16.2302 11.3122L20.0612 15.1426L10.9391 15.1426C10.4654 15.1426 10.082 15.526 10.082 15.9997C10.082 16.4734 10.4654 16.8568 10.9391 16.8568H20.0614L16.2302 20.688C16.0628 20.8554 15.9791 21.0748 15.9791 21.294C15.9791 21.5133 16.0628 21.7326 16.2302 21.9C16.565 22.2348 17.1074 22.2348 17.4423 21.9L21.6828 17.6595Z" fill="black" />
              </g>
              <defs>
                <clipPath id="clip0_2430_251">
                  <rect width="20.5714" height="20.5714" fill="white" transform="translate(6.21429 5.71428)" />
                </clipPath>
              </defs>
            </svg>
          </button> :
            <img src={assets.passicon} alt="typing" />}
        </div>
      </div>
    </>
  )
}

export default Analticsinput