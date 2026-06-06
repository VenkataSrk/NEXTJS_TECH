import React, { useEffect, useState, useRef, useLayoutEffect } from 'react'
import { GetItemFromStorage } from '../../base/customhooks/useStorage';
import { LocalStorage } from '../../base/customhooks/localStorageKeys';
import { jwtDecode } from 'jwt-decode';
import styles from "./scss/reports.module.scss"
import { assets } from '../../styles/assets/assets'
import { useSelector } from 'react-redux';
import { useDispatcher } from '../../store/redux-store/dispatch';
import SocketService from '../../base/service/socketService';
import { reportSliceActions } from '../../store/slice/report';
import { v4 as uuidv4 } from "uuid";
import { config } from '../../base/constant';
import axios from 'axios';


function Reports() {
  const dispatch = useDispatcher();
  const reportData = useSelector((state: any) => state.reportData);
  const userDetails = GetItemFromStorage(LocalStorage?.ACCESS_TOKEN);
  const userData: any = jwtDecode(userDetails);
  const [message, setMessage] = React.useState("");
  const [isChat, setIsChat] = React.useState(false);
  const scrollerRef = useRef<HTMLDivElement>(null);
  const [performanceReport, setPerformanceReport] = React.useState<string>("");
  const [performanceMetrics, setPerformanceMetrics] = React.useState<string>("");
  const [performanceSummary, setPerformanceSummary] = React.useState<string>("");

  const sessionId = uuidv4();

  useLayoutEffect(() => {
    if (!scrollerRef.current) return;
    scrollerRef.current?.scrollIntoView({ behavior: 'smooth', block: "end" });

  }, [reportData.reportMessage?.length]);

  const sendReportMessage = (msg: string) => {
    try {
      if (!msg || reportData.isTyping) return;
      if (!isChat) setIsChat(true);
      const messageId = uuidv4();
      const data = {
        sessionId,
        message: msg,
        messageId,
        domainId: userData?.domainId ?? null,
        ext: Number(userData?.ext) || null,
        chatType: "OVERALL REPORT",
        sender: "userText",
        date: new Date().toISOString(),
      };
      dispatch(reportSliceActions.setIsTyping(true));
      const temp_data = !isChat ? [] : [...reportData.reportMessage];
      temp_data.push(data);
      dispatch(reportSliceActions.setReportMessage(temp_data));
      SocketService.sentReportMessage(data);
    } catch (error) {
      console.error("sendMessage error:", error);
    }
  };

  useEffect(() => {
    const fetchAllData = () => {
      getPerformanceReport()
      getPerformanceMetrics()
      getPerformanceSummary()
    }
    fetchAllData()
    let interval: any;
    if (!isChat) {
      interval = setInterval(fetchAllData, 30000);
    }
    return () => {
      if (interval) clearInterval(interval);
    };
  }, [isChat])

  const getPerformanceReport = async () => {
    try {
      const url: string = `${config().WEEKLYCAMPAIGN_PERFORMANCE_REPORT}`;
      const response = await axios.post(
        url,
        { domainId: userData?.domainId },
        {
          headers: {
            "Content-Type": "application/json",
          },
        }
      )
      if (response?.status === 200) {
        setPerformanceReport(response?.data?.Message)
      }
    } catch (error) {
      console.log(error)
    }
  }

  const getPerformanceMetrics = async () => {
    try {
      const url: string = `${config().WEEKLYCAMPAIGN_PERFORMANCE_METRIC}`;
      const response = await axios.post(
        url,
        { domainId: userData?.domainId },
        {
          headers: {
            "Content-Type": "application/json",
          },
        }
      )
      if (response?.status === 200) {
        setPerformanceMetrics(response?.data?.Message)
      }
    } catch (error) {
      console.log(error)
    }
  }

  const getPerformanceSummary = async () => {
    try {
      const url: string = `${config().WEEKLYCAMPAIGN_PERFORMANCE_SUMMARY}`;
      const response = await axios.post(
        url,
        { domainId: userData?.domainId },
        {
          headers: {
            "Content-Type": "application/json",
          },
        }
      )
      if (response?.status === 200) {
        setPerformanceSummary(response?.data?.Message)
      }
    } catch (error) {
      console.log(error)
    }
  }

  const SkeletonBox = ({ width = "100%", height = "16px", borderRadius = "6px" }) => (
    <div
      className={"skeletonBox"}
      style={{ width, height, borderRadius }}
    />
  );

  return (
    <div className={` reportspage`}>
      {!isChat && <div className={styles.center}>
        <h5>What you want to know?</h5>
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
                  sendReportMessage(message.trim());
                  setMessage("");
                }
              }
            }}
          />
          <div className="icons">
            <button className="icon-btn history" >
              <svg xmlns="http://www.w3.org/2000/svg" width="21" height="20" viewBox="0 0 21 20" fill="none">
                <path d="M20.4797 9.46286C20.1828 3.95696 15.469 -0.293043 9.95528 0.0136574C8.11248 0.113057 6.36668 0.730658 4.85678 1.76586L4.83228 1.06636C4.81368 0.526357 4.37038 0.101558 3.83428 0.101558C3.82258 0.101558 3.81088 0.101558 3.79818 0.102558C3.24638 0.122058 2.81478 0.584957 2.83428 1.13676L2.93678 4.08306C2.95538 4.62306 3.39868 5.04786 3.93478 5.04786C3.94648 5.04786 3.95818 5.04786 3.97088 5.04686L6.91818 4.94436C7.46998 4.92486 7.90158 4.46196 7.88208 3.91016C7.86348 3.35836 7.42308 2.91216 6.84788 2.94626L5.75228 2.98406C7.02368 2.12296 8.48948 1.60846 10.0373 1.52536C14.7111 1.27436 18.717 4.87006 18.968 9.54486C19.22 14.2178 15.6223 18.2246 10.9494 18.4756C8.69258 18.6084 6.50988 17.8311 4.82338 16.3164C3.13588 14.8018 2.14078 12.7207 2.01868 10.456C1.99528 10.039 1.65248 9.73526 1.22178 9.74116C0.804776 9.76366 0.484476 10.1201 0.506976 10.5381C0.650576 13.2051 1.82438 15.6572 3.81168 17.4424C5.66228 19.1035 8.01288 20.002 10.4806 20.002C10.6642 20.002 10.8468 19.9971 11.0314 19.9874C16.5373 19.6905 20.7756 14.9697 20.4797 9.46286Z" fill="black" />
                <path d="M10.125 5.11234C9.7109 5.11234 9.375 5.44824 9.375 5.86234V10.9209C9.375 11.1582 9.4873 11.3809 9.6777 11.5225L13.3095 14.2237C13.4433 14.3243 13.6005 14.3721 13.7558 14.3721C13.9853 14.3721 14.2119 14.2676 14.3583 14.0694C14.6054 13.7374 14.537 13.2676 14.204 13.0206L10.8749 10.544V5.86243C10.8749 5.44833 10.5391 5.11234 10.125 5.11234Z" fill="black" />
              </svg>
            </button>
            {!reportData.isTyping || !isChat ?
              <button className="icon-btn send"
                onClick={() => {
                  if (message.trim()) {
                    sendReportMessage(message.trim());
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
              </button>
              : <img src={assets.passicon} alt="typing" />}
          </div>
        </div>
      </div>}
      {isChat &&
        <>
          <div className={`${styles.bottomInput} bottominptdown`}>
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
                      sendReportMessage(message.trim());
                      setMessage("");
                    }
                  }
                }}
              />
              <div className="icons">
                <button className="icon-btn history" >
                  <svg xmlns="http://www.w3.org/2000/svg" width="21" height="20" viewBox="0 0 21 20" fill="none">
                    <path d="M20.4797 9.46286C20.1828 3.95696 15.469 -0.293043 9.95528 0.0136574C8.11248 0.113057 6.36668 0.730658 4.85678 1.76586L4.83228 1.06636C4.81368 0.526357 4.37038 0.101558 3.83428 0.101558C3.82258 0.101558 3.81088 0.101558 3.79818 0.102558C3.24638 0.122058 2.81478 0.584957 2.83428 1.13676L2.93678 4.08306C2.95538 4.62306 3.39868 5.04786 3.93478 5.04786C3.94648 5.04786 3.95818 5.04786 3.97088 5.04686L6.91818 4.94436C7.46998 4.92486 7.90158 4.46196 7.88208 3.91016C7.86348 3.35836 7.42308 2.91216 6.84788 2.94626L5.75228 2.98406C7.02368 2.12296 8.48948 1.60846 10.0373 1.52536C14.7111 1.27436 18.717 4.87006 18.968 9.54486C19.22 14.2178 15.6223 18.2246 10.9494 18.4756C8.69258 18.6084 6.50988 17.8311 4.82338 16.3164C3.13588 14.8018 2.14078 12.7207 2.01868 10.456C1.99528 10.039 1.65248 9.73526 1.22178 9.74116C0.804776 9.76366 0.484476 10.1201 0.506976 10.5381C0.650576 13.2051 1.82438 15.6572 3.81168 17.4424C5.66228 19.1035 8.01288 20.002 10.4806 20.002C10.6642 20.002 10.8468 19.9971 11.0314 19.9874C16.5373 19.6905 20.7756 14.9697 20.4797 9.46286Z" fill="black" />
                    <path d="M10.125 5.11234C9.7109 5.11234 9.375 5.44824 9.375 5.86234V10.9209C9.375 11.1582 9.4873 11.3809 9.6777 11.5225L13.3095 14.2237C13.4433 14.3243 13.6005 14.3721 13.7558 14.3721C13.9853 14.3721 14.2119 14.2676 14.3583 14.0694C14.6054 13.7374 14.537 13.2676 14.204 13.0206L10.8749 10.544V5.86243C10.8749 5.44833 10.5391 5.11234 10.125 5.11234Z" fill="black" />
                  </svg>
                </button>
                {!reportData.isTyping || !isChat ?
                  <button className="icon-btn send"
                    onClick={() => {
                      if (message.trim()) {
                        sendReportMessage(message.trim());
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
                  </button>
                  : <img src={assets.passicon} alt="typing" />}
              </div>
            </div>
          </div>
          <div className={styles.backnav} onClick={() => setIsChat(false)}>
            <span style={{ display: "grid" , cursor:"pointer"}}>
              <img src={assets.backicon}></img>
            </span>
            {/* <h5 style={{ fontFamily: "intermedium", fontSize: "16px" }}>{message}</h5> */}
            <h5 style={{ fontFamily: "intermedium", fontSize: "16px" }}>Back</h5>
          </div></>}

      {isChat && <div className={styles.msgbody}>
        <div className={styles.maincontainer}>
          {Array.isArray(reportData?.reportMessage) &&
            reportData.reportMessage.map((msg: any) => {
              const key = msg?.messageId
              const sender = msg?.sender;
              if (sender === "userText") {
                return (
                  <div key={key} className={`${styles.msgbubble} ${styles.right}`}>
                    <div
                      className="chatContent"
                      dangerouslySetInnerHTML={{ __html: msg?.message || "" }}
                    />
                  </div>
                );
              }
              if (sender === "botText") {
                return (
                  <div key={key} className={`${styles.msgbubble} ${styles.left}`}>
                    <div
                      className="chatContent"
                      dangerouslySetInnerHTML={{ __html: msg?.bot_response || "" }}
                    />
                  </div>
                );
              }
              return null;
            })
          }
          {reportData?.isTyping && (
            <p className="generatings">
              {/* AI is Thinking */}
              Agent is Analysing
              <div className="typingdot">
                <span></span>
                <span></span>
                <span></span>
              </div>
            </p>
          )}
          <div ref={scrollerRef} />
        </div>
      </div>}

      {!isChat && <div className={styles.maincontainer}>
          <div className={styles.contentsec}>
        {performanceReport ? (
          <div dangerouslySetInnerHTML={{ __html: performanceReport }} />
        ) : (
          <div>
            <div className="skeletonTextGroup">
              <SkeletonBox width="70%" height="20px" />
              <SkeletonBox width="80%" height="14px" />
              <SkeletonBox width="60%" height="14px" />
              <SkeletonBox width="90%" height="14px" />
              <SkeletonBox width="60%" height="14px" />
              <SkeletonBox width="100%" height="140px" />
            </div>
          </div>
        )}</div>
        <div className={styles.contentsec}>
          {performanceMetrics ? (
            <div dangerouslySetInnerHTML={{ __html: performanceMetrics }} />
          ) : (
            <div>
              <div className="skeletonTextGroup">
                <SkeletonBox width="70%" height="20px" />
                <SkeletonBox width="80%" height="14px" />
                <SkeletonBox width="60%" height="14px" />
                <SkeletonBox width="90%" height="14px" />
                <SkeletonBox width="60%" height="14px" />
                <SkeletonBox width="100%" height="140px" />
              </div>
            </div>
          )}
        </div>
        {/* <div className={styles.contentsec}>
          <h3> Channel Performance Overview</h3>
          <div className={styles.info}>
            <b>Insights:</b>
            <p>Emails are driving the highest engagement, especially with personalisation
              SMS needs monitoring due to slight opt-out increase
              Push campaigns performing well in conversion; could scale further</p>
          </div>
        </div>
        <div className={styles.tableblock}>
          <div className="worktualtablecontainer">
            <div className="tableheader">Weekly Campaign Performance Data <span className='expandicon'> <svg xmlns="http://www.w3.org/2000/svg" width="21" height="20" viewBox="0 0 21 20" fill="none">
              <path d="M18.2258 0.0126953H14.426C14.0119 0.0126953 13.676 0.348595 13.676 0.762695C13.676 1.1768 14.0119 1.5127 14.426 1.5127H17.9148L12.7961 6.6318C12.5031 6.9248 12.5031 7.3994 12.7961 7.6923C12.9426 7.8388 13.1345 7.912 13.3264 7.912C13.5183 7.912 13.7102 7.8388 13.8567 7.6923L18.9758 2.5727V6.0624C18.9758 6.4765 19.3117 6.8124 19.7258 6.8124C20.1399 6.8124 20.4758 6.4765 20.4758 6.0624V2.2626C20.4758 1.0224 19.4665 0.0126953 18.2258 0.0126953Z" fill="#404040" />
              <path d="M7.14324 12.3076L2.02414 17.4272V13.9375C2.02414 13.5234 1.68824 13.1875 1.27414 13.1875C0.860039 13.1875 0.524139 13.5234 0.524139 13.9375V17.7373C0.524139 18.9775 1.53344 19.9873 2.77414 19.9873H6.57394C6.98804 19.9873 7.32394 19.6514 7.32394 19.2373C7.32394 18.8232 6.98804 18.4873 6.57394 18.4873H3.08514L8.20384 13.3682C8.49684 13.0752 8.49684 12.6006 8.20384 12.3077C7.91084 12.0148 7.43614 12.0146 7.14324 12.3076Z" fill="#404040" />
            </svg></span></div>
            <table className='worktual-table'>
              <thead>
                <tr>
                  <th>Metric</th>
                  <th>Value</th>
                  <th>WoW (Week over Week) %</th>
                </tr>
              </thead>
              <tbody>
                <tr>
                  <td>Total Campaigns Sent</td>
                  <td>18</td>
                  <td>+2</td>
                </tr>
                <tr>
                  <td>Overall Open Rate</td>
                  <td>36%</td>
                  <td>+7%</td>
                </tr>
                <tr>
                  <td>Click-Through Rate (CTR)</td>
                  <td>11.2%</td>
                  <td>+5%</td>
                </tr>
                <tr>
                  <td>Conversion Rate</td>
                  <td>4.8%</td>
                  <td>+0.8%</td>
                </tr>
                <tr>
                  <td>Engagement Score</td>
                  <td>78/100</td>
                  <td>+6</td>
                </tr>
              </tbody>
            </table>
          </div>
        </div> */}
        <div className={styles.contentsec}>
          {performanceSummary ? (
            <div dangerouslySetInnerHTML={{ __html: performanceSummary }} />
          ) : (
            <div>
              <div className={styles.info}>
                <div className="skeletonTextGroup">
                  <SkeletonBox width="80%" height="14px" />
                  <SkeletonBox width="60%" height="14px" />
                  <SkeletonBox width="90%" height="14px" />
                  <SkeletonBox width="70%" height="14px" />
                  <SkeletonBox width="60%" height="14px" />
                </div>
              </div>
              <div className="skeletonTextGroup">
                <SkeletonBox width="90%" height="14px" />
                <SkeletonBox width="60%" height="14px" />
                <SkeletonBox width="90%" height="14px" />
                <SkeletonBox width="70%" height="14px" />
                <SkeletonBox width="60%" height="14px" />
              </div>
              <div className={styles.info}>
                <div className="skeletonTextGroup">
                  <SkeletonBox width="90%" height="14px" />
                  <SkeletonBox width="60%" height="14px" />
                  <SkeletonBox width="90%" height="14px" />
                  <SkeletonBox width="70%" height="14px" />
                  <SkeletonBox width="60%" height="14px" />
                </div>
              </div>
            </div>
          )}
        </div>
      </div>}
    </div>
  )
}

export default Reports