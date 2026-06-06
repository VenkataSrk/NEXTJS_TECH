import { Navigate, Outlet } from 'react-router-dom';
import { GetItemFromStorage } from '../base/customhooks/useStorage';
import { LocalStorage } from '../base/customhooks/localStorageKeys';

const ControlledRoutes = () => {
    const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
    return accessToken ? <Outlet /> : <Navigate to="/login" replace />;
};

export default ControlledRoutes;