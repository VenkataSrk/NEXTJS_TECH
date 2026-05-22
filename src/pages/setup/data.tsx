
import { useRef, useState, useEffect } from "react";
import styles from "./scss/set.module.scss";

export default function DataSetup() {

    return (
        <>
            <div className={styles.setmainblock}>
                <div className={styles.setmainblockmain}>
                    <div className={styles.label}>Existing tools used</div>
                    <div className={styles.value}>Nova</div>
                </div>

                <div className={styles.setmainblockmain}>
                    <div className={styles.label}>Data sources</div>
                    <div className={styles.value}></div>
                </div>

                <div className={styles.setmainblockmain}>
                    <div className={styles.label}>Field mapping preferences</div>
                    <div className={styles.value}></div>
                </div>

                <div className={styles.setmainblockmain}>
                    <div className={styles.label}>Duplicate handling rules</div>
                    <div className={styles.value}></div>
                </div>

                <div className={styles.setmainblockmain}>
                    <div className={styles.label}>Data retention policy</div>
                    <div className={styles.value}></div>
                </div>
            </div>
        </>
    );
}
