//  prettier-ignore
export const config = () => {
      let BASE_URL;
      let CORESERVICE_URL;
      let CALLSERVICE_URL;
      let WORKTUALBOT;
      let OTHERBOT;
      let TEMPLATEBOT;
      let MERCHANT_ID;
      let SECRET_KEY;
      let PAYMENT_URL;
      let RETURN_URL;
      let ACCOUNT;
      let CONNECTOR_URL;
      let EMAIL_URL;
      let CREATE_SHOPIFY_CUSTOMER;
      let SHOPIFY_CONNECTION_LIST;
      let SHOPIFY_TOKEN_GENERATOR;
      let SHOPIFY_CHECKOUT;
      let MAGENTO_TOKEN_GENERATOR;
      let MAGENTO_PRODUCT_SEARCH;
      let MAGENTO_REFRESH_TOKEN;
      let BIGCOMMERCE_SEARCH_LIST;
      let BIGCOMMERCE_TOKEN_GENERATOR;
      let SHOPIFY_PRODUCT_SEARCH;
      let WORKTUAL_APPOINTMENT;
      let BIGCOMMERCE_GET_VARIANTS;
      let WIX_TOKEN_GENERATOR;
      let WIX_PRODUCT_SEARCH;
      let WIX_EXISTING_CUSTOMER;
      let WIX_CREATE_CUSTOMER;
      let WIX_ORDER_CREATION;
      let BIGCOMMERCE_EXISTING_CUSTOMER;
      let BIGCOMMERCE_CREATE_CUSTOMER;
      let BIGCOMMERCE_CREATE_ORDER;
      let ORDER_TOKEN_GENERATOR;
      let BIG_COMMERCE_PAYMENT;
      let MAGENTO_EXISTING_CUSTOMER;
      let MAGENTO_CREATE_CUSTOMER;
      let MAGENTO_CREATE_ORDER;
      let WIX_PRODUCT_SEARCH_ID;
      let WIX_CONTACT_UPDATE;
      let WIX_REFRESH_TOKEN_GENERATOR;
      let WOOCOMMERCE_TOKEN_GENERATOR;
      let WOOCOMMERCE_PRODUCT_SEARCH;
      let WOOCOMMERCE_GET_VARIANT;
      let WOOCOMMERCE_EXISTING_CUSTOMER;
      let ECOMMERCE_IMAGE_SEARCH;
      let WOOCOMMERECE_CREATE_CUSTOMER;
      let WOOCOMMERECE_UPDATE_CUSTOMER;
      let WOOCOMMERCE_ORDER_CREATION;
      let WOOCOMMERCE_PRODUCT_SEARCH_ID;
      let WOOCOMMERCE_OFFER_SEARCH;
      let REPORTSERVICE_URL;
      let CARE_HOME;
      let CHATBOT_PAYMENT_SERVICE;
      let B2C_PRODUCT_SEARCH;
      let MYACCOUNT;
      let NLP_CONVO_SUMMARY;
      let CONNECTOR_COMMON_LIST;
      let CONNECTOR_ZOHO_REFRESH;
      let CONNECTOR_ZOHO_LEADS;
      let SHOPIFY_PRODUCT_RECOMMENDATION;
      let WIX_CHECKOUT = "https://ccaasapi.worktual.co.uk/connector/wix/v1/checkout";

      if (process.env.REACT_APP_ENV === "development") {
            BASE_URL = "https://stageccaasapi.worktual.co.uk"
            CORESERVICE_URL = "https://stageccaasapi.worktual.co.uk/core_service/v1";
            REPORTSERVICE_URL = 'https://stageccaasapi.worktual.co.uk/reports_service/v1';
            CALLSERVICE_URL = "htps://stageccaasapi.worktual.co.uk/callservice/v1";
            WORKTUALBOT = "https://testvideo-meet.worktual.co.uk/chatbot";
            OTHERBOT = "https://testvideo-meet.worktual.co.uk/vectonechatbot";
            TEMPLATEBOT = "http://10.150.3.150:3005/ask";
            MERCHANT_ID = "Unitedfone"; // vectone  Unitedfone
            SECRET_KEY = "Tprbt9cpvb"; // KGCWbMuLRd  Tprbt9cpvb
            PAYMENT_URL = "https://pay.sandbox.realexpayments.com/pay";
            RETURN_URL = "https://liveurapi.worktual.co.uk/vectone-myaccount/v1/user/test_callback";
            ACCOUNT = "internet"; // uk internet
            CONNECTOR_URL = "https://qaai-ccaasapi.worktual.co.uk/connector/calendar/v1"
            EMAIL_URL = "https://qaai-ccaasapi.worktual.co.uk/emailService/v1"
            WORKTUAL_APPOINTMENT = "https://qabookingapi.worktual.co.uk/booking_service/v1"
            CARE_HOME = "https://carehomeapi.audracare.co.uk/homecare_service/v1";
            CHATBOT_PAYMENT_SERVICE = "https://qachatpayment.worktual.co.uk"
            B2C_PRODUCT_SEARCH = "https://b2c.worktual.co.uk/ws_b2c"
            MYACCOUNT = 'https://qamysqlapi.worktual.co.uk/myaccount-mysql/v1/worktual'
            NLP_CONVO_SUMMARY="https://nlpccaas.worktual.co.uk/summarize_convo";
            CONNECTOR_COMMON_LIST="https://qaai-ccaasapi.worktual.co.uk/connector/common/v1/list";
            CONNECTOR_ZOHO_REFRESH="https://qaai-ccaasapi.worktual.co.uk/connector/zoho/v1/refresh_token";
            CONNECTOR_ZOHO_LEADS= "https://qaai-ccaasapi.worktual.co.uk/connector/zoho/v1/leads";
            SHOPIFY_PRODUCT_RECOMMENDATION= "https://nlp.worktual.co.uk/get_product_recommendation";

      } else if (process.env.REACT_APP_ENV === "production") {
            BASE_URL = "https://ai-ccaasapi.worktual.co.uk";
            CORESERVICE_URL = "https://ai-ccaasapi.worktual.co.uk/core_service/v1";
            CALLSERVICE_URL = "https://ai-ccaasapi.worktual.co.uk/callservice/v1";
            REPORTSERVICE_URL = 'https://ai-ccaasapi.worktual.co.uk/reports_service/v1';
            WORKTUALBOT = "https://testvideo-meet.worktual.co.uk/chatbot";
            OTHERBOT = "https://testvideo-meet.worktual.co.uk/vectonechatbot";
            TEMPLATEBOT = "https://testvideo-meet.worktual.co.uk/ask";
            MERCHANT_ID = "vectone"; // vectone  Unitedfone
            SECRET_KEY = "KGCWbMuLRd"; // KGCWbMuLRd  Tprbt9cpvb
            PAYMENT_URL = "https://pay.realexpayments.com/pay";
            RETURN_URL = "https://liveurapi.worktual.co.uk/vectone-myaccount/v1/user/test_callback";
            ACCOUNT = "uk"; // uk internet
            CONNECTOR_URL = "https://ai-ccaasapi.worktual.co.uk/connector/calendar/v1"
            EMAIL_URL = "https://ai-ccaasapi.worktual.co.uk/emailService/v1"
            CREATE_SHOPIFY_CUSTOMER = "https://ai-ccaasapi.worktual.co.uk/connector/shopify/v1/customer"
            SHOPIFY_CONNECTION_LIST = 'https://ccaasapi.worktual.co.uk/connector/shopify/v1/company';
            SHOPIFY_TOKEN_GENERATOR = 'https://ccaasapi.worktual.co.uk/connector/shopify/v1/productTitle'
            SHOPIFY_CHECKOUT = 'https://ccaasapi.worktual.co.uk/connector/shopify/v1/checkout';
            MAGENTO_TOKEN_GENERATOR = 'https://ai-ccaasapi.worktual.co.uk/connector/magento/v1/shopList';
            MAGENTO_PRODUCT_SEARCH = 'https://ai-ccaasapi.worktual.co.uk/connector/magento/v1/productsTitle';
            MAGENTO_REFRESH_TOKEN = 'https://ai-ccaasapi.worktual.co.uk/connector/magento/v1/refresh_token';
            BIGCOMMERCE_SEARCH_LIST = 'https://ai-ccaasapi.worktual.co.uk/connector/bigcommerce/v1/catalog/productTitle'
            BIGCOMMERCE_TOKEN_GENERATOR = 'https://ai-ccaasapi.worktual.co.uk/connector/bigcommerce/v1/shopList'
            SHOPIFY_PRODUCT_SEARCH = 'https://ai-ccaasapi.worktual.co.uk/connector/shopify/v1/product_search'
            WORKTUAL_APPOINTMENT = "https://bookingapi.worktual.co.uk/booking_service/v1"
            BIGCOMMERCE_GET_VARIANTS = 'https://ai-ccaasapi.worktual.co.uk/connector/bigcommerce/v1/catalog/products'
            WIX_TOKEN_GENERATOR = 'https://ai-ccaasapi.worktual.co.uk/connector/wix/v1/shopList';
            WIX_PRODUCT_SEARCH = 'https://ai-ccaasapi.worktual.co.uk/connector/wix/v1/product'
            WIX_EXISTING_CUSTOMER = 'https://ai-ccaasapi.worktual.co.uk/connector/wix/v1/contactEmail';
            WIX_CREATE_CUSTOMER = 'https://ai-ccaasapi.worktual.co.uk/connector/wix/v1/contact';
            WIX_ORDER_CREATION = 'https://ai-ccaasapi.worktual.co.uk/connector/wix/v1/order';
            BIGCOMMERCE_EXISTING_CUSTOMER = 'https://ai-ccaasapi.worktual.co.uk/connector/bigcommerce/v1/customerEmail'
            BIGCOMMERCE_CREATE_CUSTOMER = 'https://ai-ccaasapi.worktual.co.uk/connector/bigcommerce/v1/customer'
            BIGCOMMERCE_CREATE_ORDER = 'https://ai-ccaasapi.worktual.co.uk/connector/bigcommerce/v1/order';
            ORDER_TOKEN_GENERATOR = 'https://ai-ccaasapi.worktual.co.uk/connector/bigcommerce/v1/payment/accessToken'
            BIG_COMMERCE_PAYMENT = 'https://ai-ccaasapi.worktual.co.uk/connector/bigcommerce/v1/processPayment'
            MAGENTO_EXISTING_CUSTOMER = 'https://ai-ccaasapi.worktual.co.uk/connector/magento/v1/customerSearch'
            MAGENTO_CREATE_CUSTOMER = 'https://ai-ccaasapi.worktual.co.uk/connector/magento/v1/customer'
            MAGENTO_CREATE_ORDER = 'https://ai-ccaasapi.worktual.co.uk/connector/magento/v1/order'
            WIX_PRODUCT_SEARCH_ID = 'https://ai-ccaasapi.worktual.co.uk/connector/wix/v1/product_search'
            WIX_CONTACT_UPDATE = 'https://ai-ccaasapi.worktual.co.uk/connector/wix/v1/contact'
            WIX_REFRESH_TOKEN_GENERATOR = 'https://ccaasapi.worktual.co.uk/connector/wix/v1/refresh_token'
            WOOCOMMERCE_TOKEN_GENERATOR = 'https://ai-ccaasapi.worktual.co.uk/connector/woocommerce/v1/shopList';
            WOOCOMMERCE_PRODUCT_SEARCH = 'https://ai-ccaasapi.worktual.co.uk/connector/woocommerce/v1/productsTitle'
            WOOCOMMERCE_GET_VARIANT = 'https://ai-ccaasapi.worktual.co.uk/connector/woocommerce/v1/products'
            WOOCOMMERCE_EXISTING_CUSTOMER = 'https://ai-ccaasapi.worktual.co.uk/connector/woocommerce/v1/customerEmail'
            WOOCOMMERECE_CREATE_CUSTOMER = 'https://ai-ccaasapi.worktual.co.uk/connector/woocommerce/v1/customer'
            // ECOMMERCE_IMAGE_SEARCH = 'http://46.43.144.145:7050/image_search'
            ECOMMERCE_IMAGE_SEARCH = 'https://scraping.worktual.co.uk/image_search';
            WOOCOMMERECE_UPDATE_CUSTOMER = 'https://ai-ccaasapi.worktual.co.uk/connector/woocommerce/v1/customer'
            WOOCOMMERCE_ORDER_CREATION = 'https://ai-ccaasapi.worktual.co.uk/connector/woocommerce/v1/order'
            WOOCOMMERCE_PRODUCT_SEARCH_ID = 'https://ai-ccaasapi.worktual.co.uk/connector/woocommerce/v1/productSearchByIds'
            WOOCOMMERCE_OFFER_SEARCH = 'https://ai-ccaasapi.worktual.co.uk/connector/woocommerce/v1/products/sale'
            CARE_HOME = "https://carehomeapi.audracare.co.uk/homecare_service/v1"
            CHATBOT_PAYMENT_SERVICE = "https://chatpayment.worktual.co.uk"
            B2C_PRODUCT_SEARCH = "https://b2c.worktual.co.uk/ws_b2c"
            MYACCOUNT = 'https://mysqlapi.worktual.co.uk/myaccount-mysql/v1/worktual'
            NLP_CONVO_SUMMARY="https://nlpccaas.worktual.co.uk/summarize_convo"
            CONNECTOR_COMMON_LIST="https://ccaasapi.worktual.co.uk/connector/common/v1/list";
            CONNECTOR_ZOHO_REFRESH="https://ccaasapi.worktual.co.uk/connector/zoho/v1/refresh_token";
            CONNECTOR_ZOHO_LEADS= "https://ccaasapi.worktual.co.uk/connector/zoho/v1/leads";
            SHOPIFY_PRODUCT_RECOMMENDATION= "https://nlp.worktual.co.uk/get_product_recommendation";

      } else if (process.env.REACT_APP_ENV === "qa") {
            BASE_URL = "https://qaai-ccaasapi.worktual.co.uk";
            CORESERVICE_URL = "https://qaai-ccaasapi.worktual.co.uk/core_service/v1";
            REPORTSERVICE_URL = 'https://qaai-ccaasapi.worktual.co.uk/reports_service/v1';
            CALLSERVICE_URL = "https://qaai-ccaasapi.worktual.co.uk/callservice/v1";
            WORKTUALBOT = "https://testvideo-meet.worktual.co.uk/qaworktualchatbot";
            OTHERBOT = "https://testvideo-meet.worktual.co.uk/vectonechatbot";
            TEMPLATEBOT = "https://testvideo-meet.worktual.co.uk/ask";
            MERCHANT_ID = "vectone"; // vectone  Unitedfone
            SECRET_KEY = "KGCWbMuLRd"; // KGCWbMuLRd  Tprbt9cpvb
            PAYMENT_URL = "https://pay.sandbox.realexpayments.com/pay";
            RETURN_URL = "https://liveurapi.worktual.co.uk/vectone-myaccount/v1/user/test_callback";
            ACCOUNT = "uk"; // uk internet
            CONNECTOR_URL = "https://qaai-ccaasapi.worktual.co.uk/connector/calendar/v1"
            EMAIL_URL = "https://qaai-ccaasapi.worktual.co.uk/emailService/v1"
            CREATE_SHOPIFY_CUSTOMER = "https://qaai-ccaasapi.worktual.co.uk/connector/shopify/v1/customer";
            SHOPIFY_CONNECTION_LIST = 'https://qaai-ccaasapi.worktual.co.uk/connector/shopify/v1/company';
            SHOPIFY_TOKEN_GENERATOR = 'https://qaai-ccaasapi.worktual.co.uk/connector/shopify/v1/productTitle'
            SHOPIFY_CHECKOUT = 'https://qaai-ccaasapi.worktual.co.uk/connector/shopify/v1/checkout'
            MAGENTO_TOKEN_GENERATOR = 'https://qaai-ccaasapi.worktual.co.uk/connector/magento/v1/shopList';
            MAGENTO_PRODUCT_SEARCH = 'https://qaai-ccaasapi.worktual.co.uk/connector/magento/v1/productsTitle';
            MAGENTO_REFRESH_TOKEN = 'https://qaai-ccaasapi.worktual.co.uk/connector/magento/v1/refresh_token';
            BIGCOMMERCE_TOKEN_GENERATOR = 'https://ccaasapi.worktual.co.uk/connector/bigcommerce/v1/shopList'
            SHOPIFY_PRODUCT_SEARCH = 'https://ccaasapi.worktual.co.uk/connector/shopify/v1/product_search'
            WORKTUAL_APPOINTMENT = "https://qabookingapi.worktual.co.uk/booking_service/v1"
            BIGCOMMERCE_SEARCH_LIST = 'https://qaai-ccaasapi.worktual.co.uk/connector/bigcommerce/v1/catalog/productTitle'
            BIGCOMMERCE_GET_VARIANTS = 'https://qaai-ccaasapi.worktual.co.uk/connector/bigcommerce/v1/catalog/products'
            WIX_TOKEN_GENERATOR = 'https://qaai-ccaasapi.worktual.co.uk/connector/wix/v1/shopList'
            WIX_PRODUCT_SEARCH = 'https://qaai-ccaasapi.worktual.co.uk/connector/wix/v1/product';
            WIX_EXISTING_CUSTOMER = 'https://qaai-ccaasapi.worktual.co.uk/connector/wix/v1/contactEmail';
            WIX_CREATE_CUSTOMER = 'https://qaai-ccaasapi.worktual.co.uk/connector/wix/v1/contact';
            WIX_ORDER_CREATION = 'https://qaai-ccaasapi.worktual.co.uk/connector/wix/v1/order';
            BIGCOMMERCE_EXISTING_CUSTOMER = 'https://qaai-ccaasapi.worktual.co.uk/connector/bigcommerce/v1/customerEmail'
            BIGCOMMERCE_CREATE_CUSTOMER = 'https://qaai-ccaasapi.worktual.co.uk/connector/bigcommerce/v1/customer'
            BIGCOMMERCE_CREATE_ORDER = 'https://qaai-ccaasapi.worktual.co.uk/connector/bigcommerce/v1/order';
            ORDER_TOKEN_GENERATOR = 'https://qaai-ccaasapi.worktual.co.uk/connector/bigcommerce/v1/payment/accessToken'
            BIG_COMMERCE_PAYMENT = 'https://qaai-ccaasapi.worktual.co.uk/connector/bigcommerce/v1/processPayment'
            MAGENTO_EXISTING_CUSTOMER = 'https://qaai-ccaasapi.worktual.co.uk/connector/magento/v1/customerSearch'
            MAGENTO_CREATE_CUSTOMER = 'https://qaai-ccaasapi.worktual.co.uk/connector/magento/v1/customer'
            MAGENTO_CREATE_ORDER = 'https://qaai-ccaasapi.worktual.co.uk/connector/magento/v1/order'
            WIX_PRODUCT_SEARCH_ID = 'https://qaai-ccaasapi.worktual.co.uk/connector/wix/v1/product_search'
            WIX_CONTACT_UPDATE = 'https://qaai-ccaasapi.worktual.co.uk/connector/wix/v1/contact'
            WIX_REFRESH_TOKEN_GENERATOR = 'https://qaai-ccaasapi.worktual.co.uk/connector/wix/v1/refresh_token'
            WOOCOMMERCE_TOKEN_GENERATOR = 'https://qaai-ccaasapi.worktual.co.uk/connector/woocommerce/v1/shopList';
            WOOCOMMERCE_PRODUCT_SEARCH = 'https://qaai-ccaasapi.worktual.co.uk/connector/woocommerce/v1/productsTitle'
            WOOCOMMERCE_GET_VARIANT = 'https://qaai-ccaasapi.worktual.co.uk/connector/woocommerce/v1/products';
            WOOCOMMERCE_EXISTING_CUSTOMER = 'https://qaai-ccaasapi.worktual.co.uk/connector/woocommerce/v1/customerEmail'
            WOOCOMMERECE_CREATE_CUSTOMER = 'https://qaai-ccaasapi.worktual.co.uk/connector/woocommerce/v1/customer'
            ECOMMERCE_IMAGE_SEARCH = 'http://46.43.144.145:7050/image_search';
            ECOMMERCE_IMAGE_SEARCH = 'https://scraping.worktual.co.uk/image_search';
            WOOCOMMERECE_UPDATE_CUSTOMER = 'https://qaai-ccaasapi.worktual.co.uk/connector/woocommerce/v1/customer'
            WOOCOMMERCE_ORDER_CREATION = 'https://qaai-ccaasapi.worktual.co.uk/connector/woocommerce/v1/order'
            WOOCOMMERCE_PRODUCT_SEARCH_ID = 'https://ccaasapi.worktual.co.uk/connector/woocommerce/v1/productSearchByIds'
            WOOCOMMERCE_OFFER_SEARCH = 'https://qaai-ccaasapi.worktual.co.uk/connector/woocommerce/v1/products/sale'
            CARE_HOME = "https://carehomeapi.audracare.co.uk/homecare_service/v1"
            CHATBOT_PAYMENT_SERVICE = "https://qachatpayment.worktual.co.uk"
            B2C_PRODUCT_SEARCH = "https://b2c.worktual.co.uk/ws_b2c"
            MYACCOUNT = 'https://qamysqlapi.worktual.co.uk/myaccount-mysql/v1/worktual'
            NLP_CONVO_SUMMARY="https://nlpccaas.worktual.co.uk/summarize_convo"
            CONNECTOR_COMMON_LIST="https://qaccaasapi.worktual.co.uk/connector/common/v1/list";
            CONNECTOR_ZOHO_REFRESH="https://qaccaasapi.worktual.co.uk/connector/zoho/v1/refresh_token";
            CONNECTOR_ZOHO_LEADS= "https://qaccaasapi.worktual.co.uk/connector/zoho/v1/leads";
            SHOPIFY_PRODUCT_RECOMMENDATION= "https://nlp.worktual.co.uk/get_product_recommendation";
            WIX_CHECKOUT = "https://qaccaasapi.worktual.co.uk/connector/wix/v1/checkout";

      }else if (process.env.REACT_APP_ENV === "uat") {
            BASE_URL = "https://uatccaasapi.worktual.co.uk";
            CORESERVICE_URL = "https://uatccaasapi.worktual.co.uk/core_service/v1";
            REPORTSERVICE_URL = 'https://uatccaasapi.worktual.co.uk/reports_service/v1';
            CALLSERVICE_URL = "https://uatccaasapi.worktual.co.uk/callservice/v1";
            WORKTUALBOT = "https://testvideo-meet.worktual.co.uk/qaworktualchatbot";
            OTHERBOT = "https://testvideo-meet.worktual.co.uk/vectonechatbot";
            TEMPLATEBOT = "https://testvideo-meet.worktual.co.uk/ask";
            MERCHANT_ID = "vectone"; // vectone  Unitedfone
            SECRET_KEY = "KGCWbMuLRd"; // KGCWbMuLRd  Tprbt9cpvb
            PAYMENT_URL = "https://pay.sandbox.realexpayments.com/pay";
            RETURN_URL = "https://liveurapi.worktual.co.uk/vectone-myaccount/v1/user/test_callback";
            ACCOUNT = "uk"; // uk internet
            CONNECTOR_URL = "https://uatccaasapi.worktual.co.uk/connector/calendar/v1"
            EMAIL_URL = "https://uatccaasapi.worktual.co.uk/emailService/v1"
            CREATE_SHOPIFY_CUSTOMER = "https://uatccaasapi.worktual.co.uk/connector/shopify/v1/customer";
            SHOPIFY_CONNECTION_LIST = 'https://uatccaasapi.worktual.co.uk/connector/shopify/v1/company';
            SHOPIFY_TOKEN_GENERATOR = 'https://uatccaasapi.worktual.co.uk/connector/shopify/v1/productTitle'
            SHOPIFY_CHECKOUT = 'https://uatccaasapi.worktual.co.uk/connector/shopify/v1/checkout'
            MAGENTO_TOKEN_GENERATOR = 'https://uatccaasapi.worktual.co.uk/connector/magento/v1/shopList';
            MAGENTO_PRODUCT_SEARCH = 'https://uatccaasapi.worktual.co.uk/connector/magento/v1/productsTitle';
            MAGENTO_REFRESH_TOKEN = 'https://uatccaasapi.worktual.co.uk/connector/magento/v1/refresh_token';
            BIGCOMMERCE_TOKEN_GENERATOR = 'https://ccaasapi.worktual.co.uk/connector/bigcommerce/v1/shopList'
            SHOPIFY_PRODUCT_SEARCH = 'https://uatccaasapi.worktual.co.uk/connector/shopify/v1/product_search'
            WORKTUAL_APPOINTMENT = "https://uatccaasapi.worktual.co.uk/booking_service/v1"
            BIGCOMMERCE_SEARCH_LIST = 'https://uatccaasapi.worktual.co.uk/connector/bigcommerce/v1/catalog/productTitle'
            BIGCOMMERCE_GET_VARIANTS = 'https://uatccaasapi.worktual.co.uk/connector/bigcommerce/v1/catalog/products'
            WIX_TOKEN_GENERATOR = 'https://uatccaasapi.worktual.co.uk/connector/wix/v1/shopList'
            WIX_PRODUCT_SEARCH = 'https://uatccaasapi.worktual.co.uk/connector/wix/v1/product';
            WIX_EXISTING_CUSTOMER = 'https://uatccaasapi.worktual.co.uk/connector/wix/v1/contactEmail';
            WIX_CREATE_CUSTOMER = 'https://uatccaasapi.worktual.co.uk/connector/wix/v1/contact';
            WIX_ORDER_CREATION = 'https://uatccaasapi.worktual.co.uk/connector/wix/v1/order';
            BIGCOMMERCE_EXISTING_CUSTOMER = 'https://uatccaasapi.worktual.co.uk/connector/bigcommerce/v1/customerEmail'
            BIGCOMMERCE_CREATE_CUSTOMER = 'https://uatccaasapi.worktual.co.uk/connector/bigcommerce/v1/customer'
            BIGCOMMERCE_CREATE_ORDER = 'https://uatccaasapi.worktual.co.uk/connector/bigcommerce/v1/order';
            ORDER_TOKEN_GENERATOR = 'https://uatccaasapi.worktual.co.uk/connector/bigcommerce/v1/payment/accessToken'
            BIG_COMMERCE_PAYMENT = 'https://uatccaasapi.worktual.co.uk/connector/bigcommerce/v1/processPayment'
            MAGENTO_EXISTING_CUSTOMER = 'https://uatccaasapi.worktual.co.uk/connector/magento/v1/customerSearch'
            MAGENTO_CREATE_CUSTOMER = 'https://uatccaasapi.worktual.co.uk/connector/magento/v1/customer'
            MAGENTO_CREATE_ORDER = 'https://uatccaasapi.worktual.co.uk/connector/magento/v1/order'
            WIX_PRODUCT_SEARCH_ID = 'https://uatccaasapi.worktual.co.uk/connector/wix/v1/product_search'
            WIX_CONTACT_UPDATE = 'https://uatccaasapi.worktual.co.uk/connector/wix/v1/contact'
            WIX_REFRESH_TOKEN_GENERATOR = 'https://uatccaasapi.worktual.co.uk/connector/wix/v1/refresh_token'
            WOOCOMMERCE_TOKEN_GENERATOR = 'https://uatccaasapi.worktual.co.uk/connector/woocommerce/v1/shopList';
            WOOCOMMERCE_PRODUCT_SEARCH = 'https://uatccaasapi.worktual.co.uk/connector/woocommerce/v1/productsTitle'
            WOOCOMMERCE_GET_VARIANT = 'https://uatccaasapi.worktual.co.uk/connector/woocommerce/v1/products';
            WOOCOMMERCE_EXISTING_CUSTOMER = 'https://uatccaasapi.worktual.co.uk/connector/woocommerce/v1/customerEmail'
            WOOCOMMERECE_CREATE_CUSTOMER = 'https://uatccaasapi.worktual.co.uk/connector/woocommerce/v1/customer'
            ECOMMERCE_IMAGE_SEARCH = 'http://46.43.144.145:7050/image_search';
            ECOMMERCE_IMAGE_SEARCH = 'https://scraping.worktual.co.uk/image_search';
            WOOCOMMERECE_UPDATE_CUSTOMER = 'https://uatccaasapi.worktual.co.uk/connector/woocommerce/v1/customer'
            WOOCOMMERCE_ORDER_CREATION = 'https://uatccaasapi.worktual.co.uk/connector/woocommerce/v1/order'
            WOOCOMMERCE_PRODUCT_SEARCH_ID = 'https://uatccaasapi.worktual.co.uk/connector/woocommerce/v1/productSearchByIds'
            WOOCOMMERCE_OFFER_SEARCH = 'https://uatccaasapi.worktual.co.uk/connector/woocommerce/v1/products/sale'
            CARE_HOME = "https://carehomeapi.audracare.co.uk/homecare_service/v1"
            CHATBOT_PAYMENT_SERVICE = "https://qachatpayment.worktual.co.uk"
            B2C_PRODUCT_SEARCH = "https://b2c.worktual.co.uk/ws_b2c"
            MYACCOUNT = 'https://qamysqlapi.worktual.co.uk/myaccount-mysql/v1/worktual'
            NLP_CONVO_SUMMARY="https://nlpccaas.worktual.co.uk/summarize_convo"
            SHOPIFY_PRODUCT_RECOMMENDATION= "https://nlp.worktual.co.uk/get_product_recommendation";

      }else if (process.env.REACT_APP_ENV === "qa_ccaas") {
            BASE_URL = "https://qaai-ccaasapi.worktual.co.uk";
            CORESERVICE_URL = "https://qaai-ccaasapi.worktual.co.uk/core_service/v1";
            REPORTSERVICE_URL = 'https://qaai-ccaasapi.worktual.co.uk/reports_service/v1';
            CALLSERVICE_URL = "https://qaai-ccaasapi.worktual.co.uk/callservice/v1";
            WORKTUALBOT = "https://testvideo-meet.worktual.co.uk/qaworktualchatbot";
            OTHERBOT = "https://testvideo-meet.worktual.co.uk/vectonechatbot";
            TEMPLATEBOT = "https://testvideo-meet.worktual.co.uk/ask";
            MERCHANT_ID = "vectone"; // vectone  Unitedfone
            SECRET_KEY = "KGCWbMuLRd"; // KGCWbMuLRd  Tprbt9cpvb
            PAYMENT_URL = "https://pay.sandbox.realexpayments.com/pay";
            RETURN_URL = "https://liveurapi.worktual.co.uk/vectone-myaccount/v1/user/test_callback";
            ACCOUNT = "uk"; // uk internet
            CONNECTOR_URL = "https://qaai-ccaasapi.worktual.co.uk/connector/calendar/v1"
            EMAIL_URL = "https://qaai-ccaasapi.worktual.co.uk/emailService/v1"
            CREATE_SHOPIFY_CUSTOMER = "https://qaai-ccaasapi.worktual.co.uk/connector/shopify/v1/customer";
            SHOPIFY_CONNECTION_LIST = 'https://qaai-ccaasapi.worktual.co.uk/connector/shopify/v1/company';
            SHOPIFY_TOKEN_GENERATOR = 'https://qaai-ccaasapi.worktual.co.uk/connector/shopify/v1/productTitle'
            SHOPIFY_CHECKOUT = 'https://qaai-ccaasapi.worktual.co.uk/connector/shopify/v1/checkout'
            MAGENTO_TOKEN_GENERATOR = 'https://qaai-ccaasapi.worktual.co.uk/connector/magento/v1/shopList';
            MAGENTO_PRODUCT_SEARCH = 'https://qaai-ccaasapi.worktual.co.uk/connector/magento/v1/productsTitle';
            MAGENTO_REFRESH_TOKEN = 'https://qaai-ccaasapi.worktual.co.uk/connector/magento/v1/refresh_token';
            BIGCOMMERCE_TOKEN_GENERATOR = 'https://ccaasapi.worktual.co.uk/connector/bigcommerce/v1/shopList'
            SHOPIFY_PRODUCT_SEARCH = 'https://ccaasapi.worktual.co.uk/connector/shopify/v1/product_search'
            WORKTUAL_APPOINTMENT = "https://qabookingapi.worktual.co.uk/booking_service/v1"
            BIGCOMMERCE_SEARCH_LIST = 'https://qaai-ccaasapi.worktual.co.uk/connector/bigcommerce/v1/catalog/productTitle'
            BIGCOMMERCE_GET_VARIANTS = 'https://qaai-ccaasapi.worktual.co.uk/connector/bigcommerce/v1/catalog/products'
            WIX_TOKEN_GENERATOR = 'https://qaai-ccaasapi.worktual.co.uk/connector/wix/v1/shopList'
            WIX_PRODUCT_SEARCH = 'https://qaai-ccaasapi.worktual.co.uk/connector/wix/v1/product';
            WIX_EXISTING_CUSTOMER = 'https://qaai-ccaasapi.worktual.co.uk/connector/wix/v1/contactEmail';
            WIX_CREATE_CUSTOMER = 'https://qaai-ccaasapi.worktual.co.uk/connector/wix/v1/contact';
            WIX_ORDER_CREATION = 'https://qaai-ccaasapi.worktual.co.uk/connector/wix/v1/order';
            BIGCOMMERCE_EXISTING_CUSTOMER = 'https://qaai-ccaasapi.worktual.co.uk/connector/bigcommerce/v1/customerEmail'
            BIGCOMMERCE_CREATE_CUSTOMER = 'https://qaai-ccaasapi.worktual.co.uk/connector/bigcommerce/v1/customer'
            BIGCOMMERCE_CREATE_ORDER = 'https://qaai-ccaasapi.worktual.co.uk/connector/bigcommerce/v1/order';
            ORDER_TOKEN_GENERATOR = 'https://qaai-ccaasapi.worktual.co.uk/connector/bigcommerce/v1/payment/accessToken'
            BIG_COMMERCE_PAYMENT = 'https://qaai-ccaasapi.worktual.co.uk/connector/bigcommerce/v1/processPayment'
            MAGENTO_EXISTING_CUSTOMER = 'https://qaai-ccaasapi.worktual.co.uk/connector/magento/v1/customerSearch'
            MAGENTO_CREATE_CUSTOMER = 'https://qaai-ccaasapi.worktual.co.uk/connector/magento/v1/customer'
            MAGENTO_CREATE_ORDER = 'https://qaai-ccaasapi.worktual.co.uk/connector/magento/v1/order'
            WIX_PRODUCT_SEARCH_ID = 'https://qaai-ccaasapi.worktual.co.uk/connector/wix/v1/product_search'
            WIX_CONTACT_UPDATE = 'https://qaai-ccaasapi.worktual.co.uk/connector/wix/v1/contact'
            WIX_REFRESH_TOKEN_GENERATOR = 'https://qaai-ccaasapi.worktual.co.uk/connector/wix/v1/refresh_token'
            WOOCOMMERCE_TOKEN_GENERATOR = 'https://qaai-ccaasapi.worktual.co.uk/connector/woocommerce/v1/shopList';
            WOOCOMMERCE_PRODUCT_SEARCH = 'https://qaai-ccaasapi.worktual.co.uk/connector/woocommerce/v1/productsTitle'
            WOOCOMMERCE_GET_VARIANT = 'https://qaai-ccaasapi.worktual.co.uk/connector/woocommerce/v1/products';
            WOOCOMMERCE_EXISTING_CUSTOMER = 'https://qaai-ccaasapi.worktual.co.uk/connector/woocommerce/v1/customerEmail'
            WOOCOMMERECE_CREATE_CUSTOMER = 'https://qaai-ccaasapi.worktual.co.uk/connector/woocommerce/v1/customer'
            ECOMMERCE_IMAGE_SEARCH = 'http://46.43.144.145:7050/image_search';
            ECOMMERCE_IMAGE_SEARCH = 'https://scraping.worktual.co.uk/image_search';
            WOOCOMMERECE_UPDATE_CUSTOMER = 'https://qaai-ccaasapi.worktual.co.uk/connector/woocommerce/v1/customer'
            WOOCOMMERCE_ORDER_CREATION = 'https://qaai-ccaasapi.worktual.co.uk/connector/woocommerce/v1/order'
            WOOCOMMERCE_PRODUCT_SEARCH_ID = 'https://ccaasapi.worktual.co.uk/connector/woocommerce/v1/productSearchByIds'
            WOOCOMMERCE_OFFER_SEARCH = 'https://qaai-ccaasapi.worktual.co.uk/connector/woocommerce/v1/products/sale'
            CARE_HOME = "https://carehomeapi.audracare.co.uk/homecare_service/v1"
            CHATBOT_PAYMENT_SERVICE = "https://qachatpayment.worktual.co.uk"
            B2C_PRODUCT_SEARCH = "https://b2c.worktual.co.uk/ws_b2c"
            MYACCOUNT = 'https://qamysqlapi.worktual.co.uk/myaccount-mysql/v1/worktual'
            NLP_CONVO_SUMMARY="https://nlpccaas.worktual.co.uk/summarize_convo"
            SHOPIFY_PRODUCT_RECOMMENDATION= "https://nlp.worktual.co.uk/get_product_recommendation";

      } else {
            BASE_URL = "https://ccaasapi.worktual.co.uk";
            REPORTSERVICE_URL = 'https://ccaasapi.worktual.co.uk/reports_service/v1';
            CORESERVICE_URL = "https://ccaasapi.worktual.co.uk/core_service/v1";
            CALLSERVICE_URL = "http://localhost:5007/callservice/v1";
            WORKTUALBOT = "http://10.150.3.150:8088/chatbot";
            OTHERBOT = "http://10.150.3.150:8088/vectonechatbot";
            TEMPLATEBOT = "https://testvideo-meet.worktual.co.uk/ask";
            MERCHANT_ID = "Unitedfone"; // vectone  Unitedfone
            SECRET_KEY = "Tprbt9cpvb"; // KGCWbMuLRd  Tprbt9cpvb
            PAYMENT_URL = "https://pay.realexpayments.com/pay";
            RETURN_URL = "https://liveurapi.worktual.co.uk/vectone-myaccount/v1/user/test_callback";
            ACCOUNT = "internet"; // uk internet
            CONNECTOR_URL = "https://qaccaasapi.worktual.co.uk/connector/calendar/v1"
            EMAIL_URL = "https://qaccaasapi.worktual.co.uk/emailService/v1"
            CARE_HOME = "https://carehomeapi.audracare.co.uk/homecare_service/v1"
            CHATBOT_PAYMENT_SERVICE = "https://chatpayment.worktual.co.uk"
            B2C_PRODUCT_SEARCH = "https://b2c.worktual.co.uk/ws_b2c"
            MYACCOUNT = 'https://qamysqlapi.worktual.co.uk/myaccount-mysql/v1/worktual'
            NLP_CONVO_SUMMARY="https://nlpccaas.worktual.co.uk/summarize_convo"
            CONNECTOR_COMMON_LIST="https://qaccaasapi.worktual.co.uk/connector/common/v1/list"
            CONNECTOR_ZOHO_REFRESH="https://qaccaasapi.worktual.co.uk/connector/zoho/v1/refresh_token";
            CONNECTOR_ZOHO_LEADS= "https://qaccaasapi.worktual.co.uk/connector/zoho/v1/leads";
            SHOPIFY_PRODUCT_RECOMMENDATION= "https://nlp.worktual.co.uk/get_product_recommendation";

      }

      return {
            CORESERVICE_URL,
            CALLSERVICE_URL,
            BASE_URL,
            WORKTUALBOT,
            OTHERBOT,
            TEMPLATEBOT,
            MERCHANT_ID,
            SECRET_KEY,
            PAYMENT_URL,
            RETURN_URL,
            ACCOUNT,
            CONNECTOR_URL,
            EMAIL_URL,
            CREATE_SHOPIFY_CUSTOMER,
            SHOPIFY_CONNECTION_LIST,
            SHOPIFY_TOKEN_GENERATOR,
            SHOPIFY_CHECKOUT,
            SHOPIFY_PRODUCT_SEARCH,
            WORKTUAL_APPOINTMENT,
            MAGENTO_TOKEN_GENERATOR,
            MAGENTO_PRODUCT_SEARCH,
            MAGENTO_REFRESH_TOKEN,
            BIGCOMMERCE_SEARCH_LIST,
            BIGCOMMERCE_TOKEN_GENERATOR,
            SHOPIFY_PRODUCT_SEARCH,
            BIGCOMMERCE_GET_VARIANTS,
            WIX_TOKEN_GENERATOR,
            WIX_PRODUCT_SEARCH,
            WIX_EXISTING_CUSTOMER,
            WIX_CREATE_CUSTOMER,
            WIX_ORDER_CREATION,
            BIGCOMMERCE_EXISTING_CUSTOMER,
            BIGCOMMERCE_CREATE_CUSTOMER,
            BIGCOMMERCE_CREATE_ORDER,
            ORDER_TOKEN_GENERATOR,
            BIG_COMMERCE_PAYMENT,
            MAGENTO_EXISTING_CUSTOMER,
            MAGENTO_CREATE_CUSTOMER,
            MAGENTO_CREATE_ORDER,
            WIX_PRODUCT_SEARCH_ID,
            WIX_CONTACT_UPDATE,
            WIX_REFRESH_TOKEN_GENERATOR,
            WOOCOMMERCE_TOKEN_GENERATOR,
            WOOCOMMERCE_PRODUCT_SEARCH,
            WOOCOMMERCE_GET_VARIANT,
            WOOCOMMERCE_EXISTING_CUSTOMER,
            ECOMMERCE_IMAGE_SEARCH,
            WOOCOMMERECE_CREATE_CUSTOMER,
            WOOCOMMERECE_UPDATE_CUSTOMER,
            WOOCOMMERCE_ORDER_CREATION,
            WOOCOMMERCE_PRODUCT_SEARCH_ID,
            WOOCOMMERCE_OFFER_SEARCH,
            CARE_HOME,
            REPORTSERVICE_URL,
            CHATBOT_PAYMENT_SERVICE,
            B2C_PRODUCT_SEARCH,
            MYACCOUNT,
            NLP_CONVO_SUMMARY,
            CONNECTOR_COMMON_LIST,
            CONNECTOR_ZOHO_REFRESH,
            CONNECTOR_ZOHO_LEADS,
            SHOPIFY_PRODUCT_RECOMMENDATION,
            WIX_CHECKOUT
      };
};

export const ponpureDomain = "20214";
export const ponpureDomainAI = "20598";
export const worktulDomain = "11165";
export const pothysDomain = "20556";
export const nickysDomainAI = "20630";
export const welcare = "20667";
export const grtDomain = "20668";
export const adbricks = "20643";
export const salesDemo = "20690";

export const QA_TAFI = "7851";
export const convosupportedDomain = [ welcare, adbricks, grtDomain, QA_TAFI, salesDemo ];