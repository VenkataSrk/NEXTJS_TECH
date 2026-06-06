import React from 'react'
import styles from "./scss/setupemail.module.scss"
import WorktualSelect from '../../../components/custom_components/WorktualSelect'
import { Link } from 'react-router-dom'

function Setupemail() {
    return (
        <div className={styles.innerbody}>
           <div className='breadcrumbs'>
               <ol>
                <li>
                    <Link to=''>Assigned Channels</Link>
                </li>
                <li>
             Set Up Email Feedback (CSAT)
                </li>
               </ol>
            </div>
            <div className={styles.headerarea}>
                <h5>Set Up Email Feedback (CSAT)</h5>
                <p>Set up your customer satisfaction (CSAT) survey to be sent via email.</p>
            </div>
            <div className='formsec formseccsat'>

                <div className='formrow'>
                    <div className='formlabels'>
                        <label htmlFor="domainname">Email ID <span className='required'>*</span></label>
                        <span className='formelemdesc'>Choose the sender email address for sending feedback requests.</span>
                    </div>
                    <div className='formelements'>
                        <WorktualSelect options={[]} />
                    </div>
                </div>
                <div className='formrow'>
                    <div className='formlabels'>
                        <label htmlFor="domainname">Sender Email <span className='required'>*</span></label>
                        <span className='formelemdesc'>Choose whether to send feedback requests from the same email ID or a different one.</span>
                    </div>
                    <div className={`formelements ${styles.radiorowinp}`}>
                        <label htmlFor="sameemail">
                            <input id="sameemail" name="sameemail" type="radio" />
                           Send Feedback from the same E-mail ID
                        </label>
                         <label htmlFor="differentemail">
                            <input id="differentemail" name="differentemail" type="radio" />
                           Send Feedback from the different E-mail ID
                        </label>
                    </div>
                </div>
                <div className='formrow'>
                    <div className='formlabels'>
                        <label htmlFor="domainname">Subject<span className='required'>*</span></label>
                        <span className='formelemdesc'>Enter the email subject line — keep it short, clear, and engaging (e.g., “We value your feedback”).</span>
                    </div>
                    <div className='formelements'>
                        <input type='text' placeholder='We’d love your feedback on [Product/Service]'/>
                    </div>
                </div>
                  <div className='formrow'>
                    <div className='formlabels'>
                        <label htmlFor="domainname">Message<span className='required'>*</span></label>
                        <span className='formelemdesc'>Write the survey email content. Use placeholders like [Customer Name] for personalization and include the feedback link.</span>
                    </div>
                    <div className='formelements'>
                      <textarea placeholder='Hi [Customer Name],  
How was your recent experience with us?  
Click here to share your feedback: [Link]'/>
                    </div>
                </div>
                 <div className='formrow'>
                    <div className='formlabels'>
                        <label htmlFor="domainname">Enable Rating<span className='required'>*</span></label>
                        
                    </div>
                    <div className='formelements'>
                        <div className={styles.alongwithswitchblock}>
                             <p>Allow customers to rate their experience directly from the email.</p>
                              <label className="switch">
                        <input
                            type="checkbox"
                            name="checkbox"

                        />
                        <span className="slider round"></span>
                    </label>
                        </div>
                    </div>
                </div>
                

                <div className='formbtns'>
                    <button className='cancelbtn'>Cancel</button><button className='primarybtn'>Save</button>
                </div>
            </div>

        </div>
    )
}

export default Setupemail