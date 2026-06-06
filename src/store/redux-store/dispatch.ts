import { TypedUseSelectorHook, useDispatch, useSelector } from "react-redux";
import { AppDispatchType, RootState } from "./root_store";

export const useDispatcher: () => AppDispatchType = useDispatch;
export const useAppSelector: TypedUseSelectorHook<RootState> = useSelector;

