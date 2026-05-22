import { useState, useEffect } from "react";
import styles from "../../assets/styles/accountinfo.module.scss";
import { GetItemFromStorage } from "../../../../base/custom_hooks/useStorage";
import { LocalStorage } from "../../../../base/custom_hooks/localStorageKeys";
import { useDispatch, useSelector } from "react-redux";
import { MyplanProfileActions } from "../../../../store/slice/my_plans/profile";
import { getUserExtensionInfo } from "../../apiservice/myplanapi";
import { roleAccess } from "../../../../base/utils";

export default function YourInfo() {

  const [profileData, setProfileData] = useState<any>(null);
  const userDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
  const LoginCcaasUserDetails = GetItemFromStorage(LocalStorage.LOGIN_USER_CRM_DETAILS);
  const myPlanUpdation: any = useSelector(
    (state: any) =>
      state?.myPlanStore?.myPlanUpdation ??
      state?.userExtension?.myPlanUpdation ??
      false
  );
  const dispatch = useDispatch();
  const [loader, setLoader] = useState(false);
  const [myplanDesc, setMyPlanDesc] = useState('');
console.log("myplanDesc",profileData)
  useEffect(() => {
      if (userDetails) {
          const body = {
              company_id: userDetails.company_id,
              orderid: userDetails.order_id,
              extension_number: Number(userDetails.ext)
          }

          getUserExtensionInfo(body)
              .then((res: any) => {
                  setProfileData(res.data.finalresponse[0]);  // store in state
                  dispatch(MyplanProfileActions.setMyPlanUpdation(false));
              })
              .catch((err: any) => console.log(err));
      }
      return () => {
          console.log("cleanup");
      };
  }, [myPlanUpdation]);

  const first = profileData?.Firstname?.[0]?.toUpperCase() || "";
  const last = profileData?.Surname?.[0]?.toUpperCase() || "";
  const initials = (first + last) || "NA";
  const roleMap: any = {
      3: "Admin",
      5: "Supervisor",
      6: "Agent"
  };

  const roleName = profileData?.Role_Name || roleMap[profileData?.org_role_id] || "Unknown";
  const contactEmail = profileData?.Email || profileData?.app_login_user_name || "Not provided";
  const contactNumber = profileData?.Mobileno || profileData?.UR_Mobileno_display || "Not provided";
  const teamName = profileData?.Department || "Not provided";
  const companyName = profileData?.company_name || "Not provided";
  const website = profileData?.companyWebsite || "Not provided";
  const companySize = profileData?.CompanySize || "Not provided";
  const vatNumber = profileData?.VAT_number || "Not provided";
  const profileSummary =
    myplanDesc ||
    `As a ${roleName} at ${companyName}, you can manage your account settings and permissions based on your assigned access.`;

  const roleText: any = {
      3: "You can manage user accounts, assign roles, configure settings, monitor system usage, and oversee billing and subscription activities. Admins ensure the platform runs smoothly and securely for all teams.",
      5: "You manage daily activities, track performance metrics, support agents, and ensure customer interactions meet organizational standards.",
      default: "As an Agent, You respond to customer inquiries, resolve issues, follow workflows, and provide high-quality service."
  };

  const roleMicroCopy: any = {
      3: "Manages system settings, performance metrics, and reporting to ensure data accuracy and drive service quality and efficiency.",
      5: "Monitors the business performance and supports agents, without managing system configurations.",
      6: "Handles customer interactions with assigned permissions to deliver high-quality service."
  };

  useEffect(() => {
      const fetchPlanDesc = async () => {
          setLoader(true);
          try {
              const data = {
                  "company_name":userDetails?.company_id,
                  "role_id": `${userDetails?.role_id}`,
                  "access_information": roleAccess[userDetails?.role_id]
              }
              const res = await getMyplanDescription(data);
              setMyPlanDesc(res || '');
          } catch (error) {
              console.error(error);
          } finally {
              setLoader(false);
          }
      };

      fetchPlanDesc();
  }, []);
  // Static data (instead of API + Redux)
  // const staticProfile = {
  //   Firstname: "Diyecedaleab",
  //   Surname: "Diyecedaleab",
  //   Email: "Diyecedaleab@example.com",
  //   Mobileno: "+91 9876543210",
  //   org_role_id: 3,
  //   role_id: 3,
  //   Department: "Customer Support",
  //   company_name: "Diyecedaleab",
  //   companyWebsite: "www.diyecedaleab.com",
  //   CompanySize: "50-100",
  //   VAT_number: "VAT123456"
  // };

  // useEffect(() => {
  //   setProfileData(staticProfile);
  // }, []);

  
  return (
    <div className={styles.yourinfo}>
      {/* <div className={`${styles.titlearea} ${styles.titlearecard}`}>
        <div className={styles.title}>
          Welcome, {profileData?.Firstname || "Not Applicable"}{" "}
          {profileData?.Surname || ""}
        </div>
        <div className={styles.desc}>
          {roleMicroCopy[profileData?.role_id]}
        </div>
      </div> */}

      <div className={`${styles.yourinfotop} ${styles.profiletop}`}>
        <div className={styles.profile}>
          <div className={styles.profileleft}>
            {initials}
          </div>
          <div className={styles.profileright}>
            <div className={styles.profiletext}>
              {profileSummary}
            </div>
          </div>
        </div>
      </div>

      <div className={styles.yourinfotop}>
        <div className={styles.yourtitle}>Contact Information</div>

        <div className={styles.fieldareamain}>
          <div className={styles.label}>Contact Email</div>
          <div className={styles.value}>{contactEmail}</div>
        </div>

        <div className={styles.fieldareamain}>
          <div className={styles.label}>Contact Number</div>
          <div className={styles.value}>{contactNumber}</div>
        </div>
      </div>

      <div className={styles.yourinfotop}>
        <div className={styles.yourtitle}>Work Details</div>

        <div className={styles.fieldareamain}>
          <div className={styles.label}>Role</div>
          <div className={styles.value}>{roleName}</div>
        </div>

        <div className={styles.fieldareamain}>
          <div className={styles.label}>Team</div>
          <div className={styles.value}>
            {teamName}
          </div>
        </div>

        <div className={styles.fieldareamain}>
          <div className={styles.label}>Organization</div>
          <div className={styles.value}>
            {companyName}
          </div>
        </div>

        <div className={styles.fieldareamain}>
          <div className={styles.label}>Company Website</div>
          <div className={styles.value}>
            {website}
          </div>
        </div>

        <div className={styles.fieldareamain}>
          <div className={styles.label}>Company Size</div>
          <div className={styles.value}>
            {companySize}
          </div>
        </div>

        <div className={styles.fieldareamain}>
          <div className={styles.label}>Company VAT Number</div>
          <div className={styles.value}>
            {vatNumber}
          </div>
        </div>
      </div>
    </div>
  );
}

async function getMyplanDescription(_data: {
  company_name: any;
  role_id: string;
  access_information: any;
}) {
  return "";
}
