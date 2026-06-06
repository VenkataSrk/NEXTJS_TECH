/* eslint-disable */
import "../../src/chatPreview.scss";
import '../../src/styles/reactphone/reactPhone.scss'
import React, { useEffect, useState, useRef } from "react";
import FooterLogo from "./../images/chatbotpoweredlogo1.png";
import { messageActions } from "../store/slice/message_slice";
import { useSelector } from "react-redux";
import { useDispatch } from "react-redux";
import ChatSocket from "../socket";
import { saveContact } from "../chatbot/utils";
import ReactPhoneInput from 'react-phone-input-2'
import countyNumCheck from '../styles/reactphone/countryList'
import Form from 'antd/es/form'
import Radio from 'antd/es/radio'
import Checkbox from 'antd/es/checkbox';
import Button from 'antd/es/button'
import Select from 'antd/es/select';
import Input from 'antd/es/input';
import { getVisitorInsightsData, isFeatureEnabled } from '../chatbot/utils2.0'
const { Option } = Select;

const ChatBot = () => {
  const [formData, setFormData] = useState({});
  const [selectedValue, setSelectedValue] = useState({});
  const [form] = Form.useForm();
  const dispatch = useDispatch();
  const messageList = useSelector((state) => state.message);
  const [formList, setFormList] = useState(messageList)
  const [codeErr, setCodeErr] = useState(false)
  const [invalidNum, setInvalidNum] = useState(null)
  const [dropDownStatus, setDropDownStatus] = useState(false);
  const [openUpwards, setOpenUpwards] = useState(false);
  const dropdownRef = useRef(null);
  const submittingRef = useRef(false);
  const [focusedField, setFocusedField] = useState(null);
  const countryCode = messageList?.clientLocationDetails?.country?.toLowerCase() || 'in';

  async function handleSubmit() {
    // form.validateFields()

    if(submittingRef?.current) return;
    submittingRef.current = true;

    let temp_data = [...formList.tableData]
    let index = 0, error = false
    for (let data of formList.tableData) {
      if (data.fieldType) {
        if (formData[data.label] === undefined || formData[data.label] === '') {
          temp_data[index] = { ...temp_data[index], message: (data.field === 'Radio Button' || data.field === 'Check Box' || data.field === 'Drop down') ? 'Please select the value' : "Please enter the value" }
          error = true
        } else {
          temp_data[index] = { ...temp_data[index], message: "" }
        }
      }
      if (data.label === 'Email' && formData[data.label] !== '' && formData[data.label]) {
        const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
        const email = formData[data.label];
        if (!emailRegex.test(email)) {
          temp_data[index] = { ...temp_data[index], message: "Please enter the valid email" }
          error = true
        } else {
          temp_data[index] = { ...temp_data[index], message: "" }
        }
      } else if (data.label === 'Phone number' && formData[data.label] !== '' && formData[data.label]) {
        // console.log("formData[data.label]-=-=", formData[data.label])
        const allowedCharactersRegex = /^[\d()+-]+$/;
        const phone = formData[data.label] ?? '';
        let numStr = phone.toString();
        try {
          if (codeErr) {
            error = true;
            temp_data[index] = { ...temp_data[index], message: "Invalid country code" };
          } else if (!allowedCharactersRegex.test(phone) || phone?.length < 8) {
            temp_data[index] = { ...temp_data[index], message: "Please enter the valid phone number" }
            error = true
          } else if (numStr.startsWith("91") && numStr.length !== 12) {
            temp_data[index] = { ...temp_data[index], message: "Please enter the valid phone number" }
            error = true
          } else {
            temp_data[index] = { ...temp_data[index], message: "" }
          }
        } catch (error) {
          temp_data[index] = { ...temp_data[index], message: "" }
        }
      } else if ((data.label === 'First name' || data.label === 'Last name') && formData[data.label] !== '' && formData[data.label]) {
        const name = formData[data.label];
        const testPattern = /[^a-zA-Z\s]/g
        if (testPattern.test(name)) {
          temp_data[index] = { ...temp_data[index], message: "Please enter the valid letters" }
          error = true
        } else {
          temp_data[index] = { ...temp_data[index], message: "" }
        }
      }
      index++
    }
    setFormList({ ...formList, tableData: temp_data })
    if (error){
      submittingRef.current =  false
      return
    }

    try {
      if (invalidNum?.code && (formData['Phone number'] == "" || formData['Phone number']?.replace(invalidNum?.code, '') === '')) {
        const { 'Phone number': _, ...data } = formData
        await saveContact(data);
        dispatch(messageActions.chatVisible(false));
      } else {
        await saveContact(formData);
        // isFeatureEnabled('withPhonenumberCallWebsiteInsights') && getVisitorInsightsData()
        dispatch(messageActions.chatVisible(false));
      }
      submittingRef.current = false;
    } catch (error) {
      console.log('error', error)
      await saveContact(formData);
      dispatch(messageActions.chatVisible(false));
    }

  }

  const handleInputChange = (event) => {
    const { name, value } = event?.target;
    setFormData((prevValues) => ({
      ...prevValues,
      [name]: value,
    }));
  };

  // const handleInputChangeRadio = (event) => {
  //   setSelectedValue(event?.target?.value)
  //   const { name, value } = event?.target;
  //   setFormData((prevValues) => ({
  //     ...prevValues,
  //     [name]: value,
  //   }));
  // };

  const handleInputChangeRadio = (e, label) => {
    const { name, value } = e.target;
    setSelectedValue((prevValues) => ({
      ...prevValues,
      [label]: value,
    }));
    setFormData((prevValues) => ({
      ...prevValues,
      [label]: value,
    }));
  };

  const countryCheck = (index, e, d) => {
    let temp_data = [...formList?.tableData]
    try {
      const data = countyNumCheck?.find((e) => e?.country === d?.name)
      setInvalidNum({ 'len': data?.phoneNumberLengthByCountry_phLength, 'code': d?.dialCode })
      if (e && e?.indexOf(d?.dialCode) !== 0) {
        temp_data[index] = { ...temp_data[index], message: "Invalid country code" }
        setCodeErr(true)
      } else {
        temp_data[index] = { ...temp_data[index], message: "" }
        setCodeErr(false)
      }
      return temp_data
    } catch (error) {
      console.log(error)
      return temp_data
    }
  }


  const checkDropdownPosition = () => {
    if (dropdownRef.current) {
      const rect = dropdownRef.current.getBoundingClientRect();
      const spaceBelow = window.innerHeight - rect.bottom;
      const spaceAbove = rect.top;
      setOpenUpwards(spaceBelow < 250 && spaceAbove > spaceBelow);
    }
  };

  useEffect(() => {
    if (dropDownStatus) {
      checkDropdownPosition();
    }
  }, [dropDownStatus])


  const handleClickOutside = (event) => {
    if (dropdownRef.current && !dropdownRef.current.contains(event.target)) {
      setDropDownStatus(false);
    }
  };

  useEffect(() => {
    document.addEventListener("mousedown", handleClickOutside);
    return () => {
      document.removeEventListener("mousedown", handleClickOutside);
    };
  }, []);

  const renderField = (field, index) => {
    if (field.field === "Text Box" || field.field === 'Single-line text') {
      if (field.label === "Phone number") {
        return (
          <Form.Item key={field?.label} name={field?.label} className="invaliderror phoneinvaliderror">
            <ReactPhoneInput
              className={`reactInputPh`}
              inputClass="formInputHeight"
              searchStyle={{ border: 0 }}
              placeholder=""
              // placeholder={field.fieldType === true ? 'Phone number*' : 'Phone number'}
              enableSearch
              value={formData?.['Phone number'] ?? ''}
              disableSearchIcon
              enableLongNumbers={15}
              country={countryCode || 'in'}
              onChange={(e, d, i) => {
                if (i?.type == 'click') {
                  const num = d?.dialCode + e?.substring(d?.dialCode?.length)
                  handleInputChange({ target: { name: 'Phone number', value: num ?? '' } })
                } else {
                  const temp_data = countryCheck(index, e, d)
                  setFormList({ ...formList, tableData: temp_data })
                  handleInputChange({ target: { name: 'Phone number', value: e ?? '' } })
                }
              }}
              onFocus={() => setFocusedField(field.label)}
              onBlur={() => setFocusedField(null)}
            />
            <label
              className={`floatingLabel ${
                (formData?.[field.label] || focusedField === field?.label) ? "active" : ""
              }`}
            >
              {field.label}
              {field.fieldType === true && <span className="star">*</span>}
            </label>
            {<div class="ant-form-item-explain-error" >{field.message}</div>}
          </Form.Item>
        )
      }
      return (
        <>
          <Form.Item
            key={field.label}
            name={field.label}
            className="invaliderror"
          // rules={[
          //   {
          //     required: (field.fieldType === "Required" || field.fieldType === true),
          //     message: `${field?.errorMessage}` !== '' ? `${field?.errorMessage}` : 'Please enter the value',
          //   },
          //   {
          //     pattern: /^([a-zA-Z0-9._-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,6})$|^[a-zA-Z0-9.]+(?:[\s]*[a-zA-Z0-9.]*)*$/,
          //     message: `${field?.errorMessage}` !== '' ? `${field?.errorMessage}` : 'Please enter the value',
          //   },
          // ]}
          >

            <Input className="formInputHeight" name={field.label} onChange={handleInputChange}
            // placeholder={field.fieldType === true ? `${field.label}*` : `${field.label}`} 
              onFocus={() => setFocusedField(field.label)}
              onBlur={() => setFocusedField(null)}
            />
            <label
              className={`floatingLabel ${
                (formData?.[field.label] || focusedField === field.label) ? "active" : ""
              }`}
            >
              {field.label}
              {field.fieldType === true && <span className="star">*</span>}
            </label>
            <div class="ant-form-item-explain-error">{field.message}</div>
          </Form.Item>
        </>
      );
    }
    if (field.field === "Text Area" || field.field === 'Multi-line text') {
      return (
        <Form.Item
          key={field.label}
          name={field.label}
          className="invaliderror"
        // rules={[
        //   {
        //     required: (field.fieldType === "Required" || field.fieldType === true),
        //     message: `${field?.errorMessage}` !== '' ? `${field?.errorMessage}` : 'Please enter the value',
        //   },
        //   {
        //     pattern: /^([a-zA-Z0-9._-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,6})$|^[a-zA-Z0-9.]+(?:[\s]*[a-zA-Z0-9.]*)*$/,
        //     message: `${field.errorMessage}`,
        //   },
        // ]}
        >
          <Input.TextArea placeholder={field.fieldType === true ? `Enter ${field.label}*` : `Enter ${field.label}`} className="formtextareaHeight" name={field.label} onChange={handleInputChange} />
          <div class="ant-form-item-explain-error">{field.message}</div>
        </Form.Item>
      );
    }
    if (field.field === "Drop down") {
      return (
        <Form.Item
          key={field.label}
          name={field.label}
          className="invaliderror formdropdown"
        // rules={[
        //   {
        //     required: (field.fieldType === "Required" || field.fieldType === true),
        //     message: `${field?.errorMessage}` !== '' ? `${field?.errorMessage}` : 'Please enter the value',
        //   },
        // ]}
        >
          {/* <Select
            className=""
            placeholder={field.fieldType === true ? `Select ${field.label}*` : `Select ${field.label}`}
            name={field.label}
            onChange={(value) => handleInputChange({ target: { name: field.label, value } })}
            suffixIcon={<svg xmlns="http://www.w3.org/2000/svg" width="12" height="6" viewBox="0 0 12 6" fill="none">
            <path d="M6.00155 5.90231C5.39905 5.90231 4.83355 5.66791 4.40825 5.24121L0.554751 1.37691C0.262251 1.08391 0.263251 0.609307 0.556251 0.316407C0.850151 0.024407 1.32525 0.0244068 1.61725 0.318407L5.47025 4.18271C5.75395 4.46591 6.24955 4.46591 6.53225 4.18271L10.3858 0.318407C10.6778 0.0245068 11.1529 0.024507 11.4468 0.316407C11.7398 0.609407 11.7407 1.08401 11.4483 1.37691L7.59475 5.24121C7.16945 5.66801 6.60365 5.90231 6.00155 5.90231Z" fill="#757676" />
          </svg>}
          >
            {field.optionalValues.map((option) => (
              <Option key={option.name} value={option.name}>
                {option.name}
              </Option>
            ))}
          </Select> */}


          <div className="antdrops forforms" ref={dropdownRef}>
            <div
              className="antdropsname"
              onClick={() => setDropDownStatus(!dropDownStatus)}
            >
              <p>{formData[field.label] ? formData[field.label] : `Select ${field.label}`}</p>
              <svg
                xmlns="http://www.w3.org/2000/svg"
                width="12"
                height="6"
                viewBox="0 0 12 6"
                fill="none"
                style={{ transform: openUpwards ? "rotate(0deg)" : "rotate(180deg)" }}
              >
                <path
                  d="M10.9149 5.90318C10.7225 5.90318 10.5301 5.82998 10.3837 5.68248L6.5302 1.81728C6.3886 1.67568 6.2001 1.59758 5.9994 1.59658C5.7987 1.59658 5.6098 1.67568 5.4677 1.81728L1.6152 5.68248C1.3232 5.97638 0.847598 5.97638 0.554698 5.68448C0.261198 5.39148 0.260298 4.91688 0.552698 4.62398L4.4057 0.75878C4.8315 0.33198 5.3974 0.0966797 5.9995 0.0966797C6.6016 0.0976797 7.1675 0.33208 7.5928 0.75878L11.4463 4.62398C11.7388 4.91698 11.7378 5.39158 11.4443 5.68448C11.2983 5.82998 11.1063 5.90318 10.9149 5.90318Z"
                  fill="#757576"
                />
              </svg>
            </div>

            {dropDownStatus && (<div className="antdropsbox"
              style={{
                position: "absolute",
                bottom: openUpwards ? "50px" : "auto",
              }}
            >
              {field.optionalValues?.map((list) => (
                <div
                  className="antdropsboxinner"
                  onClick={() => {handleInputChange({ target: { name: field.label, value: list?.name } }), setDropDownStatus(false)}}
                >
                  {list?.name}
                </div>
              ))}
            </div>
            )}
          </div>
          <div class="ant-form-item-explain-error">{field.message}</div>
        </Form.Item>
      );
    }
    if (field.field === "Radio Button") {
      return (
        <Form.Item
          key={field?.fieldType === true ? `${field?.label}*` : `${field?.label}`}
          name={field?.label}
          className="invaliderror invaliderroraddress"
        >
          <div className="addressselection" key={field?.label}>
            <label className="groupchattitle">{field?.label}:</label>
            <div className="radiobtn">
              <Radio.Group
                className="radioblock"
                onChange={(e) => handleInputChangeRadio(e, field?.label)}
                value={selectedValue[field?.label] !== null ? selectedValue[field?.label] : ''}
              >
                {field.optionalValues?.map((option) => (
                  <Radio value={option?.name} key={option?.name}>
                    {option?.name}
                  </Radio>
                ))}
              </Radio.Group>
            </div>
          </div>
          <div className="ant-form-item-explain-error">{field.message}</div>
        </Form.Item>
      );
    }
    if (field.field === "Check Box") {
      return (
        <Form.Item
          key={field.fieldType === true ? `${field.label}*` : `${field.label}`}
          name={field.label}
          valuePropName="checked"
          className="districtselection invaliderror invaliderroraddress"
        // rules={[
        //   {
        //     required: (field.fieldType === "Required" || field.fieldType === true) && (!formData[field.label] || formData[field.label].length === 0),
        //     message: `${field?.errorMessage}` !== '' ? `${field?.errorMessage}` : 'Please enter the value',
        //   },
        // ]}
        >
          <label className="groupchattitle">{field.label}:</label>
          <Checkbox.Group name={field?.label} onChange={(value) => {
            handleInputChange({ target: { name: field.label, value } })
          }}>
            {field.optionalValues.map((option) => (
              <Checkbox className="radioblock" key={option.name} value={option.name}>
                {option.name}
              </Checkbox>
            ))}
          </Checkbox.Group>
          <div class="ant-form-item-explain-error">{field.message}</div>
        </Form.Item>
      );
    }
    return <></>;
  };

  useEffect(() => {
    const deviceType = () => {
      const userAgent = navigator.userAgent;
      if (/Android/i.test(userAgent)) {
        dispatch(messageActions.deviceType("Android"));
      }
      if (/iPhone|iPad|iPod/i.test(userAgent)) {
        dispatch(messageActions.deviceType("iOS"));
      }
      if (/Windows Phone/i.test(userAgent)) {
        dispatch(messageActions.deviceType("Windows Phone"));
      }
      if (/Windows/i.test(userAgent)) {
        dispatch(messageActions.deviceType("Windows"));
      }
      if (/Macintosh/i.test(userAgent)) {
        dispatch(messageActions.deviceType("Macintosh"));
      }
      if (/Linux/i.test(userAgent)) {
        dispatch(messageActions.deviceType("Linux"));
      }
      dispatch(messageActions.deviceType("Unknown"));
    };
    deviceType();
  }, []);
  return (
    <>
      <ChatSocket />
      <div className="chatBody">
        <div className="scroller chatbotformtitle frontforms">
          <h4>{formList?.formDetails?.primaryTitleCustomize}</h4>
          <p>{formList?.formDetails?.primaryMessageCustomize}</p>
          <div className="chatbotform">
            <Form form={form} onFinish={handleSubmit}>
              {formList.tableData.map((field, index) => renderField(field, index))}
              <Form.Item>
                {formList?.formDetails?.ButtonSizeType === 1 ? <Button className="primarybtn startbtn" type="primary" htmlType="submit">
                  {formList?.formDetails?.primaryButtonLabelCustomize !== "" ? formList?.formDetails?.primaryButtonLabelCustomize : "Start"}
                </Button> : <Button className={`primarybtn startbtn ${formList?.formDetails?.ButtonPositionType === 1 ? 'primarybtnLeft' : formList?.formDetails?.ButtonPositionType === 2 ?
                  'primarybtnCenter chat' : 'primarybtnLeft primarybtnCenter primarybtnRight'}`} type="primary" htmlType="submit">
                  {formList?.formDetails?.primaryButtonLabelCustomize !== "" ? formList?.formDetails?.primaryButtonLabelCustomize : "Start"}
                </Button>}
              </Form.Item>
            </Form>
          </div>
        </div>
        {/* <div className="footercopyright">
          {""}
        </div> */}
        {/* {formList.personalisation.isWidgetUnbranding ? */}
        <div className="footercopyright" >
          <div className="footercopys">
            <p>Powered by</p>
            <img src={FooterLogo} alt="" onClick={() => window.open('https://worktual.co.uk/', "_blank")} />
          </div>
          {messageList.personalisation.isConsentForm && <p>By chatting, you consent to this chat being stored according to our privacy policy</p>}
        </div>

      </div>
    </>
  );
}
export default ChatBot;