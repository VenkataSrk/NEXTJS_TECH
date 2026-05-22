/* $Header: /xGate/quality.cpp 17    6/06/03 4:07p Bennylp $
 *   
 * XGATE
 * (C) 2001 Switchlab, Ltd
 *
 * Quality Monitoring
 *
 * $Log: /xGate/quality.cpp $
 * 
 * 17    6/06/03 4:07p Bennylp
 * Lots of changes to incorporate mapletree diagnostic (Version 54.4)
 * 
 * 16    4/09/03 11:06a Bennylp
 * Fixed memory leak (the E_Model instance is not deleted)
 * 
 * 15    9/18/02 11:55a Bennylp
 * Remove unnecessary assert().
 * 
 * 14    8/16/02 10:29a Sonny
 * 
 * 13    13/02/02 16:45 Sonny
 * changed base ie value for G729 to 11
 * 
 * 12    13/02/02 15:30 Sonny
 * updates: now show r instead of itot
 * 
 * 11    11/02/02 16:30 Sonny
 * quality monitoring updates
 * 
 * 10    7/02/02 15:41 Sonny
 * packets/bytes number is an average value
 * 
 * 9     6/02/02 19:01 Sonny
 * optimizations for quality monitoring
 * 
 * 8     4/02/02 18:03 Sonny
 * show packets instead of id and ie
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
#include <stdio.h>
#include <math.h>
#include "stdafx.h"
#include "quality.h"
#include "term.h"
#include "kernel.h"



////////////////////////////////////////////////////////////////////////////////////////////
// E_Model
////////////////////////////////////////////////////////////////////////////////////////////

List<E_Model::PayloadValue> E_Model::_payloadList;
List<E_Model::LossValue> E_Model::_lossList;

E_Model::E_Model() {
    init_lists();
    e_model_init_params();
    e_model_compute_r();
}

E_Model::~E_Model() {
    destroy_lists();
}

void E_Model::init_lists() {
    LossValue loss;
    PayloadValue payload;

   
    loss.Init(LOSS_TYPE_G729A_VAD);

    loss.InsertValue( 0, 0);
    loss.InsertValue( 1, 4);
    loss.InsertValue( 2, 8);
    loss.InsertValue( 3, 12);
    loss.InsertValue( 4, 15);
    loss.InsertValue( 5, 17.5);
    loss.InsertValue( 6, 20);
    loss.InsertValue( 7, 22);
    loss.InsertValue( 8, 25);
    loss.InsertValue( 9, 26.25);
    loss.InsertValue(10, 27.5);
    loss.InsertValue(11, 28.75);
    loss.InsertValue(12, 30);
    loss.InsertValue(13, 32);
    loss.InsertValue(14, 34);
    loss.InsertValue(15, 36);
    loss.InsertValue(16, 38);
    loss.InsertValue(17, 38);
    loss.InsertValue(18, 38);
    loss.InsertValue(19, 38);
    loss.InsertValue(20, 38);

    _lossList.push_back(loss);

    loss.Init(LOSS_TYPE_G723_1A_VAD);
    loss.InsertValue( 0, 0);
    loss.InsertValue( 1, 4);
    loss.InsertValue( 2, 9);
    loss.InsertValue( 3, 12);
    loss.InsertValue( 4, 17);
    loss.InsertValue( 5, 19.25);
    loss.InsertValue( 6, 21.5);
    loss.InsertValue( 7, 23.75);
    loss.InsertValue( 8, 26);
    loss.InsertValue( 9, 27.75);
    loss.InsertValue(10, 29.5);
    loss.InsertValue(11, 31.25);
    loss.InsertValue(12, 33);
    loss.InsertValue(13, 34.75);
    loss.InsertValue(14, 36.5);
    loss.InsertValue(15, 38.25);
    loss.InsertValue(16, 40);
    loss.InsertValue(17, 40);
    loss.InsertValue(18, 40);
    loss.InsertValue(19, 40);
    loss.InsertValue(20, 40);
    _lossList.push_back(loss);

    loss.Init(LOSS_TYPE_GSM_EFR);
    loss.InsertValue( 0, 0);
    loss.InsertValue( 1, 11);
    loss.InsertValue( 2, 16);
    loss.InsertValue( 3, 21);
    loss.InsertValue( 4, 28);
    loss.InsertValue( 5, 28);
    loss.InsertValue( 6, 28);
    loss.InsertValue( 7, 28);
    loss.InsertValue( 8, 28);
    loss.InsertValue( 9, 28);
    loss.InsertValue(10, 28);
    loss.InsertValue(11, 28);
    loss.InsertValue(12, 28);
    loss.InsertValue(13, 28);
    loss.InsertValue(14, 28);
    loss.InsertValue(15, 28);
    loss.InsertValue(16, 28);
    loss.InsertValue(17, 28);
    loss.InsertValue(18, 28);
    loss.InsertValue(19, 28);
    loss.InsertValue(20, 28);
    _lossList.push_back(loss);

    loss.Init(LOSS_TYPE_G711);
    loss.InsertValue( 0, 0);
    loss.InsertValue( 1, 25);
    loss.InsertValue( 2, 35);
    loss.InsertValue( 3, 45);
    loss.InsertValue( 4, 50);
    loss.InsertValue( 5, 55);
    loss.InsertValue( 6, 55);
    loss.InsertValue( 7, 55);
    loss.InsertValue( 8, 55);
    loss.InsertValue( 9, 55);
    loss.InsertValue(10, 55);
    loss.InsertValue(11, 55);
    loss.InsertValue(12, 55);
    loss.InsertValue(13, 55);
    loss.InsertValue(14, 55);
    loss.InsertValue(15, 55);
    loss.InsertValue(16, 55);
    loss.InsertValue(17, 55);
    loss.InsertValue(18, 55);
    loss.InsertValue(19, 55);
    loss.InsertValue(20, 55);
    _lossList.push_back(loss);

    loss.Init(LOSS_TYPE_G711PLC_RANDOM);
    loss.InsertValue( 0, 0);
    loss.InsertValue( 1, 5);
    loss.InsertValue( 2, 7);
    loss.InsertValue( 3, 10);
    loss.InsertValue( 4, 12.5);
    loss.InsertValue( 5, 15);
    loss.InsertValue( 6, 17.5);
    loss.InsertValue( 7, 20);
    loss.InsertValue( 8, 22);
    loss.InsertValue( 9, 23.5);
    loss.InsertValue(10, 25);
    loss.InsertValue(11, 27);
    loss.InsertValue(12, 29);
    loss.InsertValue(13, 31);
    loss.InsertValue(14, 33);
    loss.InsertValue(15, 35);
    loss.InsertValue(16, 37.5);
    loss.InsertValue(17, 40);
    loss.InsertValue(18, 42);
    loss.InsertValue(19, 43.5);
    loss.InsertValue(20, 45);
    _lossList.push_back(loss);

    loss.Init(LOSS_TYPE_G711PLC_BURSTY);
    loss.InsertValue( 0, 0);
    loss.InsertValue( 1, 5);
    loss.InsertValue( 2, 7);
    loss.InsertValue( 3, 10);
    loss.InsertValue( 4, 22.5);
    loss.InsertValue( 5, 30);
    loss.InsertValue( 6, 32.5);
    loss.InsertValue( 7, 35);
    loss.InsertValue( 8, 37);
    loss.InsertValue( 9, 38.5);
    loss.InsertValue(10, 40);
    loss.InsertValue(11, 41);
    loss.InsertValue(12, 42);
    loss.InsertValue(13, 43);
    loss.InsertValue(14, 44);
    loss.InsertValue(15, 45);
    loss.InsertValue(16, 46);
    loss.InsertValue(17, 47);
    loss.InsertValue(18, 48);
    loss.InsertValue(19, 49);
    loss.InsertValue(20, 50);
    _lossList.push_back(loss);


    payload.Init(RTP_PT_PCMU, LOSS_TYPE_G711PLC_RANDOM, 0, 20);
    _payloadList.push_back(payload);

    payload.Init(RTP_PT_PCMA, LOSS_TYPE_G711PLC_RANDOM, 0, 20);
    _payloadList.push_back(payload);
    
    payload.Init(RTP_PT_G726, LOSS_TYPE_G711PLC_RANDOM, 50, 20);
    _payloadList.push_back(payload);
    
    payload.Init(RTP_PT_GSM, LOSS_TYPE_GSM_EFR, 5, 20);
    _payloadList.push_back(payload);
    
    payload.Init(RTP_PT_G7231, LOSS_TYPE_G723_1A_VAD, 19, 30);
    _payloadList.push_back(payload);
    
    payload.Init(RTP_PT_DVI4_8000, LOSS_TYPE_G729A_VAD, 10, 20);
    _payloadList.push_back(payload);
    
    payload.Init(RTP_PT_DVI4_16000, LOSS_TYPE_G711, 7, 20);
    _payloadList.push_back(payload);
    
    payload.Init(RTP_PT_LPC, LOSS_TYPE_G711PLC_RANDOM, 0, 20);
    _payloadList.push_back(payload);
    
    payload.Init(RTP_PT_G722, LOSS_TYPE_G711PLC_RANDOM, 0, 20);
    _payloadList.push_back(payload);
    
    payload.Init(RTP_PT_L16_44100_2, LOSS_TYPE_G711PLC_RANDOM, 0, 20);
    _payloadList.push_back(payload);
    
    payload.Init(RTP_PT_L16_44100_1, LOSS_TYPE_G711PLC_RANDOM, 0, 20);
    _payloadList.push_back(payload);
    
    payload.Init(RTP_PT_QCELP, LOSS_TYPE_G711PLC_RANDOM, 21, 20);
    _payloadList.push_back(payload);
    
    payload.Init(RTP_PT_G728, LOSS_TYPE_G711PLC_RANDOM, 7, 20);
    _payloadList.push_back(payload);
    
    payload.Init(RTP_PT_DVI4_11025, LOSS_TYPE_G711PLC_RANDOM, 0, 20);
    _payloadList.push_back(payload);
    
    payload.Init(RTP_PT_DVI4_22050, LOSS_TYPE_G711PLC_RANDOM, 0, 20);
    _payloadList.push_back(payload);
    
    payload.Init(RTP_PT_G729, LOSS_TYPE_G729A_VAD, 11, 20);
    _payloadList.push_back(payload);
    
    payload.Init(RTP_PT_Netcoder_4, LOSS_TYPE_G723_1A_VAD, 20, 20);
    _payloadList.push_back(payload);
    
    payload.Init(RTP_PT_Netcoder_5, LOSS_TYPE_G723_1A_VAD, 17, 20);
    _payloadList.push_back(payload);
    
    payload.Init(RTP_PT_Netcoder_6, LOSS_TYPE_G723_1A_VAD, 15, 20);
    _payloadList.push_back(payload);
    
    payload.Init(RTP_PT_Netcoder_7, LOSS_TYPE_G723_1A_VAD, 14, 20);
    _payloadList.push_back(payload);
    
    payload.Init(RTP_PT_Netcoder_8, LOSS_TYPE_G723_1A_VAD, 13, 20);
    _payloadList.push_back(payload);
    
    payload.Init(RTP_PT_Netcoder_88, LOSS_TYPE_G723_1A_VAD, 12, 20);
    _payloadList.push_back(payload);
    
    payload.Init(RTP_PT_Netcoder_9, LOSS_TYPE_G729A_VAD, 11, 20);
    _payloadList.push_back(payload);
    
    payload.Init(RTP_PT_DTMF_Relay, LOSS_TYPE_G711PLC_RANDOM, 0, 20);
    _payloadList.push_back(payload);


}

void E_Model::destroy_lists() {
    while (!_payloadList.empty()) {
	_payloadList.pop_front();
    }
    while (!_lossList.empty()) {
	_lossList.pop_front();
    }



}

E_Model* E_Model::Instance() 
{
    static E_Model instance_;
    return &instance_;
}

void E_Model::RestoreDefault() {
    e_model_init_params();
    e_model_compute_r();
}


void E_Model::e_model_init_params()
{
    memset(this, 0, sizeof(E_Model));
    SLR = 8;
    RLR = 2;
    STMRs = 15;
    STMR = 15;
    Ds = 3;
    Dr = 3;
    LSTRr = 18;
    TELR = 65;
    T = 0;
    WEPL = 110;
    Tr = 0;
    Ta = 0;
    Ie = 0;
    A = 0;
    Nc = -70;
    Ps = 35;
    Pr = 35;
    qdu = 1;
    Nfor = -64;
}

/* Noise Summation (formula 3,4,5,6,7) */
void E_Model::e_model_compute_nt()
{
    double Nr1, Nr2, Pro, Pr1, Nfo, LSTR;
    
    Nr1 = Ps - SLR - Ds - 100;
    Nr1 = Nr1 + 0.004 * pow(Ps - SLR - RLR - Ds - 14, 2 );
    LSTR = STMR + Dr;
    Pro = Pr + 10 * log(1 + pow(10, (10-LSTR)/10)) / log(10);
    Pr1 = Pro + .008 * pow(Pro-35, 2);
    Nr2 = Pr1 - 121 + RLR;
    Nfo = Nfor + RLR;
    No = 10 * log( pow(10,Nr1 / 10) + 
		      pow(10,Nr2 / 10) + 
		      pow(10,Nc / 10) + 
		      pow(10,Nfo / 10)) / 
	    log(10);
    Nt = No - RLR;
    /*return Nt;*/
}

/* Signal to Noise Ratio (Ro) (formula 2) */
void E_Model::e_model_compute_ro()
{
    Ro = 15 - 1.5 * (SLR + No);
}

/* Compute Iolr (formula 9,10) */
void E_Model::e_model_compute_iolr()
{
    double Xolr;
    Xolr = SLR + RLR + 0.2 * (64 + Nt);
    Iolr = 20 * (pow(1+pow(Xolr/8,8), (1.0/8)) - Xolr/8);
    /*return Iolr;*/
}

/* Compute Ist (formula 11,12) */
void E_Model::e_model_compute_ist()
{
    double STMRo;
    
    STMRo = -10 * log( pow(10, -STMR/10) + 
		       pow(10, -TELR/10) * exp(-T/4)) / 
	    log(10);
    Ist = 10 * pow((1 + pow((STMRo-12)/5, 6)), 1.0/6) - 10;
    Ist = Ist - 46 * pow((1 + pow(STMRo/23,10)), 1.0/10) + 46;
    /*return Ist;*/
}

/* Compute Iq (formula 13-17) */
void E_Model::e_model_compute_iq()
{
    double Q, G;
    
    if (qdu < 1) qdu = 1;
    Q = 37 - 15 * log(qdu) / log(10);
    G = 1.07 + 0.258 * Q + 0.0602 * pow(Q,2);
    Iq = 15 * 
	 log(1 + 
	     pow(10, ((Ro - 100) / 15)) * pow(10, (46.0 / 8.4 - G /9)) + 
	     pow(10, (46.0 / 30 - G / 40))) / 
	 log(10);

    /*return Iq;*/
}

/* Compute Is (formula 8) */
void E_Model::e_model_compute_is()
{
    Isyn = Iolr + Ist + Iq;
    /*return Isyn;*/
}

/* Compute TERV (formula 22, 23) */
void E_Model::e_model_compute_terv()
{
    TERV = TELR + 
	   6 * exp(-0.3 * pow(T,2) ) - 
	   40 * log((1 + T / 10) / (1 + T / 150)) / 
	   log(10);
    if (STMR < 9)
	TERV = TERV * 0.5 * Ist;  
    /*return TERV;*/
}

/* Compute Idte (formula 19,20,21,24) */
void E_Model::e_model_compute_idte()
{
    double Re, Roe, Xdt;
    Re = 80 + 2.5 * (TERV - 14);
    Roe = -1.5 * (No - RLR);
    Xdt = (Roe - Re) / 2;
    Idte = Xdt + sqrt( pow(Xdt,2) + 100);
    Idte = (Idte - 1) * (1 - exp(-T));

    /* Modification to satisfy formula 24 */
    if (STMR > 15)
	Idte = sqrt( pow(Idte,2) + pow(Ist,2) ); 
    /*return Idte;*/
}

/* Compute Idle (formula 25,26) */
void E_Model::e_model_compute_idle()
{
    double Rle, Xdl;
    Rle = 10.5 * (WEPL + 7) * pow((Tr + 1), (-1 / 4.0));
    Xdl = (Ro - Rle) / 2;
    Idle = Xdl + sqrt( pow(Xdl,2) + 169);
    /*return Idle;*/
}

/* Compute Idd (formula 27,28) */
void E_Model::e_model_compute_idd()
{
    if (Ta <= 100)
	Idd = 0;
    else  {
	double X;
	X = log(Ta / 100) / log(2);
	Idd = 25 * ( pow(1 + pow(X,6),1.0/6) - 
		        3 * pow(1+pow(X/3,6) , 1.0/6) +
		        2);
    }
    /*return Idd;*/
}


/* Compute R value with E-model calculation */
void E_Model::e_model_compute_r ()
{
    e_model_compute_nt();
    e_model_compute_ro();
    e_model_compute_iolr();
    e_model_compute_ist();
    e_model_compute_iq();
    e_model_compute_is();
    e_model_compute_terv();
    e_model_compute_idte();
    e_model_compute_idle();
    e_model_compute_idd();
    
    Id = Idte + Idle + Idd;

    /* Compute R (formula 1) */
    R = Ro - Isyn - Id - Ie + A;
}


float E_Model::GetCodecValue(Rtp_Payload_Code payload, Loss_Type & lossType) {
    List<PayloadValue>::iterator it = _payloadList.begin(), end = _payloadList.end();
    for (;it!=end;it++) {
	if ((*it)._payload == payload) {
	    lossType = (*it)._lossType;
	    return (*it)._value;
	}
    }
    return -1.0;
}

float E_Model::GetLossValue(E_Model::Loss_Type lossType, float loss) {
    List<LossValue>::iterator it = _lossList.begin(), end = _lossList.end();
    LossValue lossValue;
    int index = loss;
    float delta = loss - index;
    float value, value1;
    for (;it!=end;it++) {
	if ((*it)._lossType == lossType) {
	    lossValue = (*it);
	    if (loss < 20) {
		value = lossValue._lossTable[index];
		value1 = lossValue._lossTable[index+1];
		return value + (value1-value)*delta; 
	    }
	    else {
		return lossValue._lossTable[20];
	    }
	}
    }
    return -1.0;
}

float E_Model::GetFrameSize(Rtp_Payload_Code payload) {
    List<PayloadValue>::iterator it = _payloadList.begin(), end = _payloadList.end();
    for (;it!=end;it++) {
	if ((*it)._payload == payload) {
	    return (*it)._frameSize;
	}
    }
    return -1.0;
}


float E_Model::Calculate_Id(float absoluteDelay) {
    if (absoluteDelay < 0) return -1;
    double temp_Ta, temp_Idd;
    double result;
    temp_Ta = Ta;
    temp_Idd = Idd;
    Ta = absoluteDelay;
    e_model_compute_idd();
    result = Idd + Idte + Idle;
    Ta = temp_Ta;
    Idd = temp_Idd;

    return result;
}


float E_Model::Calculate_Ie(Rtp_Payload_Code payload, float loss) {
    double codecValue, lossValue;
    Loss_Type lossType;
    codecValue = GetCodecValue(payload, lossType);
    if (codecValue < 0) return -1.0;
    lossValue = GetLossValue(lossType, loss);
    if (lossValue < 0) return -1.0;
    return codecValue + lossValue;

}


////////////////////////////////////////////////////////////////////////////////////////////
// Rtp_Quality_Data
////////////////////////////////////////////////////////////////////////////////////////////

Rtp_Quality_Data::Rtp_Quality_Data() {
    _source = NULL;
    Reset();
    Enable();
}

Rtp_Quality_Data::~Rtp_Quality_Data() {
}

void Rtp_Quality_Data::Reset() {

    _rx_packets.Reset();
    _rx_bytes.Reset();
    _rx_rtt.Reset();
    _rx_jitter.Reset();
    _rx_loss.Reset();
    _rx_fracloss.Reset();
    _rx_is.Reset();
    _rx_id.Reset();
    _rx_ie.Reset();
    _rx_itot.Reset();

    _tx_packets.Reset();
    _tx_bytes.Reset();
    _tx_rtt.Reset();
    _tx_jitter.Reset();
    _tx_loss.Reset();
    _tx_fracloss.Reset();
    _tx_is.Reset();
    _tx_id.Reset();
    _tx_ie.Reset();
    _tx_itot.Reset();

    _count = 0;
}

void Rtp_Quality_Data::Calculate(CallQualityParameter param) {
    switch (param) {
    case CQ_ALL:
    case CQ_RX_ITOT:
    case CQ_RX_IS:
	_rx_is = E_Model::Instance()->Calculate_Is();
	if (param && param != CQ_RX_ITOT) break;
    case CQ_RX_ID:
	_rx_id = E_Model::Instance()->Calculate_Id(RxAbsoluteDelay());
	if (param && param != CQ_RX_ITOT) break;
    case CQ_RX_IE:
	_rx_ie = E_Model::Instance()->Calculate_Ie(_payload, _rx_loss.GetValue());
	if (param && param != CQ_RX_ITOT) break;
    // itot = is + id + ie
	if (_rx_is.Valid() && _rx_id.Valid() && _rx_ie.Valid()) {
	    _rx_itot = _rx_is.GetValue() + _rx_id.GetValue() + _rx_ie.GetValue();
	}
	if (param) break;

    case CQ_TX_ITOT:
    case CQ_TX_IS:
	_tx_is = E_Model::Instance()->Calculate_Is();
	if (param && param != CQ_TX_ITOT) break;
    case CQ_TX_ID:
	_tx_id = E_Model::Instance()->Calculate_Id(TxAbsoluteDelay());
	if (param && param != CQ_TX_ITOT) break;
    case CQ_TX_IE:
	_tx_ie = E_Model::Instance()->Calculate_Ie(_payload, _tx_loss.GetValue());
	if (param && param != CQ_TX_ITOT) break;
    // itot = is + id + ie
	if (_tx_is.Valid() && _tx_id.Valid() && _tx_ie.Valid()) {
	    _tx_itot = _tx_is.GetValue() + _tx_id.GetValue() + _tx_ie.GetValue();
	}
	if (param) break;
    default:
	break;
    }
}

float Rtp_Quality_Data::GetValue(CallQualityParameter param) { 
    if (_source) _source->UpdateQualityData(param);
    Calculate(param);
    switch (param) {
    case CQ_RX_PACKETS:
	return _rx_packets.GetValue();
    case CQ_RX_BYTES:
	return _rx_bytes.GetValue();
    case CQ_RX_IS:
	return _rx_is.GetValue();
    case CQ_RX_ID:
	return _rx_id.GetValue();
    case CQ_RX_IE:
	return _rx_ie.GetValue();
    case CQ_RX_ITOT:
	return _rx_itot.GetValue();
    case CQ_RX_RTT:
	return _rx_rtt.GetValue();
    case CQ_RX_JITTER:
	return _rx_jitter.GetValue();
    case CQ_RX_LOSS:
	return _rx_loss.GetValue();
    case CQ_RX_FRACLOSS:
	return _rx_fracloss.GetValue();

    case CQ_TX_PACKETS:
	return _tx_packets.GetValue();
    case CQ_TX_BYTES:
	return _tx_bytes.GetValue();
    case CQ_TX_IS:
	return _tx_is.GetValue();
    case CQ_TX_ID:
	return _tx_id.GetValue();
    case CQ_TX_IE:
	return _tx_ie.GetValue();
    case CQ_TX_ITOT:
	return _tx_itot.GetValue();
    case CQ_TX_RTT:
	return _tx_rtt.GetValue();
    case CQ_TX_JITTER:
	return _tx_jitter.GetValue();
    case CQ_TX_LOSS:
	return _tx_loss.GetValue();
    case CQ_TX_FRACLOSS:
	return _tx_fracloss.GetValue();

    default:
	return -1.0f;
    }
}

void Rtp_Quality_Data::Update(CallQualityParameter param, const Rtp_Quality_Data& rhs) {

    if (rhs._count == 0) return;
    //assert(rhs._count == 1);

/*
    _rx_packets.add_avg(rhs._rx_packets);
    _rx_bytes.add_avg(rhs._rx_bytes);
    _rx_rtt.add_avg(rhs._rx_rtt);
    _rx_jitter.add_avg(rhs._rx_jitter);
    _rx_loss.add_avg(rhs._rx_loss);
    _rx_fracloss.add_avg(rhs._rx_fracloss);
    _rx_is.add_avg(rhs._rx_is);
    _rx_id.add_avg(rhs._rx_id);
    _rx_ie.add_avg(rhs._rx_ie);
    _rx_itot.add_avg(rhs._rx_itot);

    _tx_packets.add_avg(rhs._tx_packets);
    _tx_bytes.add_avg(rhs._tx_bytes);
    _tx_rtt.add_avg(rhs._tx_rtt);
    _tx_jitter.add_avg(rhs._tx_jitter);
    _tx_loss.add_avg(rhs._tx_loss);
    _tx_fracloss.add_avg(rhs._tx_fracloss);
    _tx_is.add_avg(rhs._tx_is);
    _tx_id.add_avg(rhs._tx_id);
    _tx_ie.add_avg(rhs._tx_ie);
    _tx_itot.add_avg(rhs._tx_itot);
*/

    switch (param) {
    case CQ_ALL:
    case CQ_RX_PACKETS:
	_rx_packets.add_avg(rhs._rx_packets);
	if (param) break;
    case CQ_RX_BYTES:
	_rx_bytes.add_avg(rhs._rx_bytes);
	if (param) break;
    case CQ_RX_RTT:
	_rx_rtt.add_avg(rhs._rx_rtt);
	if (param) break;
    case CQ_RX_JITTER:
	_rx_jitter.add_avg(rhs._rx_jitter);
	if (param) break;
    case CQ_RX_LOSS:
	_rx_loss.add_avg(rhs._rx_loss);
	if (param) break;
    case CQ_RX_FRACLOSS:
	_rx_fracloss.add_avg(rhs._rx_fracloss);
	if (param) break;
    case CQ_RX_IS:
	_rx_is.add_avg(rhs._rx_is);
	if (param) break;
    case CQ_RX_ID:
	_rx_id.add_avg(rhs._rx_id);
	if (param) break;
    case CQ_RX_IE:
	_rx_ie.add_avg(rhs._rx_ie);
	if (param) break;
    case CQ_RX_ITOT:
	_rx_itot.add_avg(rhs._rx_itot);
	if (param) break;

    case CQ_TX_PACKETS:
	_tx_packets.add_avg(rhs._tx_packets);
	if (param) break;
    case CQ_TX_BYTES:
	_tx_bytes.add_avg(rhs._tx_bytes);
	if (param) break;
    case CQ_TX_RTT:
	_tx_rtt.add_avg(rhs._tx_rtt);
	if (param) break;
    case CQ_TX_JITTER:
	_tx_jitter.add_avg(rhs._tx_jitter);
	if (param) break;
    case CQ_TX_LOSS:
	_tx_loss.add_avg(rhs._tx_loss);
	if (param) break;
    case CQ_TX_FRACLOSS:
	_tx_fracloss.add_avg(rhs._tx_fracloss);
	if (param) break;
    case CQ_TX_IS:
	_tx_is.add_avg(rhs._tx_is);
	if (param) break;
    case CQ_TX_ID:
	_tx_id.add_avg(rhs._tx_id);
	if (param) break;
    case CQ_TX_IE:
	_tx_ie.add_avg(rhs._tx_ie);
	if (param) break;
    case CQ_TX_ITOT:
	_tx_itot.add_avg(rhs._tx_itot);
	if (param) break;

    default:
	break;
    }

    _count += rhs._count;
}



////////////////////////////////////////////////////////////////////////////////////////////
// Rtp_Quality_Source
////////////////////////////////////////////////////////////////////////////////////////////


Rtp_Quality_Source::Rtp_Quality_Source() {
    _quality_data.Disable();
    _quality_data.SetCount(1);
    _quality_data.SetSource(this);
}

Rtp_Quality_Source::~Rtp_Quality_Source() {
}


float Rtp_Quality_Source::GetQuality(CallQualityParameter param) { 
    return _quality_data.GetValue(param);
}



////////////////////////////////////////////////////////////////////////////////////////////
// Rtp_Quality_Monitor.
////////////////////////////////////////////////////////////////////////////////////////////

Rtp_Quality_Monitor::Rtp_Quality_Monitor() {
    _minJitter = -1;
    _packingFactor = -1;
}

Rtp_Quality_Monitor::~Rtp_Quality_Monitor() {
}

void Rtp_Quality_Monitor::RecordData(Rtp_Quality_Source* source) {
    source->UpdateQualityData(CQ_ALL);
    Rtp_Quality_Data data;
    if (source->Ready()) {
	source->GetData().Calculate(CQ_ALL);
	_calls.add(1);
	
	data = source->GetData();

	_rx_packets.add_avg(data._rx_packets.GetValue());
	_rx_bytes.add_avg(data._rx_bytes.GetValue());
	_rx_rtt.add_avg(data._rx_rtt.GetValue());
	_rx_jitter.add_avg(data._rx_jitter.GetValue());
	_rx_loss.add_avg(data._rx_loss.GetValue());
	_rx_fracloss.add_avg(data._rx_fracloss.GetValue());
	_rx_is.add_avg(data._rx_is.GetValue());
	_rx_id.add_avg(data._rx_id.GetValue());
	_rx_ie.add_avg(data._rx_ie.GetValue());
	_rx_itot.add_avg(data._rx_itot.GetValue());

	_tx_packets.add_avg(data._tx_packets.GetValue());
	_tx_bytes.add_avg(data._tx_bytes.GetValue());
	_tx_rtt.add_avg(data._tx_rtt.GetValue());
	_tx_jitter.add_avg(data._tx_jitter.GetValue());
	_tx_loss.add_avg(data._tx_loss.GetValue());
	_tx_fracloss.add_avg(data._tx_fracloss.GetValue());
	_tx_is.add_avg(data._tx_is.GetValue());
	_tx_id.add_avg(data._tx_id.GetValue());
	_tx_ie.add_avg(data._tx_ie.GetValue());
	_tx_itot.add_avg(data._tx_itot.GetValue());

    }
}

void Rtp_Quality_Monitor::Register(Rtp_Quality_Source* source) {
    source->SetMinJitter(_minJitter);
    source->SetPackingFactor(_packingFactor);
    source->UpdateQualityData(CQ_ALL);
    source->GetData().Calculate(CQ_ALL);
    _sourceList.push_back(source);
}

void Rtp_Quality_Monitor::Unregister(Rtp_Quality_Source* source) {
    source->UpdateQualityData(CQ_ALL);
    source->GetData().Calculate(CQ_ALL);
    List <Rtp_Quality_Source*>::iterator it = std::find(_sourceList.begin(), _sourceList.end(), source);
    assert(it != _sourceList.end());
    _sourceList.erase(it);
    RecordData(source);
}

void Rtp_Quality_Monitor::SetMinJitter(int minJitter) {
    _minJitter = minJitter;
}

void Rtp_Quality_Monitor::SetPackingFactor(int packingFactor) {
    _packingFactor = packingFactor;
}

char* FormatData(char* str, float val, const char* format) {
    if (val < 0)
	sprintf(str, "-");
    else if (val < 1000) 
	sprintf(str, format, val);
    else {
	char modformat[32];
	strcpy(modformat, format);
	if (val < 1000000) {
	    val /= 1000;
	    sprintf(str, strcat(modformat, "K"), val);
	}
	else { 
	    val /= 1000000;
	    sprintf(str, strcat(modformat, "M"), val);
	}
    }
    return str;
}

void Rtp_Quality_Monitor::PrintActiveData(Term_Session* session) {
    char title[8];

    char calls[10];
    char rx[30], rx_rtt[10], rx_jitter[10], rx_loss[10];
    char rx_packets[10], rx_r[10];
    char tx[30], tx_rtt[10], tx_jitter[10], tx_loss[10];
    char tx_packets[10], tx_r[10];

    Rtp_Quality_Data output;
    UpdateActiveData(CQ_ALL);
    output.Update(CQ_ALL, _active_data);
    sprintf(title, "active:");

    FormatData(calls, output._count, "%6.0f");
    
    FormatData(rx_rtt, output._rx_rtt.GetValue(), "%3.0f");
    FormatData(rx_jitter, output._rx_jitter.GetValue(), "%3.0f");
    FormatData(rx_loss, output._rx_loss.GetValue(), "%4.1f%%");
    FormatData(rx_packets, output._rx_packets.GetValue(), "%6.0f");
    FormatData(rx_r, E_Model::Instance()->Calculate_R(output._rx_itot.GetValue()), "%4.0f");

    FormatData(tx_rtt, output._tx_rtt.GetValue(), "%3.0f");
    FormatData(tx_jitter, output._tx_jitter.GetValue(), "%3.0f");
    FormatData(tx_loss, output._tx_loss.GetValue(), "%4.1f%%");
    FormatData(tx_packets, output._tx_packets.GetValue(), "%6.0f");
    FormatData(tx_r, E_Model::Instance()->Calculate_R(output._tx_itot.GetValue()), "%4.0f");
    
    sprintf(rx, "%4s %4s %5s %7s %4s", rx_rtt, rx_jitter, rx_loss, rx_packets, rx_r);
    sprintf(tx, "%4s %4s %5s %7s %4s", tx_rtt, tx_jitter, tx_loss, tx_packets, tx_r);

    session->Sendf("  %7s %7s | %28s | %28s\n", title, calls, rx, tx);
}

void Rtp_Quality_Monitor::PrintData(Term_Session* session, time_t start_t, time_t end, bool now) {

    Rtp_Quality_Data output;
    output._count = _calls.calculate_sum(start_t,end);
    if (output._count < 0) output._count = 0;

    output._rx_packets = _rx_packets.calculate_avg(start_t, end);
//    output._rx_bytes = _rx_bytes.calculate_avg(start_t, end);
    output._rx_rtt = _rx_rtt.calculate_avg(start_t, end);
    output._rx_jitter = _rx_jitter.calculate_avg(start_t, end);
    output._rx_loss = _rx_loss.calculate_avg(start_t, end);
//    output._rx_fracloss = _rx_fracloss.calculate_avg(start_t, end);
//    output._rx_is = _rx_is.calculate_avg(start_t, end);
//    output._rx_id = _rx_id.calculate_avg(start_t, end);
//    output._rx_ie = _rx_ie.calculate_avg(start_t, end);
    output._rx_itot = _rx_itot.calculate_avg(start_t, end);

    output._tx_packets = _tx_packets.calculate_avg(start_t, end);
//    output._tx_bytes = _tx_bytes.calculate_avg(start_t, end);
    output._tx_rtt = _tx_rtt.calculate_avg(start_t, end);
    output._tx_jitter = _tx_jitter.calculate_avg(start_t, end);
    output._tx_loss = _tx_loss.calculate_avg(start_t, end);
//    output._tx_fracloss = _tx_fracloss.calculate_avg(start_t, end);
//    output._tx_is = _tx_is.calculate_avg(start_t, end);
//    output._tx_id = _tx_id.calculate_avg(start_t, end);
//    output._tx_ie = _tx_ie.calculate_avg(start_t, end);
    output._tx_itot = _tx_itot.calculate_avg(start_t, end);
    
    char title[8];

    char calls[10];
    char rx[30], rx_rtt[10], rx_jitter[10], rx_loss[10];
    char rx_packets[10], rx_r[10];
    char tx[30], tx_rtt[10], tx_jitter[10], tx_loss[10];
    char tx_packets[10], tx_r[10];

    struct tm* the_time;
    int start_hour, end_hour;
    if (now) {
	the_time = localtime(&start_t); start_hour = the_time->tm_hour;
        sprintf(title, "%02d-now:", start_hour);
    }
    else {
	the_time = localtime(&start_t); start_hour = the_time->tm_hour;
	the_time = localtime(&end); end_hour = the_time->tm_hour;
	sprintf(title, "%02d-%02d :", start_hour, end_hour);
    }

    FormatData(calls, output._count, "%6.0f");
    
    FormatData(rx_rtt, output._rx_rtt.GetValue(), "%3.0f");
    FormatData(rx_jitter, output._rx_jitter.GetValue(), "%3.0f");
    FormatData(rx_loss, output._rx_loss.GetValue(), "%4.1f%%");
    FormatData(rx_packets, output._rx_packets.GetValue(), "%6.0f");
    FormatData(rx_r, E_Model::Instance()->Calculate_R(output._rx_itot.GetValue()), "%4.0f");

    FormatData(tx_rtt, output._tx_rtt.GetValue(), "%3.0f");
    FormatData(tx_jitter, output._tx_jitter.GetValue(), "%3.0f");
    FormatData(tx_loss, output._tx_loss.GetValue(), "%4.1f%%");
    FormatData(tx_packets, output._tx_packets.GetValue(), "%6.0f");
    FormatData(tx_r, E_Model::Instance()->Calculate_R(output._tx_itot.GetValue()), "%4.0f");
    
    sprintf(rx, "%4s %4s %5s %7s %4s", rx_rtt, rx_jitter, rx_loss, rx_packets, rx_r);
    sprintf(tx, "%4s %4s %5s %7s %4s", tx_rtt, tx_jitter, tx_loss, tx_packets, tx_r);

    session->Sendf("  %7s %7s | %28s | %28s\n", title, calls, rx, tx);
}

void Rtp_Quality_Monitor::UpdateActiveData(CallQualityParameter param) {
    _active_data.Reset();

    // Scan all registered statistics source
    List <Rtp_Quality_Source*>::iterator it = _sourceList.begin(), end = _sourceList.end();
    ACQUIRE_SYSTEM_LOCK();
    for (; it != end; it++) {
	Rtp_Quality_Source* source = *it;
	source->UpdateQualityData(param);
	if (source->Ready()) {
	    source->GetData().Calculate(param);
	    _active_data.Update(param, source->GetData());
	}
    }
    RELEASE_SYSTEM_LOCK();
}

void Rtp_Quality_Monitor::Show(Term_Session* session, int interval) {
    if (interval <= 0 || interval > 24) return;
    
    session->Sendf("  Interface Quality:\n"
		   "            Calls |                           Rx |                           Tx\n"
		   "                  |  RTT Jitt  Lost Packets    R |  RTT Jitt  Lost Packets    R\n");


    PrintActiveData(session);

    unsigned currentHour;
    time_t ltime;
    struct tm* now;
    time(&ltime);
    now = localtime(&ltime);
    currentHour = now->tm_hour;
    now->tm_min = 0;
    now->tm_sec = 0;
    ltime = mktime(now);

    int i, n = 24 / interval + 1; if ((24 % interval) > 0) n++; 
    time_t start_t = ltime - (currentHour%interval)*3600;
    time_t end_t = ltime + 3600;

    for (i = 0; i < n; i++) {
	PrintData(session, start_t, end_t, i == 0);
	end_t = start_t;
	if (i == n-2) start_t = start_t - (interval-(currentHour%interval)-1)*3600;
	else start_t = start_t - interval*3600;
	if (end_t == start_t) break;
    }
}

Quality_Value& Rtp_Quality_Monitor::GetActiveValue(CallQualityParameter param, Quality_Value& active_value) {
    
    active_value.Reset();
    
    // Scan all registered statistics source
    List <Rtp_Quality_Source*>::iterator it;
    for (it = _sourceList.begin(); it != _sourceList.end(); it++) {
	Rtp_Quality_Source* source = *it;
	source->UpdateQualityData(param);
	if (source->Ready()) {
	    source->GetData().Calculate(param);
	    active_value.add_avg(source->GetQuality(param));
	}
    }
    return active_value;
}

float Rtp_Quality_Monitor::GetQuality(CallQualityParameter param, time_t start_t, time_t end_t, bool active) {

    Quality_Value active_value, value;

    if (start_t && end_t) {
	switch (param) {
	case CQ_RX_PACKETS:
	    value._value = _rx_packets.calculate_avg(start_t, end_t, &value._count);
	    break;
	case CQ_RX_BYTES:
	    value._value = _rx_bytes.calculate_avg(start_t, end_t, &value._count);
	    break;
	case CQ_RX_RTT:
	    value._value = _rx_rtt.calculate_avg(start_t, end_t, &value._count); 
	    break;
	case CQ_RX_JITTER:
	    value._value = _rx_jitter.calculate_avg(start_t, end_t, &value._count);
	    break;
	case CQ_RX_LOSS:
	    value._value = _rx_loss.calculate_avg(start_t, end_t, &value._count);
	    break;
	case CQ_RX_FRACLOSS:
	    value._value = _rx_fracloss.calculate_avg(start_t, end_t, &value._count);
	    break;
	case CQ_RX_IS:
	    value._value = _rx_is.calculate_avg(start_t, end_t, &value._count);
	    break;
	case CQ_RX_ID:
	    value._value = _rx_id.calculate_avg(start_t, end_t, &value._count);
	    break;
	case CQ_RX_IE:
	    value._value = _rx_ie.calculate_avg(start_t, end_t, &value._count);
	    break;
	case CQ_RX_ITOT:
	    value._value = _rx_itot.calculate_avg(start_t, end_t, &value._count);
	    break;

	case CQ_TX_PACKETS:
	    value._value = _tx_packets.calculate_avg(start_t, end_t, &value._count);
	    break;
	case CQ_TX_BYTES:
	    value._value = _tx_bytes.calculate_avg(start_t, end_t, &value._count);
	    break;
	case CQ_TX_RTT:
	    value._value = _tx_rtt.calculate_avg(start_t, end_t, &value._count); 
	    break;
	case CQ_TX_JITTER:
	    value._value = _tx_jitter.calculate_avg(start_t, end_t, &value._count);
	    break;
	case CQ_TX_LOSS:
	    value._value = _tx_loss.calculate_avg(start_t, end_t, &value._count);
	    break;
	case CQ_TX_FRACLOSS:
	    value._value = _tx_fracloss.calculate_avg(start_t, end_t, &value._count);
	    break;
	case CQ_TX_IS:
	    value._value = _tx_is.calculate_avg(start_t, end_t, &value._count);
	    break;
	case CQ_TX_ID:
	    value._value = _tx_id.calculate_avg(start_t, end_t, &value._count);
	    break;
	case CQ_TX_IE:
	    value._value = _tx_ie.calculate_avg(start_t, end_t, &value._count);
	    break;
	case CQ_TX_ITOT:
	    value._value = _tx_itot.calculate_avg(start_t, end_t, &value._count);
	    break;


	default:
	    return -1.0f;
	}
    }

    if (active) {
	GetActiveValue(param, active_value);
	value.add_avg(active_value);
    }

    return value.GetValue();

}
