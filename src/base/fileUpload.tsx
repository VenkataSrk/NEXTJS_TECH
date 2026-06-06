// import { EmailServices } from "../../base/service/apiServices";

import { CCAASEmailServices } from "./service/apiservice";
  export const handleFileUpload = async (file: File) => {
        try {
            const formData = new FormData();
            formData.append("doc", file);
            const res = await CCAASEmailServices.post(formData, `/file_upload`);
            if (res.data.statusCode === 200) {
                return res.data.fileList;
            }else{
                return [];
            }
        } catch (error) {
            console.error("File upload failed:", error);
            return [];
        }
    };