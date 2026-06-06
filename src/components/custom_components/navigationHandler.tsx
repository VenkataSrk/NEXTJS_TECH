// NavigationHandler.tsx
import { useEffect } from 'react';
import { useNavigate } from 'react-router-dom';

const NavigationHandler = () => {
  const navigate = useNavigate();

  useEffect(() => {
    const handler = (event: MessageEvent) => {
      if (event?.data?.type === "CAMPAIGN_IFRAME") {
        navigate(-1);
      }
    };

    window.addEventListener("message", handler);
    return () => window.removeEventListener("message", handler);
  }, [navigate]);

  return null;
};

export default NavigationHandler;
