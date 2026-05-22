import { useEffect, useState } from "react";
import styles from "../../assets/styles/accountinfo.module.scss";
import moment from "moment";
import TwoStepVerification from "./twostepverification";
import { GetItemFromStorage } from "../../../../base/custom_hooks/useStorage";
import { LocalStorage } from "../../../../base/custom_hooks/localStorageKeys";
import { getSecurityInfo, getSyncUpMyaccountDetails, insertSocialLogin } from "../../apiservice/myplanapi";

export default function AccountSecurity() {

  // Static password info
  // const [securityInfo] = useState<any>({
  //   password: "mypassword123",
  //   last_password_changed: "2025-01-20",
  // });

  // // Static connected accounts
  // const [syncedMyAccount, setSyncedMyAccount] = useState<any[]>([
  //   { social_account_type: "Google" }, // pre-connected example
  // ]);

  // const getConnectionStatus = (type: string) => {
  //   return syncedMyAccount.some(
  //     (acc) => acc.social_account_type.toLowerCase() === type.toLowerCase()
  //   );
  // };

  const userDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
    const [securityInfo, setSecurityInfo] = useState<any>({ password: "", last_password_changed: null });
    const [syncedMyAccount, setSyncedMyAccount] = useState<any[]>([]);

    const normalizeType = (type: string) => type?.toLowerCase?.() || "";

    useEffect(() => {
        fetchSecurityInfo();
        getSyncUpMyaccountFunction();
    }, []);

    const fetchSecurityInfo = async () => {
      try {
        const securityList = await getSecurityInfo();
        setSecurityInfo({
          password: securityList?.password ?? "",
          last_password_changed: securityList?.last_password_changed ?? null,
        });
      } catch (error) {
        console.error("Error fetching securityInfo list:", error);
      }
    };

    const getConnectionStatus = (type: string) => {
        if (!Array.isArray(syncedMyAccount)) return false;
        const compareType = normalizeType(type);
        return syncedMyAccount.some((account: any) => normalizeType(account?.social_account_type) === compareType);
    };


    const getSyncUpMyaccountFunction = async () => {
      try {
        const payload = { extension: userDetails?.ext };
        const getSyncUpMyaccountData = await getSyncUpMyaccountDetails(payload);
        setSyncedMyAccount(Array.isArray(getSyncUpMyaccountData) ? getSyncUpMyaccountData : []);
      } catch (error) {
        console.error("Error fetching synced social accounts:", error);
        setSyncedMyAccount([]);
      }
    }

    const handleToggleConnection = async (type: string) => {
      if (getConnectionStatus(type)) {
        await handleDisConnect(type);
        return;
      }

      // Keep existing UI behavior unchanged: optimistic local "Connect"
      // until OAuth connect widgets are wired for this screen.
      setSyncedMyAccount((prev) => [
        ...prev,
        { social_account_type: type },
      ]);
    };

    const handleDisConnect = async (type: any) => {
      const data = syncedMyAccount?.find(
        (item: any) => normalizeType(item?.social_account_type) === normalizeType(type)
      );

      if (!data?.social_account_email) {
        setSyncedMyAccount((prev) =>
          prev.filter((item: any) => normalizeType(item?.social_account_type) !== normalizeType(type))
        );
        return;
      }

      const payload = {
        myaccount_email: userDetails?.Email,
        social_account_email: data.social_account_email,
        processtype: 3,
        social_account_type: type,
        extension: userDetails?.ext,
      };

      const response = await insertSocialLogin(payload);
      if (response) {
        await getSyncUpMyaccountFunction();
      }
    }

  const maskedPassword = "*".repeat((securityInfo?.password || "").length);
  const lastPasswordChangedText = securityInfo?.last_password_changed
    ? moment(securityInfo.last_password_changed).format("MMM D, YYYY")
    : "Not available";

  return (
    <div className={styles.yourinfo}>

      {/* SECURITY TITLE
      <div className={`${styles.titlearea} ${styles.titlearecard}`}>
        <div className={styles.title}>Security</div>
        <div className={styles.desc}>
          Settings and recommendations to help you keep your account secure
        </div>
      </div> */}

      {/* PASSWORD */}
      <div className={styles.titlearea}>
        <div className={styles.title}>Password</div>
        <div className={styles.desc}>
          A secure password helps protect your account
        </div>
      </div>

      <div className={styles.password}>
        <div className={styles.passwordleft}>
          <div className={styles.passwordinpuw}>
            <p>Password</p>
          </div>

          <div className={styles.passwordinpuw}>
            <span>
              {maskedPassword}
            </span>

            <p className={styles.passwordchan}>
              Last changed{" "}
              {lastPasswordChangedText}
            </p>
          </div>
        </div>

        <div className={styles.passwordright}>
          <svg width="6" height="12">
            <path d="M0.749951 11.3309..." fill="#404040" />
          </svg>
        </div>
      </div>

      {/* TWO STEP */}
      <div className={styles.titlearea}>
        <div className={styles.title}>Two-step verification</div>
        <div className={styles.desc}>
          Prevent hackers from accessing your account with an additional layer of security.
        </div>
      </div>

      <TwoStepVerification />

      {/* SOCIAL SIGN IN */}
      <div className={`${styles.titlearea} ${styles.titlearecard}`}>
        <div className={styles.title}>Social Sign-in</div>
        <div className={styles.desc}>
          Connect your Worktual account with your social media accounts for simple and secure sign-in.
        </div>
      </div>

      <div className={styles.yourinfotop}>

        {/* GOOGLE */}
        <div className={`${styles.fieldareamain} ${styles.fieldconnect}`}>
          <div className={styles.label}>
            {/* <img src={assets.Google} alt="" /> */}
            Google
          </div>
          <div className={styles.icon}>
            <div className={styles.socialmediainner}>
              <button onClick={() => handleToggleConnection("Google")}>
                {getConnectionStatus("Google") ? "Connected" : "Connect"}
              </button>
            </div>
          </div>
        </div>

        {/* APPLE */}
        <div className={`${styles.fieldareamain} ${styles.fieldconnect}`}>
          <div className={styles.label}>
            {/* <img src={assets.appleIcon} alt="" /> */}
            Apple
          </div>
          <div className={styles.icon}>
            <div className={styles.socialmediainner}>
              <button onClick={() => handleToggleConnection("Apple")}>
                {getConnectionStatus("Apple") ? "Connected" : "Connect"}
              </button>
            </div>
          </div>
        </div>

        {/* MICROSOFT */}
        <div className={`${styles.fieldareamain} ${styles.fieldconnect}`}>
          <div className={styles.label}>
            {/* <img src={assets.microSoftIcon} alt="" /> */}
            Microsoft
          </div>
          <div className={styles.icon}>
            <div className={styles.socialmediainner}>
              <button onClick={() => handleToggleConnection("Microsoft")}>
                {getConnectionStatus("Microsoft") ? "Connected" : "Connect"}
              </button>
            </div>
          </div>
        </div>

      </div>
    </div>
  );
}
