import React, { useEffect, useState } from 'react';
import styles from '../../../../assets/styles/usermanagement.module.scss';
// import { GetItemFromStorage } from '../../../../../../../custom_hooks/useStorage';
// import { LocalStorage } from '../../../../../../../custom_hooks/localStorageKeys';
import { getRolesListInfo } from '../../../../apiservice/myplanapi';
import { GetItemFromStorage } from '../../../../../base/customhooks/useStorage';
import { LocalStorage } from '../../../../../base/customhooks/localStorageKeys';
import { jwtDecode } from 'jwt-decode';
import { useNavigate } from 'react-router-dom';
import { assets } from '../../../../../styles/assets/assets';
const Createrole = () => {
        const userDetails :any= jwtDecode(GetItemFromStorage(LocalStorage.ACCESS_TOKEN));
        console.log("userDetails????", userDetails)
        const [profileData, setProfileData] = useState<any>([]);
        console.log("profileDataprofileData", profileData)
        const domainAllowed :any={
          "20667":[3]
        }
        
  const navigate = useNavigate();
  const gotoParticularRole = (data: any) => {
    navigate(`/roles/useradminroles?roleId=${data?.RoleId}`);
  }
           useEffect(() => {
            try {
              
              if (userDetails) {
                  const body = {
                      company_id: userDetails?.company_id,
                      RoleCategoryId:1,
                      product_id:userDetails?.product_id
                    }
      
                    getRolesListInfo(body)
                .then((res: any) => {
                  console.log("API RESPONSE ---->", res);
                  setProfileData(res);  // store in state
                })
                .catch((err:any) => console.log(err));
              }
              return () => {
                console.log("cleanup");
              };
            } catch (error) {
              console.log("err:",error)
            }
          }, []);
    return (
        <>
            <div className={styles.createmaincontainer}>
                <div className={styles.headerole}>
                    <h3>Roles</h3>
                    <p>Create and Manage roles for the Worktual products</p>
                </div>
          {profileData?.map((item: any) => (
            <>
              {domainAllowed?.[`${userDetails?.domainId}`]?.length > 0 ? (
                <>
                  {domainAllowed?.[`${userDetails?.domainId}`]?.includes(item?.RoleId) && (
                    <div
                      className={styles.createroleblock}
                      key={item.RoleId}
                      onClick={() => gotoParticularRole(item)}
                    >
                      <div className={styles.rolename}>
                        <div className={styles.administratormodule}>
                          <label>{item.RoleName}</label>
                          <p>{item.Description}</p>
                        </div>
                      </div>
                    </div>
                  )}
                </>
              ) : (
                <div
                  className={styles.createroleblock}
                  key={item.RoleId}
                  onClick={() => gotoParticularRole(item)}
                >
                  <div className={styles.rolename}>
                    <div className={styles.administratormodule}>
                      <label>{item.RoleName}</label>
                      <p>{item.Description}</p>
                    </div>
                  </div>
                </div>
              )}
            </>
          ))}
                {profileData?.length === 0 && (
                  <div className={"nodataclass"}><img src={assets.nodataimg} />No Docs Found</div>
                )}

            </div>
        </>
    )
}
export default Createrole
