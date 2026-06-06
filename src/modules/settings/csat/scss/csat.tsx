import React from 'react'
import styles from "./csat.module.scss"
import { assets } from '../../../../styles/assets/assets'
import WorktualSelect from '../../../../components/custom_components/WorktualSelect'
// import StatusDropdown from '../../../../components/custom_components/statusdropdown/statusdropdown'


function Csat() {
    return (
        <div className={styles.innercsat}>
            {/* <div className={styles.headerarea}>
                <h5>CSAT Settings</h5>
                <p>Configure and customize your Customer Satisfaction survey.</p>
            </div> */}
 <div className={styles.topheading}>
                <div className={styles.tophdlefts}>
                    <h2>CSAT Settings</h2>
                    <p className={styles.headerdesctxt}>Configure and customize your Customer Satisfaction survey.</p>
                </div>
                
            </div>

            <div className={styles.activationbar}>
                <div className={styles.lefts}>
                    <h5>CSAT Activation</h5>
                    <p>Toggle to enable or disable CSAT. You can still configure settings while it’s off.</p>
                </div>
                <div className={styles.rights}>
                    <label className="switch">
                        <input
                            type="checkbox"
                            name="checkbox"

                        />
                        <span className="slider round"></span>
                    </label>
                </div>
            </div>
            <div className='formsec formseccsat'>
                <div className='formheading'>
                    <p>Rating Design</p>
                </div>
                <div className='formrow'>
                    <div className='formlabels'>
                        <label htmlFor="domainname">Caption <span className='required'>*</span></label>
                        <span className='formelemdesc'>Displayed as the main question on your feedback form.</span>
                    </div>
                    <div className='formelements'>
                        <input type="text" name="" id="domainname" placeholder='e.g., How satisfied are you with our service?' />
                    </div>
                </div>
                <div className='formrow'>
                    <div className='formlabels'>
                        <label htmlFor="domainname">Rating design<span className='required'>*</span></label>
                        <span className='formelemdesc'>Displayed as the main question on your feedback form.</span>
                    </div>
                    <div className={styles.ratinggreycontainer}>
                      
                            <label htmlFor="fivestar">
                            <input id="fivestar" name="fivestar" type="radio" />
                            <img src={assets.fivestar} alt="Five Star Rating" />
                        </label>
                            <label htmlFor="emotions">
                            <input id="emotions" name="emotions" type="radio" />
                            <img src={assets.emojigrp} alt="emotions" />
                        </label>
                            <label htmlFor="number">
                            <input id="number" name="number" type="radio" />
                            <img src={assets.numbergrp} alt="number" />
                        </label>
                            <label htmlFor="threeemotions">
                            <input id="threeemotions" name="threeemotions" type="radio" />
                            <img src={assets.emojigrpthree} alt="threeemotions" />
                        </label>
                            <label htmlFor="block">
                            <input id="block" name="block" type="radio" />
                            <img src={assets.likewithblock} alt="block" />
                        </label>
                            <label htmlFor="twoemotion">
                            <input id="twoemotion" name="twoemotion" type="radio" />
                            <img src={assets.emojigrptwo} alt="twoemotion" />
                          </label>
                             <label htmlFor="likedislike">
                            <input id="likedislike" name="likedislike" type="radio" />
                            <img src={assets.likdislike} alt="likedislike" />
                          </label>

                    </div>
                </div>
                <div className='formrow'>
                    <div className='formlabels'>
                        <label htmlFor="domainname">Follow Up Questions<span className='required'>*</span></label>
                        
                    </div>
                    <div className='formelements'>
                        <div className={styles.alongwithswitchblock}>
                             <p>Ask an additional question after the rating to gather more details.</p>
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
                <div className='formrow'>
                    <div className='formlabels'>
                        <label htmlFor="domainname">Thank-You Note <span className='required'>*</span></label>
                        <span className='formelemdesc'>Shown after the customer submits their feedback.</span>
                    </div>
                    <div className='formelements'>
                        <input type="text" name="" id="domainname" placeholder='e.g., Thanks for your feedback! We appreciate your time.' />
                    </div>
                </div>
                  <div className='formrow'>
                    <div className='formlabels'>
                        <label htmlFor="domainname">Don’t Disturb Until <span className='required'>*</span></label>
                        <span className='formelemdesc'>Prevents sending another CSAT request to the same customer within this period.</span>
                    </div>
                    <div className='formelements'>
                         <div className={`${styles.selectcont} selectcont`}>
                                      <WorktualSelect onChange={()=>{}} options={[]} />
                                        <WorktualSelect onChange={()=>{}} options={[]} />
                         </div>
                    </div>
                </div>
                <div className='formrow'>
                    <div className='formlabels'>
                        <label className='firstlabel' htmlFor="domainname">Assigned Channels <span className='required'>*</span></label>
                        <span className='formelemdesc'>Choose the communication channels for sending CSAT survey requests to customers.</span>
                    </div>
                    <div className='formelements'>
                        <input type="text" name="" id="domainname" placeholder='e.g., How satisfied are you with our service?' />
                    </div>
                
            
                </div>
                <div className='viewtablecontainer'>
                   <div className='row'>
                        <div className='td first'>
                         Email
                        </div>
                         <div className='td second'>
{/* <StatusDropdown/> */}
                        </div>
                         <div className='td last'>
<svg xmlns="http://www.w3.org/2000/svg" width="6" height="12" viewBox="0 0 6 12" fill="none">
  <path d="M0.847607 11.6649C0.655207 11.6649 0.462808 11.5917 0.316408 11.4442C0.0239078 11.1512 0.0249076 10.6766 0.318408 10.3837L4.18271 6.53116C4.32481 6.38956 4.40291 6.20006 4.40291 5.99896C4.40291 5.79876 4.32481 5.61026 4.18271 5.46866L0.318008 1.61516C0.025008 1.32216 0.0241079 0.84756 0.316508 0.55466C0.608508 0.26076 1.08361 0.26076 1.37751 0.55266L5.24181 4.40616C5.66761 4.83096 5.90241 5.39636 5.90291 5.99896C5.90341 6.60156 5.66851 7.16696 5.24181 7.59366L1.37711 11.4462C1.23111 11.5917 1.03901 11.6649 0.847607 11.6649Z" fill="black"/>
</svg>
                        </div>
                   </div>
                     <div className='row'>
                        <div className='td first'>
                         SMS
                        </div>
                         <div className='td second'>
{/* <StatusDropdown/> */}
                        </div>
                         <div className='td last'>
<svg xmlns="http://www.w3.org/2000/svg" width="6" height="12" viewBox="0 0 6 12" fill="none">
  <path d="M0.847607 11.6649C0.655207 11.6649 0.462808 11.5917 0.316408 11.4442C0.0239078 11.1512 0.0249076 10.6766 0.318408 10.3837L4.18271 6.53116C4.32481 6.38956 4.40291 6.20006 4.40291 5.99896C4.40291 5.79876 4.32481 5.61026 4.18271 5.46866L0.318008 1.61516C0.025008 1.32216 0.0241079 0.84756 0.316508 0.55466C0.608508 0.26076 1.08361 0.26076 1.37751 0.55266L5.24181 4.40616C5.66761 4.83096 5.90241 5.39636 5.90291 5.99896C5.90341 6.60156 5.66851 7.16696 5.24181 7.59366L1.37711 11.4462C1.23111 11.5917 1.03901 11.6649 0.847607 11.6649Z" fill="black"/>
</svg>
                        </div>
                   </div>
                     <div className='row'>
                        <div className='td first'>
                         WhatsApp
                        </div>
                         <div className='td second'>
{/* <StatusDropdown/> */}
                        </div>
                         <div className='td last'>
<svg xmlns="http://www.w3.org/2000/svg" width="6" height="12" viewBox="0 0 6 12" fill="none">
  <path d="M0.847607 11.6649C0.655207 11.6649 0.462808 11.5917 0.316408 11.4442C0.0239078 11.1512 0.0249076 10.6766 0.318408 10.3837L4.18271 6.53116C4.32481 6.38956 4.40291 6.20006 4.40291 5.99896C4.40291 5.79876 4.32481 5.61026 4.18271 5.46866L0.318008 1.61516C0.025008 1.32216 0.0241079 0.84756 0.316508 0.55466C0.608508 0.26076 1.08361 0.26076 1.37751 0.55266L5.24181 4.40616C5.66761 4.83096 5.90241 5.39636 5.90291 5.99896C5.90341 6.60156 5.66851 7.16696 5.24181 7.59366L1.37711 11.4462C1.23111 11.5917 1.03901 11.6649 0.847607 11.6649Z" fill="black"/>
</svg>
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

export default Csat