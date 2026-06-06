
import { useDispatch, useSelector } from "react-redux";
import "../../src/feedbackstyle.scss";
import React, { useEffect, useState } from "react";
import Rating from 'react-rating';
import { chatBuilderActions } from "../store/slice/builder_slice";
import { config } from "../base/constants";

import {
      starfilled, starempty, smileyempty, smileyfilled, smileyTwoEmpty, smileyTwoFilled, numberEmpty,
      numberFilled, thumpnewempty, thumpnewfilled, thumbFilled, thumbEmpty, smileyfullfilled, smileyfullempty
} from './utils2.0';
import Radio from 'antd/es/radio'
import Checkbox from 'antd/es/checkbox'
import Input from 'antd/es/input';



const FeedBackForm = () => {
      const messageList = useSelector((state) => state.message);
      const builderList = useSelector((state) => state.chatBuilder);
      const [ratingScore, setRatingScore] = useState(0);
      const [questionData, setQuestionData] = useState([]);
      const [questionAnswer, setQuestionAnswer] = useState([]);
      const [endPopup, setEndPopup] = useState(true);
      const [errorSts, setErrorSts] = useState(false)
      const dispatch = useDispatch();
      const [hoveredRating, setHoveredRating] = useState(null);

      const handleHover = (rate) => {
            setHoveredRating(rate);
      };

      const RATING_OPTIONS = {
            1: ["Very Bad", "Bad", "Satisfied", "Good", "Very Good"],
            2: ["Very Bad", "Satisfied", "Very Good"],
            3: ["Bad", "Good"]
      };

      const getRatingOptions = () => {
            if ([1, 2, 3].includes(builderList?.feedBackFormData?.ratingDesignType)) {
                  return RATING_OPTIONS[1];
            } else if ([4, 5].includes(builderList?.feedBackFormData?.ratingDesignType)) {
                  return RATING_OPTIONS[2];
            } else if ([5, 6].includes(builderList?.feedBackFormData?.ratingDesignType)) {
                  return RATING_OPTIONS[3];
            }
            return [];
      };
      const ratingOptions = getRatingOptions()
      const getIcons = (type) => {
            switch (type) {
                  case 1:
                        return { full: starfilled, empty: starempty };
                  case 2:
                        return { full: numberFilled, empty: numberEmpty };
                  case 3:
                        return { full: smileyfullfilled, empty: smileyfullempty };
                  case 4:
                        return { full: smileyfilled, empty: smileyempty };
                  case 5:
                        return { full: thumpnewfilled, empty: thumpnewempty };
                  case 6:
                        return { full: smileyTwoFilled, empty: smileyTwoEmpty };
                  case 7:
                        return { full: thumbFilled, empty: thumbEmpty };
                  default:
                        return { full: [], empty: [] };
            }
      };

      const handleClick = (index) => {
            setRatingScore(index);
            setErrorSts(false);
      }

      let icons = getIcons(builderList?.feedBackFormData?.ratingDesignType)

      useEffect(() => {

            const questionData = JSON.parse(builderList?.feedBackFormData?.additionalQuestions);
            setQuestionData(questionData);

      }, []);

      const handleTextInput = (data, indexed) => {
            const result = questionData?.map((list, index) => {
                  if (indexed === index) {
                        list.answer = data
                  }
                  return list;
            })
            setQuestionAnswer(result)
      }

      const saveCsatData = async () => {
            if (ratingScore == 0) {
                  setErrorSts(true);
                  return;
            }
            if ((builderList?.feedBackFormData?.additionalQuestionType === 1 || ratingScore <= 3) && ratingScore !== 0 && !(ratingScore > 1 && builderList?.feedBackFormData?.ratingDesignType === 6)) {
                  const reqArr = [];
                  questionData?.map((list) => {
                        if (list?.isRequired && (list?.answer === undefined || list?.answer === '')) {
                              list.errorStatus = true
                              reqArr?.push(list)
                        }
                  })
                  if (reqArr?.length > 0) {
                        setErrorSts(true);
                        return;
                  }
            }
            const body = {
                  customerFeedbackId: builderList?.feedBackFormData?.customerFeedbackId,
                  historyId: parseInt(sessionStorage?.ChatHistoryId),
                  channelTypeId: 2003002,
                  satisfactionRate: ratingScore ?? 0,
                  feedbackQuestionAnswer: JSON.stringify(questionAnswer) ?? null,
                  domainId: messageList.personalisationAll?.domainId,
                  AgentId: builderList?.feedBackFormData?.AgentId,
                  ratingType: builderList?.feedBackFormData?.ratingDesignType,
                  timeZone: Intl.DateTimeFormat().resolvedOptions().timeZone,
            };
            try {
                  const response = await fetch(`${config().CORESERVICE_URL}/insert_update_customer_feedback`, {
                        method: 'POST',
                        headers: {
                              'Content-Type': 'application/json'
                        },
                        body: JSON.stringify(body)
                  });
                  if (!response.ok) {
                        throw new Error(`Error: ${response.status} ${response.statusText}`);
                  }
                  const resData = await response.json();
                  setEndPopup(false);
            } catch (error) {
                  console.error("Request failed:", error);
            }
      }

      const handleEndWithoutCsat = () => {
            dispatch(chatBuilderActions.setClosedReferenceStatus(true))
      }

      return (
            <>
                  {endPopup ? <div className="chatBodyfeedbackform chatbodyformcsatscroll">

                        <div className="scroller chatbotformtitle">
                              <div className='closeButton' >
                                    <svg width="25" height="20" viewBox="0 0 16 15" fill="none" xmlns="http://www.w3.org/2000/svg" onClick={() => dispatch(chatBuilderActions.setFeedbackFormStatus(false))}>
                                          <path d="M8.69817 7.67121L12.1726 4.1965C12.3696 4.00002 12.3696 3.68017 12.1726 3.4837C11.9756 3.28668 11.6557 3.28668 11.4598 3.4837L7.98537 6.95835L4.51095 3.4837C4.31507 3.28668 3.99403 3.28668 3.79815 3.4837C3.60114 3.68017 3.60114 4.00002 3.79815 4.1965L7.27256 7.67121L3.79815 11.1459C3.60114 11.3424 3.60114 11.6622 3.79815 11.8587C3.89666 11.9572 4.02531 12.0065 4.15455 12.0065C4.2838 12.0065 4.41298 11.9572 4.51095 11.8587L7.98537 8.38408L11.4598 11.8587C11.5577 11.9572 11.6869 12.0065 11.8162 12.0065C11.9454 12.0065 12.0741 11.9572 12.1726 11.8587C12.3696 11.6622 12.3696 11.3424 12.1726 11.1459L8.69817 7.67121Z" fill="#505152" />
                                    </svg>
                              </div>
                              <div className="scroller">
                                    <div className="titleColor titlecolormsgcont">
                                          <img src={messageList.iconLogo} />
                                          <p>Our Center pays excellent care and consideration towards providing our clients with the best features and service.</p>
                                          <h3>We would really appreciate your feedback.</h3>
                                    </div>
                                    <div className="feedblocks feedblockpad">
                                          <div className="ratingBlock ratingblockpad">
                                                <div className="ratingBlockh5">{builderList?.feedBackFormData?.caption}</div>
                                                <Rating
                                                      stop={icons.full?.length}
                                                      emptySymbol={icons.empty}
                                                      fullSymbol={icons.full}
                                                      className="ratingshow"
                                                      onChange={handleClick}
                                                      onHover={handleHover}
                                                      initialRating={ratingScore}
                                                />

                                                {errorSts && ratingScore === 0 && <label className="reqError">This Field is required *</label>}
                                                {hoveredRating !== null ? (
                                                      <div className="hover-label">
                                                            <span className="ratinglabeltxt">
                                                                  {ratingOptions[hoveredRating - 1]}
                                                            </span>
                                                      </div>
                                                ) : (
                                                      <div className="hover-label">
                                                            <p>&nbsp;</p> {/* Adding a non-breaking space to maintain height/spacing */}
                                                      </div>
                                                )}

                                          </div>
                                          {(builderList?.feedBackFormData?.additionalQuestionType === 1 || ratingScore <= 3) && ratingScore !== 0 && !(ratingScore > 1 && builderList?.feedBackFormData?.ratingDesignType === 6) && <div className='questionBlock'>
                                                {questionData?.length > 0 && questionData?.map((list, index) => {
                                                      return (<>
                                                            {list?.questionOptiontype === 1 && <div className="questionBlockinner qnblockpad">
                                                                  <p>{`${index + 1}. ${list?.inputText}`}</p>
                                                                  <Input.TextArea placeholder='Enter Your Text' onChange={(e) => handleTextInput(e.target?.value, index)} />
                                                                  {list?.errorStatus && errorSts && <label className="reqError">This Field is required *</label>}
                                                            </div>}
                                                            {list?.questionOptiontype === 2 && <div className="questionBlockinner qnblockpad">
                                                                  <p>{`${index + 1}. ${list?.inputText}`}</p>
                                                                  <Radio.Group
                                                                        className="radioblock"
                                                                        onChange={(e) => handleTextInput(e.target?.value, index)}
                                                                  >
                                                                        {list?.options?.map((option) => (
                                                                              <Radio value={option?.value} key={option?.value}>
                                                                                    {option?.label}
                                                                              </Radio>
                                                                        ))}
                                                                  </Radio.Group>
                                                                  {list?.errorStatus && errorSts && <label className="reqError">This Field is required *</label>}
                                                            </div>}
                                                            {list?.questionOptiontype === 3 && <div className="questionBlockinner qnblockpad">
                                                                  <p>{`${index + 1}. ${list?.inputText}`}</p>
                                                                  <Checkbox.Group
                                                                        onChange={(e) => handleTextInput(e, index)}
                                                                  >
                                                                        {list?.options.map((option) => (
                                                                              <Checkbox className="radioblock" key={option.value} value={option.value}>
                                                                                    {option.label}
                                                                              </Checkbox>
                                                                        ))}
                                                                  </Checkbox.Group>
                                                                  {list?.errorStatus && errorSts && <label className="reqError">This Field is required *</label>}
                                                            </div>}
                                                      </>)
                                                })}
                                          </div>}
                                    </div>
                              </div>
                              <div className="footerbutton footerbtnfeedbckform">
                                    <div className="ftbtncancel" onClick={() => dispatch(chatBuilderActions.setFeedbackFormStatus(false))}>Cancel</div>
                                    <div className="ftbtnsave" onClick={saveCsatData}>Submit</div>
                              </div>

                        </div>
                  </div>
                        :
                        <div className="chatBodyfeedbackform" style={{ display: "flex", alignItems: "flex-end" }}>
                              <div className="scroller chatbotformtitle" style={{ height: "auto", top: "auto", bottom: 0 }}>
                                    <div className='closeButton' >
                                          <svg width="25" height="20" viewBox="0 0 16 15" fill="none" xmlns="http://www.w3.org/2000/svg" onClick={handleEndWithoutCsat}>
                                                <path d="M8.69817 7.67121L12.1726 4.1965C12.3696 4.00002 12.3696 3.68017 12.1726 3.4837C11.9756 3.28668 11.6557 3.28668 11.4598 3.4837L7.98537 6.95835L4.51095 3.4837C4.31507 3.28668 3.99403 3.28668 3.79815 3.4837C3.60114 3.68017 3.60114 4.00002 3.79815 4.1965L7.27256 7.67121L3.79815 11.1459C3.60114 11.3424 3.60114 11.6622 3.79815 11.8587C3.89666 11.9572 4.02531 12.0065 4.15455 12.0065C4.2838 12.0065 4.41298 11.9572 4.51095 11.8587L7.98537 8.38408L11.4598 11.8587C11.5577 11.9572 11.6869 12.0065 11.8162 12.0065C11.9454 12.0065 12.0741 11.9572 12.1726 11.8587C12.3696 11.6622 12.3696 11.3424 12.1726 11.1459L8.69817 7.67121Z" fill="#505152" />
                                          </svg>
                                    </div>
                                    <div className="titleColor titlecolormsgcont">
                                          <img src={messageList.iconLogo} />
                                          <p>
                                                Our Center pays excellent care and consideration towards
                                                providing our clients with the best features and service.
                                          </p>
                                    </div>
                                    <div className="thanksu titlecolormsgcont">{builderList?.feedBackFormData?.thanksNote}</div>
                                    <div className="footerbutton footerbtnfeedbckform">
                                          <div className='ftbtnsave' onClick={handleEndWithoutCsat}>Close</div>
                                    </div>
                              </div>
                        </div>
                  }
            </>
      )
}

export default FeedBackForm;