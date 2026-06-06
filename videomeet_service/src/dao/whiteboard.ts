const whiteBoard = require('../model/whiteboard-model');


export const storeWhiteBoardMessage = async (uuid: any, data: any) => {
   // console.log("storeWhiteBoardMessage >>>>>>>", data);
    whiteBoard.findOne({
        uuid: uuid
    }, function (err: any, result: any) {
        if (result == null) {

            if (data.points.type == 'startWhiteBoardSharing') {
                data.points = [];
                //console.log("111111111storeWhiteBoardMessage >>>>>>>", result);
                return whiteBoard.create(data);
            }
        } else {
           // console.log("22222222storeWhiteBoardMessage >>>>>>>", result);
            if (data.points.status == 'allDrawing') {
                result.points = [];

            } else if (data.points.status == 'myDrawing') {
                const pointsArr = [];
                for (let i = 0; i < result.points.length; i++) {
                    if (result.points[i].userId != data.points.confId) {
                        pointsArr.push(result.points[i]);
                    }
                }
                result.points = pointsArr;
            } else if (data.points.status == 'otherDrawing') {
                const pointsArr = [];
                for (let i = 0; i < result.points.length; i++) {
                    if (result.points[i].userId == data.points.confId) {

                        pointsArr.push(result.points[i]);
                    }

                }
                result.points = pointsArr;
            } else if (data.points.type == 'undo' || data.points.type == 'redo') {
                const pointsArr = [];

                for (let i = 0; i < result.points.length; i++) {
                    let visible;
                    if (data.points.type == 'undo') {
                        visible = result.points[i].id == data.points.id ? false : true;
                    } else if (data.points.type == 'redo') {
                        visible = result.points[i].id == data.points.id ? true : false;
                    }
                    pointsArr.push({
                        'id': result.points[i].id,
                        'type': result.points[i].type,
                        'xAxis': result.points[i].xAxis,
                        'yAxis': result.points[i].yAxis,
                        'xAxis_1': result.points[i].xAxis_1,
                        'yAxis_1': result.points[i].yAxis_1,
                        'userId': result.points[i].userId,
                        'radius': result.points[i].radius,
                        'action': result.points[i].action,
                        'stoke': result.points[i].stoke,
                        'timestamp': result.points[i].timestamp,
                        'text': result.points[i].text,
                        'fontType': result.points[i].fontType,
                        'fontBold': result.points[i].fontBold,
                        'fontItalic': result.points[i].fontItalic,
                        'thickness': result.points[i].thickness,
                        'color': result.points[i].color,
                        'isVisible': visible
                    });
                }
                result.points = pointsArr;

            } else {
                if (data.points.type != 'startWhiteBoardSharing' && data.points.type != 'stopWhiteBoardSharing') {
                    data.points.isVisible = false;
                    result.points.push(data.points);
                }
            }
            result.save(function (err: any, event: any) {

                //console.log("updatedY..")
                //resolve(event);
            });
        }

    });

}

export const getWhiteBoardMessage = (meeting_id: any) => {


    return new Promise(async (resolve, reject) => {
        try {
            whiteBoard.findOne({ uuid: meeting_id }).then((event: any) => {
                resolve(event);
            });
        } catch (err) {
            reject(err);
        }
    });




}

