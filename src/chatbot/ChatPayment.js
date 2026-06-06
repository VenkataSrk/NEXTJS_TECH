import React, { useEffect, useState } from "react";
import { useDispatch, useSelector } from "react-redux";
import sha1 from "js-sha1";
import moment from "moment";
import "../../src/chatPreview.scss";
import { config } from "..//base/constants";

const ChatPayment = () => {
	const messageList = useSelector((state) => state.message);

	const [hash, setHash] = useState("");
	const [timestamp, setTimestamp] = useState("");
	const [currency, setCurrency] = useState("GBP");
	const [amount, setAmount] = useState(1000);
	const [orderId, setOrderID] = useState();
	const [merchantId, setMerchantId] = useState(config().MERCHANT_ID);
	const [secret, setSecret] = useState(config().SECRET_KEY);
	const [state, setState] = useState(true);
	const [status, setStatus] = useState("");
	const [reference, setReference] = useState("");
	const [uuid, setUuid] = useState(sessionStorage.getItem("sessionUniqueId"))
	const [domainId, setDomainId] = useState(sessionStorage.getItem("domainId"))
	// const [orderId,setDomainId] = useState(sessionStorage.getItem("domainId"))
	function formatDateToYYYYMMDDHHMMSS() {
		const now = new Date();
		const year = now.getUTCFullYear().toString();
		const month = (now.getUTCMonth() + 1).toString().padStart(2, '0'); // Months are 0-based
		const day = now.getUTCDate().toString().padStart(2, '0');
		const hours = now.getUTCHours().toString().padStart(2, '0');
		const minutes = now.getUTCMinutes().toString().padStart(2, '0');
		const seconds = now.getUTCSeconds().toString().padStart(2, '0');

		return year + month + day + hours + minutes + seconds;
	}



	useEffect(async () => {
		const init = async () => {
			try {
				const payload = {
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
				};
				const response = await fetch('https://liveurapi.worktual.co.uk/vectone-myaccount/v1/user/create_payment_ref', {
					method: 'POST',
					headers: {
						'Content-Type': 'application/json',
					},
					body: JSON.stringify(payload),
				});
				if (!response.ok) {
					throw new Error(`Error: ${response.status} ${response.statusText}`);
				}
				const responseData = await response.json();
				console.log("payRef[0].ref_id", responseData.topupProcessccdcsmsResp[0].ref_id);
				setOrderID(responseData.topupProcessccdcsmsResp[0].ref_id);
				const _timestamp = moment(new Date()).format("YYYYMMDDHHMMSS");
				const formattedDateTime = formatDateToYYYYMMDDHHMMSS();
				console.log("formattedDateTime", formattedDateTime);
				setTimestamp(formattedDateTime);
				const hash = sha1.create();
				const hash2 = sha1.create();
				console.log("${_timestamp}.${merchantId}.${orderId}.${amount}.${currency}", `${_timestamp}.${merchantId}.${orderId}.${amount}.${currency}`);
				hash.update(`${formattedDateTime}.${merchantId}.${responseData.topupProcessccdcsmsResp[0].ref_id}.${amount}.${currency}`);
				let _hash = hash.hex();
				hash2.update(`${_hash}.${secret}`);
				_hash = hash2.hex();
				setHash(_hash);
			} catch (error) {
				console.error("API request failed", error);
			}

		};
		let res = await init();
		setTimeout(() => {
			(document.getElementById("3ds_form")).click()
		}, 1000);

	}, []);


	// useEffect(() => {
	// 	if (!reference || reference === "") {
	// 		const script = document.createElement('link');
	// 		script.rel = 'script';
	// 		script.href = "https://pay.sandbox.realexpayments.com/pay";
	// 		script.onload = () => {
	// 			console.log('Payment service script has loaded.');
	// 		};

	// 		script.onerror = () => {
	// 			console.error('Error loading payment service script.');
	// 		};
	// 		document.head.appendChild(script);
	// 	}
	// }, [reference]);

	return (
		<>
			<div className="portfolio-realex">
				{!reference || reference === "" ?
					<>

						<form id="elavonForm" action={"https://pay.realexpayments.com/pay"} method="POST" >
							<input type="hidden" name="TIMESTAMP" value={timestamp} />
							<input type="hidden" name="MERCHANT_ID" value={config().MERCHANT_ID} />
							<input type="hidden" name="ACCOUNT" value={`${config().ACCOUNT}`} />
							<input type="hidden" name="ORDER_ID" value={orderId} />
							<input type="hidden" name="AMOUNT" value={amount} />
							<input type="hidden" name="CURRENCY" value={currency} />
							<input type="hidden" name="SHA1HASH" value={hash} />
							<input type="hidden" name="AUTO_SETTLE_FLAG" value="1" />
							<input type="hidden" name="HPP_VERSION" value="2" />

							<input type="hidden" name="MERCHANT_RESPONSE_URL" value={`${config().RETURN_URL}?sessionId=${uuid}&domainId=${domainId}`} />
							<input type="submit" id="3ds_form" value="Click here to Pay" />

							<input type="hidden" name="HPP_BILLING_STREET1" value="Chennai" />
							<input type="hidden" name="HPP_BILLING_CITY" value="Halifax" />
							<input type="hidden" name="HPP_BILLING_POSTALCODE" value="W5 9HR" />
							<input type="hidden" name="HPP_BILLING_COUNTRY" value="826" />
							<input type="hidden" name="HPP_CHALLENGE_REQUEST_INDICATOR" value="NO_PREFERENCE" />
							<input type="hidden" name="HPP_CUSTOMER_EMAIL" value="mani4554@gmail.com" />
						</form>
						<div id="elavonPaymentForm"></div>
					</>
					:
					<div>{status} {reference}</div>
				}
			</div>
		</>
	);
};

export default ChatPayment;