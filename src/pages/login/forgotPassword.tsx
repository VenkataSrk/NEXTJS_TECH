import React, { useEffect, useState } from "react";
import styles from "./loginnew.module.scss";
import { Link } from "react-router-dom";
import { loginConfig } from "./LoginConstant";
import Lottie from "lottie-react";
import groovyWalkAnimation from "../../assets/styles/br-login-animated-dots.json";

import axios from "axios";
import { config } from "../../base/constant";

//import { LoginService } from "../../base/loginService";
type ForgetPasswordProps = {
  updateforgetPassword: (
    email?: string,
    password?: string,
    isRemember?: boolean,
    component?: any
  ) => void;
  email: string;
};
type forgetPassProps = {
  Email: string;
  emailError: boolean;
  emailErrorMessage: string;
  loading: boolean;
};
const ForgotPassword = ({
  updateforgetPassword,
  email,
}: ForgetPasswordProps) => {
  const [forgetPass, setforgetPass] = useState<forgetPassProps>({
    Email: "",
    emailError: false,
    emailErrorMessage: "",
    loading: true,
  });

  useEffect(() => {
    setforgetPass({ ...forgetPass, Email: email });
  }, []);

  const onSubmit = async () => {
    const regex =
      /^(([^<>()[\]\.,;:\s@\"]+(\.[^<>()[\]\.,;:\s@\"]+)*)|(\".+\"))@(([^<>()[\]\.,;:\s@\"]+\.)+[^<>()[\]\.,;:\s@\"]{2,})$/i;
    if (forgetPass.Email === "") {
      setforgetPass({
        ...forgetPass,
        emailError: true,
        emailErrorMessage: loginConfig().EMAIL_BLANK,
      });
    } else if (regex.test(forgetPass.Email) === false) {
      setforgetPass({
        ...forgetPass,
        emailError: true,
        emailErrorMessage: loginConfig().INVLID_EMAIL,
      });
    } else {
      setforgetPass({ ...forgetPass, loading: false });
      const finalObj: any = {
        called_by: "APP",
        emailId: forgetPass.Email,
        log_id: 0,
        new_pwd: "",
        type: 1,
      };
      try {
        axios.post(`${config().MYACCOUNTSERVICE_URL}/login/Urapploginforgotpwd`, {
          "email": forgetPass.Email,
          "type": 1,
          "called_by": "WEB"
        })
          .then((value: any) => {
            if (value && value.data?.finalresponse[0]?.errcode === 0) {
              axios.post(`${config().PRICING_URL}/forgot_password_req`, {
                "requested_password_reset": "Yes",
                "password_reset_link": value.data.finalresponse[0].ResetURL,
                "firstname": value.data.finalresponse[0].first_name,
                "email": forgetPass.Email
              })
                .then(() => {
                  setforgetPass({ ...forgetPass, loading: true });
                  updateforgetPassword(forgetPass.Email, "", false, "reSet");
                })
                .catch((err) => console.log(err))
            }
            else {
              setforgetPass({
                ...forgetPass,
                emailError: true,
                emailErrorMessage: value.data?.finalresponse?.[0]?.errmsg,
              });
            }
          })
          .catch((error) => console.log(error))

        //   CoreServices.post(finalObj, "/forgot_password").then((value: any) => {
        //     if (value && value.data.statusCode === 200) {
        //       setforgetPass({ ...forgetPass, loading: true });
        //       updateforgetPassword(forgetPass.Email, "", false, "reSet");
        //     } else {
        //       setforgetPass({
        //         ...forgetPass,
        //         emailError: true,
        //         emailErrorMessage: value.data.message,
        //       });
        //     }
        //   });

      } catch (e: any) { }
    }
  };
  const clearEmail = () => {
    setforgetPass({
      ...forgetPass,
      Email: "",
      emailError: false,
      emailErrorMessage: "",
    });
  };
  const changeEmail = (e: any) => {
    setforgetPass({
      ...forgetPass,
      Email: e.target.value,
      emailError: false,
      emailErrorMessage: "",
    });
  };
  const forgotPass = () => {
    updateforgetPassword("", "", false, "login");
    clearEmail();
  };
  return (
    <>
      {/* <img src={Logo} alt="Header Logo" onClick={forgotPass} />
                  <h4>Forgot Password?</h4>
                  <p className={styles.forgotContent}>
                    Enter your email address and we'll send you a <br />
                    link to reset your password
                  </p> */}
      <div className={styles.loginmain}>
        <label onClick={forgotPass}>Email</label>
        <input
          className={styles.emailinput}
          type="text"
          id="email"
          autoFocus={true}
          placeholder="Enter your email address"
          value={forgetPass.Email}
          onChange={changeEmail}
        />
        {forgetPass.Email !== "" && (
          <span
            // className={`icon_closefill  ${styles.showClear}`}
            onClick={clearEmail}
          ></span>
        )}

        {forgetPass.emailError && (
          <span className={`${styles.errorText}  ${styles.invaliderrorText}`}>
            {forgetPass.emailErrorMessage}
          </span>
        )}
      </div>

      <div className={styles.loginmain}>
        <button onClick={onSubmit} className={`${styles.btnSignin} ${styles.btnforgot}`}>
          <span>
            {forgetPass.loading ? (
              "Send password recovery link"
            ) : (
              <Lottie
                style={{ width: 50, margin: "0 auto" }}
                animationData={groovyWalkAnimation}
              />
            )}
          </span>
        </button>

        <div className={styles.bottomlink}>
          <Link to="" onClick={forgotPass} className={styles.backLink}>
            <span className="icon_outlineleftarrow"></span>
            <span className={styles.backText}>Back to login</span>
          </Link>
        </div>


      </div>



      {/* <div className={styles.forgetpwdscreen}>
        <h5 className={styles.loginText}>Forgot Password?</h5>
        <p className={styles.forgotContent}>
          Enter your email address and we’ll send you a <br />link to reset your password
        </p>
        <div className={styles.forgetPage}>
          <div className={styles.formLabel}>
            <label className={styles.inputLabel}>Email</label>
            <div className={`${styles.mailBox}`}>
              <input className={styles.emailinput} type="text" id="email"
                autoFocus={true}
                placeholder="Enter your email address"
                value={forgetPass.Email}
                onChange={changeEmail}
              />
              {forgetPass.Email !== "" && <span className={`icon_closefill 
              ${styles.showClear}`} onClick={clearEmail}></span>}
            </div>

            {forgetPass.emailError && <p className={styles.errorText}>{forgetPass.emailErrorMessage}</p>}
          </div>

          <div className={styles.loginbtn}>
            <button onClick={onSubmit} className={styles.btnSignin}><span>{forgetPass.loading ? "Send recovery Mail" : <Lottie style={{ "width": 50, "margin": "0 auto" }}
              animationData={groovyWalkAnimation} />}</span></button>
          </div>
        </div>
        <Link to="" className={styles.backLink}>
          <span className="icon_outlineleftarrow"></span>
          <span className={styles.backText} onClick={forgotPass}>Back to login</span></Link>
      </div> */}
    </>
  );
};

export default ForgotPassword;
