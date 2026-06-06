//import * as React from 'react';
import React, { useEffect, useState } from 'react'
import { CreditCard, PaymentForm, GooglePay } from 'react-square-web-payments-sdk';

export default function Square(props) {
	const [status, setStatus] = useState("");
	const [uuid, setUuid] = useState(localStorage.getItem("uuid"))
	const [applicationId, setApplicationId] = useState()
	console.log("applicationId", applicationId)
	const [locationId, setLocationId] = useState()
	console.log("applicationId", locationId)
	const [load, setLoad] = useState(false)
	const [domainId, setDomainId] = useState(sessionStorage.getItem("domainId"))

	useEffect(() => {
		const init = async () => {
			try {
				const response = await fetch('https://mysqlapi.worktual.co.uk/myaccount-mysql/v1/worktual/get_ChatBotPaymentIntegration_v1', {
					method: 'POST',
					headers: {
						'Content-Type': 'application/json',
					},
					body: JSON.stringify({ p_domainId: domainId }),
				});
				if (!response.ok) {
					throw new Error(`HTTP error! Status: ${response.status}`);
				}
				const data = await response.json();
				let json;
				if (data?.users?.length > 0) {
					const getObj = data.users[0].filter((ele) => ele?.paymentType.toLowerCase() === "square");
					if (getObj.length > 0) {
						json = JSON.parse(getObj[0]?.secretJson);
						setApplicationId(json.application_id);
						setLocationId(json.locationId);
					}
				}
			} catch (error) {
				console.error('Error fetching payment integration:', error);
			}
		};

		init()
	}, [])

	return (
		<>
			<div>
				{applicationId && locationId ?
					<PaymentForm
						applicationId={applicationId}
						cardTokenizeResponseReceived={async (token, buyer) => {
							try {
								const response = await fetch('https://liveurapi.worktual.co.uk/vectone-myaccount/v1/user/square_payment', {
									method: 'POST',
									headers: {
										'Content-Type': 'application/json',
									},
									body: JSON.stringify({
										sourceId: token.token,
										sessionId: uuid
									}),
								});
								if (!response.ok) {
									throw new Error(`HTTP error! Status: ${response.status}`);
								}

								const data = await response.json();
								console.log(data, "///");

								if (data.statusCode === 200) {
									window.parent.postMessage(
										{ type: 'PAYMENT_IFRAME', message: { orderId: data.refcode, status: 'success' } },
										'*'
									);
									setStatus("succeeded");
									// onPaymentCompleted();
								} else {
									window.parent.postMessage(
										{ type: 'PAYMENT_IFRAME', message: { orderId: data.refcode, status: 'failed' } },
										'*'
									);
									// setPaymentStatus("failed");
								}
							} catch (error) {
								console.error('Error processing payment:', error);
								window.parent.postMessage(
									{ type: 'PAYMENT_IFRAME', message: { orderId: null, status: 'failed' } },
									'*'
								);
							}
						}}
						locationId={locationId}
					// createVerificationDetails={() => ({
					// 	amount: 10,
					// 	billingContact: {
					// 		addressLines: ['123 Main Street', 'Apartment 1'],
					// 		familyName: 'Doe',
					// 		givenName: 'John',
					// 		countryCode: 'GB',
					// 		city: 'London',
					// 	},
					// 	currencyCode: 'GBP',
					// 	intent: 'CHARGE',
					// })}
					>
						<CreditCard
							buttonProps={{

								css: {
									backgroundColor: "rgb(0, 184, 133)",
									fontSize: "14px",
									color: "#fff",
									"&:hover": {
										backgroundColor: "rgb(0, 184, 133)",
									},
								},
							}}
							style={{
								input: {
									fontSize: '14px',
								},
								'.input-container.is-focus': {
									borderColor: 'rgb(0, 184, 133)',
								},
							}}
						/>
						{/* <GooglePay /> */}
					</PaymentForm>
					: ""}

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
		</>
	);
}

/*
export default function Square(props) {
		<SquarePaymentsForm
			applicationId="sandbox-sq0idb--sMUyptxSk9l3y0dDr8Cxg"
			locationId="LHQWENE9F0VNS"
			cardTokenizeResponseReceived={async(token, buyer) => {
				console.info({ token, buyer });
			}}
  
			createVerificationDetails={() => ({
				amount: '1.00',
				billingContact: {
					addressLines: ['123 Main Street', 'Apartment 1'],
					familyName: 'Doe',
					givenName: 'John',
					countryCode: 'GB',
					city: 'London',
				},
				currencyCode: 'GBP',
				intent: 'CHARGE',
			})}
		>
			<CreditCardInput />
		</SquarePaymentsForm>
	}

*/


