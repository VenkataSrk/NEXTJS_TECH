import React from 'react'
import styles from "./scss/setupemail.module.scss"
import WorktualSelect from '../../../components/custom_components/WorktualSelect'
import { Link } from 'react-router-dom'
function Setupwhatsapp() {
  return (
    <div className={styles.innerbody}>
  <div className='breadcrumbs'>
               <ol>
                <li>
                    <Link to=''>Assigned Channels</Link>
                </li>
                <li>WhatsApp
                </li>
               </ol>
            </div>
            <div className={styles.headerarea}>
                <h5>Set Up Whatsapp Feedback (CSAT)</h5>
                <p>Set up your customer satisfaction (CSAT) survey to be sent via WhatsApp.</p>
            </div>
            <div className='formsec formseccsat'>

                <div className='formrow'>
                    <div className='formlabels'>
                        <label htmlFor="domainname">Account(s) <span className='required'>*</span></label>
                        <span className='formelemdesc'>Choose the WhatsApp account to send your CSAT survey from.</span>
                    </div>
                    <div className='formelements'>
                        <WorktualSelect options={[]} />
                    </div>
                </div>
                
               
               

               
                

                <div className='formbtns'>
                    <button className='cancelbtn'>Cancel</button><button className='primarybtn'>Save</button>
                </div>
            </div>

        </div>
  )
}

export default Setupwhatsapp