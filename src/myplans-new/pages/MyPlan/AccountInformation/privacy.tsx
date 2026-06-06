import { useState } from "react";
import styles from "../../../assets/styles/accountinfo.module.scss";
import CustomSwitch from "../../../components/CustomSwitch";
import Planslogo from '../../../assets/images/myplan/planslogo.svg';


export default function AccountPrivacy() {

    return (
        <div className={styles.yourinfo}>
            <div className={`${styles.titlearea} ${styles.first}`}>
                <div className={styles.title}>Data and privacy settings</div>
                <div className={styles.desc}>Microcopy for Data and privacy settings</div>
            </div>

            <div className={styles.titlearea}>
                <div className={styles.title}>Desktop app usage</div>
                <div className={styles.desc}>You have the option to share information with Worktual about how you use our desktop apps. This information is associated with your Worktual account and allows us to provide you with a more personalized experience, as well as helps us improve product quality and features. This page allows you to change your preference on whether you share information with Worktual on how you use our desktop applications. This setting does not apply to beta apps. </div>
            </div>

            <div className={`${styles.password} ${styles.passwordshare}`}>
                <div className={styles.passwordleft}>Share information on how I use Worktual desktop apps.</div>
                <div className={styles.passwordright}>
                    <CustomSwitch
                        checked={true}
                        label="Enable Notifications"
                        onChange={(val: any) => console.log('Switch toggled:', val)}
                    />
                </div>
            </div>

            <div className={styles.titlearea}>
                <div className={styles.title}>Content analysis for product improvement</div>
                <div className={styles.desc}>When you use Creative Cloud and Document Cloud apps, Worktual may analyze your content processed or stored on our servers using techniques such as machine learning to improve our products and services. If you prefer that Worktual not do this, you can opt out of content analysis at any time. This setting does not apply when you separately choose to participate in product improvement programs.<br />This setting does not allow Worktual to analyze your content to train generative AI models.</div>
            </div>

            <div className={`${styles.password} ${styles.passwordshare}`}>
                <div className={styles.passwordleft}>Share information for Content analysis for product improvement</div>
                <div className={styles.passwordright}>
                    <CustomSwitch
                        checked={true}
                        label="Enable Notifications"
                        onChange={(val: any) => console.log('Switch toggled:', val)}
                    />
                </div>
            </div>

            <div className={styles.titlearea}>
                <div className={styles.title}>Connected apps</div>
                <div className={styles.desc}>These are the third party apps connected to your Worktual account and profile.</div>
            </div>
            <div className={`${styles.password} ${styles.passwordshare}`}>
                <div className={styles.passwordleft}>Share information on how I use Worktual desktop apps.</div>
                <div className={styles.passwordright}>
                    <CustomSwitch
                        checked={true}
                        label="Enable Notifications"
                        onChange={(val: any) => console.log('Switch toggled:', val)}
                    />
                </div>
            </div>



            {/* <div className={styles.titlearea}>
                <div className={styles.title}>Delete Worktual account</div>
                <div className={styles.desc}>You may request permanent deletion of your Worktual account at any time, provided you do not have active subscriptions or organizations.</div>
            </div> */}
            {/* <div className={styles.password}>
                <div className={styles.passwordleftinner}>
                    <div className={styles.desc}>Worktual account you are requesting to be deleted:</div>
                    <div className={styles.useracc}>
                        <div className={styles.useraccleft}>
                            <img src={Planslogo} alt="" />
                        </div>
                        <div className={styles.useraccright}>
                            <div className={styles.title}>User Name</div>
                            <div className={styles.desc}>nameexample@gmail.com</div>
                        </div>
                    </div>
                </div>
                <div className={styles.passwordright}>
                    <button>Continue</button>
                </div>
            </div> */}



        </div>
    );
}
