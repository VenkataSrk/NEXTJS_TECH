import React, { useEffect, useRef, useState } from 'react'
import styles from "../../assets/styles/storage.module.scss"
import moment from 'moment';
import CustomSingleSelectDropdown from '../../../component/CustomSingleSelect';
import { useDispatcher } from '../../../../store/redux-store/dispatch';
import { attachScrollPagination } from '../../../../base/custom_hooks/scrollpagination';
import { UserSettingsActions } from '../../../../store/slice/settings_slice/user_settings_slice';
import { GetItemFromStorage } from '../../../../base/custom_hooks/useStorage';
import { LocalStorage } from '../../../../base/custom_hooks/localStorageKeys';
import { useNavigate } from 'react-router-dom';
import { getStorageDatas } from '../../apiservice/myplanapi';
import { CoreServices } from '../../../../base/service/apiservices';
import Skeleton from '../../skeleton';
import { toastnotification } from '../../../../base/shared/toastmsg';

const Storage = () => {
  const loginUserCcaasDetail =
    GetItemFromStorage(LocalStorage.LOGIN_USER_CRM_DETAILS) ||
    GetItemFromStorage(LocalStorage.USER_DETAILS) ||
    {};
  // const { Option, OptGroup } = Select;
  // const { RangePicker }: any = DatePicker;
  const navigate = useNavigate();
  const [voiceRec, setvoiceRec] = useState<any[]>([]);
  // console.log("voiceRecvoiceRec", voiceRec)
  const [filter, setFilter] = useState(false);
  const [filterSearch, setFilterSearch] = useState<any>({
    nameSearch: null,
    direction: null,
    startDate: null,
    endDate: null,
    startEnd: null,
  })
  const [currentPage, setCurrentPage] = useState(1);
  const [advanceFilter, setAdvanceFilter] = useState(null);
  const [limit, setLimit] = useState(100);
  const [loader, setLoader] = useState<boolean>(true)
  const [count, setCount] = useState<any>(0);
  const [storage, setStorage] = useState<any>([]);
  const [expandedCard, setExpandedCard] = useState<string | null>(null);
  // console.log("voiceReccount", count)
  const offset = useRef<any>(0)
  const containerRef = useRef<HTMLDivElement | null>(null)
  console.log("storage>>>>>", storage)
  useEffect(() => {
    getRecordingsDetails()
  }, [currentPage, filterSearch]);

  useEffect(() => {
    const fetchStorage = async () => {
      const res = await getStorageDatas();
      if (Array.isArray(res) && Array.isArray(res[0])) {
        setStorage(res[0]);
      } else if (Array.isArray(res)) {
        setStorage(res);
      } else {
        setStorage([]);
      }
    };
    fetchStorage();
  }, []);

  const getRecordingsDetails = () => {
    setLoader(true);

    let offset = (currentPage - 1);

    let body = {
      search: filterSearch?.nameSearch ?? null,
      direction: filterSearch?.direction ?? null,
      startDate: filterSearch?.startDate ?? null,
      endDate: filterSearch?.endDate ?? null,
      agentId: loginUserCcaasDetail?.UserID ?? null,
      offset,
      limit
    };

    CoreServices.post(body, "/get_voice_user_record_info")
      .then((response: any) => {
        setLoader(false);
        if (response?.data?.statusCode === 200) {
          const rawResp = Array.isArray(response?.data?.result) ? response.data.result : [];
          const resp = rawResp.map((item: any) => ({
            ...item,
            recordUrl: item?.recordUrl || item?.recordurl || "",
            recordSize: item?.recordSize || item?.recordsize || "0",
            customerName: item?.customerName || item?.customername || "",
            customerNumber: item?.customerNumber || item?.customernumber || "",
            agentName: item?.agentName || item?.agentname || "",
          }));
          const recordCount =
            typeof response?.data?.recordCount === "number"
              ? response.data.recordCount
              : response?.data?.recordCount?.[0]?.recordCount ?? 0;
          setCount(recordCount);

          if (currentPage === 1) {
            const descsize = [...resp].sort((a: any, b: any) => (Number(b?.recordSize) || 0) - (Number(a?.recordSize) || 0));
            setvoiceRec(descsize);
          } else {
            const descsize = [...voiceRec, ...resp].sort((a: any, b: any) => (Number(b?.recordSize) || 0) - (Number(a?.recordSize) || 0));
            setvoiceRec(descsize);
          }
        } else if (currentPage === 1) {
          setvoiceRec([]);
          setCount(0);
        }
      })
      .catch(() => {
        setLoader(false);
        if (currentPage === 1) {
          setvoiceRec([]);
          setCount(0);
        }
      });
  }
  const options = [
    { key: "1", label: "Download", value: "Download" },
    ...(loginUserCcaasDetail?.roleid !== 6
      ? [{ key: "2", label: "Delete", value: "Delete" }]
      : []),
  ];

  const deleteVideoAttachment = (videoData: any) => {
    console.log("deleteVideoAttachmentdeleteVideoAttachment")

    let body = {
      rid: videoData?.rid ?? null,
    };
    console.log("downloadVideoAttachmentvideoData", body)
    getRecordingsDetails()
    CoreServices.post(body, "/delete_voice_recordings")
      .then((response: any) => {
        if (response?.data?.statusCode === 200) {
          setLoader(false);
          setCurrentPage(1);
          getRecordingsDetails();
        }
      })
      .catch(() => {
        setLoader(false);
      });
  };
  const downloadVideoAttachment = (videoData: any) => {
    // console
    // console.log("downloadVideoAttachmentvideoData",videoData)

    let filePath = videoData?.recordUrl || videoData?.recordurl;
    let fileName = filePath ? filePath?.split('/')?.pop() : "unknown_auido.mp3";
    // console.log("downloadVideoAttachment",filePath,fileName)
    var xhr = new XMLHttpRequest();
    xhr.open("GET", filePath, true);
    xhr.responseType = "blob";

    xhr.onload = function () {
      if (xhr.status === 200) {

        var urlCreator = window.URL || window.webkitURL;
        var videoUrl = urlCreator.createObjectURL(this.response);
        var tag = document.createElement("a");
        tag.href = videoUrl;
        tag.download = fileName;
        document.body.appendChild(tag);
        tag.click();
        document.body.removeChild(tag);


        toastnotification.info({
          message: `Audio downloaded successfully`,
          duration: 3000,
        });
      } else {
        toastnotification.error({
          message: `Failed to download Audio`,
          duration: 3000,
        });
      }
    };

    xhr.onerror = function () {
      toastnotification.error({
        message: `Network error during Audio download`,
        duration: 3000,
      });
    };

    xhr.send();
  };
  const handleOptionSelect = (value: any, item: any) => {
    // console.log("itemitemitem", value);
    if (value.value === "Download") {
      // console.log("Download clicked for:", item);
      downloadVideoAttachment(item);
    }

    if (value.value === "Delete") {
      console.log("Delete clicked for:", item);
      deleteVideoAttachment(item)
    }
  };
  const formatTranscriptTime = (seconds: string | number) => {
    const totalSeconds = Number(seconds);

    if (isNaN(totalSeconds) || totalSeconds <= 0) return "";

    const hours = Math.floor(totalSeconds / 3600);
    const minutes = Math.floor((totalSeconds % 3600) / 60);
    const secs = totalSeconds % 60;

    const parts = [];

    if (hours > 0) parts.push(`${hours}h`);
    if (minutes > 0) parts.push(`${minutes} mins`);
    if (secs > 0) parts.push(`${secs} secs`);

    return parts.join(" ");
  };

  const recNavigate = (i: any) => {
    let parsedTranscripts = [];

    try {
      if (Array.isArray(i?.Message)) {
        parsedTranscripts = i.Message;
      } else if (i?.Message && i.Message !== "null") {
        parsedTranscripts = JSON.parse(i.Message);
      }
    } catch (err) {
      parsedTranscripts = [];
    }
    navigate(`/videowithtranscript/${i.rid}`, {
      state: {
        agentName: i?.agentName,
        date: moment(i?.createdAt).format("MMM DD hh:mm A"),
        duration: formatTranscriptTime(i?.duration),
        summary: i?.summary ? i?.summary : "",
        videoUrl: i?.recordUrl,
        transcripts: parsedTranscripts,
        SentimentalScore: i?.SentimentalScore,
      }
    });
    console.log("formatTranscriptTime(i?.duration)", formatTranscriptTime(i?.duration))
  };
  // const fetchData = async () => {
  //   if (loader) 
  //   if (voiceRec.length >= count)
  //   setCurrentPage(prev => prev + 1);
  // };
  const fetchData = async () => {
    if (loader)
      return;

    if (voiceRec.length >= count)
      return;

    setCurrentPage(prev => prev + 1);
  };


  useEffect(() => {
    if (voiceRec.length === 0 || count === 0) return;
    if (!containerRef.current) return;

    const cleanup = attachScrollPagination(containerRef.current, fetchData);
    return () => cleanup && cleanup();
  }, [voiceRec.length, count]);
  
  const formatDuration = (seconds: string | number) => {
    const totalSeconds = Number(seconds);

    if (!totalSeconds || totalSeconds < 0) return "";

    const hours = Math.floor(totalSeconds / 3600);
    const minutes = Math.floor((totalSeconds % 3600) / 60);
    const secs = totalSeconds % 60;

    const parts = [];

    if (hours > 0) parts.push(`${hours}h`);
    if (minutes > 0) parts.push(`${minutes} mins`);
    if (secs > 0) parts.push(`${secs} secs`);

    return parts.join(" ");
  };

  const openChat = (type :any)=> {
    navigate('/details', {
      state: { 
        from: 'my_plan',
        type
      }
    });
  }
  const dispatch = useDispatcher()
  const openUpdate =()=> {
    dispatch(UserSettingsActions.setUpdateConfigModal({
      open: true,
      source: "Video",
      type: 'Upgrade'
    }));
  }
  
  const totalSize = Number(storage?.[0]?.total_size_gb ?? 0);
  const usedSize = Number(storage?.[0]?.total_size_gb_used ?? 0);
  const storagePercent = totalSize > 0 ? Math.round((usedSize / totalSize) * 100) : 0;

  return (
    <div className={styles.yourinfo}>
      <div className="scrollContainer" ref={containerRef}>

        {/* HEADER */}
        {/* <div className={`${styles.titlearea} ${styles.titlearecard}`}>
          <div className={styles.title}>Storage</div>
          <div className={styles.desc}>
            To capture the end-user's consent preferences, ensure that both the audio and video call are recorded for verification purposes
          </div>
        </div> */}

        {/* WARNING */}
        <div className={styles.header}>
          <svg xmlns="http://www.w3.org/2000/svg" width="33" height="30" viewBox="0 0 33 30" fill="none">
            <path d="M31.3953 21.2834L20.6378 2.64469C18.6065 -0.881563 13.504 -0.881563 11.4728 2.64469L0.715271 21.2834C-1.31598 24.8097 1.21902 29.2297 5.29777 29.2297H26.8128C30.8915 29.2297 33.4428 24.8097 31.3953 21.2834ZM16.9165 22.6647C16.689 22.8922 16.3803 23.0222 16.0553 23.0222C15.8928 23.0222 15.7465 22.9897 15.584 22.9247C15.4378 22.8597 15.3078 22.7784 15.194 22.6647C14.9665 22.4372 14.8365 22.1284 14.8365 21.8034C14.8365 21.4784 14.9665 21.1697 15.194 20.9422C15.3078 20.8284 15.4378 20.7309 15.584 20.6822C16.039 20.4872 16.5753 20.6009 16.9165 20.9422C17.144 21.1697 17.274 21.4784 17.274 21.8034C17.274 22.1284 17.144 22.4372 16.9165 22.6647ZM17.274 18.4072C17.274 19.0734 16.7215 19.6259 16.0553 19.6259C15.389 19.6259 14.8365 19.0734 14.8365 18.4072V7.43844C14.8365 6.77219 15.389 6.21969 16.0553 6.21969C16.7215 6.21969 17.274 6.77219 17.274 7.43844V18.4072Z" fill="#DE003D"/>
          </svg>
          <h2>{storagePercent}% storage full</h2>
          <p>You ran out of storage. Clean up space or get more storage soon.</p>
          <button onClick={() => openChat('My Plan Storage Upgrade recordings')}>Get more storage</button>
        </div>

        {/* STORAGE BAR */}
        <div className={styles.section}>
          <div className={styles.sectionTop}>
            <span className={styles.sectionTitle}>Storage used</span>
            <span className={styles.sectionValue}>{usedSize} GB of {totalSize} GB</span>
          </div>

          <div className={styles.progressBar}>
            <div className={styles.progressFill} style={{ width: `${storagePercent}%` }}></div>
          </div>
        </div>

        <h3 className={styles.title}>Your Storage Details</h3>

        {/* AUDIO CARD */}
        <div className={styles.card}>
          <div className={styles.content}>
            <h4>Audio Recordings - {storage?.[0]?.total_size_gb_used_audio ?? 0} GB</h4>
            <p>All audio recordings stored in your Worktual cloud. Store Files up to {totalSize} GB</p>
          </div>
        </div>

        {/* VIDEO CARD */}
        <div className={styles.card}>
          <div className={styles.content}>
            <h4>Video Recordings - {storage?.[0]?.total_size_gb_used_video ?? 0} GB</h4>
            <p>All video recordings stored in your Worktual cloud. Store Files up to {totalSize} GB</p>
          </div>
        </div>

        {/* UPGRADE */}
        <div className={styles.upgradeCard}>
          <div className={styles.upgradeTop}>
            <span>Get 50 GB for ₹199 / month</span>
            <div className={styles.recommended}>Recommended</div>
          </div>
          <p>Upgrade to keep all your recordings and avoid interruptions.</p>
          <button className={styles.button} onClick={() => openChat('My Plan Storage Upgrade recordings')}>Get Start</button>
        </div>

        {/* LARGE FILES */}
        <div className={styles.titlearea}>
          <div className={styles.title}>Large Files</div>
        </div>

        {/* LIST */}
        {!loader && Array.isArray(voiceRec) && voiceRec.length === 0 && (
          <div className={styles.fileItem}>
            <div className={styles.lefts}>
              <h4>No Large Files available</h4>
            </div>
          </div>
        )}

        {voiceRec.map((i: any) => (
          <div className={styles.fileItem} key={i?.rid || i?.recId || `${i?.agentName}-${i?.createdAt}`}>
            <div className={styles.lefts}>
              <h4>
                Customer :{" "}
                {(!i?.customerName || i?.customerName === "New Customer")
                  ? i?.customerNumber
                  : i?.customerName}
              </h4>

              <div className={styles.tagcontainer}>
                <span>Handled by: {i.agentName}</span>
                <span>{moment(i.createdAt).format("MMM DD YYYY hh:mm A")}</span>
              </div>

              <div className={styles.meta}>
                <span>{formatDuration(i.duration)}</span>
              </div>
            </div>

            <div className={styles.rights}>
              <span onClick={() => recNavigate(i)} style={{ cursor: 'pointer' }}>
                ▶
              </span>

              <CustomSingleSelectDropdown
                options={options}
                onChange={(selected: any) => handleOptionSelect(selected, i)}
              />
            </div>
          </div>
        ))}

      </div>
      {loader && <Skeleton count={4} />}
    </div>
  );
};

export default Storage;
