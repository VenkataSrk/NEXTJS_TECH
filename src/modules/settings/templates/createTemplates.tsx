import React from 'react';
import styles from './scss/createtemplate.module.scss';
import ContentLayout from '../../../layout/contentLayout';
import { Link } from 'react-router-dom';
import WorktualSelect from '../../../components/custom_components/WorktualSelect';
import { assets } from '../../../styles/assets/assets';

const CreateTemplates = () => {
    const selectOpt = [
        { value: 'one', label: 'One' }
    ]
    return (
        <ContentLayout noPaddingBlock={true}>
            <div className='breadcrumbs'>
                <ol>
                    <li>
                        <Link to=''>Whatsapp templates</Link>
                    </li>
                    <li>Create Template</li>
                </ol>
            </div>
            <div className={styles.topheading}>
                <div className={styles.tophdlefts}>
                    <h2>Create Template</h2>
                    <p className={styles.headerdesctxt}>Set up a new WhatsApp message template for sending notifications, alerts, or promotional content to customers.</p>
                </div>
            </div>
            <div className={styles.createtempsec}>
                <div className={styles.cttlefts}>
                    <form className='formsec'>
                        <div className='formrow'>
                            <div className='formlabels'>
                                <label htmlFor="">Template Name<span className='required'>*</span></label>
                                <span className='formelemdesc'>Enter a unique, descriptive name for this template. Only letters, numbers, and underscores are allowed.</span>
                            </div>
                            <div className='formelements'>
                                <input type="text" name="" id="" placeholder='Enter template name' />
                            </div>
                        </div>
                        <div className='formrow'>
                            <div className='formlabels'>
                                <label htmlFor="">WhatsApp Account<span className='required'>*</span></label>
                                <span className='formelemdesc'>Select the connected WhatsApp Business account where this template will be created.</span>
                            </div>
                            <div className='formelements'>
                                <div>
                                    <WorktualSelect placeholder='Select WhatsApp account' onChange={() => { }} options={selectOpt} />
                                </div>
                            </div>
                        </div>
                        <div className='formrow'>
                            <div className='formlabels'>
                                <label htmlFor="">Category<span className='required'>*</span></label>
                                <span className='formelemdesc'>Choose the category that best matches your template’s purpose — e.g., Transactional, Marketing, or Utility.</span>
                            </div>
                            <div className='formelements'>
                                <div>
                                    <WorktualSelect placeholder='Select category' onChange={() => { }} options={selectOpt} />
                                </div>
                            </div>
                        </div>
                        <div className='formrow'>
                            <div className='formlabels'>
                                <label htmlFor="">Language<span className='required'>*</span></label>
                                <span className='formelemdesc'>Select the language in which the template will be created. Ensure the message matches the selected language.</span>
                            </div>
                            <div className='formelements'>
                                <div>
                                    <WorktualSelect placeholder='Select language' onChange={() => { }} options={selectOpt} />
                                </div>
                            </div>
                        </div>
                        <div className='formrow'>
                            <div className='formlabels'>
                                <label htmlFor="">Header Type<span className='required'>*</span></label>
                                <span className='formelemdesc'>Choose the type of header for your template — Text, Image, Video, or None.</span>
                            </div>
                            <div className='formelements'>
                                <div>
                                    <WorktualSelect placeholder='Select header type' onChange={() => { }} options={selectOpt} />
                                </div>
                            </div>
                        </div>
                        <div className='formrow'>
                            <div className='formlabels'>
                                <label htmlFor="">Header<span className='required'>*</span></label>
                                <span className='formelemdesc'>Enter the header text or upload media based on your selected header type.</span>
                            </div>
                            <div className='formelements'>
                                <textarea name="" id="" placeholder='Enter header value'></textarea>
                            </div>
                        </div>
                        <div className='formrow'>
                            <div className='formlabels'>
                                <label htmlFor="">Message<span className='required'>*</span></label>
                                <span className='formelemdesc'>Write the main message body. Use placeholders like &#123;&#123; 1 &#125;&#125;,&#123;&#123; 2 &#125;&#125; for dynamic fields.</span>
                            </div>
                            <div className='formelements'>
                                <textarea name="" id="" placeholder='Enter header value'></textarea>
                            </div>
                        </div>
                        <div className='formrow'>
                            <div className='formlabels'>
                                <label htmlFor="">Footer</label>
                                <span className='formelemdesc'>Optional — Add a short note at the bottom of the message, e.g., “Thank you for choosing us.</span>
                            </div>
                            <div className='formelements'>
                                <input type="text" name="" id="" placeholder='Enter footer text' />
                            </div>
                        </div>
                        <div className='formrow'>
                            <div className='formlabels'>
                                <label htmlFor="">Button</label>
                                <span className='formelemdesc'>Add interactive buttons to your message. You can use up to 3 Quick Replies or 2 CTAs — combinations are not allowed.</span>
                            </div>
                            <div className='formelements'>
                                <div className={styles.radiowithlabel}>
                                    <input type="radio" name="button" id="none" /> <label htmlFor="none">None</label>
                                </div>
                                <div className={styles.radiowithlabel}>
                                    <input type="radio" name="button" id="CTA" /> <label htmlFor="CTA">CTA</label>
                                </div>
                                <div className={styles.radiowithlabel}>
                                    <input type="radio" name="button" id="quickreply" /> <label htmlFor="quickreplay">Quick Reply</label>
                                </div>
                            </div>
                        </div>
                        <div className='formbtns'>
                            <button className='cancelbtn'>Cancel</button>
                            <button className='primarybtn'>Submit</button>
                        </div>
                    </form>
                </div>
                <div className={styles.cttrights}>
                    <div className={styles.previewshowsec}>
                        <div className={styles.mobilelayoutviewsec}>
                            <div className={styles.bodywrapper}>
                                <div className={styles.mobileheader}>
                                    <img className={styles.topbarimg} src={assets.mobile_topbar} alt="topbarimg" />
                                    <img className={styles.camerafront} src={assets.camera_front} alt="camerafront" />
                                </div>
                                <div className={styles.mobilemainsec}>
                                    <div className={styles.userinfotop}>
                                        <span className={styles.backicon}>
                                            <svg xmlns="http://www.w3.org/2000/svg" width="11" height="17" viewBox="0 0 11 17" fill="none">
                                                <path d="M8.82725 16.626C9.04066 16.8309 9.31226 16.9427 9.63237 16.9427C10.2726 16.9427 10.7867 16.4584 10.7867 15.8436C10.7867 15.5363 10.6509 15.2569 10.4278 15.0427L3.63762 8.67199L10.4278 2.31995C10.6509 2.10573 10.7867 1.817 10.7867 1.51896C10.7867 0.904242 10.2726 0.419922 9.63237 0.419922C9.31226 0.419922 9.04066 0.531688 8.82725 0.736593L1.28046 7.81511C1.00885 8.05727 0.882747 8.35531 0.873047 8.6813C0.873047 9.00728 1.00885 9.2867 1.28046 9.53817L8.82725 16.626Z" fill="#007AFF" />
                                            </svg>
                                        </span>
                                        <div className={styles.phnnumcont}>
                                            <span className={styles.avatorcon}>
                                                <img src={assets.avatarimg} alt='avator_icon' />
                                            </span>
                                            <p>+447441476247</p>
                                        </div>
                                    </div>
                                    <div className={styles.contentsection}>
                                        <div className={styles.messagebox}>
                                            <p>This is a message sent by the business. You can customise it to with a ton of variants!</p>
                                            <ul>
                                                <li>Buttons — None, One, Two, Three, List</li>
                                                <li>Button type — Call to action button, quick reply buttons</li>
                                                <li>CTA type — Link, Contact Us</li>
                                                <li>Message — First message or repeat message</li>
                                                <li>Images — With image, without image</li>
                                            </ul>
                                            <p className={styles.lighttxt}>How would you rate your experience with us?</p>
                                        </div>
                                    </div>
                                </div>
                                <div className={styles.mobilefooter}>
                                    <img src={assets.message_bar} alt="messgagebottom" />
                                    <div className={styles.optionsbtm}>
                                        <span className={styles.optionsopen}></span>
                                    </div>
                                </div>
                            </div>

                        </div>
                    </div>
                </div>
            </div>
        </ContentLayout>
    )
}

export default CreateTemplates
