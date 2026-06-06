import React, { useEffect, useState} from 'react'
import dropin from "braintree-web-drop-in";
import Button from 'antd/es/button'
// const Button = lazy(() => import('antd/es/button'));

export default function BraintreeDropIn(props) {
	// const { show, onPaymentCompleted, token, amount, setPaymentStatus } = props;
	const [braintreeInstance, setBraintreeInstance] = useState(undefined)
	const [show, setShow] = useState("")
	const [token, setToken] = useState("")
	const [amount, setAmount] = useState(10)
	const [status, setStatus] = useState("");
	const [uuid, setUuid] = useState(sessionStorage.getItem("sessionUniqueId"))
	const [domainId, setDomainId] = useState(sessionStorage.getItem("domainId"))

	const handlePayment = async (nonce) => {
		try {
			const paymentReferenceResponse = await fetch('https://liveurapi.worktual.co.uk/vectone-myaccount/v1/user/create_payment_ref', {
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
			if (!paymentReferenceResponse.ok) {
				throw new Error('Failed to create payment reference: ' + paymentReferenceResponse.statusText);
			}
			const paymentReferenceData = await paymentReferenceResponse.json();
			const paypalCheckoutResponse = await fetch('https://liveurapi.worktual.co.uk/vectone-myaccount/v1/user/paypal_checkout', {
				method: 'POST',
				headers: {
					'Content-Type': 'application/json',
				},
				body: JSON.stringify({
					payment_method_nonce: nonce,
					amount: amount,
					sessionId: uuid,
					ref_id: paymentReferenceData.topupProcessccdcsmsResp[0].ref_id,
					domainId: domainId
				}),
			});

			if (!paypalCheckoutResponse.ok) {
				throw new Error('Failed to process PayPal checkout: ' + paypalCheckoutResponse.statusText);
			}
			const paypalCheckoutData = await paypalCheckoutResponse.json();
			if (paypalCheckoutData.success) {
				window.parent.postMessage({ type: 'PAYMENT_IFRAME', message: { orderId: paymentReferenceData.topupProcessccdcsmsResp[0].ref_id, status: 'success' } }, '*');
				setStatus("succeeded");
			} else {
				window.parent.postMessage({ type: 'PAYMENT_IFRAME', message: { orderId: paymentReferenceData.topupProcessccdcsmsResp[0].ref_id, status: 'failed' } }, '*');
			}
		} catch (error) {
			console.error(error);
		}
	};



	useEffect(() => {

		const _init = async () => {
			try {
				const response = await fetch(`https://liveurapi.worktual.co.uk/vectone-myaccount/v1/user/create_client_token/${domainId}`);
				if (!response.ok) {
					throw new Error('Failed to fetch client token: ' + response.statusText);
				}
				const data = await response.json();
				setToken(data.clientToken);
				setShow(true);
			} catch (error) {
				console.error(error);
			}
		};

		_init()
		if (show) {
			const initializeBraintree = () => dropin.create({
				authorization: token,
				container: '#braintree-drop-in-div',
				paypal: {
					flow: 'vault',
					amount: amount,
					currency: 'GBP',
					buttonStyle: {
						layout: 'vertical',
						color: 'gold',
					},
					commit: true,
				},
			}, function (error, instance) {
				if (error)
					console.error(error)
				else
					setBraintreeInstance(instance);
			});

			if (braintreeInstance) {
				braintreeInstance
					.teardown()
					.then(() => {
						initializeBraintree();
					});
			} else {
				initializeBraintree();
			}
		}
	}, [show])

	return (
		<div className="paymentscroll scroller" style={{ display: `${show ? "block" : "none"}` }}>
			<div id={"braintree-drop-in-div"} />
			<Button
				className={"braintreePayButton"}
				type="primary"
				disabled={!braintreeInstance}
				onClick={() => {
					if (braintreeInstance) {
						braintreeInstance.requestPaymentMethod(
							(error, payload) => {
								if (error) {
									console.error(error);
								} else {
									const paymentMethodNonce = payload.nonce;
									console.log("payment method nonce", payload.nonce);
									handlePayment(payload.nonce)
								}
							});
					}
				}}
			>
				{
					"Pay"
				}
			</Button>

			{status === 'succeeded' && (
				<div>
					<h2>Payment Succeeded!</h2>
				</div>
			)}

			{status === 'failed' && (
				<div>
					<h2>Payment Failed!</h2>
				</div>
			)}

			{status === 'processing' && (
				<div>
					<h2>Payment Processing...</h2>
				</div>
			)}
		</div>
	)
}

