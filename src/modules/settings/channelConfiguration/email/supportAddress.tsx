import React from 'react';
import styles from './scss/supportaddress.module.scss';
import ContentLayout from '../../../../layout/contentLayout';
import { assets } from '../../../../styles/assets/assets';

const SupportAddress = () => {
    return (
        <ContentLayout noPaddingBlock={true}>
            <div className={styles.supportaddrsec}>
                <div className={styles.topheading}>
                    <div className={styles.tophdlefts}>
                        <h2>Verify Worktual Support Address</h2>
                    </div>
                </div>
                <div className={styles.sablock}>
                    <div className={styles.subtopheading}>
                        <h4>Forward Email</h4>
                        <p className={styles.subtopdesctxt}>We’ll verify that email forwarding is set up by sending a unique code to your external support email address.</p>
                    </div>
                    {/* <div className={styles.sastatus}>
                        <img src={assets.tick_icon} alt="tick_icon" />Forwarding active
                    </div> */}
                    <div className={styles.sainstructions}>
                        <h4>Steps to follow:</h4>
                        <ol>
                            <li>Sign in to your email account and go to your <strong>Email Forwarding</strong> settings.</li>
                            <li>Enter support@worktual.com as the forwarding email address.</li>
                            <li>Under forwarding (POP/IMAP), enter the verification code: <strong>631580457</strong> .</li>
                            <li>Select <strong>“Forward a copy of incoming email…”</strong>  and click <strong>Save changes.</strong> </li>
                        </ol>
                    </div>
                    <button className={styles.verifybtn}>Verify</button>
                </div>

                <div className={styles.sablock}>
                    <div className={styles.subtopheading}>
                        <h4>SPF Configuration</h4>
                        <p className={styles.subtopdesctxt}>Validate your DNS mail server by adding the SPF record. This authorizes inbound and outbound emails for your domain.</p>
                    </div>
                    {/* <div className={styles.sastatus}>
                        <img src={assets.tick_icon} alt="tick_icon" />SPF record validated
                    </div> */}
                    <div className={styles.sainstructions}>
                        <h4>Steps:</h4>
                        <ol>
                            <li>Add this SPF TXT record to your domain host: <strong>0965ab45bfhd</strong> </li>
                            <li>Click  <strong>Verify</strong> once the record is updated.</li>
                        </ol>
                    </div>
                    <button className={styles.verifybtn}>Verify</button>

                </div>
                <div className={styles.sablock}>
                    <div className={styles.subtopheading}>
                        <h4>DNS Configuration</h4>
                        <p className={styles.subtopdesctxt}>Domain verification is required if you want outgoing emails to be sent from your domain.</p>
                    </div>
                    {/* <div className={styles.sastatus}>
                        <img src={assets.tick_icon} alt="tick_icon" />Domain verified
                    </div> */}
                    <div className={styles.sainstructions}>
                        <h4>Steps:</h4>
                        <ol>
                            <li>Add this SPF TXT record to your domain host: <strong>0965ab45bfhd</strong> </li>
                            <li>Click  <strong>Verify</strong> once the record is updated.</li>
                        </ol>
                    </div>
                    <button className={styles.verifybtn}>Verify</button>

                </div>
                <div className={styles.bottombtns}>
                    <button className='cancelbtn'>Cancel</button><button className='primarybtn'>Save</button>
                </div>
            </div>
        </ContentLayout>
    )
}

export default SupportAddress
