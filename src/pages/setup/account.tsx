import { useEffect, useMemo, useState } from "react";
import DataTable from "react-data-table-component";
import { getTeamManagementQueue } from "../../base/service/setup";

type QueueItem = {
    qid?: number;
    name?: string;
    description?: string;
    businessName?: string | null;
    callWrapUpTime?: number | string | null;
    maxCallHandlingTimeMM?: number | string | null;
    maximumCallers?: string;
    maximumWaitTime?: string;
    hours_of_operation?: string;
};

const parseJson = (value: any) => {
    if (!value) return {};
    if (typeof value === "object") return value;
    if (typeof value !== "string") return {};
    try {
        return JSON.parse(value);
    } catch {
        return {};
    }
};

export default function AccountSetup() {
    const [queueList, setQueueList] = useState<QueueItem[]>([]);

    useEffect(() => {
        const fetchQueueList = async () => {
            const list = await getTeamManagementQueue();
            setQueueList(Array.isArray(list) ? list : []);
        };
        fetchQueueList();
    }, []);

    const tableData = useMemo(() => {
        return queueList.map((item) => {
            const maxCallers = parseJson(item?.maximumCallers);
            const maxWait = parseJson(item?.maximumWaitTime);
            const hoursOfOperation = parseJson(item?.hours_of_operation);

            const businessHours =
                item?.businessName ??
                hoursOfOperation?.name ??
                (typeof item?.hours_of_operation === "string" ? item.hours_of_operation : "-") ??
                "-";

            const maximumCallersText = maxCallers?.type
                ? String(maxCallers.type).toLowerCase() === "no limit"
                    ? "No Limit"
                    : `${maxCallers.type} callers`
                : "-";

            const maximumWaitText = maxWait?.type
                ? String(maxWait.type).toLowerCase() === "keep waiting"
                    ? "Keep Waiting"
                    : `${maxWait.type} minutes`
                : "-";

            return {
                id: item?.qid ?? '-',
                teamName: item?.name ?? "-",
                teamDescription: item?.description ?? "-",
                businessHours,
                callWrapUpTime: `${item?.callWrapUpTime ?? "-"} seconds`,
                maxCallHandlingTime: `${item?.maxCallHandlingTimeMM ?? "-"} minutes`,
                maximumCallers: maximumCallersText,
                maximumWaitTime: maximumWaitText,
                actionWhenLimitReached: maxCallers?.limitResponseType === 0 ? "Offer Callback" : "Send to Voicemail",
                actionWhenTimeExceeded: maxWait?.fallBackQueue ? "Send to Queue" : "Send to Voicemail",
            };
        });
    }, [queueList]);

    const columns = [
        { name: "Team Name", selector: (row: any) => row.teamName},
        { name: "Team Description", selector: (row: any) => row.teamDescription},
        { name: "Business Hours", selector: (row: any) => row.businessHours},
        { name: "Call Wrap-up Time", selector: (row: any) => row.callWrapUpTime },
        { name: "Maximum call handling time", selector: (row: any) => row.maxCallHandlingTime},
        { name: "Maximum Callers in Team", selector: (row: any) => row.maximumCallers},
        { name: "Maximum Wait Time", selector: (row: any) => row.maximumWaitTime},
        { name: "Action When Limit Reached", selector: (row: any) => row.actionWhenLimitReached },
        { name: "Action When Time Exceeded", selector: (row: any) => row.actionWhenTimeExceeded },
    ];

    return (
        <div className="worktualtable">
            <DataTable columns={columns} data={tableData} fixedHeader responsive />
        </div>
    );
}
