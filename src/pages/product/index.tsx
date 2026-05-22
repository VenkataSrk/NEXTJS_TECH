"use-client"
import styles from './scss/product.module.scss'
import { formatDate, stripHtmlTags } from '../../base/utils';
import { useDispatcher } from '../../store/redux-store/dispatch';
import { Outlet, useNavigate, useSearchParams } from 'react-router-dom';
import { commonDataSliceActions } from '../../store/slice/commonSilce/commonSlice';
import { v4 as uuidv4 } from 'uuid';
import { GetItemFromStorage } from '../../base/custom_hooks/useStorage';
import { LocalStorage } from '../../base/custom_hooks/localStorageKeys';
import jwtDecode from 'jwt-decode';
import aiChatAssistSocketservice from '../../services/aiChatService';
import SetUpCommonEditor from '../shared/commonEditor';
import { productDataSliceActions } from '../../store/slice/productSlice';


const getInitials = (firstName?: string, lastName?: string) => {
    if (!firstName && !lastName) return "N";
    return `${firstName?.[0] || ""}${lastName?.[0] || ""}`.toUpperCase();
};

const getDisplayName = (firstName?: string, lastName?: string) => {
    const name = `${firstName || ""} ${lastName || ""}`.trim();
    return name || "New Customer";
};

const ProductsLayout = () => {
    const token = GetItemFromStorage(LocalStorage.ACCESS_TOKEN)
    const LoginCcaasUserDetails: any = token ? jwtDecode(token) : {};

    const dispatch = useDispatcher()
    const navigate = useNavigate()

    const [paramns] = useSearchParams();
    const productId = paramns.get("productId")

    const handleConvoStart = (value: any, files: any) => {
        const sessionId: any = uuidv4() + 'products';
        let body: any = {
            query: stripHtmlTags(value?.trim()),
            sessionId: sessionId,
            roleId: `${LoginCcaasUserDetails?.LoginCcaasUserDetails || ''}`,
            org_role_id_crm: LoginCcaasUserDetails?.org_role_id_crm?.toString() ?? "",
            userId: `${LoginCcaasUserDetails?.userId || ''}`,
            domainId: `${LoginCcaasUserDetails?.domainId || ''}`,
            module: "",
            ext: `${LoginCcaasUserDetails?.ext || ''}`,
            agent: 'products',
            isBot: 0,
            files: files ?? [],
            productID: productId ?? null,
        }

        aiChatAssistSocketservice.io.emit('crm_chat', body);

        dispatch(productDataSliceActions.setproductAiAgentChat({ ...body }))
        dispatch(productDataSliceActions.setproductAiAgentChatTyping({ sessionId: sessionId, typing: true }));

        const path = `/products/chat-conversation?sessionId=${sessionId}&productId=${productId}`
        dispatch(commonDataSliceActions.setTabMenuPush({ path: path, menuName: "New Conversation", close: 1, type: "products" }));
        navigate(path);
    }

    return (
        <>
            <div className={styles.tabContent}>
                <Outlet />
            </div>

            <div className={styles.editorbottom}>
                <SetUpCommonEditor onChange={handleConvoStart} />
            </div>
        </>
    )
}
export default ProductsLayout
