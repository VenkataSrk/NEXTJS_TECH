import React from "react";
import styles from "./loginnew.module.scss";
import OTPInput from "./otpinput";
import { useNavigate } from "react-router-dom";
const Emailverification = () => {
  const handleOTPChange = (otp: string) => {
    console.log("Current OTP:", otp);
  };
  const navigate = useNavigate();
  return (
    <>
      <div className={styles.loginmain}>
        <div className={`${styles.loginmainblock} ${styles.loginmainblockstp}`}>
          <h2>E-mail Verification</h2>
          <p>
            A verification code has been sent to you at{" "}
            <span>wicoy72787@gmail.com</span> Enter the code below
          </p>
          <div className={styles.loginblock}>
            <div
              className={`${styles.inputdiv} ${styles.verificationinputchange}`}
            >
              <OTPInput onChange={handleOTPChange} />
            </div>
            <div className={styles.loginbutton}>
              <>
                <div className={styles.loadingbtn}>
                  <div
                    className={`${styles.btnSignin}`}
                    onClick={() => navigate("/newpassword")}
                  >
                    Verify
                  </div>
                  <div
                    className={`${styles.btnSignin} ${styles.btnSigninemail} `}
                  >
                    <svg
                      xmlns="http://www.w3.org/2000/svg"
                      width="15"
                      height="12"
                      viewBox="0 0 15 12"
                      fill="none"
                    >
                      <path
                        d="M14.1667 5.83398L0.833415 5.83398M0.833415 5.83398L5.83341 10.834M0.833415 5.83398L5.83341 0.833984"
                        stroke="#757676"
                        stroke-width="1.66667"
                        stroke-linecap="round"
                        stroke-linejoin="round"
                      />
                    </svg>
                    Change E-mail
                  </div>
                </div>
              </>{" "}
              <div className="spinloader"></div>
              <div className={styles.loginbuttonst}>
                <div className={styles.remaining}>
                  <span>1 / 5 attempts</span> - 125 seconds remaining
                </div>
                <div className={styles.forgot}>
                  Didn’t receive the OTP? <span>Resend</span>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
    </>
  );
};
export default Emailverification;
