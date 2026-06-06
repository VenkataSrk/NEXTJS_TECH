import React from "react";
import styles from "./scss/config.module.scss";

interface SignUpStatusProps {
  status: string | null;
  onGoBack: () => void;
}

const SignupStatus: React.FC<SignUpStatusProps> = ({ status, onGoBack }) => {
  const getMessage = () => {
    switch (status) {
      case "success":
      case "updated":
        return "Integration Successful";
      case "failed":
        return "Integration Failed. Please go back and try again";
      default:
        return "";
    }
  };

  return (
    <div className={styles.integrationMessage}>
      <h1>{getMessage()}</h1>
      <button className='primarybtn' onClick={onGoBack}>
        Go Back
      </button>
    </div>
  );
};

export default SignupStatus;
