import { forwardRef, useEffect, useImperativeHandle, useRef, useState } from "react";
import styles from "./scss/texteditor.module.scss";
import CustomSingleSelectDropdown from "../component/CustomSingleSelect";

const TextEditor = forwardRef((props: any, ref) => {
  const { openTools, onchange, onlytext, bottom, initialContent, contentTick } = props;

  const editorRef = useRef<HTMLDivElement | null>(null);
  const [active, setActive] = useState<any>({});
  const savedRange = useRef<Range | null>(null);
  const [setFontSize,isSetFontSize] =useState(false);
  const [setFontFamily,isSetFontFamily] =useState(false);
  const [textColor,setTextColor] =useState("#fff");

  const updateToolbar = () => {
    if (!editorRef.current) return;

    const selection = document.getSelection();
    if (!selection || !selection.anchorNode) return;

    if (!editorRef.current.contains(selection.anchorNode)) return;

    setActive({
      bold: document.queryCommandState("bold"),
      italic: document.queryCommandState("italic"),
      underline: document.queryCommandState("underline"),
      strikeThrough: document.queryCommandState("strikeThrough"), // Fixed: was "strikeThrough"
      justifyLeft: document.queryCommandState("justifyLeft"),
      justifyCenter: document.queryCommandState("justifyCenter"),
      justifyRight: document.queryCommandState("justifyRight"),
      insertOrderedList: document.queryCommandState("insertOrderedList"),
      insertUnorderedList: document.queryCommandState("insertUnorderedList"),
    });
  };

  const exec = (command: string, value?: string) => {
    if(command=="foreColor"){
      setTextColor(value??"#fff");
    }
    if (!editorRef.current) return;

    editorRef.current.focus();

    const sel = window.getSelection();
    if (savedRange.current && sel) {
      sel.removeAllRanges();
      sel.addRange(savedRange.current);
    }

    document.execCommand(command, false, value);

    requestAnimationFrame(updateToolbar);
  };

  const insertEmoji = (emoji: string) => {
    if (!editorRef.current) return;

    editorRef.current.focus();

    const selection = window.getSelection();
    let range: Range;

    if (selection && selection.rangeCount > 0 && editorRef.current.contains(selection.anchorNode)) {
      range = selection.getRangeAt(0);
    } else {
      range = document.createRange();

      if (!editorRef.current.innerHTML.trim() || editorRef.current.innerHTML === "<p><br/></p>") {
        const p = document.createElement("p");
        editorRef.current.innerHTML = "";
        editorRef.current.appendChild(p);
        range.selectNodeContents(p);
      } else {
        const lastChild = editorRef.current.lastElementChild || editorRef.current;
        range.selectNodeContents(lastChild);
      }

      range.collapse(false);
      selection?.removeAllRanges();
      selection?.addRange(range);
    }

    const textNode = document.createTextNode(emoji);
    range.deleteContents();
    range.insertNode(textNode);

    range.setStartAfter(textNode);
    range.setEndAfter(textNode);
    selection?.removeAllRanges();
    selection?.addRange(range);

    savedRange.current = range.cloneRange();
    onchange?.({ target: { innerHTML: editorRef.current.innerHTML } });
  };
  useImperativeHandle(ref, () => ({
    insertEmoji,
  }));

  useEffect(() => {
    const handler = () => {
      updateToolbar();

      const selection = window.getSelection();
      if (selection && selection.rangeCount > 0) {
        savedRange.current = selection.getRangeAt(0).cloneRange();
      }
    };

    document.addEventListener("selectionchange", handler);
    editorRef.current?.addEventListener("keyup", handler);
    editorRef.current?.addEventListener("mouseup", handler);

    return () => {
      document.removeEventListener("selectionchange", handler);
      editorRef.current?.removeEventListener("keyup", handler);
      editorRef.current?.removeEventListener("mouseup", handler);
    };
  }, []);

  useEffect(() => {
    if (editorRef.current) {
      let content = typeof initialContent?.content === "string" ? initialContent?.content : "";
      if(initialContent?.fresh ||  !editorRef.current.innerText?.trim()){
        editorRef.current.innerHTML = "";
      } else if (initialContent?.bottomPush) {
        content = (editorRef.current.innerHTML || "") + `<br/><br/> ${content}`;
      } else {
        content = `${content} <br/><br/>` + (editorRef.current.innerHTML || "");
      }
      editorRef.current.innerHTML = initialContent?.content ? content : editorRef.current.innerHTML;
    }
  }, [initialContent?.content, contentTick]);

  const icons = {
    B: (
      <svg xmlns="http://www.w3.org/2000/svg" width="15" height="18" viewBox="0 0 15 18" fill="none">
        <path d="M14.4731 12.7199C14.4924 12.551 14.5005 12.3869 14.5005 12.2299C14.5005 12.1193 14.4963 12.0138 14.4905 11.9135C14.4905 11.9123 14.4904 11.9113 14.4903 11.9103C14.4901 11.9076 14.4903 11.9047 14.4901 11.902C14.4721 11.5307 14.4043 11.2008 14.33 10.9035L14.323 10.8776C13.9959 9.7458 13.2017 8.8451 12.2121 8.3007C12.0037 8.1852 11.7938 8.1051 11.5841 8.028C11.7836 7.8576 11.975 7.6745 12.1413 7.4645C12.1423 7.4633 12.1436 7.4623 12.1446 7.4611C12.1454 7.4601 12.1464 7.4594 12.1472 7.4584C12.7894 6.6564 13.0886 5.6774 13.0882 4.716C13.088 3.175 12.3249 1.6176 10.8637 0.8032L10.8606 0.8015C9.4814 0.0537001 7.9498 0.0046 6.5919 0H0.75C0.3358 0 0 0.3357 0 0.75C0 1.1641 0.3358 1.5 0.75 1.5L1.9886 1.4998V15.7461H0.75C0.3358 15.7461 0 16.0818 0 16.4961C0 16.9102 0.3358 17.2461 0.75 17.2461L6.5919 17.2459C7.2538 17.2459 8.1355 17.2466 9.0264 17.1758C9.92 17.1018 10.8182 16.9702 11.5802 16.6406L11.5833 16.6394C13.1568 15.9458 14.2887 14.491 14.4716 12.7307C14.4718 12.7287 14.4723 12.7268 14.4726 12.7248C14.4728 12.7231 14.4729 12.7216 14.4731 12.7199ZM6.5921 1.4997C7.9041 1.4951 9.1812 1.5851 10.1383 2.1164C11.0577 2.621 11.5886 3.6581 11.5884 4.716C11.588 5.3727 11.385 6.0134 10.9731 6.5253L10.9679 6.5319C10.7373 6.8266 10.4142 7.0854 10.073 7.2607L10.0626 7.2661C9.6126 7.5071 9.0316 7.6636 8.2891 7.664C8.2874 7.664 8.286 7.665 8.2843 7.665H3.4889V1.4997H6.5921ZM12.9827 12.5556L12.9815 12.5663C12.855 13.7772 12.0975 14.7663 10.9763 15.2675C10.5003 15.4797 9.7168 15.6191 8.9076 15.6806C8.0946 15.7453 7.2516 15.746 6.5921 15.746H3.4888V9.1649L9.068 9.1647C10.0208 9.1637 10.7949 9.2272 11.4855 9.6127L11.4883 9.6142C12.1637 9.9858 12.6734 10.5832 12.8772 11.2819C12.939 11.5314 12.9817 11.7511 12.9922 11.9816L12.9928 11.9931L13.0005 12.2299C13.0005 12.3412 12.995 12.4511 12.9827 12.5556Z" fill="#C9C9CF" />
      </svg>
    ),

    I: (
      <svg width="15" height="17" viewBox="0 0 15 17" fill="none">
        <path d="M13.5184 0H6.0406C5.6263 0 5.2906 0.3357 5.2906 0.75C5.2906 1.1641 5.6263 1.5 6.0406 1.5H9.0659L3.5815 14.8938H0.75C0.3357 14.8938 0 15.2295 0 15.6438C0 16.0579 0.3357 16.3938 0.75 16.3938H8.2278C8.642 16.3938 8.9778 16.0579 8.9778 15.6438C8.9778 15.2295 8.642 14.8938 8.2278 14.8938H5.2024L10.6868 1.5H13.5185C13.9327 1.5 14.2685 1.1641 14.2685 0.75C14.2685 0.3357 13.9326 0 13.5184 0Z" fill="#C9C9CF" />
      </svg>
    ),

    U: (
      <svg xmlns="http://www.w3.org/2000/svg" width="15" height="19" viewBox="0 0 15 19" fill="none">
        <path d="M14.2129 17.2754L0.751502 17.2481H0.750002C0.336402 17.2481 0.00100223 17.5831 2.22967e-06 17.9961C-0.00099777 18.4111 0.334502 18.7471 0.748502 18.7481L14.2099 18.7754H14.2114C14.625 18.7754 14.9604 18.4404 14.9614 18.0274C14.9624 17.6124 14.6269 17.2764 14.2129 17.2754Z" fill="#C9C9CF" />
        <path d="M0.941402 1.5H0.967802V9.7705C0.967802 13.2031 3.7603 15.9951 7.1924 15.9951H7.7695C11.2016 15.9951 13.9941 13.2031 13.9941 9.7705V1.5H14.0205C14.4346 1.5 14.7705 1.1641 14.7705 0.75C14.7705 0.3359 14.4346 0 14.0205 0H12.4756C12.0615 0 11.7256 0.3359 11.7256 0.75C11.7256 1.1641 12.0615 1.5 12.4756 1.5H12.4942V9.7705C12.4942 12.376 10.3746 14.4951 7.7696 14.4951H7.1925C4.5875 14.4951 2.4679 12.376 2.4679 9.7705V1.5H2.4865C2.9006 1.5 3.2365 1.1641 3.2365 0.75C3.2365 0.3359 2.9006 0 2.4865 0H0.941602C0.527502 0 0.191602 0.3359 0.191602 0.75C0.191602 1.1641 0.527302 1.5 0.941402 1.5Z" fill="#C9C9CF" />
      </svg>
    ),

    S: (
      <svg xmlns="http://www.w3.org/2000/svg" width="20" height="18" viewBox="0 0 20 18" fill="none">
        <path d="M13.68 11.04C13.9798 11.5148 14.2421 12.1393 14.2171 12.8261C14.1796 14.1627 13.018 15.0244 11.0695 15.1995C8.5215 15.4242 5.2615 13.9004 5.224 13.8879C4.7619 13.6632 4.1998 13.8629 3.975 14.3252C3.7502 14.7872 3.95 15.3492 4.4246 15.5742C4.5621 15.6364 7.6472 17.098 10.5325 17.098C10.7698 17.098 11.0071 17.0855 11.232 17.0605C14.7169 16.7482 16.0284 14.7121 16.0908 12.9011C16.1158 12.2141 15.9784 11.5895 15.766 11.04H13.68Z" fill="#C9C9CF" />
        <path d="M19.6474 8.10217C19.3909 7.88627 19.0403 7.88627 18.7163 7.88627H12.5863C12.5757 7.88287 12.5683 7.87467 12.5576 7.87197C12.1996 7.77557 11.8111 7.69637 11.425 7.61707C11.0585 7.54147 10.6944 7.46827 10.3577 7.37307L10.004 7.28037C9.1148 7.04737 8.1957 6.80587 7.5687 6.30457C6.9625 5.82037 6.6039 5.11407 6.5837 4.36637C6.5654 3.71257 6.8868 3.05027 7.418 2.63917C7.9968 2.19637 8.8268 2.02197 9.4026 1.92927C10.8438 1.69627 13.2687 2.24157 14.081 2.47327C14.5762 2.61477 15.0958 2.32567 15.2386 1.82927C15.3801 1.33157 15.0922 0.813169 14.5952 0.671669C14.4598 0.632669 11.2494 -0.268732 9.1013 0.0800684C8.328 0.204468 7.1942 0.452068 6.2763 1.15347C5.2608 1.93777 4.676 3.15877 4.7113 4.41637C4.7455 5.71667 5.3602 6.93767 6.3989 7.76827C6.4558 7.81367 6.5203 7.84387 6.5789 7.88627H1.2685C0.9581 7.88627 0.5937 7.88627 0.3373 8.10217C0.135 8.26407 0 8.52057 0 8.80387C0 9.08717 0.1214 9.33017 0.3238 9.50557C0.4857 9.65407 0.7017 9.69467 0.9176 9.70807H1.174C1.174 9.70807 1.093 9.70807 3.8188 9.70807H12.2701C12.2849 9.70867 12.3 9.71387 12.3147 9.71387C12.3267 9.71387 12.3377 9.70837 12.3497 9.70807H19.0671C19.2829 9.69467 19.4854 9.64037 19.6607 9.50557C19.8632 9.34367 19.9845 9.08717 19.9845 8.80387C19.9845 8.52057 19.8498 8.26417 19.6474 8.10217Z" fill="#C9C9CF" />
      </svg>
    ),
    d: (
      <svg xmlns="http://www.w3.org/2000/svg" width="20" height="17" viewBox="0 0 20 17" fill="none">
        <path d="M1.698 3.396C2.63578 3.396 3.396 2.63578 3.396 1.698C3.396 0.76022 2.63578 0 1.698 0C0.760221 0 0 0.76022 0 1.698C0 2.63578 0.760221 3.396 1.698 3.396Z" fill="#C9C9CF" />
        <path d="M1.698 10C2.63578 10 3.396 9.23978 3.396 8.302C3.396 7.36422 2.63578 6.604 1.698 6.604C0.760221 6.604 0 7.36422 0 8.302C0 9.23978 0.760221 10 1.698 10Z" fill="#C9C9CF" />
        <path d="M1.698 13.2078C0.7601 13.2078 0 13.9681 0 14.906C0 15.8439 0.7601 16.604 1.698 16.604C2.6359 16.604 3.396 15.8437 3.396 14.906C3.396 13.9683 2.6357 13.2078 1.698 13.2078Z" fill="#C9C9CF" />
        <path d="M6.5809 2.698H16.9624C17.5146 2.698 17.9624 2.2502 17.9624 1.698C17.9624 1.1458 17.5146 0.698 16.9624 0.698H6.5809C6.0285 0.698 5.5809 1.1455 5.5809 1.698C5.5809 2.2505 6.0285 2.698 6.5809 2.698Z" fill="#C9C9CF" />
        <path d="M16.9624 7.302H6.5809C6.0285 7.302 5.5809 7.7495 5.5809 8.302C5.5809 8.8545 6.0285 9.302 6.5809 9.302H16.9624C17.5146 9.302 17.9624 8.8542 17.9624 8.302C17.9624 7.7498 17.5146 7.302 16.9624 7.302Z" fill="#C9C9CF" />
        <path d="M16.9624 13.906H6.5809C6.0285 13.906 5.5809 14.3535 5.5809 14.906C5.5809 15.4585 6.0285 15.906 6.5809 15.906H16.9624C17.5146 15.906 17.9624 15.4582 17.9624 14.906C17.9624 14.3538 17.5146 13.906 16.9624 13.906Z" fill="#C9C9CF" />
      </svg>
    ),
    o: (
      <svg xmlns="http://www.w3.org/2000/svg" width="19" height="17" viewBox="0 0 19 17" fill="none">
        <path d="M1.09565 6.6921V6.6809H3.87095C4.36335 6.6809 4.71025 6.3115 4.71025 5.8415C4.71025 5.3715 4.37455 5.0023 3.87095 5.0023H3.31135V0.8394C3.31135 0.3806 2.97575 0 2.47215 0H1.36415C0.938948 0 0.536148 0.3694 0.536148 0.8394C0.536148 1.3094 0.938948 1.6788 1.37535 1.6788H1.65515V5.0135H1.09555C0.670348 5.0135 0.256348 5.3941 0.256348 5.8527C0.256348 6.3113 0.659148 6.6921 1.09565 6.6921Z" fill="#C9C9CF" />
        <path d="M3.90457 14.8286V14.8398H2.75187L3.96047 13.5303C4.72137 12.7023 4.74377 11.4042 3.99397 10.5872C3.05387 9.51297 1.36407 9.60238 0.547275 10.7551L0.155475 11.3034C-0.113125 11.6726 -0.0235251 12.1987 0.345775 12.4673C0.715075 12.7359 1.24108 12.6462 1.50968 12.277L1.90137 11.7399C2.10277 11.4602 2.50568 11.4378 2.74067 11.6952C2.91978 11.8967 2.91978 12.2099 2.74067 12.4114L0.233875 15.0972C0.0101748 15.3434 -0.0458252 15.6902 0.0883748 16.0035C0.222675 16.3058 0.524875 16.5071 0.849275 16.5071H3.90447C4.36317 16.5071 4.74367 16.1379 4.74367 15.6679C4.74367 15.1979 4.37448 14.8286 3.90457 14.8286Z" fill="#C9C9CF" />
        <path d="M6.71436 2.64941H17.0959C17.6481 2.64941 18.0959 2.20161 18.0959 1.64941C18.0959 1.09721 17.6481 0.649414 17.0959 0.649414H6.71436C6.16196 0.649414 5.71436 1.09691 5.71436 1.64941C5.71436 2.20191 6.16196 2.64941 6.71436 2.64941Z" fill="#C9C9CF" />
        <path d="M17.0959 7.25342H6.71436C6.16196 7.25342 5.71436 7.70092 5.71436 8.25342C5.71436 8.80592 6.16196 9.25342 6.71436 9.25342H17.0959C17.6481 9.25342 18.0959 8.80562 18.0959 8.25342C18.0959 7.70122 17.6481 7.25342 17.0959 7.25342Z" fill="#C9C9CF" />
        <path d="M17.0959 13.8574H6.71436C6.16196 13.8574 5.71436 14.3049 5.71436 14.8574C5.71436 15.4099 6.16196 15.8574 6.71436 15.8574H17.0959C17.6481 15.8574 18.0959 15.4096 18.0959 14.8574C18.0959 14.3052 17.6481 13.8574 17.0959 13.8574Z" fill="#C9C9CF" />
      </svg>
    )
  };

  const btn = (command: string, label: string) => (

    <div
      className={`${styles.button} ${active[command] ? styles.active : ""}`}
      onMouseDown={(e) => {
        e.preventDefault();
        exec(command);
      }}
    >
      {icons[label] || label}
    </div>
  );
  const fontOptions = [
    { key: "Inter", value: "Sans Serif" },
    { key: "Arial", value: "Arial" },
    { key: "Georgia", value: "Georgia" },
  ];
  const fontSizeOptions = [...Array(8)].map((_, i) => ({
    key: String(i + 1),     // important: exec expects 1–7 (string works safest)
    value: String(i + 10),  // what user sees (10–17)
  }));

  const colorRef = useRef<HTMLInputElement>(null);
  const colorHighRef = useRef<HTMLInputElement>(null);



  return (
    <div className={styles.editorWrapper} style={{ height:  "auto", alignItems:"center"}}>

      {!!openTools &&
        <div className={` toolbar ${styles.toolbar} ${bottom ? styles.bottom : ""} selectforeditor`}>

          {btn("bold", "B")}
          {btn("italic", "I")}
          {btn("underline", "U")}
          {btn("strikeThrough", "S")}
          {onlytext ? <> </> : <>
            {btn("justifyLeft", "L")}
            {btn("justifyCenter", "C")}
            {btn("justifyRight", "R")}</>}
          {onlytext ? <> </> :
            <>
              <div style={{ position: "relative", display: "inline-block" }}>
                <select
                  onChange={(e) => { exec("fontName", e.target.value); isSetFontFamily(true) }}
                  style={{
                    background: "#F0F1F2",
                    color: "black",
                    borderRadius: "100px",
                    minHeight: "38px",
                    paddingRight: "30px",
                    appearance: "none",
                    WebkitAppearance: "none",
                    MozAppearance: "none",
                  }}

                >
                  {/* Placeholder */}
                  {!setFontFamily && <option value="" >
                    Font
                  </option>}
                  <option value="Inter">Sans Serif</option>
                  <option value="Arial">Arial</option>
                  <option value="Georgia">Georgia</option>
                </select>

                {/* SVG Icon */}
                <span
                  style={{
                    position: "absolute",
                    right: "10px",
                    top: "46%",
                    transform: "translateY(-50%)",
                    pointerEvents: "none",
                  }}
                >
                  <svg xmlns="http://www.w3.org/2000/svg" width="12" height="6" viewBox="0 0 12 6" fill="none"><path d="M5.66561 5.80465C5.06311 5.80465 4.49761 5.57025 4.07231 5.14355L0.218813 1.27925C-0.0736866 0.98625 -0.0726864 0.511651 0.220314 0.218751C0.514214 -0.0732493 0.989313 -0.0732494 1.28131 0.220751L5.13431 4.08505C5.41801 4.36825 5.91361 4.36825 6.19631 4.08505L10.0498 0.220751C10.3418 -0.0731494 10.8169 -0.0731493 11.1108 0.218751C11.4038 0.511751 11.4047 0.98635 11.1123 1.27925L7.25881 5.14355C6.83351 5.57035 6.26771 5.80465 5.66561 5.80465Z" fill="black"></path></svg>
                </span>
              </div>
              {/* <select onChange={(e) => exec("fontName", e.target.value)}>
                <option value="Inter">Sans Serif</option>
                <option value="Arial">Arial</option>
                <option value="Georgia">Georgia</option>
              </select> */}
              {/* <CustomSingleSelectDropdown
                label="Font"
                options={fontOptions}
                onChange={(selected) => {
                  if (selected) {
                    exec("fontName", selected.key);
                  }
                }}
              /> */}

             <div style={{ position: "relative", display: "inline-block" }}>
  <select
    onChange={(e) =>{ exec("fontSize", e.target.value); isSetFontSize(true)}}
    style={{
      background: "#F0F1F2",
      color: "black",
      borderRadius: "100px",
      minHeight: "38px",
      paddingRight: "30px",
      appearance: "none",
      WebkitAppearance: "none",
      MozAppearance: "none",
    }}

  >
     {/* Placeholder */}
  {!setFontSize &&<option value="" >
    Size
  </option>}
    {[...Array(8)].map((_, i) => (
      <option key={i} value={i + 1}>
        {i + 10}
      </option>
    ))}
  </select>

  {/* SVG Icon */}
  <span
    style={{
      position: "absolute",
      right: "10px",
      top: "46%",
      transform: "translateY(-50%)",
      pointerEvents: "none",
    }}
  >
    <svg xmlns="http://www.w3.org/2000/svg" width="12" height="6" viewBox="0 0 12 6" fill="none"><path d="M5.66561 5.80465C5.06311 5.80465 4.49761 5.57025 4.07231 5.14355L0.218813 1.27925C-0.0736866 0.98625 -0.0726864 0.511651 0.220314 0.218751C0.514214 -0.0732493 0.989313 -0.0732494 1.28131 0.220751L5.13431 4.08505C5.41801 4.36825 5.91361 4.36825 6.19631 4.08505L10.0498 0.220751C10.3418 -0.0731494 10.8169 -0.0731493 11.1108 0.218751C11.4038 0.511751 11.4047 0.98635 11.1123 1.27925L7.25881 5.14355C6.83351 5.57035 6.26771 5.80465 5.66561 5.80465Z" fill="black"></path></svg>
  </span>
</div>
              {/* <CustomSingleSelectDropdown
                label="Size"
                options={fontSizeOptions}
                onChange={(selected) => {
                  if (selected) {
                    exec("fontSize", selected.key);
                  }
                }}
              /> */}

              {/* <input
                type="color"
                onChange={(e) => exec("foreColor", e.target.value)} className={styles.pickerstate}
              /> */}
              <div className={styles.colorDropdown}>
                <button
                  className={styles.dropdownBtn}
                  onClick={() => {colorRef.current?.click();}}
                >
                  <span className={styles.textIcon}>A</span>
                  <span style={{background:textColor}} className={styles.bottomline}></span>
                  <span className={styles.divider}></span>
                  <span className={styles.arrow}>
                    <svg xmlns="http://www.w3.org/2000/svg" width="8" height="4" viewBox="0 0 8 4" fill="none">
                      <path d="M3.77708 3.86977C3.37541 3.86977 2.99841 3.7135 2.71488 3.42903L0.145876 0.852833C-0.0491244 0.6575 -0.0484576 0.3411 0.146876 0.145834C0.342809 -0.0488329 0.659542 -0.0488329 0.854209 0.147167L3.42288 2.72337C3.61201 2.91217 3.94241 2.91217 4.13088 2.72337L6.69988 0.147167C6.89454 -0.0487663 7.21128 -0.0487662 7.40721 0.145834C7.60254 0.341167 7.60314 0.657567 7.40821 0.852833L4.83921 3.42903C4.55568 3.71357 4.17848 3.86977 3.77708 3.86977Z" fill="#E3E4E5" />
                    </svg>
                  </span>
                </button>
                <input
                  ref={colorRef}
                  type="color"
                  value={textColor} 
                  onChange={(e) => {
                    exec("foreColor", e.target.value); }}
                  className={styles.hiddenColorInput}
                />
              </div>

              <div className={styles.colorDropdown}>
                <button
                  className={styles.dropdownBtn}
                  onClick={() => colorHighRef.current?.click()}
                >
                  <span className={styles.textIcon} style={{display: 'grid'}}><svg xmlns="http://www.w3.org/2000/svg" width="11" height="11" viewBox="0 0 11 11" fill="none" style={{width: '14px'}}>
                  <path d="M0.133007 9.19475L0.132874 9.19314C0.0484075 9.27681 0.00100733 9.39028 0.000473997 9.51001L7.33203e-06 9.59808C-0.00139267 9.84648 0.197874 10.0475 0.446074 10.0487L1.72754 10.0556C1.84741 10.0561 1.96134 10.01 2.04581 9.92641L2.62801 9.35021L1.31614 8.02388L0.133007 9.19475Z" fill="#E3E4E5"/>
                  <path d="M10.3151 1.44415L9.20867 0.338013C8.78614 -0.0845207 8.12081 -0.113787 7.66147 0.266413L2.88121 4.23321C2.63281 4.43961 2.48181 4.74295 2.46714 5.06588C2.45574 5.32208 2.53387 5.57175 2.67967 5.77841L2.03941 6.41868C1.86301 6.59508 1.76594 6.82948 1.76594 7.07881C1.76594 7.32748 1.86261 7.56188 2.03907 7.73835L2.91407 8.61401C3.09574 8.79501 3.33434 8.88548 3.57327 8.88548C3.81187 8.88548 4.05081 8.79501 4.23281 8.61335L4.87301 7.97335C5.06801 8.11121 5.29787 8.18621 5.53327 8.18621C5.64914 8.18621 5.76567 8.16861 5.87961 8.13281C6.09121 8.06708 6.27807 7.94141 6.41934 7.77081L10.3865 2.99155C10.7689 2.53188 10.7383 1.86668 10.3151 1.44415ZM9.61721 2.35235L5.64941 7.13228C5.63087 7.15441 5.60807 7.17001 5.57974 7.17915C5.53874 7.19215 5.47621 7.18955 5.42834 7.14201L3.50974 5.22275C3.47134 5.18435 3.46481 5.14075 3.46614 5.11075C3.46747 5.08601 3.47587 5.03915 3.52021 5.00201L8.30014 1.03588C8.32814 1.01241 8.36201 1.00135 8.39587 1.00135C8.43427 1.00135 8.47234 1.01568 8.50167 1.04495L9.60847 2.15108C9.66254 2.20515 9.66634 2.29375 9.61721 2.35235Z" fill="#E3E4E5"/>
                </svg></span>
                  <span className={styles.divider}></span>
                  <span className={styles.arrow}>
                    <svg xmlns="http://www.w3.org/2000/svg" width="8" height="4" viewBox="0 0 8 4" fill="none">
                      <path d="M3.77708 3.86977C3.37541 3.86977 2.99841 3.7135 2.71488 3.42903L0.145876 0.852833C-0.0491244 0.6575 -0.0484576 0.3411 0.146876 0.145834C0.342809 -0.0488329 0.659542 -0.0488329 0.854209 0.147167L3.42288 2.72337C3.61201 2.91217 3.94241 2.91217 4.13088 2.72337L6.69988 0.147167C6.89454 -0.0487663 7.21128 -0.0487662 7.40721 0.145834C7.60254 0.341167 7.60314 0.657567 7.40821 0.852833L4.83921 3.42903C4.55568 3.71357 4.17848 3.86977 3.77708 3.86977Z" fill="#E3E4E5" />
                    </svg>
                  </span>
                </button>
                <input
                  ref={colorHighRef}
                  type="color"
                  // onChange={(e) => exec("foreColor", e.target.value)}
                  onChange={(e) => exec("hiliteColor", e.target.value)}
                  className={styles.hiddenColorInput}
                />
              </div>
            </>
          }

          {btn("insertUnorderedList", "d")}
          {btn("insertOrderedList", "o")}
        </div>}

      <div
        ref={editorRef}
        style={{ height: "100%", width:"100%" }}
        id="emailcontent"
        className={styles.editor}
        contentEditable={true}
        suppressContentEditableWarning
        data-placeholder="Write a message"
        onClick={() => {
          if (editorRef.current?.textContent?.trim() === "") {
            editorRef.current.innerHTML = "<p><br/></p>";
          }
        }}
        onInput={onchange}
        onFocus={(e) => {
          // if (!e.currentTarget.textContent?.trim()) {
          //   e.currentTarget.innerHTML = "<p><br/></p>";
          // }
        }}
      />
    </div>
  );
});

export default TextEditor;