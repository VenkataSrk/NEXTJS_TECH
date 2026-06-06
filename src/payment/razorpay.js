import React, { useState, useEffect } from 'react';
// import logo from './logo.svg';
// import './App.css';

function Razorpay() {
    const [keyId, setKeyId] = useState("");
    const [keySecret, setKeySecret] = useState("");
    const [domainId, setDomainId] = useState(sessionStorage.getItem("domainId"))
    const [uuid, setUuid] = useState(localStorage.getItem("uuid"))
    function loadScript(src) {
        return new Promise((resolve) => {
            const script = document.createElement('script');
            script.src = src;
            script.onload = () => {
                resolve(true);
            };
            script.onerror = () => {
                resolve(false);
            };
            document.body.appendChild(script);
        });
    }

    async function getPaymentDetails() {
        try {
            const response = await fetch('https://mysqlapi.worktual.co.uk/myaccount-mysql/v1/worktual/get_ChatBotPaymentIntegration_v1', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({
                    p_domainId: domainId,
                }),
            });
            if (!response.ok) {
                throw new Error('Failed to fetch payment details: ' + response.statusText);
            }
            const data = await response.json();
            if (data.users.length > 0) {
                const getObj = data.users[0].filter((ele) => ele.paymentType === "Razorpay");
                const json = JSON.parse(getObj[0]?.secretJson);
                setKeyId(json.keyId);
                setKeySecret(json.keySecret);
            }
        } catch (error) {
            console.error(error);
        }
    }

    async function displayRazorpay() {
        const res = await loadScript('https://checkout.razorpay.com/v1/checkout.js');
        if (!res) {
            alert('Razorpay SDK failed to load. Are you online?');
            return;
        }
    
        try {
            const response = await fetch(`https://liveurapi.worktual.co.uk/vectone-myaccount/v1/user/order_create/${domainId}`,{
                method: 'POST',
				headers: {
					'Content-Type': 'application/json',
				},
				body: JSON.stringify({
					currency: "INR",
					amount: sessionStorage.getItem("AMOUNT")*100,
				}),
            });
            
            if (!response.ok) {
                alert('Server error. Are you online?');
                return;
            }
    
            const result = await response.json();
    
            const { amount, id: order_id, currency } = result.data;
    
            const options = {
                key: keyId,
                amount: amount,
                currency: currency,
                name: 'Worktual',
                description: 'Test Transaction',
                order_id: order_id,
                handler: async function (response) {
                    const data = {
                        orderCreationId: order_id,
                        razorpayPaymentId: response.razorpay_payment_id,
                        razorpayOrderId: response.razorpay_order_id,
                        razorpaySignature: response.razorpay_signature,
                        sessionId: uuid
                    };
                    // window.parent.postMessage({ type: 'PAYMENT_IFRAME', message: { orderId: response.razorpay_payment_id, status: 'success' } }, '*');
                       window.parent.postMessage({ type: 'PAYMENT_IFRAME', message: { orderId: response.razorpay_payment_id, status: 'success' } }, '*');
                    
                },
                prefill: {
                    name: '',
                    email: 'example@example.com',
                    contact: '9999999999',
                },
                notes: {
                    address: 'Example Corporate Office',
                },
                theme: {
                    color: '#008c35',
                },
            };
    
            const paymentObject = new window.Razorpay(options);
            paymentObject.on("payment.failed", function (response) {
                window.parent.postMessage({ type: 'PAYMENT_IFRAME', message: { orderId: response.error.metadata.payment_id, status: 'failed' } }, '*');
            });
            paymentObject.open();
        } catch (error) {
            console.error('Error fetching order details:', error);
            alert('An error occurred while processing your request.');
        }
    }
    
   



    useEffect(() => {
        const init = async () => {
            await getPaymentDetails()
            await displayRazorpay()
        }
        init()
    }, [])

    useEffect(() => {
        setTimeout(() => {
            if (keySecret) {
                (document.getElementById("3ds_form")).click()
            }
        }, 1000);
    }, [keySecret])

    return (
        <div className='App'>
            <header className='App-header'>
                {/* <img src={logo} className='App-logo' alt='logo' /> */}
                {/* <button className='App-link' onClick={displayRazorpay}>
                    Pay 
                </button> */}
                <p id="3ds_form" onClick={displayRazorpay}>loading...</p>
            </header>
        </div>
    );
}

export default Razorpay;