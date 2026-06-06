import React, { useEffect } from 'react'
import { useState } from 'react';
function Wix3ds() {
  const [paymentStatus, setPaymentStatus] = useState("Processing")
  useEffect(async () => {
    let queryParams = new URLSearchParams(window.location.search);
    console.log("queryParams", queryParams)
    let status = queryParams.get("status");
    console.log("status", status)
    let wixpaymentObj = JSON.parse(localStorage.getItem("WixPayment"))
    if (status === "AUTHORIZED") {
      let transactionId = localStorage.getItem("transactionId");
      const data = {
        accountId: "acc31e9b-96ab-4613-99d9-99696a4a9fb4",
        transactionId: transactionId,
        amount: wixpaymentObj.amount,
        capturableAmount: wixpaymentObj.amount,
        idempotencyKey: "wix",
        externalCaptureId: "wix-product-purchase-001",
        metadata: {},
        domainId: sessionStorage.getItem("domainId"),
      };

      try {
        const response = await fetch('https://payment.worktual.co.uk/payment/v1/wix/captureTransaction', {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json',
          },
          body: JSON.stringify(data),
        });

        if (!response.ok) {
          throw new Error('Network response was not ok: ' + response.statusText);
        }

        const result = await response.json();
        console.log("response.data.result", result);
        console.log("response.data.result.transaction?", result.transaction);
        console.log("response.data.result.transaction?.captures?.status", result.transaction?.captures[0]?.status);

        if (result.transaction?.captures[0]?.status) {
          setPaymentStatus("Success");
          window.parent.postMessage({ type: 'WIX_PAYMENT_IFRAME', message: result.transaction }, '*');
        }
      } catch (error) {
        console.error('Error capturing transaction:', error);
      }
    }
  }, [])
  return (
    <div>Payment {paymentStatus}</div>
  )
}


export default Wix3ds