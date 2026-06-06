import React from 'react';
import style from './scss/customer.module.scss';
import { leftnavassets } from '../../styles/assets/assets';
const Journey = () => {
    return(
        <>
          <div className={style.journeycontainer}>
              <div className={`${style.microjourney} ${style.microjourneypage}`}>
                 <h3>Journey</h3>
                 <p>Customer Journey Microcopy</p>
                 <h4>Email opened → Interest confirmed.</h4>
                 <div className={style.mailreceive}>
                    <img src={leftnavassets.Mail_receive} alt="" />
                    <p>Email</p>
                 </div>
                 <span>8:20 PM</span>
              </div>
               <div className={style.microjourney}>
                 <h3>Email Sent – Marketing Campaign</h3>
                 
                 <div className={style.mailreceive}>
                    <img src={leftnavassets.Mail_receive} alt="" />
                    <p>Email</p>
                 </div>
                 <p>On sale now – our top picks are going fast!</p>
                 <span>8:20 PM</span>
              </div>
               <div className={style.microjourney}>
                 <h3>Notification opened → Click-through to product page.</h3>
                 
                 <div className={style.mailreceive}>
                    <img src={leftnavassets.Cart_list} alt="" />
                    <p>Transaction</p>
                 </div>
                 <p>On sale now – our top picks are going fast!</p>
                 <span>8:20 PM</span>
              </div>
               <div className={style.microjourney}>
                 <h3>Notification opened → Click-through to product page.</h3>
                 
                 <div className={style.mailreceive}>
                    <img src={leftnavassets.Cart_list} alt="" />
                    <p>Transaction</p>
                 </div>
                 <p>On sale now – our top picks are going fast!</p>
                 <span>8:20 PM</span>
              </div>
               <div className={style.microjourney}>
                 <h3>Notification opened → Click-through to product page.</h3>
                 
                 <div className={style.mailreceive}>
                    <img src={leftnavassets.Cart_list} alt="" />
                    <p>Transaction</p>
                 </div>
                 <p>On sale now – our top picks are going fast!</p>
                 <span>8:20 PM</span>
              </div>
                <div className={style.microjourney}>
                 <h3>Notification opened → Click-through to product page.</h3>
                 
                 <div className={style.mailreceive}>
                    <img src={leftnavassets.Cart_list} alt="" />
                    <p>Transaction</p>
                 </div>
                 <p>On sale now – our top picks are going fast!</p>
                 <span>8:20 PM</span>
              </div>
                <div className={style.microjourney}>
                 <h3>Notification opened → Click-through to product page.</h3>
                 
                 <div className={style.mailreceive}>
                    <img src={leftnavassets.Cart_list} alt="" />
                    <p>Transaction</p>
                 </div>
                 <p>On sale now – our top picks are going fast!</p>
                 <span>8:20 PM</span>
              </div>
                <div className={style.microjourney}>
                 <h3>Notification opened → Click-through to product page.</h3>
                 
                 <div className={style.mailreceive}>
                    <img src={leftnavassets.Cart_list} alt="" />
                    <p>Transaction</p>
                 </div>
                 <p>On sale now – our top picks are going fast!</p>
                 <span>8:20 PM</span>
              </div>
                <div className={style.microjourney}>
                 <h3>Notification opened → Click-through to product page.</h3>
                 
                 <div className={style.mailreceive}>
                    <img src={leftnavassets.Cart_list} alt="" />
                    <p>Transaction</p>
                 </div>
                 <p>On sale now – our top picks are going fast!</p>
                 <span>8:20 PM</span>
              </div>
                <div className={style.microjourney}>
                 <h3>Notification opened → Click-through to product page.</h3>
                 
                 <div className={style.mailreceive}>
                    <img src={leftnavassets.Cart_list} alt="" />
                    <p>Transaction</p>
                 </div>
                 <p>On sale now – our top picks are going fast!</p>
                 <span>8:20 PM</span>
              </div>
          </div>
        </>
    )
};
export default Journey;
