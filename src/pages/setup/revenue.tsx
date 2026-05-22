
import { useRef, useState, useEffect } from "react";
import styles from "./scss/set.module.scss";

export default function RevenueSetup() {

    return (
        <>
            <div className={styles.setmainblock}>
                <div className={styles.setmainblockmain}>
                    <div className={styles.label}>Revenue recognition type</div>
                    <div className={styles.value}>Nova</div>
                </div>

                <div className={styles.setmainblockmain}>
                    <div className={styles.label}>Payment methods</div>
                    <div className={styles.value}></div>
                </div>

                <div className={styles.setmainblockmain}>
                    <div className={styles.label}>Billing frequency</div>
                    <div className={styles.value}></div>
                </div>

                <div className={styles.setmainblockmain}>
                    <div className={styles.label}>Partial payments (Y/N)</div>
                    <div className={styles.value}></div>
                </div>

                <div className={styles.setmainblockmain}>
                    <div className={styles.label}>Refund / cancellation rules</div>
                    <div className={styles.value}></div>
                </div>
                
                <div className={styles.setmainblockmain}>
                    <div className={styles.label}>Renewal logic (if applicable)</div>
                    <div className={styles.value}></div>
                </div>

            </div>
        </>
    );
}
