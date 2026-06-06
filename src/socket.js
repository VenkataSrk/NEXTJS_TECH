import React, { useEffect, useState } from "react";
import { useDispatch, useSelector } from "react-redux";

import { config } from "../src/base/constants";
import io from "socket.io-client";
import { messageActions } from "./store/slice/message_slice";
import { v4 as uuidv4 } from "uuid";

function ChatSocket() {
  const dispatch = useDispatch();
  const messageList = useSelector((state) => state.message);
  let sessionId = sessionStorage.getItem('uuid');



  useEffect(() => {
   var socket = "";/* io(config().BASE_URL, {
      //var socket = io("http://localhost:5008", {
      path: "/ccascall-service",
      auth: {
        token: "",
        id: sessionId,
      },
    }); */
   // dispatch(messageActions.socket(socket));

 
  }, []);
  return <></>;
}
export default ChatSocket;
