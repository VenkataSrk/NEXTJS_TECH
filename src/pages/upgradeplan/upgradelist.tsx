import React, { useState } from "react";
import styles from './scss/upgrade.module.scss'
import Deals from '../../assets/images/upgradeplan/deals.svg'
import Projects from '../../assets/images/upgradeplan/projects.svg'
import Upgradeplan from ".";
import { useSearchParams } from "react-router-dom";
import Whatsapp from '../../assets/images/whatsappicon.svg'
import voicecall from '../../assets/images/callwidget/calliconwidget.svg'
import bgimage from '../../assets/images/upgradelist.svg'
import Modal from "../component/modal/modal";

const Upgradelist = () => {
    const [params] = useSearchParams()
    const type = params.get('type')

    const [openModel, setOpenModel] = useState(false);

    return (
        <>
            {type === 'deals' && (<Upgradeplan
                title="Unlock Deals"
                description="Sales starts when a lead shows interest. The AI creates and updates deals automatically. It provides a customer summary before demos and logs all activities (calls, meetings, updates). After the demo, a quotation is sent, and once closed (won), a project is created."
                image={Deals}
                buttonText="Contact Sales"
                onButtonPress={() => setOpenModel(true)}
            />)}

            {type === 'projects' && (<Upgradeplan
                title="Unlock Projects"
                description="Projects are auto-assigned by AI based on skills and availability. The project manager oversees execution, while users can track progress, team members, and task ownership for complete visibility."
                image={Projects}
                buttonText="Contact Sales"
                onButtonPress={() => setOpenModel(true)}
            />)}

            {type === 'products' && (<Upgradeplan
                title="Unlock Products"
                description="All products are centrally managed in a categorized catalog with detailed information. The Product Insights page tracks revenue, growth, recent invoices, and quotations to support data-driven decisions."
                image={Projects}
                buttonText="Contact Sales"
                onButtonPress={() => setOpenModel(true)}
            />)}

            {type === 'marketing' && (<Upgradeplan
                title="Unlock Marketing"
                description="Campaigns are created by marketers and AI, with automatic personalization based on customer behavior. Users can send instantly or schedule campaigns. Sensitive campaigns require manager approval (“Pending Approval”). The module tracks performance (conversion, open, click rates) and provides AI recommendations to optimize timing and results."
                image={Projects}
                buttonText="Contact Sales"
                onButtonPress={() => setOpenModel(true)}
            />)}


            {openModel && <Modal
                header={<div className={"modalheader"}><div>Upgrade</div></div>}
                onClose={() => setOpenModel(false)}
                children={
                    <div className={styles.upgradelistbg}>
                        <img src={bgimage} alt="Upgradeicon" />
                        <div className={styles.contactsaletitle}>
                            Upgrade your plan or contact the Sales Team to unlock this feature.
                        </div>
                        <div className={styles.contentblocker}>
                            <div className={styles.contentblock}>
                                <img src={Whatsapp} alt="Whatsapp" />
                                <div className={styles.available}>WhatsApp</div>
                            </div>
                            <div className={styles.contentile}>Or</div>
                            <div className={styles.contentblock}>
                                <img src={voicecall} alt="voicecall" />
                                <div className={styles.available}>Call: +91 8428224590</div>
                            </div>
                        </div>
                    </div>
                }
            />}


        </>
    )
}
export default Upgradelist