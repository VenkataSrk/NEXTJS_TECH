import { persistReducer, persistStore } from "redux-persist";
import { configureStore } from "@reduxjs/toolkit";
import rootReducer from "./root_reducer";
import storage from "redux-persist/lib/storage/session";

const persistConfig = {
  key: "root",
  storage: storage,
  blacklist: ["callLoginUserAgent", "callPopUpDetails"],
};

const persistedReducer = persistReducer(persistConfig, rootReducer);
const store = configureStore({
      reducer: persistedReducer,
      middleware: (getDefaultMiddleware) => getDefaultMiddleware({ serializableCheck: false }),
      devTools: true,
});
export const persistor = persistStore(store);
export type AppDispatchType = typeof store.dispatch;
export type RootState = ReturnType<typeof store.getState>;



export default store;
