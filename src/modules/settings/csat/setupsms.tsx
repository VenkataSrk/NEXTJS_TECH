import React from 'react'
import styles from "./scss/setupemail.module.scss"
import WorktualSelect from '../../../components/custom_components/WorktualSelect'
import { Link } from 'react-router-dom'

function Setupsms() {
  return (
     <div className={styles.innerbody}>
           <div className='breadcrumbs'>
               <ol>
                <li>
                    <Link to=''>Assigned Channels</Link>
                </li>
                <li>SMS
                </li>
               </ol>
            </div>

            <div className={styles.headerarea}>
                <h5>Set Up SMS Feedback (CSAT)</h5>
                <p>Set up your customer satisfaction (CSAT) survey to be sent via SMS.</p>
            </div>
            <div className='formsec formseccsat'>

                <div className='formrow'>
                    <div className='formlabels'>
                        <label htmlFor="domainname">Number(s) <span className='required'>*</span></label>
                        <span className='formelemdesc'>Choose the sender number(s) for sending feedback requests.</span>
                    </div>
                    <div className='formelements'>
                        <WorktualSelect options={[]} />
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

               
                

                <div className='formbtns'>
                    <button className='cancelbtn'>Cancel</button><button className='primarybtn'>Save</button>
                </div>
            </div>

        </div>
  )
}

export default Setupsms