import { useState, useRef } from "react";
import styles from "./scss/likes.module.scss";
import audioIcon from "../../assets/images/likes/audio.svg";
import audioStop from "../../assets/images/likes/audiostop.svg";
import dislikeIcon from "../../assets/images/likes/dislike.svg";
import dislikeActive from "../../assets/images/likes/dislikeactive.svg";
import likeIcon from "../../assets/images/likes/like.svg";
import likeActive from "../../assets/images/likes/likeactive.svg";
import shareClose from "../../assets/images/likes/shareclose.svg";
import shareTick from "../../assets/images/likes/sharetick.svg";

const getVoicesAsync = (): Promise<SpeechSynthesisVoice[]> => {
    return new Promise((resolve) => {
        const voices = speechSynthesis.getVoices();
        if (voices.length > 0) {
            resolve(voices);
            return;
        }
        speechSynthesis.onvoiceschanged = () => {
            resolve(speechSynthesis.getVoices());
        };
    });
};

const detectLanguage = (text: string): string => {
    const langPatterns: { pattern: RegExp; lang: string }[] = [
        { pattern: /[\u0600-\u06FF]/, lang: "ar" },
        { pattern: /[\u0900-\u097F]/, lang: "hi" },
        { pattern: /[\u0980-\u09FF]/, lang: "bn" },
        { pattern: /[\u0A80-\u0AFF]/, lang: "gu" },
        { pattern: /[\u0B80-\u0BFF]/, lang: "ta" },
        { pattern: /[\u0C00-\u0C7F]/, lang: "te" },
        { pattern: /[\u0C80-\u0CFF]/, lang: "kn" },
        { pattern: /[\u0D00-\u0D7F]/, lang: "ml" },
        { pattern: /[\u0A00-\u0A7F]/, lang: "pa" },
        { pattern: /[\u0B00-\u0B7F]/, lang: "or" },
        { pattern: /[\u0E00-\u0E7F]/, lang: "th" },
        { pattern: /[\u3040-\u309F\u30A0-\u30FF]/, lang: "ja" },
        { pattern: /[\u4E00-\u9FFF]/, lang: "zh" },
        { pattern: /[\uAC00-\uD7AF]/, lang: "ko" },
        { pattern: /[\u0400-\u04FF]/, lang: "ru" },
        { pattern: /[àâçéèêëïîôùûüÿœæ]/i, lang: "fr" },
        { pattern: /[äöüß]/i, lang: "de" },
        { pattern: /[ñ¿¡]/i, lang: "es" },
        { pattern: /[ãõçâê]/i, lang: "pt" },
        { pattern: /[àèéìòù]/i, lang: "it" },
    ];

    for (const { pattern, lang } of langPatterns) {
        if (pattern.test(text)) return lang;
    }
    return "en";
};

const findBestVoice = (voices: SpeechSynthesisVoice[], lang: string): SpeechSynthesisVoice | null => {
    // For English, prefer UK accent (en-GB)
    if (lang === "en") {
        const ukVoice = voices.find((v) => v.lang.toLowerCase() === "en-gb");
        if (ukVoice) return ukVoice;
    }
    const exactMatch = voices.find((v) => v.lang.toLowerCase().startsWith(lang.toLowerCase()));
    if (exactMatch) return exactMatch;
    const langBase = lang.split("-")[0];
    const baseMatch = voices.find((v) => v.lang.toLowerCase().startsWith(langBase.toLowerCase()));
    if (baseMatch) return baseMatch;
    return null;
};

export default function ChatLikes({message} : any) {
    const [isLike, setIsLike] = useState(false);
    const [isDislike, setIsDislike] = useState(false);
    const [isAudio, setIsAudio] = useState(false);
    const [isModalOpen, setIsModalOpen] = useState(false);
    const [isInfoVisible, setIsInfoVisible] = useState(false);
    const [pendingAction, setPendingAction] = useState<"like" | "dislike" | null>(null);
    const isSpeakingRef = useRef(false);


    const handleLikeClick = () => {
        // setIsModalOpen(true);
        setIsAudio(false);
        setPendingAction("like");
    };
    const handleDislikeClick = () => {
        // setIsModalOpen(true);
        setPendingAction("dislike");
    };
    const handleSubmitFeedback = () => {
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
    const handleAudioClick = async() => {
         if (isAudio) {
            speechSynthesis.cancel();
            isSpeakingRef.current = false;
            setIsAudio(false);
        } else {
            setIsAudio(true);
            await handleSpeak();
        }
    };
    const handleCloseModal = () => {
        setIsModalOpen(false);
    };

     const handleSpeak=async()=>{
        speechSynthesis.cancel();
        const voices = await getVoicesAsync();
        const stripHTML = (html :any) => new DOMParser().parseFromString(html, "text/html").body.textContent || "";
        const text = stripHTML(message);
        const detectedLang = detectLanguage(text);
        const utterance = new SpeechSynthesisUtterance(text);
        utterance.lang = detectedLang;
        const voice = findBestVoice(voices, detectedLang);
        if (voice) {
            utterance.voice = voice;
        }
        isSpeakingRef.current = true;
        await new Promise((resolve) => {
            utterance.onend = resolve;
            utterance.onerror = resolve;
            speechSynthesis.speak(utterance);
        });
        isSpeakingRef.current = false;
        setIsAudio(false);
    }
    

    return (
        <>
            <div className={`${styles.likeblock} likeblockpadding`}>
                <div className={styles.likeblockinner}>
                    <div className={styles.liketool}>Like</div>
                    <img src={isLike ? likeActive : likeIcon} alt="like" onClick={handleLikeClick} />
                </div>

                <div className={`${styles.dislikes} ${styles.likeblockinner}`}>
                    <div className={styles.liketool}>Dislike</div>
                    <img src={isDislike ? dislikeActive : dislikeIcon} alt="dislike" onClick={handleDislikeClick}
                    />
                </div>

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
                            <textarea name="" placeholder="Enter feedback"></textarea>
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
