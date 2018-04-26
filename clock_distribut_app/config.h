#ifndef _CONFIG_H_
#define _CONFIG_H_



#define SysTrace(level, fmt, ...)  \
    printf("["#level"][%s][%s][%d]:" fmt "\r\n", __FILE__,__FUNCTION__ ,__LINE__, ## __VA_ARGS__)

#define SysDebugTrace(fmt, ...)  SysTrace(Debug, fmt, ## __VA_ARGS__)
#define SysInfoTrace(fmt, ...)   SysTrace(Info, fmt, ## __VA_ARGS__)
#define SysNoticeTrace(fmt, ...) SysTrace(Notice, fmt, ## __VA_ARGS__)
#define SysWarnTrace(fmt, ...)   SysTrace(Warn, fmt, ## __VA_ARGS__)
#define SysErrorTrace(fmt, ...)  SysTrace(Error, fmt, ## __VA_ARGS__)
#define SysFatalTrace(fmt, ...)  SysTrace(Fatal, fmt, ## __VA_ARGS__)
typedef		unsigned int		uint32;
typedef		unsigned short		uint16;
typedef		unsigned char		uint8;
typedef		int					int32;
typedef		short				int16;
typedef		char				int8;


#endif
