import { useState, useRef, useCallback, useEffect } from "react";
import "./DraggableWindow.scss";
import Inboxdrawer from "../../../inbox/inboxdrawer";
import Commoneditorinbox from "../../../inbox/commonEditorinbox";
import { commonDataSliceActions } from "../../../../store/slice/commonSilce/commonSlice";
import { useDispatch } from "react-redux";
import InboxHistory from "../../../inbox/inboxhistory";
import { getAIChathistroyData } from "../../../../base/service/core_serviceApi";
import { GetItemFromStorage } from "../../../../base/custom_hooks/useStorage";
import { LocalStorage } from "../../../../base/custom_hooks/localStorageKeys";
import { inboxDataSliceActions } from "../../../../store/slice/inbox/inboxSlice";
import { v4 as uuidv4 } from "uuid";


const CloseIcon = () => (
  <svg xmlns="http://www.w3.org/2000/svg" width="15" height="15" viewBox="0 0 15 15" fill="none">
    <path d="M8.5064 7.3057L14.3614 1.4502C14.6934 1.1191 14.6934 0.5801 14.3614 0.249C14.0294 -0.083 13.4903 -0.083 13.1602 0.249L7.3052 6.1044L1.4502 0.249C1.1201 -0.083 0.5791 -0.083 0.249 0.249C-0.083 0.5801 -0.083 1.1191 0.249 1.4502L6.104 7.3057L0.249 13.1612C-0.083 13.4923 -0.083 14.0313 0.249 14.3624C0.415 14.5284 0.6318 14.6114 0.8496 14.6114C1.0674 14.6114 1.2851 14.5284 1.4502 14.3624L7.3052 8.507L13.1602 14.3624C13.3252 14.5284 13.543 14.6114 13.7608 14.6114C13.9786 14.6114 14.1954 14.5284 14.3614 14.3624C14.6934 14.0313 14.6934 13.4923 14.3614 13.1612L8.5064 7.3057Z" fill="#23232B" />
  </svg>
);


const History = () => (
  <svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 20 20" fill="none">
    <path d="M19.9738 9.46481C19.6769 3.95891 14.9631 -0.291089 9.44942 0.0156106C7.60662 0.115011 5.86082 0.732611 4.35092 1.76781L4.32642 1.06831C4.30782 0.528311 3.86452 0.103511 3.32842 0.103511C3.31672 0.103511 3.30502 0.103511 3.29232 0.104511C2.74052 0.124011 2.30892 0.586911 2.32842 1.13871L2.43092 4.08501C2.44952 4.62501 2.89282 5.04981 3.42892 5.04981C3.44062 5.04981 3.45232 5.04981 3.46502 5.04881L6.41232 4.94631C6.96412 4.92681 7.39572 4.46391 7.37622 3.91211C7.35762 3.36031 6.91722 2.91411 6.34202 2.94821L5.24642 2.98601C6.51782 2.12491 7.98362 1.61041 9.53142 1.52731C14.2052 1.27631 18.2111 4.87201 18.4621 9.54681C18.7141 14.2197 15.1164 18.2265 10.4435 18.4775C8.18672 18.6103 6.00402 17.833 4.31752 16.3183C2.63002 14.8037 1.63492 12.7226 1.51282 10.4579C1.48942 10.0409 1.14662 9.73721 0.715917 9.74311C0.298917 9.76561 -0.0213835 10.122 0.00111646 10.54C0.144716 13.207 1.31852 15.6591 3.30582 17.4443C5.15642 19.1054 7.50702 20.0039 9.97472 20.0039C10.1583 20.0039 10.3409 19.999 10.5255 19.9893C16.0314 19.6924 20.2697 14.9716 19.9738 9.46481Z" fill="#23232B" />
    <path d="M9.61914 5.11429C9.20504 5.11429 8.86914 5.45019 8.86914 5.86429V10.9229C8.86914 11.1602 8.98144 11.3829 9.17184 11.5245L12.8036 14.2257C12.9374 14.3263 13.0946 14.3741 13.2499 14.3741C13.4794 14.3741 13.706 14.2696 13.8524 14.0714C14.0995 13.7394 14.0311 13.2696 13.6981 13.0226L10.369 10.546V5.86439C10.369 5.45029 10.0332 5.11429 9.61914 5.11429Z" fill="black" />
  </svg>
)
const Compose = () => (
  <svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 20 20" fill="none">
    <path d="M18.4 9.2C18 9.2 17.6 9.5 17.6 9.9V11.9C17.6 13.6 17.5 15.5 16.4 16.6C15.3 17.7 13.5 17.9 11.9 17.9C10.4 17.9 8.9 17.9 7 17.9C5.3 17.9 3.6 17.7 2.5 16.6C1.4 15.5 1.4 13.6 1.3 11.9V11.8C1.3 10.5 1.3 9.2 1.3 7.8V7.6C1.3 5.9 1.4 4 2.5 2.9C3.7 1.8 5.5 1.6 7.1 1.6C8 1.6 8.8 1.6 9.7 1.6C10.1 1.6 10.5 1.3 10.5 0.8C10.5 0.4 10.2 0 9.7 0C8.8 0 7.9 0 7 0C5.1 0.1 3 0.2 1.5 1.8C0.1 3.4 0 5.7 0 7.5V7.7C0 9.1 0 10.4 0 11.7V11.8C0 13.7 0.1 16 1.6 17.6C3.1 19.1 5.1 19.3 7.1 19.4C8 19.4 8.9 19.4 9.7 19.4C10.5 19.4 11.3 19.4 12 19.4C13.9 19.3 16 19.2 17.5 17.6C19 16.1 19.1 13.8 19.1 11.9V9.9C19.1 9.6 18.8 9.2 18.4 9.2Z" fill="#23232B" />
    <path d="M6.1 10V12.1C6.1 12.6 6.5 13.1 7.1 13.1H9.2C9.5 13.1 9.7 13 9.9 12.8L18.4 4.6C19.4 3.6 19.4 2.1 18.4 1.1C17.9 0.6 17.3 0.3 16.6 0.3C16 0.3 15.4 0.5 14.9 1L6.4 9.2C6.2 9.5 6.1 9.8 6.1 10ZM7.5 10.2L15.8 2.1C16 1.9 16.3 1.8 16.6 1.8C16.9 1.8 17.2 1.9 17.4 2.1C17.6 2.3 17.7 2.6 17.7 2.9C17.7 3.2 17.6 3.5 17.4 3.7L9 11.8H7.4L7.5 10.2Z" fill="#23232B" />
  </svg>
)

const Chevronleft = () => {
  <svg xmlns="http://www.w3.org/2000/svg" width="8" height="16" viewBox="0 0 8 16" fill="none">
    <path d="M7.04593 15.9644C6.81453 15.9644 6.58302 15.8755 6.40723 15.6988L0.889625 10.147C-0.295875 8.95361 -0.296875 7.01131 0.888625 5.81791L6.40323 0.266115C6.75283 -0.0883855 7.32413 -0.0873855 7.67573 0.261215C8.02923 0.611814 8.03123 1.18111 7.68063 1.53371L2.16603 7.08551C1.67483 7.57961 1.67582 8.38342 2.16702 8.87751L7.68463 14.4293C8.03423 14.7818 8.03323 15.3522 7.68073 15.7027C7.50493 15.8765 7.27543 15.9644 7.04593 15.9644Z" fill="#23232B" />
  </svg>
}


const ResizeIcon = () => (
<svg width="24" height="24" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
    <g clip-path="url(#clip0_11302_184516)">
    <path d="M4.80095 5.53999C4.80095 5.34759 4.87415 5.15519 5.02065 5.00879C5.31455 4.71679 5.78925 4.71679 6.08215 5.00979L18.9805 17.9297C19.2715 18.2227 19.2725 18.6983 18.9795 18.9912C18.6856 19.2832 18.2109 19.2832 17.918 18.9902L5.01965 6.07029C4.87415 5.92379 4.80095 5.73139 4.80095 5.53999Z" fill="black"/>
    <path d="M5.59105 10.9902C5.59105 10.7988 5.66425 10.6064 5.81075 10.4599C6.10275 10.1669 6.57835 10.1669 6.87125 10.4599L13.5441 17.1249C13.8361 17.4169 13.838 17.8915 13.5441 18.1854C13.2522 18.4784 12.7765 18.4784 12.4836 18.1854L5.81075 11.5204C5.66425 11.3739 5.59105 11.1826 5.59105 10.9902Z" fill="black"/>
    </g>
    <defs>
    <clipPath id="clip0_11302_184516">
    <rect width="24" height="24" fill="white" transform="matrix(0 1 -1 0 24 0)"/>
    </clipPath>
    </defs>
    </svg>

);

export default function DraggableWindow({

  initialWidth = 420,
  initialHeight = 420,
  initialX = 80,
  initialY = 80,
  
}) {
  const [pos, setPos] = useState(() => ({
    x:
      typeof window !== "undefined"
        ? Math.max(0, window.innerWidth - initialWidth - 95)
        : 90,
    y: 70,
  }));
  const [size, setSize] = useState({ w: initialWidth, h: initialHeight });
  const [closing, setClosing] = useState(false);
  const [inputValue, setInputValue] = useState("");
  const [isVisible, setIsVisible] = useState(true);
  const savedSize = useRef({ w: initialWidth, h: initialHeight });
  const winRef = useRef<any>(null);
  const dragState = useRef<any>(null);
  const resizeState = useRef<any>(null);
  const dispatch = useDispatch();
  const LoginCcaasUserDetails: any = GetItemFromStorage(LocalStorage.USER_DETAILS);


    useEffect(() => {
      if(window.location.pathname !== "/inbox"){
        setClosing(true)
        dispatch(commonDataSliceActions.setAiChat(false));
      }
  }, [window.location.pathname]);

  const handleMouseMove = useCallback((e) => {
    if (dragState.current) {
      const { startX, startY, startLeft, startTop } = dragState.current;
      const rect = winRef.current?.getBoundingClientRect();
      const w = rect?.width ?? 0;
      const h = rect?.height ?? 0;
      const maxX = Math.max(0, window.innerWidth - w);
      const maxY = Math.max(0, window.innerHeight - h);
      const nextX = startLeft + (e.clientX - startX);
      const nextY = startTop + (e.clientY - startY);
      setPos({
        x: Math.min(Math.max(0, nextX), maxX),
        y: Math.min(Math.max(0, nextY), maxY),
      });
    }
    if (resizeState.current) {
      const { startX, startY, startW, startH, startLeft, startTop } = resizeState.current;
      const dx = e.clientX - startX;
      const maxW = Math.max(260, startLeft + startW);
      const maxH = Math.max(300, window.innerHeight - startTop);
      const newW = Math.min(Math.max(260, startW - dx), maxW);
      const newH = Math.min(Math.max(300, startH + (e.clientY - startY)), maxH);
      const newLeft = Math.max(0, startLeft + (startW - newW));
      setSize({ w: newW, h: newH });
      setPos((p) => ({ x: newLeft, y: p.y }));
    }
  }, []);

  const handleMouseUp = useCallback(() => {
    dragState.current = null;
    resizeState.current = null;
    document.body.style.cursor = "";
    document.body.style.userSelect = "";
  }, []);

  useEffect(() => {
   getInitialHistory()
  },[])

  const getInitialHistory = useCallback(async() => {
    const body = {
      sessionId: null,
      agentId: LoginCcaasUserDetails?.userId ?? null,
    };
    await getAIChathistroyData(body)
      .then((res: any) => {
        dispatch(inboxDataSliceActions.setAiChatHistoryData(res));
      })
  },[LoginCcaasUserDetails?.userId])

  useEffect(() => {
    window.addEventListener("mousemove", handleMouseMove);
    window.addEventListener("mouseup", handleMouseUp);
    return () => {
      window.removeEventListener("mousemove", handleMouseMove);
      window.removeEventListener("mouseup", handleMouseUp);
    };
  }, [handleMouseMove, handleMouseUp]);

  const onTitlebarMouseDown = (e) => {
    if (e.target.closest(".dw-icon-btn") || !winRef.current) return;
    const rect = winRef.current.getBoundingClientRect();
    dragState.current = {
      startX: e.clientX,
      startY: e.clientY,
      startLeft: rect.left,
      startTop: rect.top,
    };
    document.body.style.cursor = "grabbing";
    document.body.style.userSelect = "none";
    e.preventDefault();
  };

  const onResizeMouseDown = (e) => {
    const rect = winRef.current.getBoundingClientRect();
    resizeState.current = {
      startX: e.clientX,
      startY: e.clientY,
      startW: rect.width,
      startH: rect.height,
      startLeft: rect.left,
      startTop: rect.top,
    };
    document.body.style.cursor = "nesw-resize";
    document.body.style.userSelect = "none";
    e.preventDefault();
    e.stopPropagation();
  };

  const handleClose = () => {
    savedSize.current = size;
    dispatch(commonDataSliceActions.setAiChat(false));
    setClosing(true);
    setTimeout(() => {
      setIsVisible(false);
    }, 200);
  };

  const [toggle, setToggle] = useState(false);
 
  const newchat = () => {
    const newSessonId = uuidv4();
    dispatch(inboxDataSliceActions.setInboxChatSessionId(newSessonId));
  }

  if (!isVisible) return null;

  return (
    <div
      ref={winRef}
      className={`dw-window${closing ? " dw-window--closing" : ""}`}
      style={{
        left: pos.x,
        top: pos.y,
        width: size.w,
        height: size.h || undefined,
      }}
    >
      {/* Titlebar */}
      <div className="dw-titlebar aichatdragtitle" onMouseDown={onTitlebarMouseDown}>

        {toggle ? (
          <>
            <div className="dw-icon-btn backbtn" aria-label="Split view" onClick={() => setToggle(false)} >
              <svg xmlns="http://www.w3.org/2000/svg" width="8" height="16" viewBox="0 0 8 16" fill="none">
                <path d="M7.04593 15.9644C6.81453 15.9644 6.58302 15.8755 6.40723 15.6988L0.889625 10.147C-0.295875 8.95361 -0.296875 7.01131 0.888625 5.81791L6.40323 0.266115C6.75283 -0.0883855 7.32413 -0.0873855 7.67573 0.261215C8.02923 0.611814 8.03123 1.18111 7.68063 1.53371L2.16603 7.08551C1.67483 7.57961 1.67582 8.38342 2.16702 8.87751L7.68463 14.4293C8.03423 14.7818 8.03323 15.3522 7.68073 15.7027C7.50493 15.8765 7.27543 15.9644 7.04593 15.9644Z" fill="#23232B" />
              </svg>
            </div>
            <span className="backbtn">Back</span>
          </>
        ) : (
          <div className="dw-titlebar title"> Ai Chat </div>
        )}


        <div className="dw-titlebar__spacer" />

        {!toggle && (
          <>
            <div className="dw-icon-btn" aria-label="Split view" onClick={newchat}> <Compose /></div>
            <div className="dw-icon-btn" aria-label="Split view" onClick={() => setToggle(true)}><History /></div>
          </>
        )}
        <div className="dw-icon-btn dw-icon-btn--close" aria-label="Close" onClick={handleClose}><CloseIcon /></div>
      </div>

        {toggle ?  (
          <InboxHistory getInitialHistory={getInitialHistory} goBack={() => setToggle(false)}/>
        ) : (
          <Inboxdrawer />
        )}


      <div className="dw-resize" onMouseDown={onResizeMouseDown} aria-hidden="true">
        <ResizeIcon />
      </div>
    </div>
  );
}
