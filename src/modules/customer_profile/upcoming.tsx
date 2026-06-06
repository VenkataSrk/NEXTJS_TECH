import React from 'react';
import style from './scss/customer.module.scss';
import { leftnavassets } from '../../styles/assets/assets';
const Upcoming = () => {
    return(
        <>
            <div className={style.journeycontainer}>
              <div className={`${style.microjourney} ${style.microjourneypage}`}>
                 <h3>Next best action</h3>
                 <p>Customer Journey Microcopy</p>
                 <h4>Email</h4>
                 <div className={style.mailreceive}>
                    <img src={leftnavassets.Mail_receive} alt="" />
                    <p>Email</p>
                 </div>
                 <p>"Thanks for your purchase! You might also like these matching products."</p>
                 <span>Tomorrow, 9:00 AM</span>
              </div>
               <div className={style.microjourney}>
                 <h3>Push Notification – Accessory Bundle Offer</h3>
                 
                 <div className={style.mailreceive}>
                    <img src={leftnavassets.Mail_receive} alt="" />
                    <p>Email</p>
                 </div>
                 <p>"Bundle and save – add accessories for 15% off today only."</p>
                 <span>25/09/2025, 5:00 PM</span>
              </div>
               <div className={style.microjourney}>
                 <h3>Follow-Up SMS for Abandoned Cart Item</h3>
                 
                 <div className={style.mailreceive}>
                    <img src={leftnavassets.Cart_list} alt="" />
                    <p>Transaction</p>
                 </div>
                 <p>"Still thinking about the coffee table? It’s waiting for you – now with free delivery!"</p>
                 <span>2 days later, 10:00 AM</span>
              </div>
          </div>
        </>
    )
};
export default Upcoming;
