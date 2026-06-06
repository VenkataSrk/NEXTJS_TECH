import React, { useState } from 'react'
import { useNavigate } from "react-router-dom";

function Wixpayment() {
    const navigate = useNavigate();
    const [cardNumber, setCardNumber] = useState("")
    const [cardHolderName, setCardHolderName] = useState("")
    const [expMonth, setExpMonth] = useState("")
    const [expYear, setexpYear] = useState("")
    const [cvv, setCvv] = useState("")

    const handleSubmit = async (event) => {
        event.preventDefault();
        console.log("etse")
        let wixpaymentObj = JSON.parse(localStorage.getItem("WixPayment"))
        console.log("wixpaymentObj", wixpaymentObj)
        const createTransactionInput = {
            cardNumber: cardNumber.replace(/ /g, ''),
            securityCode: cvv,
            expiryYear: parseInt(`20${expYear}`),
            expiryMonth: parseInt(expMonth),
            holderName: cardHolderName,
            items: wixpaymentObj.items,
            amount: wixpaymentObj.amount,
            country: "GB",
            city: "New York",
            postalCode: "10003",
            firstName: wixpaymentObj.firstName,
            lastName: wixpaymentObj.lastName,
            email: wixpaymentObj.email,
            streetAddressNumber: "20",
            address: wixpaymentObj.address,
            domainId: sessionStorage.getItem("domainId")

        }
        console.log("dfsdfsdf", createTransactionInput)
        try {
            const response = await fetch('https://payment.worktual.co.uk/payment/v1/wix/createTransaction', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify(createTransactionInput),
            });

            if (!response.ok) {
                throw new Error('Network response was not ok: ' + response.statusText);
            }

            const result = await response.json();
            console.log("string", result);

            if (result.errcode === 0) {
                console.log("sdfsdf", result?.result?.transaction?.authorization?.getMethodRedirect?.url);
                window.location.href = result?.result?.transaction?.authorization?.getMethodRedirect?.url;
                localStorage.setItem("transactionId", result?.result?.transaction.id);
            } else {
                let data = {
                    authorization: {
                        status: "Failure"
                    }
                };
                console.log("dataaa", data);
                window.parent.postMessage({ type: 'WIX_PAYMENT_IFRAME', message: data }, '*');
            }
        } catch (error) {
            console.error('Error creating transaction:', error);
        }

    }

    const handleCarNumber = (e) => {
        setCardNumber(e.target.value.replace(/\D/g, '').replace(/(.{4})/g, '$1 ').trim())
    }
    const handleCardHolderName = (e) => {
        setCardHolderName(e.target.value)
    }
    const handleExpMonth = (e) => {
        if (e.target.value <= 12) {
            setExpMonth(e.target.value)
        } else {
            setExpMonth(12)
        }

    }
    const handleExpYear = (e) => {
        if (e.target.value <= 99)
            setexpYear(e.target.value)
        else
            setexpYear(99)
    }
    const handleCvv = (e) => {
        setCvv(e.target.value)
    }
    return (
        <>
            <form onSubmit={handleSubmit}>
                <div className="worktualwix">
                    <label>Card Number</label>
                    <div className="worktualicondivide">
                        <input type='text' onChange={handleCarNumber} placeholder="4242 4242 4242 4242" maxLength={19} value={cardNumber}></input>
                        <svg xmlns="http://www.w3.org/2000/svg" width="22" height="16" viewBox="0 0 22 16" fill="none">
                            <path d="M4.55688 15.808V14.7677C4.55688 14.3698 4.31449 14.1093 3.8984 14.1093C3.69035 14.1093 3.46413 14.1779 3.3086 14.4042C3.1874 14.2143 3.01369 14.1093 2.75313 14.1093C2.57942 14.1093 2.40571 14.1618 2.26836 14.3516V14.1436H1.90479V15.808H2.26836V14.8889C2.26836 14.594 2.42389 14.4546 2.66628 14.4546C2.90866 14.4546 3.02985 14.6102 3.02985 14.8889V15.808H3.39343V14.8889C3.39343 14.594 3.56714 14.4546 3.79134 14.4546C4.03373 14.4546 4.15492 14.6102 4.15492 14.8889V15.808H4.55688ZM9.94993 14.1436H9.36013V13.6406H8.99655V14.1436H8.66731V14.4728H8.99655V15.2363C8.99655 15.6181 9.15208 15.8423 9.56818 15.8423C9.72371 15.8423 9.89742 15.7898 10.0186 15.7211L9.91358 15.408C9.80854 15.4767 9.68735 15.4949 9.6005 15.4949C9.42679 15.4949 9.35811 15.3899 9.35811 15.2182V14.4728H9.94791V14.1436H9.94993ZM13.0363 14.1072C12.8283 14.1072 12.6889 14.2123 12.602 14.3496V14.1416H12.2384V15.8059H12.602V14.8687C12.602 14.592 12.7232 14.4345 12.9494 14.4345C13.0181 14.4345 13.105 14.4526 13.1757 14.4688L13.2807 14.1214C13.208 14.1072 13.105 14.1072 13.0363 14.1072ZM8.37241 14.2809C8.1987 14.1597 7.95632 14.1072 7.69576 14.1072C7.27966 14.1072 7.00294 14.3153 7.00294 14.6445C7.00294 14.9212 7.21099 15.0788 7.57456 15.1293L7.74827 15.1475C7.93814 15.1818 8.04317 15.2343 8.04317 15.3212C8.04317 15.4424 7.9038 15.5292 7.66142 15.5292C7.41903 15.5292 7.22715 15.4424 7.10595 15.3555L6.93225 15.6322C7.12211 15.7716 7.38268 15.8403 7.64324 15.8403C8.12801 15.8403 8.40675 15.6141 8.40675 15.303C8.40675 15.0081 8.18053 14.8526 7.83513 14.8L7.66142 14.7819C7.50589 14.7637 7.3847 14.7294 7.3847 14.6263C7.3847 14.5051 7.50589 14.4365 7.69778 14.4365C7.90582 14.4365 8.11387 14.5233 8.2189 14.5758L8.37241 14.2809ZM18.0456 14.1072C17.8375 14.1072 17.6982 14.2123 17.6113 14.3496V14.1416H17.2477V15.8059H17.6113V14.8687C17.6113 14.592 17.7325 14.4345 17.9587 14.4345C18.0274 14.4345 18.1143 14.4526 18.1849 14.4688L18.29 14.1254C18.2193 14.1072 18.1163 14.1072 18.0456 14.1072ZM13.3999 14.9758C13.3999 15.4787 13.7473 15.8423 14.2846 15.8423C14.527 15.8423 14.7007 15.7898 14.8744 15.6524L14.7007 15.3575C14.5613 15.4626 14.4239 15.5131 14.2664 15.5131C13.9715 15.5131 13.7635 15.305 13.7635 14.9758C13.7635 14.6627 13.9715 14.4546 14.2664 14.4385C14.4219 14.4385 14.5613 14.491 14.7007 14.594L14.8744 14.2991C14.7007 14.1597 14.527 14.1093 14.2846 14.1093C13.7473 14.1072 13.3999 14.4728 13.3999 14.9758ZM16.763 14.9758V14.1436H16.3994V14.3516C16.2782 14.1961 16.1045 14.1093 15.8783 14.1093C15.4096 14.1093 15.0461 14.4728 15.0461 14.9758C15.0461 15.4787 15.4096 15.8423 15.8783 15.8423C16.1206 15.8423 16.2943 15.7555 16.3994 15.5999V15.808H16.763V14.9758ZM15.4278 14.9758C15.4278 14.6809 15.6177 14.4385 15.9308 14.4385C16.2257 14.4385 16.4337 14.6647 16.4337 14.9758C16.4337 15.2707 16.2257 15.5131 15.9308 15.5131C15.6197 15.4949 15.4278 15.2687 15.4278 14.9758ZM11.077 14.1072C10.5923 14.1072 10.2448 14.4546 10.2448 14.9738C10.2448 15.4949 10.5923 15.8403 11.0952 15.8403C11.3376 15.8403 11.58 15.7716 11.7719 15.6141L11.5981 15.3535C11.4588 15.4585 11.2851 15.5272 11.1134 15.5272C10.8872 15.5272 10.6629 15.4222 10.6104 15.1293H11.8405C11.8405 15.0768 11.8405 15.0424 11.8405 14.9899C11.8567 14.4546 11.5436 14.1072 11.077 14.1072ZM11.077 14.4203C11.3032 14.4203 11.4588 14.5597 11.4931 14.8182H10.6266C10.6609 14.594 10.8165 14.4203 11.077 14.4203ZM20.1099 14.9758V13.4851H19.7463V14.3516C19.6251 14.1961 19.4514 14.1093 19.2252 14.1093C18.7566 14.1093 18.393 14.4728 18.393 14.9758C18.393 15.4787 18.7566 15.8423 19.2252 15.8423C19.4676 15.8423 19.6413 15.7555 19.7463 15.5999V15.808H20.1099V14.9758ZM18.7748 14.9758C18.7748 14.6809 18.9646 14.4385 19.2777 14.4385C19.5726 14.4385 19.7806 14.6647 19.7806 14.9758C19.7806 15.2707 19.5726 15.5131 19.2777 15.5131C18.9646 15.4949 18.7748 15.2687 18.7748 14.9758ZM6.60301 14.9758V14.1436H6.23943V14.3516C6.11824 14.1961 5.94453 14.1093 5.7183 14.1093C5.24969 14.1093 4.88612 14.4728 4.88612 14.9758C4.88612 15.4787 5.24969 15.8423 5.7183 15.8423C5.96069 15.8423 6.1344 15.7555 6.23943 15.5999V15.808H6.60301V14.9758ZM5.25171 14.9758C5.25171 14.6809 5.44158 14.4385 5.75466 14.4385C6.04956 14.4385 6.25761 14.6647 6.25761 14.9758C6.25761 15.2707 6.04956 15.5131 5.75466 15.5131C5.44158 15.4949 5.25171 15.2687 5.25171 14.9758Z" fill="black" />
                            <path d="M13.7215 1.50574H8.25977V11.3183H13.7215V1.50574Z" fill="#FF5A00" />
                            <path d="M8.61776 6.40842C8.61776 4.4148 9.55498 2.64538 10.9931 1.50213C9.93472 0.669942 8.59958 0.166992 7.14326 0.166992C3.69332 0.166992 0.901855 2.95846 0.901855 6.40842C0.901855 9.85837 3.69332 12.6498 7.14326 12.6498C8.59958 12.6498 9.93472 12.1469 10.9931 11.3147C9.55296 10.1876 8.61776 8.40204 8.61776 6.40842Z" fill="#EB001B" />
                            <path d="M21.0879 6.40842C21.0879 9.85837 18.2964 12.6498 14.8465 12.6498C13.3901 12.6498 12.055 12.1469 10.9966 11.3147C12.4529 10.1694 13.372 8.40204 13.372 6.40842C13.372 4.4148 12.4347 2.64538 10.9966 1.50213C12.053 0.669942 13.3881 0.166992 14.8444 0.166992C18.2964 0.166992 21.0879 2.97664 21.0879 6.40842Z" fill="#F79E1B" />
                        </svg>
                    </div>
                </div>
                <div className="worktualwix worktualwixdivide">

                    <div className="worktualwixhalf">
                        <label>Expiry date</label>
                        <div className="worktualexpires">
                            <input type='number' value={expMonth} placeholder={"MM"} onChange={handleExpMonth}></input>
                            <span>/</span>
                            <input type='number' value={expYear} placeholder={"YY"} onChange={handleExpYear}></input>
                        </div>
                    </div>

                    <div className="worktualwixhalf">
                        <label>CVV</label>
                        <div className="worktualicondivide">
                            <input type='password' onChange={handleCvv} placeholder={"CVV"} maxLength={3} ></input>
                            <svg xmlns="http://www.w3.org/2000/svg" width="20" height="16" viewBox="0 0 20 16" fill="none">
                                <path d="M16.6668 1.53064C17.6777 1.53064 18.5001 2.44664 18.5001 3.57254V12.4273C18.5001 13.5532 17.6777 14.4692 16.6668 14.4692H3.3334C2.3225 14.4692 1.5 13.5532 1.5 12.4273V3.57254C1.5 2.44664 2.3225 1.53064 3.3334 1.53064H16.6668ZM16.6668 0.0306396H3.3334C1.4923 0.0306396 0 1.61644 0 3.57254V12.4273C0 14.3834 1.4923 15.9692 3.3334 15.9692H16.6668C18.5076 15.9692 20.0001 14.3834 20.0001 12.4273V3.57254C20.0001 1.61644 18.5077 0.0306396 16.6668 0.0306396Z" fill="#757676" />
                                <path d="M10.2776 6.35724C10.2776 5.89714 10.6509 5.52394 11.1109 5.52394H15.5553C16.0154 5.52394 16.3886 5.89724 16.3886 6.35724C16.3886 6.81724 16.0153 7.19054 15.5553 7.19054H11.1109C10.6508 7.19054 10.2776 6.81724 10.2776 6.35724Z" fill="#757676" />
                                <path d="M11.1109 8.80084H15.5553C16.0154 8.80084 16.3886 9.17414 16.3886 9.63414C16.3886 10.0941 16.0153 10.4674 15.5553 10.4674H11.1109C10.6508 10.4674 10.2776 10.0941 10.2776 9.63414C10.2776 9.17414 10.6509 8.80084 11.1109 8.80084Z" fill="#757676" />
                                <path d="M5.0611 8.29184L5.1319 8.28314C5.1973 8.28314 5.2621 8.30444 5.3199 8.33884C5.4431 8.41264 5.5776 8.46854 5.7211 8.50164C5.8342 8.52764 5.9457 8.54004 6.0545 8.54004C6.3138 8.54004 6.5574 8.46954 6.7696 8.34664C6.826 8.31384 6.8892 8.29384 6.9527 8.29384L7.0251 8.30304C7.7521 8.49274 8.338 8.88664 8.6543 9.39404C8.7191 9.49804 8.7189 9.63774 8.6538 9.74154C8.1964 10.4714 7.1876 10.9793 6.0133 10.9793C5.7974 10.9793 5.576 10.9621 5.3514 10.9261C4.4782 10.7859 3.7639 10.3595 3.3934 9.78704C3.3142 9.66474 3.3131 9.50584 3.3926 9.38364C3.7235 8.87514 4.3246 8.47884 5.0611 8.29184Z" fill="#757676" />
                                <path d="M6.0182 5.01204C6.1167 5.01204 6.2179 5.02324 6.3207 5.04694C6.8152 5.16084 7.2001 5.56214 7.3095 6.07794C7.5002 6.97864 6.8489 7.77454 6.0183 7.77454C5.9196 7.77454 5.8184 7.76324 5.7156 7.73964C5.2211 7.62584 4.8362 7.22444 4.7268 6.70864C4.5361 5.80794 5.1876 5.01204 6.0182 5.01204Z" fill="#757676" />
                            </svg>
                        </div>
                    </div>
                </div>
                <div className="worktualwix">
                    <label>Cardholder Name</label>
                    <input type='text' placeholder={"Name"} onChange={handleCardHolderName}></input>
                </div>

                <div className="worktualwix">
                    <button>Pay now</button>
                </div>
            </form>
        </>
    )
}

export default Wixpayment