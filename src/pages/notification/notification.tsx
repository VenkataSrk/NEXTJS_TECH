import React, { useEffect, useMemo, useRef, useState } from "react";
import styles from "./scss/notification.module.scss"
import cancel from "../../assets/images/project/cancel.svg"
import threedot from "../../assets/images/project/threedot.svg"
import search from "../../assets/images/product/searchicon.svg"
import { GetItemFromStorage } from "../../base/custom_hooks/useStorage";
import { LocalStorage } from "../../base/custom_hooks/localStorageKeys";
import { useNavigate } from "react-router-dom";
import { useSelector } from "react-redux";
import { commonDataSliceActions } from "../../store/slice/commonSilce/commonSlice";
import { useDispatcher } from "../../store/redux-store/dispatch";
import { homeDataSliceActions } from "../../store/slice/homeSlice";
import { updateUnreadNotification } from "../../base/service/core_serviceApi";
import { v4 as uuidv4 } from "uuid";
import aiChatAssistSocketservice from "../../services/aiChatService";
import { dealsDataSliceActions } from "../../store/slice/dealsSlice";

function Notification({ close }) {
  const LoginUserDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
  const { notification } = useSelector((state: any) => state.homeStore);
  const modalRef = useRef<HTMLDivElement>(null);
  const LoginCcaasUserDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);

  const handleclick = () => {
    close(false);
  }

  const groupNotifications = (data) => {

    if (!data) return [];
    const groups = {};

    data.forEach((item) => {
      const date = new Date(item.created_at);

      const today = new Date();
      const yesterday = new Date();
      yesterday.setDate(today.getDate() - 1);
      const isToday = date.toDateString() === today.toDateString();
      const isYesterday = date.toDateString() === yesterday.toDateString();

      let label = "";

      if (isToday) {
        label = "Today";
      } else if (isYesterday) {
        label = "Yesterday";
      } else {
        label = date.toLocaleDateString("en-US", { weekday: "long" });
      }

      if (!groups[label]) {
        groups[label] = [];
      }

      groups[label].push(item);
    });

    return groups;
  };

  const getTimeAgo = (dateString: any) => {
    const diff = Date.now() - new Date(dateString).getTime();

    const minutes = Math.floor(diff / 60000);
    const hours = Math.floor(diff / 3600000);
    const days = Math.floor(diff / 86400000);

    if (minutes < 60) return `${minutes}m`;
    if (hours < 24) return `${hours}h`;
    return `${days}d`;
  };

  // const groupedNotifications = useMemo(() => groupNotifications(notification), [notification]);
  const { appTabs } = useSelector((state: any) => state.commonData);
  const navigate = useNavigate();
  const dispatch = useDispatcher();
  const [searchTerm, setSearchTerm] = useState("");


  const filteredNotifications = useMemo(() => {
    if (!searchTerm) return notification;

    return notification.filter(item =>
      item?.title?.toLowerCase().includes(searchTerm?.toLowerCase()) ||
      item?.message?.toLowerCase().includes(searchTerm?.toLowerCase())
    );
  }, [notification, searchTerm]);
  const groupedNotifications = useMemo(() =>
    groupNotifications(filteredNotifications), [filteredNotifications]
  );

  const handleMarkAllRead = async () => {
    try {
      const updated = notification.map((item: any) => ({
        ...item,
        is_read: 1,
      }));

      dispatch(homeDataSliceActions.setNotification(updated));

      const body = {
        domainId: LoginUserDetails?.domainId,
        agentId: LoginUserDetails?.userId,
        notificationId: null,
      }

      updateUnreadNotification(body);
    } catch (err) {
      console.log('handleMarkAllRead error:', err);
    }
  };


  const handleclickNoti = (data: any) => {
    try {

      dispatch(
        homeDataSliceActions.setNotification(
          notification?.map((item: any) =>
            item?.notification_id == data?.notification_id
              ? { ...item, is_read: 1 }
              : item
          )
        )
      );

      const body = {
        domainId: LoginUserDetails?.domainId,
        agentId: LoginUserDetails?.userId,
        notificationId: data?.notification_id,
      }
      if (data?.is_read === 0) {
        updateUnreadNotification(body);
      }

      dispatch(commonDataSliceActions.setActivityFlag(false));
      const type = data?.notification_type;
      let title = "";
      let path = "";
      let meetingFlag = false;

      const contacts = ['API lead', 'Email Lead', 'Manual Lead'];
      const marketing = ['pending_approval'];
      const deals = ['Deal'];

      if (contacts.includes(type)) {
        title = "Contact";
        path = `/contact/contactdetais?coid=${data?.ref_id}`;
      }
      else if (deals.includes(type) && data?.customer_id) {
        title = "Deals";
        path = `/deals/dealscontent?coid=${data?.customer_id}&dealsId=${data?.ref_id}`;
      }
      else if (marketing?.includes(type)) {
        title = "Marketing";
        path = `/marketing/pending-campaign`;
      }
      else if (type === 'Meeting') {
        title = "Contact";
        meetingFlag = true;
        path = `/contact/contactdetais?coid=${data?.ref_id}`;
      }
      else if (type === 'active_campaign') {
        title = "Marketing";
        path = `/marketing/campaign`;
      }
      else if (type === 'scheduled_campaign') {
        title = "Marketing";
        path = `/marketing/schedule-campaign`;
      }
      else if (type === 'account_creation') {
        window.open(data?.ref_id, "_blank")
      }
      else if (type === 'Invoice') {

        const sessionId: any = uuidv4() + "deals";
        const body: any = {
          query: 'Show me the invoice in a clear, well-structured format so it’s easy to read and review',
          sessionId,
          roleId: `${LoginCcaasUserDetails?.roleId || ""}`,
          org_role_id_crm: LoginCcaasUserDetails?.org_role_id_crm?.toString() ?? "",
          userId: `${LoginCcaasUserDetails?.userId || ""}`,
          domainId: `${LoginCcaasUserDetails?.domainId || ""}`,
          module: "",
          ext: `${LoginCcaasUserDetails?.ext || ""}`,
          agent: "deals",
          isBot: 0,
          customerId: `${data?.ref_id || ""}`,
          invoice: "1",
        };

        aiChatAssistSocketservice.io.emit("crm_chat", body);

        // dispatch(dealsDataSliceActions.setdealsAiAgentChat({ ...body }));
        dispatch(dealsDataSliceActions.setdealsAiAgentChatTyping({ sessionId, typing: true }));
        dispatch(
          commonDataSliceActions.setTabMenuPush({
            path: `/deals/chat-conversation?sessionId=${sessionId}`,
            menuName: "New Conversation",
            close: 1,
            type: "deals",
          })
        );

        title = "Deals";
        path = `/deals/chat-conversation?sessionId=${sessionId}`;
      }

      if (!path) return;

      dispatch(commonDataSliceActions.setActivityFlag(meetingFlag));
      const existing = appTabs?.find(
        (t: any) => t?.path?.split("/")[1] === path?.split("/")[1]
      );
      if (existing) {
        dispatch(commonDataSliceActions.setActiveAppTab(existing.id));
        navigate(path);
        close(false);
        return;
      }

      const newTab = {
        id: path,
        title,
        path,
      };

      dispatch(commonDataSliceActions.addAppTab(newTab));
      navigate(path);
      close(false);
    } catch (error) {
      console.log(error);
    }
  };

  return (<>
    <div className={styles.notificationbox} ref={modalRef} onClick={(e) => e.stopPropagation()}>
      <div className={styles.toper}>


        <div className={styles.header}>
          <div className={styles.lefts}>
            Notifications
          </div>
          <div className={styles.rights}>
            {/* <span> <img src={threedot} /></span> */}
            {/* <span onClick={(e) => {
              e.preventDefault();
              e.stopPropagation();
              handleclick();
            }}>
             </span> */}
            <div
              className={styles.markreadbtn}
              onClick={(e) => {
                e.stopPropagation();
                handleMarkAllRead();
              }}
            >
              Mark all as read
            </div>
          </div>
        </div>
        <div className={styles.tabs}>
          <span className={styles.active}>All</span>
          {/* <span>Mentions</span>
          <span>Assigned to me</span> */}
        </div>
      </div>
      <div className={styles.notificationPanel}>

        {/* Top controls */}
        {/* <div className={styles.topBar}>
          <div className={styles.search}>
            <span className={styles.icon}><img src={search} /></span>
            <input
              placeholder="Search notification by people, boards"
              value={searchTerm}
              onChange={(e) => setSearchTerm(e.target.value)}
              onClick={(e) => e.stopPropagation()}
            />          
            </div>

          <div className={styles.toggle}>
            <span>Unread only</span>
            <div className={styles.switch}>?</div>
          </div>
        </div> */}
        <div className={styles.groupedsection}>
          {Object.entries(groupedNotifications).map(([section, items]: any) => (
            <div className={styles.section} key={section}>

              <h6>{section}</h6>

              {items?.map((item: any) => (
                <div className={styles.item} key={item?.notification_id} onClick={() => handleclickNoti(item)}>
                  <div className={styles.avatar}>
                    {item?.title?.charAt(0) || "N"}
                  </div>
                  <div className={styles.content}>
                    <p>
                      <span className="boldtxt">{item?.title}</span>{" "}
                      {/* <span className={styles.blue}>{item?.notification_type}</span> */}
                    </p>
                    <span className={styles.sub}>
                      {item?.message}
                    </span>
                  </div>
                  <span style={{ display: "flex", alignItems: "center", gap: "8px", whiteSpace: "nowrap" }}>
                    {item?.is_read === 0 && (
                      <span className={`${styles.dot} ${styles.blue}`}></span>
                    )}
                    <span className={styles.time}>
                      {getTimeAgo(item?.created_at)}
                    </span>
                  </span>

                </div>
              ))}
            </div>
          ))}
        </div>

      </div>
    </div>
  </>)
} export default Notification;