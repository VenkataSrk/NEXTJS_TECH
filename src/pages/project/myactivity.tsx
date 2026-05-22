"use-client"
import styles from './scss/myactivity.module.scss'
const Myactivity = () => {
    return (
        <>
          <div className={styles.searchblockmain}>
            <div className={styles.searchlistline}>
            <div className={styles.activitylistcontainer}>
            <div className={styles.activitymaincontainer}>
                <div className={styles.blockcontainer} >
                        <div className={styles.innovationblock}>
                            <div className={styles.innovationblockcontainer}>
                                <div className={styles.innovationblocksubtitle}>REQ-005</div>
                                <div className={styles.innovationblocktitle}>Nova Systems</div>
                                <div className={styles.innovationsubtitle}>Yet to start</div>
                            </div>

                            <div className={styles.prioritylist}>P2</div>
                        </div>
                        <div className={styles.filteringdetails}>
                            <div className={styles.simulates}>Enable simultaneous filtering by price range and product categories. Dynamic multi-select filters with real-time results needed.</div>
                            <div className={styles.customerlist}><span>Customer : </span>Wilson Geidt</div>
                        </div>
                </div>
                <div className={styles.notifysystemcontainer}>
                    <div className={styles.estimatedlist}>
                        <div className={styles.assignedicon}>
                            <img src='' alt="" />
                        </div>
                        <div className={styles.assignedlist}><span>Assigned by : </span>William</div>
                        <div className={styles.assignedlist}><span>Estimated closed on : </span>20 May 2026</div>
                    </div>
                    <div className={styles.workedlist}>
                        <div className={styles.workedlistitle}>Worked on by:</div>
                        <div className={styles.workedicon}>
                            <div className={styles.avatarlist}>A</div>
                            <div className={styles.avatarlist}>A</div>
                            <div className={styles.avatarlist}>A</div>
                        </div>
                    </div>
                </div>
            </div>
            </div>
             <div className={styles.activitylistcontainer}>
            <div className={styles.activitymaincontainer}>
                <div className={styles.blockcontainer} >
                        <div className={styles.innovationblock}>
                            <div className={styles.innovationblockcontainer}>
                                <div className={styles.innovationblocksubtitle}>REQ-005</div>
                                <div className={styles.innovationblocktitle}>Nova Systems</div>
                                <div className={`${styles.innovationsubtitle} ${styles.progresstag} ${styles.completed}`}>Yet to start</div>
                            </div>

                            <div className={styles.prioritylist}>P2</div>
                        </div>
                        <div className={styles.filteringdetails}>
                            <div className={styles.simulates}>Enable simultaneous filtering by price range and product categories. Dynamic multi-select filters with real-time results needed.</div>
                            <div className={styles.customerlist}><span>Customer : </span>Wilson Geidt</div>
                        </div>
                </div>
                <div className={styles.notifysystemcontainer}>
                    <div className={styles.estimatedlist}>
                        <div className={styles.assignedicon}>
                            <img src='' alt="" />
                        </div>
                        <div className={styles.assignedlist}><span>Assigned by : </span>William</div>
                        <div className={styles.assignedlist}><span>Estimated closed on : </span>20 May 2026</div>
                    </div>
                    <div className={styles.workedlist}>
                        <div className={styles.workedlistitle}>Worked on by:</div>
                        <div className={styles.workedicon}>
                            <div className={styles.avatarlist}>A</div>
                            <div className={styles.avatarlist}>A</div>
                            <div className={styles.avatarlist}>A</div>
                        </div>
                    </div>
                </div>
            </div>
            </div>
            </div>
          </div>
        </>
    )
}
export default Myactivity;