/* $Header: /xGate/quality.h 12    4/09/03 11:06a Bennylp $
 *   
 * XGATE
 * (C) 2001 Switchlab, Ltd
 *
 * Quality Monitoring
 *
 * $Log: /xGate/quality.h $
 * 
 * 12    4/09/03 11:06a Bennylp
 * Fixed memory leak (the E_Model instance is not deleted)
 * 
 * 11    13/02/02 15:30 Sonny
 * updates: now show r instead of itot
 * 
 * 10    11/02/02 16:30 Sonny
 * quality monitoring updates
 * 
 * 9     7/02/02 15:41 Sonny
 * packets/bytes number is an average value
 * 
 * 8     6/02/02 19:01 Sonny
 * optimizations for quality monitoring
 * 
 * 7     4/02/02 12:33 Sonny
 * calls get quality from media
 * 
 * 6     1/02/02 15:35 Sonny
 * 
 * 5     1/02/02 12:20 Sonny
 * statistics monitor updates
 * 
 * 4     31/01/02 16:38 Sonny
 * bug fixes
 * 
 * 3     31/01/02 13:12 Sonny
 * add call quality monitoring
 * 
 * 2     24/01/02 20:57 Sonny
 * Initial Coding
*/

#ifndef __XGATE_CALLQUALITY_H__
#define __XGATE_CALLQUALITY_H__


//#include "stdafx.h"
#include "call.h"
#include "timed_stat.h"

#define RTP_MAX_CODEC_LIST  32

/** RTP payload constants.
 */
enum Rtp_Payload_Code {
    RTP_PT_INVALID = 255,
    RTP_PT_PCMU	 = 0,		/// G.711 u-law @ 64 Kbps
    RTP_PT_G726 = 2,		/// G.726-32
    RTP_PT_GSM = 3,		/// GSM 06.10 @ 13Kbps
    RTP_PT_G7231 = 4,		/// G.723.1 @ 6.4Kbps / 5.3Kbps
    RTP_PT_DVI4_8000 = 5,	/// DVI4 @ 8000 Hz
    RTP_PT_DVI4_16000 = 6,	/// DVI4 @ 16000 Hz
    RTP_PT_LPC = 7,		/// LPC
    RTP_PT_PCMA = 8,		/// G.711 a-law @ 64 Kbps
    RTP_PT_G722 = 9,		/// G.722
    RTP_PT_L16_44100_2 = 10,	/// L16 @ 44100 - stereo
    RTP_PT_L16_44100_1 = 11,	/// L16 @ 44100 - mono
    RTP_PT_QCELP = 12,		/// QCELP
    RTP_PT_G728 = 15,		/// G.728
    RTP_PT_DVI4_11025 = 16,	/// DVI4 @ 11025 Hz
    RTP_PT_DVI4_22050 = 17,	/// DVI4 @ 22050 Hz
    RTP_PT_G729 = 18,		/// G.729
    RTP_PT_Netcoder_4 = 49,	/// AudioCodes Netcoder @ 4.8 Kbps
    RTP_PT_Netcoder_5 = 50,	/// AudioCodes Netcoder @ 5.6 Kbps
    RTP_PT_Netcoder_6 = 51,	/// AudioCodes Netcoder @ 6.4 Kbps
    RTP_PT_Netcoder_7 = 52,	/// AudioCodes Netcoder @ 7.2 Kbps
    RTP_PT_Netcoder_8 = 53,	/// AudioCodes Netcoder @ 8.0 Kbps
    RTP_PT_Netcoder_88 = 54,	/// AudioCodes Netcoder @ 8.8 Kbps
    RTP_PT_Netcoder_9 = 55,	/// AudioCodes Netcoder @ 9.6 Kbps
    RTP_PT_DTMF_Relay = 100,	/// DTMF relayed as RTP.
    RTP_PT_RFC_2833 = 101,	/// RFC 2833 DTMF
};


////////////////////////////////////////////////////////////////////////////////////
// E Model
////////////////////////////////////////////////////////////////////////////////////

class E_Model {
    // Input parameters 
    double SLR;	    // Sender Loudness Rating 
    double RLR;	    // Receiver Loudness Rating 
    double STMRs;
    double STMR;
    double Ds;
    double Dr;
    double LSTRr;
    double TELR;
    double T;
    double WEPL;
    double Tr;
    double Ta;
    double Ie;
    double A;
    double Nc;
    double Ps;
    double Pr;
    double qdu;
    double Nfor;

    /* Output calculations */
    double Nt;
    double No;
    double Ro;
    double Iolr;
    double Ist;
    double Iq;
    double Isyn;
    double TERV;
    double Idte;
    double Idle;
    double Idd;
    double Id;
    double R;
    
    enum Loss_Type {
	LOSS_TYPE_INVALID,
	LOSS_TYPE_G729A_VAD,
	LOSS_TYPE_G723_1A_VAD,
	LOSS_TYPE_GSM_EFR,
	LOSS_TYPE_G711,
	LOSS_TYPE_G711PLC_RANDOM,
	LOSS_TYPE_G711PLC_BURSTY,
    };


    struct PayloadValue {
	Rtp_Payload_Code _payload;
	Loss_Type _lossType;
	float _value; // equipment impairment value without loss
	int _frameSize;  // frame size in milisecond

	void Init(Rtp_Payload_Code payload, Loss_Type lossType, float value, int frameSize) {
	    _payload = payload;
	    _lossType = lossType;
	    _value = value;
	    _frameSize = frameSize;
	};
	
    };

    struct LossValue {
	Loss_Type _lossType;
	float _lossTable[21];

	void Init(Loss_Type lossType)
	{
	    _lossType = lossType;
	    Clear();
	}
	
	void Clear() {
	    for (int i = 0; i < 21; i++)
		_lossTable[i] = 0.0f;
	}

	void InsertValue(int loss, float value) {
	    if (loss < 0 || loss > 20) return;
	    _lossTable[loss] = value;
	}
    };

    static List<PayloadValue> _payloadList;
    static List<LossValue> _lossList;
    
    float GetCodecValue(Rtp_Payload_Code code, Loss_Type & lossType);
    float GetLossValue(Loss_Type lossType, float loss);

    /* Initialize with default parameters */
    void e_model_init_params();
    /* Noise Summation (formula 3,4,5,6,7) */
    void e_model_compute_nt();
    /* Signal to Noise Ratio (Ro) (formula 2) */
    void e_model_compute_ro();
    /* Compute Iolr (formula 9,10) */
    void e_model_compute_iolr();
    /* Compute Ist (formula 11,12) */
    void e_model_compute_ist();
    /* Compute Iq (formula 13-17) */
    void e_model_compute_iq();
    /* Compute Is (formula 8) */
    void e_model_compute_is();
    /* Compute TERV (formula 22, 23) */
    void e_model_compute_terv();
    /* Compute Idte (formula 19,20,21,24) */
    void e_model_compute_idte();
    /* Compute Idle (formula 25,26) */
    void e_model_compute_idle();
    /* Compute Idd (formula 27,28) */
    void e_model_compute_idd();
    /* Compute R value with E-model calculation */
    void e_model_compute_r();


    void init_lists();
    void destroy_lists();

    E_Model();

public:
    ~E_Model();

    static E_Model* Instance();
    void RestoreDefault();

    float GetDefaultRo() const { return Ro; };
    float GetDefaultIs() const { return Isyn; };
    float GetDefaultId() const { return Idd + Idte + Idle; };
    float GetDefaultIe() const { return Ie; };

    float GetFrameSize(Rtp_Payload_Code code);
    float Calculate_Is() { return GetDefaultIs(); }
    float Calculate_Id(float absoluteDelay);
    float Calculate_Ie(Rtp_Payload_Code payload, float loss);
    float Calculate_R(float itot) { return (itot < 0) ? -1 : GetDefaultRo() - itot; }

};

////////////////////////////////////////////////////////////////////////////////////
// Statistics monitoring classes
////////////////////////////////////////////////////////////////////////////////////

// Statistics value
struct Quality_Value {
    enum {
	MAX_VALUE = 1000 * 1000 * 1000
    };

    float _value;
    unsigned _count;

    bool IsValid(float val) const {
	return val >= 0.0f && val < MAX_VALUE;
    }


    Quality_Value() { Reset(); }

    void Reset() {
	_value = -1;
	_count = 0;
    }

    float GetValue() const { return _value; }
    unsigned GetCount() const { return _count; }
    void SetValue(float value) { _value = value; }
    void SetCount(unsigned count) { _count = count; }

    bool Valid() const {
	return _value >= 0.0f && _value < MAX_VALUE;
    }

    void operator= (float rvalue) {
	if (IsValid(rvalue)) {
	    _value = rvalue;
	    _count = 1;
	}
    }

    float operator/ (float rvalue) {
	return (rvalue > 0) ? _value / rvalue : _value;
    }

    void add(float rvalue) {
	if (IsValid(rvalue)) {
	    if (IsValid(_value)) {
		_value += rvalue;
	    }
	    else {
		_value = rvalue;
	    }
	}
    }

    void add_avg(float rvalue) {
	if (IsValid(rvalue)) {
	    if (IsValid(_value)) {
		_value = (_value*_count + rvalue) / (_count + 1);
	    }
	    else {
		_value = rvalue;
	    }
	    _count += 1;
	}
    }

    void add_avg(const Quality_Value& rhs) {
	if (IsValid(rhs._value)) {
	    if (IsValid(_value)) {
		_value = (_value*_count + rhs._value*rhs._count) / (_count + rhs._count);
	    }
	    else {
		_value = rhs._value;
	    }
	    _count += rhs._count;
	}
    }
};


class Rtp_Quality_Source;

struct Rtp_Quality_Data {
    Quality_Value _rx_packets;
    Quality_Value _rx_bytes;
    Quality_Value _rx_rtt;
    Quality_Value _rx_jitter;
    Quality_Value _rx_fracloss;
    Quality_Value _rx_loss;
    Quality_Value _rx_is;
    Quality_Value _rx_id;
    Quality_Value _rx_ie;
    Quality_Value _rx_itot;

    Quality_Value _tx_packets;
    Quality_Value _tx_bytes;
    Quality_Value _tx_rtt;
    Quality_Value _tx_jitter;
    Quality_Value _tx_fracloss;
    Quality_Value _tx_loss;
    Quality_Value _tx_is;
    Quality_Value _tx_id;
    Quality_Value _tx_ie;
    Quality_Value _tx_itot;

    Rtp_Quality_Source* _source;
    Rtp_Payload_Code _payload;
    int _minJitter, _packingFactor;

    int _count; // only for output calculation
    bool _ready;
    

    Rtp_Quality_Data();
    ~Rtp_Quality_Data();

    void Reset();
    void Enable() { _ready = true; }
    void Disable() { _ready = false; }
    bool Ready() const { return _ready; }

    void SetCount(int count) { _count = count; }
    void SetSource(Rtp_Quality_Source* source) { _source = source; }

    void Update(CallQualityParameter param, const Rtp_Quality_Data& rhs);
    
    float RxAbsoluteDelay() {
	if (!_rx_jitter.Valid() || !_rx_rtt.Valid() || _packingFactor < 0 || _minJitter < 0) return -1.0;
	int frame = E_Model::Instance()->GetFrameSize(_payload);
	if (frame < 0 ) return -1.0;
	int actualJitter = (_rx_jitter / frame) + 1;
	actualJitter *= frame;
	return _rx_rtt/2 + 
	    ((actualJitter > 150)?150:(actualJitter>_minJitter)?actualJitter:_minJitter) + 
	    frame*2;
    }

    float TxAbsoluteDelay() {
	if (!_tx_jitter.Valid() || !_tx_rtt.Valid() || _packingFactor < 0 || _minJitter < 0) return -1.0;
	int frame = E_Model::Instance()->GetFrameSize(_payload);
	if (frame < 0) return -1.0;
	int actualJitter = (_tx_jitter / frame) + 1;
	actualJitter *= frame;
	return _rx_rtt/2 + actualJitter + frame*(_packingFactor+1);
    }
	
    void Calculate(CallQualityParameter param);
    float GetValue(CallQualityParameter param);

};


// Rtp statistics source
class Rtp_Quality_Source {
protected:

    Rtp_Quality_Data _quality_data;
    friend class Media_Quality_Monitor;
public:

    Rtp_Quality_Source();
    ~Rtp_Quality_Source();

    void Reset();

    bool Ready() const { return _quality_data.Ready(); }

    void SetMinJitter(int minJitter) { _quality_data._minJitter = minJitter; }
    void SetPackingFactor(int packingFactor) { _quality_data._packingFactor = packingFactor; }

    void CalculateQuality(CallQualityParameter param);
    float GetQuality(CallQualityParameter param);

    void Update(const Rtp_Quality_Source& source);
    Rtp_Quality_Data& GetData() { return _quality_data; }

    virtual void UpdateQualityData(CallQualityParameter param) {};
};

// Statistics monitor
class Rtp_Quality_Monitor {

    int _minJitter;
    int _packingFactor;

    timed_statistic<int, 1, 24> _calls;

    timed_statistic<float, 1, 24> _rx_packets;
    timed_statistic<float, 1, 24> _rx_bytes;
    timed_statistic<float, 1, 24> _rx_rtt;
    timed_statistic<float, 1, 24> _rx_jitter;
    timed_statistic<float, 1, 24> _rx_fracloss;
    timed_statistic<float, 1, 24> _rx_loss;
    timed_statistic<float, 1, 24> _rx_is;
    timed_statistic<float, 1, 24> _rx_id;
    timed_statistic<float, 1, 24> _rx_ie;
    timed_statistic<float, 1, 24> _rx_itot;
    
    timed_statistic<float, 1, 24> _tx_packets;
    timed_statistic<float, 1, 24> _tx_bytes;
    timed_statistic<float, 1, 24> _tx_rtt;
    timed_statistic<float, 1, 24> _tx_jitter;
    timed_statistic<float, 1, 24> _tx_fracloss;
    timed_statistic<float, 1, 24> _tx_loss;
    timed_statistic<float, 1, 24> _tx_is;
    timed_statistic<float, 1, 24> _tx_id;
    timed_statistic<float, 1, 24> _tx_ie;
    timed_statistic<float, 1, 24> _tx_itot;

    Rtp_Quality_Data _active_data;

    List<Rtp_Quality_Source*> _sourceList;

    void UpdateActiveData(CallQualityParameter param);
    void PrintActiveData(Term_Session* session);
    void PrintData(Term_Session* session, time_t start, time_t end, bool now);

    Quality_Value& GetActiveValue(CallQualityParameter param, Quality_Value& active_value);

public:
    Rtp_Quality_Monitor();
    ~Rtp_Quality_Monitor();

    void SetMinJitter(int minJitter);
    void SetPackingFactor(int packingFactor);

    void Register(Rtp_Quality_Source* source);
    void Unregister(Rtp_Quality_Source* source);

    void RecordData(Rtp_Quality_Source* source);

    void Show(Term_Session* session, int interval);

    float GetQuality(CallQualityParameter param, time_t start, time_t end, bool active);
};

#endif //__XGATE_CALLQUALITY_H__

