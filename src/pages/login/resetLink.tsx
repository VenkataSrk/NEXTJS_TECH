import { Col, Container, Row } from "react-bootstrap";
import { Link, useNavigate } from "react-router-dom";

import { CoreServices } from "../../base/service/apiservices";
// import recentlink from "../../assets/images/loginreset.svg";
import styles from "./loginnew.module.scss";
import { useState } from "react";

type ResetLinkProps = {
  updateforgetPassword: (email?: string, password?: string, isRemember?: boolean, component?: any) => void;
  email: string;
};
const ResetLink = ({ updateforgetPassword, email }: ResetLinkProps) => {

  const [disabled, setDisabled] = useState<boolean>(false);

  const onSubmit = async () => {


    if (email) {
      try {
        const finalObj: any = {
          called_by: "APP",
          emailId: email,
          log_id: 0,
          new_pwd: "",
          type: 1,
        };
        CoreServices.post(finalObj, "/forgot_password").then((value: any) => {
          if (value && value.data.statusCode === 200) {
            updateforgetPassword("", "", false, "reSet");
            setDisabled(true);
          }
        });
      } catch (error: any) {}
    }
  };

  return (
    <>
      {/* <div className={styles.loadingSec}>
        <object data={loaderblue} type="image/svg+xml">Loading...</object>
      </div> */}

      <Container fluid className={styles.mainloginscreen}>
        <Container>
          <Row className={styles.resetlink}>
            <Col sm={12}>
              <div className={styles.resetSection}>
                {/* <img src={Logo} alt="Header Logo" className={styles.logoIcon} onClick={redirectLogin} /> */}
                <div className={styles.resetpassLink}>
                  {/* <img src={recentlink} alt="" /> */}
                  <h1>Reset password link sent to email</h1>
                  <br />
                  <p className={styles.resetLinkDesc}>Please check your email in order to continue</p>
                  <div className={styles.linksTag}>
                    <Link to="" onClick={() => updateforgetPassword("", "", false, "login")} className={styles.backLink}>
                      <span className="icon_outlineleftarrow"></span>
                      <span className={styles.backText}>Back to login</span>
                    </Link>
                    <br />
                    <span className={`${styles.resendLink}  ${disabled ? styles.disabled : ""}`} onClick={onSubmit}>
                      {"Resend recovery link"}
                    </span>
                  </div>
                </div>
              </div>
            </Col>
          </Row>
        </Container>
      </Container>
    </>
  );
};

export default ResetLink;
