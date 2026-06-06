import _ from "underscore";
import { any } from "underscore";
import { uuid } from "uuidv4";
import { worktualMySQl, worktualMySQlCcaas } from "../config";
const closeCaption = require("../model/closecaption-model");
const globalKeyword = require("../model/keyword-model");

export const createClosecaption = async (uuid: any, data: any) => {
  try {
    // console.log("createClosecaption >>>>>>");
    let obj: any = {};
    obj.uuid = uuid;
    obj.transcripts = [];
    return await closeCaption.create(obj);
  } catch (err) {
    throw err;
  }
};

export const storeClosecaption = async (details: any, data: any) => {
   let device_details: any = _.findWhere(details.devicesInfo, {
  conId: data.Connection_Id
});
// console.log("storeClosecaption >>>>>>>", data);
//console.log("details >>>>>>>", details);
try {
  let profile = "";
  if (device_details && device_details.profileImg) { profile = device_details.profileImg }
  let  uuid = data.Meeting_Id;
          let  transcripts = {
             text: data.text,
             convertedText: data.convertedText,
             lang: data.lang,
             speakerName: data.speakerName,
             isHost: data.isHost,
             deviceid: data.deviceid,
             imageURL: profile,
             timeStamp: new Date().toISOString()
           };
          let MOM:any={}; 
          
     worktualMySQl.query('CALL worktual_video_meeting_create_closed_captions (?,?,?)', [
       uuid,
       JSON.stringify(transcripts),
      JSON.stringify(MOM)
      ]
      ,(err: any, response: any) => {
        if(err){
          console.log("create transcript error",err)}
          else{
          console.log("create closecaption sucess response")
          }
        })
} catch (error: any) {
  console.log("Storing close caption error", error);
}
};

export const getSettingVoiceVideoRecordingdao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      worktualMySQlCcaas.query(
        'call ccaas_get_settings_voice_video_recording(?)',
        [data],
        (err: any, result: any) => {
          if (err) {
            console.log('Error:', err);
            reject(err);
          } else {
            if (result[0]?.length > 0) {
              resolve(result[0]);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (err) {
      reject(err);
    }
  });
};


export const storeClosecaptionCcaas = async (details: any, data: any) => {
  console.log("storeClosecaptionCcaas datadatadata>>>",[
    typeof(details.meetingDetails.agentDetails.domain_id) ==="string" ? parseInt(details.meetingDetails.agentDetails.domain_id) : details.meetingDetails.agentDetails.domain_id,
    details.meetingDetails.agentDetails.sessionId,
    JSON.stringify(data),
    typeof(details.meetingDetails.agentDetails.ext) ==="string" ? parseInt(details.meetingDetails.agentDetails.ext) : details.meetingDetails.agentDetails.ext
   ]);


try {

         
    worktualMySQlCcaas.query('call videomeet_create_message_details (?,?,?,?)', [
      typeof(details.meetingDetails.agentDetails.domain_id) ==="string" ? parseInt(details.meetingDetails.agentDetails.domain_id) : details.meetingDetails.agentDetails.domain_id,
      details.meetingDetails.agentDetails.sessionId,
      JSON.stringify(data),
      typeof(details.meetingDetails.agentDetails.ext) ==="string" ? parseInt(details.meetingDetails.agentDetails.ext) : details.meetingDetails.agentDetails.ext
     ]
     ,(err: any, response: any) => {
       if(err){
         console.log("create transcript error",err)}
         else{
         console.log("create closecaption sucess response",response);
         }
       })
} catch (error: any) {
 console.log("Storing close caption error", error);
}
};
export const getTranscripts = (meeting_id: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      closeCaption.findOne({ uuid: meeting_id }).then(async (event: any) => {

        /*   console.log("getTranscripts >>>>>>>", event);
  
          let meet = await meetingFindOne_model({ uuid: meeting_id })
  
          console.log("meet", meet); */
        resolve(event);
      });
    } catch (err) {
      reject(err);
    }
  });
};

export const removeKeywordsMeeting = async (meetingid: any, keyword: any) => {
  await globalKeyword.updateOne({ uuid: meetingid }, { $set: { keywords: keyword } }, { upsert: true })
}



export const addKeywordsMeeting = async (meetingid: any, keyword: any): Promise<any> => {

  try {
    globalKeyword.findOne({ uuid: meetingid }).then(async (key: any) => {

      if (key === null) {

        return await globalKeyword.create(keyword);
      } else {
        await globalKeyword.updateOne({ uuid: meetingid }, { $set: { keywords: keyword } }, { upsert: true })
      }
    }).catch((err: any) => {
      console.log("error", err)
    });
  }
  catch (err) {
    console.log("error", err)
  }

}


export const updateCloseCaption = async (
  uuid: any,
  tid: string,
  text: any
): Promise<any> => {
  return new Promise((resolve, reject) => {
    try {
      closeCaption
        .updateOne(
          { uuid: uuid, "transcripts.tid": tid },
          { $set: { "transcripts.$.text": text } }
        )
        .then((event: any) => {

          resolve(event);
        });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

export const findCloseCaption = async (data: any): Promise<any> => {
  return new Promise((resolve, reject) => {


    try {

      closeCaption
        .aggregate([
          { $unwind: "$transcripts" },
          {
            $match: {
              uuid: data.uuid,
              "transcripts.text": {
                $regex: new RegExp(".*" + data.searchTerm + ".*", "i"),
              },
            },
          },

          //{$group : {_id:{uid:"$transcripts.tid", actualtext: "$transcripts.text", result:{ $regexMatch: { input: "$transcripts.text", regex: /world/i }  }}}}

          // { description: { $regex: /S/ } }

          // { $addFields: { result: { $regexMatch: { input: "$transcripts.text", regex: new RegExp(".*" + search + ".*", "i") } } } }
        ])
        .skip(data.offset)
        .limit(data.limit)
        .then((event: any) => {


          let transArray = [];
          for (let k = 0; k < event.length; k++) {
            transArray.push(event[k].transcripts)
          }



          resolve(transArray);
        });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};


export const filterCloseCaption = async (data: any): Promise<any> => {
  return new Promise((resolve, reject) => {


    try {

      let regexArray = [];
      for (let index = 0; index < data.filterKeys.length; index++) {

        let value = data.filterKeys[index]

        let revalue = new RegExp(".*" + value + ".*", "i")

        regexArray.push(revalue)
      }
      closeCaption
        .aggregate([
          { $unwind: "$transcripts" },
          {
            $match: { uuid: data.uuid, "transcripts.text": { $in: regexArray } },
          },

          //{$group : {_id:{uid:"$transcripts.tid", actualtext: "$transcripts.text", result:{ $regexMatch: { input: "$transcripts.text", regex: /world/i }  }}}}

          // { description: { $regex: /S/ } }

          // { $addFields: { result: { $regexMatch: { input: "$transcripts.text", regex: new RegExp(".*" + search + ".*", "i") } } } }
        ])
        .skip(data.offset)
        .limit(data.limit)
        .then((event: any) => {
          // console.log("event >>>>>>>", event);

          let transArray = [];
          for (let k = 0; k < event.length; k++) {
            transArray.push(event[k].transcripts)
          }

          // console.log("transArray >>>>>>>", transArray);

          resolve(transArray);
        });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};


export const filterNameCloseCaption = async (data: any): Promise<any> => {
  return new Promise((resolve, reject) => {


    try {


      let filterArray: any = data.filterKeys || [];

      let nameArray = data.names || [];


      let nameregexArray: any = [];
      for (let nindex = 0; nindex < nameArray.length; nindex++) {

        let namevalue = nameArray[nindex]

        let renamevalue = new RegExp(".*" + namevalue + ".*", "i")

        nameregexArray.push(renamevalue)
      }


      if (filterArray.length === 0 && nameArray.length === 0) {

        closeCaption
          .aggregate([
            { $unwind: "$transcripts" },
            {
              $match: {
                uuid: data.uuid,
                /*  "transcripts.text": {
                   $regex: new RegExp(".*" + data.searchTerm + ".*", "i"),
                 }, */
              },
            },
          ])
          .skip(data.offset)
          .limit(data.limit)
          .then((event: any) => {
            // console.log("event >>>>>>>", event);

            let transArray = [];
            for (let k = 0; k < event.length; k++) {
              transArray.push(event[k].transcripts)
            }

            // console.log("transArray >>>>>>>", transArray);

            resolve(transArray);
          });

      } else if (filterArray.length === 0) {

        closeCaption
          .aggregate([
            { $unwind: "$transcripts" },
            {
              $match: { uuid: data.uuid, "transcripts.speakerName": { $in: nameregexArray } },
            },


          ])
          .skip(data.offset)
          .limit(data.limit)
          .then((event: any) => {
            // console.log("event >>>>>>>", event);

            let transArray = [];
            for (let k = 0; k < event.length; k++) {
              transArray.push(event[k].transcripts)
            }

            // console.log("transArray >>>>>>>", transArray);

            resolve(transArray);
          });
      } else if (nameArray.length === 0) {

        let regexArray = [];


        for (let index = 0; index < filterArray.length; index++) {

          let value = filterArray[index]

          let revalue = new RegExp(".*" + value + ".*", "i")

          regexArray.push(revalue)
        }


        closeCaption
          .aggregate([
            { $unwind: "$transcripts" },
            {
              $match: { uuid: data.uuid, "transcripts.text": { $in: regexArray } },
            },

          ])
          .skip(data.offset)
          .limit(data.limit)
          .then((event: any) => {
            //console.log("event >>>>>>>", event);

            let transArray = [];
            for (let k = 0; k < event.length; k++) {
              transArray.push(event[k].transcripts)
            }

            //console.log("transArray >>>>>>>", transArray);

            resolve(transArray);
          });

      } else if (filterArray.length > 0 && nameArray.length > 0) {

        let regexArray = [];


        for (let index = 0; index < filterArray.length; index++) {

          let value = filterArray[index]

          let revalue = new RegExp(".*" + value + ".*", "i")

          regexArray.push(revalue)
        }
        closeCaption
          .aggregate([
            { $unwind: "$transcripts" },
            {
              $match: { uuid: data.uuid, "transcripts.speakerName": { $in: nameregexArray }, "transcripts.text": { $in: regexArray } },
            },


          ])
          .skip(data.offset)
          .limit(data.limit)
          .then((event: any) => {
            //console.log("event >>>>>>>", event);

            let transArray = [];
            for (let k = 0; k < event.length; k++) {
              transArray.push(event[k].transcripts)
            }

            ////console.log("transArray >>>>>>>", transArray);

            resolve(transArray);
          });
      } else {

        closeCaption
          .aggregate([
            { $unwind: "$transcripts" },
            {
              $match: {
                uuid: data.uuid,
                /*  "transcripts.text": {
                   $regex: new RegExp(".*" + data.searchTerm + ".*", "i"),
                 }, */
              },
            },
          ])
          .skip(data.offset)
          .limit(data.limit)
          .then((event: any) => {
            //console.log("event >>>>>>>", event);

            let transArray = [];
            for (let k = 0; k < event.length; k++) {
              transArray.push(event[k].transcripts)
            }

            //  console.log("transArray >>>>>>>", transArray);

            resolve(transArray);
          });
      }

    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};