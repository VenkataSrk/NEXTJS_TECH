import request from 'request';
import axios from 'axios';
import moment from "moment";
import { configuration } from "../config";

export const contactIdSearch = async (email: any) => {

    try {
        const search = configuration.hubspot.host;
        const key = configuration.hubspot.apikey;
        const options = {
            method: 'POST',
            url: search,
            qs: { hapikey: key },
            headers: {
                accept: 'application/json',
                'content-type': 'application/json',
            },
            body: {
                filterGroups: [
                    {
                        filters: [
                            { value: email, propertyName: 'email', operator: 'EQ' },
                            /*                             {
                                                            value: 'UnifiedRing',
                                                            propertyName: 'brand_name',
                                                            operator: 'EQ',
                                                        }, */
                        ],
                    },
                ],
            },
            json: true,
        };
        return new Promise(async (resolve, reject) => {
            try {
                return request(options, (error: any, response: any, body: any) => {
                    if (error) throw new Error(error);
                    resolve(body.results);
                });
            } catch (err) {
                reject(err);
            }
        });

    } catch (error) {
        console.log("contact id search error", error)
    }

}



export const emailContactSearch = async (auth: any) => {


    let res: any = {}

    console.log("Send hupspot mail calling....");

    return new Promise(async (resolve, reject) => {

        let res: any = {}
        try {



            const url = `https://apigateway.worktual.co.uk/mail/v1/hubspot/hubspot_contact_search`;
            res = await axios.post(url, auth).catch((error: any) => {
                console.log("error axios", error);


                resolve({})
            });

            console.log("res?.data.statusCode >>>>>", res?.data.statusCode);
            console.log("Res >>>>>>", res?.data?.result.results);

            if (res?.data.statusCode) {
                resolve(res?.data?.result?.results);
            } else {
                resolve({})
            }
            /*   if (!res || res.statusCode !== 200) {
                  resolve({})
              }
  
              const data = res.data;
              if (data.statusCode === 404) {
                  resolve({})
              } */



        } catch (err) {
            resolve({})
        }

        //console.log("res.data >>>>", res.data);

    });

}

export const handlePartMailTrigger=async (details: any, authorization: any, meetingDetails: any)=>{
    const emailId: any = [];
    const participantEmailId = details;
    participantEmailId.filter((List: any) => {
        emailId.push(List.email);
    });
    const meetingDate = new Date(meetingDetails.startTime).setUTCHours(0, 0, 0, 0);
    const meetingTime = new Date(meetingDetails.startTime).toISOString()

    let input;
    input={
        meeting_topic:meetingDetails.meetingName,
        meeting_date:moment(meetingDate).format("DD/MM/YYYY"),
        meeting_organizer:authorization.name,
        meeting_time:`${moment(meetingTime).format("h:mm A")} GMT`,
        meeting_password:meetingDetails.meetingPwd ? meetingDetails.meetingPwd : "No password required",
        wt_meeting_url:meetingDetails.inviteUrl,
        meeting_url:meetingDetails.inviteUrl,
        email:emailId.join(","),
        meeting_id:meetingDetails.meetingId,
        meeting_duration:`${meetingDetails.duration} Minutes`
    }
    const result = await mailTrigger(input);
}

export const mailTrigger=(data:any)=>{
    return new Promise(async (resolve, reject) => {
        try {

            return await axios
                .post(`https://mysqlapi.worktual.co.uk/urpricing-mysql/v1/meeting_invite_host`, data)
                .then((response: any) => {

                    // console.log("res >>>", response);
                    resolve(response.status);
                })
                .catch((error: any) => {
                    console.log("send meeting email error", error)
                    reject(error);
                });
        } catch (err) {
            reject(err);
        }
    });
}
export const handleHubspotEmail = async (details: any, authorization: any, meetingDetails: any) => {

    console.log(" <<<<<<<< handleHubspotEmail calling>>>>>>>>>", JSON.stringify(details));
    let autho = {
        "propertyName": "email",
        "operator": "EQ",
        "value": authorization.email
    }

    console.log("authorization >>>>>>", autho);
    let auth_data = await emailContactSearch(autho);

    const emailId: any = [];
    const userInputDetails: any = [];
    const participantEmailId = details;
    participantEmailId.filter((List: any) => {
        emailId.push(List.email);
    });

    console.log("auth_data >>>>>>.", auth_data);
    console.log("emailId >>>>>", emailId);
    for (const userId of emailId) {
        console.log("userId >>>>>", userId);

        let userSearch = {
            "propertyName": "email",
            "operator": "EQ",
            "value": userId
        }


        const hubspotRes: any = await emailContactSearch(userSearch);

        console.log("hubspotRes >>>>>>", hubspotRes);

        const meetingDate = new Date(meetingDetails.startTime).setUTCHours(0, 0, 0, 0);
        const meetingTime = new Date(meetingDetails.startTime).toISOString()

        if (Array.isArray(hubspotRes) && hubspotRes.length > 0) {
            let inputData;

            inputData = {
                vid: hubspotRes[0].id,
                properties: [
                    {
                        "property": "wt_meeting_status",
                        "value": "Scheduled"
                    },
                    {
                        "property": "wt_meeting_topic",
                        "value": meetingDetails.meetingName
                    },
                    {
                        "property": "wt_meeting_date",
                        "value": `${meetingDate}`
                    },
                    {
                        "property": "wt_meeting_organizer",
                        "value": authorization.name
                    },
                    {
                        "property": "wt_meeting_time",
                        "value": `${moment(meetingTime).format("h:mm A")} GMT`
                    },
                    {
                        "property": "wt_meeting_duration",
                        "value": `${meetingDetails.duration} Minutes`
                    },
                    {
                        "property": "wt_meeting_id",
                        "value": meetingDetails.meetingId
                    },
                    {
                        "property": "wt_meeting_password",
                        "value": meetingDetails.meetingPwd
                    },
                    {
                        "property": "wt_meeting_url",
                        "value": meetingDetails.inviteUrl
                    }
                ]
            }
            userInputDetails.push(inputData);
        }

        if (Array.isArray(auth_data) && auth_data.length > 0) {
            const result = await hubSpotSendMail(auth_data[0].id, userInputDetails);
            console.log(result);
        }
    }
}


export const hubSpotSendMail = (conatctId: any, inputData: any) => {

    try {
        let data = {
            method: "POST",
            url: "https://api.hubapi.com/contacts/v1/contact/batch",
            properties: inputData,
        }

        console.log("data", JSON.stringify(data));
        return new Promise(async (resolve, reject) => {
            try {

                return await axios
                    .post(`https://apigateway.worktual.co.uk/mail/v1/hubspot/hubspot_api`, data)
                    .then((response: any) => {

                        // console.log("res >>>", response);
                        resolve(response.status);
                    })
                    .catch((error: any) => {
                        console.log("send meeting email error", error)
                        reject(error);
                    });
            } catch (err) {
                reject(err);
            }
        });

    } catch (error) {
        console.log("hubSpotSendMail ", error)
    }

}



export const particpantEmailTemplate = async (data: any, authorization: any, meetingDetails: any) => {

    try {
        const userInputDetails: any = [];
        const emailId: any = [];
        const participantEmailId = data;
        participantEmailId.filter((List: any) => {
            emailId.push(List.email);
        });


        let participantId = emailId;

        const meetingDate = new Date(meetingDetails.startTime).setHours(0, 0, 0, 0);
        const meetingTime = new Date(meetingDetails.startTime).toISOString()
        //const currentDate = meetingDate + (5 * 60 + 30) * 60000

        const hubspot: any = await contactIdSearch(authorization.email);

        for (const userId of participantId) {
            const hubspotRes: any = await contactIdSearch(userId);

            if (Array.isArray(hubspotRes) && hubspotRes.length > 0) {
                let inputData;

                /*                 inputData = {
                                    properties: {
                                        wt_meeting_organizer: authorization.name,
                                        wt_meeting_topic: meetingDetails.meetingName,
                                        wt_meeting_status: "Scheduled",
                                        wt_meeting_date: `${meetingDate}`,
                                        wt_meeting_time: `${moment(meetingTime).format("h:mm A")} GMT`,
                                        wt_meeting_duration: `${meetingDetails.duration} Minutes`,
                                        wt_meeting_id: meetingDetails.meetingId,
                                        wt_meeting_password: meetingDetails.meetingPwd,
                                        wt_meeting_url: meetingDetails.inviteUrl,
                                    },
                                    id: hubspotRes[0].id,
                                }; */

                inputData = {
                    vid: hubspotRes[0].id,
                    properties: [
                        {
                            "property": "wt_meeting_status",
                            "value": "Scheduled"
                        },
                        {
                            "property": "wt_meeting_topic",
                            "value": meetingDetails.meetingName
                        },
                        {
                            "property": "wt_meeting_date",
                            "value": `${meetingDate}`
                        },
                        {
                            "property": "wt_meeting_organizer",
                            "value": authorization.name
                        },
                        {
                            "property": "wt_meeting_time",
                            "value": `${moment(meetingTime).format("h:mm A")} GMT`
                        },
                        {
                            "property": "wt_meeting_duration",
                            "value": `${meetingDetails.duration} Minutes`
                        },
                        {
                            "property": "wt_meeting_id",
                            "value": meetingDetails.meetingId
                        },
                        {
                            "property": "wt_meeting_password",
                            "value": meetingDetails.meetingPwd
                        },
                        {
                            "property": "wt_meeting_url",
                            "value": meetingDetails.inviteUrl
                        }
                    ]
                }
                userInputDetails.push(inputData);
            }
        }

        console.log("hubspot >>>>>>", hubspot)

        if (Array.isArray(hubspot) && hubspot.length > 0) {
            const result = await hubSpotSendMail(hubspot[0].id, userInputDetails);
            console.log(result);
        }


    } catch (error) {
        console.log("Hubspot error", error)
    }

}



export const hubSpotSendMail1 = (conatctId: any, inputData: any) => {

    try {
        const key = configuration.hubspot.apikey;
        return new Promise(async (resolve, reject) => {
            try {
                console.log("conatctId", conatctId, "key", key, "inputData", inputData)
                return await axios
                    .post(`https://api.hubapi.com/contacts/v1/contact/batch?hapikey=${key}`, inputData)
                    .then((response: any) => {

                        console.log("response", response);
                        resolve(response.status);
                    })
                    .catch((error: any) => {
                        console.log("send meeting email error", error)
                        reject(error);
                    });
            } catch (err) {
                reject(err);
            }
        });

    } catch (error) {
        console.log("hubSpotSendMail ", error)
    }

}