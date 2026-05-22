import React from "react";
import styles from "./loginnew.module.scss";
import msgicon from "./images/mailicon.svg";
import { useNavigate } from "react-router-dom";

const Forgotpasswordnew = () => {
  const navigate = useNavigate();

  return (
    <>
      <div className={styles.loginmain}>
        <div
          className={`${styles.loginmainblock} ${styles.loginmainblockstp} ${styles.loginmainsecure}`}
        >
          <h2>Forgot password?</h2>
          <p>
            Enter your registered email address and we’ll send you a OTP to
            reset your password securely.
          </p>
          <div className={styles.loginblock}>
            <div className={`${styles.inputdiv} ${styles.inputdivmail}`}>
              <input
                placeholder="Enter your E-mail"
                name="email"
                maxLength={100}
                name="email"
                autoFocus={true}
              />
              <span className={styles.msgicon}>
                <img src={msgicon} alt="Messageicon" />
              </span>
            </div>
            <div className={styles.loginbutton}>
              <div
                className={`${styles.btnSignin}`}
                onClick={() => navigate("/email")}
              >
                Confirm E-mail
              </div>
              <div className={styles.forgot}>
                Don't have a Worktual account yet? <span> Sign up</span>
              </div>
              <div className={styles.forgot}>
                Already have an account?<span>Log In</span>
              </div>

              {/* <div className={styles.forgot}><span>Forget Password?</span></div> */}
            </div>
          </div>
        </div>
      </div>
    </>
  );
};
export default Forgotpasswordnew;
