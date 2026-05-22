import { useEffect, useMemo, useRef, useState } from "react";
import styles from "../incomingcall/scss/incomingcallnotify.module.scss";
import { useDispatcher } from "../../../store/redux-store/dispatch";
import { useNavigate } from "react-router-dom";
import { useSelector } from "react-redux";
import { CallService } from "../../../base/service/call_service";

const TIMER_DURATION = 30;

function IncomingCallNotify() {
  const dispatch = useDispatcher();
  const navigate = useNavigate();
  const { callSessionDetailswhilePopup, incomingCallDetail } = useSelector((state: any) => state.callPopUpDetails);
  const allQueueList: any[] = useSelector((state: any) => state.commonData?.AllQueueList ?? []);
  const incomingCallDetails: any = useSelector((state: any) => state.chatReducer?.incomingcallDetails ?? {});
  const { acceptCallFunction, declineCallFunction } = useMemo(() => CallService(dispatch, navigate), [dispatch, navigate]);
  const [remainingSeconds, setRemainingSeconds] = useState(TIMER_DURATION);

  const popupData = callSessionDetailswhilePopup && Object.keys(callSessionDetailswhilePopup)?.length > 0 ? callSessionDetailswhilePopup : incomingCallDetail;

  const contactDetails = popupData?.contactDetails ?? {};
  const firstName = contactDetails?.firstName ?? contactDetails?.first_name ?? "";
  const lastName = contactDetails?.lastName ?? contactDetails?.last_name ?? "";
  const customerName = `${firstName} ${lastName}`.trim();
  const callerNumber = popupData?.fromcaller ?? popupData?.from ?? incomingCallDetail?.from ?? "Unknown";
  const callerDisplay = customerName || callerNumber;

  const incomingQueueId = incomingCallDetails?.qid;
  const matchedQueue = (Array.isArray(allQueueList) ? allQueueList : []).find(
    (item: any) => `${item?.qid ?? item?.queueId ?? item?.queue_id ?? item?.id ?? ""}` === `${incomingQueueId ?? ""}`
  );
  const queueName = matchedQueue?.queueName ?? matchedQueue?.queue_name ?? matchedQueue?.name ?? "-";
  const customerLabel = customerName ? "Existing customer" : "New customer";
  const timerText = String(remainingSeconds).padStart(2, "0");
  const sessionKey = `${popupData?.sessionId ?? ""}|${popupData?.fromcaller ?? popupData?.from ?? ""}`;
  const prevSessionKey = useRef<string>("");

  useEffect(() => {
    if (!popupData || Object.keys(popupData).length === 0) return;
    if (sessionKey === prevSessionKey.current) return;
    prevSessionKey.current = sessionKey;
    setRemainingSeconds(TIMER_DURATION);
    const timer = window.setInterval(() => {
      setRemainingSeconds((prev) => (prev > 0 ? prev - 1 : 0));
    }, 1000);
    return () => window.clearInterval(timer);
  }, [sessionKey]);

  const handleAcceptCall = () => {
    const callUserAgent =
      popupData?.userAgent
      ?? popupData?.useragent
      ?? incomingCallDetail?.userAgent
      ?? incomingCallDetail?.useragent
      ?? callSessionDetailswhilePopup?.userAgent
      ?? callSessionDetailswhilePopup?.useragent;
    const incomingCallerRaw =
      popupData?.fromcaller
      ?? popupData?.from
      ?? incomingCallDetail?.fromcaller
      ?? incomingCallDetail?.from
      ?? incomingCallDetails?.cli
      ?? incomingCallDetails?.customerNumber
      ?? "";
    const incomingCaller = `${incomingCallerRaw}`.trim();
    if (!callUserAgent || !incomingCaller) return;
    acceptCallFunction(callUserAgent, incomingCaller);
  };

  const handleDeclineCall = () => {
    const callUserAgent = popupData?.userAgent ?? popupData?.useragent;
    if (!callUserAgent) return;
    declineCallFunction(callUserAgent);
  };

  return (
    <div className={styles.income}>
      <div className={styles.incomeblock}>
        <div className={styles.incometitle}>{callerDisplay}</div>
        <div className={styles.incomedetail}>
          <div className={styles.teamdtls}>
            <span className={styles.bold}>Team: </span>
            {queueName}
          </div>
          <div className={styles.incometags}>
            <div className={styles.incometagsinner}>{customerLabel}</div>
          </div>
        </div>
      </div>

      <div className={styles.incomebottom}>
        <div className={styles.incometimer}>
          <div className={styles.lefts}>
            <div className={`${styles.purplebtn} ${styles.dialpadbtn}`}>
              <svg
                xmlns="http://www.w3.org/2000/svg"
                width="19"
                height="18"
                viewBox="0 0 19 18"
                fill="none"
                aria-hidden="true"
              >
                <path
                  d="M2.27006 0.566614L2.47926 0.425014C3.64676 -0.323986 5.19206 -0.0607859 6.08946 1.03931L7.31776 2.55081C8.08026 3.48881 8.18816 4.81151 7.57406 5.84401L6.56856 7.55131C6.40666 7.82111 6.88566 8.67831 8.08016 9.88601C9.28806 11.0806 10.1453 11.5596 10.4219 11.4046L12.1225 10.3921C13.1617 9.78491 14.4776 9.88601 15.4223 10.6553L16.9339 11.8836C18.0339 12.7811 18.2971 14.3265 17.5413 15.4871L17.4063 15.7032C16.3807 17.2957 14.4709 18.2471 12.5477 17.9032C10.0575 17.4508 7.47966 15.872 4.78046 13.1862C2.10136 10.4936 0.515563 7.90911 0.0701631 5.42571C-0.273937 3.50241 0.677463 1.59251 2.27006 0.566614Z"
                  fill="white"
                />
                <path
                  d="M14.513 0.00811437C14.2411 -0.0410856 13.9811 0.139714 13.9322 0.411614C13.8831 0.683214 14.0638 0.943214 14.3357 0.992414C16.2596 1.32851 17.8407 3.14731 17.9748 5.10321C17.9857 5.25471 17.9886 5.40651 17.9827 5.55821C17.9716 5.83421 18.1865 6.06671 18.4626 6.07761C18.7385 6.08851 18.9711 5.87382 18.9821 5.59782C18.9895 5.40732 18.9858 5.21762 18.972 5.03042C18.7861 2.60612 16.9278 0.454414 14.513 0.00811437Z"
                  fill="white"
                />
                <path
                  d="M13.806 2.18791C13.5341 2.13901 13.2741 2.31951 13.2253 2.59141C13.1764 2.86331 13.357 3.12331 13.6289 3.17221C14.7182 3.35731 15.6381 4.41271 15.7106 5.52631L15.7151 5.78551C15.7044 6.06141 15.9194 6.29371 16.1955 6.30441C16.4714 6.31511 16.7038 6.10011 16.7145 5.82421C16.7191 5.70001 16.7168 5.57611 16.7079 5.45361C16.5835 3.87151 15.3864 2.48321 13.806 2.18791Z"
                  fill="white"
                />
              </svg>
            </div>
          </div>

          <div className={styles.rights}>
            <div className={styles.incometimertext}>Answer in</div>
            <div className={styles.incometimercount}>{timerText}</div>
          </div>
        </div>

        <div className={styles.incomeaction}>
          <div
            className={`${styles.answerBtn} ${styles.dialpadbtn}`}
            onClick={handleAcceptCall}
            aria-label="Accept call"
          >
            <svg
              xmlns="http://www.w3.org/2000/svg"
              width="18"
              height="18"
              viewBox="0 0 18 18"
              fill="none"
              aria-hidden="true"
            >
              <path
                d="M2.27006 0.566614L2.47926 0.425014C3.64676 -0.323986 5.19206 -0.0607859 6.08946 1.03931L7.31776 2.55081C8.08026 3.48881 8.18816 4.81151 7.57406 5.84401L6.56856 7.55131C6.40666 7.82111 6.88566 8.67831 8.08016 9.88601C9.28806 11.0806 10.1453 11.5596 10.4219 11.4046L12.1225 10.3921C13.1617 9.78491 14.4776 9.88601 15.4223 10.6553L16.9339 11.8836C18.0339 12.7811 18.2971 14.3265 17.5413 15.4871L17.4063 15.7032C16.3807 17.2957 14.4709 18.2471 12.5477 17.9032C10.0575 17.4508 7.47966 15.872 4.78046 13.1862C2.10136 10.4936 0.515558 7.90911 0.0701579 5.42571C-0.273942 3.50241 0.677458 1.59251 2.27006 0.566614Z"
                fill="#00b885"
              />
            </svg>
          </div>

          <div
            className={`${styles.endBtn} ${styles.dialpadbtn}`}
            onClick={handleDeclineCall}
            aria-label="Decline call"
          >
            <svg
              xmlns="http://www.w3.org/2000/svg"
              width="22"
              height="10"
              viewBox="0 0 22 10"
              fill="none"
              aria-hidden="true"
            >
              <path
                d="M21.5189 7.1483L21.471 7.3963C21.1751 8.7515 19.8963 9.658 18.4838 9.5147L16.5465 9.3145C15.3441 9.1905 14.3325 8.3313 14.0366 7.167L13.5403 5.2488C13.464 4.9436 12.5192 4.6763 10.8206 4.6668C9.12191 4.6763 8.17701 4.9437 8.09101 5.2488L7.60441 7.1673C7.29901 8.3316 6.29701 9.1905 5.08501 9.3145L3.14761 9.5149C1.73521 9.658 0.456313 8.7515 0.170113 7.3962L0.112713 7.1479C-0.288087 5.2966 0.389613 3.2734 1.99271 2.1567C4.07351 0.7158 7.01261 0.0095 10.8205 0C14.6188 0.0095 17.5677 0.7158 19.6387 2.1567C21.242 3.2734 21.9196 5.2967 21.5189 7.1483Z"
                fill="#e02239"
              />
            </svg>
          </div>
        </div>
      </div>
    </div>
  );
}

export default IncomingCallNotify;
