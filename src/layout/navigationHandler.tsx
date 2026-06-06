import { useEffect } from "react";
import { useNavigate } from "react-router-dom";
import { useDispatch, useSelector } from "react-redux";
import { commonDataSliceActions } from "../store/slice/commonSlice/commonSlice";

const NavigationHandler = () => {
  const navigate = useNavigate();
  const dispatch = useDispatch();

  const shouldNavigate = useSelector(
    (state: any) => state.commonData.navigateOperationHub
  );

  useEffect(() => {
    if (shouldNavigate) {
      navigate("/knowledgebaseOperationsHub");
      dispatch(commonDataSliceActions.setNavigateOperationHub(false));
    }
  }, [shouldNavigate]);

  return null;
};

export default NavigationHandler;
