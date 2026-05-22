import React, { useEffect, useState } from 'react';
import styles from "../../assets/styles/accountinfo.module.scss";
import CustomSwitch from '../../components/CustomSwitch';
import { getSecurityInfo, getTwoStepVerfication, getUserExtensionInfo, insertUpdateTwoStepVerification } from '../../apiservice/myplanapi';
import { GetItemFromStorage } from '../../../../base/custom_hooks/useStorage';
import { LocalStorage } from '../../../../base/custom_hooks/localStorageKeys';

function TwoStepVerification() {

    const userDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
    const loginUserCrmDetails = GetItemFromStorage(LocalStorage.LOGIN_USER_CRM_DETAILS);
    const [profileDetails, setProfileDetails] = useState<any>({})
    const [securityInfo, setSecurityInfo] = useState<any>({ password: "" })
    const [twoStepFeilds, setTwoStepFeilds] = useState<any>({
        app_2fa_enabled: false,
        email_2fa_enabled: false,
        phone_2fa_enabled: false
    })
    console.log("profileDetails--profileDetails---->>", userDetails);

    const handleToggleFunc = (key: any, val?: boolean) => {
        setTwoStepFeilds((prev: any) => {
            const nextValue = typeof val === "boolean" ? val : !prev[key];
            const updatedFields = {
                ...prev,
                [key]: nextValue,
            };

            const body = {
                email: profileDetails?.Email || userDetails?.Email,
                companyId: userDetails?.company_id,
                PhoneNumber: profileDetails?.Mobileno,
                app_2fa_enabled: updatedFields?.app_2fa_enabled ? 1 : 0,
                email_2fa_enabled: updatedFields?.email_2fa_enabled ? 1 : 0,
                phone_2fa_enabled: updatedFields?.phone_2fa_enabled ? 1 : 0,
                password: securityInfo?.password || ""
            };
            insertUpdateTwoStep(body);
            return updatedFields;
        });
    }

    const insertUpdateTwoStep = async (body: any) => {
        try {
            const response = await insertUpdateTwoStepVerification(body);
            if (response) {
                getTwoStepList();
                return response;
            }
        } catch (error) {
            console.log("insertUpdateTwoStep", error);
        }
    }
    const getUserDetailInfo = () => {
        try {
            const companyId = userDetails?.company_id ?? userDetails?.Company_id ?? loginUserCrmDetails?.company_id;
            const orderId = userDetails?.order_id ?? userDetails?.orderid ?? loginUserCrmDetails?.order_id;
            const extensionNumber = userDetails?.ext ?? userDetails?.Extension_Number ?? userDetails?.extension_number ?? loginUserCrmDetails?.ext;

            const isMissing =
                companyId == null ||
                orderId == null ||
                extensionNumber == null ||
                extensionNumber === "";

            if (isMissing) {
                console.log("profileDetails skipped: missing payload fields", {
                    companyId,
                    orderId,
                    extensionNumber,
                    userDetails
                });
                return;
            }
            const payLoad = {
                company_id: companyId,
                orderid: orderId,
                extension_number: Number(extensionNumber)
            }
            getUserExtensionInfo(payLoad)
                .then((res: any) => {
                    const profile =
                        res?.data?.finalresponse?.[0] ??
                        res?.data?.result?.[0] ??
                        res?.finalresponse?.[0] ??
                        {};
                    console.log("profileDetails------>>", profile);
                    setProfileDetails(profile);

                })
                .catch((error: any) => {
                    console.log("getUserExtensionInfo error", error);
                })
        } catch (error) {
            console.log("userDetailsErr", error)
        }

    }
    const fetchSecurityInfo = async () => {
        try {
            const securityList = await getSecurityInfo();
            setSecurityInfo({
                password: securityList?.password || "",
            });
        } catch (error) {
            console.error("Error fetching securityInfo list:", error);
        }
    };
    const getTwoStepList = async () => {
        try {
            const body: any = {
                email: userDetails?.Email || userDetails?.emailId || profileDetails?.Email
            }
            const securityList = await getTwoStepVerfication(body);
            const newObj = {
                app_2fa_enabled: !!securityList?.app_2fa_enabled,
                email_2fa_enabled: !!securityList?.email_2fa_enabled,
                phone_2fa_enabled: !!securityList?.phone_2fa_enabled,
            };
            setTwoStepFeilds((prev: any) => ({
                ...prev,
                ...newObj,
            }));
        } catch (error) {
            console.error("Error in  twostepVerification list:", error);
        }
    }
    useEffect(() => {
        if (userDetails?.Email) {
            getTwoStepList();
        }
    }, [userDetails?.Email]);
    useEffect(() => {
        getUserDetailInfo();
        fetchSecurityInfo();
    }, []);

    return (
        <div className={`${styles.yourinfotop} ${styles.workaccessapp}`}>

            {/* Email Verification */}
            <div className={styles.fieldareamain}>
                <div className={styles.label}>Verify with email</div>
                <div className={styles.value}>
                    {profileDetails?.Email || "No email available"} (always available)
                </div>
                <div className={styles.icon}>
                    {Object.keys(twoStepFeilds).length > 0 && (
                        <CustomSwitch
                            checked={!!twoStepFeilds?.email_2fa_enabled}
                            onChange={(val: boolean) => handleToggleFunc("email_2fa_enabled", val)}
                        />
                    )}
                </div>
            </div>

            {/* SMS Verification */}
            <div className={styles.fieldareamain}>
                <div className={styles.label}>Verify with text message (SMS)</div>
                <div className={styles.value}>
                    {profileDetails?.Mobileno || "Not added"}
                </div>
                <div className={styles.icon}>
                    {Object.keys(twoStepFeilds).length > 0 && (
                        <CustomSwitch
                            checked={!!twoStepFeilds?.phone_2fa_enabled}
                            onChange={(val: boolean) => handleToggleFunc("phone_2fa_enabled", val)}
                        />
                    )}
                </div>
            </div>

        </div>
    );
}

export default TwoStepVerification;
