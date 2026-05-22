import React, { useEffect, useState } from "react";
import styles from "../../assets/styles/usermanagement.module.scss";
import Skeleton from "../../skeleton";
import { useNavigate } from "react-router-dom";
import { getAllUsers } from "../../../../store/slice/users/user_management";
import { getUserManagementInfo } from "../../apiservice/myplanapi";
import { useDispatcher } from "../../../../store/redux-store/dispatch";
import { GetItemFromStorage } from "../../../../base/custom_hooks/useStorage";
import { LocalStorage } from "../../../../base/custom_hooks/localStorageKeys";
import { getInitials } from "../../../../base/utils";
import { useSelector } from "react-redux";

const Useractivityview = () => {

  const navigate = useNavigate();
  const dispatch = useDispatcher();
  const userDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
  const [profileData, setProfileData] = useState<any[]>([]);
  const [loader, setLoader] = useState(false);
  const userList: any = GetItemFromStorage(LocalStorage.LOGIN_USER_CRM_DETAILS);

  useEffect(() => {
      dispatch(getAllUsers());
      if (userDetails) {
          setLoader(true);
          getUserManagementInfo()
              .then((res: any) => {
                  setLoader(false)
                  setProfileData(res?.data?.result || []);
              })
              .catch((err: any) => {console.log(err); setLoader(false)});
      }
      return () => {
          console.log("cleanup");
          setLoader(false)
      };
  }, []);
  
  const formatToReadableDate = (isoDate: string) => {
      if (!isoDate) return "-";
      const d = new Date(isoDate);
      return d.toLocaleString("en-US", {
          month: "short",
          day: "2-digit",
          year: "numeric"
      });
  };

  const formatToTime = (isoDate: string) => {
      if (!isoDate) return "-";
      const d = new Date(isoDate);
      return d.toLocaleString("en-US", {
          hour: "2-digit",
          minute: "2-digit",
          hour12: true
      });
  };

  const goToParticularUser = (data: any) => {
      navigate(`/useradmin?userId=${data?.Idx}`, { state: data });
  }


  const callHandling = (data: any) => {
    if (data?.inboundCall && data?.callTypeId) {
        return 3;
    } else if (data?.callTypeId) {
        return 2;
    } else if (data?.inboundCall) {
        return 1;
    } else if (data?.outboundCampaignCall) {
        return 4;
    } else if (data?.is_outbound_campaign_specific_agent) {
        return 5;
    } else {
        return 0; // default case if none match
    }
  };



  return (
    <>
      <style>
        {`
          .operationmain {
            overflow: hidden!important;
          }
        `}
      </style>

      <div className={styles.createmaincontainer}>
        {/* <div className={styles.headerole}>
          <h3>Users</h3>
          <p>
            View and manage all users in your system, including roles, access,
            and activity.
          </p>
        </div> */}

        <div className={styles.usersmaincontainer}>
          <div className={styles.usersmain}>

            {loader && <Skeleton count={6} />}

            {Array.isArray(profileData) && profileData?.map((item: any, index: number) => {

            const filteredUser = userList?.filter((user: any) => user?.ext == item?.Extension_Number)[0];

            const callType = callHandling(filteredUser);

            const callTypeName = callType === 1 ? 'Inbound Call' 
              : callType === 2 ? 'Outbound Call' 
              : callType === 3 ? 'Inbound Call & Outbound Call' 
              : callType === 4 ? 'Outbound Campaign Call'
              : callType === 5 ? 'Agent Specific - Campaign Call'
              : ''

              return (
                <div className={styles.usersmaininner} key={index} onClick={() => goToParticularUser(item)}>
                  <div className={styles.usersmaininnerleft}>
                    <div className={styles.img}>
                      {getInitials(item?.username || `${item?.Firstname || ""} ${item?.Surname || ""}`)}
                    </div>

                    <div className={styles.title}>
                      <div className={styles.titleusername}>
                        <div className={styles.name}>{item?.username || `${item?.Firstname || ""} ${item?.Surname || ""}`.trim() || "-"}</div>
                        <div
                          className={`${styles.tag} ${
                            item?.Status === "Active" ? styles.green :styles.red
                          }`}
                        >
                          {item?.Status === "Active" ? "Active" : "Inactive"}
                        </div>
                      </div>

                      <div className={styles.para}>
                        {item?.org_role_id === 3 ? "Admin" : item?.org_role_id === 5 ? "Supervisor" : "Agent"}
                      </div>
                    </div>
                  </div>

                  <div className={styles.usersmaininnerright}>
                    <div className={styles.time}>
                      <span>Last Login :</span>{" "}
                      {formatToTime(item?.lastlogin)}{" "}
                      • Joined{" "}
                      {formatToReadableDate(item?.Createdate)}
                    </div>
                     <div className={styles.time}>
                      <span>Call & Campaign Access :</span>{" "}
                       {callTypeName 
                          ?  <div className={`${styles.tag} ${styles.orange}`}>{callTypeName}</div>
                          : '-'}
                    </div>
                  </div>
                </div>
              );
            })}

            {!loader && !profileData?.length && (
              <div className={styles.nodatacontainer}>
                <div className={styles.nodatabox}>
                  {/* <img src={assets.emptyData} alt="No data" /> */}
                  <div className={styles.connected}>
                    <div className={styles.callstitle}>
                      No users available
                    </div>
                    <div className={styles.callmessage}>
                      There are no users to display at the moment.
                    </div>
                  </div>
                </div>
              </div>
            )}

          </div>
        </div>
      </div>
    </>
  );
};

export default Useractivityview;
