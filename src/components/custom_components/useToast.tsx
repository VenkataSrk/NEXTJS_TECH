import { useState } from "react";

export const useToast = () => {
  const [toastOpen, setToastOpen] = useState(false);
  const [toastMessage, setToastMessage] = useState<string>("");

  const showToast = (msg: string) => {
    setToastMessage(msg);
    setToastOpen(true);
  };

  const closeToast = () => setToastOpen(false);

  return { toastOpen, toastMessage, showToast, closeToast };
};
