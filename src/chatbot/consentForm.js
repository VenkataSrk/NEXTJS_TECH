import React, { useEffect, useState } from "react";
import ChatBot from "./index";
import ChatScreen from "./chatscreen";
import { useDispatch, useSelector } from "react-redux";
import { messageActions } from "../store/slice/message_slice";
import FooterLogo from "./../images/chatbotpoweredlogo1.png";
import "../chatPreview.scss";
import { setItemWithExpiry } from './utils2.0'

function ConsentForm() {
  const messageList = useSelector((state) => state.message);
  console.log("messageList", messageList)
  const [showDeclineMessage, setShowDeclineMessage] = useState(false);
  const [consentMessage, setConsentMessage] = useState();
  const dispatch = useDispatch();

  const handleOptin = () => {
    dispatch(messageActions.isConsentForm(false));
    messageList.chatVisible ? <ChatBot /> : <ChatScreen />;
    setItemWithExpiry("privacyPolicy", true, 24 * 60 * 60 * 1000);
  };
  const handleOptout = () => {
    setShowDeclineMessage(true);
  };

  const handleprivacyPolicy = () => {
    window.open(messageList.personalisation.privacyPolicyLink, "_blank")
  }



  return (
    <div className="chatBody">
      <div className="scroller chatbotformtitle">
        {showDeclineMessage ? (
          <div>
            <p className="consentback" onClick={() => setShowDeclineMessage(false)}>
              <svg width="36" height="36" viewBox="0 0 36 36" fill="none" xmlns="http://www.w3.org/2000/svg">
                <g filter="url(#filter0_d_10479_166341)">
                  <rect x="2" y="1" width="32" height="32" rx="16" fill="white" />
                  <rect x="2.1" y="1.1" width="31.8" height="31.8" rx="15.9" stroke="#D2D3D4" stroke-width="0.2" />
                  <g clip-path="url(#clip0_10479_166341)">
                    <path d="M25.7646 16.2519H11.4355L15.4853 12.2021C15.7783 11.9091 15.7783 11.4345 15.4853 11.1416C15.1923 10.8487 14.7177 10.8486 14.4248 11.1416L10.1548 15.4111C9.72511 15.8408 9.48981 16.4106 9.49221 17.0156C9.49511 17.6152 9.73191 18.1767 10.1548 18.5918L14.4248 22.8623C14.5713 23.0088 14.7632 23.082 14.9551 23.082C15.147 23.082 15.3389 23.0088 15.4854 22.8623C15.7784 22.5693 15.7784 22.0947 15.4854 21.8018L11.4361 17.752H25.7647C26.1788 17.752 26.5147 17.4161 26.5147 17.002C26.5147 16.5879 26.1787 16.2519 25.7646 16.2519Z" fill="#252526" />
                  </g>
                </g>
                <defs>
                  <filter id="filter0_d_10479_166341" x="0" y="0" width="36" height="36" filterUnits="userSpaceOnUse" color-interpolation-filters="sRGB">
                    <feFlood flood-opacity="0" result="BackgroundImageFix" />
                    <feColorMatrix in="SourceAlpha" type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 127 0" result="hardAlpha" />
                    <feOffset dy="1" />
                    <feGaussianBlur stdDeviation="1" />
                    <feColorMatrix type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0.051 0" />
                    <feBlend mode="normal" in2="BackgroundImageFix" result="effect1_dropShadow_10479_166341" />
                    <feBlend mode="normal" in="SourceGraphic" in2="effect1_dropShadow_10479_166341" result="shape" />
                  </filter>
                  <clipPath id="clip0_10479_166341">
                    <rect x="6" y="5" width="24" height="24" rx="10" fill="white" />
                  </clipPath>
                </defs>
              </svg>
              Back</p>
            <h4>Data Privacy Bot</h4>
            {/* <p>{messageList?.personalisation?.optOutConfirmation}</p> */}
            <div dangerouslySetInnerHTML={{ __html: messageList?.personalisation?.optOutConfirmation }} />
            <a href={messageList?.personalisation?.privacyPolicyLink} target="_blank">
              View our privacy policy
            </a>
          </div>
        ) : (
          <div className="formchat">
            <h4>Data Privacy Bot</h4>
            <div dangerouslySetInnerHTML={{ __html: messageList?.personalisation?.consentMessage.replaceAll('[Company Name]', messageList?.domainDetails[0]?.companyName) }} />
            <a href={messageList?.personalisation?.privacyPolicyLink ?? ""} target="_blank">
              View our privacy policy
            </a>
            <br></br>
            <div className="btnSection">
            <button className="btnCancel" onClick={handleOptout}> {messageList?.personalisation?.optOutButton}  </button>
              <button className="btnGreen" onClick={handleOptin}>{messageList?.personalisation?.optInButton}</button>
            </div>
          </div>
        )}
      </div>
      <div className="footercopyright footertopspace" >
        <div className="footercopys">
          <p>Powered by</p>
          <img src={FooterLogo} alt="" onClick={() => window.open('https://worktual.co.uk/', "_blank")} />

        </div>
        <p>By chatting, you consent to this chat being stored according to our privacy policy</p>
      </div>
    </div>
  );
}

export default ConsentForm;
