#ifndef _MBC_PORT_ALLOC_
#define _MBC_PORT_ALLOC_

#define HOST_AUDIO_PORT_START   10000
#define HOST_AUDIO_PORT_SPAN    20000
int g_audioPort=HOST_AUDIO_PORT_START;
#define HOSTGST_GET_AUDIO_PORT() ((g_audioPort) > (HOST_AUDIO_PORT_SPAN) ? \
    (g_audioPort = HOST_AUDIO_PORT_START) : (g_audioPort = g_audioPort+2));

#define HOST_LOOPBACK_PORT_START   21000
#define HOST_LOOPBACK_PORT_SPAN    30000
int g_loopBackPort=HOST_LOOPBACK_PORT_START;
#define HOSTGST_GET_LOOPBACK_PORT() ((g_loopBackPort) > (HOST_LOOPBACK_PORT_SPAN) ? \
    (g_loopBackPort = HOST_LOOPBACK_PORT_START) : (g_loopBackPort = g_loopBackPort+2));

#define HOST_VIDEO_PORT_START   31000
#define HOST_VIDEO_PORT_SPAN    40000
int g_videoPort=HOST_VIDEO_PORT_START;
#define HOSTGST_GET_VIDEO_PORT() ((g_videoPort) > (HOST_VIDEO_PORT_SPAN) ? \
    (g_videoPort = HOST_VIDEO_PORT_START) : (g_videoPort = g_videoPort+2));

#define HOST_VIDEO_LOOPBACK_PORT_START   41000
#define HOST_VIDEO_LOOPBACK_PORT_SPAN    50000
int g_videoLoopBackPort=HOST_VIDEO_LOOPBACK_PORT_START;
#define HOSTGST_GET_VIDEO_LOOPBACK_PORT() ((g_videoLoopBackPort) > (HOST_VIDEO_LOOPBACK_PORT_SPAN) ? \
    (g_videoLoopBackPort = HOST_VIDEO_LOOPBACK_PORT_START) : (g_videoLoopBackPort = g_videoLoopBackPort+2));

// TODO: consider converting into a class

#endif // _MBC_PORT_ALLOC_
