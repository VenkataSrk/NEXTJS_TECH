import React, { useEffect, useState } from 'react';
import styles from '../../assets/styles/usermanagement.module.scss';
import Skeleton from '../../skeleton';
import { getRolesListInfo } from '../../apiservice/myplanapi';
import { useNavigate } from 'react-router-dom';
import { GetItemFromStorage } from '../../../../base/custom_hooks/useStorage';
import { LocalStorage } from '../../../../base/custom_hooks/localStorageKeys';

const Createrole = () => {

  const navigate = useNavigate();
  const userDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
  const [profileData, setProfileData] = useState<any[]>([]);
  const [loader, setLoader] = useState(false)

  useEffect(() => {
    if (userDetails) {
      setLoader(true)
      const body = {
        company_id: userDetails.company_id,
        RoleCategoryId: 1,
        product_id: userDetails?.product_id
      }

      getRolesListInfo(body)
        .then((res: any) => {
          setProfileData(res?.data?.result || []);
          setLoader(false)
        })
        .catch((err: any) => { console.log(err); setLoader(false) });
    }
    return () => {
      console.log("cleanup");
      setLoader(false)
    };
  }, []);

  const filteredRoles = profileData?.filter(
    (role: any) => role?.RoleId === 3 || role?.RoleId === 5 || role?.RoleId === 6
  ) || [];

  const gotoParticularRole = (data: any) => {
    navigate(`/Useradminroles?roleId=${data?.RoleId}`);

  }

  return (
    <>
      <style>
        {`
          .operationmain {
            overflow: hidden!important;
          }
        `}
      </style>

      <div className={styles.createmaincontainer}>
        {/* <div className={styles.headerole}>
          <h3>Roles</h3>
          <p>Create and Manage roles for the Worktual products</p>
        </div> */}

        <div className={styles.rolecontainer}>
          <div className={styles.rolecontainerinner}>

            {loader && <Skeleton count={4} />}

            {filteredRoles?.map((role: any, index: any) => {
              return (
                <div key={index} className={styles.createroleblock} onClick={() => gotoParticularRole(role)}>
                  <div className={styles.rolename}>
                    <div className={styles.administratormodule}>
                      <div className={styles.accesstitle}>
                        {role?.RoleName || '-'}
                      </div>
                      <p>{role?.Description || '-'}</p>
                    </div>
                  </div>
                </div>
              )
            })}

            {!loader && !filteredRoles?.length &&
              <div className={styles.nodatacontainer}>
                <div className={styles.nodatabox}>
                  {/* <img src={assets.emptyData} alt="No data" /> */}
                  <div className={styles.connected}>
                    <div className={styles.callstitle}>
                      No roles available
                    </div>
                    <div className={styles.callmessage}>
                      There are no roles to display at the moment.
                    </div>
                  </div>
                </div>
              </div>
            }

          </div>
        </div>
      </div>
    </>
  )
}

export default Createrole;
