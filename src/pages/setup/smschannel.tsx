import { useEffect, useMemo, useState } from "react";
import DataTable from "react-data-table-component";
import styles from "./scss/callchannel.module.scss";
import { getSmsConfigurationList } from "../../base/service/setup";
import Dynamicnodata from "../component/Dynamicnodata";
import Nodata from "../component/nodata";
import Skeleton from "../MyPlan/skeleton";

type SmsConfigItem = {
    id?: number;
    pid?: number;
    configurationType?: any;
    supportSmsNumber?: string;
    supportSMSNumber?: string;
    your_number?: string;
    smsThreshold?: string | number;
    currentUsage?: string | number;
    currentUsageToday?: string | number;
    isBot?: number;
    type?: string | null;
};

const normalizeConfigType = (value: any) => {
    if (Array.isArray(value)) {
        return value.map((item) => item?.text ?? item?.name ?? String(item)).join(", ");
    }
    if (typeof value === "object" && value !== null) {
        return value?.text ?? value?.name ?? JSON.stringify(value);
    }
    return value ?? "-";
};

const Smschannel = () => {
    const [smsConfigList, setSmsConfigList] = useState<SmsConfigItem[]>([]);
    const [loader, setLoader] = useState(false);
    useEffect(() => {
        setLoader(true)
        const fetchSmsConfig = async () => {
            const list = await getSmsConfigurationList();
            setSmsConfigList(Array.isArray(list) ? list : []);
            setLoader(false)
        };
        fetchSmsConfig();
    }, []);

    const data = useMemo(
        () =>
            smsConfigList.map((item: SmsConfigItem, index: number) => ({
                id: item?.id ?? item?.pid ?? index,
                configurationType: (() => {
                    const apiType = item?.type ?? normalizeConfigType(item?.configurationType);
                    const baseType =
                        !apiType || apiType === "-" || String(apiType).toLowerCase() === "null"
                            ? "New Number"
                            : String(apiType);
                    return baseType;
                })(),
                supportSmsNumber: item?.your_number ?? item?.supportSmsNumber ?? item?.supportSMSNumber ?? "-",
                smsThreshold:
                    item?.smsThreshold !== undefined && item?.smsThreshold !== null
                        ? `${item.smsThreshold} messages per day`
                        : "-",
                currentUsageToday: item?.currentUsageToday ?? item?.currentUsage ?? "-",
            })),
        [smsConfigList]
    );

    const columns = [
        {
            name: "Configuration Type",
            selector: (row: any) => row.configurationType,
            cell: (row: any) => <span className={styles.configType}>{row.configurationType}</span>,
            wrap: true,
        },
        {
            name: "Support SMS Number",
            selector: (row: any) => row.supportSmsNumber,
            wrap: true,
        },
        {
            name: "SMS Threshold",
            selector: (row: any) => row.smsThreshold,
            wrap: true,
        },
        {
            name: "Current Usage",
            selector: (row: any) => row.currentUsageToday,
            wrap: true,
        },
    ];

    return (
        <div className={styles.callchannelist}>

            <div className="worktualtable">
            {loader
                                ? <Skeleton count={4} /> :
                <DataTable columns={columns}
                    data={data}
                    fixedHeader
                    responsive
                    noDataComponent={<Nodata content="Not Configured Yet" isChildren={true}/>}
                /> }
            </div>
        </div>
    );
};

export default Smschannel;