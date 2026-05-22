--
-- PostgreSQL database dump
--

\restrict uXHcZpsu8GT8zYE7ei1mvLy4RDivoyPnqm40C3GuJleezx8kvw006LrSk5uV9Rh

-- Dumped from database version 16.11 (Ubuntu 16.11-0ubuntu0.24.04.1)
-- Dumped by pg_dump version 16.11 (Ubuntu 16.11-0ubuntu0.24.04.1)

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET xmloption = content;
SET client_min_messages = warning;
SET row_security = off;

--
-- Name: ccaas_create_videomeet_session_history_details_logic(character varying, bigint, bigint, character varying, integer, smallint, bigint, bigint, bigint, bigint, bigint, integer, integer, bigint, smallint, bigint, bigint, text, bigint, integer, integer, integer, bigint, bigint, bigint, bigint, smallint, integer, integer, integer, character varying); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.ccaas_create_videomeet_session_history_details_logic(IN p_sessionid character varying, IN p_sessionstarttime bigint, IN p_sessionendtime bigint, IN p_customernumber character varying, IN p_domainid integer, IN p_isinbound smallint, IN p_dialduration bigint, IN p_disconnectedby bigint, IN p_queueduration bigint, IN p_callednumber bigint, IN p_waitduration bigint, IN p_recordingtypeid integer, IN p_customerid integer, IN p_videoduration bigint, IN p_isabandoned smallint, IN p_businessstarttime bigint, IN p_businessendtime bigint, IN p_meetingid text, IN p_agentid bigint, IN p_queueid integer, IN p_dispositionid integer, IN p_umnstatus integer, IN p_starttime bigint, IN p_endtime bigint, IN p_agentduration bigint, IN p_transferredtime bigint, IN p_ismissedcall smallint, IN p_transferedby integer, IN p_transferedto integer, INOUT p_errcode integer DEFAULT '-1'::integer, INOUT p_errmsg character varying DEFAULT 'Video inserted failed'::character varying)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_VideoHistoryId BIGINT;
    v_LastAgentId BIGINT;
    v_LastUMNStatus INT;
    v_LastVideoDurationHistoryId BIGINT;
    v_AgentId BIGINT;
    v_curDate TIMESTAMP := CURRENT_TIMESTAMP;
    v_transferedBy BIGINT;
    v_transferedTo BIGINT;
    v_SourceVideoDurationHistoryId BIGINT;
    v_VideoDurationHistoryId BIGINT;
    v_VideoLastDurationHistoryId BIGINT;
BEGIN

    DROP TABLE IF EXISTS temp_User;

    CREATE TEMP TABLE temp_User AS
    SELECT "UserID","ext"
    FROM "tb_User"
    WHERE "ext" IN (p_AgentId, p_transferedBy, p_transferedTo)
    AND "domainId" = p_domainId;

    SELECT "UserID" INTO v_transferedTo FROM temp_User WHERE "ext" = p_transferedTo LIMIT 1;
    SELECT "UserID" INTO v_transferedBy FROM temp_User WHERE "ext" = p_transferedBy LIMIT 1;
    SELECT "UserID" INTO v_AgentId FROM temp_User WHERE "ext" = p_AgentId LIMIT 1;

    IF p_domainId IS NULL OR p_domainId = 0 THEN
        p_errcode := -1;
        p_errmsg := 'DomainId should not be empty';
        RETURN;
    END IF;

    SELECT "VideoHistoryId"
    INTO v_VideoHistoryId
    FROM "tb_VideoHistory"
    WHERE "meetingId" = p_meetingId
    LIMIT 1;

    IF v_VideoHistoryId IS NOT NULL THEN

        SELECT "AgentId","UMNStatus","VideoDurationHistoryId"
        INTO v_LastAgentId, v_LastUMNStatus, v_LastVideoDurationHistoryId
        FROM "tb_VideoDurationHistory"
        WHERE "VideoHistoryId" = v_VideoHistoryId
        ORDER BY "VideoDurationHistoryId" DESC
        LIMIT 1;

        UPDATE "tb_VideoHistory"
        SET
            "sessionStartTime" = COALESCE(p_sessionStartTime,"sessionStartTime"),
            "sessionEndTime" = COALESCE(p_sessionEndTime,"sessionEndTime"),
            "customerNumber" = COALESCE(p_customerNumber,"customerNumber"),
            "domainId" = COALESCE(p_domainId,"domainId"),
            "IsInBound" = COALESCE(p_IsInBound,"IsInBound"),
            "dialDuration" = COALESCE(p_dialDuration,"dialDuration"),
            "disconnectedBy" = COALESCE(p_disconnectedBy,"disconnectedBy"),
            "queueDuration" = COALESCE(p_queueDuration,"queueDuration"),
            "calledNumber" = COALESCE(p_calledNumber,"calledNumber"),
            "waitDuration" = COALESCE(p_waitDuration,"waitDuration"),
            "recordingTypeId" = COALESCE(p_recordingTypeId,"recordingTypeId"),
            "CustomerId" = COALESCE(p_CustomerId,"CustomerId"),
            "videoDuration" = COALESCE(p_videoDuration,"videoDuration"),
            "IsAbandoned" = COALESCE(p_IsAbandoned,"IsAbandoned"),
            "updatedAt" = v_curDate
        WHERE "VideoHistoryId" = v_VideoHistoryId;

        IF v_AgentId IS NOT NULL THEN

            IF EXISTS(
                SELECT 1 FROM "tb_VideoDurationHistory"
                WHERE "VideoHistoryId" = v_VideoHistoryId
                AND "AgentId" IS NULL
            ) THEN

                UPDATE "tb_VideoDurationHistory"
                SET
                    "StartTime" = COALESCE(p_StartTime,"StartTime"),
                    "EndTime" = COALESCE(p_EndTime,"EndTime"),
                    "agentDuration" = COALESCE(p_agentDuration,"agentDuration"),
                    "AgentId" = COALESCE(v_AgentId,"AgentId"),
                    "QueueId" = COALESCE(p_QueueId,"QueueId"),
                    "dispositionId" = COALESCE(p_dispositionId,"dispositionId"),
                    "UMNStatus" = COALESCE(p_UMNStatus,"UMNStatus"),
                    "transferredTime" = COALESCE(p_transferredTime,"transferredTime"),
                    "IsMissedCall" = COALESCE(p_IsMissedCall,"IsMissedCall"),
                    "dialDuration" = COALESCE(p_dialDuration,"dialDuration"),
                    "transferedBy" = COALESCE(v_transferedBy,"transferedBy"),
                    "transferedTo" = COALESCE(v_transferedTo,"transferedTo"),
                    "updatedAt" = v_curDate
                WHERE "VideoHistoryId" = v_VideoHistoryId
                AND "AgentId" IS NULL;

            ELSIF (p_UMNStatus IN (3,4)
                OR COALESCE(v_LastAgentId,0) <> v_AgentId
                OR v_LastUMNStatus IN (3,4)) THEN

                INSERT INTO "tb_VideoDurationHistory"(
                    "VideoHistoryId","StartTime","EndTime","agentDuration",
                    "AgentId","QueueId","dispositionId","UMNStatus",
                    "updatedAt","transferredTime","IsMissedCall",
                    "transferedBy","transferedTo","dialDuration"
                )
                VALUES(
                    v_VideoHistoryId,p_StartTime,p_EndTime,p_agentDuration,
                    v_AgentId,p_QueueId,p_dispositionId,p_UMNStatus,
                    v_curDate,p_transferredTime,p_IsMissedCall,
                    v_transferedBy,v_transferedTo,p_dialDuration
                )
                RETURNING "VideoDurationHistoryId"
                INTO v_VideoLastDurationHistoryId;

            ELSE

                UPDATE "tb_VideoDurationHistory"
                SET
                    "StartTime" = COALESCE(p_StartTime,"StartTime"),
                    "EndTime" = CASE WHEN p_EndTime = 0 THEN "EndTime" ELSE p_EndTime END,
                    "agentDuration" = COALESCE(p_agentDuration,"agentDuration"),
                    "AgentId" = COALESCE(v_AgentId,"AgentId"),
                    "QueueId" = COALESCE(p_QueueId,"QueueId"),
                    "dispositionId" = CASE WHEN p_dispositionId = 0 THEN "dispositionId" ELSE p_dispositionId END,
                    "UMNStatus" = COALESCE(p_UMNStatus,"UMNStatus"),
                    "transferredTime" = CASE WHEN p_transferredTime = 0 THEN "transferredTime" ELSE p_transferredTime END,
                    "IsMissedCall" = COALESCE(p_IsMissedCall,"IsMissedCall"),
                    "dialDuration" = COALESCE(p_dialDuration,"dialDuration"),
                    "transferedBy" = COALESCE(v_transferedBy,"transferedBy"),
                    "transferedTo" = COALESCE(v_transferedTo,"transferedTo"),
                    "updatedAt" = v_curDate
                WHERE "VideoDurationHistoryId" = v_LastVideoDurationHistoryId;

            END IF;

        END IF;

        p_errcode := 0;
        p_errmsg := 'updated successfully';

    ELSE

        INSERT INTO "tb_VideoHistory"(
            "sessionId","sessionStartTime","sessionEndTime","customerNumber",
            "domainId","IsInBound","dialDuration","disconnectedBy",
            "queueDuration","calledNumber","waitDuration","recordingTypeId",
            "CustomerId","videoDuration","IsAbandoned",
            "businessStartTime","businessEndTime","meetingId",
            "createdAt","updatedAt"
        )
        VALUES(
            p_sessionId,p_sessionStartTime,p_sessionEndTime,p_customerNumber,
            p_domainId,p_IsInBound,p_dialDuration,p_disconnectedBy,
            p_queueDuration,p_calledNumber,p_waitDuration,p_recordingTypeId,
            p_CustomerId,p_videoDuration,p_IsAbandoned,
            p_businessStartTime,p_businessEndTime,p_meetingId,
            v_curDate,v_curDate
        )
        RETURNING "VideoHistoryId" INTO v_VideoHistoryId;

        INSERT INTO "tb_VideoDurationHistory"(
            "VideoHistoryId","StartTime","EndTime","agentDuration",
            "AgentId","QueueId","dispositionId","UMNStatus",
            "createdAt","updatedAt","transferredTime",
            "IsMissedCall","transferedBy","transferedTo","dialDuration"
        )
        VALUES(
            v_VideoHistoryId,p_StartTime,p_EndTime,p_agentDuration,
            v_AgentId,p_QueueId,p_dispositionId,p_UMNStatus,
            v_curDate,v_curDate,p_transferredTime,
            p_IsMissedCall,v_transferedBy,v_transferedTo,p_dialDuration
        )
        RETURNING "VideoDurationHistoryId"
        INTO v_VideoDurationHistoryId;

        p_errcode := 0;
        p_errmsg := 'inserted successfully';

    END IF;

    IF p_transferedBy IS NOT NULL THEN

        SELECT "VideoDurationHistoryId"
        INTO v_SourceVideoDurationHistoryId
        FROM "tb_VideoDurationHistory"
        WHERE "VideoHistoryId" = v_VideoHistoryId
        AND "AgentId" = p_transferedBy
        LIMIT 1;

        IF v_SourceVideoDurationHistoryId IS NOT NULL
        AND v_VideoDurationHistoryId IS NOT NULL THEN
            CALL up_TransaferVideoMessage(
                v_SourceVideoDurationHistoryId,
                v_VideoDurationHistoryId
            );
        END IF;

    END IF;

EXCEPTION
WHEN OTHERS THEN
    p_errcode := -1;
    p_errmsg := 'Transaction failed';

END;
$$;


ALTER PROCEDURE public.ccaas_create_videomeet_session_history_details_logic(IN p_sessionid character varying, IN p_sessionstarttime bigint, IN p_sessionendtime bigint, IN p_customernumber character varying, IN p_domainid integer, IN p_isinbound smallint, IN p_dialduration bigint, IN p_disconnectedby bigint, IN p_queueduration bigint, IN p_callednumber bigint, IN p_waitduration bigint, IN p_recordingtypeid integer, IN p_customerid integer, IN p_videoduration bigint, IN p_isabandoned smallint, IN p_businessstarttime bigint, IN p_businessendtime bigint, IN p_meetingid text, IN p_agentid bigint, IN p_queueid integer, IN p_dispositionid integer, IN p_umnstatus integer, IN p_starttime bigint, IN p_endtime bigint, IN p_agentduration bigint, IN p_transferredtime bigint, IN p_ismissedcall smallint, IN p_transferedby integer, IN p_transferedto integer, INOUT p_errcode integer, INOUT p_errmsg character varying) OWNER TO postgres;

--
-- Name: ccaas_insert_vioce_mail_message(character varying, integer, text, character varying, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.ccaas_insert_vioce_mail_message(IN p_session_id character varying, IN p_domain_id integer, IN p_voice_mail_summary text, IN p_voice_mail_subject character varying, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_errcode INT := -1;
    v_errmsg VARCHAR(50) := 'URL mesg added failed';
    v_voice_mail_url_id BIGINT;
BEGIN

    SELECT "voice_mail_url_id"
    INTO v_voice_mail_url_id
    FROM public."tb_voice_mail_msg"
    WHERE "session_id" = p_session_id
      AND "domain_id" = p_domain_id;

    IF v_voice_mail_url_id IS NULL THEN
        INSERT INTO public."tb_voice_mail_msg"(
            "session_id", 
            "domain_id", 
            "voice_mail_summary", 
            "voice_mail_subject", 
            "created_at", 
            "updated_at"
        )
        VALUES (
            p_session_id, 
            p_domain_id, 
            p_voice_mail_summary, 
            p_voice_mail_subject, 
            NOW(), 
            NOW()
        );

        v_errcode := 0;
        v_errmsg := 'Inserted successfully';
    END IF;

    OPEN p_result FOR 
    SELECT v_errcode AS err_code, v_errmsg AS errmsg;

END;
$$;


ALTER PROCEDURE public.ccaas_insert_vioce_mail_message(IN p_session_id character varying, IN p_domain_id integer, IN p_voice_mail_summary text, IN p_voice_mail_subject character varying, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: ccaas_insert_voice_mail_message(character varying, integer, text, character varying, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.ccaas_insert_voice_mail_message(IN p_session_id character varying, IN p_domain_id integer, IN p_voice_mail_summary text, IN p_voice_mail_subject character varying, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_errcode INT := -1;
    v_errmsg VARCHAR(50) := 'URL mesg added failed';
    v_voice_mail_url_id BIGINT;
BEGIN

    SELECT "voice_mail_url_id"
    INTO v_voice_mail_url_id
    FROM public."tb_voice_mail_msg"
    WHERE "session_id" = p_session_id
      AND "domain_id" = p_domain_id;

    IF v_voice_mail_url_id IS NULL THEN
        INSERT INTO public."tb_voice_mail_msg"(
            "session_id", 
            "domain_id", 
            "voice_mail_summary", 
            "voice_mail_subject", 
            "created_at", 
            "updated_at"
        )
        VALUES (
            p_session_id, 
            p_domain_id, 
            p_voice_mail_summary, 
            p_voice_mail_subject, 
            NOW(), 
            NOW()
        );

        v_errcode := 0;
        v_errmsg := 'Inserted successfully';
    END IF;

    OPEN p_result FOR 
    SELECT v_errcode AS err_code, v_errmsg AS errmsg;

END;
$$;


ALTER PROCEDURE public.ccaas_insert_voice_mail_message(IN p_session_id character varying, IN p_domain_id integer, IN p_voice_mail_summary text, IN p_voice_mail_subject character varying, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: ccaas_updatevideosessiondisposition_details(integer, text, text, integer, integer, text, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.ccaas_updatevideosessiondisposition_details(IN p_domainid integer, IN p_meetingid text, IN p_summary text, IN p_dispositionid integer, IN p_aftervideoworktime integer, IN p_subject text, IN p_agentextn integer, INOUT p_result refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_errcode INT DEFAULT -1;
    v_errmsg VARCHAR(50) DEFAULT 'Error : Not Updated';
    v_VideoHistoryId INT;
    v_VideoDurationHistoryId INT;
BEGIN

    -- Get latest VideoHistoryId
    WITH CTE AS (
        SELECT "VideoHistoryId",
               ROW_NUMBER() OVER (PARTITION BY "meetingId" ORDER BY "updatedAt" DESC) AS rr
        FROM "tb_VideoHistory"
        WHERE "meetingId" = p_meetingId
        AND "domainId" = p_domainId
    )
    SELECT "VideoHistoryId"
    INTO v_VideoHistoryId
    FROM CTE
    WHERE rr = 1
    LIMIT 1;

    -- Get latest VideoDurationHistoryId
    SELECT "VideoDurationHistoryId"
    INTO v_VideoDurationHistoryId
    FROM "tb_VideoDurationHistory"
    WHERE "VideoHistoryId" = v_VideoHistoryId
    AND "AgentId" = p_agentExtn
    ORDER BY "VideoDurationHistoryId" DESC
    LIMIT 1;

    -- Check existence
    IF EXISTS (
        SELECT 1
        FROM "tb_VideoHistory"
        WHERE "domainId" = p_domainId
        AND "meetingId" = p_meetingId
        LIMIT 1
    ) THEN

        UPDATE "tb_VideoDurationHistory" VDH
        SET
            "summary" = COALESCE(p_summary, VDH."summary"),
            "dispositionId" = CASE
                                WHEN p_dispositionId IS NULL OR p_dispositionId = 0
                                THEN VDH."dispositionId"
                                ELSE p_dispositionId
                              END,
            "afterVideoWorkTime" = COALESCE(p_afterVideoWorkTime, VDH."afterVideoWorkTime"),
            "subject" = COALESCE(p_subject, VDH."subject"),
            "updatedAt" = CURRENT_TIMESTAMP
        FROM "tb_VideoHistory" VH
        WHERE VDH."VideoHistoryId" = VH."VideoHistoryId"
        AND VH."meetingId" = p_meetingId
        AND VDH."AgentId" = p_agentExtn
        AND VDH."VideoDurationHistoryId" = v_VideoDurationHistoryId;

    END IF;

    v_errcode := 0;
    v_errmsg := 'Success : Updated';

    OPEN p_result FOR
    SELECT v_errcode AS errcode,
           v_errmsg AS errmsg,
           v_VideoHistoryId AS "VideoHistoryId";

END;
$$;


ALTER PROCEDURE public.ccaas_updatevideosessiondisposition_details(IN p_domainid integer, IN p_meetingid text, IN p_summary text, IN p_dispositionid integer, IN p_aftervideoworktime integer, IN p_subject text, IN p_agentextn integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_GetCompanySessionHistoryDetails(integer, character varying, character varying, integer, integer, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public."crm_GetCompanySessionHistoryDetails"(IN "p_domainId" integer, IN "p_channelType" character varying, IN p_company_name character varying, IN "p_AgentId" integer, IN p_offset integer, IN p_limit integer, INOUT p_cursor refcursor DEFAULT 'p_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    "v_CurDate"         TIMESTAMP;
    "v_offset_val"      INT;
    "v_channelType"     VARCHAR(255);
BEGIN
    "v_CurDate"     := NOW();
    "v_offset_val"  := "p_offset" * "p_limit";

    -- Handle NULL or empty channelType
    "v_channelType" := COALESCE(NULLIF("p_channelType", ''), 'Call,SMS,Email');

    -- -------------------------------------------------------
    -- Temp table: split channel types
    -- -------------------------------------------------------
    DROP TABLE IF EXISTS "temp_channeltype";
    CREATE TEMP TABLE "temp_channeltype" AS
        SELECT TRIM("unnest_val") AS "channelType"
        FROM unnest(string_to_array("v_channelType", ',')) AS "unnest_val";

    -- -------------------------------------------------------
    -- Temp table: main history holder
    -- -------------------------------------------------------
    DROP TABLE IF EXISTS "temp_History";
    CREATE TEMP TABLE "temp_History" (
        "id_col"                SERIAL,
        "sessionId"             VARCHAR(250),
        "sessionStartTime"      BIGINT,
        "sessionEndTime"        BIGINT,
        "customerNumber"        VARCHAR(40),
        "companyId"             INT             DEFAULT NULL,
        "domainId"              INT             DEFAULT NULL,
        "dispositionId"         INT             DEFAULT NULL,
        "UMNStatus"             INT             DEFAULT NULL,
        "IsInBound"             SMALLINT        DEFAULT NULL,
        "calledNumber"          BIGINT          DEFAULT NULL,
        "afterCallWorkTime"     INT             DEFAULT NULL,
        "markAsRead"            SMALLINT        DEFAULT NULL,
        "CustomerId"            INT             DEFAULT NULL,
        "summary"               TEXT            DEFAULT NULL,
        "createdAt"             TIMESTAMP       DEFAULT NULL,
        "updatedAt"             TIMESTAMP       DEFAULT NULL,
        "FromEmail"             VARCHAR(100)    DEFAULT NULL,
        "ToEmail"               VARCHAR(100)    DEFAULT NULL,
        "EmailSubject"          VARCHAR(250)    DEFAULT NULL,
        "IsSessionClosed"       SMALLINT        DEFAULT NULL,
        "Duration"              BIGINT          DEFAULT NULL,
        "AgentId"               BIGINT          DEFAULT NULL,
        "channelType"           VARCHAR(255)    DEFAULT NULL,
        "EmailHistoryId"        INT             DEFAULT NULL,
        "CallHistoryId"         INT             DEFAULT NULL,
        "SMSHistoryId"          INT             	 DEFAULT NULL,
        "RecordingUrl"       	CHARACTER VARYING    DEFAULT NULL,
		"isOpened"				SMALLINT		DEFAULT 0
		
    );

    -- -------------------------------------------------------
    -- CALL channel
    -- -------------------------------------------------------
    IF EXISTS (SELECT 1 FROM "temp_channeltype" WHERE "channelType" = 'Call') THEN

        INSERT INTO "temp_History"
        (
            "sessionId", "sessionStartTime", "sessionEndTime", "customerNumber", "domainId",
            "dispositionId", "UMNStatus", "IsInBound", "calledNumber", "afterCallWorkTime",
            "markAsRead", "CustomerId", "summary", "createdAt", "updatedAt",
            "Duration", "AgentId", "channelType", "CallHistoryId","RecordingUrl"
        )
        SELECT
            "clh"."sessionId",
            "clh"."sessionStartTime",
            "clh"."sessionEndTime",
            "clh"."customerNumber",
            "clh"."domainId",
            "cld"."dispositionId",
            "cld"."UMNStatus",
            "clh"."IsInBound",
            "clh"."calledNumber",
            "cld"."afterCallWorkTime",
            "cld"."markAsRead",
            "clh"."CustomerId",
            "cld"."summary",
            "clh"."createdAt",
            "cld"."updatedAt",
            COALESCE("cld"."Duration", 0),
            "cld"."AgentId",
            'Call',
            "clh"."CallHistoryId",
			"cld"."RecordingUrl"
        FROM    "tb_CallHistory"            "clh"
        JOIN    "tb_CallDurationHistory"    "cld" ON "clh"."CallHistoryId" = "cld"."CallHistoryId"
		JOIN 	"tb_customer_contact"		"cc"  ON "cc"."primary_phone_number" = "clh"."customerNumber"
        WHERE   "clh"."domainId"            = "p_domainId"
        AND     ("p_AgentId"                IS NULL OR "cld"."AgentId"         = "p_AgentId")
		AND 	"cc"."company_name" = "p_company_name"
        ORDER BY "clh"."domainId", "clh"."updatedAt" DESC;

    END IF;

    -- -------------------------------------------------------
    -- EMAIL channel
    -- -------------------------------------------------------
    IF EXISTS (SELECT 1 FROM "temp_channeltype" WHERE "channelType" = 'Email') THEN

        INSERT INTO "temp_History"
        (
            "sessionId", "sessionStartTime", "sessionEndTime", "companyId", "domainId",
            "dispositionId", "IsInBound", "CustomerId", "summary", "createdAt", "updatedAt",
            "AgentId", "channelType", "EmailHistoryId", "FromEmail", "ToEmail", "EmailSubject", "IsSessionClosed","isOpened"
        )
        SELECT
            "eh"."SessionId",
            "eh"."SessionStartTime",
            "eh"."SessionEndTime",
            "eh"."CompanyId",
            "eh"."DomainId",
            "eh"."DispositionId",
            "eh"."IsInBound",
            "eh"."CustomerId",
            "eh"."Summary",
            "eh"."CreatedAt",
            "eh"."UpdatedAt",
            "eh"."AgentId",
            'Email',
            "eh"."EmailHistoryId",
            "eh"."FromEmail",
            "eh"."ToEmail",
            "eh"."EmailSubject",
            "eh"."IsSessionClosed"
			,"eh"."isOpened"
		FROM    "tb_EmailHistory" "eh"
		JOIN 	"tb_customer_contact"		"cc"  ON "cc"."primary_email" = "eh"."FromEmail"
        WHERE   "eh"."DomainId"             = "p_domainId"
		AND 	"cc"."company_name" 		= "p_company_name"
        AND     ("p_AgentId"                IS NULL OR "eh"."AgentId"    = "p_AgentId")
        ORDER BY "eh"."UpdatedAt" DESC;

    END IF;

    -- -------------------------------------------------------
    -- Open REFCURSOR with final paginated result
    -- -------------------------------------------------------
    OPEN "p_cursor" FOR
    SELECT
        "TH"."sessionId",
        "TH"."sessionStartTime",
        "TH"."sessionEndTime",
        "TH"."customerNumber",
        "TH"."companyId",
        "TH"."domainId",
        "TH"."dispositionId",
        "TH"."UMNStatus",
        "TH"."IsInBound",
        "TH"."calledNumber",
        "TH"."afterCallWorkTime",
        "TH"."markAsRead",
        "TH"."CustomerId",
        "TH"."summary",
        "TH"."createdAt",
        "TH"."updatedAt",
        "TH"."FromEmail",
        "TH"."ToEmail",
        "TH"."EmailSubject",
        "TH"."IsSessionClosed",
        "TH"."Duration",
        "TH"."AgentId",
        "TH"."channelType",
        "TH"."EmailHistoryId",
        "TH"."CallHistoryId",
        "TH"."SMSHistoryId",
        "ds"."dispositionName",
        "U"."UserName" AS "agent_name",
		"TH"."RecordingUrl",
		"TH"."isOpened"
    FROM "temp_History" 				"TH"
    LEFT JOIN "tb_User" 				"U"   ON "TH"."AgentId" = "U"."UserID" AND "U"."domainId" = "TH"."domainId"
	LEFT JOIN "disposition_status"      "ds"  ON "ds"."did" = "TH"."dispositionId"
    ORDER BY "TH"."updatedAt" DESC
    LIMIT "p_limit" OFFSET "v_offset_val";

END;
$$;


ALTER PROCEDURE public."crm_GetCompanySessionHistoryDetails"(IN "p_domainId" integer, IN "p_channelType" character varying, IN p_company_name character varying, IN "p_AgentId" integer, IN p_offset integer, IN p_limit integer, INOUT p_cursor refcursor) OWNER TO postgres;

--
-- Name: crm_GetInboxSessionHistoryDetails(integer, character varying, integer, smallint, smallint, integer, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public."crm_GetInboxSessionHistoryDetails"(IN "p_domainId" integer, IN "p_channelType" character varying, IN "p_AgentId" integer, IN p_is_trash smallint, IN p_is_sent smallint, IN p_offset integer, IN p_limit integer, INOUT p_cursor refcursor DEFAULT 'p_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    "v_CurDate"         TIMESTAMP;
    "v_offset_val"      INT;
    "v_channelType"     VARCHAR(255);
BEGIN
    "v_CurDate"     := NOW();
    "v_offset_val"  := "p_offset" * "p_limit";

    -- Handle NULL or empty channelType
    "v_channelType" := COALESCE(NULLIF("p_channelType", ''), 'Call,SMS,Email');

    -- -------------------------------------------------------
    -- Temp table: split channel types
    -- -------------------------------------------------------
    DROP TABLE IF EXISTS "temp_channeltype";
    CREATE TEMP TABLE "temp_channeltype" AS
        SELECT TRIM("unnest_val") AS "channelType"
        FROM unnest(string_to_array("v_channelType", ',')) AS "unnest_val";

    -- -------------------------------------------------------
    -- Temp table: main history holder
    -- -------------------------------------------------------
    DROP TABLE IF EXISTS "temp_History";
    CREATE TEMP TABLE "temp_History" (
        "id_col"                SERIAL,
        "sessionId"             VARCHAR(250),
        "sessionStartTime"      BIGINT,
        "sessionEndTime"        BIGINT,
        "customerNumber"        VARCHAR(40),
        "companyId"             INT             DEFAULT NULL,
        "domainId"              INT             DEFAULT NULL,
        "dispositionId"         INT             DEFAULT NULL,
        "UMNStatus"             INT             DEFAULT NULL,
        "IsInBound"             SMALLINT        DEFAULT NULL,
        "calledNumber"          BIGINT          DEFAULT NULL,
        "afterCallWorkTime"     INT             DEFAULT NULL,
        "markAsRead"            SMALLINT        DEFAULT NULL,
        "CustomerId"            INT             DEFAULT NULL,
        "summary"               TEXT            DEFAULT NULL,
        "createdAt"             TIMESTAMP       DEFAULT NULL,
        "updatedAt"             TIMESTAMP       DEFAULT NULL,
        "FromEmail"             VARCHAR(100)    DEFAULT NULL,
        "ToEmail"               VARCHAR(100)    DEFAULT NULL,
        "EmailSubject"          VARCHAR(250)    DEFAULT NULL,
        "IsSessionClosed"       SMALLINT        DEFAULT NULL,
        "Duration"              BIGINT          DEFAULT NULL,
        "AgentId"               BIGINT          DEFAULT NULL,
        "channelType"           VARCHAR(255)    DEFAULT NULL,
        "EmailHistoryId"        INT             DEFAULT NULL,
        "CallHistoryId"         INT             DEFAULT NULL,
        "SMSHistoryId"          INT             	 DEFAULT NULL,
        "RecordingUrl"       	CHARACTER VARYING    DEFAULT NULL,
		"isOpened"				SMALLINT		DEFAULT 0,
		"customer_name"			VARCHAR(250)
		
    );

    -- -------------------------------------------------------
    -- CALL channel
    -- -------------------------------------------------------
    IF EXISTS (SELECT 1 FROM "temp_channeltype" WHERE "channelType" = 'Call') THEN

        INSERT INTO "temp_History"
        (
            "sessionId", "sessionStartTime", "sessionEndTime", "customerNumber", "domainId",
            "dispositionId", "UMNStatus", "IsInBound", "calledNumber", "afterCallWorkTime",
            "markAsRead", "CustomerId", "summary", "createdAt", "updatedAt",
            "Duration", "AgentId", "channelType", "CallHistoryId","RecordingUrl","customer_name"
        )
        SELECT
            "clh"."sessionId",
            "clh"."sessionStartTime",
            "clh"."sessionEndTime",
            "clh"."customerNumber",
            "clh"."domainId",
            "cld"."dispositionId",
            "cld"."UMNStatus",
            "clh"."IsInBound",
            "clh"."calledNumber",
            "cld"."afterCallWorkTime",
            "cld"."markAsRead",
            "clh"."CustomerId",
            "cld"."summary",
            "clh"."createdAt",
            "cld"."updatedAt",
            COALESCE("cld"."Duration", 0),
            "cld"."AgentId",
            'Call',
            "clh"."CallHistoryId",
			"cld"."RecordingUrl",
			CONCAT_WS(' ', "cp"."first_name", "cp"."last_name")
        FROM    "tb_CallHistory"            "clh"
        JOIN    "tb_CallDurationHistory"    "cld" ON "clh"."CallHistoryId" = "cld"."CallHistoryId"
		LEFT JOIN "tb_customer_contact"		"cp"  ON "cp"."primary_phone_number" = "clh"."customerNumber" 
        WHERE   "clh"."domainId"            = "p_domainId"
        AND     ("p_AgentId"                IS NULL OR "cld"."AgentId"         = "p_AgentId")
        ORDER BY "clh"."domainId", "clh"."updatedAt" DESC;

    END IF;

    -- -------------------------------------------------------
    -- EMAIL channel
    -- -------------------------------------------------------
    IF EXISTS (SELECT 1 FROM "temp_channeltype" WHERE "channelType" = 'Email') THEN

        INSERT INTO "temp_History"
        (
            "sessionId", "sessionStartTime", "sessionEndTime", "companyId", "domainId",
            "dispositionId", "IsInBound", "CustomerId", "summary", "createdAt", "updatedAt",
            "AgentId", "channelType", "EmailHistoryId", "FromEmail", "ToEmail", "EmailSubject", "IsSessionClosed","isOpened","customer_name"
        )
        SELECT
            "eh"."SessionId",
            "eh"."SessionStartTime",
            "eh"."SessionEndTime",
            "eh"."CompanyId",
            "eh"."DomainId",
            "eh"."DispositionId",
            "eh"."IsInBound",
            "eh"."CustomerId",
            "eh"."Summary",
            "eh"."CreatedAt",
            "eh"."UpdatedAt",
            "eh"."AgentId",
            'Email',
            "eh"."EmailHistoryId",
            "eh"."FromEmail",
            "eh"."ToEmail",
            "eh"."EmailSubject",
            "eh"."IsSessionClosed"
			,"eh"."isOpened",
			CONCAT_WS(' ', "ce"."first_name", "ce"."last_name")
		FROM    "tb_EmailHistory" "eh"
		LEFT JOIN "tb_customer_contact"		"ce"  ON "ce"."primary_email" = "eh"."FromEmail" 
        WHERE   "eh"."DomainId"             = "p_domainId"
        AND     ("p_AgentId"                IS NULL OR "eh"."AgentId" = "p_AgentId")
		AND 	("eh"."is_trash" = p_is_trash)
		AND 	("eh"."IsInBound" = p_is_sent)
        ORDER BY "eh"."UpdatedAt" DESC;

    END IF;

    -- -------------------------------------------------------
    -- Open REFCURSOR with final paginated result
    -- -------------------------------------------------------
    OPEN "p_cursor" FOR
    SELECT
        "TH"."sessionId",
        "TH"."sessionStartTime",
        "TH"."sessionEndTime",
        "TH"."customerNumber",
        "TH"."companyId",
        "TH"."domainId",
        "TH"."dispositionId",
        "TH"."UMNStatus",
        "TH"."IsInBound",
        "TH"."calledNumber",
        "TH"."afterCallWorkTime",
        "TH"."markAsRead",
        "TH"."CustomerId",
        "TH"."summary",
        "TH"."createdAt",
        "TH"."updatedAt",
        "TH"."FromEmail",
        "TH"."ToEmail",
        "TH"."EmailSubject",
        "TH"."IsSessionClosed",
        "TH"."Duration",
        "TH"."AgentId",
        "TH"."channelType",
        "TH"."EmailHistoryId",
        "TH"."CallHistoryId",
        "TH"."SMSHistoryId",
        "ds"."dispositionName",
        "U"."UserName" AS "agent_name",
		"TH"."RecordingUrl",
		"TH"."isOpened",
		"customer_name"
    FROM "temp_History" 				"TH"
    LEFT JOIN "tb_User" 				"U"   ON "TH"."AgentId" = "U"."UserID" AND "U"."domainId" = "TH"."domainId"
	LEFT JOIN "disposition_status"      "ds"  ON "ds"."did" = "TH"."dispositionId"
    ORDER BY "TH"."updatedAt" DESC
    LIMIT "p_limit" OFFSET "v_offset_val";

END;
$$;


ALTER PROCEDURE public."crm_GetInboxSessionHistoryDetails"(IN "p_domainId" integer, IN "p_channelType" character varying, IN "p_AgentId" integer, IN p_is_trash smallint, IN p_is_sent smallint, IN p_offset integer, IN p_limit integer, INOUT p_cursor refcursor) OWNER TO postgres;

--
-- Name: crm_GetSessionHistoryDetails(integer, character varying, integer, character varying, character varying, integer, integer, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public."crm_GetSessionHistoryDetails"(IN "p_domainId" integer, IN "p_channelType" character varying, IN "p_AgentId" integer, IN p_customer_number character varying, IN p_customer_email character varying, IN p_customer_id integer, IN p_offset integer, IN p_limit integer, INOUT p_cursor refcursor DEFAULT 'p_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    "v_CurDate"         TIMESTAMP;
    "v_offset_val"      INT;
    "v_channelType"     VARCHAR(255);
BEGIN
    "v_CurDate"     := NOW();
    "v_offset_val"  := "p_offset" * "p_limit";

    -- Handle NULL or empty channelType
    "v_channelType" := COALESCE(NULLIF("p_channelType", ''), 'Call,SMS,Email,Notes,Video');

    -- -------------------------------------------------------
    -- Temp table: split channel types
    -- -------------------------------------------------------
    DROP TABLE IF EXISTS "temp_channeltype";
    CREATE TEMP TABLE "temp_channeltype" AS
        SELECT TRIM("unnest_val") AS "channelType"
        FROM unnest(string_to_array("v_channelType", ',')) AS "unnest_val";

    -- -------------------------------------------------------
    -- Temp table: main history holder
    -- -------------------------------------------------------
    DROP TABLE IF EXISTS "temp_History";
    CREATE TEMP TABLE "temp_History" (
        "id_col"                SERIAL,
        "sessionId"             VARCHAR(250),
        "sessionStartTime"      BIGINT,
        "sessionEndTime"        BIGINT,
        "customerNumber"        VARCHAR(40),
        "companyId"             INT             DEFAULT NULL,
        "domainId"              INT             DEFAULT NULL,
        "dispositionId"         INT             DEFAULT NULL,
        "UMNStatus"             INT             DEFAULT NULL,
        "IsInBound"             SMALLINT        DEFAULT NULL,
        "calledNumber"          BIGINT          DEFAULT NULL,
        "afterCallWorkTime"     INT             DEFAULT NULL,
        "markAsRead"            SMALLINT        DEFAULT NULL,
        "CustomerId"            INT             DEFAULT NULL,
        "summary"               TEXT            DEFAULT NULL,
        "createdAt"             TIMESTAMP       DEFAULT NULL,
        "updatedAt"             TIMESTAMP       DEFAULT NULL,
        "FromEmail"             VARCHAR(100)    DEFAULT NULL,
        "ToEmail"               VARCHAR(100)    DEFAULT NULL,
        "EmailSubject"          VARCHAR(250)    DEFAULT NULL,
        "IsSessionClosed"       SMALLINT        DEFAULT NULL,
        "Duration"              BIGINT          DEFAULT NULL,
        "AgentId"               BIGINT          DEFAULT NULL,
        "channelType"           VARCHAR(255)    DEFAULT NULL,
        "EmailHistoryId"        INT             DEFAULT NULL,
        "CallHistoryId"         INT             DEFAULT NULL,
        "SMSHistoryId"          INT             	 DEFAULT NULL,
		"VideoHistoryId"        INT             	 DEFAULT NULL,
        "RecordingUrl"       	CHARACTER VARYING    DEFAULT NULL,
		"isOpened"				SMALLINT			 DEFAULT 0,
		"customer_name"			VARCHAR(250),
		"Notes"					TEXT,
		"imageurl"				TEXT,
		"meetingId"				VARCHAR(250),
		tags 					CHARACTER VARYING(100),
		schedule_start 			timestamp,
		schedule_end 			timestamp,
		meeting_title 			character varying(2000),
		created_by				varchar(500),
		meet_url				varchar(2500)
    );

    -- -------------------------------------------------------
    -- CALL channel
    -- -------------------------------------------------------
    IF EXISTS (SELECT 1 FROM "temp_channeltype" WHERE "channelType" = 'Call') THEN

        INSERT INTO "temp_History"
        (
            "sessionId", "sessionStartTime", "sessionEndTime", "customerNumber", "domainId",
            "dispositionId", "UMNStatus", "IsInBound", "calledNumber", "afterCallWorkTime",
            "markAsRead", "CustomerId", "summary", "createdAt", "updatedAt",
            "Duration", "AgentId", "channelType", "CallHistoryId","RecordingUrl","customer_name"
        )
        SELECT
            "clh"."sessionId",
            "clh"."sessionStartTime",
            "clh"."sessionEndTime",
            "clh"."customerNumber",
            "clh"."domainId",
            "cld"."dispositionId",
            "cld"."UMNStatus",
            "clh"."IsInBound",
            "clh"."calledNumber",
            "cld"."afterCallWorkTime",
            "cld"."markAsRead",
            "clh"."CustomerId",
            "cld"."summary",
            "clh"."createdAt",
            "cld"."updatedAt",
            COALESCE("cld"."Duration", 0),
            "cld"."AgentId",
            'Call',
            "clh"."CallHistoryId",
			"cld"."RecordingUrl",
			CONCAT_WS(' ', "cp"."first_name", "cp"."last_name")
        FROM    "tb_CallHistory"            "clh"
        JOIN    "tb_CallDurationHistory"    "cld" ON "clh"."CallHistoryId" = "cld"."CallHistoryId"
		LEFT JOIN "tb_customer_contact"		"cp"  ON "cp"."primary_phone_number" = "clh"."customerNumber" 
        WHERE   "clh"."domainId"            = "p_domainId"
        AND     ("p_AgentId"                IS NULL OR "cld"."AgentId"         = "p_AgentId")
        AND     ("clh"."customerNumber"  = "p_customer_number")
        ORDER BY "clh"."domainId", "clh"."updatedAt" DESC;

    END IF;

    -- -------------------------------------------------------
    -- EMAIL channel
    -- -------------------------------------------------------
    IF EXISTS (SELECT 1 FROM "temp_channeltype" WHERE "channelType" = 'Email') THEN

        INSERT INTO "temp_History"
        (
            "sessionId", "sessionStartTime", "sessionEndTime", "companyId", "domainId",
            "dispositionId", "IsInBound", "CustomerId", "summary", "createdAt", "updatedAt",
            "AgentId", "channelType", "EmailHistoryId", "FromEmail", "ToEmail", "EmailSubject", "IsSessionClosed","isOpened","customer_name"
        )
        SELECT
            "eh"."SessionId",
            "eh"."SessionStartTime",
            "eh"."SessionEndTime",
            "eh"."CompanyId",
            "eh"."DomainId",
            "eh"."DispositionId",
            "eh"."IsInBound",
            "eh"."CustomerId",
            "eh"."Summary",
            "eh"."CreatedAt",
            "eh"."UpdatedAt",
            "eh"."AgentId",
            'Email',
            "eh"."EmailHistoryId",
            "eh"."FromEmail",
            "eh"."ToEmail",
            "eh"."EmailSubject",
            "eh"."IsSessionClosed",
			"eh"."isOpened",
			CONCAT_WS(' ', "ce"."first_name", "ce"."last_name")
        FROM    "tb_EmailHistory" "eh"
		LEFT JOIN "tb_customer_contact"		"ce"  ON "ce"."primary_email" = "eh"."FromEmail"
        WHERE   "eh"."DomainId"             = "p_domainId"
        AND     ("p_AgentId"                IS NULL OR "eh"."AgentId"    = "p_AgentId")
        AND     ("eh"."FromEmail"  = "p_customer_email" OR "eh"."ToEmail" = "p_customer_email")
        ORDER BY "eh"."UpdatedAt" DESC;

    END IF;

	 -- -------------------------------------------------------
    -- VIDEO channel
    -- -------------------------------------------------------
    IF EXISTS (SELECT 1 FROM "temp_channeltype" WHERE "channelType" = 'Video') THEN

	INSERT INTO "temp_History"
	(
		"sessionId",
		"sessionStartTime",
		"sessionEndTime",
		"domainId",
		"dispositionId",
		"IsInBound",
		"CustomerId",
		"summary",
		"createdAt",
		"updatedAt",
		"AgentId",
		"channelType",
		"VideoHistoryId",
		"meetingId",
		tags,
		schedule_start,
		schedule_end,
		meeting_title,
		created_by,
		meet_url
	)
	SELECT
		vh."sessionId",
		vh."sessionStartTime",
		vh."sessionEndTime",
		vh."domainId",
		vdh."dispositionId",
		vh."IsInBound",
		vh."CustomerId",
		vh.meeting_description,
		vh."createdAt",
		vh."updatedAt",
		vdh."AgentId",
		'Video',
		vh."VideoHistoryId",
		vh."meetingId",
		vh.tags,
		vh.schedule_start,
		vh.schedule_end,
		vh.meeting_title,
		u."UserName",
		vh.meet_url
	FROM "tb_VideoHistory" vh
	JOIN "tb_VideoDurationHistory" vdh    ON vh."VideoHistoryId" = vdh."VideoHistoryId"
	LEFT JOIN "tb_User" u    ON u."UserID" = vh.created_by
	WHERE vh."domainId" = "p_domainId"
	AND ("p_AgentId" IS NULL OR vdh."AgentId" = "p_AgentId")
	AND vh."CustomerId" = p_customer_id
	ORDER BY vh."updatedAt" DESC;

    END IF;

	-- -------------------------------------------------------
    -- Notes
    -- -------------------------------------------------------

	IF EXISTS (SELECT 1 FROM "temp_channeltype" WHERE "channelType" = 'Notes') THEN
	INSERT INTO "temp_History"
        (
            "sessionId", "domainId", "CustomerId", "Notes", "createdAt", "updatedAt",
            "AgentId", "channelType","imageurl"
        )
        SELECT
            "n"."sessionId",
            "n"."domainId",
            "n"."coid",
            "n"."notes",
            "n"."createdAt",
            "n"."updatedAt",
            "n"."agentId",
            'Notes',
			"imageurl"
        FROM  "tb_interaction_notes_info" "n"
        WHERE   "n"."domainId"             = "p_domainId"
        AND     ("p_AgentId" IS NULL OR "n"."agentId"    = "p_AgentId")
		AND 	("n"."coid" = p_customer_id OR p_customer_id IS NULL)
        ORDER BY "n"."updatedAt" DESC;
	END IF;

    -- -------------------------------------------------------
    -- Open REFCURSOR with final paginated result
    -- -------------------------------------------------------
    OPEN "p_cursor" FOR
    SELECT
        "TH"."sessionId",
        "TH"."sessionStartTime",
        "TH"."sessionEndTime",
        "TH"."customerNumber",
        "TH"."companyId",
        "TH"."domainId",
        "TH"."dispositionId",
        "TH"."UMNStatus",
        "TH"."IsInBound",
        "TH"."calledNumber",
        "TH"."afterCallWorkTime",
        "TH"."markAsRead",
        "TH"."CustomerId",
        "TH"."summary",
        "TH"."createdAt",
        "TH"."updatedAt",
        "TH"."FromEmail",
        "TH"."ToEmail",
        "TH"."EmailSubject",
        "TH"."IsSessionClosed",
        "TH"."Duration",
        "TH"."AgentId",
        "TH"."channelType",
        "TH"."EmailHistoryId",
        "TH"."CallHistoryId",
        "TH"."SMSHistoryId",
        "ds"."dispositionName",
        "U"."UserName" AS "agent_name",
		"TH"."RecordingUrl",
		"TH"."isOpened",
		"customer_name",
		"TH"."Notes",
		"imageurl",
		"meetingId",
		tags,
		schedule_start,
		schedule_end,
		meeting_title,
		created_by,
		meet_url,
		"VideoHistoryId"
    FROM "temp_History" 				"TH"
    LEFT JOIN "tb_User" 				"U"   ON "TH"."AgentId" = "U"."UserID" AND "U"."domainId" = "TH"."domainId"
	LEFT JOIN "disposition_status"      "ds"  ON "ds"."did" = "TH"."dispositionId"
    ORDER BY "TH"."updatedAt" DESC
    LIMIT "p_limit" OFFSET "v_offset_val";

END;
$$;


ALTER PROCEDURE public."crm_GetSessionHistoryDetails"(IN "p_domainId" integer, IN "p_channelType" character varying, IN "p_AgentId" integer, IN p_customer_number character varying, IN p_customer_email character varying, IN p_customer_id integer, IN p_offset integer, IN p_limit integer, INOUT p_cursor refcursor) OWNER TO postgres;

--
-- Name: crm_InsertUpdate_did_purchase(integer, integer, character varying, character varying, character varying, bigint, character varying, integer, integer, integer, smallint, smallint, character varying, smallint, integer, character varying, integer, character varying, smallint, smallint, smallint, smallint, character varying, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public."crm_InsertUpdate_did_purchase"(IN p_pid integer, IN p_user_id integer, IN p_city character varying, IN p_country character varying, IN p_your_number character varying, IN p_domainid bigint, IN p_callflowid character varying, IN p_recording integer, IN p_status integer, IN p_queueid integer, IN p_iscall smallint, IN p_issms smallint, IN p_aliasname character varying, IN p_outboundsms smallint, IN p_smsthreshold integer, IN p_callflowname character varying, IN p_defaultupdated integer, IN p_supervisorname character varying, IN p_iscallin smallint, IN p_iscallout smallint, IN p_issmsout smallint, IN p_issmsin smallint, IN p_type character varying, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_pid INTEGER;
    v_errcode INTEGER := -1;
    v_errmsg VARCHAR(150) := 'No update in Did Purchase or pid should not be zero while updating';
    v_CurDate TIMESTAMP;
    v_queueName VARCHAR(250);
    v_row_count INTEGER;
BEGIN
    v_CurDate := NOW();

    -- Get queue name
    SELECT name INTO v_queueName
    FROM queue
    WHERE qid = p_queueId
      AND "domainId" = p_domainId;

    -- Check if pid exists
    SELECT pid INTO v_pid
    FROM did_purchase
    WHERE pid = p_pid;

    -- If not found by pid, try by your_number
    IF v_pid IS NULL THEN
        SELECT pid INTO v_pid
        FROM did_purchase
        WHERE your_number = p_your_number
          AND "domainId" = p_domainId
        LIMIT 1;
    END IF;

    IF v_pid IS NULL THEN
        -- Insert new did_purchase
        INSERT INTO did_purchase(
            user_id, city, country, your_number, "domainId",
            "callFlowId", recording, status, "queueId", "queueName",
            "isCall", "isSms", "aliasName", "outboundSms", "smsThreshold",
            "callFlowName", "defaultUpdated", "supervisorName",
            "isCallIn", "isCallOut", "isSmsOut", "isSmsIn",
            "createdAt", "updatedAt", type
        )
        VALUES(
            p_user_id, p_city, p_country, p_your_number, p_domainId,
            p_callFlowId, p_recording, p_status, p_queueId, v_queueName,
            p_isCall, p_isSms, p_aliasName, p_outboundSms, p_smsThreshold,
            p_callFlowName, p_defaultUpdated, p_supervisorName,
            p_isCallIn, p_isCallOut, p_isSmsOut, p_isSmsIn,
            v_CurDate, v_CurDate, p_type
        )
        RETURNING pid INTO v_pid;

        GET DIAGNOSTICS v_row_count = ROW_COUNT;

        IF v_row_count > 0 THEN
            v_errcode := 0;
            v_errmsg := 'Did Purchase Inserted Successfully';
        END IF;
    ELSE
        -- Update existing did_purchase
        UPDATE did_purchase
        SET user_id = COALESCE(p_user_id, user_id),
            city = COALESCE(p_city, city),
            country = COALESCE(p_country, country),
            "callFlowId" = COALESCE(p_callFlowId, "callFlowId"),
            recording = COALESCE(p_recording, recording),
            status = COALESCE(p_status, status),
            "queueId" = COALESCE(p_queueId, "queueId"),
            "queueName" = COALESCE(v_queueName, "queueName"),
            "isCall" = COALESCE(p_isCall, "isCall"),
            "isSms" = COALESCE(p_isSms, "isSms"),
            "aliasName" = COALESCE(p_aliasName, "aliasName"),
            "outboundSms" = COALESCE(p_outboundSms, "outboundSms"),
            "smsThreshold" = COALESCE(p_smsThreshold, "smsThreshold"),
            "callFlowName" = COALESCE(p_callFlowName, "callFlowName"),
            "defaultUpdated" = COALESCE(p_defaultUpdated, "defaultUpdated"),
            "supervisorName" = COALESCE(p_supervisorName, "supervisorName"),
            "isCallIn" = COALESCE(p_isCallIn, "isCallIn"),
            "isCallOut" = COALESCE(p_isCallOut, "isCallOut"),
            "isSmsOut" = COALESCE(p_isSmsOut, "isSmsOut"),
            "isSmsIn" = COALESCE(p_isSmsIn, "isSmsIn"),
            "updatedAt" = v_CurDate,
            type = COALESCE(p_type, type)
        WHERE pid = v_pid;

        GET DIAGNOSTICS v_row_count = ROW_COUNT;

        IF v_row_count > 0 THEN
            v_errcode := 0;
            v_errmsg := 'Did Purchase Updated Successfully';
        END IF;
    END IF;

    OPEN p_result FOR SELECT v_errcode AS errcode, v_errmsg AS errmsg;
END;
$$;


ALTER PROCEDURE public."crm_InsertUpdate_did_purchase"(IN p_pid integer, IN p_user_id integer, IN p_city character varying, IN p_country character varying, IN p_your_number character varying, IN p_domainid bigint, IN p_callflowid character varying, IN p_recording integer, IN p_status integer, IN p_queueid integer, IN p_iscall smallint, IN p_issms smallint, IN p_aliasname character varying, IN p_outboundsms smallint, IN p_smsthreshold integer, IN p_callflowname character varying, IN p_defaultupdated integer, IN p_supervisorname character varying, IN p_iscallin smallint, IN p_iscallout smallint, IN p_issmsout smallint, IN p_issmsin smallint, IN p_type character varying, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_call_create_message_details_bot(integer, character varying, jsonb, integer, smallint); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_call_create_message_details_bot(IN p_domainid integer, IN p_sessionid character varying, IN p_message jsonb, IN p_agentextn integer, IN p_isbot smallint)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_errcode INT;
    v_errmsg TEXT;
    v_AgentId BIGINT;
    v_CallHistoryId BIGINT;
    v_CallDurationHistoryId BIGINT;
    v_prev_Message JSONB;
    v_merged_msg JSONB;
    v_rowcount INT;
BEGIN

    -- Validation block
    IF p_domainId IS NULL OR p_domainId = 0 THEN
        RAISE EXCEPTION 'DomainId should not be empty';
    END IF;

    IF ((p_agentextn IS NULL OR p_agentextn = 0) AND p_isBot = 0) THEN
        RAISE EXCEPTION 'Agentextn should not be empty or null';
    END IF;

    -- Fetch AgentId
    IF p_agentextn IS NOT NULL THEN
        SELECT "UserID"
        INTO v_AgentId
        FROM "tb_User"
        WHERE "domainId" = p_domainId
          AND "ext" = p_agentextn
        LIMIT 1;
    END IF;

    -- Check CallHistory existence
    IF NOT EXISTS (
        SELECT 1
        FROM "tb_CallHistory"
        WHERE LOWER("sessionId") = LOWER(p_sessionId)
    ) THEN
        RAISE EXCEPTION 'Call details not available';
    END IF;

    -- Get CallHistoryId
    SELECT "CallHistoryId"
    INTO v_CallHistoryId
    FROM "tb_CallHistory"
    WHERE LOWER("sessionId") = LOWER(p_sessionId)
    LIMIT 1;

    -- Get CallDurationHistoryId
    IF v_AgentId IS NOT NULL THEN
        SELECT "CallDurationHistoryId"
        INTO v_CallDurationHistoryId
        FROM "tb_CallDurationHistory"
        WHERE "CallHistoryId" = v_CallHistoryId
          AND "AgentId" = v_AgentId
        LIMIT 1;

    ELSIF p_isBot = 1 THEN
        SELECT "CallDurationHistoryId"
        INTO v_CallDurationHistoryId
        FROM "tb_CallDurationHistory"
        WHERE "CallHistoryId" = v_CallHistoryId
          AND "IsBot" = p_isBot
        LIMIT 1;
    END IF;

    IF v_CallDurationHistoryId IS NULL THEN
        RAISE EXCEPTION 'Given Agent did not attend this call';
    END IF;

    -- Insert / Update logic
    IF NOT EXISTS (
        SELECT 1
        FROM "tb_CallMessage"
        WHERE "CallHistoryId" = v_CallHistoryId
    ) THEN

        INSERT INTO "tb_CallMessage"
        ("CallHistoryId", "Message", "createdAt", "CallDurationHistoryId")
        VALUES
        (v_CallHistoryId, p_Message, NOW(), v_CallDurationHistoryId);

    ELSIF EXISTS (
        SELECT 1
        FROM "tb_CallMessage"
        WHERE "CallHistoryId" = v_CallHistoryId
          AND "CallDurationHistoryId" = v_CallDurationHistoryId
    ) THEN

        UPDATE "tb_CallMessage"
        SET "Message" =
                CASE
                    WHEN "Message" IS NULL
                    THEN p_Message
                    ELSE "Message" || jsonb_build_array(p_Message)
                END,
            "updatedAt" = NOW()
        WHERE "CallHistoryId" = v_CallHistoryId
          AND "CallDurationHistoryId" = v_CallDurationHistoryId;

    ELSE

        SELECT "Message"
        INTO v_prev_Message
        FROM "tb_CallMessage"
        WHERE "CallHistoryId" = v_CallHistoryId
        ORDER BY COALESCE("updatedAt","createdAt") DESC
        LIMIT 1;

        v_merged_msg := v_prev_Message || jsonb_build_array(p_Message);

        INSERT INTO "tb_CallMessage"
        ("CallHistoryId", "Message", "createdAt", "CallDurationHistoryId")
        VALUES
        (v_CallHistoryId, v_merged_msg, NOW(), v_CallDurationHistoryId);

    END IF;

    GET DIAGNOSTICS v_rowcount = ROW_COUNT;

    IF v_rowcount > 0 THEN
        v_errcode := 0;
        v_errmsg := 'inserted successfully';
    END IF;

    RAISE NOTICE '%, %, %', v_errcode, v_errmsg, v_CallHistoryId;

EXCEPTION
    WHEN OTHERS THEN
        RAISE NOTICE '-1, Unexpected SQL error occurred, %', v_CallHistoryId;
END;
$$;


ALTER PROCEDURE public.crm_call_create_message_details_bot(IN p_domainid integer, IN p_sessionid character varying, IN p_message jsonb, IN p_agentextn integer, IN p_isbot smallint) OWNER TO postgres;

--
-- Name: crm_campaign_open_click_update_insert(integer, integer, character varying, character varying, character varying, character varying, integer, integer); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_campaign_open_click_update_insert(IN p_domainid integer, IN p_campaignid integer, IN p_campaign_chat_id character varying, IN p_emailid character varying, IN p_channel_type character varying, IN p_phone_no character varying, IN p_uniqueclick integer, IN p_uniqueopen integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_count INT := 0;
BEGIN
    -- 1. Check if the tracking record already exists
    SELECT COUNT(*) INTO v_count 
    FROM tb_campaign_open_click_rate_history_dtl 
    WHERE "domainId" = p_domainId 
      AND "campaignId" = p_campaignId 
      AND channel_type = p_channel_type 
      AND ("emailId" = p_emailId OR "Phone_no" = p_Phone_no);

    -- 2. Logic for Insert or Update (UPSERT)
    IF v_count = 0 THEN
        INSERT INTO tb_campaign_open_click_rate_history_dtl (
            "domainId", 
            "campaignId", 
            campaign_chat_id, 
            "emailId", 
            "Phone_no", 
            channel_type, 
            "uniqueClick", 
            "uniqueOpen"
        )
        VALUES (
            p_domainId, 
            p_campaignId, 
            p_campaign_chat_id, 
            p_emailId, 
            p_Phone_no, 
            p_channel_type, 
            p_uniqueClick, 
            p_uniqueOpen
        );
    ELSE
        UPDATE tb_campaign_open_click_rate_history_dtl
        SET "uniqueClick" = "uniqueClick" + COALESCE(p_uniqueClick, 0), 
            "uniqueOpen" = "uniqueOpen" + COALESCE(p_uniqueOpen, 0),
			"UpdatedOn" = NOW()
        WHERE "domainId" = p_domainId 
          AND "campaignId" = p_campaignId 
          AND channel_type = p_channel_type 
          AND ("emailId" = p_emailId OR "Phone_no" = p_Phone_no);
    END IF;

END;
$$;


ALTER PROCEDURE public.crm_campaign_open_click_update_insert(IN p_domainid integer, IN p_campaignid integer, IN p_campaign_chat_id character varying, IN p_emailid character varying, IN p_channel_type character varying, IN p_phone_no character varying, IN p_uniqueclick integer, IN p_uniqueopen integer) OWNER TO postgres;

--
-- Name: crm_campaign_type_details(integer, smallint, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_campaign_type_details(IN p_domainid integer, IN p_flag smallint, INOUT p_contacts refcursor DEFAULT 'p_contacts'::refcursor)
    LANGUAGE plpgsql
    AS $$
declare v_from_date date;
		v_to_date   date;
BEGIN

	DROP TABLE IF EXISTS tt_campaignIds;
	CREATE TEMP TABLE tt_campaignIds ON COMMIT DROP AS
	select a."name" as campaign_name,a."campaignGoal" as campaign_goal,'sms' as channel ,a.campaignchatid as campaignchatid, a."createdDate" as createdate, '' as created_by,
	b."schedulefromDate" as from_date,
	b."scheduletoDate" as to_date,
	a."triggerType" as sc_trigger_type,b."campaignId" as campaignId,
	a."contactDtls" as contactslist
	from sc_campaign_master_dtl a 
	join sc_campaign_master_schedule_dtl b  
	on a."domainId"=b."domainId" and a.id=b."campaignId" where a."domainId" =p_domainid ;
	
	DROP TABLE IF EXISTS tt_campaignIds1;
	CREATE TEMP TABLE tt_campaignIds1 ON COMMIT DROP AS
	select a."Name" as campaign_name,a."Goal" as campaign_goal,'email' as channel ,a.campaignchatid, a.createdate, a."createdBy" as created_by,
	a."emailDeliveryStarttime" as from_date,
	a."emailDeliveryEndtime" as to_date ,
	a."emailDelivery" as ec_trigger_type,"campaignId" as campaignId,
	a."Contactslist" as contactslist
	from ec_email_campaign_master_dtl a 
	where a."domainId" =p_domainid; 
 
	SELECT MIN(from_date) ,MAX(to_date) INTO v_from_date, v_to_date 
	FROM (
	SELECT from_date, to_date 
	FROM tt_campaignIds 
	UNION ALL 
	SELECT 	from_date, to_date 
	FROM tt_campaignIds1) AS combined;

	if p_flag =3 then
	
    OPEN p_contacts FOR
    SELECT 
        COALESCE(t1.campaign_name, t2.campaign_name) AS campaign_name,
        COALESCE(t1.campaign_goal, t2.campaign_goal) AS campaign_goal,
        CASE WHEN t1.campaignchatid IS NOT NULL AND t2.campaignchatid IS NOT NULL THEN 'email,sms' WHEN t1.campaignchatid IS NOT NULL THEN 'sms' WHEN t2.campaignchatid IS NOT NULL THEN 'email' END AS channel,
        COALESCE(t1.campaignchatid, t2.campaignchatid) AS campaignchatid,
        COALESCE(t1.createdate, t2.createdate) AS createdate,
        COALESCE(t1.created_by, t2.created_by) AS created_by,
        COALESCE(t1.from_date, t2.from_date) AS from_date,
        COALESCE(t1.to_date, t2.to_date) AS to_date,
        COALESCE(t1.sc_trigger_type, t2.ec_trigger_type) AS trigger_type
    FROM tt_campaignIds t1
    FULL OUTER JOIN tt_campaignIds1 t2 ON t1.campaignchatid = t2.campaignchatid
    WHERE (t2."ec_trigger_type" = 2 or t1."sc_trigger_type" =1);
	
	elseif p_flag=1 then -- past
	
	OPEN p_contacts FOR 
    SELECT 
        COALESCE(t1.campaign_name, t2.campaign_name) AS campaign_name,
        COALESCE(t1.campaign_goal, t2.campaign_goal) AS campaign_goal,
        CASE WHEN t1.campaignchatid IS NOT NULL AND t2.campaignchatid IS NOT NULL THEN 'email,sms' WHEN t1.campaignchatid IS NOT NULL THEN 'sms' WHEN t2.campaignchatid IS NOT NULL THEN 'email' END AS channel,
        COALESCE(t1.campaignchatid, t2.campaignchatid) AS campaignchatid,
        COALESCE(t1.createdate, t2.createdate) AS createdate,
        COALESCE(t1.created_by, t2.created_by) AS created_by,
        COALESCE(t1.from_date, t2.from_date) AS from_date,
        COALESCE(t1.to_date, t2.to_date) AS to_date,
        COALESCE(t1.sc_trigger_type, t2.ec_trigger_type) AS trigger_type
    FROM tt_campaignIds t1
    FULL OUTER JOIN tt_campaignIds1 t2 ON t1.campaignchatid = t2.campaignchatid
    WHERE NOW() > COALESCE(t1.to_date, t2.to_date); 
	
	elseif p_flag=2 then -- present
 
    OPEN p_contacts FOR
    SELECT 
        COALESCE(t1.campaign_name, t2.campaign_name) AS campaign_name,
        COALESCE(t1.campaign_goal, t2.campaign_goal) AS campaign_goal,
        CASE WHEN t1.campaignchatid IS NOT NULL AND t2.campaignchatid IS NOT NULL THEN 'email,sms' WHEN t1.campaignchatid IS NOT NULL THEN 'sms' WHEN t2.campaignchatid IS NOT NULL THEN 'email' END AS channel,
        COALESCE(t1.campaignchatid, t2.campaignchatid) AS campaignchatid,
        COALESCE(t1.createdate, t2.createdate) AS createdate,
        COALESCE(t1.created_by, t2.created_by) AS created_by,
        COALESCE(t1.from_date, t2.from_date) AS from_date,
        COALESCE(t1.to_date, t2.to_date) AS to_date,
        COALESCE(t1.sc_trigger_type, t2.ec_trigger_type) AS trigger_type,
		COALESCE(t1.campaignId, t2.campaignId) AS campaignId,
		jsonb_array_length(COALESCE(t1.contactslist, t2.contactslist, '[]'::jsonb)) AS length_of_contactslist
		-- COALESCE(t1."contactslist", t2."contactslist") AS contactslist
    FROM tt_campaignIds t1
    FULL OUTER JOIN tt_campaignIds1 t2 ON t1.campaignchatid = t2.campaignchatid
    WHERE NOW() >= COALESCE(t1.from_date, t2.from_date)
  	AND NOW() <= COALESCE(t1.to_date, t2.to_date);

	END IF; 
	 
END;
$$;


ALTER PROCEDURE public.crm_campaign_type_details(IN p_domainid integer, IN p_flag smallint, INOUT p_contacts refcursor) OWNER TO postgres;

--
-- Name: crm_create_call_session_history_details(character varying, bigint, bigint, character varying, integer, integer, integer, smallint, bigint, bigint, bigint, bigint, bigint, bigint, bigint, bigint, integer, integer, smallint, integer, smallint, integer, character varying, bigint, bigint, bigint, bigint, bigint, bigint, integer, smallint, integer, character varying, bigint, bigint, integer, json, bigint, bigint, numeric, smallint, bigint, bigint, bigint, bigint, smallint, smallint, smallint, smallint, smallint, smallint, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_create_call_session_history_details(IN p_sessionid character varying, IN p_sessionstarttime bigint, IN p_sessionendtime bigint, IN p_customernumber character varying, IN p_domainid integer, IN p_dispositionid integer, IN p_umnstatus integer, IN p_isinbound smallint, IN p_dialduration bigint, IN p_disconnectedby bigint, IN p_queueduration bigint, IN p_assignedto bigint, IN p_assignedby bigint, IN p_assignedon bigint, IN p_callednumber bigint, IN p_waitduration bigint, IN p_aftercallworktime integer, IN p_recordingtypeid integer, IN p_istransferred smallint, IN p_transferredtypeid integer, IN p_markasread smallint, IN p_customerid integer, IN p_voicemailurl character varying, IN p_ticketid bigint, IN p_starttime bigint, IN p_endtime bigint, IN p_duration bigint, IN p_agentholdduration bigint, IN p_agentid bigint, IN p_agentskillid integer, IN p_isbot smallint, IN p_botid integer, IN p_recordingurl character varying, IN p_transferredtime bigint, IN p_recordingduration bigint, IN p_queueid integer, IN p_message json, IN p_callhistoryid bigint, IN p_callduration bigint, IN p_callcost numeric, IN p_isabandoned smallint, IN p_transferedby bigint, IN p_transferedto bigint, IN p_businessstarttime bigint, IN p_businessendtime bigint, IN p_ismissedcall smallint, IN p_islisten smallint, IN p_iswhisper smallint, IN p_isbarge smallint, IN p_isoutofbussinesshour smallint, IN p_isoutboundcampaign smallint, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_errcode                   INT := 0;
    v_errmsg                    TEXT := '';
    v_CallHistoryId             BIGINT;
    v_curDate                   TIMESTAMP := CURRENT_TIMESTAMP;
    v_LoginDate                 DATE := CURRENT_DATE;
    v_AgentId_Int               BIGINT;
    v_transferedBy_Int          BIGINT;
    v_transferedTo_Int          BIGINT;
    v_SourceCallDurationHistoryId BIGINT;            
    v_LastAgentId               BIGINT;
    v_LastUMNStatus             INT;   
    v_LastCallDurationHistoryId BIGINT;
    v_coid                      INT;
    v_callAgentDispositionId    INT;
    v_sessionstartime           TIMESTAMP := CURRENT_TIMESTAMP;
    v_CallDurationHistoryId_1   BIGINT;
    v_CallDurationHistoryId_New BIGINT;
    v_rowCount                  INT;
BEGIN

    -- 1. Lookup customer contact
    SELECT customer_id INTO v_coid 
    FROM tb_customer_contact 
    WHERE domain_id = p_domainId 
    AND primary_phone_number = p_customerNumber 
    LIMIT 1;

    -- 2. Disposition logic mapping
    IF p_dispositionId IN (1121001, 1121002, 1121004, 1121003, 1121005) THEN
        SELECT DS.did INTO v_callAgentDispositionId  
        FROM disposition_status DS
        JOIN "TypeCode" TY ON TY."TypeCodeName" = DS."dispositionName"
        WHERE TY."TypeCodeID" = p_dispositionId
        AND DS."domainId" = p_domainId;
    ELSE
        v_callAgentDispositionId := CASE WHEN p_dispositionId = 0 THEN NULL ELSE p_dispositionId END;
    END IF;

    -- 3. Resolve internal UserIDs (Replaces temp table logic)
    SELECT "UserID" INTO v_AgentId_Int FROM "tb_User" WHERE ext = p_AgentId AND "domainId" = p_domainId;
    SELECT "UserID" INTO v_transferedBy_Int FROM "tb_User" WHERE ext = p_transferedBy AND "domainId" = p_domainId;
    SELECT "UserID" INTO v_transferedTo_Int FROM "tb_User" WHERE ext = p_transferedTo AND "domainId" = p_domainId;

    -- 4. Main Validation
    IF p_domainId IS NULL OR p_domainId = 0 THEN
        v_errcode := -1;
        v_errmsg := 'DomainId should not be empty';
    ELSE
        -- 5. Business Hour Check
        IF NOT EXISTS (SELECT 1 FROM "tb_AgentBusinessHour" WHERE "agentId" = v_AgentId_Int AND "DateOfLogin" = v_LoginDate)
           AND COALESCE(v_AgentId_Int, 0) <> 0 THEN
            INSERT INTO "tb_AgentBusinessHour" ("agentId", "DateOfLogin", "businessStartTime", "businessEndTime", "createDate")
            VALUES (v_AgentId_Int, v_LoginDate, p_businessStartTime, p_businessEndTime, v_curDate);
        END IF;

        -- 6. Check if Session exists
        SELECT "CallHistoryId" INTO v_CallHistoryId FROM "tb_CallHistory" WHERE "sessionId" = p_sessionId;

        IF v_CallHistoryId IS NOT NULL THEN
            -- UPDATE existing history record
            UPDATE "tb_CallHistory" SET 
                "sessionStartTime" = COALESCE(p_sessionStartTime, "sessionStartTime"),
            "sessionEndTime" = COALESCE(p_sessionEndTime, "sessionEndTime"),
            "customerNumber" = COALESCE(p_customerNumber, "customerNumber"),
            "domainId" = COALESCE(p_domainId, "domainId"),
            "IsInBound" = COALESCE(p_IsInBound, "IsInBound"),
            "dialDuration" = COALESCE(p_dialDuration, "dialDuration"),
            "disconnectedBy" = COALESCE(p_disconnectedBy, "disconnectedBy"),
            "queueDuration" = COALESCE(p_queueDuration, "queueDuration"),
            "assignedTo" = COALESCE(p_assignedTo, "assignedTo"),
            "assignedBy" = COALESCE(p_assignedBy, "assignedBy"),
            "assignedOn" = COALESCE(p_assignedOn, "assignedOn"),
            "calledNumber" = COALESCE(p_calledNumber, "calledNumber"),
            "waitDuration" = COALESCE(p_waitDuration, "waitDuration"),
            "recordingTypeId" = COALESCE(p_recordingTypeId, "recordingTypeId"),
            "CustomerId" = CASE WHEN p_CustomerId = 0 THEN v_coid ELSE p_CustomerId END,
            "voicemailUrl" = COALESCE(p_voicemailUrl, "voicemailUrl"),
            "callDuration" = COALESCE(p_callDuration, "callDuration"),
            "callCost" = COALESCE(p_callCost, "callCost"),
            "IsAbandoned" = COALESCE(p_IsAbandoned, "IsAbandoned"),
            "IsOutOfBussinessHour" = COALESCE(p_IsOutOfBussinessHour, "IsOutOfBussinessHour"),
            "updatedAt" = v_curDate
        WHERE "CallHistoryId" = v_CallHistoryId;

            -- Update generic queue record if applicable
            IF p_QueueId IS NOT NULL OR v_callAgentDispositionId IS NOT NULL THEN
                UPDATE "tb_CallDurationHistory" SET 
                    "QueueId" = COALESCE(p_QueueId, "QueueId"),
                    "dispositionId" = COALESCE(v_callAgentDispositionId, "dispositionId")
                WHERE "CallHistoryId" = v_CallHistoryId AND "AgentId" IS NULL AND "IsBot" = 0;
            END IF;

            -- Handle Agent-specific legs
            IF v_AgentId_Int IS NOT NULL THEN
                
                -- State capture for ELSIF logic
                SELECT "AgentId", "UMNStatus", "CallDurationHistoryId" 
                INTO v_LastAgentId, v_LastUMNStatus, v_LastCallDurationHistoryId 
                FROM "tb_CallDurationHistory" 
                WHERE "CallHistoryId" = v_CallHistoryId 
                ORDER BY "CallDurationHistoryId" DESC LIMIT 1;

                -- Find specific update target
                SELECT "CallDurationHistoryId" INTO v_CallDurationHistoryId_1 
                FROM "tb_CallDurationHistory" 
                WHERE "CallHistoryId" = v_CallHistoryId AND "AgentId" IS NULL AND "IsBot" = 0 LIMIT 1;

                IF v_CallDurationHistoryId_1 IS NULL AND COALESCE(p_disconnectedBy, 0) <> 0 THEN
                    SELECT "CallDurationHistoryId" INTO v_CallDurationHistoryId_1 
                    FROM "tb_CallDurationHistory" 
                    WHERE "CallHistoryId" = v_CallHistoryId AND "AgentId" = v_AgentId_Int
                    ORDER BY "CallDurationHistoryId" DESC LIMIT 1;
                END IF;

                -- BRANCHING START
                IF v_CallDurationHistoryId_1 IS NOT NULL THEN
                    UPDATE "tb_CallDurationHistory" SET
                        "StartTime" = COALESCE(p_StartTime, "StartTime"),
                    "EndTime" = COALESCE(p_EndTime, "EndTime"),
                    "Duration" = COALESCE(p_Duration, "Duration"),
                    "AgentHoldDuration" = COALESCE(p_AgentHoldDuration, "AgentHoldDuration"),
                    "AgentId" = COALESCE(v_AgentId_Int, "AgentId"),
                    "AgentSkillId" = COALESCE(p_AgentSkillId, "AgentSkillId"),
                    "IsBot" = COALESCE(p_IsBot, "IsBot"),
                    "BotId" = COALESCE(p_BotId, "BotId"),
                    "RecordingUrl" = COALESCE(p_RecordingUrl, "RecordingUrl"),
                    "transferredTime" = COALESCE(p_transferredTime, "transferredTime"),
                    "recordingDuration" = COALESCE(p_recordingDuration, "recordingDuration"),
					"dialDuration" = COALESCE(p_dialDuration, "dialDuration"),
                    "transferredTypeId" = COALESCE(p_transferredTypeId, "transferredTypeId"),
                    "QueueId" = COALESCE(p_QueueId, "QueueId"),
                    "dispositionId" = COALESCE(v_callAgentDispositionId, "dispositionId"),
                    "UMNStatus" = COALESCE(p_UMNStatus, "UMNStatus"),
                    "transferedBy" = COALESCE(v_transferedBy_Int, "transferedBy"),
                    "transferedTo" = COALESCE(v_transferedTo_Int, "transferedTo"),
                    "ticketId" = COALESCE(p_ticketId, "ticketId"),
                    "IsMissedCall" = COALESCE(p_IsMissedCall, "IsMissedCall"),
                    "isListen" = COALESCE(p_isListen, "isListen"),
                    "isWhisper" = COALESCE(p_isWhisper, "isWhisper"),
                    "isBarge" = COALESCE(p_isBarge, "isBarge"),
                    "updatedAt" = v_curDate
                WHERE "CallDurationHistoryId" = v_CallDurationHistoryId_1;
                    v_CallDurationHistoryId_New := v_CallDurationHistoryId_1;

                ELSIF (p_UMNStatus IN (3,4) OR COALESCE(v_LastAgentId,0) <> v_AgentId_Int OR v_LastUMNStatus IN (3,4)) THEN
                    INSERT INTO "tb_CallDurationHistory" (
                        "CallHistoryId", "StartTime", "EndTime", "Duration", "AgentHoldDuration", "AgentId", "AgentSkillId", "IsBot", "BotId", 
                        "RecordingUrl", "transferredTime", "recordingDuration", "QueueId", "createdAt", "dialDuration", 
                        "transferredTypeId", "dispositionId", "UMNStatus", "transferedBy", "transferedTo", "ticketId", 
                        "IsMissedCall", "isListen", "isWhisper", "isBarge", "updatedAt"
                    ) VALUES (
                        v_CallHistoryId, p_StartTime, p_EndTime, p_Duration, p_AgentHoldDuration, v_AgentId_Int, p_AgentSkillId, p_IsBot, p_BotId, 
                        p_RecordingUrl, p_transferredTime, p_recordingDuration, p_QueueId, v_curDate, p_dialDuration, 
                        p_transferredTypeId, v_callAgentDispositionId, p_UMNStatus, v_transferedBy_Int, v_transferedTo_Int, p_ticketId, 
                        p_IsMissedCall, p_isListen, p_isWhisper, p_isBarge, v_curDate
                    ) RETURNING "CallDurationHistoryId" INTO v_CallDurationHistoryId_New;

                ELSIF v_LastCallDurationHistoryId IS NOT NULL THEN
                    UPDATE "tb_CallDurationHistory" SET
                        "StartTime" = COALESCE(p_StartTime, "StartTime"),
                        "EndTime" = COALESCE(p_EndTime, "EndTime"),
                        "Duration" = COALESCE(p_Duration, "Duration"),
                        "AgentHoldDuration" = COALESCE(p_AgentHoldDuration, "AgentHoldDuration"),
                        "AgentSkillId" = COALESCE(p_AgentSkillId, "AgentSkillId"),
                        "IsBot" = COALESCE(p_IsBot, "IsBot"),
                        "BotId" = COALESCE(p_BotId, "BotId"),
                        "RecordingUrl" = COALESCE(p_RecordingUrl, "RecordingUrl"),
                        "transferredTime" = COALESCE(p_transferredTime, "transferredTime"),
                        "recordingDuration" = COALESCE(p_recordingDuration, "recordingDuration"),
                        "dialDuration" = COALESCE(p_dialDuration, "dialDuration"),
                        "transferredTypeId" = COALESCE(p_transferredTypeId, "transferredTypeId"),
                        "QueueId" = COALESCE(p_QueueId, "QueueId"),
                        "dispositionId" = COALESCE(v_callAgentDispositionId, "dispositionId"),
                        "UMNStatus" = COALESCE(p_UMNStatus, "UMNStatus"),
                        "transferedBy" = COALESCE(v_transferedBy_Int, "transferedBy"),
                        "transferedTo" = COALESCE(v_transferedTo_Int, "transferedTo"),
                        "ticketId" = COALESCE(p_ticketId, "ticketId"),
                        "IsMissedCall" = COALESCE(p_IsMissedCall, "IsMissedCall"),
                        "isListen" = COALESCE(p_isListen, "isListen"),
                        "isWhisper" = COALESCE(p_isWhisper, "isWhisper"),
                        "isBarge" = COALESCE(p_isBarge, "isBarge"),
                        "updatedAt" = v_curDate
                    WHERE "CallDurationHistoryId" = v_LastCallDurationHistoryId;
                    v_CallDurationHistoryId_New := v_LastCallDurationHistoryId;

                ELSE
                    INSERT INTO "tb_CallDurationHistory" (
                        "CallHistoryId", "StartTime", "EndTime", "Duration", "AgentHoldDuration", "AgentId", "AgentSkillId", "IsBot", "BotId", 
                        "RecordingUrl", "transferredTime", "recordingDuration", "QueueId", "createdAt", "dialDuration", 
                        "transferredTypeId", "dispositionId", "UMNStatus", "transferedBy", "transferedTo", "ticketId", 
                        IsMissedCall, isListen, isWhisper, isBarge, updatedAt
                    ) VALUES (
                        v_CallHistoryId, p_StartTime, p_EndTime, p_Duration, p_AgentHoldDuration, v_AgentId_Int, p_AgentSkillId, p_IsBot, p_BotId, 
                        p_RecordingUrl, p_transferredTime, p_recordingDuration, p_QueueId, v_curDate, p_dialDuration, 
                        p_transferredTypeId, v_callAgentDispositionId, p_UMNStatus, v_transferedBy_Int, v_transferedTo_Int, p_ticketId, 
                        p_IsMissedCall, p_isListen, p_isWhisper, p_isBarge, v_curDate
                    ) RETURNING "CallDurationHistoryId" INTO v_CallDurationHistoryId_New;
                END IF;
                -- BRANCHING END
            END IF;

            GET DIAGNOSTICS v_rowCount = ROW_COUNT;
            IF v_rowCount > 0 THEN
                v_errcode := 0;
                v_errmsg := 'updated successfully';
            END IF;

        ELSE
            -- FIRST TIME INSERT (Session does not exist)
            INSERT INTO "tb_CallHistory"(
                "sessionId", "sessionStartTime", "sessionEndTime", "customerNumber", "domainId", "IsInBound", "dialDuration", "disconnectedBy", "queueDuration",
                "assignedTo", "assignedBy", "assignedOn", "calledNumber", "waitDuration", "recordingTypeId", "CustomerId", "voicemailUrl", "createdAt", "callBack", "updatedAt",
                "callCost", "IsAbandoned", "IsOutOfBussinessHour", "isOutboundCampaign"
            ) VALUES (
                p_sessionId, p_sessionStartTime, p_sessionEndTime, p_customerNumber, p_domainId, p_IsInBound, p_dialDuration, p_disconnectedBy, p_queueDuration,
                p_assignedTo, p_assignedBy, p_assignedOn, p_calledNumber, p_waitDuration, p_recordingTypeId, p_CustomerId, p_voicemailUrl, v_curDate, 3, v_curDate,
                p_callCost, p_IsAbandoned, p_IsOutOfBussinessHour, p_isOutboundCampaign
            ) RETURNING "CallHistoryId" INTO v_CallHistoryId;

            INSERT INTO "tb_CallDurationHistory" (
                "CallHistoryId", "StartTime", "EndTime", "Duration", "AgentHoldDuration", "AgentId", "AgentSkillId", "IsBot", "BotId", "RecordingUrl", "transferredTime", "recordingDuration",
                "QueueId", "createdAt", "dialDuration", "transferredTypeId", "dispositionId", "UMNStatus", "transferedBy", "transferedTo", "ticketId", "IsMissedCall", "isListen", "isWhisper", "isBarge", "updatedAt"
            ) VALUES (
                v_CallHistoryId, p_StartTime, p_EndTime, p_Duration, p_AgentHoldDuration, v_AgentId_Int, p_AgentSkillId, p_IsBot, p_BotId, p_RecordingUrl, p_transferredTime, p_recordingDuration,
                p_QueueId, v_curDate, p_dialDuration, p_transferredTypeId, v_callAgentDispositionId, p_UMNStatus, v_transferedBy_Int, v_transferedTo_Int, p_ticketId, p_IsMissedCall, p_isListen, p_isWhisper, p_isBarge, v_curDate
            ) RETURNING "CallDurationHistoryId" INTO v_CallDurationHistoryId_New;

            v_errcode := 0;
            v_errmsg := 'inserted successfully';
        END IF;
    END IF;

    -- 7. Call Transfer Logic (External Procedure call)
    IF p_transferedBy IS NOT NULL AND v_CallHistoryId IS NOT NULL THEN
        SELECT "CallDurationHistoryId" INTO v_SourceCallDurationHistoryId        
        FROM "tb_CallDurationHistory"
        WHERE "CallHistoryId" = v_CallHistoryId AND "AgentId" = p_transferedBy;    
        
        IF v_SourceCallDurationHistoryId IS NOT NULL AND v_CallDurationHistoryId_New IS NOT NULL THEN 
            CALL up_TransaferCallMessage(v_SourceCallDurationHistoryId, v_CallDurationHistoryId_New);
        END IF;
    END IF;

    -- 8. Logging and Result Output
    RAISE NOTICE 'jsoncol: %', json_build_object('sessionId', p_sessionId, 'errcode', v_errcode, 'errmsg', v_errmsg, 'CallHistoryId', v_CallHistoryId);
    open p_result for select concat('jsoncol: %', json_build_object('sessionId', p_sessionId, 'errcode', v_errcode, 'errmsg', v_errmsg, 'CallHistoryId', v_CallHistoryId)) as jsoncol;

    INSERT INTO "callsessionstartendlog"("domainId", "sessionId", starttime, errcode, errmsg)
    VALUES (p_domainId, p_sessionId, v_sessionstartime, v_errcode, v_errmsg);

EXCEPTION WHEN OTHERS THEN
    GET STACKED DIAGNOSTICS v_errmsg = MESSAGE_TEXT;
    INSERT INTO "callsessionstartendlog"("domainId", "sessionId", starttime, errcode, errmsg)
    VALUES (p_domainId, p_sessionId, v_sessionstartime, -1, v_errmsg);
    RAISE EXCEPTION 'Procedure error: %', v_errmsg;
END;
$$;


ALTER PROCEDURE public.crm_create_call_session_history_details(IN p_sessionid character varying, IN p_sessionstarttime bigint, IN p_sessionendtime bigint, IN p_customernumber character varying, IN p_domainid integer, IN p_dispositionid integer, IN p_umnstatus integer, IN p_isinbound smallint, IN p_dialduration bigint, IN p_disconnectedby bigint, IN p_queueduration bigint, IN p_assignedto bigint, IN p_assignedby bigint, IN p_assignedon bigint, IN p_callednumber bigint, IN p_waitduration bigint, IN p_aftercallworktime integer, IN p_recordingtypeid integer, IN p_istransferred smallint, IN p_transferredtypeid integer, IN p_markasread smallint, IN p_customerid integer, IN p_voicemailurl character varying, IN p_ticketid bigint, IN p_starttime bigint, IN p_endtime bigint, IN p_duration bigint, IN p_agentholdduration bigint, IN p_agentid bigint, IN p_agentskillid integer, IN p_isbot smallint, IN p_botid integer, IN p_recordingurl character varying, IN p_transferredtime bigint, IN p_recordingduration bigint, IN p_queueid integer, IN p_message json, IN p_callhistoryid bigint, IN p_callduration bigint, IN p_callcost numeric, IN p_isabandoned smallint, IN p_transferedby bigint, IN p_transferedto bigint, IN p_businessstarttime bigint, IN p_businessendtime bigint, IN p_ismissedcall smallint, IN p_islisten smallint, IN p_iswhisper smallint, IN p_isbarge smallint, IN p_isoutofbussinesshour smallint, IN p_isoutboundcampaign smallint, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_create_customer_and_deal(integer, character varying, character varying, character varying, character varying, text, character varying, character varying, character varying, integer, integer, integer, integer, jsonb, character varying, character varying, character varying, character varying, character varying, jsonb, jsonb, character varying, character varying, character varying, character varying, character varying, jsonb, jsonb, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_create_customer_and_deal(IN p_domain_id integer, IN p_first_name character varying, IN p_last_name character varying, IN p_primary_phone_number character varying, IN p_primary_email character varying, IN p_address text, IN p_industry_type character varying, IN p_source_type character varying, IN p_company_size character varying, IN p_user_id integer, IN p_demo_graphics integer, IN p_firmo_graphics integer, IN p_behaviors integer, IN p_additional_info jsonb, IN p_company_name character varying, IN p_secondary_phone_number character varying, IN p_secondary_email character varying, IN p_deal_name character varying, IN p_total_amount character varying, IN p_deal_details jsonb, IN p_activity_timeline jsonb, IN p_weighted_amount character varying, IN p_open_amount character varying, IN p_pending_amount character varying, IN p_closed_amount character varying, IN p_new_amount character varying, IN p_ai_suggest_summary_step jsonb, IN p_deal_activity jsonb, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_cust_id INTEGER;
    v_deal_id INTEGER;
    v_errcode INTEGER;
    v_errmsg  TEXT;
BEGIN
    ------------------------------------------------------------------
    -- 1. CUSTOMER LOGIC: Find or Create
    ------------------------------------------------------------------
    SELECT customer_id INTO v_cust_id FROM tb_customer_contact WHERE (primary_email = p_primary_email OR primary_phone_number = p_primary_phone_number) AND domain_id = p_domain_id LIMIT 1;

    IF v_cust_id IS NULL THEN
	 
	
        INSERT INTO tb_customer_contact ( domain_id, first_name, last_name, primary_phone_number, primary_email, address, industry_type, source_type, company_name, company_size, user_id, demo_graphics, firmo_graphics, behaviors, created_at, updated_at, additional_info )
        VALUES ( p_domain_id, p_first_name, p_last_name, p_primary_phone_number, p_primary_email, p_address, 
		p_industry_type, p_source_type, p_company_name, p_company_size, p_user_id, p_demo_graphics, p_firmo_graphics, p_behaviors, NOW(), NOW(), p_additional_info )
        RETURNING customer_id INTO v_cust_id;

 
        IF p_secondary_phone_number IS NOT NULL OR p_secondary_email IS NOT NULL THEN
	
            INSERT INTO tb_secondary_customer_contact (fk_customer_id,domain_id , secondary_phone_number , secondary_email , created_date , updated_date)
			values(v_cust_id,p_domain_id , p_secondary_phone_number , p_secondary_email , now() , now() );

        END IF;
    END IF;

    ------------------------------------------------------------------
    -- 2. DEAL LOGIC: Create or Update Log
    ------------------------------------------------------------------
    -- Check if this specific deal name already exists for this customer
    SELECT deal_id INTO v_deal_id FROM tb_deal_master WHERE customer_id = v_cust_id AND deal_name = p_deal_name ;

    IF v_deal_id IS NULL THEN
  
		INSERT INTO tb_deal_master ( customer_id , domain_id , deal_name , deal_details , total_amount , weighted_amount , open_amount , pending_amount , closed_amount , new_amount , ai_suggest_summary_step , deal_activity , activity_timeline , created_at , updated_at)
        VALUES ( v_cust_id , p_domain_id , p_deal_name , p_deal_details , p_total_amount , p_weighted_amount , p_open_amount , p_pending_amount , p_closed_amount , p_new_amount , p_ai_suggest_summary_step , p_deal_activity , p_activity_timeline , now(), now())
        RETURNING deal_id INTO v_deal_id;
        
        v_errcode := 0;
        v_errmsg := 'New Customer and Deal created successfully';
		
    END IF;

    -- Return Consolidated Result
    OPEN p_result FOR 
        SELECT v_errcode AS errcode, v_errmsg AS errmsg, v_cust_id AS customer_id, v_deal_id AS deal_id;

EXCEPTION WHEN OTHERS THEN
    OPEN p_result FOR SELECT -1 AS errcode, SQLERRM AS errmsg, NULL::int, NULL::int;
END;
$$;


ALTER PROCEDURE public.crm_create_customer_and_deal(IN p_domain_id integer, IN p_first_name character varying, IN p_last_name character varying, IN p_primary_phone_number character varying, IN p_primary_email character varying, IN p_address text, IN p_industry_type character varying, IN p_source_type character varying, IN p_company_size character varying, IN p_user_id integer, IN p_demo_graphics integer, IN p_firmo_graphics integer, IN p_behaviors integer, IN p_additional_info jsonb, IN p_company_name character varying, IN p_secondary_phone_number character varying, IN p_secondary_email character varying, IN p_deal_name character varying, IN p_total_amount character varying, IN p_deal_details jsonb, IN p_activity_timeline jsonb, IN p_weighted_amount character varying, IN p_open_amount character varying, IN p_pending_amount character varying, IN p_closed_amount character varying, IN p_new_amount character varying, IN p_ai_suggest_summary_step jsonb, IN p_deal_activity jsonb, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_create_email_config_details(integer, character varying, character varying, character varying, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, character varying, character varying, smallint, jsonb, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_create_email_config_details(IN p_emailconfigid integer, IN p_emailaddress character varying, IN p_aliasname character varying, IN p_supervisorname character varying, IN p_emailthreshold integer, IN p_isemail integer, IN p_domainid integer, IN p_primaryemail integer, IN p_isbot integer, IN p_knowledgebasedid integer, IN p_autoassignagent integer, IN p_servertype integer, IN p_productid integer, IN p_emailtype integer, IN p_domainname character varying, IN p_forwardemail character varying, IN p_is_smtp smallint, IN p_smtp_json jsonb, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$

DECLARE
    v_EmailConfigId INTEGER;
    v_errcode INTEGER := -1;
    v_errmsg VARCHAR(255) := 'Email Configuration Failed';
    v_EmailConfig INTEGER;
    v_row_count INTEGER;
BEGIN
    SELECT "EmailConfigId" INTO v_EmailConfigId FROM email_config WHERE "EmailConfigId" = p_EmailConfigId;

    IF v_EmailConfigId IS NULL THEN
        IF EXISTS (SELECT 1 FROM email_config WHERE "domainId" = p_domainId AND "emailAddress" = p_emailAddress) THEN
            v_errcode := -1;
            v_errmsg := 'Email Address is duplicating';
        ELSE
            INSERT INTO email_config("emailAddress", "aliasName", "supervisorName", "emailThreshold", "isEmail", "domainId", "createdAt", "primaryEmail", "isBot", "knowledgeBasedId", "autoAssignAgent", "serverType", product_id, emailtype, "domainName", "forwardEmail","is_smtp","smtp_json")
            VALUES (p_emailAddress, p_aliasName, p_supervisorName, p_emailThreshold, p_isEmail, p_domainId, NOW(), p_primaryEmail, p_isBot, p_knowledgeBasedId, p_autoAssignAgent, p_serverType, COALESCE(NULLIF(p_productId, 0), 0), p_emailtype, p_domainName, p_forwardEmail,p_is_smtp,p_smtp_json)
            RETURNING "EmailConfigId" INTO v_EmailConfig;
            v_errcode := 0;
            v_errmsg := 'inserted successfully';
        END IF;
    ELSE
        IF EXISTS (SELECT 1 FROM email_config WHERE "domainId" = p_domainId AND "emailAddress" = p_emailAddress AND "EmailConfigId" <> v_EmailConfigId) THEN
            v_errcode := -1;
            v_errmsg := 'Email Address is duplicating';
        ELSE
            UPDATE email_config 
			SET "emailAddress" = COALESCE(p_emailAddress, "emailAddress")
			, "aliasName" = COALESCE(p_aliasName, "aliasName")
			, "supervisorName" = COALESCE(p_supervisorName, "supervisorName")
			, "emailThreshold" = COALESCE(p_emailThreshold, "emailThreshold")
			, "isEmail" = COALESCE(p_isEmail, "isEmail")
			, "domainId" = COALESCE(p_domainId, "domainId")
			, "updatedAt" = NOW()
			, "primaryEmail" = COALESCE(p_primaryEmail, "primaryEmail")
			, "isBot" = COALESCE(p_isBot, "isBot")
			, "knowledgeBasedId" = CASE WHEN p_knowledgeBasedId IS NOT NULL THEN p_knowledgeBasedId::varchar ELSE "knowledgeBasedId" END
			, "autoAssignAgent" = COALESCE(p_autoAssignAgent, "autoAssignAgent")
			, emailtype = COALESCE(p_emailtype, emailtype)
			, "serverType" = COALESCE(p_serverType, "serverType")
			, "domainName" = COALESCE(p_domainName, "domainName")
			, "forwardEmail" = COALESCE(p_forwardEmail, "forwardEmail")
			, "is_smtp" = COALESCE(p_is_smtp, "is_smtp")
			, "smtp_json" = COALESCE(p_smtp_json, "smtp_json")
            WHERE "EmailConfigId" = v_EmailConfigId;
            GET DIAGNOSTICS v_row_count = ROW_COUNT;
            IF v_row_count > 0 THEN v_errcode := 0; v_errmsg := 'updated successfully'; END IF;
        END IF;
    END IF;

    OPEN p_result FOR SELECT v_errcode AS errcode, v_errmsg AS errmsg, v_EmailConfig AS "EmailConfigId";
END;
$$;


ALTER PROCEDURE public.crm_create_email_config_details(IN p_emailconfigid integer, IN p_emailaddress character varying, IN p_aliasname character varying, IN p_supervisorname character varying, IN p_emailthreshold integer, IN p_isemail integer, IN p_domainid integer, IN p_primaryemail integer, IN p_isbot integer, IN p_knowledgebasedid integer, IN p_autoassignagent integer, IN p_servertype integer, IN p_productid integer, IN p_emailtype integer, IN p_domainname character varying, IN p_forwardemail character varying, IN p_is_smtp smallint, IN p_smtp_json jsonb, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_create_videomeet_session_history_details_logic(character varying, bigint, bigint, character varying, integer, smallint, bigint, integer, bigint, smallint, text, integer, integer, bigint, bigint, bigint, smallint, character varying, character varying, timestamp without time zone, timestamp without time zone, character varying, text, integer, jsonb, character varying, character varying, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_create_videomeet_session_history_details_logic(IN p_sessionid character varying, IN p_sessionstarttime bigint, IN p_sessionendtime bigint, IN p_customernumber character varying, IN p_domainid integer, IN p_isinbound smallint, IN p_disconnectedby bigint, IN p_customerid integer, IN p_videoduration bigint, IN p_isabandoned smallint, IN p_meetingid text, IN p_agentid integer, IN p_dispositionid integer, IN p_starttime bigint, IN p_endtime bigint, IN p_agentduration bigint, IN p_ismissedcall smallint, IN p_tags character varying, IN p_meet_url character varying, IN p_schedule_start timestamp without time zone, IN p_schedule_end timestamp without time zone, IN p_meeting_title character varying, IN p_meeting_description text, IN p_created_by integer, IN p_urmeet_user_list jsonb, IN p_record_size character varying, IN p_record_url character varying, INOUT p_result refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_VideoHistoryId BIGINT;
    v_LastVideoDurationHistoryId BIGINT;
    v_curDate TIMESTAMP := CURRENT_TIMESTAMP;
    v_VideoDurationHistoryId BIGINT;
    v_errcode INT := -1;
    v_errmsg TEXT := 'Video inserted failed';
BEGIN

    IF p_domainId IS NULL OR p_domainId = 0 THEN
        v_errcode := -1;
        v_errmsg := 'DomainId should not be empty';
        OPEN p_result FOR SELECT v_errcode AS errcode, v_errmsg AS errmsg;
        RETURN;
    END IF;

    SELECT "VideoHistoryId"
    INTO v_VideoHistoryId
    FROM "tb_VideoHistory"
    WHERE "meetingId" = p_meetingId
      AND "domainId" = p_domainId
    LIMIT 1;

    IF v_VideoHistoryId IS NOT NULL THEN

        SELECT "VideoDurationHistoryId"
        INTO v_LastVideoDurationHistoryId
        FROM "tb_VideoDurationHistory"
        WHERE "VideoHistoryId" = v_VideoHistoryId
        ORDER BY "VideoDurationHistoryId" DESC
        LIMIT 1;

        UPDATE "tb_VideoHistory"
        SET
            "sessionStartTime" = COALESCE(p_sessionStartTime,"sessionStartTime"),
            "sessionEndTime" = COALESCE(p_sessionEndTime,"sessionEndTime"),
            "customerNumber" = COALESCE(p_customerNumber,"customerNumber"),
            "domainId" = COALESCE(p_domainId,"domainId"),
            "IsInBound" = COALESCE(p_IsInBound,"IsInBound"),
            "disconnectedBy" = COALESCE(p_disconnectedBy,"disconnectedBy"),
            "CustomerId" = COALESCE(p_CustomerId,"CustomerId"),
            "videoDuration" = COALESCE(p_videoDuration,"videoDuration"),
            "IsAbandoned" = COALESCE(p_IsAbandoned,"IsAbandoned"),
            tags = COALESCE(p_tags,tags),
            meet_url = COALESCE(p_meet_url,meet_url),
            schedule_start = COALESCE(p_schedule_start,schedule_start),
            schedule_end = COALESCE(p_schedule_end,schedule_end),
            meeting_title = COALESCE(p_meeting_title,meeting_title),
            meeting_description = COALESCE(p_meeting_description,meeting_description),
            "updatedAt" = v_curDate
        WHERE "VideoHistoryId" = v_VideoHistoryId;

        IF p_AgentId IS NOT NULL THEN

            IF EXISTS(
                SELECT 1
                FROM "tb_VideoDurationHistory"
                WHERE "VideoHistoryId" = v_VideoHistoryId
                AND "AgentId" IS NULL
            ) THEN

                UPDATE "tb_VideoDurationHistory"
                SET
                    "StartTime" = COALESCE(p_StartTime,"StartTime"),
                    "EndTime" = COALESCE(p_EndTime,"EndTime"),
                    "agentDuration" = COALESCE(p_agentDuration,"agentDuration"),
                    "AgentId" = COALESCE(p_AgentId,"AgentId"),
                    "dispositionId" = COALESCE(p_dispositionId,"dispositionId"),
                    "IsMissedCall" = COALESCE(p_IsMissedCall,"IsMissedCall"),
                    urmeet_user_list = COALESCE(p_urmeet_user_list,urmeet_user_list),
                    record_size = COALESCE(p_record_size,record_size),
                    record_url = COALESCE(p_record_url,record_url),
                    "updatedAt" = v_curDate
                WHERE "VideoHistoryId" = v_VideoHistoryId
                AND "AgentId" IS NULL;

            ELSE

                UPDATE "tb_VideoDurationHistory"
                SET
                    "StartTime" = COALESCE(p_StartTime,"StartTime"),
                    "EndTime" = COALESCE(p_EndTime,"EndTime"),
                    "agentDuration" = COALESCE(p_agentDuration,"agentDuration"),
                    "AgentId" = COALESCE(p_AgentId,"AgentId"),
                    "dispositionId" = COALESCE(p_dispositionId,"dispositionId"),
                    "IsMissedCall" = COALESCE(p_IsMissedCall,"IsMissedCall"),
                    urmeet_user_list = COALESCE(p_urmeet_user_list,urmeet_user_list),
                    record_size = COALESCE(p_record_size,record_size),
                    record_url = COALESCE(p_record_url,record_url),
                    "updatedAt" = v_curDate
                WHERE "VideoDurationHistoryId" = v_LastVideoDurationHistoryId;

            END IF;

        END IF;

        v_errcode := 0;
        v_errmsg := 'updated successfully';

    ELSE

        INSERT INTO "tb_VideoHistory"(
            "sessionId","sessionStartTime","sessionEndTime","customerNumber",
            "domainId","IsInBound","disconnectedBy",
            "CustomerId","videoDuration","IsAbandoned",
            "meetingId","createdAt","updatedAt",
            tags,meet_url,schedule_start,schedule_end,
            meeting_title,meeting_description,created_by
        )
        VALUES(
            p_sessionId,p_sessionStartTime,p_sessionEndTime,p_customerNumber,
            p_domainId,p_IsInBound,p_disconnectedBy,
            p_CustomerId,p_videoDuration,p_IsAbandoned,
            p_meetingId,v_curDate,v_curDate,
            p_tags,p_meet_url,p_schedule_start,p_schedule_end,
            p_meeting_title,p_meeting_description,p_created_by
        )
        RETURNING "VideoHistoryId" INTO v_VideoHistoryId;

        INSERT INTO "tb_VideoDurationHistory"(
            "VideoHistoryId","StartTime","EndTime","agentDuration",
            "AgentId","dispositionId",
            "createdAt","updatedAt",
            "IsMissedCall",urmeet_user_list,record_size,record_url
        )
        VALUES(
            v_VideoHistoryId,p_StartTime,p_EndTime,p_agentDuration,
            p_AgentId,p_dispositionId,
            v_curDate,v_curDate,
            p_IsMissedCall,p_urmeet_user_list,p_record_size,p_record_url
        )
        RETURNING "VideoDurationHistoryId"
        INTO v_VideoDurationHistoryId;

        v_errcode := 0;
        v_errmsg := 'inserted successfully';

    END IF;

    OPEN p_result FOR
    SELECT v_errcode AS errcode,
           v_errmsg AS errmsg;

END;
$$;


ALTER PROCEDURE public.crm_create_videomeet_session_history_details_logic(IN p_sessionid character varying, IN p_sessionstarttime bigint, IN p_sessionendtime bigint, IN p_customernumber character varying, IN p_domainid integer, IN p_isinbound smallint, IN p_disconnectedby bigint, IN p_customerid integer, IN p_videoduration bigint, IN p_isabandoned smallint, IN p_meetingid text, IN p_agentid integer, IN p_dispositionid integer, IN p_starttime bigint, IN p_endtime bigint, IN p_agentduration bigint, IN p_ismissedcall smallint, IN p_tags character varying, IN p_meet_url character varying, IN p_schedule_start timestamp without time zone, IN p_schedule_end timestamp without time zone, IN p_meeting_title character varying, IN p_meeting_description text, IN p_created_by integer, IN p_urmeet_user_list jsonb, IN p_record_size character varying, IN p_record_url character varying, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_custom_hours_status(integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_custom_hours_status(IN p_cid integer, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN
    OPEN p_result FOR SELECT cid, name, description, hours, "createdAt", "updatedAt", "domainId", "timeZone", default_hours, "timeZoneValue", type FROM custom_hours WHERE cid = p_cid;
END;
$$;


ALTER PROCEDURE public.crm_custom_hours_status(IN p_cid integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_delete_businessholidays(integer, integer, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_delete_businessholidays(IN p_fkbusinessid integer, IN p_holidayid integer, IN p_domainid integer, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_holidayId INTEGER;
    v_errcode SMALLINT := -1;
    v_errmsg VARCHAR(60) := 'Holiday delete failed';
BEGIN
    SELECT hid INTO v_holidayId FROM "tb_Business_Holidays" WHERE "fk_businessId" = p_fkBusinessId AND hid = p_holidayId AND "domainId" = p_domainId;
    IF v_holidayId IS NOT NULL THEN
        DELETE FROM "tb_Business_Holidays" WHERE hid = v_holidayId;
        v_errcode := 0; v_errmsg := 'Holiday deleted successfully';
    END IF;
    OPEN p_result FOR SELECT v_errcode AS errcode, v_errmsg AS errmsg;
END;
$$;


ALTER PROCEDURE public.crm_delete_businessholidays(IN p_fkbusinessid integer, IN p_holidayid integer, IN p_domainid integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_delete_custom_hours(integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_delete_custom_hours(IN p_cid integer, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_errMsg VARCHAR(50) := 'failed';
    v_errCode INTEGER := -1;
    v_row_count INTEGER;
BEGIN
    DELETE FROM custom_hours WHERE cid = p_cid;
    GET DIAGNOSTICS v_row_count = ROW_COUNT;
    IF v_row_count > 0 THEN v_errMsg := 'Deleted Successfully'; v_errCode := 0; END IF;
    OPEN p_result FOR SELECT v_errCode AS errCode, v_errMsg AS errMsg;
END;
$$;


ALTER PROCEDURE public.crm_delete_custom_hours(IN p_cid integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_deleteuser(integer, character varying, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_deleteuser(IN p_domainid integer, IN p_ext character varying, INOUT p_result refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_errcode INT DEFAULT 1;
    v_errmsg VARCHAR(50) DEFAULT 'No:Delete';
    v_deleted_count INT;
BEGIN

    DROP TABLE IF EXISTS tt_ext;

    CREATE TEMP TABLE tt_ext (
        id INT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
        exts INT,
        domainid INT,
        userid INT
    ) ON COMMIT DROP;

    INSERT INTO tt_ext(exts, domainId)
    SELECT unnest(string_to_array(p_ext, ','))::INT, p_domainid;

    UPDATE tt_ext e
    SET userid = u."UserID"
    FROM "tb_User" u
    WHERE e.exts = u.ext
      AND e.domainid = u."domainId";

    DELETE FROM "skillMapping"
    WHERE usid IN (SELECT userid FROM tt_ext)
      AND "domainId" = p_domainId;

    DELETE FROM "tb_User"
    WHERE ext IN (SELECT exts FROM tt_ext)
      AND "domainId" = p_domainid;

    GET DIAGNOSTICS v_deleted_count = ROW_COUNT;

    IF v_deleted_count > 0 THEN
        v_errcode := 0;
        v_errmsg := 'Deleted: Success';
    END IF;

    OPEN p_result FOR
    SELECT v_errcode AS "errcode",
           v_errmsg  AS "errmsg";

END;
$$;


ALTER PROCEDURE public.crm_deleteuser(IN p_domainid integer, IN p_ext character varying, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_ec_create_email_campaign_master_dtl(integer, text, text, character varying, character varying, text, smallint, jsonb, jsonb, jsonb, integer, timestamp without time zone, timestamp without time zone, timestamp without time zone, integer, jsonb, jsonb, integer, integer, integer, character varying, character varying, smallint, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_ec_create_email_campaign_master_dtl(IN p_domainid integer, IN p_name text, IN p_goal text, IN p_type character varying, IN p_targetaud character varying, IN p_objective text, IN p_ab_testing smallint, IN p_aitemplatea jsonb, IN p_aitemplateb jsonb, IN p_contactemail jsonb, IN p_emaildelivery integer, IN p_emaildeliveryat timestamp without time zone, IN p_emaildeliverystarttime timestamp without time zone, IN p_emaildeliveryendtime timestamp without time zone, IN p_ab_testtype integer, IN p_ab_variant jsonb, IN p_ab_variantdistribution jsonb, IN p_ab_winningcriteria integer, IN p_ab_testwindow integer, IN p_ab_testwindowtype integer, IN p_createdby character varying, IN p_campaignchatid character varying, IN p_isindivcamp smallint, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_campaignId INTEGER;
    v_existing_emails JSONB := '[]'::jsonb;
    v_excluded_emails JSONB := '[]'::jsonb;
    v_errcode INTEGER := -1;
    v_errmsg TEXT := 'Failure';
	v_CreateExamineTime TIMESTAMP;
    v_currentDate TIMESTAMP := NOW();  
    v_TotalCount INTEGER := 0; 
    v_A_count INTEGER := 0; 
    v_B_count INTEGER := 0; 

BEGIN

    ----------------------------------------------------
    -- 1️⃣ Validation
    ----------------------------------------------------
    IF p_campaignchatid IS NULL OR trim(p_campaignchatid) = '' THEN
        OPEN p_result FOR
        SELECT -1 AS errcode,
               'CampaignChatId is required' AS errmsg,
               NULL::INTEGER AS "campaignId";
        RETURN;
    END IF;

    ----------------------------------------------------
    -- 2️⃣ Get Existing & Excluded Emails
    ----------------------------------------------------
    CALL crm_sp_exclude_email(
        p_contactemail,
		p_domainid,
        v_existing_emails,
        v_excluded_emails
    );

    ----------------------------------------------------
    -- 3️⃣ Insert Campaign Master (Existing Only)
    ----------------------------------------------------
    INSERT INTO ec_email_campaign_master_dtl(
        "domainId", "Name", "Goal", "Type", "TargetAud",
        "Objective", "AB_testing", "AItemplateA",
        "AItemplateB", "Contactslist",
        "emailDelivery", "createdBy",
        "emailDeliveryAt", "emailDeliveryStarttime",
        "emailDeliveryEndtime", campaignchatid,
        "isIndivCamp"
    )
    VALUES(
        p_domainid, p_name, p_goal, p_type, p_targetaud,
        p_objective, p_ab_testing, p_aitemplatea,
        p_aitemplateb, v_existing_emails,
        p_emaildelivery, p_createdby,
        p_emaildeliveryat, p_emaildeliverystarttime,
        p_emaildeliveryendtime, p_campaignchatid,
        p_isindivcamp
    )
    RETURNING "campaignId" INTO v_campaignId;

    ----------------------------------------------------
    -- 4️⃣ Insert Excluded Emails (If Any)
    ----------------------------------------------------
    IF jsonb_array_length(v_excluded_emails) > 0 THEN
        INSERT INTO ec_email_campaign_master_dtl_exclude_email(
            "fk_campaignId",
            "domainId",
            campaignchatid,
            contactslist,
            "createdBy",
            status
        )
        VALUES(
            v_campaignId,
            p_domainid,
            p_campaignchatid,
            v_excluded_emails,
            p_createdby,
            0
        );
    END IF;
	
	IF p_ab_testing = 1 THEN

        v_CreateExamineTime :=
            CASE
                WHEN p_ab_testwindowtype = 1 
                    THEN v_currentDate + (p_ab_testwindow || ' minutes')::INTERVAL
                WHEN p_ab_testwindowtype = 2 
                    THEN v_currentDate + (p_ab_testwindow || ' hours')::INTERVAL
                ELSE v_currentDate
            END;

        INSERT INTO ec_email_campaign_AB_testing_dtl(
            "campaignId", testtype, variant, "variantDistribution",
            "winningCriteria", "AB_testWindow", 
            "AB_testWindowtype", "examineTime"
        )
        VALUES(
            v_campaignId, p_ab_testtype, p_ab_variant, 
            p_ab_variantdistribution,
            p_ab_winningcriteria, p_ab_testwindow, 
            p_ab_testwindowtype, v_CreateExamineTime
        );
    END IF;
	
	IF v_existing_emails IS NOT NULL 
       AND jsonb_typeof(v_existing_emails) = 'array'
       AND jsonb_array_length(v_existing_emails) > 0 THEN

        DROP TABLE IF EXISTS tt_contactemail;

        CREATE TEMP TABLE tt_contactemail (
            slno SERIAL PRIMARY KEY,
            contactemail VARCHAR(150),
            deliverytime TIMESTAMP,
            template VARCHAR(2)
        ) ON COMMIT DROP;

        INSERT INTO tt_contactemail(contactemail, deliverytime)
        SELECT
            (jt->>'contactEmail')::VARCHAR(150),
            (jt->>'deliveryTime')::TIMESTAMP
        FROM jsonb_array_elements(v_existing_emails) jt;

        SELECT COUNT(*) INTO v_TotalCount FROM tt_contactemail;
		
		
		 IF p_ab_testing = 1 AND v_TotalCount > 0 THEN

            v_A_count :=
                ROUND(v_TotalCount * 
                COALESCE((p_ab_variantdistribution->>'A')::NUMERIC,0) / 100);

            v_B_count :=
                ROUND(v_TotalCount * 
                COALESCE((p_ab_variantdistribution->>'B')::NUMERIC,0) / 100);

            UPDATE tt_contactemail
            SET template = 'A'
            WHERE slno <= v_A_count;

            UPDATE tt_contactemail
            SET template = 'B'
            WHERE slno > v_A_count 
              AND slno <= (v_A_count + v_B_count);

            UPDATE tt_contactemail
            SET template = 'A'
            WHERE template IS NULL;

        ELSE
            UPDATE tt_contactemail
            SET template = 'A';
        END IF;

        ------------------------------------------------
        -- Insert Campaign Contacts
        ------------------------------------------------
        INSERT INTO ec_campaign_contacts_dtl(
            "domainId", "campaignId", 
            "contactEmail", "deliveryTime", "Template"
        )
        SELECT 
            p_domainid, v_campaignId, 
            contactemail, deliverytime, template
        FROM tt_contactemail;

    END IF;

    ----------------------------------------------------
    -- 5️⃣ Success Response
    ----------------------------------------------------
    v_errcode := 0;
    v_errmsg := 'Campaign Created Successfully';

    OPEN p_result FOR
    SELECT v_campaignId AS "campaignId",
           v_errcode AS errcode,
           v_errmsg AS errmsg;

EXCEPTION
    WHEN OTHERS THEN
        v_errmsg := SQLERRM;
        OPEN p_result FOR
        SELECT NULL::INTEGER AS "campaignId",
               -1 AS errcode,
               v_errmsg AS errmsg;
END;
$$;


ALTER PROCEDURE public.crm_ec_create_email_campaign_master_dtl(IN p_domainid integer, IN p_name text, IN p_goal text, IN p_type character varying, IN p_targetaud character varying, IN p_objective text, IN p_ab_testing smallint, IN p_aitemplatea jsonb, IN p_aitemplateb jsonb, IN p_contactemail jsonb, IN p_emaildelivery integer, IN p_emaildeliveryat timestamp without time zone, IN p_emaildeliverystarttime timestamp without time zone, IN p_emaildeliveryendtime timestamp without time zone, IN p_ab_testtype integer, IN p_ab_variant jsonb, IN p_ab_variantdistribution jsonb, IN p_ab_winningcriteria integer, IN p_ab_testwindow integer, IN p_ab_testwindowtype integer, IN p_createdby character varying, IN p_campaignchatid character varying, IN p_isindivcamp smallint, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_ec_email_insert_failed_contact_sent_dtl(integer, integer, character varying); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_ec_email_insert_failed_contact_sent_dtl(IN p_domainid integer, IN p_campaignid integer, IN p_emailid character varying)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_campaignchatid VARCHAR(300);
    v_userid INTEGER;
    v_id INTEGER;
    v_rowcount INTEGER;
BEGIN
 

    -- Get campaign contact id
    SELECT id INTO v_id FROM ec_campaign_contacts_dtl WHERE "domainId" = p_domainid AND "campaignId" = p_campaignid AND "contactEmail" = p_emailid LIMIT 1;

    IF v_id IS NOT NULL THEN

        -- Update stop flag
        UPDATE ec_campaign_contacts_dtl
        SET "isStop" = 1,
            "stopTime" = now()
        WHERE id = v_id;

        GET DIAGNOSTICS v_rowcount = ROW_COUNT;

        IF v_rowcount > 0 THEN
             -- Get campaign chat id
            SELECT campaignchatid INTO v_campaignchatid FROM ec_email_campaign_master_dtl WHERE "domainId" = p_domainid AND "campaignId" = p_campaignid LIMIT 1;
            -- Get user id
            SELECT user_id INTO v_userid FROM tb_customer_contact WHERE domain_id = p_domainid AND primary_email = p_emailid LIMIT 1;

            -- Insert failed contact
            INSERT INTO ec_email_failed_contact_dtl(domainid,campaignid,campaignchatid,userid,contactemail,createdon)
			VALUES(p_domainid,p_campaignid,v_campaignchatid,v_userid,p_emailid,CURRENT_TIMESTAMP);

            GET DIAGNOSTICS v_rowcount = ROW_COUNT;

  

        END IF;

    END IF;
 
END;
$$;


ALTER PROCEDURE public.crm_ec_email_insert_failed_contact_sent_dtl(IN p_domainid integer, IN p_campaignid integer, IN p_emailid character varying) OWNER TO postgres;

--
-- Name: crm_ec_get_emails_for_all_campaigns(integer, refcursor, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_ec_get_emails_for_all_campaigns(IN v_limit integer, INOUT p_campaigns refcursor DEFAULT 'campaigns_cursor'::refcursor, INOUT p_contacts refcursor DEFAULT 'p_contacts'::refcursor)
    LANGUAGE plpgsql
    AS $$

DECLARE
    v_CurDate TIMESTAMP;
BEGIN
    -- Calculate current date + 5 hours 30 minutes (IST offset)
    v_CurDate := NOW() + INTERVAL '5 hours 30 minutes';

    -- Create temporary table for campaign contacts
    DROP TABLE IF EXISTS tt_campaignIds;

CREATE TEMP TABLE tt_campaignIds ON COMMIT DROP AS
SELECT t1.id AS "contactId",
           t1."campaignId",
		   t1."contactEmail",
           t1."deliveryTime",
           t1."Template",
           t1."domainId"
    FROM ec_campaign_contacts_dtl t1
    JOIN ec_email_campaign_master_dtl t2 ON t1."campaignId" = t2."campaignId"
    LEFT JOIN wc_master_revert_status_details t3 ON t2.campaignchatid = t3."campaignchatId"
    WHERE t1."isSent" = 0
      AND t1."deliveryTime" <= v_CurDate
      AND t3."campaignchatId" IS NULL
      AND t1."isStop" != 1
    ORDER BY t1."deliveryTime"
    LIMIT v_Limit;

    -- First result set: Campaign details with templates
    OPEN p_campaigns FOR
    WITH CTE_campaignId AS (
        SELECT DISTINCT "campaignId"
        FROM tt_campaignIds
    )
    SELECT CMP."campaignId",
		   CMP."Name" as "Campaign_Name",
           CMP."domainId",
           CMP."AItemplateA",
           CMP."AItemplateB"
    FROM CTE_campaignId C_C
    JOIN ec_email_campaign_master_dtl CMP ON CMP."campaignId" = C_C."campaignId";

    -- Second result set: Contact details with customer info
    OPEN p_contacts FOR
    SELECT t1."contactId",
           t1."campaignId",
           t1."contactEmail",
           t1."deliveryTime",
           t1."Template",
           t2.customer_id,
           t2.domain_id as domainId,
           t2.first_name,
           t2.last_name,
           t2.primary_phone_number,
           t2.primary_email,
           t2.address,
           t2.industry_type,
           t2.source_type,
           t2.company_name,
           t2.company_size,
           t2.user_id,
           t2.demo_graphics,
           t2.firmo_graphics,
           t2.behaviors,
           t2.is_active,
           t2."lifeCycleStage",
           t2."isUnSub_Email",
           t2.created_at,
           t2.updated_at,
           t2.additional_info
    FROM tt_campaignIds t1
    LEFT JOIN tb_customer_contact t2 ON t1."contactEmail" = t2.primary_email
                                    AND t1."domainId" = t2.domain_id;

    -- Cleanup temp table
   -- DROP TABLE IF EXISTS tt_campaignIds;
END;
$$;


ALTER PROCEDURE public.crm_ec_get_emails_for_all_campaigns(IN v_limit integer, INOUT p_campaigns refcursor, INOUT p_contacts refcursor) OWNER TO postgres;

--
-- Name: crm_ec_updatecampaignissentdtl(text, smallint); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_ec_updatecampaignissentdtl(IN p_id text, IN p_issent smallint)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_ids INTEGER[];
    v_issent SMALLINT := COALESCE(p_issent,0);
    v_campaignchatid VARCHAR(300);
    v_domainid INTEGER;
    v_userid INTEGER;
    v_isindivcamp SMALLINT;
    v_createdon TIMESTAMP := CURRENT_TIMESTAMP;
    v_rowcount INTEGER;
BEGIN

    -- Convert comma separated ids once
    v_ids := string_to_array(p_id, ',')::INTEGER[];

    -- Update records
    UPDATE ec_campaign_contacts_dtl cc
    SET "isSent" = v_issent
    WHERE cc.id = ANY(v_ids);

    GET DIAGNOSTICS v_rowcount = ROW_COUNT;

    IF v_rowcount > 0 THEN

        -- Get details (first record only)
        SELECT t1."domainId",
               t2.user_id,
               t3.campaignchatid,
               t3."isIndivCamp"
        INTO v_domainid,
             v_userid,
             v_campaignchatid,
             v_isindivcamp
        FROM ec_campaign_contacts_dtl t1
        JOIN tb_customer_contact t2
             ON t1."contactEmail" = t2.primary_email
            AND t1."domainId" = t2.domain_id
        JOIN ec_email_campaign_master_dtl t3
             ON t3."campaignId" = t1."campaignId"
            AND t3."domainId" = t1."domainId"
        WHERE t1.id = v_ids[1]     -- first id
        LIMIT 1;

        IF v_isindivcamp <> 2 and v_userid is not null THEN

            INSERT INTO email_tb_indiv_user_click_open_cnt_dtl(
                domainid,
                campaignchatid,
                userid,
                actionid,
                createdon
            )
            VALUES(
                v_domainid,
                v_campaignchatid,
                v_userid,
                1,
                v_createdon
            );

        ELSE

            INSERT INTO email_tb_indiv_admin_action_dtl(
                domainid,
                campaignchatid,
                userid,
                actionid,
                createdon
            )
            VALUES(
                v_domainid,
                v_campaignchatid,
                v_userid,
                1,
                v_createdon
            );

        END IF;

    END IF;

END;
$$;


ALTER PROCEDURE public.crm_ec_updatecampaignissentdtl(IN p_id text, IN p_issent smallint) OWNER TO postgres;

--
-- Name: crm_email_config_insert_update_dtl(integer, integer, character varying, smallint, jsonb, jsonb, jsonb, jsonb); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_email_config_insert_update_dtl(IN p_emailconfigid integer, IN p_domain_id integer, IN p_first_name character varying, IN p_email_type smallint, IN p_normal_mail jsonb, IN p_gmail_provided jsonb, IN p_outlook jsonb, IN p_smtp_config jsonb)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_count INTEGER := 0;
    v_email_address VARCHAR;
    v_domain_name VARCHAR;
    v_alias_name VARCHAR;
    v_server_type SMALLINT;
    v_is_smtp SMALLINT;
BEGIN
    -- Count existing records for this domain
    SELECT COUNT(*) INTO v_count 
    FROM email_config 
    WHERE "domainId" = p_domain_id 
    AND "EmailConfigId" = p_EmailConfigId;
    
    -- Extract email configuration based on type
    IF p_email_type = 1 THEN
        -- Normal Mail (Worktual provided)
        v_email_address := p_normal_mail->>'mail';
        v_domain_name := p_normal_mail->>'domain_name';
        v_alias_name := p_first_name;
        v_server_type := NULL;
        v_is_smtp := 0;
        
    ELSIF p_email_type = 2 THEN
        -- Gmail Provided (SMTP)
        v_email_address := p_gmail_provided->>'mail';
        v_domain_name := NULL;  -- Don't store config in domainName!
        v_alias_name := p_gmail_provided->>'mail';
        v_server_type := 1;
        v_is_smtp := 1;
        
    ELSIF p_email_type = 3 THEN
        -- Outlook/Microsoft
        v_email_address := p_outlook->>'mail';
        v_domain_name := p_outlook->>'domain';
        v_alias_name := p_outlook->>'mail';
        v_server_type := 0;
        v_is_smtp := 0;
        
    ELSIF p_email_type = 4 THEN
        -- Custom SMTP Configuration
        v_email_address := p_smtp_config->>'mail';
        v_domain_name := NULL;  -- Don't store config in domainName!
        v_alias_name := p_smtp_config->>'alias_name';
        v_server_type := 1;
        v_is_smtp := 1;
        
    END IF;
    
    -- Insert or Update
    IF v_count = 0 THEN
        -- INSERT new record
        INSERT INTO email_config(
            "EmailConfigId",
            "domainId",
            "emailAddress",
            "aliasName",
            "emailtype",
            "serverType",
            "domainName",
            "is_smtp",
            "smtp_json",
            "emailThreshold",
            "isEmail",
            "primaryEmail",
            "isBot",
            "isBotPurchased",
            "product_id",
            "createdAt"
        )
        VALUES (
            p_EmailConfigId,
            p_domain_id,
            v_email_address,
            v_alias_name,
            p_email_type,
            v_server_type,
            v_domain_name,  -- Store actual domain name, NOT JSON
            v_is_smtp,
            CASE 
                WHEN p_email_type = 2 THEN p_gmail_provided  -- Store Gmail SMTP in smtp_json
                WHEN p_email_type = 4 THEN p_smtp_config     -- Store custom SMTP in smtp_json
                ELSE NULL
            END,
            24,  -- Default threshold
            1,   -- isEmail
            0,   -- primaryEmail
            1,   -- isBot
            0,   -- isBotPurchased
            1,   -- product_id
            NOW()
        );
    ELSE
        -- UPDATE existing record
        UPDATE email_config
        SET 
            "emailAddress" = COALESCE(v_email_address, "emailAddress"),
            "aliasName" = COALESCE(v_alias_name, "aliasName"),
            "emailtype" = COALESCE(p_email_type, "emailtype"),
            "serverType" = COALESCE(v_server_type, "serverType"),
            "domainName" = COALESCE(v_domain_name, "domainName"),
            "is_smtp" = COALESCE(v_is_smtp, "is_smtp"),
            "smtp_json" = CASE 
                WHEN p_email_type = 2 THEN p_gmail_provided
                WHEN p_email_type = 4 THEN p_smtp_config
                ELSE "smtp_json"
            END,
            "updatedAt" = NOW()
        WHERE "domainId" = p_domain_id
        AND "EmailConfigId" = p_EmailConfigId;
    END IF;
    
    -- Also update tb_email_config_dtl for backward compatibility
    SELECT COUNT(*) INTO v_count FROM tb_email_config_dtl WHERE domain_id = p_domain_id;
    
    IF v_count = 0 THEN
        INSERT INTO tb_email_config_dtl(
            "EmailConfigId", 
            domain_id, 
            first_name, 
            email_type, 
            normal_mail, 
            gmail_provided, 
            outlook
        )
        VALUES (
            p_EmailConfigId, 
            p_domain_id, 
            p_first_name, 
            p_email_type, 
            p_normal_mail, 
            p_gmail_provided, 
            p_outlook
        );
    ELSE
        UPDATE tb_email_config_dtl 
        SET 
            "EmailConfigId" = COALESCE(p_EmailConfigId, "EmailConfigId"),
            first_name = COALESCE(p_first_name, first_name),
            email_type = COALESCE(p_email_type, email_type),
            normal_mail = COALESCE(p_normal_mail, normal_mail),
            gmail_provided = COALESCE(p_gmail_provided, gmail_provided),
            outlook = COALESCE(p_outlook, outlook),
            "UpdatedOn" = NOW()
        WHERE domain_id = p_domain_id;
    END IF;
END;
$$;


ALTER PROCEDURE public.crm_email_config_insert_update_dtl(IN p_emailconfigid integer, IN p_domain_id integer, IN p_first_name character varying, IN p_email_type smallint, IN p_normal_mail jsonb, IN p_gmail_provided jsonb, IN p_outlook jsonb, IN p_smtp_config jsonb) OWNER TO postgres;

--
-- Name: PROCEDURE crm_email_config_insert_update_dtl(IN p_emailconfigid integer, IN p_domain_id integer, IN p_first_name character varying, IN p_email_type smallint, IN p_normal_mail jsonb, IN p_gmail_provided jsonb, IN p_outlook jsonb, IN p_smtp_config jsonb); Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON PROCEDURE public.crm_email_config_insert_update_dtl(IN p_emailconfigid integer, IN p_domain_id integer, IN p_first_name character varying, IN p_email_type smallint, IN p_normal_mail jsonb, IN p_gmail_provided jsonb, IN p_outlook jsonb, IN p_smtp_config jsonb) IS 'Insert or update email configuration. Properly stores SMTP config in smtp_json field instead of domainName.';


--
-- Name: crm_get_agent_notifications(integer, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_agent_notifications(IN p_domain_id integer, IN p_agent_id integer, INOUT p_cursor refcursor DEFAULT 'agent_notification_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN

    OPEN p_cursor FOR
    SELECT
        nm.notification_id,
        nm.domain_id,
        nm.notification_type,
        nm.title,
        nm.message,
        nm.created_at,
        na.agent_id,
        na.role_id,
        na.is_read,
        na.read_at,
		nm.ref_id,
		dm.customer_id
    FROM notification_master nm
    INNER JOIN notification_agent na ON nm.notification_id = na.notification_id
	LEFT JOIN tb_deal_master dm ON dm.deal_id::VARCHAR = nm.ref_id AND dm.domain_id = nm.domain_id 
    WHERE nm.domain_id = p_domain_id
    AND na.agent_id = p_agent_id
    ORDER BY nm.created_at DESC;

END;
$$;


ALTER PROCEDURE public.crm_get_agent_notifications(IN p_domain_id integer, IN p_agent_id integer, INOUT p_cursor refcursor) OWNER TO postgres;

--
-- Name: crm_get_agent_status_dtl(integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_agent_status_dtl(IN p_domainid integer, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN
    OPEN p_result FOR SELECT aid, "statusName", type, description, channels, "createdAt", "updatedAt", "domainId", status, "isDeleted", "isVoice", "isVoiceCallTransfer", "isChat", "isChatTransfer", "isEmail", "isEmailTransfer", "colorCode"
    FROM agent_status WHERE "domainId" = p_domainId AND "isDeleted" = 0 ORDER BY type DESC, aid DESC;
END;
$$;


ALTER PROCEDURE public.crm_get_agent_status_dtl(IN p_domainid integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_get_business_hours_list_by_name(character varying, integer, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_business_hours_list_by_name(IN p_name character varying, IN p_domainid integer, IN p_fkbusinessid integer, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN
    IF COALESCE(p_name, '') <> '' THEN
        OPEN p_result FOR SELECT cid, name, description, hours, "createdAt", "updatedAt", "domainId", "timeZone", default_hours, "timeZoneValue", type FROM custom_hours WHERE "domainId" = p_domainId AND name ILIKE '%' || p_name || '%';
    ELSE
        OPEN p_result FOR SELECT cid, name, description, hours, "createdAt", "updatedAt", "domainId", "timeZone", default_hours, "timeZoneValue", type FROM custom_hours WHERE "domainId" = p_domainId;
    END IF;
END;
$$;


ALTER PROCEDURE public.crm_get_business_hours_list_by_name(IN p_name character varying, IN p_domainid integer, IN p_fkbusinessid integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_get_business_queue_mapping_validation(integer, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_business_queue_mapping_validation(IN p_domainid integer, IN p_cid integer, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN
    OPEN p_result FOR SELECT qid, name AS "queueName" FROM queue WHERE "domainId" = p_domainId AND "hoursOfOperatioId" = p_cid;
END;
$$;


ALTER PROCEDURE public.crm_get_business_queue_mapping_validation(IN p_domainid integer, IN p_cid integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_get_businessholidays(integer, integer, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_businessholidays(IN p_fkbusinessid integer, IN p_holidayid integer, IN p_domainid integer, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN
    OPEN p_result FOR SELECT hid, "holidayName", "startDate", "endDate", "isStatus", "holidayDesc"
    FROM "tb_Business_Holidays"
    WHERE (COALESCE(p_fkbusinessid, 0) = 0 OR "fk_businessId" = p_fkbusinessid)
      AND "domainId" = p_domainid
      AND (COALESCE(p_holidayid, 0) = 0 OR hid = p_holidayid)
      AND DATE(TO_TIMESTAMP("startDate"::DOUBLE PRECISION / 1000)) >= CURRENT_DATE;
END;
$$;


ALTER PROCEDURE public.crm_get_businessholidays(IN p_fkbusinessid integer, IN p_holidayid integer, IN p_domainid integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_get_businesshoursvalidation_innersp(integer); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_businesshoursvalidation_innersp(IN p_businesshoursid integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_curDate DATE;
    v_holidayStartDate DATE;
    v_holidayEndDate DATE;
    v_holidayStatus SMALLINT;
    v_isHoliday SMALLINT DEFAULT 0;
    v_timezoneTime INTEGER DEFAULT 0;
    v_extractedSign VARCHAR(5);
    v_Business_Hours_Holiday_Flag VARCHAR(10);
    v_Business_Hours_startTime TIME;
    v_Business_Hours_endTime TIME;
    v_currentDayObject JSONB;
    v_timeZone VARCHAR(150);
    v_currentTime TIME;
    v_isBusinessHours SMALLINT DEFAULT 0;
    v_dayName VARCHAR(20);
    rec RECORD;
BEGIN
    v_curDate := CURRENT_DATE;
    v_dayName := TRIM(TO_CHAR(v_curDate, 'Day'));

    -- Get current day object from custom_hours
    SELECT ch."timeZone",
           elem.value AS json_obj
    INTO v_timeZone, v_currentDayObject
    FROM custom_hours ch,
         LATERAL jsonb_array_elements(ch.hours::jsonb) WITH ORDINALITY AS elem(value, idx)
    WHERE ch.cid = p_businessHoursId
      AND elem.value->>'day' = v_dayName
    LIMIT 1;

    DROP TABLE IF EXISTS tt_Business_Hours_Details;
    CREATE TEMP TABLE tt_Business_Hours_Details AS
    SELECT 
        v_timeZone AS "timeZone",
        COALESCE(v_currentDayObject->'entry'->0->>'startTime', '00:00') AS "startTime",
        COALESCE(v_currentDayObject->'entry'->0->>'endTime', '23:59') AS "endTime",
        COALESCE(v_currentDayObject->>'isHoliday', 'false') AS "isHoliday";

    -- Calculate timezone offset in minutes (with error handling)
    BEGIN
        SELECT 
            COALESCE(
                (REGEXP_REPLACE(SPLIT_PART(SPLIT_PART(COALESCE("timeZone",'GMT +00:00'), 'GMT ', 2), ':', 1), '[^0-9-]', '', 'g')::INTEGER * 60) +
                COALESCE(NULLIF(REGEXP_REPLACE(SPLIT_PART(SPLIT_PART(COALESCE("timeZone",'GMT +00:00'), ':', 2), ' ', 1), '[^0-9]', '', 'g'), '')::INTEGER, 0),
                0
            ),
            COALESCE(SUBSTRING(SPLIT_PART(COALESCE("timeZone",'GMT +00:00'), 'GMT', 2), 2, 1), '+')
        INTO v_timezoneTime, v_extractedSign
        FROM tt_Business_Hours_Details;
    EXCEPTION WHEN OTHERS THEN
        v_timezoneTime := 0;
        v_extractedSign := '+';
    END;

    DROP TABLE IF EXISTS tt_Business_Holidays_Info;
    CREATE TEMP TABLE tt_Business_Holidays_Info (
        "idx" SERIAL PRIMARY KEY,
        "startDate" DATE,
        "endDate" DATE,
        "isStatus" SMALLINT
    );

    -- Check if tb_Business_Holidays table exists and insert data
    BEGIN
        INSERT INTO tt_Business_Holidays_Info ("startDate", "endDate", "isStatus")
        SELECT 
            (TO_TIMESTAMP("startDate"::BIGINT / 1000) + (v_timezoneTime || ' minutes')::INTERVAL)::DATE AS "startDate",
            (TO_TIMESTAMP("endDate"::BIGINT / 1000) + (v_timezoneTime || ' minutes')::INTERVAL)::DATE AS "endDate",
            "isStatus"
        FROM "tb_Business_Holidays"
        WHERE "fk_businessId" = p_businessHoursId;
    EXCEPTION WHEN OTHERS THEN
        NULL; -- Table might not exist or have different structure
    END;

    -- Check for holidays
    FOR rec IN SELECT * FROM tt_Business_Holidays_Info LOOP
        IF v_curDate BETWEEN rec."startDate" AND COALESCE(rec."endDate", rec."startDate") THEN
            v_isHoliday := 1;
        END IF;
    END LOOP;

    SELECT "isHoliday" INTO v_Business_Hours_Holiday_Flag FROM tt_Business_Hours_Details;

    IF COALESCE(v_Business_Hours_Holiday_Flag, 'false') <> 'true' THEN
        SELECT "startTime"::TIME, "endTime"::TIME
        INTO v_Business_Hours_startTime, v_Business_Hours_endTime
        FROM tt_Business_Hours_Details;
    END IF;

    v_currentTime := (NOW() + (v_timezoneTime || ' minutes')::INTERVAL)::TIME;

    IF v_currentTime BETWEEN COALESCE(v_Business_Hours_startTime, '00:00'::TIME) AND COALESCE(v_Business_Hours_endTime, '23:59'::TIME) THEN
        v_isBusinessHours := 1;
    END IF;

    DROP TABLE IF EXISTS tt_businessHourLogic;
    CREATE TEMP TABLE tt_businessHourLogic (
        "isHoliday" SMALLINT DEFAULT 0,
        "Business_Hours_Holiday_Flag" VARCHAR(10),
        "isBusinessHours" SMALLINT DEFAULT 0
    );

    INSERT INTO tt_businessHourLogic ("isHoliday", "Business_Hours_Holiday_Flag", "isBusinessHours")
    VALUES (v_isHoliday, v_Business_Hours_Holiday_Flag, v_isBusinessHours);
END;
$$;


ALTER PROCEDURE public.crm_get_businesshoursvalidation_innersp(IN p_businesshoursid integer) OWNER TO postgres;

--
-- Name: crm_get_callmessagebysessionid(character varying, integer); Type: FUNCTION; Schema: public; Owner: postgres
--

CREATE FUNCTION public.crm_get_callmessagebysessionid(p_sessionid character varying, p_domainid integer) RETURNS TABLE(message text)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY
    SELECT CM."Message"
    FROM tb_CallHistory CH
    JOIN tb_CallDurationHistory CDH 
        ON CH."CallHistoryId" = CDH."CallHistoryId"
    LEFT JOIN tb_CallMessage CM  
        ON CM."CallHistoryId" = CH."CallHistoryId"
    WHERE CH."sessionId" = p_sessionid
      AND CH."domainId"  = p_domainid;
END;
$$;


ALTER FUNCTION public.crm_get_callmessagebysessionid(p_sessionid character varying, p_domainid integer) OWNER TO postgres;

--
-- Name: crm_get_callmessagebysessionid(character varying, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_callmessagebysessionid(IN p_sessionid character varying, IN p_domainid integer, INOUT p_cursor refcursor DEFAULT 'p_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN
    OPEN p_cursor FOR
    SELECT CM."Message"
    FROM "tb_CallHistory" CH
    JOIN "tb_CallDurationHistory" CDH ON CH."CallHistoryId" = CDH."CallHistoryId"
    LEFT JOIN "tb_CallMessage" CM ON CM."CallHistoryId" = CH."CallHistoryId"
    WHERE CH."sessionId" = p_sessionid
      AND CH."domainId"  = p_domainid;
END;
$$;


ALTER PROCEDURE public.crm_get_callmessagebysessionid(IN p_sessionid character varying, IN p_domainid integer, INOUT p_cursor refcursor) OWNER TO postgres;

--
-- Name: crm_get_campaign_approval_details(integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_campaign_approval_details(IN p_domain_id integer, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN

	OPEN p_result FOR
    SELECT id,domain_id, campaign_name, sessionid, goal, camp_objactive, approved_status, approved_by, scheduled_time, createdat, updatedat 
    FROM tb_campaign_approval_details
    WHERE domain_id = p_domain_id
	AND approved_status = 0
    ORDER BY id DESC;

	 
END;
$$;


ALTER PROCEDURE public.crm_get_campaign_approval_details(IN p_domain_id integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_get_campaign_approval_list(integer, character varying, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_campaign_approval_list(IN p_domain_id integer, IN p_sessionid character varying, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN

if p_domain_id is not null and p_sessionid is null THEN

    OPEN p_result FOR
    SELECT id,domain_id, campaign_name, sessionid, goal, camp_objactive, approved_status, approved_by, scheduled_time, createdat, updatedat , campaign_chat_id
    FROM tb_campaign_approval_details
    WHERE domain_id = p_domain_id
    ORDER BY id DESC;
	
	
	elseif(p_domain_id is not null and p_sessionid is not null) then
	
	OPEN p_result FOR
    SELECT id,domain_id, campaign_name, sessionid, goal, email_contact, sms_contact, camp_objactive, email_template, sms_template, conversation, approved_status
	, approved_by, scheduled_time, createdat, updatedat , campaign_chat_id
    FROM tb_campaign_approval_details
    WHERE domain_id = p_domain_id and sessionid = p_sessionid
    ORDER BY id DESC;
	
	end if;
	 
END;
$$;


ALTER PROCEDURE public.crm_get_campaign_approval_list(IN p_domain_id integer, IN p_sessionid character varying, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_get_campaign_dtl(character varying, character varying, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_campaign_dtl(IN p_name character varying, IN p_id character varying, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN
    OPEN p_result FOR 
    SELECT 
        "campaignId", "campaignchatid", "createdate", "domainId", "Name", 
        "Goal", "Type", "TargetAud", "Objective", "isIndivCamp", 
        "AB_testing", "AItemplateA", "AItemplateB", "Contactslist", 
        "emailDelivery", "emailDeliveryAt", "emailDeliveryStarttime", 
        "emailDeliveryEndtime", "createdBy", "status" 
    FROM ec_email_campaign_master_dtl 
    WHERE 
        -- Logic: If p_name is NULL, this part is TRUE (ignores filter). 
        -- If p_name has value, it must match "Name".
        (p_name IS NULL OR "Name" = p_name)
        AND 
        -- Logic: If p_id is NULL, this part is TRUE.
        -- If p_id has value, cast it to integer and match "campaignId".
        (p_id IS NULL OR campaignchatid = p_id);
END;
$$;


ALTER PROCEDURE public.crm_get_campaign_dtl(IN p_name character varying, IN p_id character varying, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_get_campaign_open_click_dtl(integer, character varying, character varying, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_campaign_open_click_dtl(IN p_domainid integer, IN p_campaign_chat_id character varying, IN p_emailid_phone_no character varying, INOUT result_cursor refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$

BEGIN
    IF p_emailId_Phone_no IS NULL THEN
        -- Select all records for domainId and campaignId
        OPEN result_cursor FOR
        SELECT
            "id",
            "domainId",
            "campaignId",
			campaign_chat_id,
            "emailId",
            "Phone_no",
            "channel_type",
            "uniqueClick",
            "uniqueOpen",
            "CreatedOn",
            "UpdatedOn"
        FROM "tb_campaign_open_click_rate_history_dtl"
        WHERE "domainId" = p_domainId
          AND campaign_chat_id = p_campaign_chat_id;
    ELSE
        -- Filter by emailId or Phone_no
        OPEN result_cursor FOR
        SELECT
            "id",
            "domainId",
            "campaignId",
			campaign_chat_id,
            "emailId",
            "Phone_no",
            "channel_type",
            "uniqueClick",
            "uniqueOpen",
            "CreatedOn",
            "UpdatedOn"
        FROM "tb_campaign_open_click_rate_history_dtl"
        WHERE "domainId" = p_domainId
          AND campaign_chat_id = p_campaign_chat_id
          AND ("emailId" = p_emailId_Phone_no OR "Phone_no" = p_emailId_Phone_no);
    END IF;

END;
$$;


ALTER PROCEDURE public.crm_get_campaign_open_click_dtl(IN p_domainid integer, IN p_campaign_chat_id character varying, IN p_emailid_phone_no character varying, INOUT result_cursor refcursor) OWNER TO postgres;

--
-- Name: crm_get_campaign_template_conversation_info(integer, character varying, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_campaign_template_conversation_info(IN p_domainid integer, IN p_template_id character varying, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN
    OPEN p_result FOR 
    SELECT 
        id,
        domain_id,
        campaign_chat_id,
        template_id,
        channel,
        conversation,
        template,
        team,
        createdat,
        updatedat
    FROM tb_camapign_template_conversation_dtl
    WHERE domain_id = p_domainid
      AND template_id = p_template_id;
END;
$$;


ALTER PROCEDURE public.crm_get_campaign_template_conversation_info(IN p_domainid integer, IN p_template_id character varying, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_get_chatbotpaymentintegration(integer, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_chatbotpaymentintegration(IN p_companyid integer, IN p_domainid integer, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN

    OPEN p_result FOR
    SELECT *
    FROM ChatBotPaymentIntegration
    WHERE companyid = p_companyid
    AND domainid = p_domainid;

END;
$$;


ALTER PROCEDURE public.crm_get_chatbotpaymentintegration(IN p_companyid integer, IN p_domainid integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_get_company_details(integer, character varying, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_company_details(IN p_domain_id integer, IN p_company_name character varying, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$

BEGIN
    IF p_domain_id IS NOT NULL AND p_company_name IS NULL 
	THEN
        
		OPEN p_result FOR SELECT DISTINCT a.company_id, a.domain_id, a.company_name, a.company_desc AS about, a.company_address, a.company_size, a.contact_person_email_id, a.phone_number, a.owner_name
		, a.user_id, a.company_url, a.source_name, a.industry, a.active_status, a.created_at, a.updated_at,b."UserName" AS lead_name 
		FROM tb_company_dtl a 
		LEFT JOIN "tb_User" b ON a.user_id=b."UserID" 
		WHERE a.domain_id = p_domain_id 
		ORDER BY a.created_at DESC;
   
   ELSIF p_domain_id IS NOT NULL AND p_company_name IS NOT NULL 
   THEN
       
	    OPEN p_result FOR SELECT DISTINCT a.company_id, a.domain_id, a.company_name, a.company_desc AS about, a.company_address, a.company_size, a.contact_person_email_id, a.phone_number, a.owner_name
		, a.user_id, a.company_url, a.source_name, a.industry, a.active_status, a.created_at, a.updated_at ,b."UserName" AS lead_name 
		FROM tb_company_dtl a 
		LEFT  JOIN "tb_User" b ON a.user_id=b."UserID" 
		WHERE a.domain_id = p_domain_id 
		AND a.company_name = p_company_name 
		ORDER BY a.created_at DESC;
    END IF;
END;
$$;


ALTER PROCEDURE public.crm_get_company_details(IN p_domain_id integer, IN p_company_name character varying, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_get_custom_hours_list(integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_custom_hours_list(IN p_domainid integer, INOUT ref refcursor DEFAULT 'ref'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN
    OPEN ref FOR
    SELECT *
    FROM custom_hours
    WHERE "domainId" = p_domainid
    ORDER BY cid DESC;
END;
$$;


ALTER PROCEDURE public.crm_get_custom_hours_list(IN p_domainid integer, INOUT ref refcursor) OWNER TO postgres;

--
-- Name: crm_get_customer_contact_dtl(integer, integer, character varying, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_customer_contact_dtl(IN p_customer_id integer, IN p_domain_id integer, IN p_phone_or_email character varying, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN

	
	OPEN p_result FOR
	SELECT 
	    CC.customer_id, CC.domain_id, CC.first_name, CC.last_name,
	    CC.primary_phone_number, CC.primary_email, CC.address,
	    CC.industry_type, CC.source_type, CC.company_name,
	    CC.company_size, CC.user_id, CC.demo_graphics,
	    CC.firmo_graphics, CC.behaviors, CC.created_at,
	    CC.updated_at, CC.additional_info,
	    STRING_AGG(DISTINCT TCC.secondary_phone_number, ',') AS secondary_phone_number,
	    STRING_AGG(DISTINCT TCC.secondary_email, ',') AS secondary_email,
	    CC.is_active, CC."lifeCycleStage", CC."isUnSub_Email",
	    CC.is_mql, CC.is_sql, CC.sales_agent_id,
	    UC."UserName" AS sales_name
	
	FROM tb_customer_contact CC
	LEFT JOIN tb_secondary_customer_contact TCC ON CC.customer_id = TCC.fk_customer_id AND CC.domain_id = TCC.domain_id
	LEFT JOIN "tb_User" UC ON UC."UserID" = CC.sales_agent_id
	WHERE (COALESCE(p_customer_id,0)=0 OR CC.customer_id=p_customer_id)
	AND (
	        COALESCE(p_phone_or_email,'')=''
	        OR CC.primary_phone_number = p_phone_or_email
	        OR CC.primary_email = p_phone_or_email
	        OR EXISTS (
	            SELECT 1
	            FROM tb_secondary_customer_contact T
	            WHERE T.fk_customer_id = CC.customer_id
	            AND T.domain_id = CC.domain_id
	            AND (
	                    T.secondary_phone_number = p_phone_or_email
	                 OR T.secondary_email = p_phone_or_email
	                )
	        )
	    )
	AND CC.domain_id = p_domain_id
	GROUP BY
	    CC.customer_id, CC.domain_id, CC.first_name, CC.last_name,
	    CC.primary_phone_number, CC.primary_email, CC.address,
	    CC.industry_type, CC.source_type, CC.company_name,
	    CC.company_size, CC.user_id, CC.demo_graphics,
	    CC.firmo_graphics, CC.behaviors, CC.created_at,
	    CC.updated_at, CC.additional_info, CC.is_active,
	    CC."lifeCycleStage", CC."isUnSub_Email",
	    CC.is_mql, CC.is_sql, CC.sales_agent_id,
	    UC."UserName"
	
	ORDER BY CC.created_at DESC;
	END;
$$;


ALTER PROCEDURE public.crm_get_customer_contact_dtl(IN p_customer_id integer, IN p_domain_id integer, IN p_phone_or_email character varying, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_get_customer_deal_details(integer, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_customer_deal_details(IN p_customer_id integer, IN p_domain_id integer, INOUT p_cursor refcursor DEFAULT 'p_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN
    OPEN p_cursor FOR
    SELECT "deal_id",
           "customer_id",
           "domain_id",
           "deal_name",
           "deal_details",
           "total_amount",
           "weighted_amount",
           "open_amount",
           "pending_amount",
           "closed_amount",
           "new_amount",
           "ai_suggest_summary_step",
           "deal_activity",
           "activity_timeline",
           "created_at",
           "updated_at"
    FROM "tb_deal_master"
    WHERE (p_customer_id IS NULL 
           OR "customer_id" = p_customer_id)
      AND "domain_id" = p_domain_id;
END;
$$;


ALTER PROCEDURE public.crm_get_customer_deal_details(IN p_customer_id integer, IN p_domain_id integer, INOUT p_cursor refcursor) OWNER TO postgres;

--
-- Name: crm_get_customer_deal_details_by_deal_id(integer, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_customer_deal_details_by_deal_id(IN p_deal_id integer, IN p_domain_id integer, INOUT p_cursor refcursor DEFAULT 'p_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN
    OPEN p_cursor FOR
    SELECT "deal_id",
           "customer_id",
           "domain_id",
           "deal_name",
           "deal_details",
           "total_amount",
           "weighted_amount",
           "open_amount",
           "pending_amount",
           "closed_amount",
           "new_amount",
           "ai_suggest_summary_step",
           "deal_activity",
           "activity_timeline",
           "created_at",
           "updated_at"
    FROM "tb_deal_master"
    WHERE (p_deal_id IS NULL OR deal_id = p_deal_id)
      AND "domain_id" = p_domain_id;
END;
$$;


ALTER PROCEDURE public.crm_get_customer_deal_details_by_deal_id(IN p_deal_id integer, IN p_domain_id integer, INOUT p_cursor refcursor) OWNER TO postgres;

--
-- Name: crm_get_customer_details(integer, character varying, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_customer_details(IN p_domain_id integer, IN p_company_name character varying, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$

BEGIN
    IF p_domain_id IS NOT NULL AND p_company_name IS NOT NULL 
	THEN
        OPEN p_result 
		FOR 
		SELECT customer_id , CONCAT(first_name, ' ', last_name) AS customer_name, is_active, created_at 
		FROM tb_customer_contact 
		WHERE domain_id = p_domain_id 
		AND company_name = p_company_name;
    END IF;
END;
$$;


ALTER PROCEDURE public.crm_get_customer_details(IN p_domain_id integer, IN p_company_name character varying, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_get_customer_messages_details(integer, character varying, character varying, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_customer_messages_details(IN p_domain_id integer, IN p_phone_no character varying, IN p_email character varying, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN
    -- Scenario 1: Phone provided, Email is NULL
    IF p_phone_no IS NOT NULL AND p_email IS NULL THEN
        OPEN p_result FOR 
            SELECT a."Message",b."sessionId",b."IsInBound",cd."Duration" --
            FROM "tb_CallMessage" a 
            JOIN "tb_CallHistory" b ON a."CallHistoryId" = b."CallHistoryId"
			JOIN "tb_CallDurationHistory" cd ON cd."CallHistoryId" = b."CallHistoryId"
            WHERE b."domainId" = p_domain_id AND b."customerNumber" = p_phone_no
			ORDER BY a."CallDurationHistoryId" DESC LIMIT 1;
            
    -- Scenario 2: Email provided, Phone is NULL
    ELSIF p_phone_no IS NULL AND p_email IS NOT NULL THEN
        OPEN p_result FOR 
            SELECT a."Message" 
            FROM "tb_EmailMessage" a 
            JOIN "tb_EmailHistory" b ON a."EmailHistoryId" = b."EmailHistoryId"
            WHERE b."DomainId" = p_domain_id AND ( b."ToEmail" = p_email OR b."FromEmail" = p_email)
			ORDER BY a."EmailHistoryId";
			
    -- Scenario 3: Both provided OR both are NULL (Combined History)
    ELSEif p_phone_no IS not NULL AND p_email IS NOT NULL then
        OPEN p_result FOR 
            SELECT "Message" FROM (
                SELECT a."Message",b."sessionId",b."IsInBound",cd."Duration"
                FROM "tb_CallMessage" a 
                JOIN "tb_CallHistory" b ON a."CallHistoryId" = b."CallHistoryId"
				JOIN "tb_CallDurationHistory" cd ON cd."CallHistoryId" = b."CallHistoryId"
                WHERE b."domainId" = p_domain_id 
                  AND (p_phone_no IS NULL OR b."customerNumber" = p_phone_no)
                
                UNION ALL
                
                SELECT a."Message" , NULL , NULL, NULL
                FROM "tb_EmailMessage" a 
                JOIN "tb_EmailHistory" b ON a."EmailHistoryId" = b."EmailHistoryId"
                WHERE b."DomainId" = p_domain_id 
                  AND (p_email IS NULL OR b."ToEmail" = p_email)
            ) combined_results;

	else
	
 	OPEN p_result FOR  SELECT '' as "Message";
  		
    END IF;
END;
$$;


ALTER PROCEDURE public.crm_get_customer_messages_details(IN p_domain_id integer, IN p_phone_no character varying, IN p_email character varying, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_get_customer_notes(integer, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_customer_notes(IN p_domain_id integer, IN p_customer_id integer, INOUT p_cursor refcursor DEFAULT 'p_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN

  		OPEN "p_cursor" FOR
        SELECT
			nid,
			"historyId",
			"sessionId",
			"domainId",
			coid,
			notes,
			"createdAt",
			"updatedAt",
			"agentId",
			notes,
			imageurl
        FROM  tb_interaction_notes_info
        WHERE   "domainId"             = p_domain_id
		AND 	(coid = p_customer_id OR p_customer_id IS NULL)
        ORDER BY "updatedAt" DESC;
	

END;
$$;


ALTER PROCEDURE public.crm_get_customer_notes(IN p_domain_id integer, IN p_customer_id integer, INOUT p_cursor refcursor) OWNER TO postgres;

--
-- Name: crm_get_detailed_product_info(bigint, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_detailed_product_info(IN p_fk_product_id bigint, IN p_domain_id integer, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN

    OPEN p_result FOR
    SELECT 
        p_id,
        fk_product_id,
        domain_id,
        product_details,
		total_revenue_growth,
        created_at,
        updated_at
    FROM tb_detailed_product_info
    WHERE fk_product_id = p_fk_product_id
      AND domain_id = p_domain_id;

END;
$$;


ALTER PROCEDURE public.crm_get_detailed_product_info(IN p_fk_product_id bigint, IN p_domain_id integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_get_did_purchase_domain_list(integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_did_purchase_domain_list(IN p_domainid integer, INOUT p_cursor refcursor DEFAULT 'p_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN

    OPEN p_cursor FOR
    SELECT *
    FROM did_purchase
    WHERE "domainId" = p_domainId;

END;
$$;


ALTER PROCEDURE public.crm_get_did_purchase_domain_list(IN p_domainid integer, INOUT p_cursor refcursor) OWNER TO postgres;

--
-- Name: crm_get_did_purchase_list(integer, character varying, character varying, integer, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_did_purchase_list(IN p_domain_id integer, IN p_searchby character varying, IN p_searchbyvalue character varying, IN p_offset integer, IN p_limit integer, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$

DECLARE
    v_offset INTEGER;
    v_searchByValue VARCHAR(202);
BEGIN
    v_offset := p_offset * p_limit;
    v_searchByValue := '%' || COALESCE(p_searchByValue, '') || '%';
    OPEN p_result FOR
    SELECT pid, user_id, type, city, your_number, "createdAt", "updatedAt", "domainId", "callFlowId", recording, "isCall", "isSms", "queueName", "aliasName", "outboundSms", "smsThreshold", status, "callFlowName", "defaultUpdated", "supervisorName", "isCallIn", "isCallOut", "isSmsOut", "isSmsIn", "queueId", country, "isBot", "isBotPurchased", "knowledgeBasedId", "autoAssignAgent"
    FROM did_purchase
    WHERE "domainId" = p_domain_Id
      AND (p_searchBy IS NULL OR p_searchByValue IS NULL
           OR (p_searchBy = 'All' AND (type ILIKE v_searchByValue OR city ILIKE v_searchByValue))
           OR (p_searchBy = 'type' AND type ILIKE v_searchByValue)
           OR (p_searchBy = 'city' AND city ILIKE v_searchByValue))
    ORDER BY "createdAt" DESC, "updatedAt" DESC
    LIMIT p_limit OFFSET v_offset;
END;
$$;


ALTER PROCEDURE public.crm_get_did_purchase_list(IN p_domain_id integer, IN p_searchby character varying, IN p_searchbyvalue character varying, IN p_offset integer, IN p_limit integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_get_email_config_dtl(integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_email_config_dtl(IN p_domainid integer, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$

BEGIN
    IF p_domainId IS NULL THEN
        OPEN p_result FOR SELECT id, "EmailConfigId", domain_id, first_name, email_type, normal_mail, gmail_provided, outlook, "CreatedOn", "UpdatedOn" FROM tb_email_config_dtl;
    ELSE
        OPEN p_result FOR SELECT id, "EmailConfigId", domain_id, first_name, email_type, normal_mail, gmail_provided, outlook, "CreatedOn", "UpdatedOn" FROM tb_email_config_dtl WHERE "domain_id" = p_domainId;
    END IF;
END;
$$;


ALTER PROCEDURE public.crm_get_email_config_dtl(IN p_domainid integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_get_email_details(character varying, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_email_details(IN p_from_email character varying, IN p_domainid integer, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN

    OPEN p_result FOR 
        SELECT 
            "EH"."EmailHistoryId",
            "EH"."SessionId",
            "EH"."SessionStartTime",
            "EH"."SessionEndTime",
            "EH"."FromEmail",
            "EH"."ToEmail",
            "EH"."EmailSubject",
            "EH"."EmailFlowSourceId",
            "EH"."CompanyId",
            "EH"."DomainId",
            "EH"."DispositionId",
            "EH"."dispositionSubject",
            "EH"."Summary",
            "EH"."IsInBound",
            "EH"."AssignedTo",
            "EH"."AssignedBy",
            "EH"."AssignedOn",
            "EH"."waitDuration",
            "EH"."AfterEmailWorkTime",
            "EH"."Istransferred",
            "EH"."transferredTo",
            "EH"."dispositionUpdatedBy",
            "EH"."immediateParentId",
            "EH"."rootParentId",
            "EH"."TransferredTypeId",
            "EH"."transferredDate",
            "EH"."IsSessionClosed",
            "EH"."CustomerId",
            "EH"."TicketId",
            "EH"."SupervisorName",
            "EH"."repliedDate",
            "EH"."discardedDate",
            "EH"."AgentId",
            "EH"."CreatedAt",
            "EH"."UpdatedAt",
            "EH"."SentimentalScore",
            "EH"."sentimentalScoreCustomer",
            "EH"."sentimentalScoreAgent",
            "EH"."isOpened",
            "EH"."replyChannelType",
            "EH"."responseHistoryId",
            "EH"."threadMessageId",
            "EH"."graphMessageId",
            "EH"."conversationId",
            "EH"."tags",
            "EM"."Message"
        FROM "tb_EmailHistory" "EH"
        LEFT JOIN "tb_EmailMessage" "EM"
            ON "EM"."EmailHistoryId" = "EH"."EmailHistoryId"
        WHERE ("EH"."FromEmail" = p_from_email OR  "EH"."ToEmail" = p_from_email)
          AND "EH"."DomainId" = p_domainId;

END;
$$;


ALTER PROCEDURE public.crm_get_email_details(IN p_from_email character varying, IN p_domainid integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_get_emailconfig(integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_emailconfig(IN p_domainid integer, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN
    OPEN p_result FOR SELECT "EmailConfigId", "emailAddress", "aliasName", "emailThreshold", "isEmail", "domainId", "createdAt", "updatedAt", "primaryEmail", "supervisorName", "isBot", "isBotPurchased", "knowledgeBasedId", "autoAssignAgent", product_id, is_smtp, smtp_json, emailtype, "serverType", "isRepliedEmail", "repliedEmail", "domainName", "forwardEmail", host, password FROM email_config WHERE "domainId" = p_domainId;
END;
$$;


ALTER PROCEDURE public.crm_get_emailconfig(IN p_domainid integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_get_inbox_open_history_details(integer, character varying, integer, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_inbox_open_history_details(IN "p_domainId" integer, IN "p_channelType" character varying, IN p_history_id integer, IN "p_AgentId" integer, INOUT p_cursor refcursor DEFAULT 'p_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    "v_CurDate"         TIMESTAMP;
    "v_channelType"     VARCHAR(255);
BEGIN
    "v_CurDate"     := NOW();

    -- Handle NULL or empty channelType
    "v_channelType" := COALESCE(NULLIF("p_channelType", ''), 'Call,SMS,Email,Video');

    -- -------------------------------------------------------
    -- Temp table: split channel types
    -- -------------------------------------------------------
    DROP TABLE IF EXISTS "temp_channeltype";
    CREATE TEMP TABLE "temp_channeltype" AS
        SELECT TRIM("unnest_val") AS "channelType"
        FROM unnest(string_to_array("v_channelType", ',')) AS "unnest_val";

    -- -------------------------------------------------------
    -- Temp table: main history holder
    -- -------------------------------------------------------
    DROP TABLE IF EXISTS "temp_History";
    CREATE TEMP TABLE "temp_History" (
        "id_col"                SERIAL,
        "sessionId"             VARCHAR(250),
        "sessionStartTime"      BIGINT,
        "sessionEndTime"        BIGINT,
        "customerNumber"        VARCHAR(40),
        "companyId"             INT             DEFAULT NULL,
        "domainId"              INT             DEFAULT NULL,
        "dispositionId"         INT             DEFAULT NULL,
        "UMNStatus"             INT             DEFAULT NULL,
        "IsInBound"             SMALLINT        DEFAULT NULL,
        "calledNumber"          BIGINT          DEFAULT NULL,
        "afterCallWorkTime"     INT             DEFAULT NULL,
        "afterVideoWorkTime"    INT             DEFAULT NULL,
        "markAsRead"            SMALLINT        DEFAULT NULL,
        "CustomerId"            INT             DEFAULT NULL,
        "summary"               TEXT            DEFAULT NULL,
        "createdAt"             TIMESTAMP       DEFAULT NULL,
        "updatedAt"             TIMESTAMP       DEFAULT NULL,
        "FromEmail"             VARCHAR(100)    DEFAULT NULL,
        "ToEmail"               VARCHAR(100)    DEFAULT NULL,
        "EmailSubject"          VARCHAR(250)    DEFAULT NULL,
        "IsSessionClosed"       SMALLINT        DEFAULT NULL,
        "Duration"              BIGINT          DEFAULT NULL,
        "AgentId"               BIGINT          DEFAULT NULL,
        "channelType"           VARCHAR(255)    DEFAULT NULL,
        "EmailHistoryId"        INT             DEFAULT NULL,
        "CallHistoryId"         INT             DEFAULT NULL,
        "SMSHistoryId"          INT             	 DEFAULT NULL,
        "RecordingUrl"       	CHARACTER VARYING    DEFAULT NULL,
		"isOpened"				SMALLINT		DEFAULT 0,
		"Message"				JSONB,
		"VideoHistoryId" 		INT DEFAULT NULL,
		"meetingId" VARCHAR(250),
        tags VARCHAR(100),
        schedule_start TIMESTAMP,
        schedule_end TIMESTAMP,
        meeting_title VARCHAR(2000),
        created_by VARCHAR(500),
        meet_url VARCHAR(2500),
		urmeet_user	jsonb,
		voice_mail_summary TEXT,
		voice_mail_subject TEXT
		
    );

    -- -------------------------------------------------------
    -- CALL channel
    -- -------------------------------------------------------
    IF EXISTS (SELECT 1 FROM "temp_channeltype" WHERE "channelType" = 'Call') THEN

        INSERT INTO "temp_History"
        (
            "sessionId", "sessionStartTime", "sessionEndTime", "customerNumber", "domainId",
            "dispositionId", "UMNStatus", "IsInBound", "calledNumber", "afterCallWorkTime",
            "markAsRead", "CustomerId", "summary", "createdAt", "updatedAt",
            "Duration", "AgentId", "channelType", "CallHistoryId","RecordingUrl","Message",
			"voice_mail_summary","voice_mail_subject"
        )
        SELECT
            "clh"."sessionId",
            "clh"."sessionStartTime",
            "clh"."sessionEndTime",
            "clh"."customerNumber",
            "clh"."domainId",
            "cld"."dispositionId",
            "cld"."UMNStatus",
            "clh"."IsInBound",
            "clh"."calledNumber",
            "cld"."afterCallWorkTime",
            "cld"."markAsRead",
            "clh"."CustomerId",
            "cld"."summary",
            "clh"."createdAt",
            "cld"."updatedAt",
            COALESCE("cld"."Duration", 0),
            "cld"."AgentId",
            'Call',
            "clh"."CallHistoryId",
			"cld"."RecordingUrl",
			"cm"."Message",
			"vm"."voice_mail_summary",
			"vm"."voice_mail_subject"
        FROM    "tb_CallHistory"            "clh"
        JOIN    "tb_CallDurationHistory"    "cld" ON "clh"."CallHistoryId" = "cld"."CallHistoryId"
		LEFT JOIN "tb_CallMessage"			"cm" ON "cm"."CallHistoryId" = "clh"."CallHistoryId"
		LEFT JOIN "tb_voice_mail_msg"       "vm" ON "clh"."sessionId" = "vm"."session_id"
		AND     "clh"."domainId" = "vm"."domain_id" 
        WHERE   "clh"."domainId"            = "p_domainId"
		AND 	"clh"."CallHistoryId" = "p_history_id"
        AND     ("p_AgentId"                IS NULL OR "cld"."AgentId"         = "p_AgentId")
        ORDER BY "clh"."domainId", "clh"."updatedAt" DESC;

    END IF;

    -- -------------------------------------------------------
    -- EMAIL channel
    -- -------------------------------------------------------
    IF EXISTS (SELECT 1 FROM "temp_channeltype" WHERE "channelType" = 'Email') THEN

        INSERT INTO "temp_History"
        (
            "sessionId", "sessionStartTime", "sessionEndTime", "companyId", "domainId",
            "dispositionId", "IsInBound", "CustomerId", "summary", "createdAt", "updatedAt",
            "AgentId", "channelType", "EmailHistoryId", "FromEmail", "ToEmail", "EmailSubject", "IsSessionClosed","isOpened","Message"
        )
        SELECT
            "eh"."SessionId",
            "eh"."SessionStartTime",
            "eh"."SessionEndTime",
            "eh"."CompanyId",
            "eh"."DomainId",
            "eh"."DispositionId",
            "eh"."IsInBound",
            "eh"."CustomerId",
            "eh"."Summary",
            "eh"."CreatedAt",
            "eh"."UpdatedAt",
            "eh"."AgentId",
            'Email',
            "eh"."EmailHistoryId",
            "eh"."FromEmail",
            "eh"."ToEmail",
            "eh"."EmailSubject",
            "eh"."IsSessionClosed",
			"eh"."isOpened",
			"em"."Message"
		FROM    "tb_EmailHistory" "eh"
		JOIN 	"tb_EmailMessage" "em" ON "eh"."EmailHistoryId" = "em"."EmailHistoryId"
        WHERE   "eh"."DomainId"             = "p_domainId"
		AND 	"eh"."EmailHistoryId" = "p_history_id"
        AND     ("p_AgentId"                IS NULL OR "eh"."AgentId"    = "p_AgentId")
        ORDER BY "eh"."UpdatedAt" DESC;

    END IF;

	-- -------------------------------------------------------
    -- CALL channel
    -- -------------------------------------------------------
    IF EXISTS (SELECT 1 FROM "temp_channeltype" WHERE "channelType" = 'Video') THEN

        INSERT INTO "temp_History"
        (
            "sessionId", "sessionStartTime", "sessionEndTime", "customerNumber", "domainId",
            "dispositionId", "IsInBound", "afterVideoWorkTime",
            meeting_title, "summary", "createdAt", "updatedAt",
            "Duration", "AgentId", "channelType", "VideoHistoryId","RecordingUrl","Message",
			"meetingId",tags,schedule_start,schedule_end,created_by,meet_url,urmeet_user
        )
        SELECT
            "vh"."sessionId",
            "vh"."sessionStartTime",
            "vh"."sessionEndTime",
            "vh"."customerNumber",
            "vh"."domainId",
            "vdh"."dispositionId",
            "vh"."IsInBound",
            "vdh"."afterVideoWorkTime",
			"vh"."meeting_title",
            "vh"."meeting_description",
            "vdh"."createdAt",
            "vdh"."updatedAt",
            COALESCE("vdh"."agentDuration", 0),
            "vdh"."AgentId",
            'Video',
            "vdh"."VideoHistoryId",
			"vdh"."record_url",
			"vm"."Message",
			"meetingId",
			tags,
			schedule_start,
			schedule_end,
			"UR"."UserName",
			meet_url,
			urmeet_user_list
        FROM "tb_VideoHistory" vh        
		JOIN "tb_VideoDurationHistory" vdh ON vh."VideoHistoryId" = vdh."VideoHistoryId"
		LEFT JOIN "tb_VideoMessage" vm ON vm."VideoHistoryId" = vh."VideoHistoryId"
		LEFT JOIN "tb_User" "UR"   ON "vh"."created_by" = "UR"."UserID" AND "UR"."domainId" = "vh"."domainId"
        WHERE vh."domainId" = "p_domainId"
		AND vh."VideoHistoryId" = p_history_id
        AND ("p_AgentId" IS NULL OR vdh."AgentId" = "p_AgentId")
        ORDER BY vh."domainId", vh."updatedAt" DESC;

    END IF;

    -- -------------------------------------------------------
    -- Open REFCURSOR with final paginated result
    -- -------------------------------------------------------
    OPEN "p_cursor" FOR
    SELECT
        "TH"."sessionId",
        "TH"."sessionStartTime",
        "TH"."sessionEndTime",
        "TH"."customerNumber",
        "TH"."companyId",
        "TH"."domainId",
        "TH"."dispositionId",
        "TH"."UMNStatus",
        "TH"."IsInBound",
        "TH"."calledNumber",
        "TH"."afterCallWorkTime",
        "TH"."markAsRead",
        "TH"."CustomerId",
        "TH"."summary",
        "TH"."createdAt",
        "TH"."updatedAt",
        "TH"."FromEmail",
        "TH"."ToEmail",
        "TH"."EmailSubject",
        "TH"."IsSessionClosed",
        "TH"."Duration",
        "TH"."AgentId",
        "TH"."channelType",
        "TH"."EmailHistoryId",
        "TH"."CallHistoryId",
        "TH"."SMSHistoryId",
        "ds"."dispositionName",
        "U"."UserName" AS "agent_name",
		"TH"."RecordingUrl",
		"TH"."isOpened",
		"TH"."Message",
		"TH"."afterVideoWorkTime",
		"TH".meeting_title,
		"TH"."VideoHistoryId",
		"TH"."meetingId",
		tags,
		schedule_start,
		schedule_end,
		created_by,
		meet_url,
		urmeet_user,
		"TH"."voice_mail_summary",
		"TH"."voice_mail_subject"
    FROM "temp_History" 				"TH"
    LEFT JOIN "tb_User" 				"U"   ON "TH"."AgentId" = "U"."UserID" AND "U"."domainId" = "TH"."domainId"
	LEFT JOIN "disposition_status"      "ds"  ON "ds"."did" = "TH"."dispositionId"
    ORDER BY "TH"."updatedAt" DESC;

END;
$$;


ALTER PROCEDURE public.crm_get_inbox_open_history_details(IN "p_domainId" integer, IN "p_channelType" character varying, IN p_history_id integer, IN "p_AgentId" integer, INOUT p_cursor refcursor) OWNER TO postgres;

--
-- Name: crm_get_onboard_conversation_flow(integer, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_onboard_conversation_flow(IN p_id integer, IN p_domainid integer, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN
    OPEN p_result FOR SELECT id, "domainId", conversation_flow, created_at, updated_at FROM tb_onboard_dtl WHERE "domainId" = p_domainId AND (COALESCE(p_id::TEXT, '') = '' OR id = p_id);
END;
$$;


ALTER PROCEDURE public.crm_get_onboard_conversation_flow(IN p_id integer, IN p_domainid integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_get_particular_company_details(integer, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_particular_company_details(IN p_domain_id integer, IN p_client_company_id integer, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$

BEGIN
       
	    OPEN p_result FOR SELECT DISTINCT a.company_id, a.domain_id, a.company_name, a.company_desc AS about, a.company_address, a.company_size, a.contact_person_email_id, a.phone_number, a.owner_name
		, a.user_id, a.company_url, a.source_name, a.industry, a.active_status, a.created_at, a.updated_at ,b."UserName" AS lead_name 
		FROM tb_company_dtl a 
		LEFT  JOIN "tb_User" b ON a.user_id=b."UserID" 
		WHERE a.domain_id = p_domain_id 
		AND a.company_id = p_client_company_id 
		ORDER BY a.created_at DESC;
END;
$$;


ALTER PROCEDURE public.crm_get_particular_company_details(IN p_domain_id integer, IN p_client_company_id integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_get_particular_project(integer, integer, character varying, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_particular_project(IN p_customer_id integer, IN p_domain_id integer, IN p_product_ref_id character varying, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN

    OPEN p_result FOR
    SELECT 
		pid,
		customer_id,
		domain_id,
		fk_pro_id,
		product_ref_id,
		agentic_insight,
		requirement_list,
		next_steps,
		product_stages,
		requested_requirements,
		team_execution_overview,
		files,
		created_at,
        updated_at
    FROM tb_projects_dtl
    WHERE customer_id = p_customer_id
      AND domain_id = p_domain_id
      AND (product_ref_id = p_product_ref_id OR p_product_ref_id IS NULL);

END;
$$;


ALTER PROCEDURE public.crm_get_particular_project(IN p_customer_id integer, IN p_domain_id integer, IN p_product_ref_id character varying, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_get_product_catalog_master(integer, bigint, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_product_catalog_master(IN p_domain_id integer, IN p_product_id bigint DEFAULT NULL::bigint, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN

    OPEN p_result FOR
    SELECT 
        product_id,
        domain_id,
        product_image,
        product_name,
        short_description,
        product_category,
        starting_price_plan,
        active_customers,
        rating_reviews,
        created_at,
        updated_at
    FROM tb_product_master
    WHERE domain_id = p_domain_id
    AND (p_product_id IS NULL OR product_id = p_product_id);

END;
$$;


ALTER PROCEDURE public.crm_get_product_catalog_master(IN p_domain_id integer, IN p_product_id bigint, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_get_product_details(integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_product_details(IN p_domain_id integer, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN
    OPEN p_result FOR 
        SELECT 
            id,
            domain_id,
            product_insights,
            product_tracker,
            product_catalog,
            product_notes,
            product_quotes,
            product_invoice,
            created_at,
            updated_at
        FROM public.tb_crm_product_details 
        WHERE domain_id = p_domain_id;
END;
$$;


ALTER PROCEDURE public.crm_get_product_details(IN p_domain_id integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_get_product_insights_average_deal_size(integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_product_insights_average_deal_size(IN p_domain_id integer, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN

    OPEN p_result FOR
    SELECT 
        average_deal_size_by_product,
    	average_deal_size_by_product_chart,
        created_at,
        updated_at
    FROM tb_product_insights_info
    WHERE domain_id = p_domain_id;

END;
$$;


ALTER PROCEDURE public.crm_get_product_insights_average_deal_size(IN p_domain_id integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_get_product_insights_performance_ranking(integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_product_insights_performance_ranking(IN p_domain_id integer, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN

    OPEN p_result FOR
    SELECT 
        insight_id,
        domain_id,
        product_performance_ranking,
        created_at,
        updated_at
    FROM tb_product_insights_info
    WHERE domain_id = p_domain_id;

END;
$$;


ALTER PROCEDURE public.crm_get_product_insights_performance_ranking(IN p_domain_id integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_get_product_insights_product_revenue_growth(integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_product_insights_product_revenue_growth(IN p_domain_id integer, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN

    OPEN p_result FOR
    SELECT 
        insight_id,
        domain_id,
        product_revenue_growth,
    	product_revenue_growth_chart,
        created_at,
        updated_at
    FROM tb_product_insights_info
    WHERE domain_id = p_domain_id;

END;
$$;


ALTER PROCEDURE public.crm_get_product_insights_product_revenue_growth(IN p_domain_id integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_get_product_insights_recent_invoices(integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_product_insights_recent_invoices(IN p_domain_id integer, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN

    OPEN p_result FOR
    SELECT 
        recent_invoices,
        created_at,
        updated_at
    FROM tb_product_insights_info
    WHERE domain_id = p_domain_id;

END;
$$;


ALTER PROCEDURE public.crm_get_product_insights_recent_invoices(IN p_domain_id integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_get_product_insights_recent_quotes(integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_product_insights_recent_quotes(IN p_domain_id integer, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN

    OPEN p_result FOR
    SELECT 
        recent_quotes,
        created_at,
        updated_at
    FROM tb_product_insights_info
    WHERE domain_id = p_domain_id;

END;
$$;


ALTER PROCEDURE public.crm_get_product_insights_recent_quotes(IN p_domain_id integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_get_product_insights_revenue_by_product(integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_product_insights_revenue_by_product(IN p_domain_id integer, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN

    OPEN p_result FOR
    SELECT 
        insight_id,
        domain_id,
        total_revenue_by_product,
    	total_revenue_by_product_chart,
        created_at,
        updated_at
    FROM tb_product_insights_info
    WHERE domain_id = p_domain_id;

END;
$$;


ALTER PROCEDURE public.crm_get_product_insights_revenue_by_product(IN p_domain_id integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_get_project_insights(integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_project_insights(IN p_domain_id integer, INOUT p_result refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN

    OPEN p_result FOR
    SELECT
        pid,
        domain_id,
        predictive_project_summary,
        recent_activity,
        requirements_list,
        client_deliverables,
        upcoming_delays_risk_warnings,
        workload_capacity_predictions,
        resource_allocation_insights,
        requirement_stability_rework_prediction,
        operational_efficiency_signals,
        created_by,
        created_at,
        updated_at
    FROM tb_Project_insights
    WHERE domain_id = p_domain_id;

END;
$$;


ALTER PROCEDURE public.crm_get_project_insights(IN p_domain_id integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_get_projects(integer, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_projects(IN p_customer_id integer, IN p_domain_id integer, INOUT p_cursor refcursor DEFAULT 'p_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN

    OPEN p_cursor FOR
    SELECT 
        pro_id,
        customer_id,
        domain_id,
        projests,
        created_at,
        updated_at,
		now() as cur_time
    FROM tb_projects_master
    WHERE customer_id = p_customer_id
    AND domain_id = p_domain_id;

END;
$$;


ALTER PROCEDURE public.crm_get_projects(IN p_customer_id integer, IN p_domain_id integer, INOUT p_cursor refcursor) OWNER TO postgres;

--
-- Name: crm_get_queue_setting(integer, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_queue_setting(IN p_qid integer, IN p_domainid integer, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN
    IF p_qid IS NOT NULL AND p_domainId IS NOT NULL THEN
        OPEN p_result FOR SELECT qid, name, description, "domainId", "createdAt", "updatedAt", "settingAudio", announcement, "maximumCallers", "maximumWaitTime", "OutboundCallerId", "outBoundNumber", "maxCallHandlingTimeMM", "maxCallHandlingTimeSS", "callWrapUpTime", "outBoundCallStatus", "hoursOfOperatioId", hours_of_operation, "outOfBusinessHours", status, type FROM queue WHERE qid = p_qid AND "domainId" = p_domainId;
    ELSIF p_qid IS NULL THEN
        OPEN p_result FOR SELECT qid, name, description, "domainId", "createdAt", "updatedAt", "settingAudio", announcement, "maximumCallers", "maximumWaitTime", "OutboundCallerId", "outBoundNumber", "maxCallHandlingTimeMM", "maxCallHandlingTimeSS", "callWrapUpTime", "outBoundCallStatus", "hoursOfOperatioId", hours_of_operation, "outOfBusinessHours", status, type FROM queue WHERE "domainId" = p_domainId;
    END IF;
END;
$$;


ALTER PROCEDURE public.crm_get_queue_setting(IN p_qid integer, IN p_domainid integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_get_queuesettingscustomtypeinfo(integer, integer, character varying, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_queuesettingscustomtypeinfo(IN p_qid integer, IN p_domainid integer, IN p_cli character varying, INOUT result_cursor refcursor DEFAULT 'result'::refcursor)
    LANGUAGE plpgsql
    AS $$

DECLARE
    v_delay_in_seconds SMALLINT;
    v_routingMethod SMALLINT;
    v_businessHourId INTEGER;
    v_isBusinessHoursResult SMALLINT;
BEGIN
    SELECT "hoursOfOperatioId" INTO v_businessHourId
    FROM "queue"
    WHERE "qid" = p_qid AND "domainId" = p_domainId;

    CALL crm_get_businesshoursvalidation_innersp(v_businessHourId);

    SELECT CASE
        WHEN ("isHoliday" = 1 OR "Business_Hours_Holiday_Flag" = 'true') THEN 0
        ELSE "isBusinessHours"
    END INTO v_isBusinessHoursResult
    FROM tt_businessHourLogic;

    SELECT RM."delay_in_seconds",
           CASE
               WHEN RP."routingMethod" = 'Skill Based Routing' THEN 2
               WHEN RP."routingMethod" = 'Long wait time Based Routing' THEN 1
               ELSE 0
           END
    INTO v_delay_in_seconds, v_routingMethod
    FROM "routingqueueprofilemapping" RM
    JOIN "routing_profile" RP ON RP."rid" = RM."rqmid"
    JOIN "tb_User" U ON U."routing_profile_id" = RM."rqmid"
    WHERE RM."qid" = p_qid AND RM."domainId" = p_domainId
    ORDER BY RM."priority" ASC
    LIMIT 1;

    OPEN result_cursor FOR
    SELECT json_build_object(
        'routingProfile', json_build_object(
            'delay_in_seconds', v_delay_in_seconds,
            'routingMethod', v_routingMethod
        ),
        'queueSettings', json_build_object(
            'qid', b."qid",
            'name', b."name",
            'domainId', b."domainId",
            'settingAudio', b."settingAudio",
            'announcement', b."announcement",
            'maximumCallers', b."maximumCallers",
            'maximumWaitTime', b."maximumWaitTime",
            'OutboundCallerId', b."OutboundCallerId",
            'outBoundNumber', b."outBoundNumber",
            'maxCallHandlingTimeMM', b."maxCallHandlingTimeMM",
            'maxCallHandlingTimeSS', b."maxCallHandlingTimeSS",
            'callWrapUpTime', b."callWrapUpTime",
            'outBoundCallStatus', b."outBoundCallStatus",
            'hoursOfOperatioId', b."hoursOfOperatioId",
            'hours_of_operation', b."hours_of_operation",
            'outOfBusinessHours', b."outOfBusinessHours",
            'status', b."status",
            'type', b."type",
            'hoursOfOperation', v_isBusinessHoursResult,
            'agentCount', (
                SELECT COUNT(*)
                FROM "tb_User" U
                WHERE U."domainId" = p_domainId
                  AND U."routing_profile_id" IN (
                      SELECT qm."rqmid" FROM "routingqueueprofilemapping" qm
                      WHERE qm."qid" IN (SELECT qb."qid" FROM "queue" qb WHERE qb."domainId" = p_domainId AND qb."qid" = p_qid)
                        AND qm."domainId" = p_domainId
                  )
            )
        )
    ) AS jsoncol
    FROM "queue" b
    WHERE "qid" = p_qid AND "domainId" = p_domainId;

    DROP TABLE IF EXISTS tt_businessHourLogic;
END;
$$;


ALTER PROCEDURE public.crm_get_queuesettingscustomtypeinfo(IN p_qid integer, IN p_domainid integer, IN p_cli character varying, INOUT result_cursor refcursor) OWNER TO postgres;

--
-- Name: crm_get_reminder_details(refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_reminder_details(INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN

    -- Drop and recreate temp table
    DROP TABLE IF EXISTS tt_meeting_reminder;
    CREATE TEMP TABLE tt_meeting_reminder AS
    SELECT 
        a."VideoHistoryId",
        a."meetingId",
        a."sessionId",
        a."domainId",
        a.meet_url,
        a.schedule_start,
        a.schedule_end,
        a.meeting_title,
        a.meeting_description,
        b.urmeet_user_list
    FROM "tb_VideoHistory" a 
    JOIN "tb_VideoDurationHistory" b ON a."VideoHistoryId" = b."VideoHistoryId"
    WHERE TO_CHAR(a.schedule_start, 'YYYY-MM-DD HH24:MI') = TO_CHAR(NOW() + INTERVAL '30 minutes', 'YYYY-MM-DD HH24:MI')
    AND a.is_reminder = 0;

    -- Return result
    OPEN p_result FOR 
    SELECT 
        "VideoHistoryId",
        "meetingId",
        "sessionId",
        "domainId",
        meet_url,
        schedule_start,
        schedule_end,
        meeting_title,
        meeting_description,
        urmeet_user_list
    FROM tt_meeting_reminder;

    UPDATE "tb_VideoHistory" vh
    SET is_reminder = 1
    FROM tt_meeting_reminder mr
    WHERE vh."meetingId" = mr."meetingId";

END;
$$;


ALTER PROCEDURE public.crm_get_reminder_details(INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_get_role_master(integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_role_master(IN p_domain_id integer, INOUT p_cursor refcursor DEFAULT 'role_master_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN

    OPEN p_cursor FOR
    SELECT *
    FROM "tb_role_master"
    WHERE (domain_id = p_domain_id OR domain_id IS NULL)
    ORDER BY role_id;

END;
$$;


ALTER PROCEDURE public.crm_get_role_master(IN p_domain_id integer, INOUT p_cursor refcursor) OWNER TO postgres;

--
-- Name: crm_get_scheduled_meetings(integer, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_scheduled_meetings(IN v_domain_id integer, IN v_customer_id integer, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$

BEGIN
        OPEN p_result FOR 
           SELECT "VideoHistoryId",
                  "meetingId",
                  "sessionId",
				  "sessionStartTime",
                  "sessionEndTime",
                  "customerNumber",
				  "domainId",
				  "IsInBound",
				  "dialDuration",
				  "disconnectedBy",
				  "queueDuration",
				  "calledNumber",
				  "waitDuration", 
				  "recordingTypeId",
				  "CustomerId",
				  "videoDuration",
				  "createdAt",
				  "updatedAt",
				  "IsAbandoned",
				  "businessStartTime",
				  "businessEndTime",
				  tags,
				  meet_url,
				  schedule_start,
				  schedule_end,
				  meeting_title,
				  meeting_description,
				  created_by
                        FROM "tb_VideoHistory"
                        WHERE "domainId" = v_domain_id
						AND "CustomerId" = v_customer_id
						AND schedule_start >= now()
                        ORDER BY schedule_start DESC;

END;
$$;


ALTER PROCEDURE public.crm_get_scheduled_meetings(IN v_domain_id integer, IN v_customer_id integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_get_screen_module(refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_screen_module(INOUT p_cursor refcursor DEFAULT 'screen_module_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN

    OPEN p_cursor FOR
    SELECT *
    FROM "tb_screen_module";

END;
$$;


ALTER PROCEDURE public.crm_get_screen_module(INOUT p_cursor refcursor) OWNER TO postgres;

--
-- Name: crm_get_search_list_queue(character varying, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_search_list_queue(IN p_name character varying, IN p_domainid integer, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN
    IF COALESCE(p_name, '') <> '' THEN
        OPEN p_result FOR SELECT q.qid, q.name, q.description, q."domainId", q."createdAt", q."updatedAt", q."settingAudio", q.announcement, q."maximumCallers", q."maximumWaitTime", q."OutboundCallerId", q."outBoundNumber", q."maxCallHandlingTimeMM", q."maxCallHandlingTimeSS", q."callWrapUpTime", q."outBoundCallStatus", q."hoursOfOperatioId", q.hours_of_operation, q."outOfBusinessHours", q.status, q.type, c.name AS "businessName" FROM queue q LEFT JOIN custom_hours c ON c.cid = q."hoursOfOperatioId" WHERE q."domainId" = p_domainId AND q.name ILIKE '%' || p_name || '%';
    ELSE
        OPEN p_result FOR SELECT q.qid, q.name, q.description, q."domainId", q."createdAt", q."updatedAt", q."settingAudio", q.announcement, q."maximumCallers", q."maximumWaitTime", q."OutboundCallerId", q."outBoundNumber", q."maxCallHandlingTimeMM", q."maxCallHandlingTimeSS", q."callWrapUpTime", q."outBoundCallStatus", q."hoursOfOperatioId", q.hours_of_operation, q."outOfBusinessHours", q.status, q.type, c.name AS "businessName" FROM queue q LEFT JOIN custom_hours c ON c.cid = q."hoursOfOperatioId" WHERE q."domainId" = p_domainId;
    END IF;
END;
$$;


ALTER PROCEDURE public.crm_get_search_list_queue(IN p_name character varying, IN p_domainid integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_get_secondary_contact_details(integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_secondary_contact_details(IN p_customer_id integer, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN
    OPEN p_result FOR 
        SELECT 
			fk_customer_id, 
			domain_id,
			secondary_phone_number,
			secondary_email,
			created_date,
			updated_date
        FROM tb_secondary_customer_contact
        WHERE fk_customer_id = p_customer_id;
END;
$$;


ALTER PROCEDURE public.crm_get_secondary_contact_details(IN p_customer_id integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_get_sms_campaign_dtl(character varying, character varying, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_sms_campaign_dtl(IN p_name character varying, IN p_campaignchatid character varying, INOUT result_cursor refcursor DEFAULT 'result'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN
    -- PostgreSQL handles isolation at the transaction level, 
    -- 'READ UNCOMMITTED' acts as 'READ COMMITTED' in Postgres.

    IF p_name IS NOT NULL OR p_campaignchatid IS NOT NULL THEN
	
        OPEN result_cursor FOR SELECT id, campaignchatid, "createdDate", "domainId", name, "campaignGoal", "campaignObjective", "websiteURL", "targetAudienceType", "contactDtls", "senderName", "templatePrompt", "triggerType", status, updateddate FROM sc_campaign_master_dtl WHERE (name = p_name or campaignchatid = p_campaignchatid );
		
    ELSE
	
        OPEN result_cursor FOR SELECT id, campaignchatid, "createdDate", "domainId", name, "campaignGoal", "campaignObjective", "websiteURL", "targetAudienceType", "contactDtls", "senderName", "templatePrompt", "triggerType", status, updateddate FROM sc_campaign_master_dtl;

    END IF;
END;
$$;


ALTER PROCEDURE public.crm_get_sms_campaign_dtl(IN p_name character varying, IN p_campaignchatid character varying, INOUT result_cursor refcursor) OWNER TO postgres;

--
-- Name: crm_get_total_agent_assigned_user_count(integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_total_agent_assigned_user_count(IN p_domain_id integer, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN

    OPEN p_result FOR 
    SELECT 
        a."UserID",
        a."UserName",
        COUNT(b.sales_agent_id) AS total_agents
    FROM "tb_User" a
    JOIN tb_customer_contact b
        ON a."domainId" = b.domain_id
       AND a."UserID" = b.sales_agent_id
    WHERE a."domainId" = p_domain_id
    GROUP BY a."UserID", a."UserName";

END;
$$;


ALTER PROCEDURE public.crm_get_total_agent_assigned_user_count(IN p_domain_id integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_get_user_dtl(integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_user_dtl(IN p_domainid integer, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$

BEGIN
    OPEN p_result FOR 
	SELECT "U"."UserID", "U"."UserName", "U"."emailId", "U"."companyId", "U"."domainId", "U"."ext", "U"."roleid", "U"."queueId", "U"."isActive", "U"."StatusID", "U"."companyName", "U"."inboundCall", "U"."outboundCampaignCall", "U"."isVoice"
	, "U"."isVoiceCallTransfer", "U"."isAvailableForVoice", "U"."isEmail", "U"."isEmailTransfer", "U"."isSms", "U"."ProfileImage", "U"."directNumber", "U"."address", "U"."routing_profile_id", "U"."report_to_UserID", "U"."sipLoginId",
	"U"."createdAt", "U"."updatedAt", "U"."addSkill", "U"."callTypeId", "U"."voice", "U"."email", "U"."sms", "U"."region", "U"."state", "U"."experience_level", "U"."performance_score","AL"."pri_language","AL"."sec_language"
	FROM "tb_User" "U"
	LEFT JOIN "tb_userAddonLanguageInfo" "AL" ON "AL"."domainId" = "U"."domainId" AND "U"."UserID" = "AL"."userId"
	WHERE "U"."domainId" = p_domainId;
END;
$$;


ALTER PROCEDURE public.crm_get_user_dtl(IN p_domainid integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_get_user_onboard_settings_completion(character varying, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_user_onboard_settings_completion(IN p_email_id character varying, IN p_domainid integer, INOUT p_cursor refcursor DEFAULT 'p_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN
    -- Note: SET TRANSACTION is not allowed here because procedures 
    -- run within the caller's transaction context.

    OPEN p_cursor FOR
    SELECT 
        email_id, 
        "domainId", 
        flow_track, 
        "isCompleted", 
        "createdAt", 
        "updatedAt"
    FROM tb_user_onboard_setting_flow
    WHERE email_id = p_email_id
      AND "domainId" = p_domainId;
END;
$$;


ALTER PROCEDURE public.crm_get_user_onboard_settings_completion(IN p_email_id character varying, IN p_domainid integer, INOUT p_cursor refcursor) OWNER TO postgres;

--
-- Name: crm_get_user_queue_list(integer, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_user_queue_list(IN p_domainid integer, IN p_ext integer, INOUT result_cursor refcursor DEFAULT 'result'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN
    OPEN result_cursor FOR
    SELECT json_agg(
        json_build_object(
            'qid', CAST(a."queueId" AS INTEGER),
            'priority', 1,
            'delay', 0,
            'routingMethod', NULL,
            'agent_skills', (
                SELECT json_agg(json_build_object('Skid', SK.skid, 'value', ss.skill_name))
                FROM "skillMapping" SK
                JOIN skills ss ON ss.sid = SK.skid AND ss."domainId" = SK."domainId"
                WHERE SK.usid = a."UserID" AND SK."domainId" = p_domainId
            )
        )
    ) AS jsoncol
    FROM "tb_User" a
    WHERE a."domainId" = p_domainId
      AND a.ext = p_ext
      AND a."isVoice" = 1
      AND a."isActive" = 1
      AND a.roleid IN (3, 5, 6);
END;
$$;


ALTER PROCEDURE public.crm_get_user_queue_list(IN p_domainid integer, IN p_ext integer, INOUT result_cursor refcursor) OWNER TO postgres;

--
-- Name: crm_get_user_queue_mapping_validation(integer, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_user_queue_mapping_validation(IN p_domainid integer, IN p_qid integer, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN
    OPEN p_result FOR SELECT "UserID", "UserName" FROM "tb_User" WHERE "domainId" = p_domainId AND "queueId"::TEXT LIKE '%' || p_qid::TEXT || '%';
END;
$$;


ALTER PROCEDURE public.crm_get_user_queue_mapping_validation(IN p_domainid integer, IN p_qid integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_get_user_status_details(integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_user_status_details(IN p_domainid integer, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN
    OPEN p_result FOR
    SELECT b."statusName",
           a."UserID",
           a."UserName",
           a."emailId",
           a."directNumber",
           a.address,
           a.routing_profile_id,
           a."report_to_UserID",
           a."sipLoginId",
           a."companyId",
           a."domainId",
           a.ext,
           a."isActive",
           a."callTypeId",
           a.roleid,
           a.localization,
           a."currentSession",
           b."isVoice",
           a."isVoiceCallTransfer",
           b."isChat",
           a."isChatTransfer",
           a."isEmail",
           a."isEmailTransfer",
           a."StatusID",
           a."isSms",
           a."isSocialMedia",
           a."companyName",
           a."addSkill",
           a.voice,
           a.chat,
           a.email,
           a.sms,
           a."socialMedia",
           a."stateTimer",
           b."isVoiceCallTransfer" AS "isVoiceCallTransfer_AgentStatus",
           b."isChatTransfer" AS "isChatTransfer_AgentStatus",
           b."isEmailTransfer" AS "isEmailTransfer_AgentStatus",
           a."queueId" AS "quId"
    FROM "tb_User" a
    JOIN agent_status b ON a."StatusID" = b.aid
    WHERE a."domainId" = p_domainId
      AND b."statusName" = 'Ready';
END;
$$;


ALTER PROCEDURE public.crm_get_user_status_details(IN p_domainid integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_get_video_meet_message(integer, character varying, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_get_video_meet_message(IN p_domainid integer, IN p_meeting_id character varying, INOUT p_result refcursor)
    LANGUAGE plpgsql
    AS $$
begin


	OPEN p_result FOR
	SELECT  vh."VideoHistoryId",vh."meetingId",vm."Message"
	FROM "tb_VideoHistory" vh
	JOIN "tb_VideoMessage" vm ON vh."VideoHistoryId" = vm."VideoHistoryId"
	WHERE vh."domainId" = p_domainid
	AND vh."meetingId" = p_meeting_id;

end
$$;


ALTER PROCEDURE public.crm_get_video_meet_message(IN p_domainid integer, IN p_meeting_id character varying, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_getcallconfigurationlist(integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_getcallconfigurationlist(IN p_domainid integer, INOUT p_cursor refcursor DEFAULT 'p_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN

    OPEN p_cursor FOR
        SELECT  CC."contactCenterNumber",
                CC.name,
                CC.status,
                CC."isMultilingualStatus",
                CC."Language",
                CC."isBot",
                CC."virtualAgent",
                CC."assistPriority",
                CC."humanAgent"
        FROM    call_configuration CC
        WHERE   CC."domainId" = p_domainId;

END;
$$;


ALTER PROCEDURE public.crm_getcallconfigurationlist(IN p_domainid integer, INOUT p_cursor refcursor) OWNER TO postgres;

--
-- Name: crm_getcallhistorybysessionid(character varying, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_getcallhistorybysessionid(IN p_sessionid character varying, IN p_domainid integer, INOUT result_cursor refcursor DEFAULT 'result'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_emailAddress VARCHAR(200);
    v_your_number VARCHAR(255);
BEGIN
    -- Get email address
    SELECT "emailAddress" INTO v_emailAddress
    FROM email_config
    WHERE "domainId" = p_domainId
    LIMIT 1;

    -- Get SMS number
    SELECT your_number INTO v_your_number
    FROM did_purchase
    WHERE "domainId" = p_domainId
      AND "isSmsIn" = 1
      AND "isSmsOut" = 1
    LIMIT 1;

    -- Return call history with message
    OPEN result_cursor FOR
    SELECT 
        'Call'::VARCHAR AS channeltype,
        v_emailAddress AS "companyEmailAddress",
        v_your_number AS "companySMSNumber",
        CH."CallHistoryId",
        CH."sessionId",
        CH."sessionStartTime",
        CH."sessionEndTime",
        CH."customerNumber",
        CH."domainId",
        CH."IsInBound",
        CH."isOutboundCampaign",
        CH."dialDuration",
        CH."disconnectedBy",
        CH."queueDuration",
        CH."assignedTo",
        CH."assignedBy",
        CH."assignedOn",
        CH."calledNumber",
        CH."waitDuration",
        CH."recordingTypeId",
        CH."CustomerId",
        CH."voicemailUrl",
        CH."isVoiceMailAnswered",
        CH."voiceMailAnsweredCallHistoryId",
        CH."callDuration",
        CH."callBack",
        CH."CallBackCallHistoryId",
        CH."voiceMailDuration",
        CH."createdAt",
        CH."updatedAt",
        CH."callCost",
        CH."IsAbandoned",
        CH."businessStartTime",
        CH."businessEndTime",
        CH."isVMCallBackCompleted",
        CH."IsOutOfBussinessHour",
        CM."Message"
    FROM "tb_CallHistory" CH
    JOIN "tb_CallDurationHistory" CDH ON CH."CallHistoryId" = CDH."CallHistoryId"
    LEFT JOIN "tb_CallMessage" CM ON CM."CallHistoryId" = CH."CallHistoryId"
    WHERE CH."sessionId" = p_sessionId
      AND CH."domainId" = p_domainId;
END;
$$;


ALTER PROCEDURE public.crm_getcallhistorybysessionid(IN p_sessionid character varying, IN p_domainid integer, INOUT result_cursor refcursor) OWNER TO postgres;

--
-- Name: crm_getsessionhistorydetails(integer, character varying, integer, character varying, character varying, integer, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_getsessionhistorydetails(IN p_domainid integer, IN p_channeltype character varying, IN p_agentid integer, IN p_customer_number character varying, IN p_customer_email character varying, IN p_offset integer, IN p_limit integer, INOUT p_cursor refcursor DEFAULT 'p_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_curdate   TIMESTAMP;
    v_offset    INT;
BEGIN

    v_curdate := CURRENT_TIMESTAMP;
    v_offset  := p_offset * p_limit;

    IF p_channeltype IS NULL THEN
        p_channeltype := 'Call,SMS,Email';
    END IF;

    ---------------------------------------------------
    -- TEMP TABLE: channel types
    ---------------------------------------------------
    DROP TABLE IF EXISTS temp_channeltype;
    CREATE TEMP TABLE temp_channeltype(channeltype VARCHAR);

    INSERT INTO temp_channeltype(channeltype)
    SELECT trim(value)
    FROM unnest(string_to_array(p_channeltype, ',')) AS value;

    ---------------------------------------------------
    -- TEMP TABLE: History
    ---------------------------------------------------
    DROP TABLE IF EXISTS temp_history;

    CREATE TEMP TABLE temp_history
    (
        sessionid           VARCHAR(250),
        sessionstarttime    BIGINT,
        sessionendtime      BIGINT,
        customernumber      VARCHAR(40),
        companyid           INT,
        domainid            INT,
        dispositionid       INT,
        umnstatus           INT,
        isinbound           SMALLINT,
        callednumber        BIGINT,
        aftercallworktime   INT,
        markasread          SMALLINT,
        customerid          INT,
        summary             TEXT,
        createdat           TIMESTAMP,
        updatedat           TIMESTAMP,
        fromemail           VARCHAR(100),
        toemail             VARCHAR(100),
        emailsubject        VARCHAR(250),
        issessionclosed     SMALLINT,
        duration            BIGINT,
        agentid             BIGINT,
        channeltype         VARCHAR(255),
        emailhistoryid      INT,
        callhistoryid       INT,
        smshistoryid        INT,
        dispositionname     VARCHAR(100)
    );

    ---------------------------------------------------
    -- CALL DATA
    ---------------------------------------------------
    IF EXISTS (SELECT 1 FROM temp_channeltype WHERE channeltype = 'Call') THEN

        INSERT INTO temp_history
        (
            sessionid, sessionstarttime, sessionendtime, customernumber,
            domainid, dispositionid, umnstatus, isinbound, callednumber,
            aftercallworktime, markasread, customerid, summary,
            createdat, updatedat, duration, agentid, channeltype, callhistoryid
        )
        SELECT
            clh.sessionid,
            clh.sessionstarttime,
            clh.sessionendtime,
            clh.customernumber,
            clh.domainid,
            cld.dispositionid,
            cld.umnstatus,
            clh.isinbound,
            clh.callednumber,
            cld.aftercallworktime,
            cld.markasread,
            clh.customerid,
            cld.summary,
            clh.createdat,
            cld.updatedat,
            COALESCE(cld.duration,0),
            cld.agentid,
            'Call',
            clh.callhistoryid
        FROM tb_callhistory clh
        JOIN tb_calldurationhistory cld
              ON clh.callhistoryid = cld.callhistoryid
        WHERE clh.domainid = p_domainid
          AND (cld.agentid = p_agentid OR p_agentid IS NULL)
          AND (clh.customernumber = p_customer_number OR p_customer_number IS NULL)
        ORDER BY clh.updatedat DESC;

    END IF;

    ---------------------------------------------------
    -- EMAIL DATA
    ---------------------------------------------------
    IF EXISTS (SELECT 1 FROM temp_channeltype WHERE channeltype = 'Email') THEN

        INSERT INTO temp_history
        (
            sessionid, sessionstarttime, sessionendtime,
            companyid, domainid, dispositionid, isinbound,
            customerid, summary, createdat, updatedat,
            agentid, channeltype, emailhistoryid,
            fromemail, toemail, emailsubject, issessionclosed
        )
        SELECT
            eh.sessionid,
            eh.sessionstarttime,
            eh.sessionendtime,
            eh.companyid,
            eh.domainid,
            eh.dispositionid,
            eh.isinbound,
            eh.customerid,
            eh.summary,
            eh.createdat,
            eh.updatedat,
            eh.agentid,
            'Email',
            eh.emailhistoryid,
            eh.fromemail,
            eh.toemail,
            eh.emailsubject,
            eh.issessionclosed
        FROM tb_emailhistory eh
        WHERE eh.domainid = p_domainid
          AND (eh.agentid = p_agentid OR p_agentid IS NULL)
          AND (eh.fromemail = p_customer_email OR p_customer_email IS NULL)
        ORDER BY eh.updatedat DESC;

    END IF;

    ---------------------------------------------------
    -- FINAL RESULT
    ---------------------------------------------------
    OPEN p_cursor FOR
    SELECT
        th.*,
        u.username AS agent_name
    FROM temp_history th
    LEFT JOIN tb_user u
           ON th.agentid = u.userid
          AND u.domainid = th.domainid
    LIMIT p_limit OFFSET v_offset;

END;
$$;


ALTER PROCEDURE public.crm_getsessionhistorydetails(IN p_domainid integer, IN p_channeltype character varying, IN p_agentid integer, IN p_customer_number character varying, IN p_customer_email character varying, IN p_offset integer, IN p_limit integer, INOUT p_cursor refcursor) OWNER TO postgres;

--
-- Name: crm_getuserdetailslist_info(integer, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_getuserdetailslist_info(IN p_domainid integer, IN p_userid integer, INOUT result_cursor refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$

DECLARE
    v_reporting_to_name VARCHAR(255);
    v_reporting_to_userid BIGINT;
BEGIN
    -- Get reporting manager's user ID
    SELECT u."report_to_UserID"
    INTO v_reporting_to_userid
    FROM "tb_User" u
    WHERE u."domainId" = p_domainId
      AND u."UserID" = p_UserID;

    -- Get reporting manager's name
    SELECT u."UserName"
    INTO v_reporting_to_name
    FROM "tb_User" u
    WHERE u."domainId" = p_domainId
      AND u."UserID" = v_reporting_to_userid;

    -- Open cursor with results
    OPEN result_cursor FOR
    SELECT
        v_reporting_to_name AS reporting_to_name,
        b."name" AS routing_profile_name,
        ual."pri_language" AS "primaryLanguage",
        ual."sec_language" AS "secondaryLanguage",
        a."UserID",
        a."UserName",
        a."emailId",
        a."directNumber",
        a."address",
        a."routing_profile_id",
        a."report_to_UserID",
        a."sipLoginId",
        a."companyId",
        a."domainId",
        a."ext",
        a."isActive",
        a."callTypeId",
        a."inboundCall",
        a."outboundCampaignCall",
        a."roleid",
        a."localization",
        a."currentSession",
        a."isVoice",
        a."isVoiceCallTransfer",
        a."isAvailableForVoice",
        a."isChat",
        a."isChatTransfer",
        a."isEmail",
        a."isEmailTransfer",
        a."StatusID",
        a."isSms",
        a."isSocialMedia",
        a."companyName",
        (
            SELECT json_agg(
                json_build_object(
                    'skid', sk."sid",
                    'value', sk."skill_name",
                    'type', sk."type"
                )
            )
            FROM "skills" sk
            JOIN "skillMapping" sm ON sk."sid" = sm."skid"
            WHERE sk."domainId" = a."domainId"
              AND sm."usid" = a."UserID"
        ) AS "addSkill",
        a."voice",
        a."chat",
        a."email",
        a."sms",
        a."socialMedia",
        a."chatLimit",
        a."createdAt",
        a."updatedAt",
        a."stateTimer",
        a."AssignedTime",
        a."chatFlag",
        a."ProfileImage",
        a."emp_id",
        a."queueId"
    FROM "tb_User" a
    LEFT JOIN "routing_profile" b
        ON b."rid" = a."routing_profile_id"
        AND a."domainId" = b."domainId"
    LEFT JOIN "tb_userAddonLanguageInfo" ual
        ON a."UserID" = ual."userId"
    WHERE a."domainId" = p_domainId
      AND a."UserID" = p_UserID;

END;
$$;


ALTER PROCEDURE public.crm_getuserdetailslist_info(IN p_domainid integer, IN p_userid integer, INOUT result_cursor refcursor) OWNER TO postgres;

--
-- Name: crm_insert_agent_skill(integer, integer, jsonb, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_insert_agent_skill(IN p_domainid integer, IN p_userid integer, IN p_addskill jsonb, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_userId INTEGER;
    v_errcode INTEGER := -1;
    v_errmsg VARCHAR(150) := 'Failed';
    v_curDate TIMESTAMP := NOW();
BEGIN
    -- Check if user exists
    SELECT "UserID" INTO v_userId
    FROM "tb_User"
    WHERE "domainId" = p_domainId
      AND "UserID" = p_userId;

    IF v_userId IS NOT NULL THEN

        -- Create temp table for parsed skills
        CREATE TEMP TABLE IF NOT EXISTS tt_addSkill (
            j_type VARCHAR(250),
            j_value VARCHAR(250)
        ) ON COMMIT DROP;

        TRUNCATE tt_addSkill;

        -- Parse JSON into temp table
        INSERT INTO tt_addSkill (j_type, j_value)
        SELECT jt.type, jt.value
        FROM jsonb_to_recordset(p_addSkill)
             AS jt(type VARCHAR(250), value VARCHAR(250));

        -- Insert new skills if not exists
        INSERT INTO skills (
            skill_name,
            skill_description,
            "domainId",
            "createdAt",
            "updatedAt",
            status,
            type
        )
        SELECT
            t.j_value,
            t.j_value,
            p_domainId,
            v_curDate,
            v_curDate,
            1,
            t.j_type
        FROM tt_addSkill t
        LEFT JOIN skills s
               ON s.skill_name = t.j_value
              AND s."domainId" = p_domainId
        WHERE s.skill_name IS NULL;

        -- Delete existing skill mappings for this user
        DELETE FROM "skillMapping"
        WHERE usid = v_userId
          AND "domainId" = p_domainId;

        -- Insert new skill mappings (WITHOUT updatedAt column)
        INSERT INTO "skillMapping" (
            skid,
            usid,
            "domainId",
            "createdAt"
        )
        SELECT s.sid,
               v_userId,
               p_domainId,
               NOW()
        FROM skills s
        JOIN tt_addSkill t
             ON s.skill_name = t.j_value
            AND s."domainId" = p_domainId;

        -- Update user's addSkill column
        UPDATE "tb_User"
        SET "addSkill" = p_addSkill
		,"updatedAt" = now()
        WHERE "UserID" = v_userId
          AND "domainId" = p_domainId;

        v_errcode := 0;
        v_errmsg := 'Skills inserted/mapped successfully';

    ELSE
        v_errcode := -1;
        v_errmsg := 'User not found';
    END IF;

    OPEN p_result FOR
        SELECT v_errcode AS errcode,
               v_errmsg AS errmsg;

END;
$$;


ALTER PROCEDURE public.crm_insert_agent_skill(IN p_domainid integer, IN p_userid integer, IN p_addskill jsonb, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_insert_notification(integer, character varying, character varying, character varying, character varying, character varying, character varying, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_insert_notification(IN p_domain_id integer, IN p_notification_type character varying, IN p_title character varying, IN p_message character varying, IN p_role_id character varying, IN p_ext character varying, IN p_ref_id character varying, INOUT p_cursor refcursor DEFAULT 'p_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE 
    v_notification_id BIGINT;
    v_errcode   SMALLINT := -1;
    v_errmsg    VARCHAR(250) := 'Notification insert failed';
BEGIN

    -- Create temporary role table
    DROP TABLE IF EXISTS temp_role_id;
    CREATE TEMP TABLE temp_role_id(role_id SMALLINT);

    INSERT INTO temp_role_id(role_id)
    SELECT unnest(string_to_array(p_role_id, ',')::SMALLINT[]);

	DROP TABLE IF EXISTS temp_ext;
	CREATE TEMP TABLE temp_ext(ext_no SMALLINT);

	INSERT INTO temp_ext(ext_no)
	SELECT unnest(string_to_array(p_ext, ',')::SMALLINT[]);

    -- Get users for those roles
	IF p_role_id IS NOT NULL
	THEN
	    DROP TABLE IF EXISTS temp_user_details;
	    CREATE TEMP TABLE temp_user_details AS
	    SELECT U."UserID", U.roleid
	    FROM "tb_User" U
	    JOIN temp_role_id TR ON U.roleid = TR.role_id
		AND U."domainId" = p_domain_id
	    WHERE U."domainId" = p_domain_id;
	ELSE 
		DROP TABLE IF EXISTS temp_user_details;
	    CREATE TEMP TABLE temp_user_details AS
	    SELECT U."UserID", U.roleid
	    FROM "tb_User" U
		JOIN temp_ext TE ON U.ext = TE.ext_no
		AND U."domainId" = p_domain_id		
	    WHERE U."domainId" = p_domain_id;
	END IF;

    -- Insert notification
    INSERT INTO notification_master (domain_id,notification_type,title,message,ref_id)
    VALUES (p_domain_id,p_notification_type,p_title,p_message,p_ref_id)
    RETURNING notification_id INTO v_notification_id;

    -- Insert notification_agent records
    INSERT INTO notification_agent (notification_id,agent_id,role_id,is_read)
    SELECT v_notification_id,TM."UserID",TM.roleid,0
    FROM temp_user_details TM;

    v_errcode := 0;
    v_errmsg  := 'Notification inserted successfully';

    -- Return status
    OPEN p_cursor FOR
    SELECT v_errcode AS errcode,
           v_errmsg  AS errmsg,
           v_notification_id AS notification_id;

EXCEPTION WHEN OTHERS THEN
    OPEN p_cursor FOR
    SELECT -1 AS errcode,
           SQLERRM AS errmsg,
           NULL::BIGINT AS notification_id;
END;
$$;


ALTER PROCEDURE public.crm_insert_notification(IN p_domain_id integer, IN p_notification_type character varying, IN p_title character varying, IN p_message character varying, IN p_role_id character varying, IN p_ext character varying, IN p_ref_id character varying, INOUT p_cursor refcursor) OWNER TO postgres;

--
-- Name: crm_insert_onboard_user(integer, character varying, character varying, bigint, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, character varying, text, character varying, character varying, integer, character varying, jsonb, character varying, character varying, character varying, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_insert_onboard_user(IN p_userid integer, IN p_username character varying, IN p_emailid character varying, IN p_directnumber bigint, IN p_siploginid integer, IN p_companyid integer, IN p_domainid integer, IN p_ext integer, IN p_isactive integer, IN p_calltypeid integer, IN p_roleid integer, IN p_isvoice integer, IN p_isvoicecalltransfer integer, IN p_isemail integer, IN p_isemailtransfer integer, IN p_issms integer, IN p_voice integer, IN p_email integer, IN p_sms integer, IN p_companyname character varying, IN p_profileimage text, IN p_pri_language character varying, IN p_emp_id character varying, IN p_productid integer, IN p_department character varying, IN p_permissions jsonb, IN p_region character varying, IN p_state character varying, IN p_experience_level character varying, IN p_performance_score integer, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$

DECLARE
    v_errcode INTEGER := -1;
    v_errmsg VARCHAR(255) := 'Already Exists';
    v_CurDate TIMESTAMP;
    v_userId INTEGER;
    v_exists INTEGER;
BEGIN
    v_CurDate := NOW();

    -- Check if UserID is null or empty
    IF p_UserID IS NULL THEN
        v_errcode := -1;
        v_errmsg := 'UserID should not be null';
        OPEN p_result FOR SELECT v_errcode AS errcode, v_errmsg AS errmsg, NULL::INTEGER AS "userId";
        RETURN;
    END IF;

    -- Check if user already exists
    SELECT COUNT(*) INTO v_exists
    FROM "tb_User"
    WHERE ("emailId" = p_emailId OR emp_id = p_emp_id)
      AND "domainId" = p_domainId;

    IF v_exists = 0 THEN
        -- Insert into tb_User
        INSERT INTO "tb_User"(
            "UserID", "UserName", "emailId", "directNumber", "sipLoginId",
            "companyId", "domainId", ext, "isActive", "callTypeId", roleid,
            "isVoice", "isVoiceCallTransfer", "isEmail", "isEmailTransfer",
            "isSms", "companyName", voice, email, sms, "ProfileImage",
            emp_id, permissions, "createdAt", "updatedAt",region,state,experience_level,performance_score
        )
        VALUES(
            p_UserID, p_UserName, p_emailId, p_directNumber, p_sipLoginId,
            p_companyId, p_domainId, p_ext, p_isActive, p_callTypeId, p_roleid,
            p_isVoice, p_isVoiceCallTransfer, p_isEmail, p_isEmailTransfer,
            p_isSms, p_companyName, p_voice, p_email, p_sms, p_ProfileImage,
            p_emp_id, p_permissions, v_CurDate, v_CurDate,p_region,p_state,p_experience_level,p_performance_score
        )
        RETURNING uid INTO v_userId;

        -- Insert into tb_userAddonLanguageInfo
        INSERT INTO "tb_userAddonLanguageInfo"("domainId", "userId", ext, pri_language, "createdAt", "updatedAt")
        VALUES(p_domainId, p_UserID, p_ext, p_pri_language, NOW(), NOW());

        v_errcode := 0;
        v_errmsg := 'User inserted successfuly';
    END IF;

    OPEN p_result FOR SELECT v_errcode AS errcode, v_errmsg AS errmsg, COALESCE(p_UserID, v_userId) AS "userId";
END;
$$;


ALTER PROCEDURE public.crm_insert_onboard_user(IN p_userid integer, IN p_username character varying, IN p_emailid character varying, IN p_directnumber bigint, IN p_siploginid integer, IN p_companyid integer, IN p_domainid integer, IN p_ext integer, IN p_isactive integer, IN p_calltypeid integer, IN p_roleid integer, IN p_isvoice integer, IN p_isvoicecalltransfer integer, IN p_isemail integer, IN p_isemailtransfer integer, IN p_issms integer, IN p_voice integer, IN p_email integer, IN p_sms integer, IN p_companyname character varying, IN p_profileimage text, IN p_pri_language character varying, IN p_emp_id character varying, IN p_productid integer, IN p_department character varying, IN p_permissions jsonb, IN p_region character varying, IN p_state character varying, IN p_experience_level character varying, IN p_performance_score integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_insert_prompt_text(character varying, character varying, character varying, character varying, text, integer, character varying, integer, text, character varying, character varying, integer, integer, character varying, character varying, integer, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_insert_prompt_text(IN p_fieldname character varying, IN p_encoding character varying, IN p_mimetype character varying, IN p_filename character varying, IN p_path text, IN p_size integer, IN p_type character varying, IN p_prompttype integer, IN p_url text, IN p_name character varying, IN p_description character varying, IN p_domainid integer, IN p_companyid integer, IN p_message character varying, IN p_promptstatus character varying, IN p_queueid integer, IN p_uuid integer, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_errcode   SMALLINT := -1;
    v_errmsg    VARCHAR(30) := 'Failed';
    v_uuid      INTEGER;
    v_insertid  INTEGER := -1;
BEGIN

    -- Check if UUID exists
    SELECT uuid INTO v_uuid
    FROM prompt_library
    WHERE uuid = p_uuid
      AND "domainId" = p_domainid;

    IF v_uuid IS NULL THEN

        INSERT INTO prompt_library (
            fieldname, encoding, mimetype, filename, "path",
            size, "type", "promptType", url, "name",
            "description", "domainId", "companyId",
            message, "promptStatus", "createdAt", "queueId"
        )
        VALUES (
            p_fieldname, p_encoding, p_mimetype, p_filename, p_path,
            p_size, p_type, p_prompttype, p_url, p_name,
            p_description, p_domainid, p_companyid,
            p_message, p_promptstatus, NOW(), p_queueid
        )
        RETURNING uuid INTO v_insertid;

        v_errcode := 0;
        v_errmsg  := 'Inserted Successfully';

    ELSE

        UPDATE prompt_library
        SET fieldname      = COALESCE(p_fieldname, fieldname),
            encoding       = COALESCE(p_encoding, encoding),
            mimetype       = COALESCE(p_mimetype, mimetype),
            filename       = COALESCE(p_filename, filename),
            "path"         = COALESCE(p_path, "path"),
            size           = COALESCE(p_size, size),
            "type"         = COALESCE(p_type, "type"),
            "promptType"   = COALESCE(p_prompttype, "promptType"),
            url            = COALESCE(p_url, url),
            "name"         = COALESCE(p_name, "name"),
            "description"  = COALESCE(p_description, "description"),
            message        = COALESCE(p_message, message),
            "promptStatus" = COALESCE(p_promptstatus, "promptStatus"),
            "queueId"      = COALESCE(p_queueid, "queueId")
			, "updateddAt" = now()
        WHERE uuid = v_uuid;

        v_insertid := v_uuid;
        v_errcode  := 0;
        v_errmsg   := 'Updated Successfully';

    END IF;

    OPEN p_result FOR
        SELECT v_errcode AS "errCode",
               v_errmsg  AS "errMsg",
               v_insertid AS "Id";

END;
$$;


ALTER PROCEDURE public.crm_insert_prompt_text(IN p_fieldname character varying, IN p_encoding character varying, IN p_mimetype character varying, IN p_filename character varying, IN p_path text, IN p_size integer, IN p_type character varying, IN p_prompttype integer, IN p_url text, IN p_name character varying, IN p_description character varying, IN p_domainid integer, IN p_companyid integer, IN p_message character varying, IN p_promptstatus character varying, IN p_queueid integer, IN p_uuid integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_insert_revert_status_details(integer, character varying, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_insert_revert_status_details(IN p_domainid integer, IN p_campaignchatid character varying, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_errCode SMALLINT := -1;
    v_errMsg VARCHAR(50) := 'failed';
    v_exists INTEGER;
    v_row_count INTEGER;
BEGIN
    -- Check if record already exists
    SELECT COUNT(*) INTO v_exists
    FROM wc_master_revert_status_details
    WHERE "domainId" = p_domainId
      AND "campaignchatId" = p_campaignchatId;

    IF v_exists = 0 THEN
        -- Insert new record
        INSERT INTO wc_master_revert_status_details("campaignchatId", "domainId", "isReverted")
        VALUES(p_campaignchatId, p_domainId, 1);

        -- Get row count
        GET DIAGNOSTICS v_row_count = ROW_COUNT;

        IF v_row_count > 0 THEN
            v_errCode := 0;
            v_errMsg := 'Inserted';
        END IF;
    END IF;

    OPEN p_result FOR SELECT v_errCode AS "errCode", v_errMsg AS "v_errMsg";
END;
$$;


ALTER PROCEDURE public.crm_insert_revert_status_details(IN p_domainid integer, IN p_campaignchatid character varying, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_insert_update_agent_status(integer, character varying, character varying, character varying, character varying, integer, integer, integer, integer, integer, integer, integer, character varying, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_insert_update_agent_status(IN p_aid integer, IN p_statusname character varying, IN p_type character varying, IN p_description character varying, IN p_channels character varying, IN p_status integer, IN p_domainid integer, IN p_isdeleted integer, IN p_isvoice integer, IN p_isvoicecalltransfer integer, IN p_isemail integer, IN p_isemailtransfer integer, IN p_colorcode character varying, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$

DECLARE
    v_aid INTEGER;
    v_errcode INTEGER := -1;
    v_errmsg VARCHAR(100) := 'Agent status insert / update fail';
    v_agentStatusId INTEGER;
BEGIN
    -- Check if agent status exists
    SELECT aid INTO v_aid
    FROM agent_status
    WHERE aid = p_aid
      AND "domainId" = p_domainId;

    IF v_aid IS NULL THEN
        -- Insert new agent status
        INSERT INTO agent_status(
            "statusName", type, description, channels, status, "domainId",
            "createdAt", "isVoice", "isVoiceCallTransfer", "isEmail",
            "isEmailTransfer", "colorCode", "isDeleted"
        )
        VALUES(
            p_statusName, p_type, p_description, p_channels, p_status, p_domainId,
            NOW(), p_isVoice, p_isVoiceCallTransfer, p_isEmail,
            p_isEmailTransfer, p_colorCode, p_isDeleted
        )
        RETURNING aid INTO v_agentStatusId;

        v_errcode := 0;
        v_errmsg := 'Agent status inserted successfully';
    ELSE
        -- Update existing agent status
        UPDATE agent_status
        SET "statusName" = COALESCE(p_statusName, "statusName"),
            type = COALESCE(p_type, type),
            description = COALESCE(p_description, description),
            channels = COALESCE(p_channels, channels),
            status = COALESCE(p_status, status),
            "isDeleted" = COALESCE(p_isDeleted, "isDeleted"),
            "isVoice" = COALESCE(p_isVoice, "isVoice"),
            "isVoiceCallTransfer" = COALESCE(p_isVoiceCallTransfer, "isVoiceCallTransfer"),
            "isEmail" = COALESCE(p_isEmail, "isEmail"),
            "isEmailTransfer" = COALESCE(p_isEmailTransfer, "isEmailTransfer"),
            "colorCode" = COALESCE(p_colorCode, "colorCode"),
            "updatedAt" = NOW()
        WHERE aid = v_aid;

        v_errcode := 1;
        v_errmsg := 'Agent status updated successfully';
    END IF;

    OPEN p_result FOR SELECT v_errcode AS errcode, v_errmsg AS errmsg, COALESCE(v_agentStatusId, v_aid) AS aid;
END;
$$;


ALTER PROCEDURE public.crm_insert_update_agent_status(IN p_aid integer, IN p_statusname character varying, IN p_type character varying, IN p_description character varying, IN p_channels character varying, IN p_status integer, IN p_domainid integer, IN p_isdeleted integer, IN p_isvoice integer, IN p_isvoicecalltransfer integer, IN p_isemail integer, IN p_isemailtransfer integer, IN p_colorcode character varying, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_insert_update_businessHolidays(integer, integer, integer, character varying, bigint, bigint, text, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public."crm_insert_update_businessHolidays"(IN p_holidayid integer, IN p_domainid integer, IN p_fkbusinessid integer, IN p_holidayname character varying, IN p_startdate bigint, IN p_enddate bigint, IN p_holidaydesc text, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_errcode INTEGER := -1;
    v_errmsg VARCHAR(60) := 'Holiday Insert/Update Failed';
    v_HolidayId INTEGER;
    v_curdate TIMESTAMP;
BEGIN
    v_curdate := NOW();

    -- Check if holiday exists
    SELECT hid INTO v_HolidayId
    FROM "tb_Business_Holidays"
    WHERE hid = p_holidayId
      AND "fk_businessId" = p_fkBusinessId
      AND "domainId" = p_domainId;

    IF v_HolidayId IS NULL THEN
        -- Insert new holiday
        INSERT INTO "tb_Business_Holidays"(
            "domainId", "fk_businessId", "holidayName", "startDate",
            "endDate", "holidayDesc", "createdAt", "updatedAt"
        )
        VALUES(
            p_domainId, p_fkBusinessId, p_holidayName, p_startDate,
            p_endDate, p_holidayDesc, v_curdate, v_curdate
        );

        v_errcode := 0;
        v_errmsg := 'Business holiday inserted successfully';
    ELSE
        -- Update existing holiday
        UPDATE "tb_Business_Holidays"
        SET "holidayName" = COALESCE(p_holidayName, "holidayName"),
            "startDate" = COALESCE(p_startDate, "startDate"),
            "endDate" = COALESCE(p_endDate, "endDate"),
            "holidayDesc" = COALESCE(p_holidayDesc, "holidayDesc"),
            "updatedAt" = v_curdate
        WHERE hid = v_HolidayId;

        v_errcode := 1;
        v_errmsg := 'Business holiday updated successfully';
    END IF;

    OPEN p_result FOR SELECT v_errcode AS errcode, v_errmsg AS errmsg;
END;
$$;


ALTER PROCEDURE public."crm_insert_update_businessHolidays"(IN p_holidayid integer, IN p_domainid integer, IN p_fkbusinessid integer, IN p_holidayname character varying, IN p_startdate bigint, IN p_enddate bigint, IN p_holidaydesc text, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_insert_update_call_configuration(text, bigint, integer, text, text, smallint, jsonb, integer, smallint, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_insert_update_call_configuration(IN p_name text, IN p_contactcenternumber bigint, IN p_domainid integer, IN p_language text, IN p_greetingmessage text, IN p_status smallint, IN p_intents jsonb, IN p_threshold integer, IN p_ismultilingualstatus smallint, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$

DECLARE
    v_CurDate TIMESTAMP;
    v_ErrCode INTEGER := -1;
    v_ErrMsg VARCHAR(100) := 'Call Configuration';
    v_contactCenterNumber BIGINT;
    v_queueId INTEGER;
    v_cfid VARCHAR(255);
BEGIN
    v_CurDate := NOW();

    -- Check if call configuration exists
    SELECT "contactCenterNumber" INTO v_contactCenterNumber
    FROM call_configuration
    WHERE "contactCenterNumber" = p_contactCenterNumber;

    IF v_contactCenterNumber IS NULL THEN
        v_contactCenterNumber := p_contactCenterNumber;

        -- Insert new call configuration
        INSERT INTO call_configuration(
            "contactCenterNumber", name, "domainId", "Language",
            "greetingMessage", status, threshold, "createdDate",
            "updatedDate", "isMultilingualStatus"
        )
        VALUES(
            v_contactCenterNumber, p_name, p_domainId, p_Language,
            p_greetingMessage, p_status, p_threshold, v_CurDate,
            v_CurDate, p_isMultilingualStatus
        );

        v_ErrCode := 0;
        v_ErrMsg := 'Call Configuration Inserted Successfully';
    ELSE
        -- Update existing call configuration
        UPDATE call_configuration
        SET name = COALESCE(p_name, name),
            "Language" = COALESCE(p_Language, "Language"),
            "greetingMessage" = COALESCE(p_greetingMessage, "greetingMessage"),
            status = COALESCE(p_status, status),
            threshold = COALESCE(p_threshold, threshold),
            "updatedDate" = v_CurDate,
            "isMultilingualStatus" = COALESCE(p_isMultilingualStatus, "isMultilingualStatus")
			-- ,"updatedDate" = now()
        WHERE "contactCenterNumber" = v_contactCenterNumber
          AND "domainId" = p_domainId;

        v_ErrCode := 0;
        v_ErrMsg := 'Call Configuration Updated Successfully';
    END IF;

    -- Update did_purchase table
    UPDATE did_purchase
    SET status = COALESCE(p_status, status)
    WHERE your_number::bigint = v_contactCenterNumber
      AND "domainId" = p_domainId;

    OPEN p_result FOR SELECT v_ErrCode AS "errCode", v_ErrMsg AS "errMsg";
END;
$$;


ALTER PROCEDURE public.crm_insert_update_call_configuration(IN p_name text, IN p_contactcenternumber bigint, IN p_domainid integer, IN p_language text, IN p_greetingmessage text, IN p_status smallint, IN p_intents jsonb, IN p_threshold integer, IN p_ismultilingualstatus smallint, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_insert_update_campaign_approval_dtl(integer, character varying, character varying, character varying, json, json, json, text, text, json, integer, character varying, timestamp without time zone, character varying, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_insert_update_campaign_approval_dtl(IN p_domain_id integer, IN p_campaign_name character varying, IN p_sessionid character varying, IN p_goal character varying, IN p_email_contact json, IN p_sms_contact json, IN p_camp_objactive json, IN p_email_template text, IN p_sms_template text, IN p_conversation json, IN p_approved_status integer, IN p_approved_by character varying, IN p_scheduled_time timestamp without time zone, IN p_campaign_chat_id character varying, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_existing_id INTEGER;
    v_id INTEGER;
    v_errcode INTEGER := -1;
    v_errmsg VARCHAR(50) := 'Failure';
	v_campaign_chat_id VARCHAR(50);
BEGIN

     SELECT id INTO v_existing_id FROM tb_campaign_approval_details WHERE domain_id = p_domain_id and sessionid= p_sessionid;

    IF v_existing_id IS NULL THEN
        
         INSERT INTO tb_campaign_approval_details
        (domain_id, campaign_name, sessionid, goal, email_contact, sms_contact, camp_objactive, email_template, sms_template, conversation, approved_status, approved_by, scheduled_time, createdat, updatedat , campaign_chat_id)
        VALUES(p_domain_id, p_campaign_name, p_sessionid, p_goal, p_email_contact, p_sms_contact, p_camp_objactive, p_email_template, p_sms_template, p_conversation, p_approved_status, p_approved_by, p_scheduled_time, now(), now(), p_campaign_chat_id)
		RETURNING campaign_chat_id INTO v_campaign_chat_id;
        v_errcode := 0;
        v_errmsg  := 'Inserted Successfully';

    ELSE
        
    
        UPDATE tb_campaign_approval_details
        SET
            campaign_name =COALESCE(p_campaign_name, campaign_name),
			goal =COALESCE(p_goal, goal),
			email_contact =COALESCE(p_email_contact, email_contact),
			sms_contact =COALESCE(p_sms_contact, sms_contact),
			camp_objactive =COALESCE(p_camp_objactive, camp_objactive),
			email_template =COALESCE(p_email_template, email_template),
			conversation = COALESCE(conversation::jsonb, '[]'::jsonb) || p_conversation::jsonb,			approved_by =COALESCE(p_approved_by, approved_by),
			approved_status =COALESCE(p_approved_status, approved_status),
			scheduled_time =COALESCE(p_scheduled_time, scheduled_time),
			campaign_chat_id =COALESCE(p_campaign_chat_id, campaign_chat_id),
            updatedat = now()
    WHERE domain_id = p_domain_id and sessionid= p_sessionid
	RETURNING campaign_chat_id INTO v_campaign_chat_id;

        v_errcode := 1;
        v_errmsg  := 'Updated Successfully';

    END IF;

     OPEN p_result FOR SELECT v_errcode AS errcode, v_errmsg  AS errmsg, v_campaign_chat_id AS campaign_chat_id;
END;
$$;


ALTER PROCEDURE public.crm_insert_update_campaign_approval_dtl(IN p_domain_id integer, IN p_campaign_name character varying, IN p_sessionid character varying, IN p_goal character varying, IN p_email_contact json, IN p_sms_contact json, IN p_camp_objactive json, IN p_email_template text, IN p_sms_template text, IN p_conversation json, IN p_approved_status integer, IN p_approved_by character varying, IN p_scheduled_time timestamp without time zone, IN p_campaign_chat_id character varying, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_insert_update_campaign_template_conversation_info(integer, character varying, character varying, character varying, jsonb, jsonb, smallint, integer, character varying); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_insert_update_campaign_template_conversation_info(IN p_domain_id integer, IN p_campaign_chat_id character varying, IN p_template_id character varying, IN p_channel character varying, IN p_conversation jsonb, IN p_template jsonb, IN p_team smallint, INOUT errcode integer DEFAULT NULL::integer, INOUT errmsg character varying DEFAULT NULL::character varying)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_id BIGINT;
BEGIN
    -- Search for existing record
    SELECT id INTO v_id
    FROM tb_camapign_template_conversation_dtl
    WHERE domain_id = p_domain_id
      AND template_id = p_template_id
    LIMIT 1;

    IF v_id IS NOT NULL THEN
        UPDATE tb_camapign_template_conversation_dtl
        SET conversation = COALESCE(conversation, '[]'::jsonb) || p_conversation,
		updatedat =now()
        WHERE id = v_id;

        errcode := 1;
        errmsg := 'Updated successfully';
    ELSE
        INSERT INTO tb_camapign_template_conversation_dtl(
            domain_id, campaign_chat_id, template_id, channel, conversation, template, team
        )
        VALUES(
            p_domain_id, p_campaign_chat_id, p_template_id, p_channel, p_conversation, p_template, p_team
        );

        errcode := 0;
        errmsg := 'Inserted successfully';
    END IF;
END;
$$;


ALTER PROCEDURE public.crm_insert_update_campaign_template_conversation_info(IN p_domain_id integer, IN p_campaign_chat_id character varying, IN p_template_id character varying, IN p_channel character varying, IN p_conversation jsonb, IN p_template jsonb, IN p_team smallint, INOUT errcode integer, INOUT errmsg character varying) OWNER TO postgres;

--
-- Name: crm_insert_update_company_info(integer, integer, character varying, text, text, character varying, character varying, character varying, character varying, character varying, character varying, integer, integer, character varying, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_insert_update_company_info(IN p_comp_id integer, IN p_domain_id integer, IN p_company_name character varying, IN p_company_desc text, IN p_company_address text, IN p_company_size character varying, IN p_contact_person_email_id character varying, IN p_phone_number character varying, IN p_owner_name character varying, IN p_source_name character varying, IN p_industry character varying, IN p_active_status integer, IN p_user_id integer, IN p_company_url character varying, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$

DECLARE
    v_errcode INTEGER := -1;
    v_errmsg VARCHAR(250) := 'failed to create company';
    v_comp_id INTEGER;
BEGIN
    -- Check if company exists
    SELECT company_id INTO v_comp_id
    FROM tb_company_dtl
    WHERE company_id = p_comp_id
      AND domain_id = p_domain_id;

    IF v_comp_id IS NOT NULL THEN
        -- Update existing company
        UPDATE tb_company_dtl
        SET company_name = COALESCE(p_company_name, company_name),
            company_desc = COALESCE(p_company_desc, company_desc),
            company_address = COALESCE(p_company_address, company_address),
            company_size = COALESCE(p_company_size, company_size),
            contact_person_email_id = COALESCE(p_contact_person_email_id, contact_person_email_id),
            phone_number = COALESCE(p_phone_number, phone_number),
            owner_name = COALESCE(p_owner_name, owner_name),
            source_name = COALESCE(p_source_name, source_name),
            industry = COALESCE(p_industry, industry),
            active_status = COALESCE(p_active_status, active_status),
            user_id = COALESCE(p_user_id, user_id),
            company_url = COALESCE(p_company_url, company_url),
			updated_at = now()
        WHERE company_id = v_comp_id;

        v_errcode := 1;
        v_errmsg := 'Company details updated successfully';
    ELSE
        -- Insert new company
        INSERT INTO tb_company_dtl(
            company_name, company_desc, company_address, company_size,
            domain_id, contact_person_email_id, phone_number, owner_name,
            source_name, industry, active_status, user_id, company_url
        )
        VALUES(
            p_company_name, p_company_desc, p_company_address, p_company_size,
            p_domain_id, p_contact_person_email_id, p_phone_number, p_owner_name,
            p_source_name, p_industry, p_active_status, p_user_id, p_company_url
        );

        v_errcode := 0;
        v_errmsg := 'Company details inserted successfully';
    END IF;

    OPEN p_result FOR SELECT v_errcode AS errcode, v_errmsg AS errmsg;
END;
$$;


ALTER PROCEDURE public.crm_insert_update_company_info(IN p_comp_id integer, IN p_domain_id integer, IN p_company_name character varying, IN p_company_desc text, IN p_company_address text, IN p_company_size character varying, IN p_contact_person_email_id character varying, IN p_phone_number character varying, IN p_owner_name character varying, IN p_source_name character varying, IN p_industry character varying, IN p_active_status integer, IN p_user_id integer, IN p_company_url character varying, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_insert_update_createbusinesshours(integer, character varying, character varying, text, integer, character varying, character varying, character varying, character varying, jsonb, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_insert_update_createbusinesshours(IN p_cid integer, IN p_name character varying, IN p_description character varying, IN p_hours text, IN p_domainid integer, IN p_timezone character varying, IN p_default_hours character varying, IN p_timezonevalue character varying, IN p_type character varying, IN p_holiday jsonb, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$

DECLARE
    v_cid INTEGER;
    v_curdate TIMESTAMP;
    v_errcode SMALLINT := -1;
    v_errmsg VARCHAR(150) := 'Create Business Hours Failed';
    v_lastId INTEGER;
    v_exists INTEGER;
    v_p_timeZone VARCHAR(100);
    v_p_hours JSONB;
BEGIN
    v_curdate := NOW();
    v_p_timeZone := p_timeZone;
    v_p_hours := p_hours::JSONB;

    -- Fix timezone format
    IF v_p_timeZone = 'Asia/Kolkata (GMT+05:30)' THEN
        v_p_timeZone := 'Asia/Kolkata (IST) GMT +05:30';
    END IF;

    -- Check if business hours exists
    SELECT cid INTO v_cid
    FROM custom_hours
    WHERE cid = p_cid
      AND "domainId" = p_domainId;

    -- Create temp table for business hours
    DROP TABLE IF EXISTS tt_business_hours;
    CREATE TEMP TABLE tt_business_hours(
        tt_id SERIAL PRIMARY KEY,
        tt_day VARCHAR(250),
        tt_isHoliday VARCHAR(250),
        tt_startTimeStamp BIGINT,
        tt_endTimeStamp BIGINT,
        tt_startTime TIME,
        tt_endTime TIME
    );

    -- Parse hours JSON and insert into temp table
    INSERT INTO tt_business_hours(tt_day, tt_isHoliday, tt_startTimeStamp, tt_endTimeStamp, tt_startTime, tt_endTime)
    SELECT
        (item->>'day')::VARCHAR(250),
        (item->>'isHoliday')::VARCHAR(250),
        (item->'entry'->0->>'startTimeStamp')::BIGINT,
        (item->'entry'->0->>'endTimeStamp')::BIGINT,
        (item->'entry'->0->>'startTime')::TIME,
        (item->'entry'->0->>'endTime')::TIME
    FROM jsonb_array_elements(v_p_hours) AS item;

    -- Create temp table for holidays
    DROP TABLE IF EXISTS tt_business_holiday;
    CREATE TEMP TABLE tt_business_holiday(
        tt_id SERIAL PRIMARY KEY,
        tt_holidayName VARCHAR(1000),
        tt_startDate BIGINT,
        tt_endDate BIGINT,
        tt_isStatus SMALLINT,
        tt_holidayDesc TEXT
    );

    -- Parse holiday JSON and insert into temp table
    IF p_holiday IS NOT NULL AND jsonb_array_length(p_holiday) > 0 THEN
        INSERT INTO tt_business_holiday(tt_holidayName, tt_startDate, tt_endDate, tt_isStatus, tt_holidayDesc)
        SELECT
            (item->>'holidayName')::VARCHAR(1000),
            (item->>'startDate')::BIGINT,
            (item->>'endDate')::BIGINT,
            (item->>'isStatus')::SMALLINT,
            (item->>'holidayDesc')::TEXT
        FROM jsonb_array_elements(p_holiday) AS item;
    END IF;

    IF v_cid IS NULL THEN
        -- Insert new business hours
        INSERT INTO custom_hours(name, description, "timeZone", hours, "domainId", default_hours, "timeZoneValue", type, "createdAt", "updatedAt")
        VALUES(p_name, p_description, v_p_timeZone, p_hours, p_domainId, p_default_hours, p_timeZoneValue, p_type, v_curdate, v_curdate)
        RETURNING cid INTO v_lastId;

        v_errcode := 0;
        v_errmsg := 'Business Hours Created Successfully!';

        -- Insert business hours details
        INSERT INTO tb_business_hours_json_info("fk_businessId", "domainId", day_name, "isHoliday", "startTimeStamp", "endTimeStamp", "startTime", "endTime", "createdAt", "updatedAt")
        SELECT v_lastId, p_domainId, tt_day, tt_isHoliday, tt_startTimeStamp, tt_endTimeStamp, tt_startTime, tt_endTime, v_curdate, v_curdate
        FROM tt_business_hours;

        -- Insert holidays if provided
        IF p_holiday IS NOT NULL AND jsonb_array_length(p_holiday) > 0 THEN
            INSERT INTO "tb_Business_Holidays"("domainId", "fk_businessId", "holidayName", "startDate", "endDate", "isStatus", "holidayDesc", "createdAt", "updatedAt")
            SELECT p_domainId, v_lastId, tt_holidayName, tt_startDate, tt_endDate, tt_isStatus, tt_holidayDesc, v_curdate, v_curdate
            FROM tt_business_holiday;
        END IF;
    ELSE
        -- Update existing business hours
        UPDATE custom_hours
        SET name = COALESCE(p_name, name),
            description = COALESCE(p_description, description),
            hours = COALESCE(p_hours, hours),
            "timeZone" = COALESCE(v_p_timeZone, "timeZone"),
            default_hours = COALESCE(p_default_hours, default_hours),
            "timeZoneValue" = COALESCE(p_timeZoneValue, "timeZoneValue"),
            type = COALESCE(p_type, type),
            "updatedAt" = v_curdate
        WHERE cid = v_cid;

        -- Check if temp table has data
        SELECT COUNT(*) INTO v_exists FROM tt_business_hours;
        IF v_exists > 0 THEN
            -- Delete existing and re-insert
            DELETE FROM tb_business_hours_json_info
            WHERE "fk_businessId" = v_cid
              AND "domainId" = p_domainId;

            INSERT INTO tb_business_hours_json_info("fk_businessId", "domainId", day_name, "isHoliday", "startTimeStamp", "endTimeStamp", "startTime", "endTime", "createdAt", "updatedAt")
            SELECT v_cid, p_domainId, tt_day, tt_isHoliday, tt_startTimeStamp, tt_endTimeStamp, tt_startTime, tt_endTime, v_curdate, v_curdate
            FROM tt_business_hours;
        END IF;

        -- Insert holidays if provided
        IF p_holiday IS NOT NULL AND jsonb_array_length(p_holiday) > 0 THEN
            INSERT INTO "tb_Business_Holidays"("domainId", "fk_businessId", "holidayName", "startDate", "endDate", "isStatus", "holidayDesc", "createdAt", "updatedAt")
            SELECT p_domainId, v_cid, tt_holidayName, tt_startDate, tt_endDate, tt_isStatus, tt_holidayDesc, v_curdate, v_curdate
            FROM tt_business_holiday;
        END IF;

        v_errcode := 1;
        v_errmsg := 'Business Hours Updated Successfully';
    END IF;

    OPEN p_result FOR SELECT v_errcode AS errcode, v_errmsg AS errmsg;

    -- Cleanup temp tables
    DROP TABLE IF EXISTS tt_business_hours;
    DROP TABLE IF EXISTS tt_business_holiday;
END;
$$;


ALTER PROCEDURE public.crm_insert_update_createbusinesshours(IN p_cid integer, IN p_name character varying, IN p_description character varying, IN p_hours text, IN p_domainid integer, IN p_timezone character varying, IN p_default_hours character varying, IN p_timezonevalue character varying, IN p_type character varying, IN p_holiday jsonb, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_insert_update_customer_contact(integer, integer, character varying, character varying, character varying, character varying, text, character varying, character varying, character varying, character varying, integer, integer, integer, integer, character varying, character varying, jsonb, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_insert_update_customer_contact(IN p_customer_id integer, IN p_domain_id integer, IN p_first_name character varying, IN p_last_name character varying, IN p_primary_phone_number character varying, IN p_primary_email character varying, IN p_address text, IN p_industry_type character varying, IN p_source_type character varying, IN p_company_name character varying, IN p_company_size character varying, IN p_user_id integer, IN p_demo_graphics integer, IN p_firmo_graphics integer, IN p_behaviors integer, IN p_secondary_phone_number character varying, IN p_secondary_email character varying, IN p_additional_info jsonb, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_customer_id INTEGER default null;
    v_last_id INTEGER;
    v_errcode INTEGER;
    v_errmsg TEXT;

	
BEGIN

	IF (COALESCE(TRIM(p_primary_phone_number), '') IN ('', '0'))
       AND (COALESCE(TRIM(p_primary_email), '') IN ('', '0')) THEN
        OPEN p_result FOR
            SELECT -3 AS errcode,
                   'Primary phone number or email is required' AS errmsg;
        RETURN;
    END IF;

    -- Check if record exists
    SELECT customer_id
    INTO v_customer_id
    FROM tb_customer_contact
    WHERE customer_id = p_customer_id
      AND domain_id = p_domain_id;

    ------------------------------------------------------------------
    -- INSERT BLOCK
    ------------------------------------------------------------------
    IF v_customer_id IS NULL THEN

        INSERT INTO tb_customer_contact (
            domain_id, first_name, last_name,
            primary_phone_number, primary_email,
            address, industry_type, source_type,
            company_name, company_size, user_id,
            demo_graphics, firmo_graphics, behaviors,
            created_at, updated_at, additional_info
        )
        VALUES (
            p_domain_id, p_first_name, p_last_name,
            p_primary_phone_number, p_primary_email,
            p_address, p_industry_type, p_source_type,
            p_company_name, p_company_size, p_user_id,
            p_demo_graphics, p_firmo_graphics, p_behaviors,
            NOW(), NOW(), p_additional_info
        )
        RETURNING customer_id INTO v_last_id;

        -- Insert secondary contact if provided
        IF p_secondary_phone_number IS NOT NULL 
           OR p_secondary_email IS NOT NULL THEN

			INSERT INTO tb_secondary_customer_contact (
        fk_customer_id, 
        domain_id, 
        secondary_phone_number, 
        secondary_email, 
        created_date, 
        updated_date
    )
    SELECT 
        v_last_id, 
        p_domain_id, 
        TRIM(s_phone.val), 
        TRIM(s_email.val), 
        NOW(), 
        NOW()
    FROM 
        -- Split phones into rows with an index (ordinality)
        unnest(string_to_array(p_secondary_phone_number, ',')) WITH ORDINALITY AS s_phone(val, idx)
    FULL OUTER JOIN 
        -- Split emails into rows with an index (ordinality)
        unnest(string_to_array(p_secondary_email, ',')) WITH ORDINALITY AS s_email(val, idx)
    ON s_phone.idx = s_email.idx;

	
        END IF;

        v_errcode := 0;
        v_errmsg := 'Contact inserted successfully';

    ------------------------------------------------------------------
    -- UPDATE BLOCK
    ------------------------------------------------------------------
    ELSE
 
        UPDATE tb_customer_contact
        SET
            first_name           = COALESCE(p_first_name, first_name),
            last_name            = COALESCE(p_last_name, last_name),
            primary_phone_number = COALESCE(p_primary_phone_number, primary_phone_number),
            primary_email        = COALESCE(p_primary_email, primary_email),
            address              = COALESCE(p_address, address),
            industry_type        = COALESCE(p_industry_type, industry_type),
            source_type          = COALESCE(p_source_type, source_type),
            company_name         = COALESCE(p_company_name, company_name),
            company_size         = COALESCE(p_company_size, company_size),
            user_id              = COALESCE(p_user_id, user_id),
            demo_graphics        = COALESCE(p_demo_graphics, demo_graphics),
            firmo_graphics       = COALESCE(p_firmo_graphics, firmo_graphics),
            behaviors            = COALESCE(p_behaviors, behaviors),
            additional_info      = COALESCE(p_additional_info, additional_info),
            updated_at         = NOW()
        WHERE customer_id = v_customer_id
          AND domain_id = p_domain_id;

        IF NOT FOUND THEN
            v_errcode := -2;
            v_errmsg := 'No matching record found to update';
        ELSE

            -- Secondary Contact UPSERT logic
            IF p_secondary_phone_number IS NOT NULL 
               OR p_secondary_email IS NOT NULL THEN
 
            delete from tb_secondary_customer_contact where fk_customer_id = p_customer_id;

			 INSERT INTO tb_secondary_customer_contact ( fk_customer_id, domain_id, secondary_phone_number, secondary_email,         created_date, updated_date )
    SELECT p_customer_id, p_domain_id, TRIM(s_phone.val), TRIM(s_email.val), NOW(), NOW()
    FROM 
	unnest(string_to_array(p_secondary_phone_number, ',')) WITH ORDINALITY AS s_phone(val, idx)
    FULL OUTER JOIN 
	unnest(string_to_array(p_secondary_email, ',')) WITH ORDINALITY AS s_email(val, idx)
    ON s_phone.idx = s_email.idx;

	
                -- INSERT INTO tb_secondary_customer_contact (
                --     fk_customer_id, domain_id,
             --    --     secondary_phone_number,
                --     secondary_email,
                --     created_date,
                --     updated_date
                -- )
                -- VALUES (
                --     v_customer_id, p_domain_id,
                --     p_secondary_phone_number,
                --     p_secondary_email,
                --     NOW(),
                --     NOW()
                -- )
                -- ON CONFLICT (fk_customer_id)
                -- DO UPDATE 
				-- SET 
				-- secondary_phone_number = CASE 
				-- 							 WHEN tb_secondary_customer_contact.secondary_phone_number IS NULL THEN EXCLUDED.secondary_phone_number
				-- 							 WHEN EXCLUDED.secondary_phone_number IS NULL THEN tb_secondary_customer_contact.secondary_phone_number
				-- 							 ELSE tb_secondary_customer_contact.secondary_phone_number || ', ' || EXCLUDED.secondary_phone_number
				-- 						 END,
				-- secondary_email = 
				-- 						CASE 
				-- 							WHEN tb_secondary_customer_contact.secondary_email IS NULL THEN EXCLUDED.secondary_email
				-- 							WHEN EXCLUDED.secondary_email IS NULL THEN tb_secondary_customer_contact.secondary_email
				-- 							ELSE tb_secondary_customer_contact.secondary_email || ', ' || EXCLUDED.secondary_email
				-- 						END,
				-- updated_date = NOW();

-- INSERT INTO tb_secondary_customer_contact (
--     fk_customer_id, 
--     domain_id, 
--     secondary_phone_number, 
--     secondary_email, 
--     created_date, 
--     updated_date
-- )
-- SELECT 
--     v_customer_id, 
--     p_domain_id, 
--     TRIM(s.phone), -- TRIM removes extra spaces around commas
--     TRIM(s.email), 
--     NOW(), 
--     NOW()
-- FROM 
--     -- unnest converts the array into rows
--     unnest(string_to_array(p_secondary_phone_number, ',')) AS s(phone)
-- FULL OUTER JOIN 
--     unnest(string_to_array(p_secondary_email, ',')) AS s(email) 
-- ON TRUE; -- This creates rows for all combinations

            END IF;

            v_errcode := 1;
            v_errmsg := 'Contact updated successfully';

        END IF;

    END IF;

    -- Return result
    OPEN p_result FOR
        SELECT v_errcode AS errcode,
               v_errmsg AS errmsg,
			   COALESCE(v_last_id,p_customer_id) AS coid;

EXCEPTION
    WHEN OTHERS THEN
        OPEN p_result FOR
            SELECT -1 AS errcode,
                   SQLERRM AS errmsg;
END;
$$;


ALTER PROCEDURE public.crm_insert_update_customer_contact(IN p_customer_id integer, IN p_domain_id integer, IN p_first_name character varying, IN p_last_name character varying, IN p_primary_phone_number character varying, IN p_primary_email character varying, IN p_address text, IN p_industry_type character varying, IN p_source_type character varying, IN p_company_name character varying, IN p_company_size character varying, IN p_user_id integer, IN p_demo_graphics integer, IN p_firmo_graphics integer, IN p_behaviors integer, IN p_secondary_phone_number character varying, IN p_secondary_email character varying, IN p_additional_info jsonb, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_insert_update_customer_deal(integer, integer, character varying, jsonb, character varying, character varying, character varying, character varying, character varying, character varying, jsonb, jsonb, jsonb, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_insert_update_customer_deal(IN p_customer_id integer, IN p_domain_id integer, IN p_deal_name character varying, IN p_deal_details jsonb, IN p_total_amount character varying, IN p_weighted_amount character varying, IN p_open_amount character varying, IN p_pending_amount character varying, IN p_closed_amount character varying, IN p_new_amount character varying, IN p_ai_suggest_summary_step jsonb, IN p_deal_activity jsonb, IN p_activity_timeline jsonb, INOUT p_cursor refcursor DEFAULT 'p_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_deal_id INT;
	v_customer_id INT;
    v_errcode INT;
    v_errmsg  VARCHAR(100);
BEGIN

    -- Get deal_id if exists
    SELECT deal_id
    INTO v_deal_id
    FROM "tb_deal_master"
    WHERE customer_id = p_customer_id
      AND domain_id = p_domain_id
    LIMIT 1;

    -- Insert log only if deal exists
    IF v_deal_id IS NOT NULL THEN
        INSERT INTO tb_deal_master_log(
            fk_deal_id, customer_id, domain_id, deal_name, deal_details,
            total_amount, weighted_amount, open_amount, pending_amount,
            closed_amount, new_amount, ai_suggest_summary_step,
            deal_activity, activity_timeline,
            created_at, updated_at, log_date
        )
        SELECT deal_id, customer_id, domain_id, deal_name, deal_details,
               total_amount, weighted_amount, open_amount, pending_amount,
               closed_amount, new_amount, ai_suggest_summary_step,
               deal_activity, activity_timeline,
               created_at, updated_at, NOW()
        FROM tb_deal_master
        WHERE deal_id = v_deal_id;
    END IF;

    -- INSERT
    IF v_deal_id IS NULL THEN

 
        INSERT INTO "tb_deal_master"(
            customer_id, domain_id, deal_name, deal_details,
            total_amount, weighted_amount, open_amount,
            pending_amount, closed_amount, new_amount,
            ai_suggest_summary_step, deal_activity,
            activity_timeline, created_at
        )
        VALUES (
            p_customer_id, p_domain_id, p_deal_name, p_deal_details,
            p_total_amount, p_weighted_amount, p_open_amount,
            p_pending_amount, p_closed_amount, p_new_amount,
            p_ai_suggest_summary_step, p_deal_activity,
            p_activity_timeline, NOW()
        )
        RETURNING deal_id INTO v_deal_id;

        v_errcode := 0;
        v_errmsg  := 'Contact inserted successfully';

    ELSE
        -- UPDATE
        UPDATE "tb_deal_master"
        SET customer_id             = COALESCE(p_customer_id, customer_id),
            deal_name               = COALESCE(p_deal_name, deal_name),
            deal_details            = COALESCE(p_deal_details, deal_details),
            total_amount            = COALESCE(p_total_amount, total_amount),
            weighted_amount         = COALESCE(p_weighted_amount, weighted_amount),
            open_amount             = COALESCE(p_open_amount, open_amount),
            pending_amount          = COALESCE(p_pending_amount, pending_amount),
            closed_amount           = COALESCE(p_closed_amount, closed_amount),
            new_amount              = COALESCE(p_new_amount, new_amount),
            ai_suggest_summary_step = COALESCE(p_ai_suggest_summary_step, ai_suggest_summary_step),
            deal_activity           = COALESCE(p_deal_activity, deal_activity),
            activity_timeline       = COALESCE(p_activity_timeline, activity_timeline),
            updated_at              = NOW()
        WHERE deal_id = v_deal_id;

        v_errcode := 1;
        v_errmsg  := 'Contact updated successfully';
    END IF;

    -- Open cursor like MySQL SELECT
    OPEN p_cursor FOR
        SELECT v_errcode AS errcode,
               v_errmsg  AS errmsg,
               v_deal_id AS deal_id;

END;
$$;


ALTER PROCEDURE public.crm_insert_update_customer_deal(IN p_customer_id integer, IN p_domain_id integer, IN p_deal_name character varying, IN p_deal_details jsonb, IN p_total_amount character varying, IN p_weighted_amount character varying, IN p_open_amount character varying, IN p_pending_amount character varying, IN p_closed_amount character varying, IN p_new_amount character varying, IN p_ai_suggest_summary_step jsonb, IN p_deal_activity jsonb, IN p_activity_timeline jsonb, INOUT p_cursor refcursor) OWNER TO postgres;

--
-- Name: crm_insert_update_detailed_product_info(bigint, integer, jsonb, jsonb, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_insert_update_detailed_product_info(IN p_fk_product_id bigint, IN p_domain_id integer, IN p_product_details jsonb, IN p_total_revenue_growth jsonb, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_pid INT;
    v_errcode INT := 0;
    v_errmsg VARCHAR(100);
BEGIN

    SELECT p_id
    INTO v_pid
    FROM tb_detailed_product_info
    WHERE fk_product_id = p_fk_product_id
      AND domain_id = p_domain_id
    LIMIT 1;

    IF v_pid IS NOT NULL
    THEN

        UPDATE tb_detailed_product_info
        SET product_details = COALESCE(p_product_details, product_details),
			total_revenue_growth = COALESCE(p_total_revenue_growth,total_revenue_growth),
            updated_at      = CURRENT_TIMESTAMP
        WHERE p_id = v_pid;

        v_errmsg := 'Updated Successfully';

    ELSE

        INSERT INTO tb_detailed_product_info(
            fk_product_id,
            domain_id,
            product_details,
			total_revenue_growth,
            created_at,
            updated_at
        )
        VALUES(
            p_fk_product_id,
            p_domain_id,
            p_product_details,
			p_total_revenue_growth,
            CURRENT_TIMESTAMP,
            CURRENT_TIMESTAMP
        );

        v_errmsg := 'Inserted Successfully';

    END IF;

    OPEN p_result FOR
    SELECT v_errcode AS errcode,
           v_errmsg AS errmsg;

END;
$$;


ALTER PROCEDURE public.crm_insert_update_detailed_product_info(IN p_fk_product_id bigint, IN p_domain_id integer, IN p_product_details jsonb, IN p_total_revenue_growth jsonb, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_insert_update_interaction_notes(integer, character varying, integer, integer, character varying, text, character varying, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_insert_update_interaction_notes(IN p_domainid integer, IN p_sessionid character varying, IN p_coid integer, IN p_agentid integer, IN p_channeltype character varying, IN p_notes text, IN p_imageurl character varying, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_errcode INT := -1;
    v_errmsg  TEXT := 'Notes Insert Fail';
BEGIN

    IF NULLIF(TRIM(p_notes), '') IS NOT NULL THEN

        INSERT INTO tb_interaction_notes_info (
            "domainId",
            "sessionId",
            "coid",
            "agentId",
            "channelType",
            "notes",
            "imageurl",
            "createdAt",
            "updatedAt"
        )
        VALUES (
            p_domainid,
            p_sessionid,
            p_coid,
            p_agentid,
            p_channeltype,
            p_notes,
            p_imageurl,
            CURRENT_TIMESTAMP,
            CURRENT_TIMESTAMP
        );

        v_errcode := 0;
        v_errmsg  := 'Note Insert Successfully';

    ELSE
        v_errmsg := 'Note is empty or null';
    END IF;

    OPEN p_result FOR 
        SELECT v_errcode AS errcode, v_errmsg AS errmsg;

EXCEPTION WHEN OTHERS THEN
    GET STACKED DIAGNOSTICS v_errmsg = MESSAGE_TEXT;
    v_errcode := -1;

    OPEN p_result FOR 
        SELECT v_errcode AS errcode, v_errmsg AS errmsg;
END;
$$;


ALTER PROCEDURE public.crm_insert_update_interaction_notes(IN p_domainid integer, IN p_sessionid character varying, IN p_coid integer, IN p_agentid integer, IN p_channeltype character varying, IN p_notes text, IN p_imageurl character varying, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_insert_update_onboard_conversation_flow(integer, integer, jsonb, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_insert_update_onboard_conversation_flow(IN p_id integer, IN p_domainid integer, IN p_conversation_flow jsonb, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$

DECLARE
    v_id INTEGER;
    v_errcode INTEGER := -1;
    v_errmsg VARCHAR(255) := 'Already Exists';
BEGIN
    SELECT id INTO v_id FROM tb_onboard_dtl WHERE "domainId" = p_domainId;
    IF v_id IS NULL THEN
        INSERT INTO tb_onboard_dtl("domainId", conversation_flow, created_at, updated_at) VALUES(p_domainId, p_conversation_flow, NOW(), NOW());
        v_errcode := 0; v_errmsg := 'Conversation inserted successfully';
    ELSE
        UPDATE tb_onboard_dtl SET conversation_flow = p_conversation_flow, updated_at = NOW() WHERE id = v_id;
        v_errcode := 1; v_errmsg := 'Conversation updated successfully';
    END IF;
    OPEN p_result FOR SELECT v_errcode AS errcode, v_errmsg AS errmsg;
END;
$$;


ALTER PROCEDURE public.crm_insert_update_onboard_conversation_flow(IN p_id integer, IN p_domainid integer, IN p_conversation_flow jsonb, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_insert_update_particular_project(integer, integer, integer, character varying, jsonb, jsonb, text, jsonb, jsonb, jsonb, jsonb, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_insert_update_particular_project(IN p_customer_id integer, IN p_domain_id integer, IN p_fk_pro_id integer, IN p_product_ref_id character varying, IN p_agentic_insight jsonb, IN p_requirement_list jsonb, IN p_next_steps text, IN p_product_stages jsonb, IN p_requested_requirements jsonb, IN p_team_execution_overview jsonb, IN p_files jsonb, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_pid INT;
    v_errcode INT := 0;
    v_errmsg VARCHAR(100);
BEGIN

    SELECT pid
    INTO v_pid
    FROM tb_projects_dtl
    WHERE customer_id = p_customer_id
      AND domain_id = p_domain_id
      AND product_ref_id = p_product_ref_id
    LIMIT 1;

    IF v_pid IS NOT NULL 
	THEN

        UPDATE tb_projects_dtl
        SET fk_pro_id 				= COALESCE(p_fk_pro_id,fk_pro_id),
            agentic_insight 		= COALESCE(p_agentic_insight,agentic_insight),
            requirement_list 		= COALESCE(p_requirement_list,requirement_list),
            next_steps 				= COALESCE(p_next_steps,next_steps),
            product_stages 			= COALESCE(p_product_stages,product_stages),
            requested_requirements 	= COALESCE(p_requested_requirements,requested_requirements),
            team_execution_overview = COALESCE(p_team_execution_overview,team_execution_overview),
            files 					= COALESCE(p_files,files),
            updated_at 				= CURRENT_TIMESTAMP
        WHERE pid = v_pid;

        v_errmsg := 'Updated Successfully';

    ELSE

        INSERT INTO tb_projects_dtl(
            customer_id,
            domain_id,
            fk_pro_id,
            product_ref_id,
            agentic_insight,
            requirement_list,
            next_steps,
            product_stages,
            requested_requirements,
            team_execution_overview,
            files,
            created_at
        )
        VALUES(
            p_customer_id,
            p_domain_id,
            p_fk_pro_id,
            p_product_ref_id,
            p_agentic_insight,
            p_requirement_list,
            p_next_steps,
            p_product_stages,
            p_requested_requirements,
            p_team_execution_overview,
            p_files,
            CURRENT_TIMESTAMP
        );

        v_errmsg := 'Inserted Successfully';

    END IF;

    OPEN p_result FOR
    SELECT v_errcode AS errcode,
           v_errmsg AS errmsg;

END;
$$;


ALTER PROCEDURE public.crm_insert_update_particular_project(IN p_customer_id integer, IN p_domain_id integer, IN p_fk_pro_id integer, IN p_product_ref_id character varying, IN p_agentic_insight jsonb, IN p_requirement_list jsonb, IN p_next_steps text, IN p_product_stages jsonb, IN p_requested_requirements jsonb, IN p_team_execution_overview jsonb, IN p_files jsonb, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_insert_update_product_catalog_master(bigint, integer, text, character varying, character varying, character varying, character varying, integer, character varying, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_insert_update_product_catalog_master(IN p_product_id bigint, IN p_domain_id integer, IN p_product_image text, IN p_product_name character varying, IN p_short_description character varying, IN p_product_category character varying, IN p_starting_price_plan character varying, IN p_active_customers integer, IN p_rating_reviews character varying, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_product_id BIGINT;
    v_errcode INT := 0;
    v_errmsg VARCHAR(100);
BEGIN

    SELECT product_id
    INTO v_product_id
    FROM tb_product_master
    WHERE product_id = p_product_id
      AND domain_id = p_domain_id
    LIMIT 1;

    IF v_product_id IS NOT NULL
    THEN

        UPDATE tb_product_master
        SET product_image       = COALESCE(p_product_image, product_image),
            product_name        = COALESCE(p_product_name, product_name),
            short_description   = COALESCE(p_short_description, short_description),
            product_category    = COALESCE(p_product_category, product_category),
            starting_price_plan = COALESCE(p_starting_price_plan, starting_price_plan),
            active_customers    = COALESCE(p_active_customers, active_customers),
            rating_reviews      = COALESCE(p_rating_reviews, rating_reviews),
            updated_at          = CURRENT_TIMESTAMP
        WHERE product_id = v_product_id
        AND domain_id = p_domain_id;

        v_errmsg := 'Updated Successfully';

    ELSE

        INSERT INTO tb_product_master(
            domain_id,
            product_image,
            product_name,
            short_description,
            product_category,
            starting_price_plan,
            active_customers,
            rating_reviews,
            created_at,
            updated_at
        )
        VALUES(
            p_domain_id,
            p_product_image,
            p_product_name,
            p_short_description,
            p_product_category,
            p_starting_price_plan,
            p_active_customers,
            p_rating_reviews,
            CURRENT_TIMESTAMP,
            CURRENT_TIMESTAMP
        );

        v_errmsg := 'Inserted Successfully';

    END IF;

    OPEN p_result FOR
    SELECT v_errcode AS errcode,
           v_errmsg AS errmsg;

END;
$$;


ALTER PROCEDURE public.crm_insert_update_product_catalog_master(IN p_product_id bigint, IN p_domain_id integer, IN p_product_image text, IN p_product_name character varying, IN p_short_description character varying, IN p_product_category character varying, IN p_starting_price_plan character varying, IN p_active_customers integer, IN p_rating_reviews character varying, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_insert_update_product_details(integer, jsonb, jsonb, jsonb, jsonb, jsonb, jsonb, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_insert_update_product_details(IN p_domain_id integer, IN p_product_insights jsonb, IN p_product_tracker jsonb, IN p_product_catalog jsonb, IN p_product_notes jsonb, IN p_product_quotes jsonb, IN p_product_invoice jsonb, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$

DECLARE 
    v_id INTEGER;
    v_errmsg VARCHAR(250);

BEGIN

    SELECT id
    INTO v_id
    FROM tb_crm_product_details
    WHERE domain_id = p_domain_id
    LIMIT 1;

    IF v_id IS NOT NULL THEN

        UPDATE tb_crm_product_details
        SET 
            product_insights = COALESCE(p_product_insights, product_insights),
            product_tracker  = COALESCE(p_product_tracker, product_tracker),
            product_catalog  = COALESCE(p_product_catalog, product_catalog),
            product_notes    = COALESCE(p_product_notes, product_notes),
            product_quotes   = COALESCE(p_product_quotes, product_quotes),
            product_invoice  = COALESCE(p_product_invoice, product_invoice),
            updated_at       = CURRENT_TIMESTAMP
        WHERE id = v_id;

        v_errmsg := 'Updated Successfully';

    ELSE

        INSERT INTO tb_crm_product_details (
            domain_id,
            product_insights,
            product_tracker,
            product_catalog,
            product_notes,
            product_quotes,
            product_invoice,
            created_at
        )
        VALUES (
            p_domain_id,
            COALESCE(p_product_insights, '{}'::jsonb),
            COALESCE(p_product_tracker, '[]'::jsonb),
            COALESCE(p_product_catalog, '{}'::jsonb),
            COALESCE(p_product_notes, '{}'::jsonb),
            COALESCE(p_product_quotes, '{}'::jsonb),
            COALESCE(p_product_invoice, '{}'::jsonb),
            CURRENT_TIMESTAMP
        );

        v_errmsg := 'Inserted Successfully';

    END IF;

    OPEN p_result FOR
        SELECT v_errmsg AS errmsg;

END;
$$;


ALTER PROCEDURE public.crm_insert_update_product_details(IN p_domain_id integer, IN p_product_insights jsonb, IN p_product_tracker jsonb, IN p_product_catalog jsonb, IN p_product_notes jsonb, IN p_product_quotes jsonb, IN p_product_invoice jsonb, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_insert_update_product_insights_average_deal_size(integer, jsonb, jsonb, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_insert_update_product_insights_average_deal_size(IN p_domain_id integer, IN p_average_deal_size_by_product jsonb, IN p_average_deal_size_by_product_chart jsonb, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_insight_id BIGINT;
    v_errcode INT := 0;
    v_errmsg VARCHAR(100);
BEGIN

    SELECT insight_id
    INTO v_insight_id
    FROM tb_product_insights_info
    WHERE domain_id = p_domain_id
    LIMIT 1;

    IF v_insight_id IS NOT NULL
    THEN

        UPDATE tb_product_insights_info
        SET average_deal_size_by_product  = COALESCE(p_average_deal_size_by_product, average_deal_size_by_product),
            average_deal_size_by_product_chart     = COALESCE(p_average_deal_size_by_product_chart, average_deal_size_by_product_chart),
            updated_at                  = CURRENT_TIMESTAMP
        WHERE insight_id = v_insight_id;

        v_errmsg := 'Updated Successfully';

    ELSE

        INSERT INTO tb_product_insights_info(
            domain_id,
            average_deal_size_by_product,
            average_deal_size_by_product_chart,
            created_at,
            updated_at
        )
        VALUES(
            p_domain_id,
            p_average_deal_size_by_product,
            average_deal_size_by_product_chart,
            CURRENT_TIMESTAMP,
            CURRENT_TIMESTAMP
        );

        v_errmsg := 'Inserted Successfully';

    END IF;

    OPEN p_result FOR
    SELECT v_errcode AS errcode,
           v_errmsg AS errmsg;

END;
$$;


ALTER PROCEDURE public.crm_insert_update_product_insights_average_deal_size(IN p_domain_id integer, IN p_average_deal_size_by_product jsonb, IN p_average_deal_size_by_product_chart jsonb, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_insert_update_product_insights_performance_ranking(integer, jsonb, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_insert_update_product_insights_performance_ranking(IN p_domain_id integer, IN p_product_performance_ranking jsonb, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_insight_id BIGINT;
    v_errcode INT := 0;
    v_errmsg VARCHAR(100);
BEGIN

    SELECT insight_id
    INTO v_insight_id
    FROM tb_product_insights_info
    WHERE domain_id = p_domain_id
    LIMIT 1;

    IF v_insight_id IS NOT NULL
    THEN

        UPDATE tb_product_insights_info
        SET product_performance_ranking  = COALESCE(p_product_performance_ranking, product_performance_ranking)
            ,updated_at                  = CURRENT_TIMESTAMP
        WHERE insight_id = v_insight_id;

        v_errmsg := 'Updated Successfully';

    ELSE

        INSERT INTO tb_product_insights_info(
            domain_id,
            product_performance_ranking,
            created_at,
            updated_at
        )
        VALUES(
            p_domain_id,
            p_product_performance_ranking,
            CURRENT_TIMESTAMP,
            CURRENT_TIMESTAMP
        );

        v_errmsg := 'Inserted Successfully';

    END IF;

    OPEN p_result FOR
    SELECT v_errcode AS errcode,
           v_errmsg AS errmsg;

END;
$$;


ALTER PROCEDURE public.crm_insert_update_product_insights_performance_ranking(IN p_domain_id integer, IN p_product_performance_ranking jsonb, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_insert_update_product_insights_product_revenue_growth(integer, jsonb, jsonb, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_insert_update_product_insights_product_revenue_growth(IN p_domain_id integer, IN p_product_revenue_growth jsonb, IN p_product_revenue_growth_chart jsonb, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_insight_id BIGINT;
    v_errcode INT := 0;
    v_errmsg VARCHAR(100);
BEGIN

    SELECT insight_id
    INTO v_insight_id
    FROM tb_product_insights_info
    WHERE domain_id = p_domain_id
    LIMIT 1;

    IF v_insight_id IS NOT NULL
    THEN

        UPDATE tb_product_insights_info
        SET  product_revenue_growth       = COALESCE(p_product_revenue_growth, product_revenue_growth),
            product_revenue_growth_chart = COALESCE(p_product_revenue_growth_chart, product_revenue_growth_chart),
            updated_at                  = CURRENT_TIMESTAMP
        WHERE insight_id = v_insight_id;

        v_errmsg := 'Updated Successfully';

    ELSE

        INSERT INTO tb_product_insights_info(
            domain_id,
            product_revenue_growth,
            product_revenue_growth_chart,
            created_at,
            updated_at
        )
        VALUES(
            p_domain_id,
            p_product_revenue_growth,
            p_product_revenue_growth_chart,
            CURRENT_TIMESTAMP,
            CURRENT_TIMESTAMP
        );

        v_errmsg := 'Inserted Successfully';

    END IF;

    OPEN p_result FOR
    SELECT v_errcode AS errcode,
           v_errmsg AS errmsg;

END;
$$;


ALTER PROCEDURE public.crm_insert_update_product_insights_product_revenue_growth(IN p_domain_id integer, IN p_product_revenue_growth jsonb, IN p_product_revenue_growth_chart jsonb, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_insert_update_product_insights_recent_invoices(integer, jsonb, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_insert_update_product_insights_recent_invoices(IN p_domain_id integer, IN p_recent_invoices jsonb, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_insight_id BIGINT;
    v_errcode INT := 0;
    v_errmsg VARCHAR(100);
BEGIN

    SELECT insight_id
    INTO v_insight_id
    FROM tb_product_insights_info
    WHERE domain_id = p_domain_id
    LIMIT 1;

    IF v_insight_id IS NOT NULL
    THEN

        UPDATE tb_product_insights_info
        SET recent_invoices     = COALESCE(p_recent_invoices, recent_invoices),
            updated_at                  = CURRENT_TIMESTAMP
        WHERE insight_id = v_insight_id;

        v_errmsg := 'Updated Successfully';

    ELSE

        INSERT INTO tb_product_insights_info(
            domain_id,
            recent_invoices,
            created_at,
            updated_at
        )
        VALUES(
            p_domain_id,
            p_recent_invoices,
            CURRENT_TIMESTAMP,
            CURRENT_TIMESTAMP
        );

        v_errmsg := 'Inserted Successfully';

    END IF;

    OPEN p_result FOR
    SELECT v_errcode AS errcode,
           v_errmsg AS errmsg;

END;
$$;


ALTER PROCEDURE public.crm_insert_update_product_insights_recent_invoices(IN p_domain_id integer, IN p_recent_invoices jsonb, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_insert_update_product_insights_recent_quotes(integer, jsonb, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_insert_update_product_insights_recent_quotes(IN p_domain_id integer, IN p_recent_quotes jsonb, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_insight_id BIGINT;
    v_errcode INT := 0;
    v_errmsg VARCHAR(100);
BEGIN

    SELECT insight_id
    INTO v_insight_id
    FROM tb_product_insights_info
    WHERE domain_id = p_domain_id
    LIMIT 1;

    IF v_insight_id IS NOT NULL
    THEN

        UPDATE tb_product_insights_info
        SET recent_quotes     = COALESCE(p_recent_quotes, recent_quotes),
            updated_at                  = CURRENT_TIMESTAMP
        WHERE insight_id = v_insight_id;

        v_errmsg := 'Updated Successfully';

    ELSE

        INSERT INTO tb_product_insights_info(
            domain_id,
            recent_quotes,
            created_at,
            updated_at
        )
        VALUES(
            p_domain_id,
            p_recent_quotes,
            CURRENT_TIMESTAMP,
            CURRENT_TIMESTAMP
        );

        v_errmsg := 'Inserted Successfully';

    END IF;

    OPEN p_result FOR
    SELECT v_errcode AS errcode,
           v_errmsg AS errmsg;

END;
$$;


ALTER PROCEDURE public.crm_insert_update_product_insights_recent_quotes(IN p_domain_id integer, IN p_recent_quotes jsonb, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_insert_update_product_insights_revenue_by_product(integer, jsonb, jsonb, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_insert_update_product_insights_revenue_by_product(IN p_domain_id integer, IN p_total_revenue_by_product jsonb, IN p_total_revenue_by_product_chart jsonb, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_insight_id BIGINT;
    v_errcode INT := 0;
    v_errmsg VARCHAR(100);
BEGIN

    SELECT insight_id
    INTO v_insight_id
    FROM tb_product_insights_info
    WHERE domain_id = p_domain_id
    LIMIT 1;

    IF v_insight_id IS NOT NULL
    THEN

        UPDATE tb_product_insights_info
        SET total_revenue_by_product  = COALESCE(p_total_revenue_by_product, total_revenue_by_product)
        , total_revenue_by_product_chart  = COALESCE(p_total_revenue_by_product_chart, total_revenue_by_product_chart)
            ,updated_at                  = CURRENT_TIMESTAMP
        WHERE insight_id = v_insight_id;

        v_errmsg := 'Updated Successfully';

    ELSE

        INSERT INTO tb_product_insights_info(
            domain_id,
            total_revenue_by_product,
            total_revenue_by_product_chart,
            created_at,
            updated_at
        )
        VALUES(
            p_domain_id,
            p_total_revenue_by_product,
            p_total_revenue_by_product_chart,
            p_recent_invoices,
            CURRENT_TIMESTAMP,
            CURRENT_TIMESTAMP
        );

        v_errmsg := 'Inserted Successfully';

    END IF;

    OPEN p_result FOR
    SELECT v_errcode AS errcode,
           v_errmsg AS errmsg;

END;
$$;


ALTER PROCEDURE public.crm_insert_update_product_insights_revenue_by_product(IN p_domain_id integer, IN p_total_revenue_by_product jsonb, IN p_total_revenue_by_product_chart jsonb, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_insert_update_project_insights(integer, jsonb, jsonb, jsonb, jsonb, jsonb, jsonb, jsonb, jsonb, jsonb, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_insert_update_project_insights(IN p_domain_id integer, IN p_predictive_project_summary jsonb, IN p_recent_activity jsonb, IN p_requirements_list jsonb, IN p_client_deliverables jsonb, IN p_upcoming_delays_risk_warnings jsonb, IN p_workload_capacity_predictions jsonb, IN p_resource_allocation_insights jsonb, IN p_requirement_stability_rework_prediction jsonb, IN p_operational_efficiency_signals jsonb, IN p_created_by integer, INOUT p_result refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_id INT;
    v_errcode INT := -1;
    v_errmsg TEXT := 'Operation Failed';
BEGIN

    IF p_domain_id IS NULL THEN
        v_errmsg := 'domain_id cannot be null';
        OPEN p_result FOR SELECT v_errcode AS errcode, v_errmsg AS errmsg;
        RETURN;
    END IF;

    SELECT pid
    INTO v_id
    FROM tb_Project_insights
    WHERE domain_id = p_domain_id
    LIMIT 1;

    IF v_id IS NOT NULL THEN

        UPDATE tb_Project_insights
        SET
            predictive_project_summary = COALESCE(p_predictive_project_summary, predictive_project_summary),
            recent_activity = COALESCE(p_recent_activity, recent_activity),
            requirements_list = COALESCE(p_requirements_list, requirements_list),
            client_deliverables = COALESCE(p_client_deliverables, client_deliverables),
            upcoming_delays_risk_warnings = COALESCE(p_upcoming_delays_risk_warnings, upcoming_delays_risk_warnings),
            workload_capacity_predictions = COALESCE(p_workload_capacity_predictions, workload_capacity_predictions),
            resource_allocation_insights = COALESCE(p_resource_allocation_insights, resource_allocation_insights),
            requirement_stability_rework_prediction = COALESCE(p_requirement_stability_rework_prediction, requirement_stability_rework_prediction),
            operational_efficiency_signals = COALESCE(p_operational_efficiency_signals, operational_efficiency_signals),
            updated_at = CURRENT_TIMESTAMP
        WHERE domain_id = p_domain_id;

        v_errcode := 0;
        v_errmsg := 'Updated Successfully';

    ELSE

        INSERT INTO tb_Project_insights
        (
            domain_id,
            predictive_project_summary,
            recent_activity,
            requirements_list,
            client_deliverables,
            upcoming_delays_risk_warnings,
            workload_capacity_predictions,
            resource_allocation_insights,
            requirement_stability_rework_prediction,
            operational_efficiency_signals,
            created_by,
            created_at,
            updated_at
        )
        VALUES
        (
            p_domain_id,
            p_predictive_project_summary,
            p_recent_activity,
            p_requirements_list,
            p_client_deliverables,
            p_upcoming_delays_risk_warnings,
            p_workload_capacity_predictions,
            p_resource_allocation_insights,
            p_requirement_stability_rework_prediction,
            p_operational_efficiency_signals,
            p_created_by,
            CURRENT_TIMESTAMP,
            CURRENT_TIMESTAMP
        );

        v_errcode := 0;
        v_errmsg := 'Inserted Successfully';

    END IF;

    OPEN p_result FOR
    SELECT v_errcode AS errcode, v_errmsg AS errmsg;

END;
$$;


ALTER PROCEDURE public.crm_insert_update_project_insights(IN p_domain_id integer, IN p_predictive_project_summary jsonb, IN p_recent_activity jsonb, IN p_requirements_list jsonb, IN p_client_deliverables jsonb, IN p_upcoming_delays_risk_warnings jsonb, IN p_workload_capacity_predictions jsonb, IN p_resource_allocation_insights jsonb, IN p_requirement_stability_rework_prediction jsonb, IN p_operational_efficiency_signals jsonb, IN p_created_by integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_insert_update_projects(integer, integer, jsonb, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_insert_update_projects(IN p_customer_id integer, IN p_domain_id integer, IN p_projests jsonb, INOUT p_cursor refcursor DEFAULT 'p_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE 
    v_errcode INT;
    v_errmsg VARCHAR(250);
BEGIN

    -- Check if record exists
    IF EXISTS (
        SELECT 1
        FROM tb_projects_master
        WHERE customer_id = p_customer_id
        AND domain_id = p_domain_id
    ) THEN

        -- Update existing record
        UPDATE tb_projects_master
        SET 
            projests = p_projests,
            updated_at = NOW()
        WHERE customer_id = p_customer_id
        AND domain_id = p_domain_id;

        v_errcode := 1;
        v_errmsg := 'Project updated successfully';

    ELSE

        -- Insert new record
        INSERT INTO tb_projects_master(
            customer_id,
            domain_id,
            projests,
            created_at
        )
        VALUES(
            p_customer_id,
            p_domain_id,
            p_projests,
            NOW()
        );

        v_errcode := 0;
        v_errmsg := 'Project inserted successfully';

    END IF;

    -- Return result
    OPEN p_cursor FOR
    SELECT v_errcode AS errcode, v_errmsg AS errmsg;

END;
$$;


ALTER PROCEDURE public.crm_insert_update_projects(IN p_customer_id integer, IN p_domain_id integer, IN p_projests jsonb, INOUT p_cursor refcursor) OWNER TO postgres;

--
-- Name: crm_insert_update_queue(integer, character varying, character varying, integer, text, text, text, text, character varying, character varying, integer, integer, integer, integer, text, text, integer, character varying, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_insert_update_queue(IN p_qid integer, IN p_name character varying, IN p_description character varying, IN p_domainid integer, IN p_settingaudio text, IN p_announcement text, IN p_maximumcallers text, IN p_maximumwaittime text, IN p_outboundcallerid character varying, IN p_outboundnumber character varying, IN p_maxcallhandlingtimemm integer, IN p_callwrapuptime integer, IN p_outboundcallstatus integer, IN p_hoursofoperatioid integer, IN p_hours_of_operation text, IN p_outofbusinesshours text, IN p_status integer, IN p_type character varying, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_existing_qid INTEGER;
    v_qid INTEGER;
    v_errcode INTEGER := -1;
    v_errmsg VARCHAR(50) := 'Failure';
BEGIN
    SELECT qid INTO v_existing_qid FROM queue WHERE "domainId" = p_domainId AND qid = p_qid;
    IF v_existing_qid IS NULL THEN
        INSERT INTO queue(name, description, "domainId", "OutboundCallerId", "outBoundNumber", "maxCallHandlingTimeMM", "callWrapUpTime", hours_of_operation, "outBoundCallStatus", "settingAudio", announcement, "maximumCallers", "maximumWaitTime", "hoursOfOperatioId", "createdAt", "updatedAt", "outOfBusinessHours", status, type)
        VALUES (p_name, p_description, p_domainId, p_OutboundCallerId, p_outBoundNumber, p_maxCallHandlingTimeMM, p_callWrapUpTime, p_hours_of_operation, p_outBoundCallStatus, p_settingAudio, p_announcement, p_maximumCallers, p_maximumWaitTime, p_hoursOfOperatioId, NOW(), NOW(), p_outOfBusinessHours, p_status, p_type)
        RETURNING qid INTO v_qid;
        INSERT INTO tb_queue_permission(fk_qid, "domainId", "agentMonitoring", "addOrRemove", "reportingDashboard", "createdAt", "updatedAt")
        VALUES(v_qid, p_domainId, 0, 0, 0, NOW(), NOW());
        v_errcode := 0; v_errmsg := 'Inserted';
    ELSE
        v_qid := v_existing_qid;
        UPDATE queue SET name = COALESCE(p_name, name), description = COALESCE(p_description, description), "settingAudio" = COALESCE(p_settingAudio, "settingAudio"), announcement = COALESCE(p_announcement, announcement), "maximumCallers" = COALESCE(p_maximumCallers, "maximumCallers"), "maximumWaitTime" = COALESCE(p_maximumWaitTime, "maximumWaitTime"), "OutboundCallerId" = COALESCE(p_OutboundCallerId, "OutboundCallerId"), "outBoundNumber" = COALESCE(p_outBoundNumber, "outBoundNumber"), "maxCallHandlingTimeMM" = COALESCE(p_maxCallHandlingTimeMM, "maxCallHandlingTimeMM"), "callWrapUpTime" = COALESCE(p_callWrapUpTime, "callWrapUpTime"), "outBoundCallStatus" = COALESCE(p_outBoundCallStatus, "outBoundCallStatus"), "hoursOfOperatioId" = COALESCE(p_hoursOfOperatioId, "hoursOfOperatioId"), hours_of_operation = COALESCE(p_hours_of_operation, hours_of_operation), "outOfBusinessHours" = COALESCE(p_outOfBusinessHours, "outOfBusinessHours"), status = COALESCE(p_status, status), type = COALESCE(p_type, type), "updatedAt" = NOW()
        WHERE qid = v_existing_qid;
        v_errcode := 1; v_errmsg := 'Updated';
    END IF;
    OPEN p_result FOR SELECT v_errcode AS errcode, v_errmsg AS errmsg, v_qid AS qid;
END;
$$;


ALTER PROCEDURE public.crm_insert_update_queue(IN p_qid integer, IN p_name character varying, IN p_description character varying, IN p_domainid integer, IN p_settingaudio text, IN p_announcement text, IN p_maximumcallers text, IN p_maximumwaittime text, IN p_outboundcallerid character varying, IN p_outboundnumber character varying, IN p_maxcallhandlingtimemm integer, IN p_callwrapuptime integer, IN p_outboundcallstatus integer, IN p_hoursofoperatioid integer, IN p_hours_of_operation text, IN p_outofbusinesshours text, IN p_status integer, IN p_type character varying, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_insert_update_user_onboard_settings_completion(character varying, integer, jsonb, smallint, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_insert_update_user_onboard_settings_completion(IN p_email_id character varying, IN p_domainid integer, IN p_flow_track jsonb, IN p_is_completed smallint, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_errcode INT;
    v_errmsg  TEXT;
BEGIN
    -- 1. Check if the record already exists
    IF EXISTS (
        SELECT 1 FROM tb_user_onboard_setting_flow 
        WHERE email_id = p_email_id AND "domainId" = p_domainId
    ) THEN
        -- 2. Perform UPDATE
        UPDATE tb_user_onboard_setting_flow
        SET  
            flow_track    = COALESCE(p_flow_track, flow_track),
            "isCompleted" = COALESCE(p_is_Completed, "isCompleted"),
            "updatedAt"   = CURRENT_TIMESTAMP
        WHERE email_id = p_email_id
          AND "domainId" = p_domainId;

        v_errcode := 1;
        v_errmsg  := 'Updated successfully';
    ELSE
        -- 3. Perform INSERT
        INSERT INTO tb_user_onboard_setting_flow (
            email_id, 
            "domainId", 
            flow_track, 
            "isCompleted", 
            "createdAt"
        ) 
        VALUES (
            p_email_id, 
            p_domainId, 
            p_flow_track, 
            p_is_Completed, 
            CURRENT_TIMESTAMP
        );

        v_errcode := 0;
        v_errmsg  := 'Inserted successfully';
    END IF;

    -- Open cursor for success result
    OPEN p_result FOR SELECT v_errcode AS errcode, v_errmsg AS errmsg;

EXCEPTION WHEN OTHERS THEN
    -- Capture any database errors
    v_errcode := -1;
    GET STACKED DIAGNOSTICS v_errmsg = MESSAGE_TEXT;
    
    -- Ensure cursor is open even on error
    IF NOT is_open(p_result) THEN
        OPEN p_result FOR SELECT v_errcode AS errcode, v_errmsg AS errmsg;
    ELSE
        -- If already open, we'd need to close/reopen or handle differently
        CLOSE p_result;
        OPEN p_result FOR SELECT v_errcode AS errcode, v_errmsg AS errmsg;
    END IF;
END;
$$;


ALTER PROCEDURE public.crm_insert_update_user_onboard_settings_completion(IN p_email_id character varying, IN p_domainid integer, IN p_flow_track jsonb, IN p_is_completed smallint, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_insert_voice_mail_message(character varying, integer, text, character varying, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_insert_voice_mail_message(IN p_session_id character varying, IN p_domain_id integer, IN p_voice_mail_summary text, IN p_voice_mail_subject character varying, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_errcode INT := -1;
    v_errmsg VARCHAR(50) := 'URL mesg added failed';
    v_voice_mail_url_id BIGINT;
BEGIN

    SELECT "voice_mail_url_id"
    INTO v_voice_mail_url_id
    FROM public."tb_voice_mail_msg"
    WHERE "session_id" = p_session_id
      AND "domain_id" = p_domain_id;

    IF v_voice_mail_url_id IS NULL THEN
        INSERT INTO public."tb_voice_mail_msg"(
            "session_id", 
            "domain_id", 
            "voice_mail_summary", 
            "voice_mail_subject", 
            "created_at", 
            "updated_at"
        )
        VALUES (
            p_session_id, 
            p_domain_id, 
            p_voice_mail_summary, 
            p_voice_mail_subject, 
            NOW(), 
            NOW()
        );

        v_errcode := 0;
        v_errmsg := 'Inserted successfully';
    END IF;

    OPEN p_result FOR 
    SELECT v_errcode AS err_code, v_errmsg AS errmsg;

END;
$$;


ALTER PROCEDURE public.crm_insert_voice_mail_message(IN p_session_id character varying, IN p_domain_id integer, IN p_voice_mail_summary text, IN p_voice_mail_subject character varying, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_insertupdate_chatbotpaymentintegration(integer, integer, integer, character varying, jsonb, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_insertupdate_chatbotpaymentintegration(IN p_tokenid integer, IN p_companyid integer, IN p_domainid integer, IN p_paymenttype character varying, IN p_secretjson jsonb, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_errcode INTEGER;
    v_errmsg VARCHAR(50);
    v_count INTEGER;
BEGIN

    IF p_companyid IS NULL THEN
        v_errcode := -1;
        v_errmsg := 'CompanyId should not be empty';

        OPEN p_result FOR
        SELECT v_errcode AS errcode, v_errmsg AS errmsg;
        RETURN;
    END IF;


    IF (p_tokenid IS NULL OR p_tokenid = 0) THEN
        IF EXISTS (
            SELECT 1 
            FROM ChatBotPaymentIntegration
            WHERE paymenttype = p_paymenttype
            AND companyid = p_companyid
        ) THEN
            v_errcode := -1;
            v_errmsg := 'Record already exists';

            OPEN p_result FOR
            SELECT v_errcode AS errcode, v_errmsg AS errmsg;
            RETURN;
        END IF;
    END IF;


    IF NOT EXISTS (
        SELECT 1 
        FROM ChatBotPaymentIntegration
        WHERE tokenid = p_tokenid
        AND companyid = p_companyid
    ) THEN

        INSERT INTO ChatBotPaymentIntegration
        (companyid, domainid, paymenttype, secretjson, createdat, updatedat)
        VALUES
        (p_companyid, p_domainid, p_paymenttype, p_secretjson, NOW(), NOW());

        GET DIAGNOSTICS v_count = ROW_COUNT;

        IF v_count > 0 THEN
            v_errcode := 1;
            v_errmsg := 'Inserted Successfully';
        ELSE
            v_errcode := -1;
            v_errmsg := 'Not Inserted';
        END IF;

    ELSE

        UPDATE ChatBotPaymentIntegration
        SET secretjson = p_secretjson,
            updatedat = NOW()
        WHERE tokenid = p_tokenid
        AND companyid = p_companyid;

        GET DIAGNOSTICS v_count = ROW_COUNT;

        IF v_count > 0 THEN
            v_errcode := 1;
            v_errmsg := 'Updated Successfully';
        ELSE
            v_errcode := -1;
            v_errmsg := 'Not Updated';
        END IF;

    END IF;


    OPEN p_result FOR
    SELECT v_errcode AS errcode, v_errmsg AS errmsg;

END;
$$;


ALTER PROCEDURE public.crm_insertupdate_chatbotpaymentintegration(IN p_tokenid integer, IN p_companyid integer, IN p_domainid integer, IN p_paymenttype character varying, IN p_secretjson jsonb, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_prequery_info(character varying, character varying, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_prequery_info(IN p_cli character varying, IN p_ddi character varying, IN p_domain_id integer, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$

DECLARE
    v_jsoncol JSONB;
    v_customerNumber BIGINT;
    v_ext SMALLINT;
    v_autoAttendant SMALLINT;
    v_greetingMessage TEXT;
    v_coid BIGINT;
    v_assistPriority SMALLINT;
    v_status SMALLINT;
    v_domainId INTEGER;
    v_humanAgent SMALLINT;
    v_queueId INTEGER;
    v_firstName VARCHAR(250);
BEGIN
    v_domainId := p_domain_id;
    IF LENGTH(p_cli) > 5 THEN v_customerNumber := p_cli::BIGINT;
    ELSE v_ext := p_cli::SMALLINT; v_customerNumber := p_ddi::BIGINT; END IF;
    SELECT status, "queueId" INTO v_status, v_queueId FROM did_purchase WHERE "domainId" = p_domain_id AND your_number = p_ddi;
    
	-- SELECT status, "assistPriority", "humanAgent" INTO v_autoAttendant, v_assistPriority, v_humanAgent FROM call_configuration WHERE "contactCenterNumber" = p_ddi::BIGINT AND "domainId" = p_domain_id;
    
	SELECT customer_id, first_name INTO v_coid, v_firstName FROM tb_customer_contact WHERE primary_phone_number = v_customerNumber::TEXT AND domain_id = p_domain_id ORDER BY customer_id LIMIT 1;
	
    v_jsoncol := jsonb_build_object(
        'cli_block', 0, 
        'firstName', COALESCE(v_firstName, ''), 
        'lastName', '', 
        'greetingMessage', COALESCE(v_greetingMessage, ''), 
        'status', COALESCE(v_status, 1), 
        'domainId', COALESCE(v_domainId, 0), 
        'queueId', v_queueId::TEXT -- This ensures it returns as a string
    );
	
	
    OPEN p_result FOR SELECT v_jsoncol AS jsoncol;
END;
$$;


ALTER PROCEDURE public.crm_prequery_info(IN p_cli character varying, IN p_ddi character varying, IN p_domain_id integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_queue_weigh_get_time_details(refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_queue_weigh_get_time_details(INOUT result_cursor refcursor DEFAULT 'result'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN
    OPEN result_cursor FOR
    SELECT json_agg(
        json_build_object(
            'qid', a."qid",
            'queueName', a."queueName",
            'queueType', a."queueType",
            'domainId', a."domainId",
            'queueList', a."queueList",
            'createdAt', a."createdAt"
        )
    ) AS json
    FROM "queueWeighTime" a;
END;
$$;


ALTER PROCEDURE public.crm_queue_weigh_get_time_details(INOUT result_cursor refcursor) OWNER TO postgres;

--
-- Name: crm_queue_weigh_time_details(integer, character varying, character varying, integer, text, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_queue_weigh_time_details(IN p_qid integer, IN p_queuename character varying, IN p_queuetype character varying, IN p_domainid integer, IN p_queuelist text, INOUT result_cursor refcursor DEFAULT 'result'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_errcode INTEGER DEFAULT -1;
    v_errmsg VARCHAR(255) DEFAULT 'failure';
    v_qwtId INTEGER;
    v_row_count INTEGER;
BEGIN
    SELECT "qwtId" INTO v_qwtId
    FROM "queueWeighTime" a
    WHERE a."qid" = p_qid
      AND a."domainId" = p_domainId
      AND a."queueType" = p_queueType
    LIMIT 1;

    IF v_qwtId IS NULL THEN
        INSERT INTO "queueWeighTime" ("qid", "queueName", "queueType", "domainId", "queueList", "createdAt", "updatedAt")
        VALUES (p_qid, p_queueName, p_queueType, p_domainId, p_queueList, NOW(), NOW());

        GET DIAGNOSTICS v_row_count = ROW_COUNT;
        IF v_row_count > 0 THEN
            v_errcode := 0;
            v_errmsg := 'inserted successfully';
        END IF;
    ELSE
        UPDATE "queueWeighTime" a
        SET "queueList" = p_queueList
        WHERE a."qwtId" = v_qwtId;

        GET DIAGNOSTICS v_row_count = ROW_COUNT;
        IF v_row_count > 0 THEN
            v_errcode := 0;
            v_errmsg := 'updated successfully';
        END IF;
    END IF;

    OPEN result_cursor FOR
    SELECT v_errcode AS errcode, v_errmsg AS errmsg;
END;
$$;


ALTER PROCEDURE public.crm_queue_weigh_time_details(IN p_qid integer, IN p_queuename character varying, IN p_queuetype character varying, IN p_domainid integer, IN p_queuelist text, INOUT result_cursor refcursor) OWNER TO postgres;

--
-- Name: crm_sc_getcroneforsmscampdtl(refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_sc_getcroneforsmscampdtl(INOUT result_cursor refcursor DEFAULT 'result'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_currentdatetime TIMESTAMP;
BEGIN

    v_currentdatetime :=
        date_trunc(
            'minute',
            (CURRENT_TIMESTAMP AT TIME ZONE 'UTC') AT TIME ZONE 'Asia/Kolkata'
        );

    DROP TABLE IF EXISTS tt_croneforsmscampdtl;

    CREATE TEMP TABLE tt_croneforsmscampdtl AS
    SELECT t1."campaignId",
           t1."domainId",
           t1.id
    FROM sc_campaign_master_schedule_dtl t1
    JOIN sc_campaign_master_dtl t2
        ON t1."campaignId" = t2.id
    LEFT JOIN wc_master_revert_status_details t3
        ON t2.campaignchatid = t3."campaignchatId"
    WHERE t2."triggerType" = 1
      AND COALESCE(t1."sentStatus", 0) <> 1
      AND date_trunc('minute', t1."schedulefromDate") <= v_currentdatetime
      AND t3."campaignchatId" IS NULL;

    -- This SELECT works only when directly executed in psql/pgAdmin
    OPEN result_cursor FOR SELECT t1.*
    FROM sc_campaign_master_dtl t1
    JOIN "tt_croneforsmscampdtl" t2
        ON t1.id = t2."campaignId";

    UPDATE sc_campaign_master_schedule_dtl t1
    SET "sentStatus" = 1
    FROM "tt_croneforsmscampdtl" t2
    WHERE t1.id = t2.id;

END;
$$;


ALTER PROCEDURE public.crm_sc_getcroneforsmscampdtl(INOUT result_cursor refcursor) OWNER TO postgres;

--
-- Name: crm_sc_insert_sms_sent_status_details(integer, character varying, character varying, character varying, character varying, character varying, character varying, character varying); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_sc_insert_sms_sent_status_details(IN p_domainid integer, IN p_campaignchatid character varying, IN p_frommobileno character varying, IN p_tomobileno character varying, IN p_status character varying, IN p_messageid character varying, IN p_description character varying, IN p_bulkid character varying)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_userid INTEGER;
    v_actionid INTEGER := 5;
    v_currentdate TIMESTAMP := CURRENT_TIMESTAMP;
    v_frommobileno VARCHAR(20);
BEGIN
 
    -- Get user id
    SELECT user_id INTO v_userid FROM tb_customer_contact WHERE primary_phone_number = p_tomobileno AND domain_id = p_domainid LIMIT 1;

    -- If fromMobileNo null get default sender
    IF p_frommobileno IS NULL THEN
        SELECT sendernumber INTO v_frommobileno FROM sc_smsconfigdtl WHERE domainid = p_domainid AND smsenable = 1 LIMIT 1;
    ELSE
        v_frommobileno := p_frommobileno;
    END IF;

    -- Action mapping
    IF p_status = 'DELIVERED' THEN
        v_actionid := 2;
    ELSIF p_status = 'CLICK' THEN
        v_actionid := 4;
    END IF;

    -- Check campaign exists
    IF EXISTS ( SELECT 1 FROM sc_campaign_master_dtl WHERE "domainId" = p_domainid AND campaignchatid = p_campaignchatid ) THEN

        INSERT INTO sc_sms_sent_status_details( domainid, campaignchatid, frommobileno, tomobileno, status, messageid, description, bulkid, sentat )
        VALUES( p_domainid, p_campaignchatid, COALESCE(v_frommobileno, p_frommobileno), p_tomobileno, p_status, p_messageid, p_description, p_bulkid, v_currentdate );
 
    END IF;

    -- Insert click/open count
    IF v_userid IS NOT NULL THEN
        INSERT INTO sms_tb_indiv_user_click_open_cnt_dtl( domainid, campaignchatid, userid, actionid, createdon )
        VALUES( p_domainid, p_campaignchatid, v_userid, v_actionid, v_currentdate );
    END IF;
 
END;
$$;


ALTER PROCEDURE public.crm_sc_insert_sms_sent_status_details(IN p_domainid integer, IN p_campaignchatid character varying, IN p_frommobileno character varying, IN p_tomobileno character varying, IN p_status character varying, IN p_messageid character varying, IN p_description character varying, IN p_bulkid character varying) OWNER TO postgres;

--
-- Name: crm_sms_campaign_create(integer, character varying, integer, character varying, text, integer, timestamp without time zone, timestamp without time zone, character varying, jsonb, character varying, character varying, character varying, character varying, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_sms_campaign_create(IN p_domainid integer, IN p_name character varying, IN p_targetaudiencetype integer, IN p_sendername character varying, IN p_templateprompt text, IN p_triggertype integer, IN p_schedulefromdate timestamp without time zone, IN p_scheduletodate timestamp without time zone, IN p_status character varying, IN p_contactdtls jsonb, IN p_campaigngoal character varying, IN p_campaignobjective character varying, IN p_websiteurl character varying, IN p_campaignchatid character varying, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_newInserted INTEGER;
    v_errCode SMALLINT := -1;
    v_errMsg VARCHAR(200) := 'Failed';
BEGIN

    IF p_campaignchatid IS NULL THEN
        OPEN p_result FOR SELECT v_errCode AS errCode, v_errMsg AS errMsg;
        RETURN;
    END IF;

    INSERT INTO sc_campaign_master_dtl(
        "domainId", name, "targetAudienceType",
        "senderName", "templatePrompt",
        "triggerType", status,
        "contactDtls", "campaignGoal",
        "campaignObjective", "websiteURL",
        campaignchatid
    )
    VALUES(
        p_domainId, p_name, p_targetAudienceType,
        p_senderName, p_templatePrompt,
        p_triggerType, p_status,
        p_contactDtls, p_campaignGoal,
        p_campaignObjective, p_websiteURL,
        p_campaignchatid
    )
    RETURNING id INTO v_newInserted;

    IF p_triggerType IN (0,1) THEN
        INSERT INTO sc_campaign_master_schedule_dtl(
            "domainId", "campaignId", "schedulefromDate","scheduletoDate"
        )
        VALUES(
            p_domainId, v_newInserted,
            COALESCE(p_schedulefromdate, NOW()),COALESCE(p_scheduletodate, NOW())
        );
    ELSE
        INSERT INTO sc_campaign_master_schedule_dtl(
            "domainId", "campaignId",
            "schedulefromDate", "scheduletoDate"
        )
        VALUES(
            p_domainId, v_newInserted,
            p_schedulefromdate, p_scheduletodate
        );
    END IF;

    v_errCode := 1;
    v_errMsg := 'Inserted Successfully';

    OPEN p_result FOR
    SELECT v_errCode AS errCode,
           v_errMsg AS errMsg;

EXCEPTION WHEN OTHERS THEN
    v_errCode := -1;
    v_errMsg := SQLERRM;

    OPEN p_result FOR
    SELECT v_errCode AS errCode,
           v_errMsg AS errMsg;
END;
$$;


ALTER PROCEDURE public.crm_sms_campaign_create(IN p_domainid integer, IN p_name character varying, IN p_targetaudiencetype integer, IN p_sendername character varying, IN p_templateprompt text, IN p_triggertype integer, IN p_schedulefromdate timestamp without time zone, IN p_scheduletodate timestamp without time zone, IN p_status character varying, IN p_contactdtls jsonb, IN p_campaigngoal character varying, IN p_campaignobjective character varying, IN p_websiteurl character varying, IN p_campaignchatid character varying, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_sp_exclude_email(jsonb, integer); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_sp_exclude_email(IN p_contactemail jsonb, IN p_domain_id integer, OUT o_existing_email_json jsonb, OUT o_excluded_email_json jsonb)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_cnt INT DEFAULT 0;
    v_cnt_exclude INT DEFAULT 0;
BEGIN

    DROP TABLE IF EXISTS tt_customer_details_excluded_mail;

    CREATE TEMP TABLE tt_customer_details_excluded_mail
    (
        cid INT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
        contactemail VARCHAR(255),
        deliverytime TIMESTAMP,
        domain_id INT
    );

    INSERT INTO tt_customer_details_excluded_mail (contactemail, deliverytime, domain_id)
    SELECT 
        x->>'contactEmail',
        (x->>'deliveryTime')::timestamp,
        p_domain_id
    FROM jsonb_array_elements(p_contactemail) AS x;

	 SELECT COUNT(*)
    INTO v_cnt
    FROM tt_customer_details_excluded_mail t
    WHERE EXISTS (
        SELECT 1
        FROM tb_customer_contact w
        WHERE w.primary_email = t.contactemail
		AND w.domain_id	= p_domain_id
    );

    SELECT COUNT(*)
    INTO v_cnt_exclude
    FROM tt_customer_details_excluded_mail t
    WHERE NOT EXISTS (
        SELECT 1
        FROM tb_customer_contact w
        WHERE w.primary_email = t.contactemail
		AND w.domain_id	= p_domain_id
    );

    IF v_cnt > 0 THEN
        SELECT jsonb_agg(
                   jsonb_build_object(
                       'contactEmail', contactemail,
                       'deliveryTime', to_char(deliverytime,'YYYY-MM-DD HH24:MI:SS')
                   )
               )
        INTO o_existing_email_json
        FROM tt_customer_details_excluded_mail t
        WHERE EXISTS (
            SELECT 1
            FROM tb_customer_contact w
            WHERE w.primary_email = t.contactemail
			AND w.domain_id	= p_domain_id
        );
    ELSE
        o_existing_email_json := '[]'::jsonb;
    END IF;

    IF v_cnt_exclude > 0 THEN
        SELECT jsonb_agg(
                   jsonb_build_object(
                       'contactEmail', contactemail,
                       'deliveryTime', to_char(deliverytime,'YYYY-MM-DD HH24:MI:SS')
                   )
               )
        INTO o_excluded_email_json
        FROM tt_customer_details_excluded_mail t
        WHERE NOT EXISTS (
            SELECT 1
            FROM tb_customer_contact w
            WHERE w.primary_email = t.contactemail
			AND w.domain_id	= p_domain_id
        );
    ELSE
        o_excluded_email_json := '[]'::jsonb;
    END IF; 

END;
$$;


ALTER PROCEDURE public.crm_sp_exclude_email(IN p_contactemail jsonb, IN p_domain_id integer, OUT o_existing_email_json jsonb, OUT o_excluded_email_json jsonb) OWNER TO postgres;

--
-- Name: crm_splitwithslno(character varying, character varying); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_splitwithslno(IN v_string character varying, IN v_delimiter character varying)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_idx INTEGER;
    v_slice VARCHAR(8000);
BEGIN
    DROP TABLE IF EXISTS tt_Split_temptable;
    CREATE TEMP TABLE tt_Split_temptable (
        "SlNo" SERIAL PRIMARY KEY,
        "items" VARCHAR(8000)
    );

    IF COALESCE(LENGTH(v_String), 0) < 1 THEN
        RETURN;
    END IF;

    WHILE LENGTH(v_String) > 0 LOOP
        v_idx := POSITION(v_Delimiter IN v_String);

        IF v_idx > 0 THEN
            v_slice := LEFT(v_String, v_idx - 1);
            v_String := SUBSTRING(v_String FROM v_idx + 1);
        ELSE
            v_slice := v_String;
            v_String := '';
        END IF;

        IF LENGTH(v_slice) > 0 THEN
            INSERT INTO tt_Split_temptable ("items") VALUES (v_slice);
        END IF;
    END LOOP;
END;
$$;


ALTER PROCEDURE public.crm_splitwithslno(IN v_string character varying, IN v_delimiter character varying) OWNER TO postgres;

--
-- Name: crm_up_callbacksessionid(character varying, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_up_callbacksessionid(IN p_sessionid character varying, INOUT result_cursor refcursor DEFAULT 'result'::refcursor)
    LANGUAGE plpgsql
    AS $$

DECLARE
    v_errcode INTEGER DEFAULT -1;
    v_errmsg VARCHAR(50) DEFAULT 'Updated Failed';
    v_row_count INTEGER;
BEGIN
    UPDATE "tb_CallHistory"
    SET "callBack" = 1,
	updatedAt = now()
    WHERE "sessionId" = p_sessionId;

    GET DIAGNOSTICS v_row_count = ROW_COUNT;
    IF v_row_count > 0 THEN
        v_errcode := 0;
        v_errmsg := 'Updated Successfully';
    END IF;

    OPEN result_cursor FOR
    SELECT v_errcode AS errcode, v_errmsg AS errmsg;
END;
$$;


ALTER PROCEDURE public.crm_up_callbacksessionid(IN p_sessionid character varying, INOUT result_cursor refcursor) OWNER TO postgres;

--
-- Name: crm_up_getvoiceagentdetails(integer, integer, character varying, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_up_getvoiceagentdetails(IN p_domainid integer, IN p_queueid integer, IN p_skills character varying, INOUT result_cursor refcursor DEFAULT 'result'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_Skills VARCHAR(1000);
BEGIN
    v_Skills := COALESCE(p_Skills, '');

    DROP TABLE IF EXISTS tt_skilledAgent;
    CREATE TEMP TABLE tt_skilledAgent (
        "UserID" BIGINT,
        "skillMatch" SMALLINT
    );

    IF v_Skills <> '' THEN
        CALL crm_splitwithslno(v_Skills, ',');
 

        INSERT INTO tt_skilledAgent("UserID", "skillMatch")
        SELECT SM.usid, COUNT(1)::SMALLINT AS "skillMatch"
        FROM tt_split_temptable TT
        JOIN skills S ON TT.items = S.skill_name AND S."domainId" = p_DomainId
        JOIN "skillMapping" SM ON SM.skid = S.sid AND SM."domainId" = S."domainId"
        GROUP BY SM.usid;
 
        -- DROP TABLE IF EXISTS tt_split_temptable;
    END IF;

    OPEN result_cursor FOR
    SELECT json_build_object(
        'agent_extn', U.ext,
        'agent_name', U."UserName",
        'agent_skills', (
            SELECT json_agg(json_build_object('Skid', SK.skid, 'value', ss.skill_name))
            FROM "skillMapping" SK
            JOIN skills ss ON ss.sid = SK.skid AND ss."domainId" = SK."domainId"
            WHERE U."UserID" = SK.usid AND SK."domainId" = p_DomainId
        ),
        'userId', CAST(U."UserID" AS VARCHAR(50)),
        'email', U."emailId",
        'agent_status', A_S."statusName",
        'agent_statusId', A_S.aid,
        'isVoice', U."isVoice",
        'isVoiceCallTransfer', A_S."isVoiceCallTransfer",
        'routingMethod', CASE
            WHEN RP."routingMethod" = 'Skill Based Routing' THEN 2
            WHEN RP."routingMethod" = 'Long wait time Based Routing' THEN 1
            ELSE NULL
        END,
        'delay', R_MAP.delay_in_seconds
    ) AS "AgentDetails"
    FROM "tb_User" U
    JOIN agent_status A_S ON A_S.aid = U."StatusID" AND A_S."domainId" = U."domainId"
    LEFT JOIN routingqueueprofilemapping R_MAP ON R_MAP.rqmid = U.routing_profile_id
        AND R_MAP."domainId" = U."domainId"
        AND R_MAP.qid = p_QueueId
        AND R_MAP.voice = 1
    LEFT JOIN routing_profile RP ON RP.rid = R_MAP.rqmid AND RP."domainId" = R_MAP."domainId"
    LEFT JOIN tt_skilledAgent SA ON SA."UserID" = U."UserID"
    WHERE U."domainId" = p_DomainId
      AND U.roleid IN (3, 5, 6)
      AND U.voice = 1
      AND U."isAvailableForVoice" = 1
      AND U."inboundCall" = 1
      AND A_S."isVoice" = 1
      AND A_S."isVoiceCallTransfer" = 0
    ORDER BY SA."skillMatch" DESC NULLS LAST, R_MAP.priority ASC NULLS LAST, U.roleid DESC, U."updatedAt" ASC
    LIMIT 1;
    -- Note: tt_skilledAgent will be auto-dropped at session end
END;
$$;


ALTER PROCEDURE public.crm_up_getvoiceagentdetails(IN p_domainid integer, IN p_queueid integer, IN p_skills character varying, INOUT result_cursor refcursor) OWNER TO postgres;

--
-- Name: crm_up_insertgetcustomercontact(integer, character varying, character varying, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_up_insertgetcustomercontact(IN p_domainid integer, IN p_channeltype character varying, IN p_channelvalue character varying, INOUT result_cursor refcursor DEFAULT 'result'::refcursor)
    LANGUAGE plpgsql
    AS $$

DECLARE
    v_coid INTEGER;
    v_phoneNumber TEXT;
    v_email TEXT;
    v_facebook VARCHAR(150);
    v_instagram VARCHAR(150);
    v_twitter VARCHAR(150);
    v_curdate TIMESTAMP;
    v_firstName VARCHAR(150);
BEGIN
    v_curdate := NOW();

    IF p_ChannelType IN ('Call') THEN
        v_phoneNumber := p_ChannelValue;

 SELECT a.customer_id, a.first_name INTO v_coid, v_firstName
        FROM tb_customer_contact a 
        WHERE a.domain_id = p_domainId
          AND a.primary_phone_number = p_ChannelValue
		  LIMIT 1;
		
		if v_coid is null THEN
		
SELECT a.customer_id, a.first_name INTO v_coid, v_firstName
        FROM tb_customer_contact a 
		join tb_secondary_customer_contact b on a.customer_id = b.fk_customer_id
        WHERE a.domain_id = p_domainId
          AND b.secondary_phone_number = p_ChannelValue
        LIMIT 1;
		
		end if;
		 
    ELSIF p_ChannelType = 'Email' THEN
        v_email := p_ChannelValue;


 SELECT a.customer_id, a.first_name INTO v_coid, v_firstName
        FROM tb_customer_contact a 
        WHERE a.domain_id = p_domainId
          AND a.primary_email = p_ChannelValue
		  LIMIT 1;
		
		if v_coid is null THEN
		
SELECT a.customer_id, a.first_name INTO v_coid, v_firstName
        FROM tb_customer_contact a 
		join tb_secondary_customer_contact b on a.customer_id = b.fk_customer_id
        WHERE a.domain_id = p_domainId
          AND b.secondary_email = p_ChannelValue
        LIMIT 1;
		
		end if;
		

    END IF;
 

    IF v_coid IS NULL THEN
        INSERT INTO tb_customer_contact("domain_id","first_name","primary_phone_number","primary_email","created_at","updated_at" )
        VALUES (p_domainId,v_firstName,v_phoneNumber,v_email,now(),now())
        RETURNING "customer_id" INTO v_coid;
    END IF;

    OPEN result_cursor FOR select p_domainId,v_firstName,v_phoneNumber,v_email,v_coid;
END;
$$;


ALTER PROCEDURE public.crm_up_insertgetcustomercontact(IN p_domainid integer, IN p_channeltype character varying, IN p_channelvalue character varying, INOUT result_cursor refcursor) OWNER TO postgres;

--
-- Name: crm_up_insertupdateemailhistory(bigint, character varying, bigint, bigint, character varying, character varying, character varying, integer, integer, integer, bigint, integer, integer, jsonb, integer, character varying, character varying, character varying, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_up_insertupdateemailhistory(IN p_emailhistoryid bigint, IN p_sessionid character varying, IN p_sessionstarttime bigint, IN p_sessionendtime bigint, IN p_fromemail character varying, IN p_toemail character varying, IN p_emailsubject character varying, IN p_companyid integer, IN p_domainid integer, IN p_isinbound integer, IN p_assignedon bigint, IN p_issessionclosed integer, IN p_customerid integer, IN p_message jsonb, IN p_agentid integer, IN p_threadmessageid character varying, IN p_graphmessageid character varying, IN p_conversationid character varying, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$

DECLARE
    v_CurDate TIMESTAMP;
    v_existing_EmailHistoryId BIGINT;
    v_EmailHistoryId BIGINT;
    v_errCode INTEGER := -1;
    v_errMsg VARCHAR(150) := 'No Email';
BEGIN
    v_CurDate := NOW();
    SELECT "EmailHistoryId" INTO v_existing_EmailHistoryId FROM "tb_EmailHistory" WHERE "EmailHistoryId" = p_EmailHistoryId;
    IF v_existing_EmailHistoryId IS NULL THEN
        INSERT INTO "tb_EmailHistory"("SessionId", "AgentId", "SessionStartTime", "SessionEndTime", "FromEmail", "ToEmail", "EmailSubject", "CompanyId", "DomainId", "IsInBound", "AssignedOn", "IsSessionClosed", "CustomerId", "threadMessageId", "CreatedAt", "UpdatedAt", "graphMessageId", "conversationId")
        VALUES (p_SessionId, p_AgentId, p_SessionStartTime, p_SessionEndTime, p_FromEmail, p_ToEmail, p_EmailSubject, p_CompanyId, p_DomainId, p_IsInBound, p_AssignedOn, p_IsSessionClosed, p_CustomerId, p_threadMessageId, v_CurDate, v_CurDate, p_graphMessageId, p_conversationId)
        RETURNING "EmailHistoryId" INTO v_EmailHistoryId;
        v_errCode := 0; v_errMsg := 'Successfully';
        IF p_Message IS NOT NULL THEN INSERT INTO "tb_EmailMessage"("EmailHistoryId", "Message", "CreatedAt", "UpdatedAt") VALUES (v_EmailHistoryId, p_Message, v_CurDate, v_CurDate); END IF;
    ELSE
        v_EmailHistoryId := v_existing_EmailHistoryId;
        UPDATE "tb_EmailHistory" SET "SessionId" = COALESCE(p_SessionId, "SessionId"), "AgentId" = COALESCE(p_AgentId, "AgentId"), "SessionStartTime" = COALESCE(p_SessionStartTime, "SessionStartTime"), "SessionEndTime" = COALESCE(p_SessionEndTime, "SessionEndTime"), "FromEmail" = COALESCE(p_FromEmail, "FromEmail"), "ToEmail" = COALESCE(p_ToEmail, "ToEmail"), "EmailSubject" = COALESCE(p_EmailSubject, "EmailSubject"), "CompanyId" = COALESCE(p_CompanyId, "CompanyId"), "DomainId" = COALESCE(p_DomainId, "DomainId"), "IsInBound" = COALESCE(p_IsInBound, "IsInBound"), "AssignedOn" = COALESCE(p_AssignedOn, "AssignedOn"), "IsSessionClosed" = COALESCE(p_IsSessionClosed, "IsSessionClosed"), "CustomerId" = COALESCE(p_CustomerId, "CustomerId"), "threadMessageId" = COALESCE(p_threadMessageId, "threadMessageId"), "UpdatedAt" = v_CurDate WHERE "EmailHistoryId" = v_EmailHistoryId;
        UPDATE "tb_EmailMessage" SET "Message" = "Message" || jsonb_build_array(p_Message), "UpdatedAt" = v_CurDate WHERE "EmailHistoryId" = v_EmailHistoryId;
        v_errCode := 0; v_errMsg := 'Email Updated Successfully';
    END IF;
    OPEN p_result FOR SELECT v_errCode AS errcode, v_errMsg AS errmsg, v_EmailHistoryId AS "EmailHistoryId";
END;
$$;


ALTER PROCEDURE public.crm_up_insertupdateemailhistory(IN p_emailhistoryid bigint, IN p_sessionid character varying, IN p_sessionstarttime bigint, IN p_sessionendtime bigint, IN p_fromemail character varying, IN p_toemail character varying, IN p_emailsubject character varying, IN p_companyid integer, IN p_domainid integer, IN p_isinbound integer, IN p_assignedon bigint, IN p_issessionclosed integer, IN p_customerid integer, IN p_message jsonb, IN p_agentid integer, IN p_threadmessageid character varying, IN p_graphmessageid character varying, IN p_conversationid character varying, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_up_transafercallmessage(bigint, bigint); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_up_transafercallmessage(IN p_sourcecalldurationhistoryid bigint, IN p_destinationcalldurationhistoryid bigint)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_CurDate TIMESTAMP;
    v_errCode INTEGER DEFAULT -1;
    v_errMsg VARCHAR(150) DEFAULT 'No Message Transfered';
    v_SourceCallDurationHistoryId BIGINT;
BEGIN
    v_CurDate := NOW();

    SELECT "CallDurationHistoryId" INTO v_SourceCallDurationHistoryId
    FROM "tb_CallMessage"
    WHERE "CallDurationHistoryId" = p_SourceCallDurationHistoryId;

    IF v_SourceCallDurationHistoryId IS NOT NULL THEN
        INSERT INTO "tb_CallMessage" (
            "CallDurationHistoryId",
            "CallHistoryId",
            "Message",
            "createdAt",
            "updatedAt"
        )
        SELECT p_DestinationCallDurationHistoryId,
               "CallHistoryId",
               "Message",
               v_CurDate,
               v_CurDate
        FROM "tb_CallMessage"
        WHERE "CallDurationHistoryId" = p_SourceCallDurationHistoryId;

        v_errCode := 0;
        v_errMsg := 'Message Transfered Successfully';
    END IF;
END;
$$;


ALTER PROCEDURE public.crm_up_transafercallmessage(IN p_sourcecalldurationhistoryid bigint, IN p_destinationcalldurationhistoryid bigint) OWNER TO postgres;

--
-- Name: crm_up_updateagentstatus(integer, integer, character varying, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_up_updateagentstatus(IN p_domainid integer, IN p_ext integer, IN p_statusname character varying, INOUT result_cursor refcursor DEFAULT 'result'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_errcode INTEGER DEFAULT -1;
    v_errmsg VARCHAR(100) DEFAULT 'Failure';
    v_CurDate TIMESTAMP;
    v_status_id INTEGER;
    v_row_count INTEGER;
BEGIN
    v_CurDate := NOW();

    SELECT "aid" INTO v_status_id
    FROM "agent_status"
    WHERE "domainId" = p_DomainId
      AND "statusName" = p_statusName
    LIMIT 1;

    UPDATE "tb_User" U
    SET "StatusID" = v_status_id,
        "updatedAt" = v_CurDate,
        "stateTimer" = CAST(EXTRACT(EPOCH FROM NOW()) AS VARCHAR)
    WHERE U."domainId" = p_DomainId
      AND U."ext" = p_ext;

    GET DIAGNOSTICS v_row_count = ROW_COUNT;
    IF v_row_count > 0 THEN
        v_errcode := 0;
        v_errmsg := 'Updated Successfully';
    END IF;

    OPEN result_cursor FOR
    SELECT v_errcode AS errcode, v_errmsg AS errmsg;
END;
$$;


ALTER PROCEDURE public.crm_up_updateagentstatus(IN p_domainid integer, IN p_ext integer, IN p_statusname character varying, INOUT result_cursor refcursor) OWNER TO postgres;

--
-- Name: crm_update_agent_queue(integer, integer, character varying, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_update_agent_queue(IN p_domainid integer, IN p_userid integer, IN p_queueid character varying, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_UserID INTEGER;
    v_errcode INTEGER := -1;
    v_errmsg VARCHAR(100) := 'Failure';
BEGIN
    SELECT "UserID" INTO v_UserID FROM "tb_User" WHERE "domainId" = p_domainId AND "UserID" = p_userId LIMIT 1;
    IF v_UserID IS NOT NULL THEN
        UPDATE "tb_User" SET "queueId" = p_queueId, "updatedAt" = NOW() WHERE "UserID" = v_UserID;
        v_errcode := 0; v_errmsg := 'Updated Successfully';
    END IF;
    OPEN p_result FOR SELECT v_errcode AS errcode, v_errmsg AS errmsg;
END;
$$;


ALTER PROCEDURE public.crm_update_agent_queue(IN p_domainid integer, IN p_userid integer, IN p_queueid character varying, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_update_business_queue(integer, integer, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_update_business_queue(IN p_domainid integer, IN p_cid integer, IN p_qid integer, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_qid INTEGER;
    v_errcode INTEGER := -1;
    v_errmsg VARCHAR(50) := 'Failure';
BEGIN
    SELECT qid INTO v_qid FROM queue WHERE "domainId" = p_domainId AND qid = p_qid LIMIT 1;
    IF v_qid IS NOT NULL THEN
        UPDATE queue SET "hoursOfOperatioId" = p_cid, "updatedAt" = NOW() WHERE qid = v_qid;
        v_errcode := 1; v_errmsg := 'Updated';
    END IF;
    OPEN p_result FOR SELECT v_errcode AS errcode, v_errmsg AS errmsg, v_qid AS qid;
END;
$$;


ALTER PROCEDURE public.crm_update_business_queue(IN p_domainid integer, IN p_cid integer, IN p_qid integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_update_businessholiday_status(integer, integer, smallint, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_update_businessholiday_status(IN p_fkbusinessid integer, IN p_holidayid integer, IN p_status smallint, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_HolidayId INTEGER;
    v_curdate TIMESTAMP;
    v_errcode INTEGER := -1;
    v_errmsg VARCHAR(60) := 'Holiday Status Update Failed';
BEGIN
    v_curdate := NOW();
    SELECT hid INTO v_HolidayId FROM "tb_Business_Holidays" WHERE hid = p_holidayId AND "fk_businessId" = p_fkBusinessId;
    IF v_HolidayId IS NOT NULL THEN
        UPDATE "tb_Business_Holidays" SET "isStatus" = p_status, "updatedAt" = v_curdate WHERE hid = v_HolidayId;
        v_errcode := 1; v_errmsg := 'Holiday Status Updated successfully';
    END IF;
    OPEN p_result FOR SELECT v_errcode AS errcode, v_errmsg AS errmsg;
END;
$$;


ALTER PROCEDURE public.crm_update_businessholiday_status(IN p_fkbusinessid integer, IN p_holidayid integer, IN p_status smallint, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_update_customer_un_subscribe(character varying, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_update_customer_un_subscribe(IN p_primary_email character varying, IN p_domain_id integer, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_customer_id INTEGER;
    v_errcode INTEGER;
    v_errmsg TEXT;
BEGIN

    -- Check if record exists
    SELECT customer_id
    INTO v_customer_id
    FROM tb_customer_contact
    WHERE primary_email = p_primary_email
    AND domain_id = p_domain_id;

    IF v_customer_id IS NOT NULL 
	THEN
 
        UPDATE tb_customer_contact
        SET
            "isUnSub_Email"      = 1,
            updated_at         = NOW()
        WHERE customer_id = v_customer_id
        AND domain_id = p_domain_id;

            v_errcode := 1;
            v_errmsg := 'Update sucessfully';

    END IF;

    -- Return result
    OPEN p_result FOR
        SELECT v_errcode AS errcode,
               v_errmsg AS errmsg;

END;
$$;


ALTER PROCEDURE public.crm_update_customer_un_subscribe(IN p_primary_email character varying, IN p_domain_id integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_update_did_purchase_bot_status(integer, integer, smallint, character varying, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_update_did_purchase_bot_status(IN p_domainid integer, IN p_isbot integer, IN p_isbotpurchased smallint, IN p_your_number character varying, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$

DECLARE
    v_row_count INTEGER;
    v_errcode INTEGER := 0;
    v_errmsg VARCHAR(50) := 'Success';
BEGIN
    UPDATE  did_purchase 
	SET "isBot" = p_isBot, 
	"isBotPurchased" = p_isBotPurchased,
	"updatedAt" = now()
	WHERE "domainId" = p_domainId AND your_number = p_your_number;
    GET DIAGNOSTICS v_row_count = ROW_COUNT;
    IF v_row_count > 0 THEN v_errcode := 0; v_errmsg := 'updated'; END IF;
    OPEN p_result FOR SELECT v_errcode AS errcode, v_errmsg AS errmsg;
END;
$$;


ALTER PROCEDURE public.crm_update_did_purchase_bot_status(IN p_domainid integer, IN p_isbot integer, IN p_isbotpurchased smallint, IN p_your_number character varying, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_update_email_bot_status(integer, integer, smallint, character varying, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_update_email_bot_status(IN p_domainid integer, IN p_isbot integer, IN p_isbotpurchased smallint, IN p_emailaddress character varying, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$

DECLARE
    v_row_count INTEGER;
    v_errcode INTEGER := -1;
    v_errmsg VARCHAR(50) := 'Failed';
BEGIN
    UPDATE email_config 
	SET "isBot" = p_isBot, 
	"isBotPurchased" = p_isBotPurchased 
	, "updatedAt" = now()
	WHERE "domainId" = p_domainId AND "emailAddress" = p_emailAddress;
    GET DIAGNOSTICS v_row_count = ROW_COUNT;
    IF v_row_count > 0 THEN v_errcode := 0; v_errmsg := 'updated'; END IF;
    OPEN p_result FOR SELECT v_errcode AS errcode, v_errmsg AS errmsg;
END;
$$;


ALTER PROCEDURE public.crm_update_email_bot_status(IN p_domainid integer, IN p_isbot integer, IN p_isbotpurchased smallint, IN p_emailaddress character varying, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_update_email_open_status(integer, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_update_email_open_status(IN p_domainid integer, IN p_historyid integer, INOUT p_cursor refcursor DEFAULT 'p_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_emailhistoryid INT;
    v_errcode        INT := -1;
    v_errmsg         VARCHAR := 'Message open failed!';
BEGIN

    SELECT "EmailHistoryId"
    INTO v_emailhistoryid
    FROM "tb_EmailHistory"
    WHERE "DomainId" = p_domainid
      AND "EmailHistoryId" = p_historyid;

    IF v_emailhistoryid IS NOT NULL THEN
        UPDATE "tb_EmailHistory"
        SET "isOpened" = 1
        WHERE "EmailHistoryId" = v_emailhistoryid;

        v_errcode := 1;
        v_errmsg  := 'Message successfully opened';
    END IF;

    OPEN p_cursor FOR
        SELECT v_errcode AS errcode,
               v_errmsg  AS errmsg;

END;
$$;


ALTER PROCEDURE public.crm_update_email_open_status(IN p_domainid integer, IN p_historyid integer, INOUT p_cursor refcursor) OWNER TO postgres;

--
-- Name: crm_update_email_trash(integer, integer, smallint, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_update_email_trash(IN p_domainid integer, IN p_historyid integer, IN p_istrash smallint, INOUT p_cursor refcursor DEFAULT 'p_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_emailhistoryid INT;
    v_errcode        INT := -1;
    v_errmsg         VARCHAR := 'Message move to trash failed!';
BEGIN

    SELECT "EmailHistoryId"
    INTO v_emailhistoryid
    FROM "tb_EmailHistory"
    WHERE "DomainId" = p_domainid
      AND "EmailHistoryId" = p_historyid;

    IF v_emailhistoryid IS NOT NULL THEN
        UPDATE "tb_EmailHistory"
        SET "is_trash" = p_istrash
        WHERE "EmailHistoryId" = v_emailhistoryid;

        v_errcode := 1;
        v_errmsg  := 'Message successfully moved to the trash.';
    END IF;

    OPEN p_cursor FOR
        SELECT v_errcode AS errcode,
               v_errmsg  AS errmsg;

END;
$$;


ALTER PROCEDURE public.crm_update_email_trash(IN p_domainid integer, IN p_historyid integer, IN p_istrash smallint, INOUT p_cursor refcursor) OWNER TO postgres;

--
-- Name: crm_update_mql_sql_status(integer, text, smallint, smallint, integer); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_update_mql_sql_status(IN p_domain_id integer, IN p_contact_value text, IN p_is_mql smallint, IN p_is_sql smallint, IN p_customer_lead_id integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_customer_id INT;
BEGIN

    SELECT customer_id into v_customer_id
     FROM tb_customer_contact
    WHERE domain_id = p_domain_id
      AND (primary_phone_number = p_contact_value
           OR primary_email = p_contact_value)
    LIMIT 1;
	
    IF v_customer_id IS NOT NULL THEN
        UPDATE tb_customer_contact
        SET is_mql = p_is_mql,
            is_sql = p_is_sql,
			sales_agent_id =p_customer_lead_id
        WHERE customer_id = v_customer_id;

        RAISE NOTICE 'Status updated successfully';
    ELSE
        RAISE NOTICE 'Customer not found';
    END IF;

END;
$$;


ALTER PROCEDURE public.crm_update_mql_sql_status(IN p_domain_id integer, IN p_contact_value text, IN p_is_mql smallint, IN p_is_sql smallint, IN p_customer_lead_id integer) OWNER TO postgres;

--
-- Name: crm_update_queue_status(integer, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_update_queue_status(IN p_qid integer, IN p_status integer, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_qid INTEGER;
    v_errcode INTEGER := -1;
    v_errmsg VARCHAR(50) := 'Failure';
BEGIN
    SELECT qid INTO v_qid FROM queue WHERE qid = p_qid;
    IF v_qid IS NOT NULL THEN
        UPDATE queue SET "updatedAt" = NOW(), status = p_status WHERE qid = v_qid;
        v_errcode := 1; v_errmsg := 'Updated';
    END IF;
    OPEN p_result FOR SELECT v_errcode AS errcode, v_errmsg AS errmsg, v_qid AS qid;
END;
$$;


ALTER PROCEDURE public.crm_update_queue_status(IN p_qid integer, IN p_status integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_updatevideosessiondisposition_details(integer, text, text, integer, integer, text, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_updatevideosessiondisposition_details(IN p_domainid integer, IN p_meetingid text, IN p_summary text, IN p_dispositionid integer, IN p_aftervideoworktime integer, IN p_subject text, IN p_agentextn integer, INOUT p_result refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_errcode INT DEFAULT -1;
    v_errmsg VARCHAR(50) DEFAULT 'Error : Not Updated';
    v_VideoHistoryId INT;
    v_VideoDurationHistoryId INT;
BEGIN

    -- Get latest VideoHistoryId
    WITH CTE AS (
        SELECT "VideoHistoryId",
               ROW_NUMBER() OVER (PARTITION BY "meetingId" ORDER BY "updatedAt" DESC) AS rr
        FROM "tb_VideoHistory"
        WHERE "meetingId" = p_meetingId
        AND "domainId" = p_domainId
    )
    SELECT "VideoHistoryId"
    INTO v_VideoHistoryId
    FROM CTE
    WHERE rr = 1
    LIMIT 1;

    -- Get latest VideoDurationHistoryId
    SELECT "VideoDurationHistoryId"
    INTO v_VideoDurationHistoryId
    FROM "tb_VideoDurationHistory"
    WHERE "VideoHistoryId" = v_VideoHistoryId
    AND "AgentId" = p_agentExtn
    ORDER BY "VideoDurationHistoryId" DESC
    LIMIT 1;

    -- Check existence
    IF EXISTS (
        SELECT 1
        FROM "tb_VideoHistory"
        WHERE "domainId" = p_domainId
        AND "meetingId" = p_meetingId
        LIMIT 1
    ) THEN

        UPDATE "tb_VideoDurationHistory" VDH
        SET
            "summary" = COALESCE(p_summary, VDH."summary"),
            "dispositionId" = CASE
                                WHEN p_dispositionId IS NULL OR p_dispositionId = 0
                                THEN VDH."dispositionId"
                                ELSE p_dispositionId
                              END,
            "afterVideoWorkTime" = COALESCE(p_afterVideoWorkTime, VDH."afterVideoWorkTime"),
            "subject" = COALESCE(p_subject, VDH."subject"),
            "updatedAt" = CURRENT_TIMESTAMP
        FROM "tb_VideoHistory" VH
        WHERE VDH."VideoHistoryId" = VH."VideoHistoryId"
        AND VH."meetingId" = p_meetingId
        AND VDH."AgentId" = p_agentExtn
        AND VDH."VideoDurationHistoryId" = v_VideoDurationHistoryId;

    END IF;

    v_errcode := 0;
    v_errmsg := 'Success : Updated';

    OPEN p_result FOR
    SELECT v_errcode AS errcode,
           v_errmsg AS errmsg,
           v_VideoHistoryId AS "VideoHistoryId";

END;
$$;


ALTER PROCEDURE public.crm_updatevideosessiondisposition_details(IN p_domainid integer, IN p_meetingid text, IN p_summary text, IN p_dispositionid integer, IN p_aftervideoworktime integer, IN p_subject text, IN p_agentextn integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_user_update_info(character varying, integer, integer, integer, integer, character varying, character varying, integer, integer, integer, integer, character varying, character varying, character varying, character varying, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_user_update_info(IN p_emailid character varying, IN p_domainid integer, IN p_isvoice integer, IN p_isemail integer, IN p_issms integer, IN p_pri_language character varying, IN p_sec_language character varying, IN p_inboundcall integer, IN p_outboundcampaigncall integer, IN p_outboundcall integer, IN p_role_id integer, IN p_username character varying, IN p_region character varying, IN p_state character varying, IN p_experience_level character varying, IN p_performance_score integer, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$

DECLARE
    v_errcode INTEGER := -1;
    v_errmsg VARCHAR(100) := 'Failure';
    v_UserID INTEGER;
    v_ext INTEGER;
    v_aid INTEGER;
BEGIN
    -- Get user details
    SELECT "UserID", ext
    INTO v_UserID, v_ext
    FROM "tb_User"
    WHERE "emailId" = p_emailId
    AND "domainId" = p_domainId;

    -- Get addon language info
    SELECT aid INTO v_aid
    FROM "tb_userAddonLanguageInfo"
    WHERE "domainId" = p_domainId
    AND ext = v_ext
    AND "userId" = v_UserID;

    IF v_UserID IS NOT NULL THEN
        -- Update user record (12 param version - no chat, socialMedia, chatLimit)
        UPDATE "tb_User"
        SET voice = COALESCE(p_isVoice, voice),
            "isVoice" = COALESCE(p_isVoice, "isVoice"),
            email = COALESCE(p_isEmail, email),
            sms = COALESCE(p_isSms, sms),
            "inboundCall" = COALESCE(p_inboundCall, "inboundCall"),
            "callTypeId" = COALESCE(p_outboundCall, "callTypeId"),
            "outboundCampaignCall" = COALESCE(p_outboundCampaignCall, "outboundCampaignCall"),
            roleid = COALESCE(p_role_id, roleid),
            "UserName" = COALESCE(p_UserName, "UserName"),
			region = COALESCE(p_region, region),
			state = COALESCE(p_state, state),
			experience_level = COALESCE(p_experience_level, experience_level),
			performance_score = COALESCE(p_performance_score, performance_score),
			"updatedAt" = now()
        WHERE "UserID" = v_UserID;

        IF v_aid IS NOT NULL THEN
            -- Update existing language info
            UPDATE "tb_userAddonLanguageInfo"
            SET pri_language = COALESCE(p_pri_language, pri_language),
                sec_language = COALESCE(p_sec_language, sec_language),
                "updatedAt" = NOW()
            WHERE "userId" = v_UserID;
        ELSE
            -- Insert new language info
            INSERT INTO "tb_userAddonLanguageInfo"("domainId", "userId", ext, pri_language, sec_language, "createdAt", "updatedAt")
            VALUES(p_domainId, v_UserID, v_ext, p_pri_language, p_sec_language, NOW(), NOW());
        END IF;

        v_errcode := 0;
        v_errmsg := 'Success';
    END IF;

    OPEN p_result FOR SELECT v_errcode AS errcode, v_errmsg AS errmsg;
END;
$$;


ALTER PROCEDURE public.crm_user_update_info(IN p_emailid character varying, IN p_domainid integer, IN p_isvoice integer, IN p_isemail integer, IN p_issms integer, IN p_pri_language character varying, IN p_sec_language character varying, IN p_inboundcall integer, IN p_outboundcampaigncall integer, IN p_outboundcall integer, IN p_role_id integer, IN p_username character varying, IN p_region character varying, IN p_state character varying, IN p_experience_level character varying, IN p_performance_score integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_videomeet_create_message_details(integer, character varying, jsonb, integer, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_videomeet_create_message_details(IN p_domainid integer, IN p_meeting_id character varying, IN p_message jsonb, IN p_agent_id integer, INOUT p_result refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE
    v_errcode INT := -1;
    v_errmsg VARCHAR(55);
    v_AgentId BIGINT;
    v_VideoHistoryId BIGINT;
    v_VideoDurationHistoryId BIGINT;
    v_prev_Message JSONB;
    v_merged_msg JSONB;
    v_rowcount INT;
BEGIN

    -- Get AgentId
    IF p_agent_id IS NOT NULL THEN
        SELECT "UserID"
        INTO v_AgentId
        FROM "tb_User"
        WHERE "domainId" = p_domainId
        AND "UserID" = p_agent_id
        LIMIT 1;
    END IF;

    -- Validation
    IF p_domainId IS NULL OR p_domainId = 0 THEN
        v_errmsg := 'DomainId should not be empty';
        OPEN p_result FOR SELECT v_errcode AS errcode, v_errmsg AS errmsg, NULL::BIGINT AS "VideoHistoryId";
        RETURN;
    END IF;

    IF p_agent_id IS NULL OR p_agent_id = 0 THEN
        v_errmsg := 'Agentextn should not be empty or null';
        OPEN p_result FOR SELECT v_errcode AS errcode, v_errmsg AS errmsg, NULL::BIGINT AS "VideoHistoryId";
        RETURN;
    END IF;

    -- Check VideoHistory
    IF NOT EXISTS (
        SELECT 1 FROM "tb_VideoHistory"
        WHERE "meetingId" = p_meeting_id
        LIMIT 1
    ) THEN
        v_errmsg := 'videomeet details not available';
        OPEN p_result FOR SELECT v_errcode AS errcode, v_errmsg AS errmsg, NULL::BIGINT AS "VideoHistoryId";
        RETURN;
    END IF;

    -- Get VideoHistoryId
    SELECT "VideoHistoryId"
    INTO v_VideoHistoryId
    FROM "tb_VideoHistory"
    WHERE "meetingId" = p_meeting_id
    LIMIT 1;

    -- Get VideoDurationHistoryId
    SELECT "VideoDurationHistoryId"
    INTO v_VideoDurationHistoryId
    FROM "tb_VideoDurationHistory"
    WHERE "VideoHistoryId" = v_VideoHistoryId
    AND "AgentId" = v_AgentId
    LIMIT 1;

    IF v_VideoDurationHistoryId IS NULL THEN
        v_errmsg := 'Given Agent did not attend this videomeet';
        OPEN p_result FOR SELECT v_errcode AS errcode, v_errmsg AS errmsg, v_VideoHistoryId;
        RETURN;
    END IF;

    -- Insert or Update Message
    IF NOT EXISTS (
        SELECT 1
        FROM "tb_VideoMessage"
        WHERE "VideoHistoryId" = v_VideoHistoryId
    ) THEN

        INSERT INTO "tb_VideoMessage"
        ("VideoHistoryId","Message","createdAt","VideoDurationHistoryId")
        VALUES
        (v_VideoHistoryId,p_Message,CURRENT_TIMESTAMP,v_VideoDurationHistoryId);

    ELSIF EXISTS (
        SELECT 1
        FROM "tb_VideoMessage"
        WHERE "VideoHistoryId" = v_VideoHistoryId
        AND "VideoDurationHistoryId" = v_VideoDurationHistoryId
    ) THEN

        UPDATE "tb_VideoMessage"
        SET
            "Message" = CASE
                        WHEN "Message" IS NULL THEN p_Message
                        ELSE "Message" || jsonb_build_array(p_Message)
                        END,
            "updatedAt" = CURRENT_TIMESTAMP
        WHERE "VideoHistoryId" = v_VideoHistoryId
        AND "VideoDurationHistoryId" = v_VideoDurationHistoryId;

    ELSE

        SELECT "Message"
        INTO v_prev_Message
        FROM "tb_VideoMessage"
        WHERE "VideoHistoryId" = v_VideoHistoryId
        ORDER BY COALESCE("updatedAt","createdAt") DESC
        LIMIT 1;

        v_merged_msg := v_prev_Message || jsonb_build_array(p_Message);

        INSERT INTO "tb_VideoMessage"
        ("VideoHistoryId","Message","createdAt","VideoDurationHistoryId")
        VALUES
        (v_VideoHistoryId,v_merged_msg,CURRENT_TIMESTAMP,v_VideoDurationHistoryId);

    END IF;

    GET DIAGNOSTICS v_rowcount = ROW_COUNT;

    IF v_rowcount > 0 THEN
        v_errcode := 0;
        v_errmsg := 'Message inserted successfully';
    END IF;

    OPEN p_result FOR
    SELECT v_errcode AS errcode,
           v_errmsg AS errmsg,
           v_VideoHistoryId AS "VideoHistoryId";

END;
$$;


ALTER PROCEDURE public.crm_videomeet_create_message_details(IN p_domainid integer, IN p_meeting_id character varying, IN p_message jsonb, IN p_agent_id integer, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_wc_ai_chat_current_campaign_report(integer, date, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_wc_ai_chat_current_campaign_report(IN p_domainid integer, IN p_createdorscheduledate date, INOUT p_result refcursor DEFAULT 'result_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$

BEGIN
    OPEN p_result FOR
    WITH cte1 AS (
        SELECT e."domainId", e."campaignId" AS "CampaignId", e.campaignchatid AS "CampaignChatId", e."Name" AS "CampaignName", 'Email' AS "Channel", e.createdate AS "createdOn"
        FROM ec_email_campaign_master_dtl e WHERE e."domainId" = p_DomainId AND DATE(e."emailDeliveryStarttime") <= p_CreatedOrScheduleDate AND DATE(e."emailDeliveryEndtime") >= p_CreatedOrScheduleDate AND e."isIndivCamp" != 2
        UNION ALL
        SELECT m."domainId", m.id AS "CampaignId", m.campaignchatid AS "CampaignChatId", m.name AS "CampaignName", 'SMS' AS "Channel", m."createdDate" AS "createdOn"
        FROM sc_campaign_master_dtl m JOIN sc_campaign_master_schedule_dtl s ON m.id = s."campaignId" AND m."domainId" = s."domainId"
        WHERE m."domainId" = p_DomainId AND (DATE(m."createdDate") = p_CreatedOrScheduleDate OR (m."triggerType" = 0 AND DATE(s."schedulefromDate") = p_CreatedOrScheduleDate) OR (m."triggerType" = 1 AND s."scheduletoDate" IS NOT NULL AND DATE(s."schedulefromDate") <= p_CreatedOrScheduleDate AND DATE(s."scheduletoDate") >= p_CreatedOrScheduleDate) OR (m."triggerType" = 1 AND s."scheduletoDate" IS NULL AND DATE(s."schedulefromDate") = p_CreatedOrScheduleDate))
    )
    SELECT c."CampaignChatId", jsonb_agg(jsonb_build_object('CampaignId', c."CampaignId", 'ChannelName', c."Channel", 'CampaignName', c."CampaignName")) AS campaign_Details, MIN(c."createdOn") AS mincreated
    FROM cte1 c GROUP BY c."CampaignChatId" ORDER BY mincreated DESC;
END;
$$;


ALTER PROCEDURE public.crm_wc_ai_chat_current_campaign_report(IN p_domainid integer, IN p_createdorscheduledate date, INOUT p_result refcursor) OWNER TO postgres;

--
-- Name: crm_wc_get_user_dlt_by_location_or_lifecycle(integer, character varying, character varying, character varying, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.crm_wc_get_user_dlt_by_location_or_lifecycle(IN p_domainid integer, IN p_lifecyclestage character varying, IN p_industrytype character varying, IN p_companyname character varying, INOUT p_cursor refcursor DEFAULT 'p_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
BEGIN

    OPEN p_cursor FOR
        SELECT "customer_id","domain_id","first_name","last_name","primary_phone_number","primary_email","address","industry_type","source_type"
			,"company_name","company_size","user_id","demo_graphics","firmo_graphics","behaviors","is_active","lifeCycleStage","isUnSub_Email","created_at","updated_at"
			,"additional_info","is_mql","is_sql","sales_agent_id"
        FROM tb_customer_contact
        WHERE domain_id = p_domainId
          AND (COALESCE(p_lifeCycleStage, '') = '' OR "lifeCycleStage" = p_lifeCycleStage)
          AND (COALESCE(p_industryType, '') = '' OR industry_type = p_industryType)
          AND (COALESCE(p_companyName, '') = '' OR company_name = p_companyName);

END;
$$;


ALTER PROCEDURE public.crm_wc_get_user_dlt_by_location_or_lifecycle(IN p_domainid integer, IN p_lifecyclestage character varying, IN p_industrytype character varying, IN p_companyname character varying, INOUT p_cursor refcursor) OWNER TO postgres;

--
-- Name: sp_insert_notification(integer, character varying, character varying, character varying, character varying, refcursor); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.sp_insert_notification(IN p_domain_id integer, IN p_notification_type character varying, IN p_title character varying, IN p_message character varying, IN p_role_id character varying, INOUT p_cursor refcursor DEFAULT 'p_cursor'::refcursor)
    LANGUAGE plpgsql
    AS $$
DECLARE 
    v_notification_id BIGINT;
    v_errcode   SMALLINT := -1;
    v_errmsg    VARCHAR(250) := 'Notification insert failed';
BEGIN

    -- Create temporary role table
    DROP TABLE IF EXISTS temp_role_id;
    CREATE TEMP TABLE temp_role_id(role_id SMALLINT);

    INSERT INTO temp_role_id(role_id)
    SELECT unnest(string_to_array(p_role_id, ',')::SMALLINT[]);

    -- Get users for those roles
    DROP TABLE IF EXISTS temp_user_details;
    CREATE TEMP TABLE temp_user_details AS
    SELECT U."UserID", U.roleid
    FROM "tb_User" U
    JOIN temp_role_id TR ON U.roleid = TR.role_id
    WHERE U."domainId" = p_domain_id;

    -- Insert notification
    INSERT INTO notification_master (domain_id,notification_type,title,message)
    VALUES (p_domain_id,p_notification_type,p_title,p_message)
    RETURNING notification_id INTO v_notification_id;

    -- Insert notification_agent records
    INSERT INTO notification_agent (notification_id,agent_id,role_id,is_read)
    SELECT v_notification_id,TM."UserID",TM.roleid,0
    FROM temp_user_details TM;

    v_errcode := 0;
    v_errmsg  := 'Notification inserted successfully';

    -- Return status
    OPEN p_cursor FOR
    SELECT v_errcode AS errcode,
           v_errmsg  AS errmsg,
           v_notification_id AS notification_id;

EXCEPTION WHEN OTHERS THEN
    OPEN p_cursor FOR
    SELECT -1 AS errcode,
           SQLERRM AS errmsg,
           NULL::BIGINT AS notification_id;
END;
$$;


ALTER PROCEDURE public.sp_insert_notification(IN p_domain_id integer, IN p_notification_type character varying, IN p_title character varying, IN p_message character varying, IN p_role_id character varying, INOUT p_cursor refcursor) OWNER TO postgres;

--
-- Name: split(text, integer); Type: PROCEDURE; Schema: public; Owner: postgres
--

CREATE PROCEDURE public.split(IN p_string text, IN p_delimiter integer)
    LANGUAGE plpgsql
    AS $$
BEGIN

    -- Drop temp table if exists
    DROP TABLE IF EXISTS tt_split_temptable;

    -- Create temporary table
    CREATE TEMP TABLE tt_split_temptable (
        items TEXT
    ) ON COMMIT DROP;

    -- If string is NULL or empty, exit
    IF p_string IS NULL OR LENGTH(p_string) = 0 THEN
        RETURN;
    END IF;

    -- Insert split values
    INSERT INTO tt_split_temptable(items)
    SELECT unnest(string_to_array(p_string, p_delimiter));

END;
$$;


ALTER PROCEDURE public.split(IN p_string text, IN p_delimiter integer) OWNER TO postgres;

SET default_tablespace = '';

SET default_table_access_method = heap;

--
-- Name: TypeCode; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public."TypeCode" (
    "TypeCodeID" integer NOT NULL,
    "TypeCode" character varying(50) DEFAULT NULL::character varying,
    "TypeCodeName" character varying(250) DEFAULT NULL::character varying,
    "TypeGroupID" integer,
    "IsActive" smallint,
    "SortOrder" integer,
    "CreatedAt" timestamp without time zone NOT NULL,
    "UpdatedAt" timestamp without time zone NOT NULL
);


ALTER TABLE public."TypeCode" OWNER TO postgres;

--
-- Name: agent_status; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.agent_status (
    aid integer NOT NULL,
    "statusName" character varying(100) DEFAULT NULL::character varying,
    type character varying(255) DEFAULT NULL::character varying,
    description character varying(250) DEFAULT NULL::character varying,
    channels character varying(100) DEFAULT NULL::character varying,
    "createdAt" timestamp without time zone,
    "updatedAt" timestamp without time zone,
    "domainId" integer,
    status integer,
    "isDeleted" smallint DEFAULT 0,
    "isVoice" integer,
    "isVoiceCallTransfer" integer,
    "isChat" integer,
    "isChatTransfer" integer,
    "isEmail" integer,
    "isEmailTransfer" integer,
    "colorCode" character varying(50) DEFAULT NULL::character varying
);


ALTER TABLE public.agent_status OWNER TO postgres;

--
-- Name: agent_status_aid_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.agent_status_aid_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.agent_status_aid_seq OWNER TO postgres;

--
-- Name: agent_status_aid_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.agent_status_aid_seq OWNED BY public.agent_status.aid;


--
-- Name: callFlow; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public."callFlow" (
    "callFlowId" integer NOT NULL,
    "templateName" character varying(255) DEFAULT NULL::character varying,
    nodes text,
    edges text,
    version character varying(55) DEFAULT NULL::character varying,
    type character varying(100) DEFAULT NULL::character varying,
    "createDate" bigint,
    "lastModifiedDate" bigint,
    status integer,
    cfid character varying(255) DEFAULT NULL::character varying,
    "companyId" integer,
    "domainId" integer,
    description character varying(255) DEFAULT NULL::character varying,
    "createdAt" timestamp without time zone,
    "updatedAt" timestamp without time zone,
    "publishData" text
);


ALTER TABLE public."callFlow" OWNER TO postgres;

--
-- Name: callFlow_callFlowId_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public."callFlow_callFlowId_seq"
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public."callFlow_callFlowId_seq" OWNER TO postgres;

--
-- Name: callFlow_callFlowId_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public."callFlow_callFlowId_seq" OWNED BY public."callFlow"."callFlowId";


--
-- Name: callMap; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public."callMap" (
    cid integer NOT NULL,
    "channelName" character varying(255) DEFAULT NULL::character varying,
    "sourceId" character varying(255) DEFAULT NULL::character varying,
    "callFlowId" character varying(255) DEFAULT NULL::character varying,
    "createdAt" timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    "updatedAt" timestamp without time zone,
    "domainId" integer,
    description character varying(200) DEFAULT NULL::character varying,
    "codeSnippet" text
);


ALTER TABLE public."callMap" OWNER TO postgres;

--
-- Name: callMap_cid_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public."callMap_cid_seq"
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public."callMap_cid_seq" OWNER TO postgres;

--
-- Name: callMap_cid_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public."callMap_cid_seq" OWNED BY public."callMap".cid;


--
-- Name: call_configuration; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.call_configuration (
    "contactCenterNumber" bigint NOT NULL,
    name text,
    "domainId" integer,
    "Language" text,
    "isBot" smallint,
    "callFlowName" text,
    "callFlowId" integer,
    "greetingMessage" text,
    threshold integer,
    status smallint,
    "createdDate" timestamp without time zone,
    "updatedDate" timestamp without time zone,
    "virtualAgent" smallint,
    "assistPriority" smallint,
    "isMultilingualStatus" smallint DEFAULT 0,
    "humanAgent" smallint DEFAULT 0
);


ALTER TABLE public.call_configuration OWNER TO postgres;

--
-- Name: callsessionstartendlog; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.callsessionstartendlog (
    id integer NOT NULL,
    "domainId" integer,
    "sessionId" character varying(250) DEFAULT NULL::character varying,
    starttime timestamp without time zone,
    endtime timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    errcode integer,
    errmsg character varying(155) DEFAULT NULL::character varying
);


ALTER TABLE public.callsessionstartendlog OWNER TO postgres;

--
-- Name: callsessionstartendlog_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.callsessionstartendlog_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.callsessionstartendlog_id_seq OWNER TO postgres;

--
-- Name: callsessionstartendlog_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.callsessionstartendlog_id_seq OWNED BY public.callsessionstartendlog.id;


--
-- Name: ccaas_settings_voice_video_recording_dtl; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.ccaas_settings_voice_video_recording_dtl (
    rid integer NOT NULL,
    "domainId" integer,
    "isVoice" smallint DEFAULT 0,
    "isInbound" smallint DEFAULT 0,
    "isOutbound" smallint DEFAULT 0,
    "isVoiceConsentNotice" smallint DEFAULT 0,
    "isVoiceTextToSpeech" smallint DEFAULT 0,
    "voiceText" text,
    "isVoiceSelectPrompt" smallint DEFAULT 0,
    "voicePrompt" character varying(250) DEFAULT NULL::character varying,
    "voiceUrl" text,
    "isVoiceAgentAccess" smallint DEFAULT 0,
    "isVideo" smallint DEFAULT 0,
    "isVideoConsentNotice" smallint DEFAULT 0,
    "isVideoTextToSpeech" smallint DEFAULT 0,
    "videoText" text,
    "isVideoSelectPrompt" smallint DEFAULT 0,
    "videoPrompt" character varying(250) DEFAULT NULL::character varying,
    "videoUrl" text,
    "isVideoAgentAccess" smallint DEFAULT 0,
    "createdAt" timestamp without time zone,
    "updatedAt" timestamp without time zone
);


ALTER TABLE public.ccaas_settings_voice_video_recording_dtl OWNER TO postgres;

--
-- Name: ccaas_settings_voice_video_recording_dtl_rid_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.ccaas_settings_voice_video_recording_dtl_rid_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.ccaas_settings_voice_video_recording_dtl_rid_seq OWNER TO postgres;

--
-- Name: ccaas_settings_voice_video_recording_dtl_rid_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.ccaas_settings_voice_video_recording_dtl_rid_seq OWNED BY public.ccaas_settings_voice_video_recording_dtl.rid;


--
-- Name: chatbotpaymentintegration; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.chatbotpaymentintegration (
    tokenid integer NOT NULL,
    companyid integer,
    domainid integer,
    paymenttype character varying(50),
    secretjson jsonb,
    createdat timestamp without time zone,
    updatedat timestamp without time zone
);


ALTER TABLE public.chatbotpaymentintegration OWNER TO postgres;

--
-- Name: chatbotpaymentintegration_tokenid_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

ALTER TABLE public.chatbotpaymentintegration ALTER COLUMN tokenid ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME public.chatbotpaymentintegration_tokenid_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: custom_hours; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.custom_hours (
    cid integer NOT NULL,
    name character varying(255) DEFAULT NULL::character varying,
    description character varying(255) DEFAULT NULL::character varying,
    hours text,
    "createdAt" timestamp without time zone,
    "updatedAt" timestamp without time zone,
    "domainId" integer,
    "timeZone" character varying(100) DEFAULT NULL::character varying,
    default_hours character varying(100) DEFAULT NULL::character varying,
    "timeZoneValue" character varying(40) DEFAULT NULL::character varying,
    type character varying(50) DEFAULT NULL::character varying
);


ALTER TABLE public.custom_hours OWNER TO postgres;

--
-- Name: custom_hours_cid_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.custom_hours_cid_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.custom_hours_cid_seq OWNER TO postgres;

--
-- Name: custom_hours_cid_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.custom_hours_cid_seq OWNED BY public.custom_hours.cid;


--
-- Name: customer_contact; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.customer_contact (
    coid integer NOT NULL,
    "firstName" character varying(250) DEFAULT NULL::character varying,
    "lastName" character varying(250) DEFAULT NULL::character varying,
    "primaryPhoneNumber" character varying(20) DEFAULT NULL::character varying,
    "phoneNumber" character varying(1000) DEFAULT NULL::character varying,
    "primaryEmail" character varying(150) DEFAULT NULL::character varying,
    email character varying(1500) DEFAULT '[]'::character varying,
    "accountNumber" character varying(100) DEFAULT NULL::character varying,
    tags character varying(750) DEFAULT '[]'::character varying,
    location character varying(100) DEFAULT NULL::character varying,
    address character varying(225) DEFAULT NULL::character varying,
    country character varying(100) DEFAULT NULL::character varying,
    state character varying(100) DEFAULT NULL::character varying,
    city character varying(100) DEFAULT NULL::character varying,
    "postalCode" character varying(100) DEFAULT NULL::character varying,
    facebook character varying(100) DEFAULT NULL::character varying,
    instagram character varying(100) DEFAULT NULL::character varying,
    whatsapp character varying(100) DEFAULT NULL::character varying,
    twitter character varying(100) DEFAULT NULL::character varying,
    "createdAt" timestamp without time zone,
    "updatedAt" timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    "voiceBiometric" character varying(100) DEFAULT NULL::character varying,
    "domainId" integer,
    "mergeCustomer" integer DEFAULT 0,
    "blockCustomer" integer DEFAULT 0,
    "doNotDisturb" integer DEFAULT 0,
    "blockCustomerReq" integer DEFAULT 0,
    "doNotDisturbReq" integer DEFAULT 0,
    type character varying(40) DEFAULT NULL::character varying,
    "currentCustomerNumber" character varying(20) DEFAULT NULL::character varying,
    "customerType" character varying(255) DEFAULT NULL::character varying,
    "Timezone" character varying(100) DEFAULT NULL::character varying,
    "blockedBy" integer,
    "doNotDisturbBy" integer,
    "blockedReqBy" integer,
    "doNotDisturbReqBy" integer,
    "Reason" character varying(225) DEFAULT NULL::character varying,
    "cookiesId" character varying(150) DEFAULT NULL::character varying,
    "Source" character varying(250) DEFAULT NULL::character varying,
    attachments json,
    "CustomerUniqueId" character varying(150) DEFAULT NULL::character varying
);


ALTER TABLE public.customer_contact OWNER TO postgres;

--
-- Name: customer_contact_coid_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.customer_contact_coid_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.customer_contact_coid_seq OWNER TO postgres;

--
-- Name: customer_contact_coid_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.customer_contact_coid_seq OWNED BY public.customer_contact.coid;


--
-- Name: did_purchase; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.did_purchase (
    pid integer NOT NULL,
    user_id integer,
    type character varying(255) DEFAULT NULL::character varying,
    city character varying(255) DEFAULT NULL::character varying,
    your_number character varying(255) DEFAULT NULL::character varying,
    "createdAt" timestamp without time zone,
    "updatedAt" timestamp without time zone,
    "domainId" integer,
    "callFlowId" character varying(100) DEFAULT NULL::character varying,
    recording integer,
    "isCall" smallint,
    "isSms" smallint,
    "queueName" character varying(40) DEFAULT NULL::character varying,
    "aliasName" character varying(40) DEFAULT NULL::character varying,
    "outboundSms" smallint,
    "smsThreshold" integer,
    status integer,
    "callFlowName" character varying(100) DEFAULT NULL::character varying,
    "defaultUpdated" integer DEFAULT 0,
    "supervisorName" character varying(200) DEFAULT NULL::character varying,
    "isCallIn" smallint,
    "isCallOut" smallint,
    "isSmsOut" smallint,
    "isSmsIn" smallint,
    "queueId" integer,
    country character varying(100) DEFAULT NULL::character varying,
    "isBot" smallint DEFAULT 0,
    "isBotPurchased" smallint DEFAULT 0,
    "knowledgeBasedId" character varying(150) DEFAULT NULL::character varying,
    "autoAssignAgent" smallint
);


ALTER TABLE public.did_purchase OWNER TO postgres;

--
-- Name: did_purchase_pid_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.did_purchase_pid_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.did_purchase_pid_seq OWNER TO postgres;

--
-- Name: did_purchase_pid_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.did_purchase_pid_seq OWNED BY public.did_purchase.pid;


--
-- Name: disposition_status; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.disposition_status (
    did integer NOT NULL,
    "dispositionName" character varying(100) DEFAULT NULL::character varying,
    type character varying(255) DEFAULT NULL::character varying,
    description character varying(250) DEFAULT NULL::character varying,
    status integer,
    "createdAt" timestamp without time zone,
    "updatedAt" timestamp without time zone,
    "domainId" integer,
    "colorCode" character varying(50) DEFAULT NULL::character varying,
    "accessType" integer DEFAULT 1
);


ALTER TABLE public.disposition_status OWNER TO postgres;

--
-- Name: disposition_status_did_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.disposition_status_did_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.disposition_status_did_seq OWNER TO postgres;

--
-- Name: disposition_status_did_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.disposition_status_did_seq OWNED BY public.disposition_status.did;


--
-- Name: ec_campaign_contacts_dtl; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.ec_campaign_contacts_dtl (
    id integer NOT NULL,
    createdate timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    "domainId" integer,
    "campaignId" integer,
    "contactEmail" character varying(100) DEFAULT NULL::character varying,
    "isSent" smallint DEFAULT 0,
    "Template" character varying(2) DEFAULT NULL::character varying,
    "templateType" character(1) DEFAULT NULL::bpchar,
    "isStop" smallint DEFAULT 0 NOT NULL,
    "deliveryTime" timestamp without time zone,
    "stopTime" timestamp without time zone,
    updateddate timestamp without time zone
);


ALTER TABLE public.ec_campaign_contacts_dtl OWNER TO postgres;

--
-- Name: ec_campaign_contacts_dtl_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.ec_campaign_contacts_dtl_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.ec_campaign_contacts_dtl_id_seq OWNER TO postgres;

--
-- Name: ec_campaign_contacts_dtl_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.ec_campaign_contacts_dtl_id_seq OWNED BY public.ec_campaign_contacts_dtl.id;


--
-- Name: ec_email_campaign_AB_testing_dtl; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public."ec_email_campaign_AB_testing_dtl" (
    id integer NOT NULL,
    createdate timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    "campaignId" integer,
    testtype integer,
    variant jsonb,
    "variantDistribution" jsonb,
    "winningCriteria" integer,
    "AB_testWindow" integer,
    "AB_testWindowtype" integer,
    "examineTime" timestamp without time zone
);


ALTER TABLE public."ec_email_campaign_AB_testing_dtl" OWNER TO postgres;

--
-- Name: ec_email_campaign_AB_testing_dtl_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public."ec_email_campaign_AB_testing_dtl_id_seq"
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public."ec_email_campaign_AB_testing_dtl_id_seq" OWNER TO postgres;

--
-- Name: ec_email_campaign_AB_testing_dtl_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public."ec_email_campaign_AB_testing_dtl_id_seq" OWNED BY public."ec_email_campaign_AB_testing_dtl".id;


--
-- Name: ec_email_campaign_master_dtl; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.ec_email_campaign_master_dtl (
    "campaignId" integer NOT NULL,
    campaignchatid character varying(300) NOT NULL,
    createdate timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    "domainId" integer NOT NULL,
    "Name" text,
    "Goal" text,
    "Type" character varying(100) DEFAULT NULL::character varying,
    "TargetAud" text DEFAULT NULL::character varying,
    "Objective" text,
    "isIndivCamp" smallint DEFAULT 0 NOT NULL,
    "AB_testing" smallint DEFAULT 0,
    "AItemplateA" jsonb,
    "AItemplateB" jsonb,
    "Contactslist" jsonb,
    "emailDelivery" integer,
    "emailDeliveryAt" date,
    "emailDeliveryStarttime" timestamp without time zone NOT NULL,
    "emailDeliveryEndtime" timestamp without time zone NOT NULL,
    "createdBy" character varying(100) DEFAULT NULL::character varying,
    status character varying(50) DEFAULT NULL::character varying
);


ALTER TABLE public.ec_email_campaign_master_dtl OWNER TO postgres;

--
-- Name: ec_email_campaign_master_dtl_campaignId_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public."ec_email_campaign_master_dtl_campaignId_seq"
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public."ec_email_campaign_master_dtl_campaignId_seq" OWNER TO postgres;

--
-- Name: ec_email_campaign_master_dtl_campaignId_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public."ec_email_campaign_master_dtl_campaignId_seq" OWNED BY public.ec_email_campaign_master_dtl."campaignId";


--
-- Name: ec_email_campaign_master_dtl_exclude_email; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.ec_email_campaign_master_dtl_exclude_email (
    id integer NOT NULL,
    "fk_campaignId" integer NOT NULL,
    campaignchatid character varying(300) NOT NULL,
    createdate timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    "domainId" integer NOT NULL,
    contactslist jsonb,
    "createdBy" character varying(100) DEFAULT NULL::character varying,
    status character varying(50) DEFAULT NULL::character varying
);


ALTER TABLE public.ec_email_campaign_master_dtl_exclude_email OWNER TO postgres;

--
-- Name: ec_email_campaign_master_dtl_exclude_email_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.ec_email_campaign_master_dtl_exclude_email_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.ec_email_campaign_master_dtl_exclude_email_id_seq OWNER TO postgres;

--
-- Name: ec_email_campaign_master_dtl_exclude_email_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.ec_email_campaign_master_dtl_exclude_email_id_seq OWNED BY public.ec_email_campaign_master_dtl_exclude_email.id;


--
-- Name: ec_email_failed_contact_dtl; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.ec_email_failed_contact_dtl (
    id integer NOT NULL,
    domainid integer NOT NULL,
    campaignid integer NOT NULL,
    campaignchatid character varying(300) NOT NULL,
    userid integer,
    contactemail character varying(100) NOT NULL,
    createdon timestamp without time zone,
    updatedon timestamp without time zone DEFAULT CURRENT_TIMESTAMP
);


ALTER TABLE public.ec_email_failed_contact_dtl OWNER TO postgres;

--
-- Name: ec_email_failed_contact_dtl_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

ALTER TABLE public.ec_email_failed_contact_dtl ALTER COLUMN id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME public.ec_email_failed_contact_dtl_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: email_config; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.email_config (
    "EmailConfigId" integer NOT NULL,
    "emailAddress" character varying(200) DEFAULT NULL::character varying,
    "aliasName" character varying(50) DEFAULT NULL::character varying,
    "emailThreshold" integer,
    "isEmail" integer,
    "domainId" integer,
    "createdAt" timestamp without time zone,
    "updatedAt" timestamp without time zone,
    "primaryEmail" integer,
    "supervisorName" character varying(50) DEFAULT NULL::character varying,
    "isBot" smallint DEFAULT 0,
    "isBotPurchased" smallint DEFAULT 0,
    "knowledgeBasedId" character varying(150) DEFAULT NULL::character varying,
    "autoAssignAgent" smallint,
    product_id integer DEFAULT 1,
    is_smtp smallint DEFAULT 0,
    smtp_json jsonb,
    emailtype integer DEFAULT 1,
    "serverType" smallint DEFAULT 0,
    "isRepliedEmail" smallint DEFAULT 0,
    "repliedEmail" character varying(250) DEFAULT NULL::character varying,
    "domainName" character varying(250) DEFAULT NULL::character varying,
    "forwardEmail" character varying(250) DEFAULT NULL::character varying,
    host character varying(250) DEFAULT NULL::character varying,
    password character varying(50) DEFAULT NULL::character varying
);


ALTER TABLE public.email_config OWNER TO postgres;

--
-- Name: email_config_EmailConfigId_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public."email_config_EmailConfigId_seq"
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public."email_config_EmailConfigId_seq" OWNER TO postgres;

--
-- Name: email_config_EmailConfigId_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public."email_config_EmailConfigId_seq" OWNED BY public.email_config."EmailConfigId";


--
-- Name: email_tb_indiv_admin_action_dtl; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.email_tb_indiv_admin_action_dtl (
    id integer NOT NULL,
    domainid integer NOT NULL,
    campaignchatid character varying(300) NOT NULL,
    userid integer,
    actionid integer NOT NULL,
    channeltype character varying(20) DEFAULT 'Email'::character varying,
    createdon timestamp without time zone DEFAULT CURRENT_TIMESTAMP
);


ALTER TABLE public.email_tb_indiv_admin_action_dtl OWNER TO postgres;

--
-- Name: email_tb_indiv_admin_action_dtl_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

ALTER TABLE public.email_tb_indiv_admin_action_dtl ALTER COLUMN id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME public.email_tb_indiv_admin_action_dtl_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: email_tb_indiv_user_click_open_cnt_dtl; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.email_tb_indiv_user_click_open_cnt_dtl (
    id integer NOT NULL,
    domainid integer NOT NULL,
    campaignchatid character varying(300) NOT NULL,
    userid integer NOT NULL,
    actionid integer NOT NULL,
    channeltype character varying(20) DEFAULT 'Email'::character varying,
    createdon timestamp without time zone DEFAULT CURRENT_TIMESTAMP
);


ALTER TABLE public.email_tb_indiv_user_click_open_cnt_dtl OWNER TO postgres;

--
-- Name: email_tb_indiv_user_click_open_cnt_dtl_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

ALTER TABLE public.email_tb_indiv_user_click_open_cnt_dtl ALTER COLUMN id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME public.email_tb_indiv_user_click_open_cnt_dtl_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: notification_agent; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.notification_agent (
    notification_id bigint NOT NULL,
    agent_id integer NOT NULL,
    role_id smallint,
    is_read smallint DEFAULT 0,
    read_at timestamp without time zone
);


ALTER TABLE public.notification_agent OWNER TO postgres;

--
-- Name: notification_master; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.notification_master (
    notification_id bigint NOT NULL,
    domain_id integer NOT NULL,
    notification_type character varying(50) NOT NULL,
    title character varying(255),
    message character varying(2000),
    created_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    ref_id character varying
);


ALTER TABLE public.notification_master OWNER TO postgres;

--
-- Name: notification_master_notification_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.notification_master_notification_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.notification_master_notification_id_seq OWNER TO postgres;

--
-- Name: notification_master_notification_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.notification_master_notification_id_seq OWNED BY public.notification_master.notification_id;


--
-- Name: prompt_library; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.prompt_library (
    uuid integer NOT NULL,
    fieldname character varying(100) DEFAULT NULL::character varying,
    encoding character varying(100) DEFAULT NULL::character varying,
    mimetype character varying(100) DEFAULT NULL::character varying,
    filename character varying(150) DEFAULT NULL::character varying,
    path text,
    size integer,
    type character varying(100) DEFAULT NULL::character varying,
    "promptType" integer,
    url text,
    name character varying(100) DEFAULT NULL::character varying,
    description character varying(255) DEFAULT NULL::character varying,
    "domainId" integer,
    "companyId" integer,
    message character varying(3000) DEFAULT NULL::character varying,
    "createdAt" timestamp without time zone,
    "updateddAt" timestamp without time zone,
    "promptStatus" character varying(25) DEFAULT NULL::character varying,
    "queueId" integer
);


ALTER TABLE public.prompt_library OWNER TO postgres;

--
-- Name: prompt_library_uuid_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.prompt_library_uuid_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.prompt_library_uuid_seq OWNER TO postgres;

--
-- Name: prompt_library_uuid_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.prompt_library_uuid_seq OWNED BY public.prompt_library.uuid;


--
-- Name: queue; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.queue (
    qid integer NOT NULL,
    name character varying(255) DEFAULT NULL::character varying,
    description character varying(1000) DEFAULT NULL::character varying,
    "domainId" integer,
    "createdAt" timestamp without time zone,
    "updatedAt" timestamp without time zone,
    "settingAudio" text,
    announcement text,
    "maximumCallers" text,
    "maximumWaitTime" text,
    "OutboundCallerId" character varying(100) DEFAULT NULL::character varying,
    "outBoundNumber" character varying(20) DEFAULT NULL::character varying,
    "maxCallHandlingTimeMM" integer,
    "maxCallHandlingTimeSS" integer,
    "callWrapUpTime" integer,
    "outBoundCallStatus" integer,
    "hoursOfOperatioId" integer,
    hours_of_operation text,
    "outOfBusinessHours" text,
    status integer,
    type character varying(55) DEFAULT NULL::character varying
);


ALTER TABLE public.queue OWNER TO postgres;

--
-- Name: queueWeighTime; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public."queueWeighTime" (
    "qwtId" integer NOT NULL,
    qid integer NOT NULL,
    "queueName" character varying(255) DEFAULT NULL::character varying,
    "queueType" character varying(255) DEFAULT NULL::character varying,
    "domainId" integer NOT NULL,
    "queueList" text,
    "createdAt" timestamp without time zone,
    "updatedAt" timestamp without time zone
);


ALTER TABLE public."queueWeighTime" OWNER TO postgres;

--
-- Name: queueWeighTime_qwtId_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public."queueWeighTime_qwtId_seq"
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public."queueWeighTime_qwtId_seq" OWNER TO postgres;

--
-- Name: queueWeighTime_qwtId_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public."queueWeighTime_qwtId_seq" OWNED BY public."queueWeighTime"."qwtId";


--
-- Name: queue_qid_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.queue_qid_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.queue_qid_seq OWNER TO postgres;

--
-- Name: queue_qid_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.queue_qid_seq OWNED BY public.queue.qid;


--
-- Name: routing_profile; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.routing_profile (
    rid integer NOT NULL,
    name character varying(255),
    description character varying(255),
    default_outbound_queue character varying(255),
    no_of_linked_queues integer,
    status integer,
    "createdAt" timestamp without time zone,
    "updatedAt" timestamp without time zone,
    "domainId" integer,
    "routingMethod" character varying(100),
    "concurrentChat" integer DEFAULT 1,
    type character varying(50)
);


ALTER TABLE public.routing_profile OWNER TO postgres;

--
-- Name: routing_profile_rid_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

ALTER TABLE public.routing_profile ALTER COLUMN rid ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME public.routing_profile_rid_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: routingqueueprofilemapping; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.routingqueueprofilemapping (
    rqid integer NOT NULL,
    rqmid integer,
    qid integer,
    calls integer,
    chat integer,
    emails integer,
    sms integer,
    social_apps integer,
    priority integer,
    delay_in_seconds integer,
    "createdAt" timestamp without time zone,
    "updatedAt" timestamp without time zone,
    "domainId" integer,
    voice integer
);


ALTER TABLE public.routingqueueprofilemapping OWNER TO postgres;

--
-- Name: routingqueueprofilemapping_rqid_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.routingqueueprofilemapping_rqid_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.routingqueueprofilemapping_rqid_seq OWNER TO postgres;

--
-- Name: routingqueueprofilemapping_rqid_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.routingqueueprofilemapping_rqid_seq OWNED BY public.routingqueueprofilemapping.rqid;


--
-- Name: sc_campaign_master_dtl; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.sc_campaign_master_dtl (
    id integer NOT NULL,
    campaignchatid character varying(300) NOT NULL,
    "createdDate" timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    "domainId" integer NOT NULL,
    name character varying(250) DEFAULT NULL::character varying,
    "campaignGoal" character varying(250) DEFAULT NULL::character varying,
    "campaignObjective" character varying(400) DEFAULT NULL::character varying,
    "websiteURL" character varying(250) DEFAULT NULL::character varying,
    "targetAudienceType" integer,
    "contactDtls" jsonb,
    "senderName" character varying(250) DEFAULT NULL::character varying,
    "templatePrompt" text,
    "triggerType" integer,
    status character varying(100) DEFAULT NULL::character varying,
    updateddate timestamp without time zone
);


ALTER TABLE public.sc_campaign_master_dtl OWNER TO postgres;

--
-- Name: sc_campaign_master_dtl_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.sc_campaign_master_dtl_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.sc_campaign_master_dtl_id_seq OWNER TO postgres;

--
-- Name: sc_campaign_master_dtl_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.sc_campaign_master_dtl_id_seq OWNED BY public.sc_campaign_master_dtl.id;


--
-- Name: sc_campaign_master_schedule_dtl; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.sc_campaign_master_schedule_dtl (
    id integer NOT NULL,
    "domainId" integer,
    "campaignId" integer,
    "schedulefromDate" timestamp without time zone NOT NULL,
    "scheduletoDate" timestamp without time zone NOT NULL,
    "sentStatus" smallint DEFAULT 0,
    "updatedDate" timestamp without time zone
);


ALTER TABLE public.sc_campaign_master_schedule_dtl OWNER TO postgres;

--
-- Name: sc_campaign_master_schedule_dtl_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.sc_campaign_master_schedule_dtl_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.sc_campaign_master_schedule_dtl_id_seq OWNER TO postgres;

--
-- Name: sc_campaign_master_schedule_dtl_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.sc_campaign_master_schedule_dtl_id_seq OWNED BY public.sc_campaign_master_schedule_dtl.id;


--
-- Name: sc_sms_sent_status_details; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.sc_sms_sent_status_details (
    id integer NOT NULL,
    domainid integer NOT NULL,
    campaignchatid character varying(300) NOT NULL,
    frommobileno character varying(20) NOT NULL,
    tomobileno character varying(20) NOT NULL,
    status character varying(50) NOT NULL,
    createdon timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    updatedon timestamp without time zone,
    messageid character varying(100),
    sentat timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    description character varying(800),
    bulkid character varying(100)
);


ALTER TABLE public.sc_sms_sent_status_details OWNER TO postgres;

--
-- Name: sc_sms_sent_status_details_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.sc_sms_sent_status_details_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.sc_sms_sent_status_details_id_seq OWNER TO postgres;

--
-- Name: sc_sms_sent_status_details_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.sc_sms_sent_status_details_id_seq OWNED BY public.sc_sms_sent_status_details.id;


--
-- Name: sc_smsconfigdtl; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.sc_smsconfigdtl (
    id integer NOT NULL,
    domainid integer,
    sendernumber character varying(20),
    aliasname character varying(150),
    senderid character varying(150),
    smsenable smallint DEFAULT 0,
    smsbot smallint DEFAULT 0,
    createdon timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    updatedon timestamp without time zone
);


ALTER TABLE public.sc_smsconfigdtl OWNER TO postgres;

--
-- Name: sc_smsconfigdtl_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.sc_smsconfigdtl_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.sc_smsconfigdtl_id_seq OWNER TO postgres;

--
-- Name: sc_smsconfigdtl_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.sc_smsconfigdtl_id_seq OWNED BY public.sc_smsconfigdtl.id;


--
-- Name: skillMapping; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public."skillMapping" (
    sqid integer NOT NULL,
    skid integer,
    usid integer,
    "domainId" integer,
    "createdAt" timestamp without time zone,
    "updateddAt" timestamp without time zone
);


ALTER TABLE public."skillMapping" OWNER TO postgres;

--
-- Name: skillMapping_sqid_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public."skillMapping_sqid_seq"
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public."skillMapping_sqid_seq" OWNER TO postgres;

--
-- Name: skillMapping_sqid_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public."skillMapping_sqid_seq" OWNED BY public."skillMapping".sqid;


--
-- Name: skills; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.skills (
    sid integer NOT NULL,
    skill_name character varying(255) DEFAULT NULL::character varying,
    skill_description character varying(255) DEFAULT NULL::character varying,
    "domainId" integer,
    "createdAt" timestamp without time zone,
    "updatedAt" timestamp without time zone,
    status integer,
    type character varying(255) DEFAULT NULL::character varying
);


ALTER TABLE public.skills OWNER TO postgres;

--
-- Name: skills_sid_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.skills_sid_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.skills_sid_seq OWNER TO postgres;

--
-- Name: skills_sid_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.skills_sid_seq OWNED BY public.skills.sid;


--
-- Name: sms_tb_indiv_user_click_open_cnt_dtl; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.sms_tb_indiv_user_click_open_cnt_dtl (
    id integer NOT NULL,
    domainid integer NOT NULL,
    campaignchatid character varying(300) NOT NULL,
    userid integer NOT NULL,
    actionid integer NOT NULL,
    channeltype character varying(20) DEFAULT 'SMS'::character varying,
    createdon timestamp without time zone DEFAULT CURRENT_TIMESTAMP
);


ALTER TABLE public.sms_tb_indiv_user_click_open_cnt_dtl OWNER TO postgres;

--
-- Name: sms_tb_indiv_user_click_open_cnt_dtl_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.sms_tb_indiv_user_click_open_cnt_dtl_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.sms_tb_indiv_user_click_open_cnt_dtl_id_seq OWNER TO postgres;

--
-- Name: sms_tb_indiv_user_click_open_cnt_dtl_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.sms_tb_indiv_user_click_open_cnt_dtl_id_seq OWNED BY public.sms_tb_indiv_user_click_open_cnt_dtl.id;


--
-- Name: tb_AgentBusinessHour; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public."tb_AgentBusinessHour" (
    "agentId" bigint NOT NULL,
    "DateOfLogin" timestamp without time zone NOT NULL,
    "businessStartTime" bigint,
    "businessEndTime" bigint,
    "createDate" timestamp without time zone
);


ALTER TABLE public."tb_AgentBusinessHour" OWNER TO postgres;

--
-- Name: tb_Business_Holidays; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public."tb_Business_Holidays" (
    hid integer NOT NULL,
    "domainId" integer,
    "fk_businessId" integer,
    "holidayName" character varying(1000) DEFAULT NULL::character varying,
    "startDate" bigint,
    "endDate" bigint,
    "isStatus" smallint DEFAULT 1,
    "holidayDesc" text,
    "createdAt" timestamp without time zone,
    "updatedAt" timestamp without time zone
);


ALTER TABLE public."tb_Business_Holidays" OWNER TO postgres;

--
-- Name: tb_Business_Holidays_hid_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public."tb_Business_Holidays_hid_seq"
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public."tb_Business_Holidays_hid_seq" OWNER TO postgres;

--
-- Name: tb_Business_Holidays_hid_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public."tb_Business_Holidays_hid_seq" OWNED BY public."tb_Business_Holidays".hid;


--
-- Name: tb_CallDurationHistory; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public."tb_CallDurationHistory" (
    "CallDurationHistoryId" bigint NOT NULL,
    "CallHistoryId" bigint,
    "StartTime" bigint,
    "EndTime" bigint,
    "agentDispositionId" integer,
    "Duration" bigint,
    "AgentHoldDuration" bigint,
    "AgentId" bigint,
    "AgentSkillId" integer,
    "IsBot" smallint,
    "BotId" integer,
    "RecordingUrl" character varying(500) DEFAULT NULL::character varying,
    "transferredTime" bigint,
    "IsMissedCall" smallint DEFAULT 0,
    "recordingDuration" bigint,
    "QueueId" integer,
    "createdAt" timestamp without time zone,
    "updatedAt" timestamp without time zone,
    "dispositionId" integer,
    summary text,
    subject text,
    "afterCallWorkTime" integer,
    "SentimentalScore" real,
    "sentimentalScoreCustomer" real,
    "sentimentalScoreAgent" real,
    "markAsRead" smallint,
    "dialDuration" bigint,
    "answeredDuration" bigint,
    "transferredTypeId" integer,
    "UMNStatus" integer,
    "transferedBy" integer,
    "transferedTo" integer,
    "ticketId" bigint,
    "isListen" smallint DEFAULT 0,
    "isWhisper" smallint DEFAULT 0,
    "isBarge" smallint DEFAULT 0,
    "IsAbandoned" smallint DEFAULT 0,
    "isTrigger" smallint DEFAULT 0,
    tags json
);


ALTER TABLE public."tb_CallDurationHistory" OWNER TO postgres;

--
-- Name: tb_CallDurationHistory_CallDurationHistoryId_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public."tb_CallDurationHistory_CallDurationHistoryId_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public."tb_CallDurationHistory_CallDurationHistoryId_seq" OWNER TO postgres;

--
-- Name: tb_CallDurationHistory_CallDurationHistoryId_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public."tb_CallDurationHistory_CallDurationHistoryId_seq" OWNED BY public."tb_CallDurationHistory"."CallDurationHistoryId";


--
-- Name: tb_CallHistory; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public."tb_CallHistory" (
    "CallHistoryId" bigint NOT NULL,
    "sessionId" character varying(250) DEFAULT NULL::character varying,
    "sessionStartTime" bigint,
    "sessionEndTime" bigint,
    "customerNumber" character varying(20) DEFAULT NULL::character varying,
    "domainId" integer,
    "IsInBound" smallint,
    "isOutboundCampaign" smallint DEFAULT 0,
    "dialDuration" bigint,
    "disconnectedBy" bigint,
    "queueDuration" bigint,
    "assignedTo" bigint,
    "assignedBy" bigint,
    "assignedOn" bigint,
    "calledNumber" bigint,
    "waitDuration" bigint,
    "recordingTypeId" integer,
    "CustomerId" integer,
    "voicemailUrl" character varying(500) DEFAULT NULL::character varying,
    "isVoiceMailAnswered" smallint DEFAULT 0,
    "voiceMailAnsweredCallHistoryId" bigint,
    "callDuration" bigint,
    "callBack" integer,
    "CallBackCallHistoryId" bigint,
    "voiceMailDuration" bigint,
    "createdAt" timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    "updatedAt" timestamp without time zone,
    "callCost" numeric(20,2) DEFAULT NULL::numeric,
    "IsAbandoned" smallint,
    "businessStartTime" bigint DEFAULT 0,
    "businessEndTime" bigint DEFAULT 0,
    "isVMCallBackCompleted" smallint DEFAULT 0,
    "IsOutOfBussinessHour" smallint
);


ALTER TABLE public."tb_CallHistory" OWNER TO postgres;

--
-- Name: tb_CallHistory_CallHistoryId_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public."tb_CallHistory_CallHistoryId_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public."tb_CallHistory_CallHistoryId_seq" OWNER TO postgres;

--
-- Name: tb_CallHistory_CallHistoryId_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public."tb_CallHistory_CallHistoryId_seq" OWNED BY public."tb_CallHistory"."CallHistoryId";


--
-- Name: tb_CallMessage; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public."tb_CallMessage" (
    "CallHistoryId" bigint,
    "Message" jsonb,
    "createdAt" timestamp without time zone,
    "updatedAt" timestamp without time zone,
    "CallDurationHistoryId" bigint NOT NULL
);


ALTER TABLE public."tb_CallMessage" OWNER TO postgres;

--
-- Name: tb_EmailHistory; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public."tb_EmailHistory" (
    "EmailHistoryId" bigint NOT NULL,
    "SessionId" character varying(250) DEFAULT NULL::character varying,
    "SessionStartTime" bigint DEFAULT ((EXTRACT(epoch FROM now()) * (1000)::numeric))::bigint,
    "SessionEndTime" bigint,
    "FromEmail" character varying(100) DEFAULT NULL::character varying,
    "ToEmail" character varying(100) DEFAULT NULL::character varying,
    "EmailSubject" character varying(250) DEFAULT NULL::character varying,
    "EmailFlowSourceId" integer,
    "CompanyId" integer,
    "DomainId" integer,
    "DispositionId" integer,
    "dispositionSubject" character varying(250) DEFAULT NULL::character varying,
    "Summary" text,
    "IsInBound" smallint,
    "AssignedTo" bigint,
    "AssignedBy" bigint,
    "AssignedOn" bigint,
    "waitDuration" bigint,
    "AfterEmailWorkTime" integer,
    "Istransferred" integer,
    "transferredTo" bigint,
    "dispositionUpdatedBy" bigint,
    "immediateParentId" bigint,
    "rootParentId" bigint,
    "TransferredTypeId" integer,
    "transferredDate" timestamp without time zone,
    "IsSessionClosed" smallint,
    "CustomerId" integer,
    "TicketId" bigint,
    "SupervisorName" character varying(100) DEFAULT NULL::character varying,
    "repliedDate" timestamp without time zone,
    "discardedDate" timestamp without time zone,
    "AgentId" bigint,
    "CreatedAt" timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    "UpdatedAt" timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    "SentimentalScore" real,
    "sentimentalScoreCustomer" real,
    "sentimentalScoreAgent" real,
    "isOpened" smallint DEFAULT 0,
    "replyChannelType" character varying(50) DEFAULT NULL::character varying,
    "responseHistoryId" integer,
    "threadMessageId" character varying(1000) DEFAULT NULL::character varying,
    "graphMessageId" character varying(2500) DEFAULT NULL::character varying,
    "conversationId" character varying(2500) DEFAULT NULL::character varying,
    tags jsonb,
    is_trash smallint DEFAULT 0
);


ALTER TABLE public."tb_EmailHistory" OWNER TO postgres;

--
-- Name: tb_EmailHistory_EmailHistoryId_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public."tb_EmailHistory_EmailHistoryId_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public."tb_EmailHistory_EmailHistoryId_seq" OWNER TO postgres;

--
-- Name: tb_EmailHistory_EmailHistoryId_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public."tb_EmailHistory_EmailHistoryId_seq" OWNED BY public."tb_EmailHistory"."EmailHistoryId";


--
-- Name: tb_EmailMessage; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public."tb_EmailMessage" (
    "EmailHistoryId" bigint NOT NULL,
    "Message" jsonb,
    "CreatedAt" timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    "UpdatedAt" timestamp without time zone DEFAULT CURRENT_TIMESTAMP
);


ALTER TABLE public."tb_EmailMessage" OWNER TO postgres;

--
-- Name: tb_User; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public."tb_User" (
    uid bigint NOT NULL,
    "UserID" integer,
    "UserName" character varying(255) DEFAULT NULL::character varying,
    "emailId" character varying(255) DEFAULT NULL::character varying,
    "companyId" integer,
    "domainId" integer,
    ext integer,
    roleid smallint,
    "queueId" character varying(500) DEFAULT NULL::character varying,
    "isActive" smallint,
    "StatusID" integer,
    emp_id character varying(20) DEFAULT NULL::character varying,
    "companyName" character varying(64) DEFAULT NULL::character varying,
    "addSkill" jsonb,
    permissions jsonb,
    "callTypeId" integer,
    "inboundCall" smallint DEFAULT 1,
    "outboundCampaignCall" smallint DEFAULT 0,
    "isVoice" smallint,
    "isVoiceCallTransfer" smallint,
    "isAvailableForVoice" smallint DEFAULT 1,
    "isEmail" smallint,
    "isEmailTransfer" smallint,
    "isSms" smallint,
    "isChat" smallint,
    "isChatTransfer" smallint,
    "isSocialMedia" smallint,
    voice integer,
    chat integer,
    email integer,
    sms integer,
    "socialMedia" integer,
    recording smallint DEFAULT 0,
    "knowledgeBase" smallint DEFAULT 0,
    "chatLimit" smallint DEFAULT 1,
    "stateTimer" character varying(50) DEFAULT NULL::character varying,
    "AssignedTime" timestamp without time zone,
    "chatFlag" smallint,
    "ProfileImage" text,
    localization character varying(20) DEFAULT NULL::character varying,
    "currentSession" character varying(100) DEFAULT NULL::character varying,
    "myAccUserId" integer,
    "directNumber" bigint,
    address character varying(255) DEFAULT NULL::character varying,
    routing_profile_id integer,
    "report_to_UserID" bigint,
    "sipLoginId" integer,
    "createdAt" timestamp without time zone,
    "updatedAt" timestamp without time zone,
    region character varying(255),
    state character varying(255),
    experience_level character varying(255),
    performance_score integer
);


ALTER TABLE public."tb_User" OWNER TO postgres;

--
-- Name: tb_User_uid_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public."tb_User_uid_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public."tb_User_uid_seq" OWNER TO postgres;

--
-- Name: tb_User_uid_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public."tb_User_uid_seq" OWNED BY public."tb_User".uid;


--
-- Name: tb_VideoDurationHistory; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public."tb_VideoDurationHistory" (
    "VideoDurationHistoryId" bigint NOT NULL,
    "VideoHistoryId" bigint,
    "StartTime" bigint,
    "EndTime" bigint,
    "agentDuration" bigint,
    "AgentId" bigint,
    "transferredTime" bigint,
    "IsMissedCall" smallint DEFAULT 0,
    "QueueId" integer,
    "dispositionId" integer,
    summary text,
    subject text,
    "UMNStatus" smallint,
    "afterVideoWorkTime" integer,
    "SentimentalScore" double precision,
    "sentimentalScoreCustomer" double precision,
    "sentimentalScoreAgent" double precision,
    "markAsRead" smallint,
    "dialDuration" bigint,
    "transferedBy" integer,
    "transferedTo" integer,
    "ticketId" bigint,
    "createdAt" timestamp without time zone,
    "updatedAt" timestamp without time zone,
    urmeet_user_list jsonb,
    record_size character varying(20),
    record_url character varying(2500)
);


ALTER TABLE public."tb_VideoDurationHistory" OWNER TO postgres;

--
-- Name: tb_VideoDurationHistory_VideoDurationHistoryId_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

ALTER TABLE public."tb_VideoDurationHistory" ALTER COLUMN "VideoDurationHistoryId" ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME public."tb_VideoDurationHistory_VideoDurationHistoryId_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: tb_VideoHistory; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public."tb_VideoHistory" (
    "VideoHistoryId" bigint NOT NULL,
    "meetingId" text,
    "sessionId" character varying(500),
    "sessionStartTime" bigint,
    "sessionEndTime" bigint,
    "customerNumber" character varying(20),
    "domainId" integer,
    "IsInBound" smallint,
    "dialDuration" bigint,
    "disconnectedBy" bigint,
    "queueDuration" bigint,
    "calledNumber" bigint,
    "waitDuration" bigint,
    "recordingTypeId" integer,
    "CustomerId" integer,
    "videoDuration" bigint,
    "createdAt" timestamp without time zone,
    "updatedAt" timestamp without time zone,
    "IsAbandoned" smallint,
    "businessStartTime" bigint DEFAULT 0,
    "businessEndTime" bigint DEFAULT 0,
    tags character varying(100),
    meet_url character varying(2500),
    schedule_start timestamp without time zone,
    schedule_end timestamp without time zone,
    meeting_title character varying(2000),
    meeting_description text,
    created_by integer,
    is_reminder integer DEFAULT 0
);


ALTER TABLE public."tb_VideoHistory" OWNER TO postgres;

--
-- Name: tb_VideoHistory_VideoHistoryId_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

ALTER TABLE public."tb_VideoHistory" ALTER COLUMN "VideoHistoryId" ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME public."tb_VideoHistory_VideoHistoryId_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: tb_VideoMessage; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public."tb_VideoMessage" (
    "VideoHistoryId" bigint,
    "Message" jsonb,
    "createdAt" timestamp without time zone,
    "updatedAt" timestamp without time zone,
    "VideoDurationHistoryId" bigint NOT NULL
);


ALTER TABLE public."tb_VideoMessage" OWNER TO postgres;

--
-- Name: tb_access_level_master; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_access_level_master (
    access_level_id smallint NOT NULL,
    access_code character varying(30) NOT NULL,
    access_description text,
    created_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP
);


ALTER TABLE public.tb_access_level_master OWNER TO postgres;

--
-- Name: tb_business_hours_json_info; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_business_hours_json_info (
    id integer NOT NULL,
    "fk_businessId" integer,
    "domainId" integer,
    day_name character varying(250) DEFAULT NULL::character varying,
    "isHoliday" character varying(250) DEFAULT NULL::character varying,
    "startTimeStamp" bigint,
    "endTimeStamp" bigint,
    "startTime" time without time zone,
    "endTime" time without time zone,
    "createdAt" timestamp without time zone,
    "updatedAt" timestamp without time zone
);


ALTER TABLE public.tb_business_hours_json_info OWNER TO postgres;

--
-- Name: tb_business_hours_json_info_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.tb_business_hours_json_info_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.tb_business_hours_json_info_id_seq OWNER TO postgres;

--
-- Name: tb_business_hours_json_info_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.tb_business_hours_json_info_id_seq OWNED BY public.tb_business_hours_json_info.id;


--
-- Name: tb_camapign_template_conversation_dtl; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_camapign_template_conversation_dtl (
    id bigint NOT NULL,
    domain_id integer,
    campaign_chat_id character varying(255),
    template_id character varying(255),
    channel character varying(50),
    conversation jsonb,
    template jsonb,
    team smallint,
    createdat timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    updatedat timestamp without time zone DEFAULT CURRENT_TIMESTAMP
);


ALTER TABLE public.tb_camapign_template_conversation_dtl OWNER TO postgres;

--
-- Name: tb_camapign_template_conversation_dtl_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.tb_camapign_template_conversation_dtl_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.tb_camapign_template_conversation_dtl_id_seq OWNER TO postgres;

--
-- Name: tb_camapign_template_conversation_dtl_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.tb_camapign_template_conversation_dtl_id_seq OWNED BY public.tb_camapign_template_conversation_dtl.id;


--
-- Name: tb_campaign_approval_details; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_campaign_approval_details (
    id integer NOT NULL,
    domain_id integer,
    campaign_name character varying(200),
    sessionid character varying(250),
    goal character varying(200),
    email_contact json,
    sms_contact json,
    camp_objactive json,
    email_template text,
    sms_template text,
    conversation json,
    approved_status integer,
    approved_by character varying(200),
    scheduled_time timestamp without time zone,
    createdat timestamp without time zone,
    updatedat timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    campaign_chat_id character varying(50)
);


ALTER TABLE public.tb_campaign_approval_details OWNER TO postgres;

--
-- Name: tb_campaign_approval_details_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

ALTER TABLE public.tb_campaign_approval_details ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME public.tb_campaign_approval_details_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: tb_campaign_open_click_rate_history_dtl; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_campaign_open_click_rate_history_dtl (
    id integer NOT NULL,
    "domainId" integer NOT NULL,
    "campaignId" integer NOT NULL,
    "emailId" character varying(100) DEFAULT NULL::character varying,
    "Phone_no" character varying(100) DEFAULT NULL::character varying,
    channel_type character varying(30) NOT NULL,
    "uniqueClick" integer DEFAULT 0,
    "uniqueOpen" integer DEFAULT 0,
    "CreatedOn" timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    "UpdatedOn" timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    campaign_chat_id character varying(255)
);


ALTER TABLE public.tb_campaign_open_click_rate_history_dtl OWNER TO postgres;

--
-- Name: tb_campaign_open_click_rate_history_dtl_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.tb_campaign_open_click_rate_history_dtl_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.tb_campaign_open_click_rate_history_dtl_id_seq OWNER TO postgres;

--
-- Name: tb_campaign_open_click_rate_history_dtl_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.tb_campaign_open_click_rate_history_dtl_id_seq OWNED BY public.tb_campaign_open_click_rate_history_dtl.id;


--
-- Name: tb_ccaas_customerSatisfactionRatingDesign; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public."tb_ccaas_customerSatisfactionRatingDesign" (
    "domainId" integer NOT NULL,
    caption text,
    "isEnable" smallint DEFAULT 1,
    "ratingDesignType" smallint,
    "labelRatingType" smallint,
    "followUpQuestion" smallint,
    "thanksNote" text,
    "dontDisturbType" smallint,
    "dontDistrubTime" smallint,
    "additionalQuestionType" smallint,
    "additionalQuestions" jsonb,
    "isVoice" smallint DEFAULT 0,
    "callFlows" character varying(2000) DEFAULT NULL::character varying,
    "minimumDurationType" smallint,
    "minimumDuration" smallint,
    "isWebWidget" smallint DEFAULT 0,
    "Widgets" character varying(2000) DEFAULT NULL::character varying,
    "isSms" smallint DEFAULT 0,
    "smsNumbers" character varying(2000) DEFAULT NULL::character varying,
    "smsNumberSend" character varying(30) DEFAULT NULL::character varying,
    "isSendSmsFromSameNumber" smallint DEFAULT 0,
    "smsMessage" jsonb,
    "isEmail" smallint DEFAULT 0,
    emails character varying(2000) DEFAULT NULL::character varying,
    "emailIdSend" character varying(200) DEFAULT NULL::character varying,
    "isSendEmailFromSameEmail" smallint DEFAULT 0,
    "emailSubject" character varying(2000) DEFAULT NULL::character varying,
    "emailMessage" jsonb,
    "isWhatsApp" smallint DEFAULT 0,
    "whatsAppChatFlows" character varying(2000) DEFAULT NULL::character varying,
    "isFacebook" smallint DEFAULT 0,
    "facebookChatFlows" character varying(2000) DEFAULT NULL::character varying,
    "isInstagram" smallint DEFAULT 0,
    "instagramChatFlows" character varying(2000) DEFAULT NULL::character varying,
    "isVideo" smallint DEFAULT 0,
    "createdDate" timestamp without time zone,
    "updatedDate" timestamp without time zone
);


ALTER TABLE public."tb_ccaas_customerSatisfactionRatingDesign" OWNER TO postgres;

--
-- Name: tb_company_dtl_comp_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.tb_company_dtl_comp_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.tb_company_dtl_comp_id_seq OWNER TO postgres;

--
-- Name: tb_company_dtl; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_company_dtl (
    company_id integer DEFAULT nextval('public.tb_company_dtl_comp_id_seq'::regclass) NOT NULL,
    domain_id integer,
    company_name character varying(1500) DEFAULT NULL::character varying,
    company_desc text,
    company_address text,
    company_size character varying(200) DEFAULT NULL::character varying,
    contact_person_email_id character varying(100) DEFAULT NULL::character varying,
    phone_number character varying(100) DEFAULT NULL::character varying,
    owner_name character varying(100) DEFAULT NULL::character varying,
    user_id integer,
    company_url character varying(500) DEFAULT NULL::character varying,
    source_name character varying(100) DEFAULT NULL::character varying,
    industry character varying(100) DEFAULT NULL::character varying,
    active_status smallint DEFAULT 1,
    created_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    updated_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP
);


ALTER TABLE public.tb_company_dtl OWNER TO postgres;

--
-- Name: tb_crm_product_details; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_crm_product_details (
    id integer NOT NULL,
    domain_id integer NOT NULL,
    product_insights jsonb DEFAULT '{}'::jsonb,
    product_tracker jsonb DEFAULT '[]'::jsonb,
    product_catalog jsonb DEFAULT '{}'::jsonb,
    product_notes jsonb DEFAULT '{}'::jsonb,
    product_quotes jsonb DEFAULT '{}'::jsonb,
    product_invoice jsonb DEFAULT '{}'::jsonb,
    created_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    updated_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP
);


ALTER TABLE public.tb_crm_product_details OWNER TO postgres;

--
-- Name: tb_crm_product_details_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.tb_crm_product_details_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.tb_crm_product_details_id_seq OWNER TO postgres;

--
-- Name: tb_crm_product_details_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.tb_crm_product_details_id_seq OWNED BY public.tb_crm_product_details.id;


--
-- Name: tb_customer_contact; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_customer_contact (
    customer_id integer NOT NULL,
    domain_id integer,
    first_name character varying(200) DEFAULT NULL::character varying,
    last_name character varying(200) DEFAULT NULL::character varying,
    primary_phone_number character varying(25) DEFAULT NULL::character varying,
    primary_email character varying(500) DEFAULT NULL::character varying,
    address text,
    industry_type character varying(250) DEFAULT NULL::character varying,
    source_type character varying(100) DEFAULT NULL::character varying,
    company_name character varying(1500) DEFAULT NULL::character varying,
    company_size character varying(100) DEFAULT NULL::character varying,
    user_id integer,
    demo_graphics integer,
    firmo_graphics integer,
    behaviors integer,
    is_active smallint DEFAULT 1,
    "lifeCycleStage" character varying(50) DEFAULT 'Lead'::character varying,
    "isUnSub_Email" smallint DEFAULT 0,
    created_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    updated_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    additional_info jsonb,
    is_mql smallint DEFAULT 0,
    is_sql smallint DEFAULT 0,
    sales_agent_id integer
);


ALTER TABLE public.tb_customer_contact OWNER TO postgres;

--
-- Name: tb_customer_contact_customer_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.tb_customer_contact_customer_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.tb_customer_contact_customer_id_seq OWNER TO postgres;

--
-- Name: tb_customer_contact_customer_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.tb_customer_contact_customer_id_seq OWNED BY public.tb_customer_contact.customer_id;


--
-- Name: tb_deal_master; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_deal_master (
    deal_id integer NOT NULL,
    customer_id integer,
    domain_id integer,
    deal_name character varying(500),
    deal_details jsonb,
    total_amount character varying(10),
    weighted_amount character varying(10),
    open_amount character varying(10),
    pending_amount character varying(10),
    closed_amount character varying(10),
    new_amount character varying(10),
    ai_suggest_summary_step jsonb,
    deal_activity jsonb,
    activity_timeline jsonb,
    created_at timestamp without time zone,
    updated_at timestamp without time zone
);


ALTER TABLE public.tb_deal_master OWNER TO postgres;

--
-- Name: tb_deal_master_deal_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

ALTER TABLE public.tb_deal_master ALTER COLUMN deal_id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME public.tb_deal_master_deal_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: tb_deal_master_log; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_deal_master_log (
    deal_log_id integer NOT NULL,
    fk_deal_id integer,
    customer_id integer,
    domain_id integer,
    deal_name character varying(500),
    deal_details jsonb,
    total_amount character varying(10),
    weighted_amount character varying(10),
    open_amount character varying(10),
    pending_amount character varying(10),
    closed_amount character varying(10),
    new_amount character varying(10),
    ai_suggest_summary_step jsonb,
    deal_activity jsonb,
    activity_timeline jsonb,
    created_at timestamp without time zone,
    updated_at timestamp without time zone,
    log_date timestamp without time zone
);


ALTER TABLE public.tb_deal_master_log OWNER TO postgres;

--
-- Name: tb_deal_master_log_deal_log_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

ALTER TABLE public.tb_deal_master_log ALTER COLUMN deal_log_id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME public.tb_deal_master_log_deal_log_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: tb_detailed_product_info; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_detailed_product_info (
    p_id bigint NOT NULL,
    fk_product_id bigint,
    domain_id integer,
    product_details jsonb,
    created_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    updated_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    total_revenue_growth jsonb
);


ALTER TABLE public.tb_detailed_product_info OWNER TO postgres;

--
-- Name: tb_detailed_product_info_p_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.tb_detailed_product_info_p_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.tb_detailed_product_info_p_id_seq OWNER TO postgres;

--
-- Name: tb_detailed_product_info_p_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.tb_detailed_product_info_p_id_seq OWNED BY public.tb_detailed_product_info.p_id;


--
-- Name: tb_email_config_dtl; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_email_config_dtl (
    id integer NOT NULL,
    "EmailConfigId" integer,
    domain_id integer NOT NULL,
    first_name character varying(150) DEFAULT NULL::character varying,
    email_type smallint NOT NULL,
    normal_mail jsonb,
    gmail_provided jsonb,
    outlook jsonb,
    "CreatedOn" timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    "UpdatedOn" timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    smtp_config jsonb
);


ALTER TABLE public.tb_email_config_dtl OWNER TO postgres;

--
-- Name: tb_email_config_dtl_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.tb_email_config_dtl_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.tb_email_config_dtl_id_seq OWNER TO postgres;

--
-- Name: tb_email_config_dtl_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.tb_email_config_dtl_id_seq OWNED BY public.tb_email_config_dtl.id;


--
-- Name: tb_interaction_notes_info; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_interaction_notes_info (
    nid integer NOT NULL,
    "domainId" integer,
    "historyId" integer,
    coid integer,
    "sessionId" character varying(500),
    "agentId" integer,
    "channelType" character varying(20),
    notes text,
    "createdAt" timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    "updatedAt" timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    imageurl character varying(1000)
);


ALTER TABLE public.tb_interaction_notes_info OWNER TO postgres;

--
-- Name: tb_interaction_notes_info_nid_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.tb_interaction_notes_info_nid_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.tb_interaction_notes_info_nid_seq OWNER TO postgres;

--
-- Name: tb_interaction_notes_info_nid_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.tb_interaction_notes_info_nid_seq OWNED BY public.tb_interaction_notes_info.nid;


--
-- Name: tb_mapIntentQueue; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public."tb_mapIntentQueue" (
    "contactCenterNumber" bigint NOT NULL,
    "intentGroupId" smallint,
    "queueId" integer NOT NULL,
    intents character varying(150) NOT NULL,
    "domainId" integer,
    "createdDate" timestamp without time zone,
    "updatedDate" timestamp without time zone
);


ALTER TABLE public."tb_mapIntentQueue" OWNER TO postgres;

--
-- Name: tb_onboard_dtl; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_onboard_dtl (
    id integer NOT NULL,
    "domainId" integer,
    conversation_flow jsonb,
    created_at timestamp without time zone,
    updated_at timestamp without time zone
);


ALTER TABLE public.tb_onboard_dtl OWNER TO postgres;

--
-- Name: tb_onboard_dtl_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.tb_onboard_dtl_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.tb_onboard_dtl_id_seq OWNER TO postgres;

--
-- Name: tb_onboard_dtl_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.tb_onboard_dtl_id_seq OWNED BY public.tb_onboard_dtl.id;


--
-- Name: tb_product_insights_info; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_product_insights_info (
    insight_id bigint NOT NULL,
    domain_id integer,
    product_performance_ranking jsonb,
    total_revenue_by_product jsonb,
    total_revenue_by_product_chart jsonb,
    product_revenue_growth jsonb,
    product_revenue_growth_chart jsonb,
    average_deal_size_by_product jsonb,
    average_deal_size_by_product_chart jsonb,
    recent_quotes jsonb,
    recent_invoices jsonb,
    created_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    updated_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP
);


ALTER TABLE public.tb_product_insights_info OWNER TO postgres;

--
-- Name: tb_product_insights_info_insight_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.tb_product_insights_info_insight_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.tb_product_insights_info_insight_id_seq OWNER TO postgres;

--
-- Name: tb_product_insights_info_insight_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.tb_product_insights_info_insight_id_seq OWNED BY public.tb_product_insights_info.insight_id;


--
-- Name: tb_product_master; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_product_master (
    product_id bigint NOT NULL,
    domain_id integer,
    product_image text,
    product_name character varying(200) NOT NULL,
    short_description character varying(500),
    product_category character varying(100),
    starting_price_plan character varying,
    active_customers integer DEFAULT 0,
    rating_reviews character varying,
    created_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    updated_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP
);


ALTER TABLE public.tb_product_master OWNER TO postgres;

--
-- Name: tb_product_master_product_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.tb_product_master_product_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.tb_product_master_product_id_seq OWNER TO postgres;

--
-- Name: tb_product_master_product_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.tb_product_master_product_id_seq OWNED BY public.tb_product_master.product_id;


--
-- Name: tb_project_insights; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_project_insights (
    pid integer NOT NULL,
    domain_id integer NOT NULL,
    predictive_project_summary jsonb DEFAULT '{}'::jsonb,
    recent_activity jsonb DEFAULT '{}'::jsonb,
    requirements_list jsonb DEFAULT '{}'::jsonb,
    client_deliverables jsonb DEFAULT '{}'::jsonb,
    upcoming_delays_risk_warnings jsonb DEFAULT '{}'::jsonb,
    workload_capacity_predictions jsonb DEFAULT '{}'::jsonb,
    resource_allocation_insights jsonb DEFAULT '{}'::jsonb,
    requirement_stability_rework_prediction jsonb DEFAULT '{}'::jsonb,
    operational_efficiency_signals jsonb DEFAULT '{}'::jsonb,
    created_by integer,
    created_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    updated_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP
);


ALTER TABLE public.tb_project_insights OWNER TO postgres;

--
-- Name: tb_project_insights_pid_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.tb_project_insights_pid_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.tb_project_insights_pid_seq OWNER TO postgres;

--
-- Name: tb_project_insights_pid_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.tb_project_insights_pid_seq OWNED BY public.tb_project_insights.pid;


--
-- Name: tb_projects_dtl; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_projects_dtl (
    pid integer NOT NULL,
    customer_id integer,
    domain_id integer,
    fk_pro_id integer,
    product_ref_id character varying(250),
    created_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    updated_at timestamp without time zone,
    agentic_insight jsonb,
    requirement_list jsonb,
    next_steps text,
    product_stages jsonb,
    requested_requirements jsonb,
    team_execution_overview jsonb,
    files jsonb
);


ALTER TABLE public.tb_projects_dtl OWNER TO postgres;

--
-- Name: tb_projects_dtl_pid_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

ALTER TABLE public.tb_projects_dtl ALTER COLUMN pid ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME public.tb_projects_dtl_pid_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: tb_projects_master; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_projects_master (
    pro_id integer NOT NULL,
    customer_id integer,
    domain_id integer,
    projests jsonb,
    created_at timestamp without time zone DEFAULT now(),
    updated_at timestamp without time zone
);


ALTER TABLE public.tb_projects_master OWNER TO postgres;

--
-- Name: tb_projects_master_pro_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.tb_projects_master_pro_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.tb_projects_master_pro_id_seq OWNER TO postgres;

--
-- Name: tb_projects_master_pro_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.tb_projects_master_pro_id_seq OWNED BY public.tb_projects_master.pro_id;


--
-- Name: tb_queue_permission; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_queue_permission (
    fk_qid integer NOT NULL,
    "domainId" integer,
    "agentMonitoring" smallint DEFAULT 0,
    "addOrRemove" smallint DEFAULT 0,
    "reportingDashboard" smallint DEFAULT 0,
    "createdAt" timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    "updatedAt" timestamp without time zone DEFAULT CURRENT_TIMESTAMP
);


ALTER TABLE public.tb_queue_permission OWNER TO postgres;

--
-- Name: tb_role_master; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_role_master (
    role_id integer NOT NULL,
    domain_id integer,
    role_name character varying(100) NOT NULL,
    permissions jsonb NOT NULL,
    createdat timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    updatedat timestamp without time zone DEFAULT CURRENT_TIMESTAMP
);


ALTER TABLE public.tb_role_master OWNER TO postgres;

--
-- Name: tb_role_master_role_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

ALTER TABLE public.tb_role_master ALTER COLUMN role_id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME public.tb_role_master_role_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: tb_role_permission; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_role_permission (
    role_id integer NOT NULL,
    module_id integer NOT NULL,
    access_level_id smallint
);


ALTER TABLE public.tb_role_permission OWNER TO postgres;

--
-- Name: tb_screen_module; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_screen_module (
    id integer NOT NULL,
    title character varying(100) NOT NULL,
    parent_title_id integer DEFAULT 0 NOT NULL,
    created_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP
);


ALTER TABLE public.tb_screen_module OWNER TO postgres;

--
-- Name: tb_screen_module_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

ALTER TABLE public.tb_screen_module ALTER COLUMN id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME public.tb_screen_module_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: tb_secondary_customer_contact; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_secondary_customer_contact (
    id integer NOT NULL,
    fk_customer_id integer NOT NULL,
    domain_id integer,
    secondary_phone_number character varying(500) DEFAULT NULL::character varying,
    secondary_email character varying(500) DEFAULT NULL::character varying,
    created_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    updated_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP
);


ALTER TABLE public.tb_secondary_customer_contact OWNER TO postgres;

--
-- Name: tb_secondary_customer_contact_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.tb_secondary_customer_contact_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public.tb_secondary_customer_contact_id_seq OWNER TO postgres;

--
-- Name: tb_secondary_customer_contact_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.tb_secondary_customer_contact_id_seq OWNED BY public.tb_secondary_customer_contact.id;


--
-- Name: tb_userAddonLanguageInfo; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public."tb_userAddonLanguageInfo" (
    aid integer NOT NULL,
    "domainId" integer,
    "userId" integer,
    ext integer,
    pri_language character varying(100) DEFAULT NULL::character varying,
    sec_language character varying(1000) DEFAULT NULL::character varying,
    "createdAt" timestamp without time zone,
    "updatedAt" timestamp without time zone
);


ALTER TABLE public."tb_userAddonLanguageInfo" OWNER TO postgres;

--
-- Name: tb_userAddonLanguageInfo_aid_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public."tb_userAddonLanguageInfo_aid_seq"
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER SEQUENCE public."tb_userAddonLanguageInfo_aid_seq" OWNER TO postgres;

--
-- Name: tb_userAddonLanguageInfo_aid_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public."tb_userAddonLanguageInfo_aid_seq" OWNED BY public."tb_userAddonLanguageInfo".aid;


--
-- Name: tb_user_onboard_setting_flow; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_user_onboard_setting_flow (
    email_id character varying(150) NOT NULL,
    "domainId" integer,
    flow_track jsonb,
    "isCompleted" smallint DEFAULT 0,
    "createdAt" timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    "updatedAt" timestamp without time zone
);


ALTER TABLE public.tb_user_onboard_setting_flow OWNER TO postgres;

--
-- Name: tb_voice_mail_msg; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.tb_voice_mail_msg (
    voice_mail_url_id bigint NOT NULL,
    session_id character varying(250),
    domain_id integer,
    voice_mail_summary text,
    voice_mail_subject text,
    created_at timestamp without time zone,
    updated_at timestamp without time zone
);


ALTER TABLE public.tb_voice_mail_msg OWNER TO postgres;

--
-- Name: tb_voice_mail_msg_voice_mail_url_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

ALTER TABLE public.tb_voice_mail_msg ALTER COLUMN voice_mail_url_id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME public.tb_voice_mail_msg_voice_mail_url_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: wc_master_revert_status_details; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.wc_master_revert_status_details (
    "campaignchatId" character varying(300) NOT NULL,
    "domainId" integer NOT NULL,
    "isReverted" smallint DEFAULT 0 NOT NULL,
    "createdDate" timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    "updatedDate" timestamp without time zone
);


ALTER TABLE public.wc_master_revert_status_details OWNER TO postgres;

--
-- Name: agent_status aid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.agent_status ALTER COLUMN aid SET DEFAULT nextval('public.agent_status_aid_seq'::regclass);


--
-- Name: callFlow callFlowId; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public."callFlow" ALTER COLUMN "callFlowId" SET DEFAULT nextval('public."callFlow_callFlowId_seq"'::regclass);


--
-- Name: callMap cid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public."callMap" ALTER COLUMN cid SET DEFAULT nextval('public."callMap_cid_seq"'::regclass);


--
-- Name: callsessionstartendlog id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.callsessionstartendlog ALTER COLUMN id SET DEFAULT nextval('public.callsessionstartendlog_id_seq'::regclass);


--
-- Name: ccaas_settings_voice_video_recording_dtl rid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.ccaas_settings_voice_video_recording_dtl ALTER COLUMN rid SET DEFAULT nextval('public.ccaas_settings_voice_video_recording_dtl_rid_seq'::regclass);


--
-- Name: custom_hours cid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.custom_hours ALTER COLUMN cid SET DEFAULT nextval('public.custom_hours_cid_seq'::regclass);


--
-- Name: customer_contact coid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.customer_contact ALTER COLUMN coid SET DEFAULT nextval('public.customer_contact_coid_seq'::regclass);


--
-- Name: did_purchase pid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.did_purchase ALTER COLUMN pid SET DEFAULT nextval('public.did_purchase_pid_seq'::regclass);


--
-- Name: disposition_status did; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.disposition_status ALTER COLUMN did SET DEFAULT nextval('public.disposition_status_did_seq'::regclass);


--
-- Name: ec_campaign_contacts_dtl id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.ec_campaign_contacts_dtl ALTER COLUMN id SET DEFAULT nextval('public.ec_campaign_contacts_dtl_id_seq'::regclass);


--
-- Name: ec_email_campaign_AB_testing_dtl id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public."ec_email_campaign_AB_testing_dtl" ALTER COLUMN id SET DEFAULT nextval('public."ec_email_campaign_AB_testing_dtl_id_seq"'::regclass);


--
-- Name: ec_email_campaign_master_dtl campaignId; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.ec_email_campaign_master_dtl ALTER COLUMN "campaignId" SET DEFAULT nextval('public."ec_email_campaign_master_dtl_campaignId_seq"'::regclass);


--
-- Name: ec_email_campaign_master_dtl_exclude_email id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.ec_email_campaign_master_dtl_exclude_email ALTER COLUMN id SET DEFAULT nextval('public.ec_email_campaign_master_dtl_exclude_email_id_seq'::regclass);


--
-- Name: email_config EmailConfigId; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.email_config ALTER COLUMN "EmailConfigId" SET DEFAULT nextval('public."email_config_EmailConfigId_seq"'::regclass);


--
-- Name: notification_master notification_id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.notification_master ALTER COLUMN notification_id SET DEFAULT nextval('public.notification_master_notification_id_seq'::regclass);


--
-- Name: prompt_library uuid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.prompt_library ALTER COLUMN uuid SET DEFAULT nextval('public.prompt_library_uuid_seq'::regclass);


--
-- Name: queue qid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.queue ALTER COLUMN qid SET DEFAULT nextval('public.queue_qid_seq'::regclass);


--
-- Name: queueWeighTime qwtId; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public."queueWeighTime" ALTER COLUMN "qwtId" SET DEFAULT nextval('public."queueWeighTime_qwtId_seq"'::regclass);


--
-- Name: routingqueueprofilemapping rqid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.routingqueueprofilemapping ALTER COLUMN rqid SET DEFAULT nextval('public.routingqueueprofilemapping_rqid_seq'::regclass);


--
-- Name: sc_campaign_master_dtl id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.sc_campaign_master_dtl ALTER COLUMN id SET DEFAULT nextval('public.sc_campaign_master_dtl_id_seq'::regclass);


--
-- Name: sc_campaign_master_schedule_dtl id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.sc_campaign_master_schedule_dtl ALTER COLUMN id SET DEFAULT nextval('public.sc_campaign_master_schedule_dtl_id_seq'::regclass);


--
-- Name: sc_sms_sent_status_details id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.sc_sms_sent_status_details ALTER COLUMN id SET DEFAULT nextval('public.sc_sms_sent_status_details_id_seq'::regclass);


--
-- Name: sc_smsconfigdtl id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.sc_smsconfigdtl ALTER COLUMN id SET DEFAULT nextval('public.sc_smsconfigdtl_id_seq'::regclass);


--
-- Name: skillMapping sqid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public."skillMapping" ALTER COLUMN sqid SET DEFAULT nextval('public."skillMapping_sqid_seq"'::regclass);


--
-- Name: skills sid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.skills ALTER COLUMN sid SET DEFAULT nextval('public.skills_sid_seq'::regclass);


--
-- Name: sms_tb_indiv_user_click_open_cnt_dtl id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.sms_tb_indiv_user_click_open_cnt_dtl ALTER COLUMN id SET DEFAULT nextval('public.sms_tb_indiv_user_click_open_cnt_dtl_id_seq'::regclass);


--
-- Name: tb_Business_Holidays hid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public."tb_Business_Holidays" ALTER COLUMN hid SET DEFAULT nextval('public."tb_Business_Holidays_hid_seq"'::regclass);


--
-- Name: tb_CallDurationHistory CallDurationHistoryId; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public."tb_CallDurationHistory" ALTER COLUMN "CallDurationHistoryId" SET DEFAULT nextval('public."tb_CallDurationHistory_CallDurationHistoryId_seq"'::regclass);


--
-- Name: tb_CallHistory CallHistoryId; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public."tb_CallHistory" ALTER COLUMN "CallHistoryId" SET DEFAULT nextval('public."tb_CallHistory_CallHistoryId_seq"'::regclass);


--
-- Name: tb_EmailHistory EmailHistoryId; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public."tb_EmailHistory" ALTER COLUMN "EmailHistoryId" SET DEFAULT nextval('public."tb_EmailHistory_EmailHistoryId_seq"'::regclass);


--
-- Name: tb_User uid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public."tb_User" ALTER COLUMN uid SET DEFAULT nextval('public."tb_User_uid_seq"'::regclass);


--
-- Name: tb_business_hours_json_info id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_business_hours_json_info ALTER COLUMN id SET DEFAULT nextval('public.tb_business_hours_json_info_id_seq'::regclass);


--
-- Name: tb_camapign_template_conversation_dtl id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_camapign_template_conversation_dtl ALTER COLUMN id SET DEFAULT nextval('public.tb_camapign_template_conversation_dtl_id_seq'::regclass);


--
-- Name: tb_campaign_open_click_rate_history_dtl id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_campaign_open_click_rate_history_dtl ALTER COLUMN id SET DEFAULT nextval('public.tb_campaign_open_click_rate_history_dtl_id_seq'::regclass);


--
-- Name: tb_crm_product_details id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_crm_product_details ALTER COLUMN id SET DEFAULT nextval('public.tb_crm_product_details_id_seq'::regclass);


--
-- Name: tb_customer_contact customer_id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_customer_contact ALTER COLUMN customer_id SET DEFAULT nextval('public.tb_customer_contact_customer_id_seq'::regclass);


--
-- Name: tb_detailed_product_info p_id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_detailed_product_info ALTER COLUMN p_id SET DEFAULT nextval('public.tb_detailed_product_info_p_id_seq'::regclass);


--
-- Name: tb_email_config_dtl id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_email_config_dtl ALTER COLUMN id SET DEFAULT nextval('public.tb_email_config_dtl_id_seq'::regclass);


--
-- Name: tb_interaction_notes_info nid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_interaction_notes_info ALTER COLUMN nid SET DEFAULT nextval('public.tb_interaction_notes_info_nid_seq'::regclass);


--
-- Name: tb_onboard_dtl id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_onboard_dtl ALTER COLUMN id SET DEFAULT nextval('public.tb_onboard_dtl_id_seq'::regclass);


--
-- Name: tb_product_insights_info insight_id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_product_insights_info ALTER COLUMN insight_id SET DEFAULT nextval('public.tb_product_insights_info_insight_id_seq'::regclass);


--
-- Name: tb_product_master product_id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_product_master ALTER COLUMN product_id SET DEFAULT nextval('public.tb_product_master_product_id_seq'::regclass);


--
-- Name: tb_project_insights pid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_project_insights ALTER COLUMN pid SET DEFAULT nextval('public.tb_project_insights_pid_seq'::regclass);


--
-- Name: tb_projects_master pro_id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_projects_master ALTER COLUMN pro_id SET DEFAULT nextval('public.tb_projects_master_pro_id_seq'::regclass);


--
-- Name: tb_secondary_customer_contact id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_secondary_customer_contact ALTER COLUMN id SET DEFAULT nextval('public.tb_secondary_customer_contact_id_seq'::regclass);


--
-- Name: tb_userAddonLanguageInfo aid; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public."tb_userAddonLanguageInfo" ALTER COLUMN aid SET DEFAULT nextval('public."tb_userAddonLanguageInfo_aid_seq"'::regclass);


--
-- Name: TypeCode TypeCode_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public."TypeCode"
    ADD CONSTRAINT "TypeCode_pkey" PRIMARY KEY ("TypeCodeID");


--
-- Name: agent_status agent_status_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.agent_status
    ADD CONSTRAINT agent_status_pkey PRIMARY KEY (aid);


--
-- Name: callFlow callFlow_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public."callFlow"
    ADD CONSTRAINT "callFlow_pkey" PRIMARY KEY ("callFlowId");


--
-- Name: callMap callMap_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public."callMap"
    ADD CONSTRAINT "callMap_pkey" PRIMARY KEY (cid);


--
-- Name: call_configuration call_configuration_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.call_configuration
    ADD CONSTRAINT call_configuration_pkey PRIMARY KEY ("contactCenterNumber");


--
-- Name: callsessionstartendlog callsessionstartendlog_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.callsessionstartendlog
    ADD CONSTRAINT callsessionstartendlog_pkey PRIMARY KEY (id);


--
-- Name: ccaas_settings_voice_video_recording_dtl ccaas_settings_voice_video_recording_dtl_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.ccaas_settings_voice_video_recording_dtl
    ADD CONSTRAINT ccaas_settings_voice_video_recording_dtl_pkey PRIMARY KEY (rid);


--
-- Name: chatbotpaymentintegration chatbotpaymentintegration_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.chatbotpaymentintegration
    ADD CONSTRAINT chatbotpaymentintegration_pkey PRIMARY KEY (tokenid);


--
-- Name: custom_hours custom_hours_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.custom_hours
    ADD CONSTRAINT custom_hours_pkey PRIMARY KEY (cid);


--
-- Name: customer_contact customer_contact_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.customer_contact
    ADD CONSTRAINT customer_contact_pkey PRIMARY KEY (coid);


--
-- Name: did_purchase did_purchase_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.did_purchase
    ADD CONSTRAINT did_purchase_pkey PRIMARY KEY (pid);


--
-- Name: disposition_status disposition_status_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.disposition_status
    ADD CONSTRAINT disposition_status_pkey PRIMARY KEY (did);


--
-- Name: ec_campaign_contacts_dtl ec_campaign_contacts_dtl_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.ec_campaign_contacts_dtl
    ADD CONSTRAINT ec_campaign_contacts_dtl_pkey PRIMARY KEY (id);


--
-- Name: ec_email_campaign_AB_testing_dtl ec_email_campaign_AB_testing_dtl_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public."ec_email_campaign_AB_testing_dtl"
    ADD CONSTRAINT "ec_email_campaign_AB_testing_dtl_pkey" PRIMARY KEY (id);


--
-- Name: ec_email_campaign_master_dtl ec_email_campaign_master_dtl_campaignchatid_key; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.ec_email_campaign_master_dtl
    ADD CONSTRAINT ec_email_campaign_master_dtl_campaignchatid_key UNIQUE (campaignchatid);


--
-- Name: ec_email_campaign_master_dtl_exclude_email ec_email_campaign_master_dtl_exclude_email_campaignchatid_key; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.ec_email_campaign_master_dtl_exclude_email
    ADD CONSTRAINT ec_email_campaign_master_dtl_exclude_email_campaignchatid_key UNIQUE (campaignchatid);


--
-- Name: ec_email_campaign_master_dtl_exclude_email ec_email_campaign_master_dtl_exclude_email_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.ec_email_campaign_master_dtl_exclude_email
    ADD CONSTRAINT ec_email_campaign_master_dtl_exclude_email_pkey PRIMARY KEY (id);


--
-- Name: ec_email_campaign_master_dtl ec_email_campaign_master_dtl_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.ec_email_campaign_master_dtl
    ADD CONSTRAINT ec_email_campaign_master_dtl_pkey PRIMARY KEY ("campaignId");


--
-- Name: ec_email_failed_contact_dtl ec_email_failed_contact_dtl_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.ec_email_failed_contact_dtl
    ADD CONSTRAINT ec_email_failed_contact_dtl_pkey PRIMARY KEY (id);


--
-- Name: email_config email_config_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.email_config
    ADD CONSTRAINT email_config_pkey PRIMARY KEY ("EmailConfigId");


--
-- Name: email_tb_indiv_admin_action_dtl email_tb_indiv_admin_action_dtl_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.email_tb_indiv_admin_action_dtl
    ADD CONSTRAINT email_tb_indiv_admin_action_dtl_pkey PRIMARY KEY (id);


--
-- Name: email_tb_indiv_user_click_open_cnt_dtl email_tb_indiv_user_click_open_cnt_dtl_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.email_tb_indiv_user_click_open_cnt_dtl
    ADD CONSTRAINT email_tb_indiv_user_click_open_cnt_dtl_pkey PRIMARY KEY (id);


--
-- Name: notification_agent notification_agent_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.notification_agent
    ADD CONSTRAINT notification_agent_pkey PRIMARY KEY (notification_id, agent_id);


--
-- Name: notification_master notification_master_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.notification_master
    ADD CONSTRAINT notification_master_pkey PRIMARY KEY (notification_id);


--
-- Name: tb_VideoDurationHistory pk_tb_VideoDurationHistory; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public."tb_VideoDurationHistory"
    ADD CONSTRAINT "pk_tb_VideoDurationHistory" PRIMARY KEY ("VideoDurationHistoryId");


--
-- Name: tb_VideoHistory pk_tb_VideoHistory; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public."tb_VideoHistory"
    ADD CONSTRAINT "pk_tb_VideoHistory" PRIMARY KEY ("VideoHistoryId");


--
-- Name: tb_VideoMessage pk_tb_VideoMessage; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public."tb_VideoMessage"
    ADD CONSTRAINT "pk_tb_VideoMessage" PRIMARY KEY ("VideoDurationHistoryId");


--
-- Name: prompt_library prompt_library_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.prompt_library
    ADD CONSTRAINT prompt_library_pkey PRIMARY KEY (uuid);


--
-- Name: queueWeighTime queueWeighTime_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public."queueWeighTime"
    ADD CONSTRAINT "queueWeighTime_pkey" PRIMARY KEY ("qwtId", qid, "domainId");


--
-- Name: queue queue_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.queue
    ADD CONSTRAINT queue_pkey PRIMARY KEY (qid);


--
-- Name: routing_profile routing_profile_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.routing_profile
    ADD CONSTRAINT routing_profile_pkey PRIMARY KEY (rid);


--
-- Name: routingqueueprofilemapping routingqueueprofilemapping_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.routingqueueprofilemapping
    ADD CONSTRAINT routingqueueprofilemapping_pkey PRIMARY KEY (rqid);


--
-- Name: sc_campaign_master_dtl sc_campaign_master_dtl_campaignchatid_key; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.sc_campaign_master_dtl
    ADD CONSTRAINT sc_campaign_master_dtl_campaignchatid_key UNIQUE (campaignchatid);


--
-- Name: sc_campaign_master_dtl sc_campaign_master_dtl_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.sc_campaign_master_dtl
    ADD CONSTRAINT sc_campaign_master_dtl_pkey PRIMARY KEY (id);


--
-- Name: sc_campaign_master_schedule_dtl sc_campaign_master_schedule_dtl_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.sc_campaign_master_schedule_dtl
    ADD CONSTRAINT sc_campaign_master_schedule_dtl_pkey PRIMARY KEY (id);


--
-- Name: sc_sms_sent_status_details sc_sms_sent_status_details_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.sc_sms_sent_status_details
    ADD CONSTRAINT sc_sms_sent_status_details_pkey PRIMARY KEY (id);


--
-- Name: sc_smsconfigdtl sc_smsconfigdtl_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.sc_smsconfigdtl
    ADD CONSTRAINT sc_smsconfigdtl_pkey PRIMARY KEY (id);


--
-- Name: skillMapping skillMapping_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public."skillMapping"
    ADD CONSTRAINT "skillMapping_pkey" PRIMARY KEY (sqid);


--
-- Name: skills skills_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.skills
    ADD CONSTRAINT skills_pkey PRIMARY KEY (sid);


--
-- Name: sms_tb_indiv_user_click_open_cnt_dtl sms_tb_indiv_user_click_open_cnt_dtl_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.sms_tb_indiv_user_click_open_cnt_dtl
    ADD CONSTRAINT sms_tb_indiv_user_click_open_cnt_dtl_pkey PRIMARY KEY (id);


--
-- Name: tb_AgentBusinessHour tb_AgentBusinessHour_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public."tb_AgentBusinessHour"
    ADD CONSTRAINT "tb_AgentBusinessHour_pkey" PRIMARY KEY ("agentId", "DateOfLogin");


--
-- Name: tb_Business_Holidays tb_Business_Holidays_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public."tb_Business_Holidays"
    ADD CONSTRAINT "tb_Business_Holidays_pkey" PRIMARY KEY (hid);


--
-- Name: tb_CallDurationHistory tb_CallDurationHistory_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public."tb_CallDurationHistory"
    ADD CONSTRAINT "tb_CallDurationHistory_pkey" PRIMARY KEY ("CallDurationHistoryId");


--
-- Name: tb_CallHistory tb_CallHistory_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public."tb_CallHistory"
    ADD CONSTRAINT "tb_CallHistory_pkey" PRIMARY KEY ("CallHistoryId");


--
-- Name: tb_CallMessage tb_CallMessage_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public."tb_CallMessage"
    ADD CONSTRAINT "tb_CallMessage_pkey" PRIMARY KEY ("CallDurationHistoryId");


--
-- Name: tb_EmailHistory tb_EmailHistory_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public."tb_EmailHistory"
    ADD CONSTRAINT "tb_EmailHistory_pkey" PRIMARY KEY ("EmailHistoryId");


--
-- Name: tb_EmailMessage tb_EmailMessage_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public."tb_EmailMessage"
    ADD CONSTRAINT "tb_EmailMessage_pkey" PRIMARY KEY ("EmailHistoryId");


--
-- Name: tb_User tb_User_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public."tb_User"
    ADD CONSTRAINT "tb_User_pkey" PRIMARY KEY (uid);


--
-- Name: tb_access_level_master tb_access_level_master_access_code_key; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_access_level_master
    ADD CONSTRAINT tb_access_level_master_access_code_key UNIQUE (access_code);


--
-- Name: tb_access_level_master tb_access_level_master_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_access_level_master
    ADD CONSTRAINT tb_access_level_master_pkey PRIMARY KEY (access_level_id);


--
-- Name: tb_business_hours_json_info tb_business_hours_json_info_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_business_hours_json_info
    ADD CONSTRAINT tb_business_hours_json_info_pkey PRIMARY KEY (id);


--
-- Name: tb_camapign_template_conversation_dtl tb_camapign_template_conversation_dtl_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_camapign_template_conversation_dtl
    ADD CONSTRAINT tb_camapign_template_conversation_dtl_pkey PRIMARY KEY (id);


--
-- Name: tb_campaign_approval_details tb_campaign_approval_details_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_campaign_approval_details
    ADD CONSTRAINT tb_campaign_approval_details_pkey PRIMARY KEY (id);


--
-- Name: tb_campaign_open_click_rate_history_dtl tb_campaign_open_click_rate_history_dtl_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_campaign_open_click_rate_history_dtl
    ADD CONSTRAINT tb_campaign_open_click_rate_history_dtl_pkey PRIMARY KEY (id);


--
-- Name: tb_ccaas_customerSatisfactionRatingDesign tb_ccaas_customerSatisfactionRatingDesign_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public."tb_ccaas_customerSatisfactionRatingDesign"
    ADD CONSTRAINT "tb_ccaas_customerSatisfactionRatingDesign_pkey" PRIMARY KEY ("domainId");


--
-- Name: tb_company_dtl tb_company_dtl_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_company_dtl
    ADD CONSTRAINT tb_company_dtl_pkey PRIMARY KEY (company_id);


--
-- Name: tb_crm_product_details tb_crm_product_details_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_crm_product_details
    ADD CONSTRAINT tb_crm_product_details_pkey PRIMARY KEY (id);


--
-- Name: tb_customer_contact tb_customer_contact_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_customer_contact
    ADD CONSTRAINT tb_customer_contact_pkey PRIMARY KEY (customer_id);


--
-- Name: tb_deal_master_log tb_deal_master_log_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_deal_master_log
    ADD CONSTRAINT tb_deal_master_log_pkey PRIMARY KEY (deal_log_id);


--
-- Name: tb_deal_master tb_deal_master_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_deal_master
    ADD CONSTRAINT tb_deal_master_pkey PRIMARY KEY (deal_id);


--
-- Name: tb_detailed_product_info tb_detailed_product_info_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_detailed_product_info
    ADD CONSTRAINT tb_detailed_product_info_pkey PRIMARY KEY (p_id);


--
-- Name: tb_email_config_dtl tb_email_config_dtl_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_email_config_dtl
    ADD CONSTRAINT tb_email_config_dtl_pkey PRIMARY KEY (id);


--
-- Name: tb_interaction_notes_info tb_interaction_notes_info_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_interaction_notes_info
    ADD CONSTRAINT tb_interaction_notes_info_pkey PRIMARY KEY (nid);


--
-- Name: tb_mapIntentQueue tb_mapIntentQueue_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public."tb_mapIntentQueue"
    ADD CONSTRAINT "tb_mapIntentQueue_pkey" PRIMARY KEY ("contactCenterNumber", intents);


--
-- Name: tb_onboard_dtl tb_onboard_dtl_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_onboard_dtl
    ADD CONSTRAINT tb_onboard_dtl_pkey PRIMARY KEY (id);


--
-- Name: tb_product_insights_info tb_product_insights_info_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_product_insights_info
    ADD CONSTRAINT tb_product_insights_info_pkey PRIMARY KEY (insight_id);


--
-- Name: tb_product_master tb_product_master_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_product_master
    ADD CONSTRAINT tb_product_master_pkey PRIMARY KEY (product_id);


--
-- Name: tb_project_insights tb_project_insights_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_project_insights
    ADD CONSTRAINT tb_project_insights_pkey PRIMARY KEY (pid);


--
-- Name: tb_projects_dtl tb_projects_dtl_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_projects_dtl
    ADD CONSTRAINT tb_projects_dtl_pkey PRIMARY KEY (pid);


--
-- Name: tb_projects_master tb_projects_master_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_projects_master
    ADD CONSTRAINT tb_projects_master_pkey PRIMARY KEY (pro_id);


--
-- Name: tb_queue_permission tb_queue_permission_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_queue_permission
    ADD CONSTRAINT tb_queue_permission_pkey PRIMARY KEY (fk_qid);


--
-- Name: tb_role_master tb_role_master_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_role_master
    ADD CONSTRAINT tb_role_master_pkey PRIMARY KEY (role_id);


--
-- Name: tb_role_permission tb_role_permission_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_role_permission
    ADD CONSTRAINT tb_role_permission_pkey PRIMARY KEY (role_id, module_id);


--
-- Name: tb_screen_module tb_screen_module_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_screen_module
    ADD CONSTRAINT tb_screen_module_pkey PRIMARY KEY (id);


--
-- Name: tb_secondary_customer_contact tb_secondary_customer_contact_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_secondary_customer_contact
    ADD CONSTRAINT tb_secondary_customer_contact_pkey PRIMARY KEY (id);


--
-- Name: tb_userAddonLanguageInfo tb_userAddonLanguageInfo_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public."tb_userAddonLanguageInfo"
    ADD CONSTRAINT "tb_userAddonLanguageInfo_pkey" PRIMARY KEY (aid);


--
-- Name: tb_user_onboard_setting_flow tb_user_onboard_setting_flow_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_user_onboard_setting_flow
    ADD CONSTRAINT tb_user_onboard_setting_flow_pkey PRIMARY KEY (email_id);


--
-- Name: tb_voice_mail_msg tb_voice_mail_msg_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_voice_mail_msg
    ADD CONSTRAINT tb_voice_mail_msg_pkey PRIMARY KEY (voice_mail_url_id);


--
-- Name: tb_crm_product_details unique_domain_id; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_crm_product_details
    ADD CONSTRAINT unique_domain_id UNIQUE (domain_id);


--
-- Name: chatbotpaymentintegration uq_company_payment; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.chatbotpaymentintegration
    ADD CONSTRAINT uq_company_payment UNIQUE (companyid, paymenttype);


--
-- Name: tb_detailed_product_info uq_product_domain; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_detailed_product_info
    ADD CONSTRAINT uq_product_domain UNIQUE (fk_product_id, domain_id);


--
-- Name: tb_User uq_user_id; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public."tb_User"
    ADD CONSTRAINT uq_user_id UNIQUE ("UserID");


--
-- Name: wc_master_revert_status_details wc_master_revert_status_details_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.wc_master_revert_status_details
    ADD CONSTRAINT wc_master_revert_status_details_pkey PRIMARY KEY ("campaignchatId");


--
-- Name: FK_TypeCode_TypeGroupID; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX "FK_TypeCode_TypeGroupID" ON public."TypeCode" USING btree ("TypeGroupID");


--
-- Name: IND_DI; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX "IND_DI" ON public.customer_contact USING btree ("domainId");


--
-- Name: IX_CallHistory_DomainIdSessionId; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX "IX_CallHistory_DomainIdSessionId" ON public."tb_CallHistory" USING btree ("domainId", "sessionId");


--
-- Name: fk_CallDurationHistory_CallHistoryId; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX "fk_CallDurationHistory_CallHistoryId" ON public."tb_CallDurationHistory" USING btree ("CallHistoryId");


--
-- Name: fk_custom_hours_cid; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX fk_custom_hours_cid ON public.tb_business_hours_json_info USING btree ("fk_businessId");


--
-- Name: fk_tb_business_holidays_fk_businessid; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX fk_tb_business_holidays_fk_businessid ON public."tb_Business_Holidays" USING btree ("fk_businessId");


--
-- Name: fk_tb_customer_contact_cust_id; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX fk_tb_customer_contact_cust_id ON public.tb_secondary_customer_contact USING btree (fk_customer_id);


--
-- Name: fk_user_statusid; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX fk_user_statusid ON public."tb_User" USING btree ("StatusID");


--
-- Name: idx_CustomerId; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX "idx_CustomerId" ON public."tb_CallHistory" USING btree ("CustomerId");


--
-- Name: idx_agent_status_1; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_agent_status_1 ON public.agent_status USING btree ("domainId");


--
-- Name: idx_agent_status_2; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_agent_status_2 ON public.agent_status USING btree (aid, "domainId");


--
-- Name: idx_agent_status_3; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_agent_status_3 ON public.agent_status USING btree ("statusName", "domainId");


--
-- Name: idx_agent_status_domainid; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_agent_status_domainid ON public.agent_status USING btree ("domainId");


--
-- Name: idx_agent_status_domainid_statusname; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_agent_status_domainid_statusname ON public.agent_status USING btree ("domainId", "statusName");


--
-- Name: idx_agent_status_statusname; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_agent_status_statusname ON public.agent_status USING btree ("statusName");


--
-- Name: idx_business_holidays_fkbusinessid; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_business_holidays_fkbusinessid ON public."tb_Business_Holidays" USING btree ("fk_businessId");


--
-- Name: idx_calldur_call_agent_bot; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_calldur_call_agent_bot ON public."tb_CallDurationHistory" USING btree ("CallHistoryId", "AgentId", "IsBot");


--
-- Name: idx_calledNumber; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX "idx_calledNumber" ON public."tb_CallHistory" USING btree ("calledNumber");


--
-- Name: idx_callhist_domain_time_inbound; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_callhist_domain_time_inbound ON public."tb_CallHistory" USING btree ("domainId", "sessionStartTime", "IsInBound");


--
-- Name: idx_callhistory_agent_isbot; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_callhistory_agent_isbot ON public."tb_CallDurationHistory" USING btree ("CallHistoryId", "AgentId", "IsBot");


--
-- Name: idx_callmap_1; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_callmap_1 ON public."callMap" USING btree (cid, "domainId", "sourceId");


--
-- Name: idx_campaign_contacts_campaignid; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_campaign_contacts_campaignid ON public.ec_campaign_contacts_dtl USING btree ("campaignId");


--
-- Name: idx_campaign_sent_stop; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_campaign_sent_stop ON public.ec_campaign_contacts_dtl USING btree ("campaignId", "isSent", "isStop");


--
-- Name: idx_custom_hours_1; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_custom_hours_1 ON public.custom_hours USING btree (cid, "domainId");


--
-- Name: idx_custom_hours_2; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_custom_hours_2 ON public.custom_hours USING btree (cid, "domainId", name);


--
-- Name: idx_custom_hours_domainid; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_custom_hours_domainid ON public.custom_hours USING btree ("domainId");


--
-- Name: idx_customerNumber; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX "idx_customerNumber" ON public."tb_CallHistory" USING btree ("customerNumber");


--
-- Name: idx_customer_contact_1; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_customer_contact_1 ON public.customer_contact USING btree (coid, "domainId");


--
-- Name: idx_customer_contact_3; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_customer_contact_3 ON public.customer_contact USING btree ("blockCustomer", "domainId");


--
-- Name: idx_customer_contact_4; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_customer_contact_4 ON public.customer_contact USING btree ("doNotDisturb", "domainId");


--
-- Name: idx_customer_contact_5; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_customer_contact_5 ON public.customer_contact USING btree ("primaryPhoneNumber", "domainId");


--
-- Name: idx_customer_contact_domainid_primaryemail; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_customer_contact_domainid_primaryemail ON public.customer_contact USING btree ("domainId", "primaryEmail");


--
-- Name: idx_customer_contact_domainid_primaryphone; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_customer_contact_domainid_primaryphone ON public.customer_contact USING btree ("domainId", "primaryPhoneNumber");


--
-- Name: idx_customer_contact_email; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_customer_contact_email ON public.tb_customer_contact USING btree (primary_email);


--
-- Name: idx_customer_contact_lead_id; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_customer_contact_lead_id ON public.tb_customer_contact USING btree (sales_agent_id);


--
-- Name: idx_did_purchase_1; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_did_purchase_1 ON public.did_purchase USING btree (pid, user_id, "domainId");


--
-- Name: idx_did_purchase_domainid_sms; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_did_purchase_domainid_sms ON public.did_purchase USING btree ("domainId", "isSmsIn", "isSmsOut");


--
-- Name: idx_disposition_status_1; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_disposition_status_1 ON public.disposition_status USING btree (did, "domainId");


--
-- Name: idx_disposition_status_2; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_disposition_status_2 ON public.disposition_status USING btree (did, "domainId", "dispositionName");


--
-- Name: idx_disposition_status_domainid; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_disposition_status_domainid ON public.disposition_status USING btree ("domainId");


--
-- Name: idx_domainid; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_domainid ON public.queue USING btree ("domainId");


--
-- Name: idx_email_campaign_master_dtl_campaignchatid_domainid; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_email_campaign_master_dtl_campaignchatid_domainid ON public.ec_email_campaign_master_dtl USING btree (campaignchatid, "domainId");


--
-- Name: idx_email_config_domainid; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_email_config_domainid ON public.email_config USING btree ("domainId");


--
-- Name: idx_product_domain; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_product_domain ON public.tb_product_master USING btree (domain_id, product_id);


--
-- Name: idx_product_insights_domain; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_product_insights_domain ON public.tb_product_insights_info USING btree (domain_id);


--
-- Name: idx_prompt_library_1; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_prompt_library_1 ON public.prompt_library USING btree (uuid, "domainId");


--
-- Name: idx_qid; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_qid ON public.queue USING btree (qid);


--
-- Name: idx_queue_1; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_queue_1 ON public.queue USING btree (qid, "domainId");


--
-- Name: idx_queue_2; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_queue_2 ON public.queue USING btree ("domainId", name);


--
-- Name: idx_queue_3; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_queue_3 ON public.queue USING btree ("domainId", "hoursOfOperatioId");


--
-- Name: idx_queue_4; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_queue_4 ON public.queue USING btree ("domainId", name);


--
-- Name: idx_queue_domainid_qid; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_queue_domainid_qid ON public.queue USING btree ("domainId", qid);


--
-- Name: idx_revert_status_domainid_chatid; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_revert_status_domainid_chatid ON public.wc_master_revert_status_details USING btree ("domainId", "campaignchatId");


--
-- Name: idx_routing_profile_1; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_routing_profile_1 ON public.routing_profile USING btree (rid, "domainId");


--
-- Name: idx_routing_profile_2; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_routing_profile_2 ON public.routing_profile USING btree (rid, "domainId", name);


--
-- Name: idx_routing_profile_domainid_rid; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_routing_profile_domainid_rid ON public.routing_profile USING btree ("domainId", rid);


--
-- Name: idx_routingqueueprofilemapping_1; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_routingqueueprofilemapping_1 ON public.routingqueueprofilemapping USING btree (rqid, "domainId");


--
-- Name: idx_routingqueueprofilemapping_2; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_routingqueueprofilemapping_2 ON public.routingqueueprofilemapping USING btree (rqid, "domainId", rqmid);


--
-- Name: idx_routingqueueprofilemapping_domainid_qid; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_routingqueueprofilemapping_domainid_qid ON public.routingqueueprofilemapping USING btree ("domainId", qid);


--
-- Name: idx_sessionEndTime; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX "idx_sessionEndTime" ON public."tb_CallHistory" USING btree ("sessionEndTime");


--
-- Name: idx_sessionId; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX "idx_sessionId" ON public."tb_CallHistory" USING btree ("sessionId");


--
-- Name: idx_skillmapping_1; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_skillmapping_1 ON public."skillMapping" USING btree (skid, "domainId");


--
-- Name: idx_skillmapping_usid_domainid; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_skillmapping_usid_domainid ON public."skillMapping" USING btree (usid, "domainId");


--
-- Name: idx_skills_1; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_skills_1 ON public.skills USING btree (sid, "domainId");


--
-- Name: idx_skills_2; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_skills_2 ON public.skills USING btree (sid, "domainId", skill_name);


--
-- Name: idx_tb_CallHistory_domainId01; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX "idx_tb_CallHistory_domainId01" ON public."tb_CallHistory" USING btree ("domainId", "callBack", "sessionStartTime", "calledNumber");


--
-- Name: idx_tb_CallHistory_domainId02; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX "idx_tb_CallHistory_domainId02" ON public."tb_CallHistory" USING btree ("domainId", "voicemailUrl", "isVMCallBackCompleted", "sessionStartTime", "calledNumber");


--
-- Name: idx_tb_calldurationhistory_agentid; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_tb_calldurationhistory_agentid ON public."tb_CallDurationHistory" USING btree ("AgentId");


--
-- Name: idx_tb_calldurationhistory_callhistoryid_agentid; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_tb_calldurationhistory_callhistoryid_agentid ON public."tb_CallDurationHistory" USING btree ("CallHistoryId", "AgentId");


--
-- Name: idx_tb_callhistory_customernumber; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_tb_callhistory_customernumber ON public."tb_CallHistory" USING btree ("customerNumber");


--
-- Name: idx_tb_callhistory_domainid_createdat; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_tb_callhistory_domainid_createdat ON public."tb_CallHistory" USING btree ("domainId", "createdAt" DESC);


--
-- Name: idx_tb_callhistory_domainid_sessionid; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_tb_callhistory_domainid_sessionid ON public."tb_CallHistory" USING btree ("domainId", "sessionId");


--
-- Name: idx_tb_callhistory_inbound; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_tb_callhistory_inbound ON public."tb_CallHistory" USING btree ("domainId", "sessionStartTime") WHERE ("IsInBound" = 1);


--
-- Name: idx_tb_callmessage_callhistoryid; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_tb_callmessage_callhistoryid ON public."tb_CallMessage" USING btree ("CallHistoryId");


--
-- Name: idx_tb_emailhistory_01; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_tb_emailhistory_01 ON public."tb_EmailHistory" USING btree ("DomainId", "SessionStartTime", "IsSessionClosed");


--
-- Name: idx_tb_user_active_voice; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_tb_user_active_voice ON public."tb_User" USING btree ("domainId", "UserID") WHERE (("isActive" = 1) AND ("isVoice" = 1));


--
-- Name: idx_tb_user_domainid_ext; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_tb_user_domainid_ext ON public."tb_User" USING btree ("domainId", ext);


--
-- Name: idx_tb_user_domainid_isactive_roleid; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_tb_user_domainid_isactive_roleid ON public."tb_User" USING btree ("domainId", "isActive", roleid);


--
-- Name: idx_tb_user_emailid; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_tb_user_emailid ON public."tb_User" USING btree ("emailId");


--
-- Name: idx_tb_user_emailid_domainid; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_tb_user_emailid_domainid ON public."tb_User" USING btree ("emailId", "domainId");


--
-- Name: idx_tb_user_user; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_tb_user_user ON public."tb_User" USING btree ("UserID");


--
-- Name: idx_tb_user_userid; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_tb_user_userid ON public."tb_User" USING btree ("UserID");


--
-- Name: idx_type_1_fulltext; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_type_1_fulltext ON public.disposition_status USING gin (to_tsvector('english'::regconfig, (((COALESCE(type, ''::character varying))::text || ' '::text) || (COALESCE("dispositionName", ''::character varying))::text)));


--
-- Name: idx_useraddon_domainid_userid; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_useraddon_domainid_userid ON public."tb_userAddonLanguageInfo" USING btree ("domainId", "userId");


--
-- Name: index_cust_domain_pro; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX index_cust_domain_pro ON public.tb_projects_dtl USING btree (customer_id, domain_id, product_ref_id);


--
-- Name: index_fk_campaignid; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX index_fk_campaignid ON public.ec_email_campaign_master_dtl_exclude_email USING btree ("fk_campaignId");


--
-- Name: ix_CallHistory_CHdomainIdSTETCN; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX "ix_CallHistory_CHdomainIdSTETCN" ON public."tb_CallHistory" USING btree ("CallHistoryId", "domainId", "sessionStartTime", "sessionEndTime", "customerNumber");


--
-- Name: ix_CallHistory_CallHistoryId1; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX "ix_CallHistory_CallHistoryId1" ON public."tb_CallHistory" USING btree ("domainId", "createdAt");


--
-- Name: ix_CallHistory_createdAt; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX "ix_CallHistory_createdAt" ON public."tb_CallHistory" USING btree ("createdAt", "domainId", "sessionEndTime");


--
-- Name: ix_CallHistory_sessionStartTime1; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX "ix_CallHistory_sessionStartTime1" ON public."tb_CallHistory" USING btree ("domainId", "sessionStartTime");


--
-- Name: ix_did_purchase_domainid; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX ix_did_purchase_domainid ON public.did_purchase USING btree ("domainId");


--
-- Name: ix_did_purchase_domainid_your_number; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX ix_did_purchase_domainid_your_number ON public.did_purchase USING btree ("domainId", your_number);


--
-- Name: ix_user_domainid; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX ix_user_domainid ON public."tb_User" USING btree (uid, "domainId");


--
-- Name: ix_user_domainid_ext; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX ix_user_domainid_ext ON public."tb_User" USING btree ("domainId", ext, uid, "currentSession");


--
-- Name: tb_CallDurationHistory fk_CallDurationHistory_CallHistoryId_fk; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public."tb_CallDurationHistory"
    ADD CONSTRAINT "fk_CallDurationHistory_CallHistoryId_fk" FOREIGN KEY ("CallHistoryId") REFERENCES public."tb_CallHistory"("CallHistoryId");


--
-- Name: tb_VideoDurationHistory fk_VideoDurationHistory_VideoHistoryId; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public."tb_VideoDurationHistory"
    ADD CONSTRAINT "fk_VideoDurationHistory_VideoHistoryId" FOREIGN KEY ("VideoHistoryId") REFERENCES public."tb_VideoHistory"("VideoHistoryId");


--
-- Name: tb_business_hours_json_info fk_custom_hours_cid; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_business_hours_json_info
    ADD CONSTRAINT fk_custom_hours_cid FOREIGN KEY ("fk_businessId") REFERENCES public.custom_hours(cid) ON DELETE CASCADE;


--
-- Name: tb_customer_contact fk_customer_contact_user; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_customer_contact
    ADD CONSTRAINT fk_customer_contact_user FOREIGN KEY (sales_agent_id) REFERENCES public."tb_User"("UserID") ON UPDATE CASCADE ON DELETE RESTRICT;


--
-- Name: tb_EmailMessage fk_emailmessage_emailhistoryid; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public."tb_EmailMessage"
    ADD CONSTRAINT fk_emailmessage_emailhistoryid FOREIGN KEY ("EmailHistoryId") REFERENCES public."tb_EmailHistory"("EmailHistoryId");


--
-- Name: ec_email_campaign_master_dtl_exclude_email fk_exclude_campaign; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.ec_email_campaign_master_dtl_exclude_email
    ADD CONSTRAINT fk_exclude_campaign FOREIGN KEY ("fk_campaignId") REFERENCES public.ec_email_campaign_master_dtl("campaignId") ON UPDATE CASCADE ON DELETE CASCADE;


--
-- Name: notification_agent fk_notification; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.notification_agent
    ADD CONSTRAINT fk_notification FOREIGN KEY (notification_id) REFERENCES public.notification_master(notification_id) ON DELETE CASCADE;


--
-- Name: tb_queue_permission fk_queue_qid; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_queue_permission
    ADD CONSTRAINT fk_queue_qid FOREIGN KEY (fk_qid) REFERENCES public.queue(qid) ON DELETE CASCADE;


--
-- Name: tb_Business_Holidays fk_tb_business_holidays_fk_businessid; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public."tb_Business_Holidays"
    ADD CONSTRAINT fk_tb_business_holidays_fk_businessid FOREIGN KEY ("fk_businessId") REFERENCES public.custom_hours(cid) ON DELETE CASCADE;


--
-- Name: tb_detailed_product_info fk_tb_product_master_product_id; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_detailed_product_info
    ADD CONSTRAINT fk_tb_product_master_product_id FOREIGN KEY (fk_product_id) REFERENCES public.tb_product_master(product_id);


--
-- Name: tb_projects_dtl fk_tb_projects_master; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_projects_dtl
    ADD CONSTRAINT fk_tb_projects_master FOREIGN KEY (fk_pro_id) REFERENCES public.tb_projects_master(pro_id);


--
-- Name: tb_User fk_user_statusid; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public."tb_User"
    ADD CONSTRAINT fk_user_statusid FOREIGN KEY ("StatusID") REFERENCES public.agent_status(aid);


--
-- Name: tb_role_permission tb_role_permission_access_level_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_role_permission
    ADD CONSTRAINT tb_role_permission_access_level_id_fkey FOREIGN KEY (access_level_id) REFERENCES public.tb_access_level_master(access_level_id);


--
-- Name: tb_role_permission tb_role_permission_module_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_role_permission
    ADD CONSTRAINT tb_role_permission_module_id_fkey FOREIGN KEY (module_id) REFERENCES public.tb_screen_module(id) ON DELETE CASCADE;


--
-- Name: tb_role_permission tb_role_permission_role_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.tb_role_permission
    ADD CONSTRAINT tb_role_permission_role_id_fkey FOREIGN KEY (role_id) REFERENCES public.tb_role_master(role_id) ON DELETE CASCADE;


--
-- Name: SCHEMA public; Type: ACL; Schema: -; Owner: pg_database_owner
--

GRANT USAGE ON SCHEMA public TO dbdev;


--
-- Name: TABLE "TypeCode"; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public."TypeCode" TO dbdev;


--
-- Name: TABLE agent_status; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.agent_status TO dbdev;


--
-- Name: TABLE "callFlow"; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public."callFlow" TO dbdev;


--
-- Name: TABLE "callMap"; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public."callMap" TO dbdev;


--
-- Name: TABLE call_configuration; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.call_configuration TO dbdev;


--
-- Name: TABLE callsessionstartendlog; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.callsessionstartendlog TO dbdev;


--
-- Name: SEQUENCE callsessionstartendlog_id_seq; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON SEQUENCE public.callsessionstartendlog_id_seq TO dbdev;


--
-- Name: TABLE ccaas_settings_voice_video_recording_dtl; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.ccaas_settings_voice_video_recording_dtl TO dbdev;


--
-- Name: TABLE chatbotpaymentintegration; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.chatbotpaymentintegration TO dbdev;


--
-- Name: SEQUENCE chatbotpaymentintegration_tokenid_seq; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON SEQUENCE public.chatbotpaymentintegration_tokenid_seq TO dbdev;


--
-- Name: TABLE custom_hours; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.custom_hours TO dbdev;


--
-- Name: TABLE customer_contact; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.customer_contact TO dbdev;


--
-- Name: SEQUENCE customer_contact_coid_seq; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON SEQUENCE public.customer_contact_coid_seq TO dbdev;


--
-- Name: TABLE did_purchase; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.did_purchase TO dbdev;


--
-- Name: TABLE disposition_status; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.disposition_status TO dbdev;


--
-- Name: SEQUENCE disposition_status_did_seq; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON SEQUENCE public.disposition_status_did_seq TO dbdev;


--
-- Name: TABLE ec_campaign_contacts_dtl; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.ec_campaign_contacts_dtl TO dbdev;


--
-- Name: TABLE "ec_email_campaign_AB_testing_dtl"; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public."ec_email_campaign_AB_testing_dtl" TO dbdev;


--
-- Name: TABLE ec_email_campaign_master_dtl; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.ec_email_campaign_master_dtl TO dbdev;


--
-- Name: TABLE ec_email_campaign_master_dtl_exclude_email; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.ec_email_campaign_master_dtl_exclude_email TO dbdev;


--
-- Name: TABLE ec_email_failed_contact_dtl; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.ec_email_failed_contact_dtl TO dbdev;


--
-- Name: SEQUENCE ec_email_failed_contact_dtl_id_seq; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON SEQUENCE public.ec_email_failed_contact_dtl_id_seq TO dbdev;


--
-- Name: TABLE email_config; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.email_config TO dbdev;


--
-- Name: TABLE email_tb_indiv_admin_action_dtl; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.email_tb_indiv_admin_action_dtl TO dbdev;


--
-- Name: SEQUENCE email_tb_indiv_admin_action_dtl_id_seq; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON SEQUENCE public.email_tb_indiv_admin_action_dtl_id_seq TO dbdev;


--
-- Name: TABLE email_tb_indiv_user_click_open_cnt_dtl; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.email_tb_indiv_user_click_open_cnt_dtl TO dbdev;


--
-- Name: SEQUENCE email_tb_indiv_user_click_open_cnt_dtl_id_seq; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON SEQUENCE public.email_tb_indiv_user_click_open_cnt_dtl_id_seq TO dbdev;


--
-- Name: TABLE notification_agent; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.notification_agent TO dbdev;


--
-- Name: TABLE notification_master; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.notification_master TO dbdev;


--
-- Name: SEQUENCE notification_master_notification_id_seq; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON SEQUENCE public.notification_master_notification_id_seq TO dbdev;


--
-- Name: TABLE prompt_library; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.prompt_library TO dbdev;


--
-- Name: TABLE queue; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.queue TO dbdev;


--
-- Name: TABLE "queueWeighTime"; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public."queueWeighTime" TO dbdev;


--
-- Name: TABLE routing_profile; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.routing_profile TO dbdev;


--
-- Name: SEQUENCE routing_profile_rid_seq; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON SEQUENCE public.routing_profile_rid_seq TO dbdev;


--
-- Name: TABLE routingqueueprofilemapping; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.routingqueueprofilemapping TO dbdev;


--
-- Name: SEQUENCE routingqueueprofilemapping_rqid_seq; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON SEQUENCE public.routingqueueprofilemapping_rqid_seq TO dbdev;


--
-- Name: TABLE sc_campaign_master_dtl; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.sc_campaign_master_dtl TO dbdev;


--
-- Name: TABLE sc_campaign_master_schedule_dtl; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.sc_campaign_master_schedule_dtl TO dbdev;


--
-- Name: TABLE sc_sms_sent_status_details; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.sc_sms_sent_status_details TO dbdev;


--
-- Name: SEQUENCE sc_sms_sent_status_details_id_seq; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON SEQUENCE public.sc_sms_sent_status_details_id_seq TO dbdev;


--
-- Name: TABLE sc_smsconfigdtl; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.sc_smsconfigdtl TO dbdev;


--
-- Name: SEQUENCE sc_smsconfigdtl_id_seq; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON SEQUENCE public.sc_smsconfigdtl_id_seq TO dbdev;


--
-- Name: TABLE "skillMapping"; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public."skillMapping" TO dbdev;


--
-- Name: TABLE skills; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.skills TO dbdev;


--
-- Name: TABLE sms_tb_indiv_user_click_open_cnt_dtl; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.sms_tb_indiv_user_click_open_cnt_dtl TO dbdev;


--
-- Name: SEQUENCE sms_tb_indiv_user_click_open_cnt_dtl_id_seq; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON SEQUENCE public.sms_tb_indiv_user_click_open_cnt_dtl_id_seq TO dbdev;


--
-- Name: TABLE "tb_AgentBusinessHour"; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public."tb_AgentBusinessHour" TO dbdev;


--
-- Name: TABLE "tb_Business_Holidays"; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public."tb_Business_Holidays" TO dbdev;


--
-- Name: TABLE "tb_CallDurationHistory"; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public."tb_CallDurationHistory" TO dbdev;


--
-- Name: SEQUENCE "tb_CallDurationHistory_CallDurationHistoryId_seq"; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON SEQUENCE public."tb_CallDurationHistory_CallDurationHistoryId_seq" TO dbdev;


--
-- Name: TABLE "tb_CallHistory"; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public."tb_CallHistory" TO dbdev;


--
-- Name: SEQUENCE "tb_CallHistory_CallHistoryId_seq"; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON SEQUENCE public."tb_CallHistory_CallHistoryId_seq" TO dbdev;


--
-- Name: TABLE "tb_CallMessage"; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public."tb_CallMessage" TO dbdev;


--
-- Name: TABLE "tb_EmailHistory"; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public."tb_EmailHistory" TO dbdev;


--
-- Name: TABLE "tb_EmailMessage"; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public."tb_EmailMessage" TO dbdev;


--
-- Name: TABLE "tb_User"; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public."tb_User" TO dbdev;


--
-- Name: TABLE "tb_VideoDurationHistory"; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public."tb_VideoDurationHistory" TO dbdev;


--
-- Name: SEQUENCE "tb_VideoDurationHistory_VideoDurationHistoryId_seq"; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON SEQUENCE public."tb_VideoDurationHistory_VideoDurationHistoryId_seq" TO dbdev;


--
-- Name: TABLE "tb_VideoHistory"; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public."tb_VideoHistory" TO dbdev;


--
-- Name: SEQUENCE "tb_VideoHistory_VideoHistoryId_seq"; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON SEQUENCE public."tb_VideoHistory_VideoHistoryId_seq" TO dbdev;


--
-- Name: TABLE "tb_VideoMessage"; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public."tb_VideoMessage" TO dbdev;


--
-- Name: TABLE tb_access_level_master; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.tb_access_level_master TO dbdev;


--
-- Name: TABLE tb_business_hours_json_info; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.tb_business_hours_json_info TO dbdev;


--
-- Name: TABLE tb_camapign_template_conversation_dtl; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.tb_camapign_template_conversation_dtl TO dbdev;


--
-- Name: SEQUENCE tb_camapign_template_conversation_dtl_id_seq; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON SEQUENCE public.tb_camapign_template_conversation_dtl_id_seq TO dbdev;


--
-- Name: TABLE tb_campaign_approval_details; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.tb_campaign_approval_details TO dbdev;


--
-- Name: SEQUENCE tb_campaign_approval_details_id_seq; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON SEQUENCE public.tb_campaign_approval_details_id_seq TO dbdev;


--
-- Name: TABLE tb_campaign_open_click_rate_history_dtl; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.tb_campaign_open_click_rate_history_dtl TO dbdev;


--
-- Name: TABLE "tb_ccaas_customerSatisfactionRatingDesign"; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public."tb_ccaas_customerSatisfactionRatingDesign" TO dbdev;


--
-- Name: SEQUENCE tb_company_dtl_comp_id_seq; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON SEQUENCE public.tb_company_dtl_comp_id_seq TO dbdev;


--
-- Name: TABLE tb_company_dtl; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.tb_company_dtl TO dbdev;


--
-- Name: TABLE tb_crm_product_details; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.tb_crm_product_details TO dbdev;


--
-- Name: SEQUENCE tb_crm_product_details_id_seq; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON SEQUENCE public.tb_crm_product_details_id_seq TO dbdev;


--
-- Name: TABLE tb_customer_contact; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.tb_customer_contact TO dbdev;


--
-- Name: TABLE tb_deal_master; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.tb_deal_master TO dbdev;


--
-- Name: SEQUENCE tb_deal_master_deal_id_seq; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON SEQUENCE public.tb_deal_master_deal_id_seq TO dbdev;


--
-- Name: TABLE tb_deal_master_log; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.tb_deal_master_log TO dbdev;


--
-- Name: SEQUENCE tb_deal_master_log_deal_log_id_seq; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON SEQUENCE public.tb_deal_master_log_deal_log_id_seq TO dbdev;


--
-- Name: TABLE tb_detailed_product_info; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.tb_detailed_product_info TO dbdev;


--
-- Name: SEQUENCE tb_detailed_product_info_p_id_seq; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON SEQUENCE public.tb_detailed_product_info_p_id_seq TO dbdev;


--
-- Name: TABLE tb_email_config_dtl; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.tb_email_config_dtl TO dbdev;


--
-- Name: TABLE tb_interaction_notes_info; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.tb_interaction_notes_info TO dbdev;


--
-- Name: SEQUENCE tb_interaction_notes_info_nid_seq; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON SEQUENCE public.tb_interaction_notes_info_nid_seq TO dbdev;


--
-- Name: TABLE "tb_mapIntentQueue"; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public."tb_mapIntentQueue" TO dbdev;


--
-- Name: TABLE tb_onboard_dtl; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.tb_onboard_dtl TO dbdev;


--
-- Name: TABLE tb_product_insights_info; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.tb_product_insights_info TO dbdev;


--
-- Name: SEQUENCE tb_product_insights_info_insight_id_seq; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON SEQUENCE public.tb_product_insights_info_insight_id_seq TO dbdev;


--
-- Name: TABLE tb_product_master; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.tb_product_master TO dbdev;


--
-- Name: SEQUENCE tb_product_master_product_id_seq; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON SEQUENCE public.tb_product_master_product_id_seq TO dbdev;


--
-- Name: TABLE tb_project_insights; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.tb_project_insights TO dbdev;


--
-- Name: SEQUENCE tb_project_insights_pid_seq; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON SEQUENCE public.tb_project_insights_pid_seq TO dbdev;


--
-- Name: TABLE tb_projects_dtl; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.tb_projects_dtl TO dbdev;


--
-- Name: SEQUENCE tb_projects_dtl_pid_seq; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON SEQUENCE public.tb_projects_dtl_pid_seq TO dbdev;


--
-- Name: TABLE tb_projects_master; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.tb_projects_master TO dbdev;


--
-- Name: SEQUENCE tb_projects_master_pro_id_seq; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON SEQUENCE public.tb_projects_master_pro_id_seq TO dbdev;


--
-- Name: TABLE tb_queue_permission; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.tb_queue_permission TO dbdev;


--
-- Name: TABLE tb_role_master; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.tb_role_master TO dbdev;


--
-- Name: SEQUENCE tb_role_master_role_id_seq; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON SEQUENCE public.tb_role_master_role_id_seq TO dbdev;


--
-- Name: TABLE tb_role_permission; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.tb_role_permission TO dbdev;


--
-- Name: TABLE tb_screen_module; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.tb_screen_module TO dbdev;


--
-- Name: SEQUENCE tb_screen_module_id_seq; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON SEQUENCE public.tb_screen_module_id_seq TO dbdev;


--
-- Name: TABLE tb_secondary_customer_contact; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.tb_secondary_customer_contact TO dbdev;


--
-- Name: TABLE "tb_userAddonLanguageInfo"; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public."tb_userAddonLanguageInfo" TO dbdev;


--
-- Name: TABLE tb_user_onboard_setting_flow; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.tb_user_onboard_setting_flow TO dbdev;


--
-- Name: TABLE tb_voice_mail_msg; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.tb_voice_mail_msg TO dbdev;


--
-- Name: SEQUENCE tb_voice_mail_msg_voice_mail_url_id_seq; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON SEQUENCE public.tb_voice_mail_msg_voice_mail_url_id_seq TO dbdev;


--
-- Name: TABLE wc_master_revert_status_details; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.wc_master_revert_status_details TO dbdev;


--
-- Name: DEFAULT PRIVILEGES FOR SEQUENCES; Type: DEFAULT ACL; Schema: public; Owner: postgres
--

ALTER DEFAULT PRIVILEGES FOR ROLE postgres IN SCHEMA public GRANT ALL ON SEQUENCES TO dbdev;


--
-- Name: DEFAULT PRIVILEGES FOR TABLES; Type: DEFAULT ACL; Schema: public; Owner: postgres
--

ALTER DEFAULT PRIVILEGES FOR ROLE postgres IN SCHEMA public GRANT ALL ON TABLES TO dbdev;


--
-- PostgreSQL database dump complete
--

\unrestrict uXHcZpsu8GT8zYE7ei1mvLy4RDivoyPnqm40C3GuJleezx8kvw006LrSk5uV9Rh

