
import { useEffect, useMemo, useState } from "react";
import { getBusinessprofileDetails } from "../../base/service/setup";
import styles from "./scss/set.module.scss";

// type CompanyProfile = {
//     company_name?: string;
//     industry?: string;
//     business_model?: string;
//     company_size?: string | number;
//     regions?: string[] | string;
// };

export default function BusinessSetup() {
    const [companyProfile, setCompanyProfile] = useState<any>(null);

    useEffect(() => {
        const fetchBusinessProfile = async () => {
            const profile = await getBusinessprofileDetails();
            setCompanyProfile(profile);
        };

        fetchBusinessProfile();
    }, []);

    const regionText = useMemo(() => {
        const regions = companyProfile?.regions;
        if (!regions) return "-";
        if (Array.isArray(regions)) return regions.length ? regions.join(", ") : "-";
        return String(regions).trim() || "-";
    }, [companyProfile]);

    return (
        <>
        <div className={styles.setdown}>
                <div className={styles.setmainblock}>
                <div className={styles.setmainblockmain}>
                    <div className={styles.label}>Business name</div>
                    <div className={styles.value}>{companyProfile?.company_name ?? "-"}</div>
                </div>

                <div className={styles.setmainblockmain}>
                    <div className={styles.label}>Industry / Vertical</div>
                    <div className={styles.value}>{companyProfile?.industry ?? "-"}</div>
                </div>

                <div className={styles.setmainblockmain}>
                    <div className={styles.label}>Business model</div>
                    <div className={styles.value}>{companyProfile?.business_model ?? "-"}</div>
                </div>

                <div className={styles.setmainblockmain}>
                    <div className={styles.label}>Company size</div>
                    <div className={styles.value}>{companyProfile?.company_size ?? "-"}</div>
                </div>

                
              <div className={styles.setmainblockmain}>
                    <div className={styles.label}>Regions / Markets served</div>
                    <div className={styles.value}>{regionText}</div>
                </div>

                <div className={styles.setmainblockmain}>
                    <div className={styles.label}>Competitive Landscape</div>
                    <div className={styles.value}>{companyProfile?.competitors?.join(", ") ?? "-"}</div>
                </div>

            </div>
            </div>
        </>
    );
}
