import store from "../../store/redux-store/root_store";
import { ShopingSiceActions } from "../../store/slice/shoping_slice";
import { useEffect, useState } from "react";
import { config } from '../../base/constants'

const expired = 'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpbnN0YW5jZUlkIjoiOTdiMDM1OTUtMjQzNy00YjZmLThjZWMtNzlhZWY4MWZmNTQyIiwic2l0ZU5hbWUiOiJOaWNreSdzIiwiaWF0IjoxNzY1MjkwOTY3fQ.fGbAT99BM2YP0EraQxaQIJfUULj8eBRIMJ60Iut0TZg'

export const getPerticularData = async (id, dispatch, rauthToken = null) => {
  try {
    const url = `https://ccaasapi.worktual.co.uk/connector/wix/v1/particularProduct/${id}`;

    let authToken = store.getState().shopingStore?.authToken || rauthToken;
    let message = store.getState().message;

    if (!message.personalisationAll.domainId) return;

    if (!authToken) {
      authToken = await getAuthToken(19568);
      dispatch(ShopingSiceActions.setAuthToken(authToken));
    }

    let response = await fetch(url, {
      method: 'GET',
      headers: {
        'Content-Type': 'application/json',
        'Authorization': authToken,
      }
    });

    response = await response.json()
    if (response?.statusCode === 401) {
      let rauthToken = await getRefreshToken(19568, authToken)
      dispatch(ShopingSiceActions.setAuthToken(authToken));
      const r = await getPerticularData(id, dispatch, rauthToken)
      return r
    } else if (response?.statusCode === 200) {
      const res = response?.result?.product ?? {}
      return res
    }
    return {}
  } catch (error) {
    console.log(error)
    return {}
  }
};

export const getAuthToken = async (id) => {
  try {
    const url = `https://ccaasapi.worktual.co.uk/connector/wix/v1/shopList/${id}`

    let response = await fetch(url, {
      method: 'get',
      headers: {
        'Content-Type': 'application/json',
      }
    });

    response = await response.json()
    if (response.statusCode === 200 && response?.result?.length > 0) {
      const res = response?.result[0]?.token
      return res
    }
    return null
  } catch (error) {
    console.log(error)
    return null
  }
}

export const getRefreshToken = async (id, authToken) => {
  try {

    const url = `${config().WIX_REFRESH_TOKEN_GENERATOR}/${id}`;

    let response = await fetch(url, {
      method: 'get',
      headers: {
        'Content-Type': 'application/json',
        Authorization: authToken,
      }
    });

    const tokenresponse = await response.json()
    console.log("getRefreshToken ==", tokenresponse)
    if (tokenresponse.statusCode === 200) {
      console.log("getRefreshToken == 2", tokenresponse?.results?.token)
      const res = tokenresponse?.result?.token || tokenresponse?.results?.token;
      return res
    }

    return null;

  } catch (error) {
    console.log(error)
    return null
  }
}

export function getOptimizedWixImage(originalUrl, options = {}) {
  try {

    if (!originalUrl || typeof originalUrl !== "string") {
      return originalUrl;
    }

    if (!originalUrl.includes("static.wixstatic.com/media/")) {
      return originalUrl;
    }

    const { width = 1000, height = 1070, quality = 85 } = options;
    const baseImageName = originalUrl.split("/").pop();

    const optimizedUrl =
      originalUrl.split("/media/")[0] +
      "/media/" +
      originalUrl
        .split("/media/")[1]
        .split("/v1/")[0] +
      `/v1/fill/w_${width},h_${height},al_c,q_${quality},usm_0.66_1.00_0.01,enc_avif,quality_auto/` +
      baseImageName;

    return new Promise((resolve) => {
      try {
        const img = new Image();

        img.onload = function () {
          resolve(optimizedUrl);
        };

        img.onerror = function () {
          resolve(originalUrl);
        };

        img.src = optimizedUrl;
      } catch (error) {
        resolve(originalUrl);
      }
    });
  } catch (error) {
    return Promise.resolve(originalUrl);
  }
}


export const WixOptimizedImage = ({ src, className = "", alt = "", styles = {} }) => {
  const [finalSrc, setFinalSrc] = useState(src);

  useEffect(() => {
    let isMounted = true;

    try {
      getOptimizedWixImage(src, { width: 1000, height: 1070, quality: 85 })
        .then((url) => {
          if (isMounted) setFinalSrc(url);
        })
        .catch(() => {
          if (isMounted) setFinalSrc(src);
        });
    } catch (error) {
      setFinalSrc(src);
    }

    return () => {
      isMounted = false;
    };
  }, [src]);

  return <img src={finalSrc} className={className} alt={alt} styles={styles} />;
};

export const wixCheckoutPage = async (cartData, dispatch) => {
  try {
    let authToken = store.getState().shopingStore?.authToken;;
    if (!authToken) {
      authToken = await getAuthToken(19568);
      dispatch(ShopingSiceActions.setAuthToken(authToken));
    }

    const payload = {
      lineItems: buildWixLineItems(cartData),
    };

    const doRequest = async (token) => {
      const res = await fetch(config().WIX_CHECKOUT, {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
          Authorization: token,
        },
        body: JSON.stringify(payload),
      });

      const response = await res.json();
      console.log("doRequest res ===>>>", response)

      return response
    };

    let result = await doRequest(authToken);
    console.log("doRequest ---- 1", result)

    if (result.statusCode === 401) {

      const newToken = await getRefreshToken(19568, authToken);

      console.log("doRequest ---- getRefreshToken", newToken);
      
      dispatch(ShopingSiceActions.setAuthToken(newToken));

      result = await doRequest(newToken);
      console.log("doRequest ---- 2", result);

      if (result.statusCode === 401) {
        console.error('doRequest failed attempt 2', result)
      }
    }

    console.log("doRequest ---- <<<<doRequest>>>> ", result)

    if (result?.statusCode === 200 && result?.result?.checkout?.id) {
      const checkoutid = result?.result?.checkout?.id;
      const res = await getCheckoutURL(checkoutid, authToken)

      console.log("res")
      return res
    }

    return { statusCode: 422 }

  } catch (error) {
    console.error("wixCheckoutPage error", error);
    throw error;
  }
};

const getCheckoutURL = async (checkoutid, token) => {
  try {
    const res = await fetch(`${config().WIX_CHECKOUT}/${checkoutid}`, {
      method: "GET",
      headers: {
        "Content-Type": "application/json",
        Authorization: token,
      },
    });
    const data = await res.json();
    console.log("getCheckoutURL>>>>>", data);

    return data
  } catch (error) {
    return { statusCode: 422 }
  }
}

function buildWixLineItems(cartItems = []) {
  const WIX_STORES_APP_ID = "1380b703-ce81-ff05-f115-39571d94dfcd";
  return cartItems
    .filter((item) => Number(item?.quantity) > 0)
    .map((item) => {

      const { product_id, variant_id, quantity, message, messageTitle } = item;
      const catalogReference = {
        appId: WIX_STORES_APP_ID,
        catalogItemId: product_id,
      };

      const options = {};
      if (variant_id) {
        options.variantId = variant_id;
      }
      if (message && message.trim().length > 0) {
        options.customTextFields = {
          [messageTitle]: message?.trim(),
        };
      }

      if (Object.keys(options)?.length > 0) {
        catalogReference.options = options;
      }

      return {
        quantity: String(quantity),
        catalogReference,
      };
    });
}

export const stripHtml = (html) => {
  try {
    const div = document.createElement("div");
    div.innerHTML = html;
    return div.textContent || div.innerText || "";
  } catch (err) {
    return "";
  }
};