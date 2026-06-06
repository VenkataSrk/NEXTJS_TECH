import React, { useEffect, useState } from 'react'
import { loadStripe } from '@stripe/stripe-js';

import "../../src/chatPreview.scss";
import {
  PaymentElement,
  Elements,
  useStripe,
  useElements,
  CardElement
} from '@stripe/react-stripe-js';
import { useDispatch, useSelector } from "react-redux";
import { messageActions } from "../store/slice/message_slice";


export default function Stripe(props) {
  const [status, setStatus] = useState("");
  const [uuid, setUuid] = useState(sessionStorage.getItem("sessionUniqueId"))
  const [domainId, setDomainId] = useState(sessionStorage.getItem("domainId"))
  const [privateKey, setPrivateKey] = useState("");
  const [isLoading, setIsLoading] = useState(false);
  const dispatch = useDispatch();
  const CheckoutForm = () => {
    const stripe = useStripe();
    const elements = useElements();
    // const addressElement = elements.create('address', options);
    // addressElement.mount('#address-element');
    // const addressElement = elements.create('address', options);
    // addressElement.mount('#address-element');
    const [errorMessage, setErrorMessage] = useState(null);
    const appearance = {
      theme: 'flat',
      variables: {
        fontFamily: 'Verdana',
        fontLineHeight: '1.5',
        borderRadius: '0',
        colorBackground: '#fff',
        focusBoxShadow: 'none',
        focusOutline: '-webkit-focus-ring-color auto 1px',
        tabIconSelectedColor: 'var(--colorText)'
      },
      rules: {
        '.Input, .CheckboxInput, .CodeInput': {
          transition: 'none',
          boxShadow: 'inset -1px -1px #ffffff, inset 1px 1px #0a0a0a, inset -2px -2px #dfdfdf, inset 2px 2px #808080'
        },
        '.Input': {
          padding: '12px'
        },
        '.Input--invalid': {
          color: '#DF1B41'
        },
        '.Tab, .Block, .PickerItem--selected': {
          backgroundColor: '#dfdfdf',
          boxShadow: 'inset -1px -1px #0a0a0a, inset 1px 1px #ffffff, inset -2px -2px #808080, inset 2px 2px #dfdfdf'
        },
        '.Tab': {
          transition: 'none'
        },
        '.Tab:hover': {
          backgroundColor: '#eee'
        },
        '.Tab--selected, .Tab--selected:focus, .Tab--selected:hover': {
          color: 'var(--colorText)',
          backgroundColor: '#ccc'
        },
        '.Tab:focus, .Tab--selected:focus': {
          boxShadow: 'inset -1px -1px #0a0a0a, inset 1px 1px #ffffff, inset -2px -2px #808080, inset 2px 2px #dfdfdf',
          outline: 'none'
        },
        '.Tab:focus-visible': {
          outline: 'var(--focusOutline)'
        },
        '.PickerItem': {
          backgroundColor: '#dfdfdf',
          boxShadow: 'inset -1px -1px #0a0a0a, inset 1px 1px #ffffff, inset -2px -2px #808080, inset 2px 2px #dfdfdf',
          transition: 'none'
        },
        '.PickerItem:hover': {
          backgroundColor: '#eee'
        },
        '.PickerItem--highlight': {
          outline: '1px solid blue'
        },
        '.PickerItem--selected:hover': {
          backgroundColor: '#dfdfdf'
        }
      }
    };

    // Pass the appearance object to the Elements instance


    // Pass the appearance object to the Elements instance
    // const elements1 = stripe.elements({clientSecret: process.env.NEXT_PUBLIC_STRIPE_SECRET_KEY, appearance});

    const handleSubmit = async (event) => {
      event.preventDefault();

      if (!stripe || !elements) {
        // Stripe.js hasn't yet loaded.
        // Make sure to disable form submission until Stripe.js has loaded.
        return;
      }

      const { error: submitError } = await elements.submit();
      if (submitError) {
        setErrorMessage(submitError.message);
        return;
      }

      const paymentStatus = async (status, paymentIntent) => {
        try {
          const response1 = await fetch('https://liveurapi.worktual.co.uk/vectone-myaccount/v1/user/create_payment_ref', {
            method: 'POST',
            headers: {
              'Content-Type': 'application/json',
            },
            body: JSON.stringify({
              sitecode: 'MCM',
              productcode: "EBUN",
              applicationcode: "VMUK",
              paymentagent: "VMUK",
              servicetype: "RES",
              paymentmode: "DEF",
              paymentstep: 1,
              paymentstatus: 0,
              accountid: 447574457482,
              last6digitccno: "0077",
              totalamount: 10,
              currency: "GBP",
              subscriptionid: '',
              merchantid: 'uk01login',
              providercode: 'CS',
              csreasoncode: '100',
              generalerrorcode: 0,
              returnerror: 0,
              email: "",
              IPpaymentagent: '182.66.104.114',
              IPClient: '0.0.0.0',
              ReplyMessage: 'Payment Reference Created',
              device_type: "Chatbot",
              browser: "WEB",
              os_version: "",
              topup_url: "",
              bundle_id: 4,
              expirydate: "04",
              cardtype: "VISA"
            }),
          });

          if (!response1.ok) {
            throw new Error(`HTTP error! Status: ${response1.status}`);
          }

          const data = await response1.json();
          console.log("payRef[0].ref_id", data.topupProcessccdcsmsResp[0].ref_id);

          if (status === '00') {
            window.parent.postMessage({
              type: 'PAYMENT_IFRAME',
              message: {
                orderId: data.topupProcessccdcsmsResp[0].ref_id,
                status: 'success'
              }
            }, '*');
          } else {
            window.parent.postMessage({
              type: 'PAYMENT_IFRAME',
              message: { status: 'failure' }
            }, '*');
          }
        } catch (error) {
          console.error('Error in paymentStatus:', error);
          window.parent.postMessage({
            type: 'PAYMENT_IFRAME',
            message: { status: 'error' }
          }, '*');
        }
      };

      const amount = sessionStorage.getItem("AMOUNT")

      const res = await fetch('https://liveurapi.worktual.co.uk/vectone-myaccount/v1/user/payment', {
        method: 'POST',
        //body: JSON.stringify({ amount: "10", currency: 'usd' })

        body: JSON.stringify({
          domain_id: domainId,
          amount: amount,
          // amount: 1000,
          currency: "eur",
          description: "Booking",

        })
      });
      const { clientSecret } = await res.json();

      const { paymentIntent, error } = await stripe.confirmPayment({
        automatic_payment_methods: { enabled: true },
        elements,
        clientSecret,
        confirmParams: {
          return_url: `${window.location.origin}/chatpayment`,
        },
        redirect: "if_required",
      });
      console.log(">>>>>>>>>>>>>>>>>>>>", paymentIntent)
      if (error) {
        setErrorMessage(error.message);
      } else if (paymentIntent.status === 'succeeded') {
        dispatch(messageActions.paymentForm(false))
        paymentStatus("00", paymentIntent)

        setStatus('succeeded');
      } else {
        paymentStatus("01", paymentIntent)
        setStatus('processing');
      }
      // props.setPaymentStatus(paymentIntent.status);

    };

    const handleCancel = () => {
      console.log("test")
      dispatch(messageActions.paymentEnable(false))
    }

    useEffect(() => {
      const init = async () => {
        try {
          const response = await fetch('https://mysqlapi.worktual.co.uk/myaccount-mysql/v1/worktual/get_ChatBotPaymentIntegration_v1', {
            method: 'POST',
            headers: {
              'Content-Type': 'application/json',
            },
            body: JSON.stringify({
              p_domainId: 11226,
            }),
          });

          if (!response.ok) {
            throw new Error(`HTTP error! Status: ${response.status}`);
          }
          const data = await response.json();
          console.log(">>>>>>>>>>>>>", data);

          if (data?.users?.length > 0) {
            const getObj = data.users[0].filter((ele) => ele.paymentType.toLowerCase() === "stripe");
            if (getObj.length > 0) {
              const json = JSON.parse(getObj[0]?.secretJson);
              setPrivateKey(json.private_key);
            }
          }
        } catch (error) {
          console.error('Error in init:', error);
        }
      };

      init()
    }, [])

    return (
      <>
        <form onSubmit={handleSubmit}>
          <PaymentElement />
          <button className='cardpaybtn' style={{ background: '#00B885' }} type="submit" disabled={!stripe || !elements}>
            <span id="button-text">
              {isLoading ? <div className="spinner" id="spinner"></div> : "Pay "}
            </span>
          </button>

          {errorMessage && <div>Error:{errorMessage}</div>}
        </form>
        {/* <button onClick={()=>handleCancel()}>cancel</button> */}
      </>
    );
  };

  const stripePromise = loadStripe(privateKey);
  // const stripePromise = loadStripe("pk_test_51OQTDYEjDdjQ4iyaYk6ZCeugH0gkCYICVWzYSD0M58ODV8CzDHmQbcsWWImvr5tF9IdRBPhdp8Q8cxxkHp18nUcl00rQoGnIfu");
  const options = {
    mode: 'payment',
    amount: 1000,
    currency: 'eur',
    appearance: {
      theme: 'stripe',
      variables: {
        fontWeightNormal: '500',
        borderRadius: '2px',
        colorPrimary: '#f360a6',
        tabIconSelectedColor: '#fff',
        gridRowSpacing: '16px'
      },
      rules: {
        '.Tab, .Input, .Block, .CheckboxInput, .CodeInput': {
          boxShadow: 'none',
          border: '1px solid #D2D3D4',
          borderRadius: '6px',
        },
        '.Input--invalid': {
          boxShadow: 'none',
        },
        '.Block': {
          borderColor: 'transparent'
        },
        '.BlockDivider': {
          backgroundColor: '#ebebeb'
        },
        '.Tab, .Tab:hover, .Tab:focus': {
          border: '0'
        },
        '.Tab--selected, .Tab--selected:hover': {
          backgroundColor: '#f360a6',
          color: '#fff'
        },
        '.Label': {
          background: '#828385',
          marginBottom: '8px',
        },
        '.Error': {
          fontSize: '12px',
        }
      }
    }
  };

  return (
    <>
      <div>
        <Elements stripe={stripePromise} options={options}>
          <CheckoutForm />

        </Elements>

        {/* {status === 'succeeded' && (
          <div>
            <h2>Payment Succeeded!</h2>
          </div>
        )} */}
        {/* {status === 'failed' && (
          <div>
            <h2>Payment Failed!</h2>
          </div>
        )}

        {status === 'processing' && (
          <div>
            <h2>Payment Processing...</h2>
          </div>
        )} */}
      </div>
    </>
  );

}

