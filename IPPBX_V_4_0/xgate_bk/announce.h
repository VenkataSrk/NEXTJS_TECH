/* $Header: /xGate/announce.h 3     4/24/03 5:58p Bennylp $
 *   
 * XGATE
 * (C) 2001 Switchlab, Ltd
 *
 * Announcement resource
 *
 * $Log: /xGate/announce.h $
 * 
 * 3     4/24/03 5:58p Bennylp
 * Fix name inconsistency: XG_HAS_ changed to XGATE_HAS_
 * 
 * 2     11/04/02 5:29p Bennylp
 * Added device name.
 * 
 * 1     10/30/02 4:28p Bennylp
 * Initial version
 */

#ifndef __ANNOUNCE_RES_H__
#define __ANNOUNCE_RES_H__

#include "xgconfig.h"

#ifdef XGATE_HAS_ANNOUNCE_RES

#include "cfg.h"
#include "ServiceProvider.h"
#include "switching.h"

class Announce_Res;
class Announce_API;

///////////////////////////////////////////////////////////////////////////////
/** Base class for device/channel capable of being used to play tone/voice.
 */
class Announce_Device : public SupportSwitching
{
public:
    enum Type
    {
	TYPE_VOICE,
	TYPE_TONE_GEN,
    };

    enum Tone_Sig
    {
	TONE_DTMF,
	TONE_MF,
	TONE_R1,
	TONE_R2_IN,
	TONE_R2_OUT,
    };

    enum Voice_Codec
    {
	VOICE_PCMA,
	VOICE_PCMU,
	VOICE_G726,
    };

    Announce_Device(Announce_Res *res, SwitchDevice *swdev);
    virtual ~Announce_Device();

    const Announce_Res *get_res() const;

    virtual bool play_voice(Voice_Codec codec, const char *path) = 0;
    virtual bool play_tone(Tone_Sig tone_sig, const char *digits) = 0;
    virtual void stop() = 0;
    virtual const char *get_name() const = 0;
    virtual const char *get_status() const = 0;

private:
    Announce_Res    *res_;
};


///////////////////////////////////////////////////////////////////////////////
/** The announcement resource in xGate.
 */
class Announce_Res : public ServiceProvider
{
public:
    Announce_Res(const char *name);
    ~Announce_Res();

    /*
     * Attributes.
     */
    const char			*get_name() const;
    const char			*get_board() const;
    Announce_Device::Type	 get_type() const;
    Announce_Device::Voice_Codec get_voice_codec() const;
    const char			*get_voice_path() const;
    Announce_Device::Tone_Sig	 get_tone_sig() const;
    const char			*get_tone_digits() const;
    bool			 is_enabled() const;
    bool			 is_running() const;
    Announce_Device		*get_device();

    /*
     * xGate interface
     */
    virtual bool    Load();
    virtual bool    Unload();
    virtual bool    CommitConfiguration();
    virtual bool    OnShowStatus(String & output);
    virtual STATUS  OnShowConfig(const Cfg_Tree_Entry *entry, String &output, 
				const String &indent, int indent_size) const;
    virtual void    DoTermCmd(Term_Session *, bool no, Term_Cmd *cmd);
    Cfg_Tree_Group *GetConfig();

private:
    Announce_Device		*dev_;
    String			 name_;
    String			 board_;
    Announce_Device::Type	 type_;
    Announce_Device::Voice_Codec voice_codec_;
    String			 voice_path_;
    Announce_Device::Tone_Sig	 tone_sig_;
    String			 tone_digits_;
    Cfg_Tree_Group		*cfg_;

    bool	     is_enabled_, is_running_;

    bool Start();
    bool Stop();
};


///////////////////////////////////////////////////////////////////////////////
/** The announcement resource manager.
 */
class Announce_Mgr : public ServiceProvider
{
public:
    static Announce_Mgr *Create();
    static Announce_Mgr *Instance();

    Announce_Mgr();
    ~Announce_Mgr();

    virtual bool Load();
    virtual bool Unload();
    virtual bool CommitConfiguration();
    virtual void DoTermCmd(Term_Session *, bool no, Term_Cmd *cmd);

    Announce_Res *     FindResource(const char *name);
    Cfg_Tree_Group *   GetCfgEntry();

private:
    static Announce_Mgr *instance_;

    Cfg_Tree_Group	  *cfg_;
    Vector<Announce_Res *> res_;
    Announce_API	  *api_;

    Announce_Res **find_resource(const char *name);
};

#endif	/* XGATE_HAS_ANNOUNCE_RES */

#endif	/* __ANNOUNCE_RES_H__ */

