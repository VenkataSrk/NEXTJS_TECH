import React from "react";
import styles from "../../../assets/styles/paymentsucess.module.scss";
import success from "../../../assets/images/myplan/success.svg";

const PaymentSuccess = () => {
  return (

    <>
      <div className={styles.paymentsuccess}>
        <div className={styles.paymentimg}>
          <img src={success} alt="" />
        </div>
        <div className={styles.bigtitle}>Payment is successful</div>
        <div className={styles.paymentdetailsmain}>
          <div className={styles.paymentdetails}>
            <div className={styles.paymentdetailsleft}>Payment Id</div>
            <div className={styles.paymentdetailsright}></div>
          </div>

          <div className={styles.paymentdetails}>
            <div className={styles.paymentdetailsleft}>Amount paid</div>
            <div className={styles.paymentdetailsright}>5654645</div>
          </div>

          <div className={styles.paymentdetails}>
            <div className={styles.paymentdetailsleft}>Billing period</div>
            <div className={styles.paymentdetailsright}></div>
          </div>

          <div className={styles.paymentdetails}>
            <div className={styles.paymentdetailsleft}>Next billing date</div>
            <div className={styles.paymentdetailsright}></div>
          </div>
        </div>
        <div className={styles.buttongroup}>
          <button>Go to dashboard</button>
          <button >Download invoice</button>
        </div>
        <div className={styles.subscription}>Go to Subscription management</div>
      </div>
    </>

  );
};

export default PaymentSuccess;
