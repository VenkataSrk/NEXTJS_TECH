import { useEffect, useState } from "react";
import styles from "./likes.module.scss";
import audioIcon from "./likes/audio.svg";
import audioStop from "./likes/audiostop.svg";
import dislikeIcon from "./likes/dislike.svg";
import dislikeActive from "./likes/dislikeactive.svg";
import likeIcon from "./likes/like.svg";
import likeActive from "./likes/likeactive.svg";
import shareClose from "./likes/shareclose.svg";
import shareTick from "./likes/sharetick.svg";
import SocketService from "../base/service/socketService";
import { whatsappService } from "../base/service/apiservice";
import { useToast } from "../components/custom_components/useToast";
import BlurToastDemo from "../components/custom_components/blurtast/blurtoast";

export default function ChatLikes({changeindex,allConversation,likeOrDislike,msg,campaignChatId,message} :any) {
    const [isLike, setIsLike] = useState(false);
    const [isDislike, setIsDislike] = useState(false);
    const [isAudio, setIsAudio] = useState(false);
    const [isModalOpen, setIsModalOpen] = useState(false);
    const [isInfoVisible, setIsInfoVisible] = useState(false);
    const [pendingAction, setPendingAction] = useState<"like" | "dislike" | null>(null);
    const [feedBackData,setFeedBackData]= useState<string>("");
    const handleLikeClick = () => {
        if(window.location.href.split('/').pop()=="drafts"){
            alert("❌ Conversation in draft—restore to ‘Like.’");
            return ;
        }
            
       const newMsg :any ={
            ...msg,
            campaignChatId,
            messageId:msg?.messageId,
            feedbackData:''
        }
        setIsAudio(false);
        if(pendingAction=="like"){
            setPendingAction(null);
            setIsLike(false);
            updateConversation({...newMsg,likeOrDislike:''});
        }
        else{
            setPendingAction("like");
            setIsLike(true);
            setIsDislike(false)
            
            updateConversation({...newMsg,likeOrDislike:'like'});
        }
        

    };
    const handleDislikeClick = () => {
        if(window.location.href.split('/').pop()=="drafts"){
            alert("❌ Conversation in draft—restore to ‘DisLike.’");
            return ;
        }
            
        if(pendingAction=="dislike"){
            setPendingAction(null);
            setIsDislike(false)
        }
        else{
            setPendingAction("dislike");
            setIsDislike(true)
            
            setIsLike(false);
        }
        if(!isDislike){
            setIsModalOpen(true);
        }
       
    };
    const handleSubmitFeedback = () => {
        const newMsg :any ={
            ...msg,
            campaignChatId,
            messageId:msg?.messageId,
            likeOrDislike:pendingAction,
            feedbackData:feedBackData
        }
        updateConversation(newMsg)
        setFeedBackData('');
        if (pendingAction === "like") {
            setIsLike(true);
            setIsDislike(false);
          } else if (pendingAction === "dislike") {
            setIsDislike(true);
            setIsLike(false);
          }

        setPendingAction(null);
        setIsModalOpen(false);
        setIsInfoVisible(true);
        setTimeout(() => setIsInfoVisible(false), 3000);
    };
    const handleSpeak=async()=>{
        speechSynthesis.cancel();
        // const voices = speechSynthesis.getVoices();
        speechSynthesis.getVoices().forEach(v => console.log(v.name, v.lang));
        // Try to find a female voice (common ones: Zira, Jenny, Aria)
        // const femaleVoice =voices.find(v => /zira|aria|jenny|susan|female/i.test(v.name)) ||voices.find(v => v.name.toLowerCase().includes("woman")) ||voices[0]; // f
        const stripHTML = (html :any) => new DOMParser().parseFromString(html, "text/html").body.textContent || "";
        const utterance = new SpeechSynthesisUtterance(stripHTML(message));
        utterance.lang = 'en-US';
        // utterance.voice = femaleVoice;
        // utterance.rate = 0.78; // normal speaking rate
        // utterance.pitch = 0.7; 
        await new Promise((resolve) => {
            utterance.onend = resolve;
            utterance.onerror = resolve; // also resolve if an error occurs
            speechSynthesis.speak(utterance);
        });
        setIsAudio(false);
    }

    const handleAudioClick = async() => {
        if (window.location.href.split('/').pop() == "drafts") {
            alert("❌ Conversation in draft—restore to ‘play Audio’");
            return;
        }
            
        if (isAudio) {
            speechSynthesis.cancel();
        } else {
            setIsAudio(!isAudio);
            await handleSpeak();
        }
        
    };

    const handleCloseModal = () => {
         const newMsg :any ={
            ...msg,
            campaignChatId,
            messageId:msg?.messageId,
            likeOrDislike:pendingAction,
            feedbackData:"",
            
        }
        
        setFeedBackData('');
        setIsModalOpen(false);
    };

    const handleFeedBack=(e :any)=>{
        setFeedBackData(e?.target?.value);
    }

    const updateConversation = (updatedMsg :any)=>{
        try {

            let data = {
                campaignChatId: campaignChatId,
                conversation: [
                    ...(allConversation.slice(0, changeindex)),
                    updatedMsg,
                    ...(allConversation.slice(changeindex + 1))]
            }
            

        } catch (err) {
            console.log("err", err)
        }
}   


    return (
        <>
            <div className={styles.likeblock}>
                {!(isDislike || likeOrDislike=="dislike")&&
                <div className={styles.likeblockinner}>
                    <div className={styles.liketool}>Like</div>
                    <img src={(isLike || likeOrDislike=="like")? likeActive : likeIcon} alt="like" onClick={handleLikeClick} />
                </div>}
                {!(isLike || likeOrDislike=="like") &&
                <div className={`${styles.dislikes} ${styles.likeblockinner}`}>
                    <div className={styles.liketool}>Dislike</div>
                    <img src={(isDislike || likeOrDislike=="dislike")? dislikeActive : dislikeIcon} alt="dislike" onClick={handleDislikeClick}
                    />
                </div>}

                <div className={`${styles.speaker} ${styles.likeblockinner}`}>
                    <div className={styles.liketool}>{isAudio ? "Speaker Off" : "Speaker On"}</div>
                    <img src={isAudio ? audioStop : audioIcon} alt="audio" onClick={handleAudioClick} />
                </div>
            </div>

            {isModalOpen && (
                <div className={styles.modal}>
                    <div className={styles.modalinner}>
                        <div className={styles.modalheader}>
                            <div className={styles.title}>Share your feeddback</div>
                            <img src={shareClose} alt="" onClick={handleCloseModal} />
                        </div>
                        <div className={styles.modalbody}>
                            <div className={styles.desc}>Your feedback helps make better.</div>
                            <textarea name="" placeholder="Enter feedback" value={feedBackData} onChange={(e:any)=>handleFeedBack(e)}></textarea>
                        </div>
                        <div className={styles.modalfooter}>
                            <button onClick={handleCloseModal}>Skip</button>
                            <button onClick={handleSubmitFeedback}>Submit</button>
                        </div>
                    </div>
                </div>
            )}

            {isInfoVisible && (
                <div className={styles.infoblock}>
                    <div className={styles.infoblockinner}><img src={shareTick} alt="" />Your feedback makes us better</div>
                </div>
            )}
            
        </>
    );
}
