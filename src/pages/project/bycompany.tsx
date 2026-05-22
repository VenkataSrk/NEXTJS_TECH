"use-client"
import styles from './scss/allrequirement.module.scss'
const Bycompany = () => {
    const requirementData = [
        {
            name: 'Maren Vaccaro',
            company: 'TechVision Inc.',
            productTitle: 'Multi-Filter Search Enhancement',
            productDescription: 'Enable simultaneous filtering by price range...',
            priority: 'P2',
            status: 'In Progress',
            buildTitle: 'Backend engineer implementing API integration',
            serviceTitle: 'Sarah is building authentication service endpoints'
        },
        {
            name: 'John Doe',
            company: 'Nova Labs',
            productTitle: 'Real-Time Notifications',
            productDescription: 'Push notifications with WebSockets.',
            priority: 'P1',
            status: 'Blocked',
            buildTitle: 'Infra team setting up Redis streams',
            serviceTitle: 'Waiting for DevOps approval'
        }
    ];

    const requirementList = [
        {
            productTitle: 'Multi-Filter Search Enhancement',
            productDescription: 'Enable simultaneous filtering...',
            priority: 'P2',
            status: 'In Progress',
            buildTitle: 'Backend engineer implementing API integration',
            serviceTitle: 'Sarah working on auth service'
        },
        {
            productTitle: 'Real-Time Analytics',
            productDescription: 'Live dashboard using WebSockets',
            priority: 'P1',
            status: 'Blocked',
            buildTitle: 'Infra team setting up Kafka',
            serviceTitle: 'Waiting for DevOps approval'
        },
        {
            productTitle: 'Real-Time Analytics',
            productDescription: 'Live dashboard using WebSockets',
            priority: 'P1',
            status: 'Blocked',
            buildTitle: 'Infra team setting up Kafka',
            serviceTitle: 'Waiting for DevOps approval'
        }
    ];

    return (
        <>
            <div className={styles.searchcontainer}>
                {requirementData.map((item, index) => (
                    <div className={styles.requirement} key={index}>
                        <div className={styles.requirementmulti}>
                            <div className={styles.enhancementprogress}>
                                <div className={styles.liststatus}>
                                    <div className={styles.enhancetitle}>{item.name}</div>
                                    <div className={styles.enhancesubtitle}>{item.company}</div>
                                </div>
                                <div className={styles.companylister}>3 Requirements</div>
                            </div>

                        </div>
                        {requirementList.map((item, index) => (
                            <div className={styles.requirementcontainer} key={index}>
                                 <div className={styles.multicontainer}>
                                    <div className={styles.multicontainertitle}>
                                        <div className={styles.productitlename}>{item.productTitle}</div>
                                    </div>

                                    <div className={styles.prioritystatus}>{item.priority}</div>
                                </div>
                                <div className={styles.multicontainer}>
                                    <div className={styles.multicontainertitle}>
                                        <div className={styles.productitlename}>{item.productTitle}</div>
                                        <div className={styles.productrange}>{item.productDescription}</div>
                                    </div>
                                </div>

                                <div className={styles.progresscontainer}>
                                    <div className={styles.inprogress}>{item.status}</div>

                                    <div className={styles.buildingcontainer}>
                                        <div className={styles.buildingtitle}>{item.buildTitle}</div>
                                        <div className={styles.servicetitle}>{item.serviceTitle}</div>
                                    </div>
                                </div>
                            </div>
                        ))}

                    </div>
                ))}

            </div>
        </>
    )
}
export default Bycompany;