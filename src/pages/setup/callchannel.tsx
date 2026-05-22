import { useEffect, useMemo, useState } from "react";
import styles from "./scss/callchannel.module.scss";
import DataTable from "react-data-table-component";
import { getCallConfigurationList } from "../../base/service/setup";
import Nodata from "../component/nodata";
import Skeleton from "../MyPlan/skeleton";

type CallConfigItem = {
    id?: number;
    pid?: number;
    contactCenterNumber?: string;
    your_number?: string;
    number?: string;
    name?: string;
    callFlowName?: string;
    recording?: number | boolean;
    callRecording?: number | boolean;
    voicemail?: number | boolean;
    status?: number | boolean;
};

const Callchannel = () => {
    const [callConfigList, setCallConfigList] = useState<CallConfigItem[]>([]);
    const [loader, setLoader] = useState(false);
    useEffect(() => {
        setLoader(true)
        const fetchCallConfig = async () => {
            const list = await getCallConfigurationList();
            setCallConfigList(Array.isArray(list) ? list : []);
            setLoader(false)
        };
        fetchCallConfig();
    }, []);

    const toEnabledDisabled = (value: any) => {
        const normalized = Number(value);
        return normalized === 1 ? "Enabled" : "Disabled";
    };

    const data = useMemo(
        () =>
            callConfigList.map((item: CallConfigItem, index: number) => ({
                id: item?.id ?? item?.pid ?? index,
                activeNumber: item?.contactCenterNumber ?? item?.your_number ?? item?.number ?? "-",
                callFlow: item?.name ?? item?.callFlowName ?? "-",
                callRecording: toEnabledDisabled(item?.recording ?? item?.callRecording ?? item?.status),
                voicemail: toEnabledDisabled(item?.voicemail ?? item?.status),
            })),
        [callConfigList]
    );

    const columns = [
        {
            name: "Active Number",
            selector: (row: any) => row.activeNumber,
            wrap: true,
        },
        {
            name: "Call Flow",
            selector: (row: any) => row.callFlow,
            wrap: true,
        },
        {
            name: "Call Recording",
            selector: (row: any) => row.callRecording,
            cell: (row: any) => (
                <span className={`${styles.tag} ${row.callRecording === "Disabled" ? styles.disable : ""}`}>
                    {row.callRecording}
                </span>
            ),
            wrap: true,
        },
        {
            name: "Voicemail",
            selector: (row: any) => row.voicemail,
            cell: (row: any) => (
                <span className={`${styles.tag} ${row.voicemail === "Disabled" ? styles.disable : ""}`}>
                    {row.voicemail}
                </span>
            ),
            wrap: true,
        },
    ];
    return (
        <>
            <div className={styles.callchannelist}>
                <div className="worktualtable">
                    {loader
                        ? <Skeleton count={4} />
                        : (
                            <DataTable
                                columns={columns}
                                data={data}
                                fixedHeader
                                responsive
                                noDataComponent={<Nodata content="Not Configured Yet" isChildren={true} />}
                            />
                        )}
                </div>
            </div>
        </>
    );
};
export default Callchannel;
