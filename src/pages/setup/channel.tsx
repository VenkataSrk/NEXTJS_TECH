
import { useRef, useState, useEffect } from "react";
import styles from "./scss/set.module.scss";

export default function ChannelSetup() {

    return (
        <>
            <div className={styles.setmainblock}>
                <div className={styles.setmainblockmain}>
                    <div className={styles.label}>Email</div>
                    <div className={styles.value}>Nova</div>
                </div>

                <div className={styles.setmainblockmain}>
                    <div className={styles.label}>Voice</div>
                    <div className={styles.value}></div>
                </div>

                <div className={styles.setmainblockmain}>
                    <div className={styles.label}>Web chat</div>
                    <div className={styles.value}></div>
                </div>

                <div className={styles.setmainblockmain}>
                    <div className={styles.label}>Social platforms</div>
                    <div className={styles.value}></div>
                </div>

                <div className={styles.setmainblockmain}>
                    <div className={styles.label}>Google Ads, LinkedIn Ads</div>
                    <div className={styles.value}></div>
                </div>
            </div>
        </>
    );
}
