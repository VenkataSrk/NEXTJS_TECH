import React from 'react';
import style from './scss/knowledgebase.module.scss'
import { assets } from '../../styles/images/assets';
import micIcon from '../../styles/images/customai/mic.svg'

const Scrapedwebsite = () => {
    return(
        <>
            <div  className='overallscrollnone'>
          <div className={style.header}>
              <div className={style.lefts}>
                <h3>Scraped website</h3>
              </div>
              <div className={style.rights}>
                <span><svg xmlns="http://www.w3.org/2000/svg" width="6" height="18" viewBox="0 0 6 18" fill="none">
                  <path fill-rule="evenodd" clip-rule="evenodd" d="M3.00009 4.513C4.23399 4.513 5.24359 3.5034 5.24359 2.2695C5.24359 1.0356 4.23399 0.026001 3.00009 0.026001C1.76619 0.026001 0.756592 1.0356 0.756592 2.2695C0.756592 3.5034 1.76619 4.513 3.00009 4.513ZM3.00009 6.7565C1.76619 6.7565 0.756592 7.7661 0.756592 9C0.756592 10.2339 1.76619 11.2435 3.00009 11.2435C4.23399 11.2435 5.24359 10.2339 5.24359 9C5.24359 7.7661 4.23399 6.7565 3.00009 6.7565ZM0.756592 15.7305C0.756592 14.4966 1.76619 13.487 3.00009 13.487C4.23399 13.487 5.24359 14.4966 5.24359 15.7305C5.24359 16.9644 4.23399 17.974 3.00009 17.974C1.76619 17.974 0.756592 16.9644 0.756592 15.7305Z" fill="#404040" />
                </svg></span>
              </div>
            </div>
           <div className={style.ticketright}>
                
                     <div className={`${style.elevatebrandbody} ${style.evelatelefts}`}>
          
            <div className={style.innerbody}>
              <div className={style.promtabmain}>
                <div className={style.sewidthmain}>
                  <div className={style.inputsetrightlab}>
                     <input type="text" name="" id="" placeholder='Search' className='searchinput'/>
                  </div>
                     <div className={style.librarytabp}>
                        <div className={style.msgchatrightlib}>
                            {/* <img src={assets.Pdfup} alt="" /> */}
                            <div className={style.msgchatleftlib}>
                            <div className={style.siptrunkleftlib}>
                                <h3>SIP Trunk<span>EN</span></h3>
                                <svg xmlns="http://www.w3.org/2000/svg" width="18" height="6" viewBox="0 0 18 6" fill="none">
                                  <path fill-rule="evenodd" clip-rule="evenodd" d="M13.4869 3.00009C13.4869 4.23399 14.4965 5.24359 15.7304 5.24359C16.9643 5.24359 17.9739 4.23399 17.9739 3.00009C17.9739 1.76619 16.9643 0.756592 15.7304 0.756592C14.4965 0.756592 13.4869 1.76619 13.4869 3.00009ZM11.2434 3.00009C11.2434 1.76619 10.2338 0.756592 8.99988 0.756592C7.76598 0.756592 6.75638 1.76619 6.75638 3.00009C6.75638 4.23399 7.76598 5.24359 8.99988 5.24359C10.2338 5.24359 11.2434 4.23399 11.2434 3.00009ZM2.26938 0.756592C3.50328 0.756592 4.51288 1.76619 4.51288 3.00009C4.51288 4.23399 3.50328 5.24359 2.26938 5.24359C1.03548 5.24359 0.0258789 4.23399 0.0258789 3.00009C0.0258789 1.76619 1.03548 0.756592 2.26938 0.756592Z" fill="#252526"/>
                                </svg>
                            </div>
                            <p>Please enter your website url so we can scrape your website.</p>
                            </div>
                       </div>
                        <div className={style.msgchatrightlib}>
                            {/* <img src={assets.Worldweb} alt="" /> */}
                            <div className={style.msgchatleftlib}>
                            <div className={style.siptrunkleftlib}>
                                <h3>SIP Trunk<span>EN</span></h3>
                                <svg xmlns="http://www.w3.org/2000/svg" width="18" height="6" viewBox="0 0 18 6" fill="none">
                                  <path fill-rule="evenodd" clip-rule="evenodd" d="M13.4869 3.00009C13.4869 4.23399 14.4965 5.24359 15.7304 5.24359C16.9643 5.24359 17.9739 4.23399 17.9739 3.00009C17.9739 1.76619 16.9643 0.756592 15.7304 0.756592C14.4965 0.756592 13.4869 1.76619 13.4869 3.00009ZM11.2434 3.00009C11.2434 1.76619 10.2338 0.756592 8.99988 0.756592C7.76598 0.756592 6.75638 1.76619 6.75638 3.00009C6.75638 4.23399 7.76598 5.24359 8.99988 5.24359C10.2338 5.24359 11.2434 4.23399 11.2434 3.00009ZM2.26938 0.756592C3.50328 0.756592 4.51288 1.76619 4.51288 3.00009C4.51288 4.23399 3.50328 5.24359 2.26938 5.24359C1.03548 5.24359 0.0258789 4.23399 0.0258789 3.00009C0.0258789 1.76619 1.03548 0.756592 2.26938 0.756592Z" fill="#252526"/>
                                </svg>
                            </div>
                            <p>Please enter your website url so we can scrape your website.</p>
                            </div>
                       </div>
                        <div className={style.msgchatrightlib}>
                            {/* <img src={assets.Worldweb} alt="" /> */}
                            <div className={style.msgchatleftlib}>
                            <div className={style.siptrunkleftlib}>
                                <h3>SIP Trunk<span>EN</span></h3>
                                <svg xmlns="http://www.w3.org/2000/svg" width="18" height="6" viewBox="0 0 18 6" fill="none">
                                  <path fill-rule="evenodd" clip-rule="evenodd" d="M13.4869 3.00009C13.4869 4.23399 14.4965 5.24359 15.7304 5.24359C16.9643 5.24359 17.9739 4.23399 17.9739 3.00009C17.9739 1.76619 16.9643 0.756592 15.7304 0.756592C14.4965 0.756592 13.4869 1.76619 13.4869 3.00009ZM11.2434 3.00009C11.2434 1.76619 10.2338 0.756592 8.99988 0.756592C7.76598 0.756592 6.75638 1.76619 6.75638 3.00009C6.75638 4.23399 7.76598 5.24359 8.99988 5.24359C10.2338 5.24359 11.2434 4.23399 11.2434 3.00009ZM2.26938 0.756592C3.50328 0.756592 4.51288 1.76619 4.51288 3.00009C4.51288 4.23399 3.50328 5.24359 2.26938 5.24359C1.03548 5.24359 0.0258789 4.23399 0.0258789 3.00009C0.0258789 1.76619 1.03548 0.756592 2.26938 0.756592Z" fill="#252526"/>
                                </svg>
                            </div>
                            <p>Please enter your website url so we can scrape your website.</p>
                            </div>
                       </div>
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
export default Scrapedwebsite;
