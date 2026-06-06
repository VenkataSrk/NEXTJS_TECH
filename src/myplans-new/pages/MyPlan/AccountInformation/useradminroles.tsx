import React, { useEffect, useMemo, useState } from "react";
import { useNavigate, useSearchParams } from "react-router-dom";
import moment from "moment-timezone";
import styles from "../../../assets/styles/accountinfo.module.scss";
import { getAllMembersRole, getUserDetails } from "../../../apiservice/myplanapi";

type RoleInfo = {
  members: any[];
  firstCreated?: string;
  lastModified?: string;
  lastViewed?: string;
};

const roleMap: Record<number, string> = {
  1: "Owner",
  2: "Sub-Owner",
  3: "Admin",
  4: "Member",
  5: "Supervisor",
  6: "Agent",
  7: "Consultant",
  8: "Executive",
};

export default function Useradminroles() {
  const navigate = useNavigate();
  const [searchParams] = useSearchParams();

  const roleId = searchParams.get("roleId");
  const roleIdNum = useMemo(() => {
    if (!roleId) return null;
    const num = Number(roleId);
    return Number.isFinite(num) ? num : null;
  }, [roleId]);

  const [roleInfo, setRoleInfo] = useState<RoleInfo>({ members: [] });
  const [viewAll, setViewAll] = useState(false);
  const [userInfo, setUserInfo] = useState({ queueNames: "", reportToNames: "" });

  const getParticularRole = async (roleIdValue: number) => {
    if (!roleIdValue) return;
    try {
      const body = { roleId: Number(roleIdValue) };
      const res: any = await getAllMembersRole(body);

      if (Array.isArray(res) && res.length > 0) {
        const firstCreated = res
          .map((item: any) => new Date(item?.created_date))
          .filter((d: any) => !Number.isNaN(d?.getTime?.()))
          .sort((a: any, b: any) => a - b)[0];

        const lastModified = res
          .map((item: any) =>
            item?.modified_date
              ? new Date(item.modified_date)
              : item?.last_viewed
                ? new Date(item.last_viewed)
                : null
          )
          .filter(Boolean)
          .filter((d: any) => !Number.isNaN(d?.getTime?.()))
          .sort((a: any, b: any) => b - a)[0];

        setRoleInfo({
          members: res,
          firstCreated: firstCreated ? moment(firstCreated).format("MMM DD, YYYY") : "-",
          lastModified: lastModified ? moment(lastModified).format("MMM DD, YYYY") : "-",
          lastViewed: lastModified ? moment(lastModified).format("MMM DD, YYYY") : "-",
        });
      } else {
        setRoleInfo({ members: [] });
      }
    } catch (error) {
      setRoleInfo({ members: [] });
      console.log("getParticularRole Error", error);
    }
  };

  const getUserDetail = async (roleIdValue: number) => {
    try {
      const body = { roleId: Number(roleIdValue) };
      const res: any = await getUserDetails(body);
      if (Array.isArray(res)) {
        setUserInfo({
          queueNames: Array.from(new Set(res?.map((data: any) => data?.queueName).filter(Boolean))).join(", "),
          reportToNames: Array.from(new Set(res?.map((data: any) => data?.report_to_UserName).filter(Boolean))).join(
            ", "
          ),
        });
      }
    } catch (error) {
      console.log("getUserDetail Error", error);
    }
  };

  useEffect(() => {
    if (!roleIdNum) return;
    getParticularRole(roleIdNum);
    getUserDetail(roleIdNum);
  }, [roleIdNum]);

  const roleName = roleIdNum ? roleMap[roleIdNum] || "Unknown" : "Unknown";
  const visibleMembers = viewAll ? roleInfo?.members : roleInfo?.members?.slice(0, 2);

  return (
    <>
      <div className={styles.infocontainer}>
        <div className={styles.yourinfo}>
          <div className={styles.breadcrumb}>
            <p onClick={() => navigate("/roles")}>
              Roles{" "}
              <svg xmlns="http://www.w3.org/2000/svg" width="6" height="12" viewBox="0 0 6 12" fill="none">
                <path
                  d="M0.847363 11.6653C0.654963 11.6653 0.462564 11.5921 0.316164 11.4446C0.0236636 11.1516 0.0246635 10.677 0.318163 10.3841L4.18246 6.53165C4.32456 6.39005 4.40266 6.20055 4.40266 5.99945C4.40266 5.79925 4.32456 5.61075 4.18246 5.46915L0.317764 1.61565C0.0247639 1.32265 0.0238637 0.848048 0.316264 0.555148C0.608264 0.261248 1.08336 0.261248 1.37726 0.553148L5.24156 4.40665C5.66736 4.83145 5.90216 5.39685 5.90266 5.99945C5.90316 6.60205 5.66826 7.16745 5.24156 7.59415L1.37686 11.4466C1.23086 11.5921 1.03876 11.6653 0.847363 11.6653Z"
                  fill="black"
                />
              </svg>
            </p>
            <h3>{roleName}</h3>
          </div>

        <div className={`${styles.titlearea} ${styles.titlefl}`}>
          <div className={styles.profileleft}>{roleName?.charAt(0).toUpperCase()}</div>
          <div>
            <div className={styles.title}>{roleName}</div>
            <div className={styles.desc}>Manage the personal details used across all your Worktual applications</div>
          </div>
        </div>

        <div className={`${styles.yourinfotop} ${styles.profilets}`}>
          <div className={`${styles.fieldareamain} ${styles.fieldareamainrst}`}>
            <div className={styles.fieldmain}>
              <div className={styles.fieldleft}>
                <h3>Created</h3>
                <p>{roleInfo?.firstCreated || "-"}</p>
              </div>
              <div className={`${styles.profileright} ${styles.fieldleft}`}>
                <h3>Product</h3>
              </div>
            </div>
            <div className={styles.fieldmain}>
              <div className={styles.fieldleft}>
                <h3>Modified</h3>
                <p>{roleInfo?.lastModified || "-"}</p>
              </div>
              <div className={`${styles.profileright} ${styles.fieldleft}`}>Campaign Management</div>
            </div>
          </div>

          <div className={styles.yourinfotop}>
            <div className={styles.yourtitle}>Role Information</div>
            <div className={styles.fieldareamain}>
              <div className={styles.label}>Role Category</div>
              <div className={styles.value}>{roleName}</div>
              <div className={styles.icon}></div>
            </div>
            {/* <div className={styles.fieldareamain}>
              <div className={styles.label}>Queue names</div>
              <div className={styles.value}>{userInfo?.queueNames || "-"}</div>
              <div className={styles.icon}></div>
            </div> */}
            {/* {roleIdNum !== 3 && (
              <div className={styles.fieldareamain}>
                <div className={styles.label}>Reporting To</div>
                <div className={styles.value}>{userInfo?.reportToNames || "-"}</div>
                <div className={styles.icon}></div>
              </div>
            )} */}
            <div className={styles.fieldareamain}>
              <div className={styles.label}>Last Viewed</div>
              <div className={styles.value}>{roleInfo?.lastViewed || "-"}</div>
              <div className={styles.icon}></div>
            </div>
          </div>
        </div>

        <div className={styles.iconaccess}>
          <div className={styles.accesspermisson}>
            <div className={styles.accesspert}>
              <svg xmlns="http://www.w3.org/2000/svg" width="23" height="15" viewBox="0 0 23 15" fill="none">
                <path
                  d="M10.4258 14.9248H20.8516C21.5046 14.9248 21.9639 14.8307 22.2295 14.6426C22.4951 14.46 22.6279 14.186 22.6279 13.8208C22.6279 13.2896 22.4647 12.7279 22.1382 12.1357C21.8172 11.5436 21.3524 10.9902 20.7437 10.4756C20.1405 9.9554 19.4072 9.53483 18.5439 9.21387C17.6862 8.88737 16.7178 8.72412 15.6387 8.72412C14.5651 8.72412 13.5967 8.88737 12.7334 9.21387C11.8757 9.53483 11.1424 9.9554 10.5337 10.4756C9.92497 10.9902 9.46012 11.5436 9.13916 12.1357C8.8182 12.7279 8.65771 13.2896 8.65771 13.8208C8.65771 14.186 8.78776 14.46 9.04785 14.6426C9.31348 14.8307 9.77279 14.9248 10.4258 14.9248ZM15.647 7.23828C16.2391 7.23828 16.7869 7.0778 17.2905 6.75684C17.7941 6.43034 18.1981 5.9904 18.5024 5.43701C18.8068 4.88363 18.959 4.26107 18.959 3.56934C18.959 2.88867 18.8068 2.27995 18.5024 1.74316C18.1981 1.20085 17.7941 0.77474 17.2905 0.464844C16.7869 0.154948 16.2391 0 15.647 0C15.0549 0 14.507 0.157715 14.0034 0.473145C13.4998 0.788574 13.0931 1.21745 12.7832 1.75977C12.4788 2.30208 12.3267 2.91081 12.3267 3.58594C12.3267 4.27214 12.4788 4.89193 12.7832 5.44531C13.0876 5.99316 13.4915 6.43034 13.9951 6.75684C14.4987 7.0778 15.0493 7.23828 15.647 7.23828ZM1.46094 14.9248H7.76123C7.54541 14.6094 7.44303 14.2303 7.4541 13.7876C7.46517 13.3449 7.56755 12.88 7.76123 12.3931C7.95492 11.9061 8.22054 11.4357 8.55811 10.9819C8.9012 10.5226 9.29688 10.1187 9.74512 9.77002C9.28027 9.46012 8.74626 9.20557 8.14307 9.00635C7.54541 8.80159 6.85921 8.69922 6.08447 8.69922C5.14925 8.69922 4.30534 8.85417 3.55273 9.16406C2.80566 9.46842 2.1665 9.8724 1.63525 10.376C1.10954 10.874 0.705566 11.4191 0.42334 12.0112C0.141113 12.6034 0 13.1872 0 13.7627C0 14.1335 0.110677 14.4185 0.332031 14.6177C0.558919 14.8224 0.935221 14.9248 1.46094 14.9248ZM6.08447 7.4292C6.60465 7.4292 7.08333 7.28809 7.52051 7.00586C7.95768 6.72363 8.30908 6.3418 8.57471 5.86035C8.84033 5.37337 8.97314 4.83105 8.97314 4.2334C8.97314 3.63574 8.84033 3.10449 8.57471 2.63965C8.30908 2.16927 7.95492 1.80127 7.51221 1.53564C7.07503 1.26449 6.59912 1.12891 6.08447 1.12891C5.57536 1.12891 5.09945 1.26725 4.65674 1.54395C4.21956 1.8151 3.8654 2.18587 3.59424 2.65625C3.32308 3.12663 3.19027 3.65788 3.1958 4.25C3.1958 4.84212 3.32861 5.37891 3.59424 5.86035C3.85986 6.3418 4.21126 6.72363 4.64844 7.00586C5.09115 7.28809 5.56982 7.4292 6.08447 7.4292Z"
                  fill="black"
                />
              </svg>
              <div className={`${styles.featuresa} ${styles.featuresalist}`}>
                <h3>Members with this Role</h3>
                <p>This role contributes to ongoing operations across the organization.</p>
              </div>
            </div>

            {Array.isArray(roleInfo?.members) && roleInfo.members.length > 2 && (
              <div className={styles.featurenet}>
                <p onClick={() => setViewAll(!viewAll)}>
                  {viewAll ? "Show Less" : "View All Members"}
                  <svg xmlns="http://www.w3.org/2000/svg" width="6" height="12" viewBox="0 0 6 12" fill="none">
                    <path
                      d="M0.749951 11.3309C0.557551 11.3309 0.365152 11.2577 0.218752 11.1102C-0.0737485 10.8172 -0.0727486 10.3426 0.220751 10.0497L4.08505 6.19718C4.22715 6.05558 4.30525 5.86608 4.30525 5.66498C4.30525 5.46478 4.22715 5.27628 4.08505 5.13468L0.220352 1.28118C-0.0726482 0.988176 -0.0735484 0.513576 0.218852 0.220676C0.510852 -0.0732242 0.985952 -0.0732245 1.27985 0.218675L5.14415 4.07218C5.56995 4.49698 5.80475 5.06238 5.80525 5.66498C5.80575 6.26758 5.57085 6.83298 5.14415 7.25968L1.27945 11.1122C1.13345 11.2577 0.941351 11.3309 0.749951 11.3309Z"
                      fill="#6B46C1"
                    />
                  </svg>
                </p>
              </div>
            )}
          </div>
          <div className={styles.visiblememberlist}>
          {Array.isArray(visibleMembers) && visibleMembers.length > 0 ? (
            visibleMembers.map((item: any, id: number) => (
              <div key={id} className={styles.accesspermisson}>
                <div className={styles.accesspert}>
                  <span className={styles.profileic}>
                    {(item?.Name ?? item?.name ?? item?.AgentName ?? "?").toString().charAt(0).toUpperCase()}
                  </span>

                  <div className={styles.featuresa}>
                    <h3>{item?.Name ?? item?.name ?? item?.AgentName ?? "-"}</h3>
                    <p>{item?.RoleName ?? item?.role_name ?? "-"}</p>
                  </div>
                </div>

                <div className={styles.featurenet}>
                  <div className={styles.assignedactive}>
                    <p>Assigned</p>
                    <p>{item?.created_date ? moment(item.created_date).format("MMM DD, YYYY") : "-"}</p>
                  </div>
                  <span>{item?.Status ?? item?.status ?? ""}</span>
                </div>
              </div>
            ))
          ) : (
            <p>No members available</p>
          )}
          </div>
        </div>
      </div>
      </div>
    </>
  );
}
