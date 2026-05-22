import { useEffect, useState } from "react";
import styles from "../../assets/styles/accountinfo.module.scss";
import CustomSwitch from "../../components/CustomSwitch";
import { getDataPrivacySet, insertUpdateDataPrivacySet } from "../../apiservice/myplanapi";
import { GetItemFromStorage } from "../../../../base/custom_hooks/useStorage";
import { LocalStorage } from "../../../../base/custom_hooks/localStorageKeys";

export default function AccountPrivacy() {

  const loginUserCcaasDetail = GetItemFromStorage(LocalStorage.LOGIN_USER_CRM_DETAILS);
  const loginUserDetail = GetItemFromStorage(LocalStorage.USER_DETAILS);
  const userId = loginUserCcaasDetail?.UserID || loginUserDetail?.UserID || loginUserDetail?.userId;
  const [dataPrivacy, setDataPrivacy] = useState({
      isWorktualDesktopApp: 0,
      isContentAnalysis: 0,
      isConnectedApps: 0,
  });

  const handleToggleChange = (key: string, value: boolean) => {
      const updatedValue = value ? 1 : 0;

      const updatedState = {
          ...dataPrivacy,
          [key]: updatedValue,
      };
      setDataPrivacy(updatedState);
      insertUpdateDataPrivacy(updatedState);
  };
  const insertUpdateDataPrivacy = async (data: any) => {
      try {
          if (!userId) return;
          const body = {
              userId,
              isWorktualDestopApp: data?.isWorktualDesktopApp,
              isContentAnalysis: data?.isContentAnalysis,
              isConnectedApps: data?.isConnectedApps,
          };

          await insertUpdateDataPrivacySet(body);

          setDataPrivacy({
              isWorktualDesktopApp: body?.isWorktualDestopApp,
              isContentAnalysis: body?.isContentAnalysis,
              isConnectedApps: body?.isConnectedApps,
          });

      } catch (error) {
          console.log("Privacy Insert Error:", error);
      }
  };

  const getDataPrivacy = async () => {
      try {
          if (!userId) return;
          const body = { userId };
          const response = await getDataPrivacySet(body);

          if (response) {
              setDataPrivacy({
                  isWorktualDesktopApp: response?.isWorktualDestopApp ? 1 : 0,
                  isContentAnalysis: response?.isContentAnalysis ? 1 : 0,
                  isConnectedApps: response?.isConnectedApps ? 1 : 0,
              });
          }
      } catch (err) {
          console.log("Privacy GET Error:", err);
      }
  };

  useEffect(() => {
      getDataPrivacy();
  }, [userId]);
  
  return (

    <div className={styles.yourinfo}>
      {/* <div className={styles.titlearea}>
        <div className={styles.title}>Data and privacy settings</div>
        <div className={styles.desc}>
          Manage how your data is used, shared, and protected across the services.
        </div>
      </div> */}

      <div className={styles.titleareanew}>
        <div className={styles.title}>Two-step verification</div>
        <div className={styles.desc}>
          You have the option to share information with Worktual about how you use our desktop apps...
        </div>
      </div>

      <div className={`${styles.password} ${styles.passwordshare}`}>
        <div className={styles.passwordleft}>
          Share information on how I use Worktual desktop apps.
        </div>
        <div className={styles.passwordright}>
          <CustomSwitch
            checked={dataPrivacy?.isWorktualDesktopApp === 1}
            label="Enable Notifications"
            onChange={(val: any) =>
              handleToggleChange("isWorktualDesktopApp", val)
            }
          />
        </div>
      </div>

      <div className={styles.titleareanew}>
        <div className={styles.title}>
          Content analysis for product improvement
        </div>
        <div className={styles.desc}>
          When you use Creative Cloud and Document Cloud apps, Worktual may analyze your content...
        </div>
        <div className={styles.desc} style={{ margin: "12px 0 0" }}>
          This setting does not allow Worktual to analyze your content to train generative AI models.
        </div>
      </div>

      <div className={`${styles.password} ${styles.passwordshare}`}>
        <div className={styles.passwordleft}>
          Share information on how I use Worktual desktop apps.
        </div>
        <div className={styles.passwordright}>
          <CustomSwitch
            checked={dataPrivacy?.isContentAnalysis === 1}
            label="Enable Notifications"
            onChange={(val: any) =>
              handleToggleChange("isContentAnalysis", val)
            }
          />
        </div>
      </div>

      <div className={styles.titleareanew}>
        <div className={styles.title}>Connected apps</div>
        <div className={styles.desc}>
          View and manage third-party applications connected to your account and profile.
        </div>
      </div>

      <div className={`${styles.password} ${styles.passwordshare}`}>
        <div className={styles.passwordleft}>
          Share information on how I use Worktual desktop apps.
        </div>
        <div className={styles.passwordright}>
          <CustomSwitch
            checked={dataPrivacy?.isConnectedApps === 1}
            label="Enable Notifications"
            onChange={(val: any) =>
              handleToggleChange("isConnectedApps", val)
            }
          />
        </div>
      </div>
    </div>
  );
}
