
import react, { useState, useEffect, useRef } from 'react';
import { useSelector } from "react-redux";
import { newChatSendMsg, continusBlock } from "./utils";
import { chatMessageUpdate } from '../service/socketservice'
import { messageActions } from "../store/slice/message_slice";
import store from "../store/redux-store/root_store";
import { useDispatch } from "react-redux";
import { v4 as uuidv4 } from "uuid";
import { config } from "../base/constants";

const DropdownComponent = (props) => {
      const messageList = useSelector((state) => state.message);
      const dispatch = useDispatch();
      const [dropDownStatus, setDropDownStatus] = useState(false);
      const [selectedValue, setSelectedValue] = useState(null);
      const currentData = props?.dropDownData?.dropDownDataArr?.information?.data && JSON.parse(props?.dropDownData?.dropDownDataArr?.information?.data);
      const dropDownArr = currentData?.getInputData?.options;

      useEffect(() => {
            if (props?.dropDownData?.selectedDropDownValue) {
                  setSelectedValue(props?.dropDownData?.selectedDropDownValue)
            }
      }, []);

      const handleInputChange = async (event, selectedOption) => {
            let test = [...messageList.messageList];
            setSelectedValue(event);
            setDropDownStatus(!dropDownStatus);
            const result = test?.reverse().map((list, indexed) => {
                  if (indexed === props?.index) {
                        return {
                              ...list, agentText: {
                                    ...list.agentText,
                                    message: {
                                          ...list.agentText.message,
                                          selectedDropDownValue: event
                                    }
                              }
                        }
                  } else {
                        return { ...list }
                  }
            });
            const updatedResult = [...result].reverse();
            dispatch(messageActions.setLikeMessage(updatedResult));
            let daata = selectedOption?.option?.childId;
            const replaceData = {
                  chatHistoryId: parseInt(sessionStorage?.ChatHistoryId),
                  chatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
                  messageArr: updatedResult
            };
            try {
                  const response = await fetch(`${config().CALLSERVICE_URL}/replace_webchat_message`, {
                        method: 'POST',
                        headers: {
                              'Content-Type': 'application/json'
                        },
                        body: JSON.stringify(replaceData)
                  });
                  if (!response.ok) {
                        throw new Error(`Error: ${response.status} ${response.statusText}`);
                  }
                  const resData = await response.json();
                  handleContinueFlow(daata, event);
            } catch (error) {
                  console.error("Request failed:", error);
                  handleContinueFlow(daata, event);
            }

      }



      const handleContinueFlow = (daata, event) => {
            const ansCaptureKeyValue = currentData?.getInputData?.contactSave
            newChatSendMsg("Text", event, "IN", 0);
            store.dispatch(messageActions.dropDownList(''));
            if (currentData?.getInputData?.answerSave) {
                  let arr = [...messageList.inputEntity];
                  let index = messageList.inputEntity.findIndex((item) => {
                        return Object.keys(item).includes(ansCaptureKeyValue);
                  });
                  if (index !== -1) {
                        arr[index] = { ...arr[index], [ansCaptureKeyValue]: event };
                  } else {
                        arr = [...arr, { [ansCaptureKeyValue]: event }];
                  }
                  dispatch(messageActions.inputEntity(arr));
            }
            store.dispatch(messageActions.setAnsCaptureStatus(false));
            store.dispatch(messageActions.setAnsCaptureKey(''));
            let caroType = [];
            if (daata?.includes('carousel')) {
                  let dataArr = props?.dropDownData?.dropDownDataArr?.children ?? props?.dropDownData?.children
                  dataArr?.some((item) => {
                        if (item?.type === event) {
                              caroType?.push(item)
                        }
                  });
            }
            const data1 = {
                  session_id: messageList.uuid,
                  domain_id: messageList.personalisationAll?.domainId,
                  channelType: 'Chat',
                  response: "success",
                  btnValue: daata,
                  callFlowId: props?.dropDownData?.flowIdData?.flowId ? parseInt(props?.dropDownData?.flowIdData?.flowId) : messageList.personalisationAll?.chatFlowId,
                  messageID: uuidv4(),
                  call_type: event,
                  carousal_type: caroType?.length > 0 ? caroType : '',
                  ChatHistoryId: parseInt(sessionStorage.ChatHistoryId),
                  ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
                  inputParams: messageList?.inputEntity
            };
            chatMessageUpdate(data1);
      };

      const dropOptions = dropDownArr?.map((option) => ({
            label: option.value,
            value: option.value,
            option,
      }));



      const [openUpwards, setOpenUpwards] = useState(false);
      const dropdownRef = useRef(null);

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

      return (
            <>
                  {/* <Select
                        className="fordropdowns"
                        onChange={(value, selectedOption) => handleInputChange(value, selectedOption)}
                        options={dropOptions}
                        value={props?.dropDownData?.selectedDropDownValue ? props?.dropDownData?.selectedDropDownValue : `${currentData?.getInputData?.place_holder}`}
                        disabled={props?.dropDownData?.selectedDropDownValue ? true : false}
                  >
                  </Select> */}
                  <div className="antdrops" ref={dropdownRef}>
                        <div
                              className="antdropsname"
                              onClick={() => setDropDownStatus(!dropDownStatus)}
                        >
                              <p>{selectedValue ? selectedValue : `${currentData?.getInputData?.place_holder}` ?? "Select"}</p>
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
                        {/* {(dropDownStatus && !selectedValue) && ( */}
                        {dropDownStatus && (
                              <div className="antdropsbox"
                                    style={{
                                          position: "absolute",
                                          bottom: openUpwards ? "50px" : "auto",
                                    }}
                              >
                                    {dropOptions?.map((list) => (
                                          <div
                                                className="antdropsboxinner"
                                                onClick={() => handleInputChange(list?.value, list)}
                                          >
                                                {list?.label}
                                          </div>
                                    ))}
                              </div>
                        )}
                  </div>

            </>
      )
}

export default DropdownComponent;