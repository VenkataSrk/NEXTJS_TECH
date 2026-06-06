/* eslint-disable */
import "../../src/chatPreview.scss";
import React, { useEffect, useState } from "react";
import { messageActions } from '../store/slice/message_slice'
import { useSelector } from "react-redux";
import { useDispatch } from "react-redux";
import { config } from "../base/constants";
import moment from 'moment'
import { v4 as uuidv4 } from "uuid";
import { oneToOneChatSend, updateCallbackData, dashboardWebChatCallBack } from '../service/socketservice'
import { contactStorde, postIntegrationApiData, getIntegrationApiData, patchIntegrationApiData } from './utils'
import { chatBuilderActions } from "../store/slice/builder_slice";
import ReactPhoneInput from 'react-phone-input-2' //react-phone-input-2 component
import '../../src/styles/reactphone/reactPhone.scss' // react-phone-input-2 scss*
import countyNumCheck from '../styles/reactphone/countryList'
import Form from 'antd/es/form';
import Input from 'antd/es/input'
import Button from 'antd/es/button'
import { checkConvoSupported } from "./utils2.0";
// const Input = lazy(() => import('antd/es/input'));
// const Button = lazy(() => import('antd/es/button'));


function CallBackRequest({ type }) {
      console.log('type:::::::::::', type)
      const timeformat = 'HH:mm';
      const [form] = Form.useForm();
      const dispatch = useDispatch();
      const startTimeDefault = new Date()
      const [formData, setFormData] = useState([]);
      const [time, setTime] = useState('PM');
      const [startTime, setStartTime] = useState("");
      const [startDate, setStartDate] = useState("");
      const messageList = useSelector((state) => state.message);
      const [requestSuccess, setRequestSuccess] = useState(true);
      const [outputArray, setOutputArray] = useState([]);
      const chatBuilder = useSelector((state) => state.chatBuilder);

      const [codeErr, setCodeErr] = useState(false)
      const [invalidNum, setInvalidNum] = useState(null)

      useEffect(() => {
            if (type === 'Wix') {
                  setOutputArray([
                        { field: 'First name', value: '', message: '' },
                        { field: 'Last name', value: '', message: '' },
                        { field: 'Email address', value: chatBuilder?.customerEmail, message: '' },
                        { field: 'Phone number', value: '', message: '' },
                        { field: 'Address1', value: '', message: '' },
                        { field: 'Address2', value: '', message: '' },
                        { field: 'City', value: '', message: '' },
                        { field: 'Zipcode', value: '', message: '' },
                  ])
            } else if (type === 'BigCommerce') {
                  setOutputArray([
                        { field: 'First name', value: '', message: '' },
                        { field: 'Last name', value: '', message: '' },
                        { field: 'Email address', value: '', message: '' },
                        { field: 'Phone number', value: '', message: '' },
                        { field: 'Address1', value: '', message: '' },
                        { field: 'Address2', value: '', message: '' },
                        { field: 'City', value: '', message: '' },
                        { field: 'Zipcode', value: '', message: '' },
                        { field: 'state', value: '', message: '' },
                  ])
            } else if (type === 'Magento') {
                  setOutputArray([
                        { field: 'First name', value: '', message: '' },
                        { field: 'Last name', value: '', message: '' },
                        { field: 'Email address', value: '', message: '' },
                        { field: 'Phone number', value: '', message: '' },
                        { field: 'Address1', value: '', message: '' },
                        { field: 'Address2', value: '', message: '' },
                        { field: 'City', value: '', message: '' },
                        { field: 'Zipcode', value: '', message: '' },
                        { field: 'Region', value: '', message: '' },
                        { field: 'Gender', value: '', message: '' },
                        { field: 'Date of Birth', value: '', message: '' },
                  ])
            } else if (type === 'Wix_update') {
                  setOutputArray([
                        { field: 'Address1', value: '', message: '' },
                        { field: 'Address2', value: '', message: '' },
                        { field: 'City', value: '', message: '' },
                        { field: 'Zipcode', value: '', message: '' },
                  ])
            } else if (type === 'WooCommerce') {
                  setOutputArray([
                        { field: 'First name', value: '', message: '' },
                        { field: 'Last name', value: '', message: '' },
                        { field: 'User name', value: '', message: '' },
                        { field: 'Email address', value: '', message: '' },
                        { field: 'Phone number', value: '', message: '' },
                        { field: 'Address1', value: '', message: '' },
                        { field: 'Address2', value: '', message: '' },
                        { field: 'City', value: '', message: '' },
                        { field: 'Zipcode', value: '', message: '' },
                        { field: 'Region', value: '', message: '' },
                  ])
            } else if (type === 'WooCommerce_update') {
                  setOutputArray([
                        { field: 'Address1', value: '', message: '' },
                        { field: 'Address2', value: '', message: '' },
                        { field: 'City', value: '', message: '' },
                        { field: 'Zipcode', value: '', message: '' },
                        { field: 'Region', value: '', message: '' },
                  ])
            } else {
                  const dataOutputArray = messageList?.dynamicChatFormList.map(field => ({ "field": field, "message": '' }));
                  setOutputArray(dataOutputArray)
            }
      }, []);

      const handleSubmit = async () => {
            let error = false;
            let temp_data = [...outputArray];
            for (let index = 0; index < temp_data.length; index++) {
                  const key = temp_data[index];

                  if (key?.value === '' || key?.value === undefined) {
                        error = true;
                        temp_data[index] = { ...temp_data[index], message: "Please enter a value" };
                  } else {
                        temp_data[index] = { ...temp_data[index], message: "" };
                  }

                  if (key?.field === 'Email' && key?.value !== '' && key?.value) {
                        const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
                        const email = key?.value;
                        if (!emailRegex.test(email)) {
                              error = true;
                              temp_data[index] = { ...temp_data[index], message: "Please enter a valid email" };
                        } else {
                              temp_data[index] = { ...temp_data[index], message: "" };
                        }
                  }

                  if (key?.field === 'Phone number' && key?.value !== '' && key?.value) {
                        const allowedCharactersRegex = /^[\d()+-]+$/;
                        const phone = key?.value;
                        if (codeErr) {
                              error = true;
                              temp_data[index] = { ...temp_data[index], message: "Invalid country code" };
                        } else if (!allowedCharactersRegex.test(phone) || phone?.length < 8) {
                              error = true;
                              temp_data[index] = { ...temp_data[index], message: "Please enter a valid phone number" };
                        } else {
                              temp_data[index] = { ...temp_data[index], message: "" };
                        }
                  }
            }

            setOutputArray(temp_data);
            if (error) return;

            let data1 = {
                  isBotConnection: true,
                  isAgentConnection: false,
                  isDTMFConnection: false,
            };

            const CoidData = {
                  coid: messageList?.customerFormData?.coid,
                  firstName: formData['First name'] ?? null,
                  email: null,
                  phoneNumber: [],
                  primaryPhoneNumber: formData['Phone number']?.replace(/\D/g, "") ?? null,
                  primaryEmail: formData['Email'] ?? null,
                  domainId: messageList.personalisationAll?.domainId ?? 0,
                  timeZone: Intl.DateTimeFormat().resolvedOptions().timeZone,
            };

            try {
                  const response = await fetch(`${config().CORESERVICE_URL}/insert_update_customer_contact_noauth`, {
                        method: 'POST',
                        headers: {
                              'Content-Type': 'application/json',
                        },
                        body: JSON.stringify(CoidData),
                  });

                  const data = await response.json();

                  if (data?.statusCode === 200) {
                        dispatch(messageActions.customerFormData(data?.insertContactResult));
                        let callbackData = {
                              ChatHistoryId: parseInt(sessionStorage?.ChatHistoryId),
                              domainId: messageList?.personalisationAll?.domainId,
                              coid: messageList?.customerFormData?.coid
                        };
                        updateCallbackData(callbackData);
                  } else {
                        dispatch(messageActions.customerFormData(data?.insertContactResult[0]));
                        dispatch(messageActions.customerBlockStatus(data?.insertContactResult[0]?.blockCustomer));
                  }

                  dispatch(messageActions.setConnectionType(data1));
                  sessionStorage.setItem("isConnected", "bot");
                  dispatch(messageActions.callBackStatus(false));
                  let toEmail = '';
                  if (messageList?.callbackAgentStatus === "Unavailable") {
                        toEmail = messageList?.automatedResponse?.unavilableNotifyAdminEmail;
                  } else if (messageList?.callbackAgentStatus === "Busy") {
                        toEmail = messageList?.automatedResponse?.busyNotifyAdminEmail;
                  }

                  const emailData = {
                        callbackDetails: [formData],
                        to: toEmail,
                  };

                  await fetch(`${config().CORESERVICE_URL}/send_callback_email_user`, {
                        method: 'POST',
                        headers: {
                              'Content-Type': 'application/json',
                        },
                        body: JSON.stringify(emailData),
                  });

                  contactStorde();

                  const body = {
                        startTmpstmp: moment().startOf('day').valueOf(),
                        endTmpstmp: moment().endOf('day').valueOf(),
                        domainId: messageList?.personalisationAll?.domainId,
                  };

                  dashboardWebChatCallBack(body);

            } catch (error) {
                  console.error('Error:', error);
            }
      };


      const handleInputChange = (event) => {
            const { name, value } = event.target;
            setOutputArray((prevValues) => {
                  const updatedValues = prevValues.map((item) => {
                        if (item.field === name) {
                              return { field: name, value, message: item.message };
                        }
                        return item;
                  });
                  if (!updatedValues.some((item) => item.field === name)) {
                        updatedValues.push({ field: name, value, message: '' });
                  }
                  return updatedValues;
            });
            setFormData((prevValues) => ({
                  ...prevValues,
                  [name]: value,
            }));
      };

      const checkNum = (index, e, obj) => {
            try {
                  const data = countyNumCheck.find((e) => e.country === obj?.name)
                  setInvalidNum({ 'len': data?.phoneNumberLengthByCountry_phLength, 'code': obj?.dialCode })
                  setOutputArray((prevValues) => {
                        const updatedValues = prevValues.map((item) => {
                              if (item.field === 'Phone number') {
                                    if (e && e.indexOf(obj.dialCode) !== 0) {
                                          setCodeErr(true)
                                          return { field: 'Phone number', value: e, message: 'Invalid country code' };
                                    } else {
                                          setCodeErr(false)
                                          return { field: 'Phone number', value: e, message: '' };
                                    }
                              }
                              return item;
                        });
                        return updatedValues;
                  });
            } catch (error) {
                  console.log(error)
            }
      }

      const formattedTime = new Intl.DateTimeFormat([], { hour: 'numeric', minute: 'numeric', hour12: false }).format(startTime);
      const renderField = (field, index) => {
            if (field.field === "Phone number") {
                  return (
                        <Form.Item key={field?.field} name={field?.field} className="invaliderror" >
                              <ReactPhoneInput
                                    className={`reactInputPh`}
                                    inputClass="formInputHeight"
                                    searchStyle={{ border: 0 }}
                                    placeholder={'Phone number*'}
                                    enableSearch
                                    value={formData?.['Phone number'] ?? null}
                                    disableSearchIcon
                                    enableLongNumbers={15}
                                    onChange={(val, obj, i) => {
                                          if (i?.type == 'click') {
                                                const num = obj?.dialCode + val?.substring(obj?.dialCode?.length)
                                                handleInputChange({ target: { name: 'Phone number', value: num } })
                                          } else {
                                                handleInputChange({ target: { name: 'Phone number', value: val } })
                                                checkNum(index, val, obj);
                                          }
                                    }}
                              />
                              {<div class="ant-form-item-explain-error" >{field.message}</div>}
                        </Form.Item>
                  )
            }
            return (
                  <>
                        <Form.Item
                              key={field?.field}
                              name={field?.field}
                              className="invaliderror"
                        >
                              <Input placeholder={`${field?.field}*`} className="formInputHeight" name={field?.field} value={field?.value} onChange={handleInputChange} />
                              {<div class="ant-form-item-explain-error" >{field.message}</div>}
                        </Form.Item>
                  </>
            );
      };

      const handleWix = async () => {

            let error = false;
            let temp_data = [...outputArray];

            for (let index = 0; index < temp_data.length; index++) {
                  const key = temp_data[index];
                  console.log('key?.value-=-=-=-=-=', key?.value)
                  if (key?.value === '' || key?.value === undefined) {
                        error = true;
                        temp_data[index] = { ...temp_data[index], message: "Please enter a value" };
                  } else {
                        temp_data[index] = { ...temp_data[index], message: "" }
                  }
                  if (key?.field === 'Email address' && key?.value !== '' && key?.value) {
                        const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
                        const email = key?.value;
                        if (!emailRegex.test(email)) {
                              error = true;
                              temp_data[index] = { ...temp_data[index], message: "Please enter a valid email" };
                        } else {
                              temp_data[index] = { ...temp_data[index], message: "" }
                        }
                  }
                  if (key?.field === 'Phone number' && key?.value !== '' && key?.value) {
                        const allowedCharactersRegex = /^[\d()+-]+$/;
                        const phone = key?.value;
                        if (codeErr) {
                              error = true;
                              temp_data[index] = { ...temp_data[index], message: "Invalid country code" };

                        } else if (!allowedCharactersRegex.test(phone) || phone?.length < 8) {
                              error = true;
                              temp_data[index] = { ...temp_data[index], message: "Please enter a valid phone number" };

                        } else {
                              temp_data[index] = { ...temp_data[index], message: "" }
                        }
                  }
            }
            setOutputArray(temp_data);
            console.log('handleShopify temp_data', temp_data)
            if (error) {
                  return;
            } else {
                  if (chatBuilder?.connectionName === 'Wix' && type === 'Wix') {
                        const payload = {
                              "firstName": temp_data[0].value,
                              "lastName": temp_data[1].value,
                              "email": temp_data[2].value,
                              "phoneCountryCode": 'GB',
                              "phoneNumber": temp_data[3].value,
                              "address": {
                                    "tag": 'HOME',
                                    "addressLine": temp_data[4].value,
                                    "addressLine2": temp_data[5].value,
                                    "city": temp_data[6].value,
                                    "postalCode": temp_data[7].value,
                                    "country": 'GB'
                              }
                        };
                        dispatch(chatBuilderActions.setCustomerPaymentData(payload));
                        const end_point = `${config()?.WIX_TOKEN_GENERATOR}/${messageList.personalisationAll.domainId}`
                        const res_data = await getIntegrationApiData(end_point, '')
                        if (res_data?.result.length > 0) {
                              const tokenId = res_data?.result[0]?.token;
                              const end_point_refresh = `${config()?.WIX_REFRESH_TOKEN_GENERATOR}/${messageList.personalisationAll.domainId}`
                              const res_data_refresh = await getIntegrationApiData(end_point_refresh, tokenId);
                              const refreshToken = res_data_refresh?.results?.token
                              dispatch(chatBuilderActions.setEcommercePurchase({ refreshToken }));
                              const token = res_data_refresh?.results?.token;
                              const url = config().WIX_CREATE_CUSTOMER;
                              const createCustomer = await postIntegrationApiData(url, token, payload);
                              if (createCustomer?.statusCode === 200) {
                                    dispatch(chatBuilderActions.setEnableCustomerForm(false));
                                    let url = `${config().WIX_EXISTING_CUSTOMER}/${temp_data[2].value}`
                                    const response = await getIntegrationApiData(url, res_data_refresh?.results?.token);
                                    if (response?.result?.contacts?.length > 0) {
                                          // dispatch(chatBuilderActions.setAddressList(response?.data?.result?.contacts));
                                          // dispatch(chatBuilderActions.setShowAddress(true));
                                          const addressData = response?.result?.contacts[0]?.info?.addresses?.items[0]?.address
                                          let carousalPayloads = {
                                                messageID: uuidv4(),
                                                from: messageList.uuid,
                                                to: messageList?.connectionType?.isBotConnection ? messageList.uuid : `${messageList.agentDomainExt.domainId}_${messageList.agentDomainExt.ext}`,
                                                sessionId: messageList.uuid,
                                                message: `Confirm the shipping address: ${addressData?.addressLine}  ${addressData?.addressLine2}  ${addressData?.city}  ${addressData?.postalCode}`,
                                                customerAddress: `${addressData?.addressLine}  ${addressData?.addressLine2}  ${addressData?.city}  ${addressData?.postalCode}`,
                                                sendAt: Date.now(),
                                                messageType: 'addressLine',
                                                isBot: "IN",
                                                isCustomer: false,
                                                sender: '',
                                                isLike: 0,
                                                domainId: messageList?.personalisationAll?.domainId,
                                                isLiveagent: false,
                                                direction: 'IN',
                                                ChatHistoryId: parseInt(sessionStorage?.ChatHistoryId),
                                                ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
                                                socketId: messageList?.socketId,
                                                assistType: checkConvoSupported() ? "Convo" : "Template",
                                          };
                                          oneToOneChatSend(carousalPayloads);
                                          let texts = {
                                                message: carousalPayloads,
                                          };
                                          dispatch(messageActions.setMessageChat({ agentText: texts }));
                                    }
                              } else if (createCustomer?.statusCode === 409) {
                                    dispatch(chatBuilderActions.setEnableCustomerForm(false));
                                    let url = `${config().WIX_EXISTING_CUSTOMER}/${temp_data[2].value}`
                                    let token = chatBuilder?.ecommercePurchase?.token;
                                    const response = await getIntegrationApiData(url, token);
                                    if (response?.result?.contacts?.length > 0) {
                                          // dispatch(chatBuilderActions.setAddressList(response?.data?.result?.contacts));
                                          // dispatch(chatBuilderActions.setShowAddress(true));
                                          const addressData = response?.result?.contacts[0]?.info?.addresses?.items[0]?.address
                                          let carousalPayloads = {
                                                messageID: uuidv4(),
                                                from: messageList.uuid,
                                                to: messageList?.connectionType?.isBotConnection ? messageList.uuid : `${messageList.agentDomainExt.domainId}_${messageList.agentDomainExt.ext}`,
                                                sessionId: messageList.uuid,
                                                message: `Confirm the shipping address: ${addressData?.addressLine}  ${addressData?.addressLine2}  ${addressData?.city}  ${addressData?.postalCode}`,
                                                sendAt: Date.now(),
                                                customerAddress: `${addressData?.addressLine}  ${addressData?.addressLine2} ${addressData?.city}  ${addressData?.postalCode}`,
                                                messageType: 'addressLine',
                                                isBot: "IN",
                                                isCustomer: false,
                                                sender: '',
                                                isLike: 0,
                                                domainId: messageList?.personalisationAll?.domainId,
                                                isLiveagent: false,
                                                direction: 'IN',
                                                ChatHistoryId: parseInt(sessionStorage?.ChatHistoryId),
                                                ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
                                                socketId: messageList?.socketId,
                                                assistType: checkConvoSupported() ? "Convo" : "Template",
                                                supervisorList: messageList?.superVisorExt
                                          };
                                          oneToOneChatSend(carousalPayloads);
                                          let texts = {
                                                message: carousalPayloads,
                                          };
                                          dispatch(messageActions.setMessageChat({ agentText: texts }));
                                    }
                              } else {
                                    dispatch(chatBuilderActions.setEnableCustomerForm(false))
                              }
                        }
                  } else if (chatBuilder?.connectionName === 'BigCommerce') {
                        const payload = {
                              "email": temp_data[2].value,
                              "firstName": temp_data[0].value,
                              "lastName": temp_data[1].value,
                              "phoneNumber": temp_data[3].value,
                              "address": {
                                    "address1": temp_data[4].value,
                                    "address2": temp_data[5].value,
                                    "addressType": "RESIDENTIAL",
                                    "city": temp_data[6].value,
                                    "countryCode": "GB",
                                    "firstName": temp_data[0].value,
                                    "lastName": temp_data[1].value,
                                    "phoneNumber": temp_data[3].value,
                                    "postalCode": temp_data[7].value,
                                    "state": temp_data[8].value
                              }
                        }

                        const token = chatBuilder?.ecommercePurchase?.token;
                        const url = config().BIGCOMMERCE_CREATE_CUSTOMER;
                        const response = await postIntegrationApiData(url, token, payload);
                        if (response?.statusCode === 200 && response?.result?.data.length > 0) {
                              const result = response?.result?.data[0]
                              const items = [{ 'address': { addressLine: result?.addresses[0]?.address1, addressLine2: result?.addresses[0]?.address2, city: result?.addresses[0]?.city, postalCode: result?.addresses[0]?.postalCode } }]
                              const addressListData = [{
                                    'info': { 'addresses': { 'items': items } }
                              }]
                              dispatch(chatBuilderActions.setAddressList(addressListData));
                              dispatch(chatBuilderActions.setShowAddress(true));
                              payload.contactId = result?.addresses[0]?.customer_id
                              payload.country_code = result?.addresses[0]?.country_code
                              payload.country = result?.addresses[0]?.country
                              dispatch(chatBuilderActions.setCustomerPaymentData(payload));
                        }
                        dispatch(chatBuilderActions.setEnableCustomerForm(false))
                  } else if (chatBuilder?.connectionName === 'Magento') {
                        const payload = {
                              "email": temp_data[2].value,
                              "firstname": temp_data[0].value,
                              "lastname": temp_data[1].value,
                              "gender": temp_data[9].value.toLowerCase() === 'male' ? 1 : 0,
                              "dob": temp_data[10].value,
                              "addresses": [{
                                    "default_shipping": true,
                                    "default_billing": true,
                                    "firstname": temp_data[0].value,
                                    "lastname": temp_data[1].value,
                                    "company": "",
                                    "street": [
                                          temp_data[4].value
                                    ],
                                    "city": temp_data[6].value,
                                    "region": temp_data[8].value,
                                    "postcode": temp_data[7].value,
                                    "country_id": "GB",
                                    "telephone": temp_data[3].value
                              },
                              {
                                    "default_shipping": true,
                                    "default_billing": true,
                                    "firstname": temp_data[0].value,
                                    "lastname": temp_data[1].value,
                                    "company": "",
                                    "street": [
                                          temp_data[5].value
                                    ],
                                    "city": temp_data[6].value,
                                    "region": temp_data[8].value,
                                    "postcode": temp_data[7].value,
                                    "country_id": "GB",
                                    "telephone": temp_data[3].value
                              }
                              ]
                        }
                        const token = chatBuilder?.ecommercePurchase?.token;
                        const url = config().MAGENTO_CREATE_CUSTOMER;
                        const response = await postIntegrationApiData(url, token, payload);
                        if (response?.statusCode === 200 && response?.result) {
                              const result = response?.result
                              const items = [{ 'address': { addressLine: result?.addresses[0]?.street[0], addressLine2: result?.addresses[1]?.street[0], city: result?.addresses[0]?.city, postalCode: result?.addresses[0]?.postcode } }]
                              const addressListData = [{
                                    'info': { 'addresses': { 'items': items } }
                              }]
                              dispatch(chatBuilderActions.setAddressList(addressListData));
                              dispatch(chatBuilderActions.setShowAddress(true));
                              payload.contactId = result?.addresses[0]?.customer_id
                              payload.country_code = result?.addresses[0]?.country_id
                              dispatch(chatBuilderActions.setCustomerPaymentData(payload));
                        }
                        dispatch(chatBuilderActions.setEnableCustomerForm(false))
                  } else if (type === 'Wix_update') {
                        const oldCustomerDetails = chatBuilder?.customerUpdateAddress[0];
                        const newAddress = {
                              "tag": "HOME",
                              "address": {
                                    "addressLine": temp_data[0].value,
                                    "addressLine2": temp_data[1].value,
                                    "city": temp_data[2].value,
                                    "postalCode": temp_data[3].value,
                                    "country": 'GB'
                              }
                        };
                        const addressList =oldCustomerDetails?.info?.addresses?.items!==undefined? oldCustomerDetails?.info?.addresses?.items?.map((list) => {
                              let setaddressData = {
                                    "tag": "HOME",
                                    "address": {
                                          "addressLine": list?.address?.addressLine,
                                          "addressLine2": list?.address?.addressLine2,
                                          "city": list?.address?.city,
                                          "postalCode": list?.address?.postalCode,
                                          "country": list?.address?.country
                                    }
                              }
                              return setaddressData
                        }):[]
                        console.log('new:::::::', oldCustomerDetails?.info?.addresses?.items)
                        addressList.push(newAddress)
                        const payload = {
                              "firstName": oldCustomerDetails?.info?.name?.first,
                              "lastName": oldCustomerDetails?.info?.name?.last,
                              "email": oldCustomerDetails?.primaryInfo?.email,
                              "phoneCountryCode": oldCustomerDetails?.info?.phones?.items[0]?.countryCode,
                              "phoneNumber": oldCustomerDetails?.primaryInfo?.phone,
                              "address": addressList,
                              "revision": oldCustomerDetails?.revision
                        }
                        const token = chatBuilder?.ecommercePurchase?.token;
                        console.log("ecommercePurchase",chatBuilder?.ecommercePurchase)
                        const url = `${config().WIX_CONTACT_UPDATE}/${oldCustomerDetails?.id}`;
                        const createCustomer = await patchIntegrationApiData(url, token, payload);
                        dispatch(chatBuilderActions.setAddressList([createCustomer?.result?.contact]));
                        dispatch(chatBuilderActions.setCustomerUpdateAddress(createCustomer?.result?.contact))
                        dispatch(chatBuilderActions.setShowUpdateAddresss(false))
                        dispatch(chatBuilderActions.setShowAddress(true));
                  } else if (chatBuilder?.connectionName === 'WooCommerce' && type === 'WooCommerce') {
                        const payload = {
                              "email": temp_data[3].value,
                              "firstName": temp_data[0].value,
                              "lastName": temp_data[1].value,
                              "userName": temp_data[2].value,
                              "billing": {
                                    "firstName": temp_data[0].value,
                                    "lastName": temp_data[1].value,
                                    "company": "",
                                    "address_1": temp_data[5].value,
                                    "address_2": temp_data[6].value,
                                    "city": temp_data[7].value,
                                    "state": temp_data[9].value,
                                    "postcode": temp_data[8].value,
                                    "country": 'GB',
                                    "email": temp_data[3].value,
                                    "phone": temp_data[4].value
                              },
                              "shipping": {
                                    "firstName": temp_data[0].value,
                                    "lastName": temp_data[1].value,
                                    "company": "",
                                    "address_1": temp_data[5].value,
                                    "address_2": temp_data[6].value,
                                    "city": temp_data[7].value,
                                    "state": temp_data[9].value,
                                    "postcode": temp_data[8].value,
                                    "country": 'GB',
                                    "email": temp_data[3].value,
                                    "phone": temp_data[4].value
                              }
                        }
                        const token = chatBuilder?.ecommercePurchase?.token;
                        const url = config().WOOCOMMERECE_CREATE_CUSTOMER;
                        const response = await postIntegrationApiData(url, token, payload);
                        const result = response?.result;
                        if (response?.result && response?.result?.code !== 'registration-error-username-exists') {
                              const items = [{ 'address': { addressLine: result?.billing?.address_1, addressLine2: result?.billing?.address_2, city: result?.billing?.city, postalCode: result?.billing?.postcode } }]
                              const addressListData = [{
                                    'info': { 'addresses': { 'items': items } }
                              }]
                              dispatch(chatBuilderActions.setAddressList(addressListData));
                              dispatch(chatBuilderActions.setShowAddress(true));
                              const customerPayload = {
                                    "firstName": result?.first_name,
                                    "lastName": result?.last_name,
                                    "email": result?.email,
                                    "userName": temp_data[0].value,
                                    "phoneCurrencyCode": 'GB',
                                    "phoneNumber": result?.billing?.phone,
                                    "contactId": result?.id,
                                    "address": {
                                          "addressLine": result?.billing?.address_1,
                                          "addressLine2": result?.billing?.address_2,
                                          "city": result?.billing?.city,
                                          "postalCode": result?.billing?.postcode,
                                          "country": result?.billing?.country,
                                          "state": result?.billing?.state,
                                    }
                              };
                              dispatch(chatBuilderActions.setCustomerPaymentData(customerPayload));
                        } else if (response?.result?.code === 'registration-error-username-exists') {
                              outputArray[2].message = 'User name is already exists'
                              setOutputArray(outputArray);
                              return;
                        }
                        dispatch(chatBuilderActions.setEnableCustomerForm(false))
                  } else if (type === 'WooCommerce_update') {
                        const payload = {
                              "email": chatBuilder?.coustomerPaymentData?.email,
                              "firstName": chatBuilder?.coustomerPaymentData?.firstName,
                              "lastName": chatBuilder?.coustomerPaymentData?.lastName,
                              "userName": chatBuilder?.coustomerPaymentData?.userName,
                              "billing": {
                                    "firstName": chatBuilder?.coustomerPaymentData?.firstName,
                                    "lastName": chatBuilder?.coustomerPaymentData?.lastName,
                                    "company": "",
                                    "address_1": temp_data[0]?.value,
                                    "address_2": temp_data[1]?.value,
                                    "city": temp_data[2]?.value,
                                    "state": temp_data[4]?.value,
                                    "postcode": temp_data[3]?.value,
                                    "country": 'GB',
                                    "email": chatBuilder?.coustomerPaymentData?.email,
                                    "phone": chatBuilder?.coustomerPaymentData?.billing?.phone
                              },
                              "shipping": {
                                    "firstName": chatBuilder?.coustomerPaymentData?.firstName,
                                    "lastName": chatBuilder?.coustomerPaymentData?.lastName,
                                    "company": "",
                                    "address_1": temp_data[0]?.value,
                                    "address_2": temp_data[1]?.value,
                                    "city": temp_data[2]?.value,
                                    "state": temp_data[4]?.value,
                                    "postcode": temp_data[3]?.value,
                                    "country": 'GB',
                                    "email": chatBuilder?.coustomerPaymentData?.email,
                                    "phone": chatBuilder?.coustomerPaymentData?.billing?.phone
                              }
                        }
                        const token = chatBuilder?.ecommercePurchase?.token;
                        const url = `${config().WOOCOMMERECE_UPDATE_CUSTOMER}/${chatBuilder?.coustomerPaymentData?.contactId}`;
                        const response = await patchIntegrationApiData(url, token, payload);
                        const result = response?.result;
                        dispatch(chatBuilderActions.setEnableCustomerForm(false))
                        if (response?.result) {
                              const items = [{ 'address': { addressLine: result?.billing?.address_1, addressLine2: result?.billing?.address_2, city: result?.billing?.city, postalCode: result?.billing?.postcode } }]
                              const addressListData = [{
                                    'info': { 'addresses': { 'items': items } }
                              }]
                              dispatch(chatBuilderActions.setAddressList(addressListData));
                              dispatch(chatBuilderActions.setShowAddress(true));
                              const customerPayload = {
                                    "firstName": result?.first_name,
                                    "lastName": result?.last_name,
                                    "email": result?.email,
                                    "userName": temp_data[0].value,
                                    "phoneCurrencyCode": 'GB',
                                    "phoneNumber": result?.billing?.phone,
                                    "contactId": result?.id,
                                    "address": {
                                          "addressLine": result?.billing?.address_1,
                                          "addressLine2": result?.billing?.address_2,
                                          "city": result?.billing?.city,
                                          "postalCode": result?.billing?.postcode,
                                          "country": result?.billing?.country,
                                          "state": result?.billing?.state,
                                    }
                              };
                              dispatch(chatBuilderActions.setCustomerPaymentData(customerPayload));
                        }
                        dispatch(chatBuilderActions.setShowUpdateAddresss(false))
                  }

            }
      }

      return (
            <>
                  <div className="chatBody chatBodymessageview" >
                        <div className={["Wix", "BigCommerce", "Magento", "Wix_update", "WooCommerce", "WooCommerce_update"].includes(type) ? "chatbotformtitleWix" : "chatbotformtitle chatmsgform callbackreq"}>
                              {chatBuilder?.connectionName === 'Wix' && <h4>Fill the following details to proceed to checkout</h4>}
                              <div className="row">
                                    <div className="chatbotform">
                                          <Form form={form} onFinish={["Wix", "BigCommerce", "Magento", "Wix_update", "WooCommerce", "WooCommerce_update"].includes(type) ? handleWix : handleSubmit}>
                                                {outputArray?.map((field) => renderField(field))}
                                                <Form.Item>
                                                      {outputArray?.length > 0 && <Button className="primarybtn startbtn" type="primary" htmlType="submit">
                                                            {"Submit"}
                                                      </Button>}
                                                </Form.Item>
                                          </Form>
                                    </div>
                              </div>
                        </div>
                  </div>
            </>
      );
}
export default CallBackRequest;

