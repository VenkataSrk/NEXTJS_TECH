import React from 'react'
import styles from "../../../../assets/styles/paymentoption.module.scss"

function Paymentform() {
  return (
    <>
      <div className={styles.navigation}>
        Payment Option
        <svg xmlns="http://www.w3.org/2000/svg" width="6" height="12" viewBox="0 0 6 12" fill="none">
          <path d="M0.847363 11.6668C0.654963 11.6668 0.462564 11.5936 0.316164 11.4461C0.0236636 11.1531 0.0246635 10.6785 0.318163 10.3856L4.18246 6.53311C4.32456 6.39151 4.40266 6.20201 4.40266 6.00091C4.40266 5.80071 4.32456 5.61221 4.18246 5.47061L0.317764 1.61711C0.0247639 1.32411 0.0238637 0.849513 0.316264 0.556613C0.608264 0.262713 1.08336 0.262713 1.37726 0.554613L5.24156 4.40811C5.66736 4.83291 5.90216 5.39831 5.90266 6.00091C5.90316 6.60351 5.66826 7.16891 5.24156 7.59561L1.37686 11.4481C1.23086 11.5936 1.03876 11.6668 0.847363 11.6668Z" fill="black" />
        </svg>
        <span>Add Card</span>
      </div>
      <div className={styles.titlearea}>
        <div className={styles.titlearealeft}>
          <div className={styles.title}>Add Card details</div>
          <div className={styles.desc}>Configure your email settings for your different mails</div>
        </div>
      </div>

      <div className={styles.cardform}>
        <div className={styles.fieldmain}>
          <div className={styles.label}>Name on card<span>*</span></div>
          <div className={styles.value}>
            <input type="text" name="" placeholder="" />
          </div>
        </div>

        <div className={styles.fieldmain}>
          <div className={styles.label}>Card Number<span>*</span></div>
          <div className={styles.value}>
            <input type="text" name="" placeholder="" />
          </div>
        </div>

        <div className={styles.fieldmain}>
          <div className={styles.label}>Expiry Date<span>*</span></div>
          <div className={styles.value}>
            <input type="text" name="" placeholder="" />
            <input type="text" name="" placeholder="" />
          </div>
        </div>

        <div className={styles.fieldmain}>
          <div className={styles.label}>CVV<span>*</span></div>
          <div className={styles.value}>
            <input type="text" name="" placeholder="" />
          </div>
        </div>

        <div className={styles.titlearea}>
          <div className={styles.titlearealeft}>
            <div className={styles.title}>Address Details</div>
          </div>
        </div>

        <div className={styles.fieldmain}>
          <div className={styles.label}>Address<span>*</span></div>
          <div className={styles.value}>
            <input type="text" name="" placeholder="" />
          </div>
        </div>

        <div className={styles.fieldmain}>
          <div className={styles.label}>City<span>*</span></div>
          <div className={styles.value}>
            <input type="text" name="" placeholder="" />
          </div>
        </div>

        <div className={styles.fieldmain}>
          <div className={styles.label}>Country<span>*</span></div>
          <div className={styles.value}>
            <input type="text" name="" placeholder="" />
          </div>
        </div>

        <div className={styles.buttongroup}>
          <button>Cancel</button>
          <button >Save</button>
        </div>
      </div>


    </>
  )
}

export default Paymentform
