#ifndef __XGATE_SUBSCRIBE_SERVICE_MSG_H__
#define __XGATE_SUBSCRIBE_SERVICE_MSG_H__

class xGateSttInfo
{

  public:
    xGateSttInfo()
    {
      m_roomId = "";
      m_callId = "";
      m_connId = "";
      m_lang = "";
      m_ccState = false;
      m_speaker = "";
      m_isHost = false;
      m_deviceId = "";
      m_imageUrl = "";
      m_joinStatus = "";
      m_speakerLang = "";
    }
    ~xGateSttInfo()
    {
      m_roomId = "";
      m_callId = "";
      m_connId = "";
      m_lang = "";
      m_ccState = false;
      m_speaker = "";
      m_isHost = false;
      m_deviceId = "";
      m_imageUrl = "";
      m_joinStatus = "";
      m_speakerLang = "";
    }

    xGateSttInfo(const xGateSttInfo &rhs)
    {
      m_roomId = rhs.m_roomId.c_str();
      m_callId = rhs.m_callId.c_str();
      m_connId = rhs.m_connId.c_str();
      m_lang = rhs.m_lang.c_str();
      m_ccState = rhs.m_ccState;
      m_speaker = rhs.m_speaker.c_str();
      m_isHost = rhs.m_isHost;
      m_deviceId = rhs.m_deviceId.c_str();
      m_imageUrl = rhs.m_imageUrl.c_str();
      m_joinStatus = rhs.m_joinStatus.c_str();
      m_speakerLang = rhs.m_speakerLang.c_str();
    }

    operator=(const xGateSttInfo &rhs)
    {
      m_roomId = rhs.m_roomId.c_str();
      m_callId = rhs.m_callId.c_str();
      m_connId = rhs.m_connId.c_str();
      m_lang = rhs.m_lang.c_str();
      m_ccState = rhs.m_ccState;
      m_speaker = rhs.m_speaker.c_str();
      m_isHost = rhs.m_isHost;
      m_deviceId = rhs.m_deviceId.c_str();
      m_imageUrl = rhs.m_imageUrl.c_str();
      m_joinStatus = rhs.m_joinStatus.c_str();
      m_speakerLang = rhs.m_speakerLang.c_str();
    }

   void setRoomId(std::string roomid){
      m_roomId = roomid.c_str();
    }
    std::string getRoomId(){
      return m_roomId;
    }

    void setConnectionId(std::string connid){
      m_connId = connid.c_str();
    }
    std::string getConnectionId(){
      return m_connId;
    }

    void setCallId(std::string callid){
      m_callId = callid.c_str();
    }
    std::string getCallId(){
      return m_callId;
    }

    void setLanguage(std::string lang)
    {
      m_lang = lang.c_str();
    }
    std::string getLanguage()
    {
      return m_lang;
    }

    void setCcState(bool state)
    {
      m_ccState = state;
    }
    bool getCcState()
    {
      return m_ccState;
    }

    void setSpeaker(std::string speaker)
    {
      m_speaker = speaker.c_str();
    }
    std::string getSpeaker()
    {
      return m_speaker;
    }

    void setisHost(bool is_host)
    {
      m_isHost = is_host;
    }
    bool getisHost()
    {
      return m_isHost;
    }

    void setDeviceId(std::string device_id)
    {
      m_deviceId = device_id.c_str();
    }
    std::string getDeviceId()
    {
      return m_deviceId;
    }

    void setImageUrl(std::string image_url)
    {
      m_imageUrl = image_url.c_str();
    }
    std::string getImageUrl()
    {
      return m_imageUrl;
    }

    void setJoinStatus(std::string joinstatus)
    {
      m_joinStatus = joinstatus.c_str();
    }
    std::string getJoinStatus()
    {
      return m_joinStatus;
    }

    void setSpeakerLanguage(std::string lang)
    {
      m_speakerLang = lang.c_str();
    }
    std::string getSpeakerLanguage()
    {
      return m_speakerLang;
    }
    
    list<std::string> m_transcriptenabledlist;

  private:
    std::string m_roomId;
    std::string m_callId;
    std::string m_connId;
    std::string m_lang;
    bool m_ccState;
    std::string m_speaker;
    bool m_isHost;
    std::string m_deviceId;
    std::string m_imageUrl;
    std::string m_joinStatus;
    std::string m_speakerLang;
};

#endif
