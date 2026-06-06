
import React, { useState, useEffect } from 'react';
import { useSelector } from "react-redux";
import { v4 as uuidv4 } from "uuid";
import { continusBlock, newChatSendMsg, storeSelectedItems, storeWixSelectedItems, storeBigCommerceItem, storeMagentoItem, storeWooCommerceItem, checkoutProductStock } from "./utils"
import { messageActions } from "../store/slice/message_slice";
import { oneToOneChatSend, chatMessageUpdate } from "../service/socketservice"
import store from "../store/redux-store/root_store";
import Select from 'antd/es/select';
import { checkConvoSupported } from "./utils2.0";
// const Select = lazy(() => import('antd/es/select'));

function CarouselForm(props) {
  const [carouselForm, setcarouselForm] = useState({
    total: 0,
    previous: 0,
    next: 0,
    carouselData: [],
    allCarousel: []
  });
  const builderList = store.getState().chatBuilder
  const [imageData, setImageData] = useState([])
  const [slideIndex, setSlideIndex] = useState(0)
  const [randomStr, setRandomStr] = useState('')
  const [quantityCount, setQuantityCount] = useState(1);
  const [selectColour, setSelectColour] = useState('');
  const [selectSize, setSelectSize] = useState('');
  const messageList = useSelector((state) => state.message);
  let data = props?.carouselData;

  // useEffect(() => {
  //   //props?.carouselData
  //   if (props?.carouselData[0]?.data?.information && Object.keys(props?.carouselData[0]?.data?.information).length > 0) {
  //     let imgData = JSON.parse(props?.carouselData[0]?.data?.information?.carousel_data)
  //     setcarouselForm({ ...carouselForm, carouselData: imgData, next: props?.carouselData?.length - 1, total: props?.carouselData?.length, allCarousel: props?.carouselData[0] })
  //   }
  //   if (props?.carouselData.length > 0) {
  //     const data = []
  //     for (let img of props?.carouselData) {
  //       let imgData = JSON.parse(img?.data?.information?.carousel_data)
  //       data.push(imgData)
  //     }
  //     setImageData(data)
  //   }
  //   generateString(10)
  // }, []);

  useEffect(()=>{
    generateString(10);
  },[])

  useEffect(() => {
    if (props?.carouselData?.length > 0) {
      const firstCarousel = props.carouselData[0]?.data?.information?.carousel_data;

      if (firstCarousel) {
        try {
          const imgData = JSON.parse(firstCarousel);
          setcarouselForm((prev) => ({
            ...prev,
            carouselData: imgData,
            next: props.carouselData.length - 1,
            total: props.carouselData.length,
            allCarousel: props.carouselData[0]
          }));
        } catch (error) {
          console.error("Error parsing JSON:", error);
        }
      }
      const parsedImageData = props.carouselData.map((img) => {
        try {
          return JSON.parse(img?.data?.information?.carousel_data);
        } catch (error) {
          console.error("Error parsing JSON:", error);
          return null;
        }
      }).filter(Boolean);

      setImageData(parsedImageData);
    }

    generateString(10);
  }, [props.carouselData]);


  const characters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
  const generateString = (length) => {
    let result = ' ';
    const charactersLength = characters.length;
    for (let i = 0; i < length; i++) {
      result += characters.charAt(Math.floor(Math.random() * charactersLength));
    }
    setRandomStr(result)
  }
  const previousClick = () => {
    try {
      let nextRes = carouselForm?.previous - 1;
      if (props?.carouselData[nextRes] === undefined)
        return
      let imgData = JSON.parse(props?.carouselData[nextRes]?.data?.information?.carousel_data)
      setcarouselForm({ ...carouselForm, carouselData: imgData, next: carouselForm?.next + 1, previous: nextRes, allCarousel: props?.carouselData[nextRes] })
      if (typeof window === "undefined")
        return
      const sliderWrapper = document.getElementById(randomStr);
      if (sliderWrapper && slideIndex > 0) {
        setSlideIndex((prev) => prev - 1)
        sliderWrapper.scrollLeft -= sliderWrapper.clientWidth - 60;
      }
    } catch (error) {
      console.log(error)
    }
  }
  const nextClick = () => {
    try {
      let nextRes = carouselForm?.previous + 1;
      if (props?.carouselData[nextRes] === undefined)
        return
      let imgData = JSON.parse(props?.carouselData[nextRes]?.data?.information?.carousel_data)
      setcarouselForm({ ...carouselForm, carouselData: imgData, next: carouselForm?.next - 1, previous: nextRes, allCarousel: props?.carouselData[nextRes] })
      if (typeof window === "undefined")
        return
      const sliderWrapper = document.getElementById(randomStr);
      if (sliderWrapper && slideIndex < sliderWrapper.children.length - 1) {
        sliderWrapper.scrollLeft += sliderWrapper.clientWidth - 60;
        setSlideIndex((prev) => prev + 1)
      } else {
        alert(slideIndex)
      }
    } catch (error) {
      console.log(error)
    }
  }

  const setCurrentPrice = (event) => {

    const resultData = imageData?.map((data, index) => {
      if (event === index) {
        data?.buttons[0]?.productDetails?.variants?.map((list) => {
          if (data?.buttons[0]?.selectedColour !== '' && data?.buttons[0]?.selectedColour !== undefined && data?.buttons[0]?.selectedSize !== undefined && data?.buttons[0]?.selectedSize !== '') {
            if ((list?.option1 === data?.buttons[0]?.selectedSize || list?.option2 === data?.buttons[0]?.selectedSize || list?.option3 === data?.buttons[0]?.selectedSize) &&
              (list?.option1 === data?.buttons[0]?.selectedColour || list?.option2 === data?.buttons[0]?.selectedColour || list?.option3 === data?.buttons[0]?.selectedColour)) {
              data.price = list?.price;
            }
          } else if (data?.buttons[0]?.selectedColour !== '' && data?.buttons[0]?.selectedColour !== undefined) {
            if ((list?.option1 === data?.buttons[0]?.selectedColour || list?.option2 === data?.buttons[0]?.selectedColour || list?.option3 === data?.buttons[0]?.selectedColour)) {
              data.price = list?.price;
            }
          } else if (data?.buttons[0]?.selectedSize !== '' && data?.buttons[0]?.selectedSize !== undefined) {
            if ((list?.option1 === data?.buttons[0]?.selectedSize || list?.option2 === data?.buttons[0]?.selectedSize || list?.option3 === data?.buttons[0]?.selectedSize)) {
              data.price = list?.price;
            }
          } else {
            data.price = list?.price;
          }
        })
      }
      return data
    })
    setImageData(resultData);
  }


  const nextChildEvent = (item, value) => {
    
    if (item?.carouselId === 'yes_Carousel_1') {
      if (builderList?.connectionName === 'shopify') {
        let msg = item?.productDetails?.title;
        newChatSendMsg("Text", msg, "IN", 0);
        let checkoutMessage = 'Item has been added to cart.Do you want to continue shopping or proceed to checkout.'
        //newChatSendMsg("Text", checkoutMessage, "OUT");
        let site = JSON.parse(messageList?.personalisationAll?.websiteId);
        let typeRes = messageList.personalisationAll?.plan_id === 54 || messageList.personalisationAll?.plan_id === 32 || messageList.personalisationAll?.plan_id === 97 || messageList.personalisationAll?.plan_id === 96 || messageList.personalisationAll?.plan_id === 103 || messageList.personalisationAll?.plan_id === 107 || messageList.personalisationAll?.plan_id === 120 || messageList.personalisationAll?.plan_id === 121 || messageList.personalisationAll?.plan_id == 122 || messageList.personalisationAll?.plan_id === 123 || messageList.personalisationAll?.plan_id === 124 || messageList.personalisationAll?.plan_id === 116 ? "webcrawl" : "assist";
        let carousalPayloads = {
          messageID: uuidv4(),
          from: messageList.uuid,
          to: messageList?.connectionType?.isBotConnection ? messageList.uuid : `${messageList.agentDomainExt.domainId}_${messageList.agentDomainExt.ext}`,
          sessionId: messageList.uuid,
          message: checkoutMessage,
          sendAt: Date.now(),
          messageType: 'Checkout',
          isBot: "IN",
          isCustomer: false,
          sender: '',
          isLike: 0,
          type: typeRes,
          websiteId: site?.length > 0 ? site[0] : "",
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
        store.dispatch(messageActions.setMessageChat({ agentText: texts }));
        storeSelectedItems(item?.productDetails, item?.selectedColour, item?.selectedSize, item?.quantityCount, value)
      } else if (builderList?.connectionName === 'Magento') {
        let msg = item?.productDetails?.name;
        newChatSendMsg("Text", msg, "IN", 0);
        let checkoutMessage = 'Item has been added to cart.Do you want to continue shopping or proceed to checkout.'
        //newChatSendMsg("Text", checkoutMessage, "OUT");
        let site = JSON.parse(messageList?.personalisationAll?.websiteId);
        let typeRes = messageList.personalisationAll?.plan_id === 54 || messageList.personalisationAll?.plan_id === 32 || messageList.personalisationAll?.plan_id === 97 || messageList.personalisationAll?.plan_id === 96 || messageList.personalisationAll?.plan_id === 103 || messageList.personalisationAll?.plan_id === 107 || messageList.personalisationAll?.plan_id === 120 || messageList.personalisationAll?.plan_id === 121 || messageList.personalisationAll?.plan_id == 122 || messageList.personalisationAll?.plan_id === 123 || messageList.personalisationAll?.plan_id === 124 || messageList.personalisationAll?.plan_id === 116 ? "webcrawl" : "assist";
        let carousalPayloads = {
          messageID: uuidv4(),
          from: messageList.uuid,
          to: messageList?.connectionType?.isBotConnection ? messageList.uuid : `${messageList.agentDomainExt.domainId}_${messageList.agentDomainExt.ext}`,
          sessionId: messageList.uuid,
          message: checkoutMessage,
          sendAt: Date.now(),
          messageType: 'Checkout',
          isBot: "IN",
          isCustomer: false,
          sender: '',
          isLike: 0,
          type: typeRes,
          websiteId: site?.length > 0 ? site[0] : "",
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
        store.dispatch(messageActions.setMessageChat({ agentText: texts }));
        storeMagentoItem(item, value)
      } else if (builderList?.connectionName === 'Wix') {
        let msg = item?.productDetails?.name;
        newChatSendMsg("Text", msg, "IN", 0);
        let checkoutMessage = 'Item has been added to cart.Do you want to continue shopping or proceed to checkout.'
        //newChatSendMsg("Text", checkoutMessage, "OUT");
        let site = JSON.parse(messageList?.personalisationAll?.websiteId);
        let typeRes = messageList.personalisationAll?.plan_id === 54 || messageList.personalisationAll?.plan_id === 32 || messageList.personalisationAll?.plan_id === 97 || messageList.personalisationAll?.plan_id === 96 || messageList.personalisationAll?.plan_id === 103 || messageList.personalisationAll?.plan_id === 107 || messageList.personalisationAll?.plan_id === 120 || messageList.personalisationAll?.plan_id === 121 || messageList.personalisationAll?.plan_id == 122 || messageList.personalisationAll?.plan_id === 123 || messageList.personalisationAll?.plan_id === 124 || messageList.personalisationAll?.plan_id === 116 ? "webcrawl" : "assist";
        let carousalPayloads = {
          messageID: uuidv4(),
          from: messageList.uuid,
          to: messageList?.connectionType?.isBotConnection ? messageList.uuid : `${messageList.agentDomainExt.domainId}_${messageList.agentDomainExt.ext}`,
          sessionId: messageList.uuid,
          message: checkoutMessage,
          sendAt: Date.now(),
          messageType: 'Checkout',
          isBot: "IN",
          isCustomer: false,
          sender: '',
          isLike: 0,
          type: typeRes,
          websiteId: site?.length > 0 ? site[0] : "",
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
        store.dispatch(messageActions.setMessageChat({ agentText: texts }));
        storeWixSelectedItems(item, value)
      } else if (builderList?.connectionName === 'BigCommerce') {
        let msg = value?.title;
        newChatSendMsg("Text", msg, "IN", 0);
        let checkoutMessage = 'Item has been added to cart.Do you want to continue shopping or proceed to checkout.'
        //newChatSendMsg("Text", checkoutMessage, "OUT");
        let site = JSON.parse(messageList?.personalisationAll?.websiteId);
        let typeRes = messageList.personalisationAll?.plan_id === 54 || messageList.personalisationAll?.plan_id === 32 || messageList.personalisationAll?.plan_id === 97 || messageList.personalisationAll?.plan_id === 96 || messageList.personalisationAll?.plan_id === 103 || messageList.personalisationAll?.plan_id === 107 || messageList.personalisationAll?.plan_id === 120 || messageList.personalisationAll?.plan_id === 121 || messageList.personalisationAll?.plan_id == 122 || messageList.personalisationAll?.plan_id === 123 || messageList.personalisationAll?.plan_id === 124 || messageList.personalisationAll?.plan_id === 116 ? "webcrawl" : "assist";
        let carousalPayloads = {
          messageID: uuidv4(),
          from: messageList.uuid,
          to: messageList?.connectionType?.isBotConnection ? messageList.uuid : `${messageList.agentDomainExt.domainId}_${messageList.agentDomainExt.ext}`,
          sessionId: messageList.uuid,
          message: checkoutMessage,
          sendAt: Date.now(),
          messageType: 'Checkout',
          isBot: "IN",
          isCustomer: false,
          sender: '',
          isLike: 0,
          type: typeRes,
          websiteId: site?.length > 0 ? site[0] : "",
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
        store.dispatch(messageActions.setMessageChat({ agentText: texts }));
        storeBigCommerceItem(item, value)
      } else if (builderList?.connectionName === 'WooCommerce') {
        let msg = value?.title;
        newChatSendMsg("Text", msg, "IN", 0);
        let checkoutMessage = 'Item has been added to cart.Do you want to continue shopping or proceed to checkout.'
        let carousalPayloads = {
          messageID: uuidv4(),
          from: messageList.uuid,
          to: messageList?.connectionType?.isBotConnection ? messageList.uuid : `${messageList.agentDomainExt.domainId}_${messageList.agentDomainExt.ext}`,
          sessionId: messageList.uuid,
          message: checkoutMessage,
          sendAt: Date.now(),
          messageType: 'Checkout',
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
        store.dispatch(messageActions.setMessageChat({ agentText: texts }));
        storeWooCommerceItem(item, value)
      }
    } else {
      let caroType = carouselForm?.allCarousel?.data?.children[0]?.childId?.includes('carousel');

      if (caroType) {
        let carouselData = [];
        carouselForm?.allCarousel?.data?.children?.map((list) => {
          if (list?.type === item?.value) {
            let result = {
              "childId": list.childId,
              "type": "carousel"
            }
            carouselData.push(result);
          }
        })
        const body = {
          session_id: messageList.uuid,
          domain_id: messageList.personalisationAll?.domainId,
          channelType: 'Chat',
          response: "success",
          btnValue: carouselForm.allCarousel?.data?.children[0].childId,
          callFlowId: messageList.personalisationAll?.chatFlowId,
          call_type: caroType ? "carousel" : "",
          messageID: uuidv4(),
          inputParams: messageList?.inputEntity,
          stream_finished: messageList.personalisationAll?.plan_id === 56 || messageList.personalisationAll?.plan_id === 55 ? 3 : "",
          carousal_type: caroType ? carouselData : "",
          ChatHistoryId: parseInt(sessionStorage.ChatHistoryId),
          ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
          targetLang: messageList?.targetLang,
          //targetLang : messageList?.personalisation?.dynamiclanguage !== undefined?messageList?.personalisation?.dynamiclanguage :'en',
        };
        chatMessageUpdate(body);
      } else {
        let findAgent1 = carouselForm.allCarousel?.data?.children?.find(o => o?.type === (item?.engValue ? item.engValue : item?.value));

        let data = {
          childId: findAgent1?.childId,
          type: 'error'
        }
        continusBlock(data);
      }
      store.dispatch(messageActions.carouselData([]));
      if (item?.link !== '' && item?.link !== undefined) {
        window.open(item?.link, '_blank');
      }
      let msg = carouselForm?.carouselData?.title;
      newChatSendMsg("Text", msg, "IN", 0);
    }
  }

  const handleWheel = (event) => {
    if (event.deltaX !== 0) {
      event.preventDefault();
      return;
    };
  };

  const saveSelectedOption = async (data, event) => {
    const filterData = await Promise.all(imageData?.map((list, index) => {
      if (index === event) {
        list.buttons[0].selectedSize = data;
      }
      if (builderList?.connectionName === 'Wix') {
        list?.buttons[0].productDetails?.variants?.map((price) => {
          if (price?.choices?.Size === data) {
            list.price = price?.variant?.priceData?.price ?? list.price
          }
        })
      } else if (builderList?.connectionName === 'BigCommerce') {
        list?.buttons[0]?.productDetails?.variantData?.map((varData) => {
          varData?.option_values?.map((datas) => {
            if ((datas?.option_display_name.toLowerCase() === 'size') && datas?.label === data) {
              list.file_url = varData?.image_url;
              list.price = varData?.price;
              list.variantId = varData?.id;
            }
          })
        })
      } else if (builderList?.connectionName === 'WooCommerce') {

        list?.buttons[0]?.productDetails?.variantData?.map((v_data) => {
          v_data?.attributes?.map((loop_data) => {
            if (loop_data?.option === data && (list.buttons[0].selectedColour !== undefined ? (loop_data?.option === list.buttons[0].selectedColour) : true)) {
              list.variantId = v_data?.id;
              list.price = v_data?.price;
              list.file_url = v_data?.image?.src;
            }
          })

        })
      }
      return list
    }));
    if (builderList?.connectionName === 'shopify') {
      setCurrentPrice(event);
    }
    setImageData(filterData);
  }

  const setSelectedColor = async (data, event) => {
    const filterData = await Promise.all(imageData?.map(async (list, index) => {
      if (index === event) {
        list.buttons[0].selectedColour = data;
        if (builderList?.connectionName === 'shopify') {
          list.buttons[0].productDetails?.variants?.map((arr) => {
            if (arr?.option1 === data || arr?.option2 === data || arr?.option3) {
              list.buttons[0].productDetails?.images?.map((img) => {
                if (img?.variant_ids?.includes(arr?.id)) {
                  list.file_url = img?.src ?? list.file_url;
                }
              })
            }

          })
          setCurrentPrice(event);
        } else if (builderList?.connectionName === 'Wix') {
          let colorName = '';
          list.buttons[0].productDetails?.productOptions?.map((img) => {
            if (img?.name === 'Color') {
              img?.choices?.map((variantImg) => {
                if (variantImg?.value === data) {
                  colorName = variantImg?.description
                  list.file_url = variantImg?.media?.items[0]?.image?.url ?? list.file_url
                  list.file_url_id = variantImg?.media?.items[0]?.id
                }
              });
              list?.buttons[0].productDetails?.variants?.map((price) => {
                if (price?.choices?.Color === colorName) {
                  list.price = price?.variant?.priceData?.price
                  list.defaultVariantId = price?.id
                }
              })
            }
          })
        } else if (builderList?.connectionName === 'BigCommerce') {
          list?.buttons[0]?.productDetails?.variantData?.map((varData) => {
            varData?.option_values?.map((datas) => {
              if ((datas?.option_display_name.toLowerCase() === 'color') && datas?.label === data) {
                list.file_url = varData?.image_url;
                list.price = varData?.price;
                list.variantId = varData?.id
              }
            })
          })

        } else if (builderList?.connectionName === 'WooCommerce') {
          list?.buttons[0]?.productDetails?.variantData?.map((v_data) => {
            v_data?.attributes?.map((loop_data) => {
              if (loop_data?.option === data && (list.buttons[0].selectedSize !== undefined ? loop_data?.option === list.buttons[0].selectedSize : true)) {
                list.variantId = v_data?.id;
                list.price = v_data?.price;
                list.file_url = v_data?.image?.src;
              }
            })
          })
        }

      }
      return list
    }));
    setImageData(filterData);

  }


  const maintainQuantity = async (data, index) => {
    const filterData = await Promise.all(imageData?.map(async (list, event) => {
      if (list.buttons[0]?.quantityCount >= 1 && index === event) {
        if (data === '+') {
          list.buttons[0].quantityCount = list.buttons[0].quantityCount + 1;
        } else if (data === '-') {
          list.buttons[0].quantityCount = list.buttons[0].quantityCount === 1 ? list.buttons[0].quantityCount : list.buttons[0].quantityCount - 1;
        }
      }
      return list
    }));
    const allUndefined = filterData.every((value) => value === undefined);

    if (allUndefined) {
      setImageData(imageData)
    } else {
      setImageData(filterData)
    }
  }
  console.log('imageData-=-=-=-=-=', imageData)
  return (

    <div className="worktualcarosuelmain">

      <svg width="63" height="81" viewBox="0 0 63 81" fill="none" xmlns="http://www.w3.org/2000/svg" onClick={previousClick}
        className={`${slideIndex === 0 ? "disable" : ""}`}>
        <g filter="url(#filter0_dd_2415_135155)">
          <rect x="5.5" y="0.377319" width="40" height="40" rx="20" fill="white" />
          <path d="M27.6533 26.0428C27.4619 26.0428 27.27 25.9696 27.1235 25.824L23.2588 21.9705C22.833 21.5457 22.5982 20.9803 22.5977 20.3777C22.5972 19.7751 22.8321 19.2097 23.2588 18.783L27.124 14.9305C27.417 14.6385 27.8921 14.6385 28.1845 14.9325C28.477 15.2255 28.476 15.7001 28.1825 15.993L24.3178 19.8455C24.1757 19.9871 24.0976 20.1766 24.0976 20.3777C24.0976 20.5779 24.1757 20.7664 24.3178 20.908L28.183 24.7615C28.476 25.0545 28.4769 25.5291 28.1845 25.822C28.038 25.9695 27.8456 26.0428 27.6533 26.0428Z" fill="#757676" />
        </g>
        <defs>
          <filter id="filter0_dd_2415_135155" x="-14.5" y="0.377319" width="80" height="80" filterUnits="userSpaceOnUse" color-interpolation-filters="sRGB">
            <feFlood flood-opacity="0" result="BackgroundImageFix" />
            <feColorMatrix in="SourceAlpha" type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 127 0" result="hardAlpha" />
            <feMorphology radius="5" operator="erode" in="SourceAlpha" result="effect1_dropShadow_2415_135155" />
            <feOffset dy="10" />
            <feGaussianBlur stdDeviation="5" />
            <feColorMatrix type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0.04 0" />
            <feBlend mode="normal" in2="BackgroundImageFix" result="effect1_dropShadow_2415_135155" />
            <feColorMatrix in="SourceAlpha" type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 127 0" result="hardAlpha" />
            <feMorphology radius="5" operator="erode" in="SourceAlpha" result="effect2_dropShadow_2415_135155" />
            <feOffset dy="20" />
            <feGaussianBlur stdDeviation="12.5" />
            <feColorMatrix type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0.1 0" />
            <feBlend mode="normal" in2="effect1_dropShadow_2415_135155" result="effect2_dropShadow_2415_135155" />
            <feBlend mode="normal" in="SourceGraphic" in2="effect2_dropShadow_2415_135155" result="shape" />
          </filter>
        </defs>
      </svg>
      <div className="content-wrapper" id={randomStr} onWheel={handleWheel}>
        {imageData.map((row, index) => {
          const isActive = index === slideIndex;
          return (
            <div
              key={index}
              className={`worktualcarosuelblock ${isActive ? "active" : "inactive"}`}
              style={isActive ? {} : { pointerEvents: "none"}}
            >
              <div className="worktualcarosuelimages">
                <img src={row?.file_url} alt="product_image" />
              </div>
              <h3>{row?.title}</h3>
              {row?.description ? <p>{row?.description}</p> : ""}
              {row?.isDynamicPrice && (
                <>
                  <h3 className="worktualpricing">{row?.currencySign ?? '£'} {row?.price}</h3>

                  {row?.variantColour[0] && (
                    <div className="variantsColour">
                      {row?.variantColour[0]?.map((list, event) => (
                        <div
                          key={event}
                          className="variantsChild"
                          style={row?.buttons[0]?.selectedColour === list ? { border: `1px solid ${list}` } : {}}
                          onClick={isActive ? () => setSelectedColor(list, index) : undefined}
                        >
                          <p style={{ background: `${list}` }}></p>
                        </div>
                      ))}
                    </div>
                  )}

                  {row?.variantSize[0] && (
                    <div className="variationsworktual">
                      <Select
                        className="fordropdowns"
                        value={row?.buttons[0]?.selectedSize}
                        onChange={isActive ? (value) => saveSelectedOption(value, index) : undefined}
                        disabled={!isActive}
                      >
                        {row?.variantSize[0]?.map((list, idx) => (
                          <Select.Option value={list} key={idx}>
                            {list}
                          </Select.Option>
                        ))}
                      </Select>
                    </div>
                  )}
                </>
              )}

              <div className="btns">
                {row?.buttons?.map((item, idx) => (
                  <div
                    key={idx}
                    className="btnsh5"
                    onClick={isActive ? () => nextChildEvent(item, row) : undefined}
                    style={!isActive ? { pointerEvents: "none", opacity: 0.5 } : {}}
                  >
                    {item?.value}
                  </div>
                ))}
              </div>
            </div>
          );
        })}
      </div>

      {/* <div className='worktualcarosuelblock'>
              <div className="worktualcarosuelimages">
                <img src={carouselForm?.carouselData?.file_url} />
                </div>
                <h3>{carouselForm?.carouselData?.title}</h3>
                <p>{carouselForm?.carouselData?.description}</p>
                {carouselForm?.carouselData?.isDynamicPrice && <p>Price $ {carouselForm?.carouselData?.price}</p>}
                <div className='btns'>
                {
                    carouselForm?.carouselData?.buttons?.map((item, index) => {
                      return (
                        <>
                          <h5 onClick={() => nextChildEvent(item)}>{item?.value}</h5>
                        </>
                      );
                    })
                  }
                  
                </div>
            </div> */}


      <svg width="63" height="81" viewBox="0 0 63 81" fill="none" xmlns="http://www.w3.org/2000/svg" onClick={nextClick} className={`${imageData?.length === slideIndex + 1 ? "disable" : ""}`}>
        <g filter="url(#filter0_dd_2429_135185)">
          <rect x="8.5" y="0.877319" width="40" height="40" rx="20" fill="white" />
          <path d="M26.3476 26.5427C26.1552 26.5427 25.9628 26.4695 25.8164 26.322C25.5239 26.029 25.5249 25.5544 25.8184 25.2615L29.6827 21.409C29.8248 21.2674 29.9029 21.0779 29.9029 20.8768C29.9029 20.6766 29.8248 20.4881 29.6827 20.3465L25.818 16.493C25.525 16.2 25.5241 15.7254 25.8165 15.4325C26.1085 15.1386 26.5836 15.1386 26.8775 15.4305L30.7418 19.284C31.1676 19.7088 31.4024 20.2742 31.4029 20.8768C31.4034 21.4794 31.1685 22.0448 30.7418 22.4715L26.8771 26.324C26.7311 26.4695 26.539 26.5427 26.3476 26.5427Z" fill="#757676" />
        </g>
        <defs>
          <filter id="filter0_dd_2429_135185" x="-11.5" y="0.877319" width="80" height="80" filterUnits="userSpaceOnUse" color-interpolation-filters="sRGB">
            <feFlood flood-opacity="0" result="BackgroundImageFix" />
            <feColorMatrix in="SourceAlpha" type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 127 0" result="hardAlpha" />
            <feMorphology radius="5" operator="erode" in="SourceAlpha" result="effect1_dropShadow_2429_135185" />
            <feOffset dy="10" />
            <feGaussianBlur stdDeviation="5" />
            <feColorMatrix type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0.04 0" />
            <feBlend mode="normal" in2="BackgroundImageFix" result="effect1_dropShadow_2429_135185" />
            <feColorMatrix in="SourceAlpha" type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 127 0" result="hardAlpha" />
            <feMorphology radius="5" operator="erode" in="SourceAlpha" result="effect2_dropShadow_2429_135185" />
            <feOffset dy="20" />
            <feGaussianBlur stdDeviation="12.5" />
            <feColorMatrix type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0.1 0" />
            <feBlend mode="normal" in2="effect1_dropShadow_2429_135185" result="effect2_dropShadow_2429_135185" />
            <feBlend mode="normal" in="SourceGraphic" in2="effect2_dropShadow_2429_135185" result="shape" />
          </filter>
        </defs>
      </svg>


    </div>
  )
}
export default CarouselForm;