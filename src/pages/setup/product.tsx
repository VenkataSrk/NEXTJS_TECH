
import { useRef, useState, useEffect } from "react";
import styles from "./scss/set.module.scss";

export default function ProductSetup() {

    return (
        <>
            <div className={styles.setmainblock}>
                <div className={styles.setmainblockmain}>
                    <div className={styles.label}>Product / service catalog</div>
                    <div className={styles.value}>Nova</div>
                </div>

                <div className={styles.setmainblockmain}>
                    <div className={styles.label}>Pricing model</div>
                    <div className={styles.value}></div>
                </div>

                <div className={styles.setmainblockmain}>
                    <div className={styles.label}>Packaging & plans</div>
                    <div className={styles.value}></div>
                </div>

                <div className={styles.setmainblockmain}>
                    <div className={styles.label}>Discount limits</div>
                    <div className={styles.value}></div>
                </div>

                <div className={styles.setmainblockmain}>
                    <div className={styles.label}>Taxes & compliance rules</div>
                    <div className={styles.value}></div>
                </div>
                
                <div className={styles.setmainblockmain}>
                    <div className={styles.label}>Currency handling</div>
                    <div className={styles.value}></div>
                </div>

                <div className={styles.setmainblockmain}>
                    <div className={styles.label}>Quote → order → invoice flow</div>
                    <div className={styles.value}></div>
                </div>
            </div>
        </>
    );
}
