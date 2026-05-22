import { useEffect, useMemo, useState } from "react";
import { getCustomHoursList, getUpdateBusinessHolidays } from "../../base/service/setup";
import styles from "./scss/set.module.scss";
import { useDispatch, useSelector } from "react-redux";
import { SetupSliceActions } from "../../store/slice/setupSlice/setupSlice";

const getEntry = (entry: any) => {
    if (!entry) return null;
    if (Array.isArray(entry)) return entry[0];
    if (typeof entry === "object") return entry;
    return null;
};

type SalesSetupProps = {
    onDetailViewChange?: (isDetailView: boolean) => void;
};
export default function SalesSetup({ onDetailViewChange }: SalesSetupProps) {
    const dispatch = useDispatch();
    const hoursList = useSelector((state: any) => state?.setupStore?.businessHoursList ?? []);
    const businessHolidays = useSelector((state: any) => state?.setupStore?.businessHolidaysList ?? []);
    const [particularData, setParticularData] = useState<{ cid: number | null; particularView: boolean }>({
        cid: null,
        particularView: false,
    });

    useEffect(() => {
        const fetchData = async () => {
            if (hoursList.length > 0 && businessHolidays.length > 0) {
                return;
            }

            const list = await getCustomHoursList();
            const hours = Array.isArray(list) ? list : [];
            dispatch(SetupSliceActions.setBusinessHoursList(hours));

            const businessIds = Array.from(
                new Set(
                    hours
                        .map(
                            (item: any) =>
                                item?.cid ??
                                item?.businessId ??
                                item?.business_id ??
                                item?.bid ??
                                null
                        )
                        .filter((id: any) => id !== null && id !== undefined && `${id}`.trim() !== "")
                        .map((id: any) => String(id))
                )
            );

            if (businessIds.length === 0) {
                dispatch(SetupSliceActions.setBusinessHolidaysList([]));
                return;
            }

            const holidayResponses = await Promise.all(
                businessIds.map((id) => getUpdateBusinessHolidays(id, null))
            );

            const merged = holidayResponses
                .filter((list) => Array.isArray(list))
                .flat();

            dispatch(SetupSliceActions.setBusinessHolidaysList(merged));
        };

        fetchData();
    }, [businessHolidays.length, dispatch, hoursList.length]);

    const tableData = useMemo(() => {
        return hoursList.map((item: any) => {
            let hoursArray = item?.hours;

            if (typeof hoursArray === "string") {
                try {
                    hoursArray = JSON.parse(hoursArray);
                } catch {
                    hoursArray = [];
                }
            }

            if (typeof hoursArray === "string") {
                try {
                    hoursArray = JSON.parse(hoursArray);
                } catch {
                    hoursArray = [];
                }
            }

            hoursArray = Array.isArray(hoursArray) ? hoursArray : [];

            const workingDays =
                hoursArray
                    .filter((h: any) => !h?.isHoliday)
                    .map((h: any) => h?.day)
                    .join(", ") || "-";

            const operatingHours =
                hoursArray
                    .filter((h: any) => !h?.isHoliday)
                    .map((h: any) => {
                        const entry = getEntry(h?.entry);
                        if (!entry) return "-";
                        return `${entry?.startTime ?? "-"} - ${entry?.endTime ?? "-"}`;
                    })
                    .join(", ") || "-";

            const dayOff =
                hoursArray
                    .filter((h: any) => h?.isHoliday)
                    .map((h: any) => h?.day)
                    .join(", ") || "-";

            return {
                cid: item?.cid,
                name: item?.name ?? "Main Hours",
                timeZone: item?.timeZone ?? item?.timezone ?? item?.timezoneName ?? item?.time_zone ?? "-",
                operatingHours,
                workingDays,
                dayOff,
            };
        });
    }, [hoursList]);

    const selectedData = useMemo(
        () => hoursList.find((item) => item?.cid === particularData?.cid) ?? null,
        [hoursList, particularData?.cid]
    );

    const selectedHours = useMemo(() => {
        if (!selectedData?.hours) return [];
        let parsed = selectedData.hours;
        if (typeof parsed === "string") {
            try {
                parsed = JSON.parse(parsed);
            } catch {
                parsed = [];
            }
        }
        return Array.isArray(parsed) ? parsed : [];
    }, [selectedData]);

    const weekDays = ["Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"];

    const formatTime = (value: string) => {
        if (!value || typeof value !== "string" || !value.includes(":")) return value || "-";
        const [h, m] = value.split(":").map(Number);
        if (Number.isNaN(h) || Number.isNaN(m)) return value;
        const suffix = h >= 12 ? "PM" : "AM";
        const hour12 = h % 12 === 0 ? 12 : h % 12;
        return `${hour12}:${String(m).padStart(2, "0")} ${suffix}`;
    };

    const getDuration = (from?: string, to?: string) => {
        if (!from || !to || !from.includes(":") || !to.includes(":")) return "0 hrs";
        const [fh, fm] = from.split(":").map(Number);
        const [th, tm] = to.split(":").map(Number);
        if ([fh, fm, th, tm].some((v) => Number.isNaN(v))) return "0 hrs";
        let start = fh * 60 + fm;
        let end = th * 60 + tm;
        if (end < start) end += 24 * 60;
        const diff = end - start;
        const hrs = Math.floor(diff / 60);
        const mins = diff % 60;
        return mins ? `${hrs}:${String(mins).padStart(2, "0")} hrs` : `${hrs} hrs`;
    };

    const holidayRows = useMemo(() => {
        const formatDate = (value: any) => {
            if (!value) return "";
            const date = new Date(Number(value));
            if (Number.isNaN(date.getTime())) return String(value);
            return date.toLocaleDateString("en-US", {
                weekday: "long",
                year: "numeric",
                month: "long",
                day: "numeric",
            });
        };

        const validBusinessIds = new Set(
            hoursList
                .map(
                    (item: any) =>
                        item?.cid ??
                        item?.businessId ??
                        item?.business_id ??
                        item?.bid ??
                        null
                )
                .filter((id: any) => id !== null && id !== undefined && `${id}`.trim() !== "")
                .map((id: any) => String(id))
        );

        return businessHolidays
            .filter((holiday: any) => {
                const holidayBusinessId =
                    holiday?.businessId ??
                    holiday?.business_id ??
                    holiday?.cid ??
                    holiday?.bid ??
                    null;

                // If holiday payload has no business id field, keep it.
                if (holidayBusinessId === null || holidayBusinessId === undefined || `${holidayBusinessId}`.trim() === "") {
                    return true;
                }

                return validBusinessIds.has(String(holidayBusinessId));
            })
            .map((holiday: any, index: number) => ({
            key: `holiday-${index}`,
            name: holiday?.holidayName ?? holiday?.name ?? "Holiday",
            detail: (() => {
                const start = formatDate(holiday?.startDate);
                const end = formatDate(holiday?.endDate);
                if (start && end) return start === end ? start : `${start} - ${end}`;
                return start || end || "-";
            })(),
        }));
    }, [businessHolidays, hoursList]);

    const setIsOpen = () => {
        setParticularData({ cid: null, particularView: false });
    };
    useEffect(() => {
        onDetailViewChange?.(particularData?.particularView);
        return () => {
            onDetailViewChange?.(false);
        };
    }, [particularData?.particularView, onDetailViewChange]);

    if (particularData?.particularView && selectedData) {
        return (
        
            <>
               <div className="breadcrumb" style={{top:"24px", paddingBottom:"12px" }}>

                    <a style={{cursor:"pointer"}}  onClick={setIsOpen} >  Business hours & holidays</a>
                    <span className="separator"><svg xmlns="http://www.w3.org/2000/svg" width="6" height="12" viewBox="0 0 6 12" fill="none">
                        <path d="M0.749951 11.3309C0.557551 11.3309 0.365152 11.2577 0.218752 11.1102C-0.0737485 10.8172 -0.0727486 10.3426 0.220751 10.0497L4.08505 6.19718C4.22715 6.05558 4.30525 5.86608 4.30525 5.66498C4.30525 5.46478 4.22715 5.27628 4.08505 5.13468L0.220352 1.28118C-0.0726482 0.988176 -0.0735484 0.513576 0.218852 0.220676C0.510852 -0.0732242 0.985952 -0.0732245 1.27985 0.218675L5.14415 4.07218C5.56995 4.49698 5.80475 5.06238 5.80525 5.66498C5.80575 6.26758 5.57085 6.83298 5.14415 7.25968L1.27945 11.1122C1.13345 11.2577 0.941351 11.3309 0.749951 11.3309Z" fill="#2D2B2F" />
                    </svg></span>
                    <span className="current">{selectedData?.name ?? "-"}</span>
                </div>
            <div className={styles.timeslotpage}>
                {/* <div className={styles.settitle}>
                    <span className={styles.clickable} onClick={setIsOpen}>
                        Business hours & holidays
                    </span>
                    <span> &gt; </span>
                    <span>{selectedData?.name ?? "-"}</span>
                </div> */}
              

                <div className={styles.timeslotcontainer}>
                    <div className={styles.row}>
                        <div className={styles.lefts}>Timezone</div>
                        <div className={styles.rights}>
                            {selectedData?.timeZone ?? selectedData?.timezone ?? selectedData?.timezoneName ?? "-"}
                        </div>
                    </div>
                    {weekDays.map((day) => {
                        const dayEntries = selectedHours.filter((h: any) => h?.day === day);
                        const workingEntries = dayEntries.filter((h: any) => !h?.isHoliday);
                        const isDayOff = dayEntries.length === 0 || workingEntries.length === 0;

                        return (
                            <div className={styles.row} key={day}>
                                <div className={styles.lefts}>{day}</div>
                                <div className={styles.rights}>
                                    {isDayOff ? (
                                        <div className={styles.inptrow}>Day Off</div>
                                    ) : (
                                        workingEntries.map((slot: any, index: number) => {
                                            const entry = getEntry(slot?.entry);
                                            const from = entry?.startTime ?? "-";
                                            const to = entry?.endTime ?? "-";
                                            return (
                                                <div className={styles.inptrow} key={`${day}-${index}`}>
                                                    <div className={styles.inputdiv}>{formatTime(from)}</div>
                                                    <span>To</span>
                                                    <div className={styles.inputdiv}>{formatTime(to)}</div>
                                                    <span>{getDuration(from, to)}</span>
                                                </div>
                                            );
                                        })
                                    )}
                                </div>
                            </div>
                        );
                    })}
                </div>
            </div>
            </>
        );
    }

    return (
        <>
            <div className={styles.salesmaincontainer}>
                <div className={styles.contentviewtable}>
                    <div className={styles.header}>
                        <div className={styles.headerinner}>Schedule Name</div>
                        <div className={styles.headerinner}>Time Zone</div>
                    </div>
                    {(tableData.length > 0 ? tableData : [{ cid: -1, name: "-", timeZone: "-" }]).map((row) => (
                        <div className={styles.row} key={`${row?.cid}-${row?.name}`}>
                            <div
                                className={`${styles.col} ${styles.clickable}`}
                                onClick={() => setParticularData({ cid: row?.cid, particularView: true })}
                            >
                                {row?.name}
                            </div>
                            <div className={styles.col}>{row?.timeZone}</div>
                        </div>
                    ))}
                </div>

                <div className={styles.contentviewtable}>
                    <div className={styles.header}>Holidays & Special Closures</div>
                    {(holidayRows.length > 0 ? holidayRows : [{ key: "holiday-default", name: "-", detail: "-" }]).map((row) => (
                        <div className={styles.row} key={row.key}>
                            <div className={styles.col}>{row.name}</div>
                            <div className={styles.col}>{row.detail}</div>
                        </div>
                    ))}
                </div>
            </div>
        </>
    );
}
