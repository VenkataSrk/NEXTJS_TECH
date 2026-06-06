import { persistReducer, persistStore } from "redux-persist";
import { configureStore } from "@reduxjs/toolkit";
import rootReducer from "./root_reducer";
import storage from "redux-persist/lib/storage/session";

// import { createBlacklistFilter } from "redux-persist-transform-filter";
// const blacklistFilter = createBlacklistFilter("editAgent", ["isModelOpen"]);

const persistConfig = {
  key: "root",
  storage: storage,
  // transforms: [blacklistFilter, callPopUp],
};

// const persistedReducer = persistReducer(persistConfig, rootReducer);

const store = configureStore({
  reducer: rootReducer,
  // reducer: persistedReducer,
  middleware: (getDefaultMiddleware) => getDefaultMiddleware({ serializableCheck: false }),
});
// export type AppDispatch = typeof store.dispatch;
// export type RootState = ReturnType<typeof store.getState>;



export const persistor = persistStore(store);
export default store;
