import CryptoJS from 'crypto-js';
const algorithm = 'aes-256-ctr';
const secretKey = 'vOVH6sdmpNWjRRIqCc7rdxs01lwHzfr3';

export const encrypt = (text : any) => {
  const cipher = CryptoJS.AES.encrypt(text, secretKey).toString();
  return {
    content: cipher,
  };
};

export const decrypt = (hash : any) => {
  const decrypttext: any = CryptoJS.AES.encrypt(hash, secretKey);
  const decryptedData = decrypttext.toString(CryptoJS.enc.Utf8);

  return decryptedData;
};
