#ifndef _DATA_ASSIST_H_
#define _DATA_ASSIST_H_


//const define

#define MAX_PACKET_LEN 168
#define AUDIO_PACKET_HEAD_LEN 8
#define MAX_CHANNEL_NUM 32
#define MAX_FILE_NAME_LEN 128
#define LOCAL_N_WEEKS 7		///< days of week
#define LOCAL_N_TSECT	6		///< time sect num
#define LOCAL_MD_REGION_ROW 32

#define  NAME_PASSWORD_LEN  64
#define  DECODER_NAME_LEN	64
#define  EMAIL_ADDR_LEN  32
#define  N_MIN_TSECT 2
#define  N_ALARMSERVERKEY 5
#define  MAX_FILTERIP_NUM 64
#define  MAX_MAC_LEN 32
#define  MAX_PROTOCOL_LENGTH 32
#define  LOCAL_N_ALM_OUT  32
#define  LOCALSDK_MAX_DRIVER_PER_DISK 4
#define  LOCALSDK_MAX_DISK_PER_MACHINE 8
#define  LOCALSDK_MAX_CHANNEL_WIDTH 320
#define MAX_ENCODER_NUM  32
#define COM_TYPES 2 
#define LOCAL_MAX_ARSP_TYPE 5
#define MAX_DIRPATH_LENGTH 260
#define MAX_MSK_SIZE 16 //掩码数组的大小

enum Versions
{
	V_32DEV,
	V_64DEV,
};
typedef unsigned int						uint;			///< sizeof(uint) == sizeof(int) == 4
typedef unsigned char						uchar;	

//error code
enum LOCALSDK_RETURN_CODE	//
{
	LOCALSDK_SUCCESS	= 0,
	LOCALSDK_NOT_SUPPORT_STREAM = -1000,
	LOCALSDK_NOT_SUPPORT_CHANNEL = -1001,
	LOCALSDK_CAPTURE_FAIL = -1002,
	LOCALSDK_NO_INIT = -1003,
	LOCALSDK_ILLEGAL_PARAM = -1004,
	LOCALSDK_TIMEOUT = -1005,
	LOCALSDK_INVALID_HANDLE = -1006,
	LOCALSDK_IN_PLAYING = -1007,
	LOCALSDK_NOT_FOUND_FILE = -1008,
	LOCALSDK_GET_DATA_FAIL = -1009,
	LOCALSDK_NOT_IN_PLAYING = -1010,
	LOCALSDK_CREATE_FILE_FAIL = -1011,
	LOCALSDK_SET_DATA_FAIL = -1012,
	LOCALSDK_VERSION_DISMATCH = -1013,
	LOCALSDK_NOT_SUPPORT_FUNCTION = -1014,
	LOCALSDK_OVER_SIZE = -1015,
	LOCALSDK_NEED_REBOOT = -1016
};

/// 报警模式
enum AlarmTypes
{
	ALARM_CFG,			// 配置 
	ALARM_HAND,		    // 手动 
	ALARM_CLOSE,		// 关闭 
};

/// capture channel type
enum SDK_CaptureChannelTypes 
{
	SDK_CAPTURE_CHN_MAIN = 0,		///< main stream 1
	SDK_CAPTURE_CHN_2END = 1,		///< extra stream 2
	SDK_CAPTURE_CHN_3IRD = 2,		///< extra stream 3
	SDK_CAPTURE_CHN_4RTH = 3,		///< extra stream 4
	SDK_CAPTURE_CHN_JPEG = 4,		///< JPEG stream
	SDK_CAPTURE_CHN_NR
};

enum SDK_video_standard_t {
	VIDEO_STANDARD_PAL = 0,	///< PAL制式。
	VIDEO_STANDARD_NTSC,	///< NTSC制式。
};

/// FrameType
enum FRAME_TYPE
{
    FRAME_VIDEO = 1,
    FRAME_AUDIO = 2,
    FRAME_DATA = 3
};

/// FrameSubType
enum FRAME_SUB_TYPE
{
    I_FRAME = 0,
    P_FRAME = 1,
    B_FRAME = 2,
    S_FRAME = 3,
    DATA_TEXT = 5,
    DATA_TNTL = 6,
    VIRTUAL_I_FRAME = 7
};

enum NET_ISP
{
    ISP_AUTO,     //自动选择
    ISP_TELCOM,   //电信
    ISP_UNIONCOM, //联通
    ISP_CNC,      //网通
    ISP_MOBILE,   //移动，铁通
    ISP_USER      //自定义
};

/// 码流类型
enum ENCODE_TYPE
{
    StreamTypeEmpty = 0,
    StreamTypeH264 = 2,
    StreamTypeJpeg = 3,
    StreamTypeGeneral = 4,
    StreamTypeH265 = 5,
    StreamTypePCM8 = 7,
    StreamTypeStd = 8
};


////////////////////// struct define ///////////////
typedef struct
{
	int  year;		///< year  
	int  month;		///< month，January = 1, February = 2, and so on.   
	int  day;		///< day   
	int  wday;		///< weekday, Sunday = 0, Monday = 1, and so on   
	int  hour;		///< hour   
	int  minute;	///< min   
	int  second;	///< sec   
	int  isdst;		///< 1: using daylight saving time
} LOCALSDK_TIME, *LPLOCALSDK_TIME;

typedef struct LocalSDK_DeviceInfo_t
{
	unsigned int	uiIP; 				//device IP  
	unsigned short	usPort;				//HTTP port
	char    szDeviceName[32];			//device name
	char	szDeviceType[32];			//device type
	unsigned char	nVideoNum;			//video channels
	unsigned char	nAudioNum;			//audio channels
	unsigned char	nSubstreamNum;		//0:not support sub-stream
	unsigned char	nPlaybackNum;		//playback channels
	unsigned char	nAlarmInNum;		//alarm-in channels
	unsigned char	nAlarmOutNum;		//alarm-out channels
	unsigned char	nHddNum;			//hdd num
	char szVersion[32];                 //Version of Sofia
	char szSerialNo[17];                //SerualNum of Sofia
	uchar maxchnum;			//最大通道号
	uchar videostandard;			//视频制式: SDK_video_standard_t
	unsigned char	reserved[12];		//reserve
}LOCALSDK_DEVICE_INFO;

typedef struct LocalSDK_DeviceInfo_t_V2
{
	unsigned int	uiIP; 				//device IP  
	unsigned short	usPort;				//HTTP port
	char    szDeviceName[32];			//device name
	char	szDeviceType[32];			//device type
	unsigned char	nVideoNum;			//video channels
	unsigned char	nAudioNum;			//audio channels
	unsigned char	nSubstreamNum;		//0:not support sub-stream
	unsigned char	nPlaybackNum;		//playback channels
	unsigned char	nAlarmInNum;		//alarm-in channels
	unsigned char	nAlarmOutNum;		//alarm-out channels
	unsigned char	nHddNum;			//hdd num
	char szVersion[64];                 //Version of Sofia
	char szSerialNo[17];                //SerualNum of Sofia
	uchar maxchnum;			//最大通道号
	uchar videostandard;			//视频制式: 0表示P制; 1表示NTSC制
	unsigned char	reserved[12];		//reserve
}LOCALSDK_DEVICE_INFO_V2;

/// 设备信息
typedef struct SystemInformation
{
	char sSoftWareVersion[64];	///< 软件版本信息
	char sHardWareVersion[64];	///< 硬件版本信息
	char sEncryptVersion[64];	///< 加密版本信息
	LOCALSDK_TIME tmBuildTime;	///< 软件创建时间
	char sSerialNo[64];			///< 设备序列号
	int iVideoInChannel;	///< 视频输入通道数
	int iVideoOutChannel;	///< 视频输出通道数
	int iAlarmInChannel;	///< 报警输入通道数
	int iAlarmOutChannel;	///< 报警输出通道数
	int iTalkInChannel;		///< 对讲输入通道数
	int iTalkOutChannel;	///< 对讲输出通道数	
	int iExtraChannel;		///< 扩展通道数	
	int iAudioInChannel;	///< 音频输入通道数
	int iCombineSwitch;		///< 组合编码通道分割模式是否支持切换
	int iDigChannel;		///<数字通道数
	uint uiDeviceRunTime;	///<系统运行时间
	char sHardWare[64];	///<设备型号
	char uUpdataTime[64];	///<更新日期
	uint uUpdataType;	///<更新内容
} LOCALSDK_DEVICE_SYSTEM_INFO;


typedef struct 
{
	unsigned char *pHeader;				///< buffer header address
	unsigned char *pContent;            ///< frame data address
	unsigned long nLength;              ///< packet len
	unsigned long nFrameLength;         ///< frame len

	unsigned int FrameType;            ///< refer to FRAME_TYPE
	unsigned int nSubType;             ///< refer to FRAME_SUB_TYPE

	unsigned int nEncodeType;		///ENCODE_TYPE

	unsigned long nYear;				///< timestamp
	unsigned long nMonth;
	unsigned long nDay;
	unsigned long nHour;
	unsigned long nMinute;
	unsigned long nSecond;
	unsigned long nTimeStamp;	

	unsigned int  nFrameRate;			///< frame rate
	int nWidth;							///< video resolution，width and height
	int nHeight;
	unsigned long nRequence;

	unsigned int nChannels;
	unsigned int nBitsPerSample;		///< audio sample bit depth
	unsigned int nSamplesPerSecond;		///< audio sample rate

	unsigned long nParam1;		// 
	unsigned long nParam2;		// 
	unsigned long version;		// 版本号
	unsigned long reserved[10]; // 扩展用
} SDK_H264_FRAME_INFO;

/// 录像类型
enum RecordTypes
{
	RECORD_TYPE_COMMON = 0x00,		///< 普通录像
	RECORD_TYPE_ALARM = 0x01,		///< 报警录像
	RECORD_TYPE_MOTION = 0x02,		///< 动检录像
	RECORD_TYPE_HAND = 0x03,		///< 手动录像
	RECORD_TYPE_SNAP = 0x04,		///
	RECORD_TYPE_NR
};

enum LINK_FILE_TYPE //因为何种原因联动产生的录像或图片
{
	LINK_ALL,        //不分原因,所有
	LINK_LOCALALARM, //本地报警输入
	LINK_VIDEOALARM, //视频丢失、移动侦测等于视频相关的报警
	LINK_ALLALARM,   //上面两种报警中任一种发生
	LINK_HANDLE,     //手动录像或抓图
	LINK_NORMAL     //配置录像中的普通情况下的录像
};
//search condtion
typedef struct 
{
	int nChannel;			//channel id
	int nFileType;			//file type, refer to LINK_FILE_TYPE
	LOCALSDK_TIME stBeginTime;	//begin time
	LOCALSDK_TIME stEndTime;	//end time
    int StreamType;             // 0 主码流; 1 辅码流;
}LOCALSDK_FINDINFO;

/// 驱动器类型
enum FileSystemDriverTypes
{
	DRIVER_TYPE_BASE	= 0,	///< 驱动器类型起始值
	DRIVER_READ_WRITE	= 0,	///< 读写驱动器
	DRIVER_READ_ONLY	= 1,	///< 只读驱动器
	DRIVER_EVENTS		= 2,	///< 事件驱动器
	DRIVER_REDUNDANT	= 3,	///< 冗余驱动器
	DRIVER_SNAPSHOT		= 4,	///< 快照驱动器
	DRIVER_TYPE_NR		= 5,	///< 驱动器类型个数
	DRIVER_UNUSED		= 0xff	///< 没有使用的驱动器结构
};

typedef struct 
{
	int nChannel;			//通道号
	unsigned int 		nFileType;		//文件类型(掩码), 见RecordTypes
	unsigned int 		nDriverTypes;		//录像为3，抓图为1 << DRIVER_SNAPSHOT，其他暂不支持，以后可能会扩展
	LOCALSDK_TIME stBeginTime;	//开始时间
	LOCALSDK_TIME stEndTime;	//结束时间
	int StreamType;				//主辅码流
	int MaxFileNum;
	uint resu[2];
}LOCALSDK_FINDINFOV2;

//record file info
typedef struct 
{
	int nChannel;				//channel id
	unsigned int nSize;			//file size
	char sFileName[128];		///< file name
	LOCALSDK_TIME stBeginTime;	///< begin time
	LOCALSDK_TIME stEndTime;	///< end time
}LOCALSDK_FILE_DATA;

typedef struct 
{
	LOCALSDK_FILE_DATA *pFileData;  //buffer for search return
	int nMaxSize;                   //max item num
	int nRetSize;                   //actually return item num
}LOCALSDK_FIND_FILE_RET;

typedef struct 
{
	int nFrameType : 4;             //fram type, refer to FRAME_TYPE
	int nFrameSubType : 4;          //fram sub type,refer to FRAME_SUB_TYPE
	int nFrameLen : 24;             //fram len
}FRAME_INDEX;

typedef struct 
{
	int nEnable;
	unsigned int	uiServerIP; 		//server IP 
	unsigned short	usPort;				//port 
	unsigned short	usDownlodPort;		//download port
	int nSyncTime;                      //is sync time from server
	int nStreamType;                    //0: main stream 1: extra stream
	char szPassword[16];
	char szSID[32]; 
}LOCALSDK_XINGWAN_INFO;

typedef struct 
{
	int nEnable;
	char szServerIP[64]; 		//server IP 
	unsigned int 	uiPort;		//port 
	unsigned int    uiDevID;    //device id
	unsigned int    uiDisplayID;
	int nGPSHeartBeat;          //gps hearbeat
	int nSysHeartBeat;          //svr hearbeat
	char szNoUsed[64]; 
}LOCALSDK_WELLSUN_INFO;

typedef struct 
{
	int	nBrightness;		///< Brightness	0-100
	int	nContrast;			///< Contrast	0-100
	int	nSaturation;		///< Saturation	0-100
	int	nHue;				///< Hue	0-100
	int	mGain;				///< Gain	0-100 
	int	mWhitebalance;		///< Whitebalance，bit7: 1-enable 0-disable. bit0~bit6:0x0-high 0x1-mid 0x2-low
}LOCALSDK_VIDEOCOLOR;

enum CaptureBitrateControlType 
{
	CAPTURE_BITRATE_CBR,			///< fixed bitrate
	CAPTURE_BITRATE_VBR,			///< Variable bitrate
	CAPTURE_BITRATE_MBR,			///< mix bitrate。
	CAPTURE_BITRATE_NR				///< 
};

enum CaptureCompressType 
{
	CAPTURE_COMPRESS_DIVX_MPEG4,	///< DIVX MPEG4。
	CAPTURE_COMPRESS_MS_MPEG4,		///< MS MPEG4。
	CAPTURE_COMPRESS_MPEG2,			///< MPEG2。
	CAPTURE_COMPRESS_MPEG1,			///< MPEG1。
	CAPTURE_COMPRESS_H263,			///< H.263
	CAPTURE_COMPRESS_MJPG,			///< MJPG
	CAPTURE_COMPRESS_FCC_MPEG4,		///< FCC MPEG4
	CAPTURE_COMPRESS_H264,			///< H.264
	CAPTURE_COMPRESS_H265,			///< H.265
	CAPTURE_COMPRESS_NR				///< 
};


/// iamge quality
enum CaptureImageQuality
{
	CAPTURE_IMG_QUALITY_WOREST = 1,		///< worest
	CAPTURE_IMG_QUALITY_WORSE = 2,		///< worse
	CAPTURE_IMG_QUALITY_GENERAL = 3,	///< general
	CAPTURE_IMG_QUALITY_GOOD = 4,		///< good
	CAPTURE_IMG_QUALITY_BETTER = 5,		///< better
	CAPTURE_IMG_QUALITY_BEST = 6,		///< best
	CAPTURE_IMG_QUALITY_NR = 7
};

typedef struct 
{
	int		iCompression;			//  refer to CaptureCompressType
	int		iResolution;			//  refer to capture_size_t	
	int		iBitRateControl;		//  refer to capture_brc_t
	int		iQuality;				//  refer to CaptureImageQuality
	int		nFPS;					//  frame per second 2~12
	int		nBitRate;				//  0-4096k
	int		iGOP;					//  GOP
}LOCALSDK_VIDEO_FORMAT;

typedef struct 
{
	int nMainAudio;                // Main stream audio flag: 0 - disable 1 - enable
	int nExtraVideo;               // Extra stream video flag: 0 - disable 1 - enable
	int nExtraAudio;               // Extra stream audio flag: 0 - disable 1 - enable
	LOCALSDK_VIDEO_FORMAT stMain;  // Main stream
	LOCALSDK_VIDEO_FORMAT stExtra; // Extra stream
}LOCALSDK_VIDEOENCODE;

typedef struct 
{
	int nMainAudio;                // Main stream audio flag: 0 - disable 1 - enable
	int nExtraVideo;               // Extra stream video flag: 0 - disable 1 - enable
	int nExtraAudio;               // Extra stream audio flag: 0 - disable 1 - enable
	LOCALSDK_VIDEO_FORMAT stMain;  // Main stream
	LOCALSDK_VIDEO_FORMAT stExtra; // Extra stream
	LOCALSDK_VIDEO_FORMAT stSnap;  // Snap stream
}LOCALSDK_VIDEOENCODEV2;

/*解码器连接类型*/
enum DecorderConnType
{
	CONN_SINGLE = 0, 	/*单连接*/
	CONN_MULTI = 1,		/*多连接轮巡*/
	CONN_TYPE_NR,
};
enum DevType
{
	DEV_TYPE_IPC,
	DEV_TYPE_DVR,
	DEV_TYPE_HVR,
	DEV_TYPE_POEIPC,
	DEV_TYPE_NR
};

enum TransferProtocol_V2
{
	TRANSFER_PROTOCOL_NETIP,
	TRANSFER_PROTOCOL_ONVIF,
	TRANSFER_PROTOCOL_MAC,
	TRANSFER_PROTOCOL_NAT,
	TRANSFER_PROTOCOL_DAHUA,
	TRANSFER_PROTOCOL_NR_V2,
	TRANSFER_PROTOCOL_ONVIF_DEFAULT = 128,
	TRANSFER_PROTOCOL_ONVIF_NR_V2,
};

///< 解码器地址设置
typedef struct 
{
	bool Enable;						///< 是否开启
	char UserName[NAME_PASSWORD_LEN];	///< 
	char PassWord[NAME_PASSWORD_LEN];	///< 
	char Address[MAX_FILE_NAME_LEN];
	int Protocol;
	int Port;							///< 解码器连接端口
	int Channel;						///< 解码器连接通道号
	int Interval;                       ///< 轮巡的间隔时间(s),0:表示永久
	char ConfName[DECODER_NAME_LEN];	///<配置名称
	int DevType;						///<设备类型
	int StreamType;						///<连接的码流类型 SDK_CaptureChannelTypes
	char MainRtspUrl[MAX_FILE_NAME_LEN];		///<rtsp协议时表示前端设备的主码流地址
	char SubRtspUrl[MAX_FILE_NAME_LEN];		///<rtsp协议时表示前端设备的辅码流地址
	char MacAddr[MAX_MAC_LEN];			///<设备MAC地址
	char SerialNo[MAX_MAC_LEN];			///<设备序列号
} NetDecorderConfigV3;


typedef struct 
{
	bool Enable;		/*数字通道是否开启*/		
	int ConnType;		/*连接类型，取 DecorderConnType 的值*/
	int TourIntv;		/*多连接时轮巡间隔*/
	unsigned int SingleConnId;	/*单连接时的连接配置ID, 从1开始，0表示无效*/
	bool EnCheckTime;	/*开启对时*/
	NetDecorderConfigV3 vDecorderConfigAll[MAX_ENCODER_NUM];
}LOCALSDK_NETDIGITCHNCONFIG;

enum PTZ_LINK_TYPES
{
	PTZ_LINK_NONE,			// none 
	PTZ_LINK_PRESET,		// goto preset
	PTZ_LINK_TOUR,			// start tour
	PTZ_LINK_PATTERN		// start pattern
};

//  PTZ link action
typedef struct 
{
	int iType;		// refer to PTZ_LINK_TYPES
	int iValue;		// value according to PTZ_LINK_TYPES
}PTZ_LINK;

typedef struct
{
	int enable;
	int startHour;
	int	startMinute;
	int	startSecond;
	int	endHour;
	int	endMinute;
	int	endSecond;
}TIME_SECTION;

/// worksheet
typedef struct
{	
	TIME_SECTION	tsSchedule[LOCAL_N_WEEKS][LOCAL_N_TSECT];
}WORK_SHEET;

/// event handler
typedef struct
{
	unsigned int	dwRecord;		// channel mask of record
	int		iRecordLatch;			// latch time：10～300 sec  	
	unsigned int	dwTour;					// channel mask of tour
	unsigned int	dwSnapShot;				// channel mask of snapshot 
	unsigned int	dwAlarmOut;				// channel mask of alarm-out 
	unsigned int	dwMatrix;				// channel mask of matrix
	int		iEventLatch;			// interval of alarm
	int		iAOLatch;				// alarm-out latch time: 10～300 sec  
	PTZ_LINK PtzLink[MAX_CHANNEL_NUM];		// ptz link action
	WORK_SHEET schedule;		// worksheet

	bool	bRecordEn;				// record enable flag
	bool	bTourEn;				// tour enable flag
	bool	bSnapEn;				// snapshot enable flag
	bool	bAlarmOutEn;			// alarm-out enable flag
	bool	bPtzEn;					// ptz link action enable flag
	bool	bTip;					// screen tip enable flag
	bool	bMail;					// send email enable flag
	bool	bMessage;				// send message enable flag
	bool	bBeep;					// beep enable flag
	bool	bVoice;					// voice enable flag
	bool	bFTP;					// ftp enable flag
	bool	bMatrixEn;				// matrix enable flag
	bool	bLog;					// log enable flag
	bool	bMessagetoNet;			// send message to net enable flag

	bool	bShowInfo;              // 是否在GUI上和编码里显示报警信息
	unsigned int	dwShowInfoMask;         // 要联动显示报警信息的通道掩码
	char	pAlarmInfo[NAME_PASSWORD_LEN];//要显示的报警信息

	bool    bShortMsg;              //发送短信
	bool    bMultimediaMsg;         //发送彩信
}EVENT_HANDLER;

typedef struct 
{
	bool bEnable;							// enable flag 
	int nLevel;								// Sensitivity: 1-6
	unsigned int mRegion[LOCAL_MD_REGION_ROW]; // Region of motion， bitmask for each line
	EVENT_HANDLER hEvent;					// motion detect handler 
}LOCALSDK_MOTION_DETECT;

///< 遮挡检测配置
typedef struct 
{
	bool	bEnable;		///< 遮挡检测开启
	int		iLevel;			///< 灵敏度：1～6
	EVENT_HANDLER hEvent;	///< 遮挡检测联动参数
}LOCALSDK_BLIND_DETECT;

///< 基本事件结构
typedef struct 
{
	bool bEnable;			///< 使能
	EVENT_HANDLER hEvent;	///< 处理参数
}LOCALSDK_GENERAL_CFG;

///< 本地报警配置
typedef struct 
{
	bool	bEnable;		///< 报警输入开关
	int		iSensorType;	///< 传感器类型常开 or 常闭
	EVENT_HANDLER hEvent;	///< 报警联动
}LOCALSDK_ALARM_CFG;

/// 事件处理参数
typedef struct
{
	uchar	dwRecord[MAX_MSK_SIZE];				// 录象掩码 
	int		iRecordLatch;			// 录像延时：10～300 sec  	 
	uchar	dwTour[MAX_MSK_SIZE];	// 轮巡掩码 	 
	uchar	dwSnapShot[MAX_MSK_SIZE];			// 抓图掩码  
	uchar	dwAlarmOut[MAX_MSK_SIZE];			// 报警输出通道掩码  
	uchar	dwMatrix[MAX_MSK_SIZE];				// 矩阵掩码  
	int		iEventLatch;			// 联动开始延时时间，s为单位 
	int		iAOLatch;				// 报警输出延时：10～300 sec  
	PTZ_LINK PtzLink[64];		// 云台联动项
	WORK_SHEET schedule;		// 录像时间段

	bool	bRecordEn;				// 录像使能 
	bool	bTourEn;				// 轮巡使能 
	bool	bSnapEn;				// 抓图使能 	
	bool	bAlarmOutEn;			// 报警使能 
	bool	bPtzEn;					// 云台联动使能 
	bool	bTip;					// 屏幕提示使能 	
	bool	bMail;					// 发送邮件 	
	bool	bMessage;				// 发送消息到报警中心 	
	bool	bBeep;					// 蜂鸣 	
	bool	bVoice;					// 语音提示 		
	bool	bFTP;					// 启动FTP传输 
	bool	bMatrixEn;				// 矩阵使能 
	bool	bLog;					// 日志使能
	bool	bMessagetoNet;			// 消息上传给网络使能 

	bool	bShowInfo;              // 是否在GUI上和编码里显示报警信息
	//uint	dwShowInfoMask;         // 要联动显示报警信息的通道掩码 
	uchar	dwShowInfoMask[MAX_MSK_SIZE];         // 要联动显示报警信息的通道掩码
	char	pAlarmInfo[NAME_PASSWORD_LEN];//要显示的报警信息

	bool    bShortMsg;              //发送短信
	bool    bMultimediaMsg;         //发送彩信
}EventHandler_V2;

typedef struct 
{
	bool bEnable;							// enable flag 
	int nLevel;								// Sensitivity: 1-6
	unsigned int mRegion[LOCAL_MD_REGION_ROW]; // Region of motion， bitmask for each line
	EventHandler_V2 hEvent;					// motion detect handler 
}LOCALSDK_MOTION_DETECT_V2;

///< 遮挡检测配置
typedef struct 
{
	bool	bEnable;		///< 遮挡检测开启
	int		iLevel;			///< 灵敏度：1～6
	EventHandler_V2 hEvent;	///< 遮挡检测联动参数
}LOCALSDK_BLIND_DETECT_V2;

///< 基本事件结构
typedef struct 
{
	bool bEnable;			///< 使能
	EventHandler_V2 hEvent;	///< 处理参数
}LOCALSDK_GENERAL_CFG_V2;

///< 本地报警配置
typedef struct 
{
	bool	bEnable;		///< 报警输入开关
	int		iSensorType;	///< 传感器类型常开 or 常闭
	EventHandler_V2 hEvent;	///< 报警联动
}LOCALSDK_ALARM_CFG_V2;

///< 本地报警输出配置
typedef struct
{
	int nAlarmOutType;		///< 报警输出类型: 配置,手动,关闭,见AlarmTypes
	int nAlarmOutStatus;    ///< 报警状态: 0:打开 1;闭合
}LOCALSDK_ALARMOUT_CFG;


///< 所有通道的报警输出配置
typedef struct
{
	LOCALSDK_ALARMOUT_CFG vAlarmOutConfigAll[LOCAL_N_ALM_OUT];
}LOCALSDK_ALARMOUT_CFG_ALL;

typedef struct
{
	LOCALSDK_ALARMOUT_CFG* vAlarmOutConfigAll;
	int maxChannelNum;
	int retChannelNum;
}LOCALSDK_ALARMOUT_CFG_ALL_V2;

typedef union {	//IP addr
	unsigned char	c[4];
	unsigned short	s[2];
	unsigned int	l;
} LOCALSDK_IPADDR;

///< 普通网络设置
typedef struct
{
	char HostName[NAME_PASSWORD_LEN];	///< 主机名
	LOCALSDK_IPADDR HostIP;		///< 主机IP
	LOCALSDK_IPADDR Submask;		///< 子网掩码
	LOCALSDK_IPADDR Gateway;		///< 网关IP
	int HttpPort;			///< HTTP服务端口
	int TCPPort;			///< TCP侦听端口
	int SSLPort;			///< SSL侦听端口
	int UDPPort;			///< UDP侦听端口
	int MaxConn;			///< 最大连接数
	int MonMode;			///< 监视协议 {"TCP","UDP","MCAST",…}
	int MaxBps;				///< 限定码流值
	int TransferPlan;		///< 传输策略 见TransferPolicy
	bool bUseHSDownLoad;	///< 是否启用高速录像下载测率	
	char sMac[MAX_MAC_LEN]; ///< MAC地址
}LOCALSDK_NETCOMMON;

///< 服务器结构定义
typedef struct
{
	char ServerName[NAME_PASSWORD_LEN];	///< 服务名
	LOCALSDK_IPADDR ip;						///< IP地址
	int Port;							///< 端口号
	char UserName[NAME_PASSWORD_LEN];		///< 用户名
	char Password[NAME_PASSWORD_LEN];		///< 密码	
	bool Anonymity;							///< 是否匿名登录
}LOCALSDK_REMOTE_CFG;

///< pppoe设置
typedef struct
{
	bool Enable;		///< 是否开启
	LOCALSDK_REMOTE_CFG Server;		///< PPPOE服务器
	LOCALSDK_IPADDR addr;		///< 拨号后获得的IP地址
}LOCALSDK_PPPOE_CFG;

typedef struct
{
	int	iDataBits;	// 数据位取值为5,6,7,8 
	int	iStopBits;	// 停止位
	int	iParity;	// 校验位
	int	iBaudRate;	// 实际波特率
}LOCALSDK_COMM_ATTRI;

// 云台设置
typedef struct
{
	char sProtocolName[MAX_PROTOCOL_LENGTH];	// 协议名称 	
	int	ideviceNo;				// 云台设备地址编号 	
	int	iNumberInMatrixs;		// 在矩阵中的统一编号	
	int iPortNo;				// 串口端口号	[1, 4] 	
	LOCALSDK_COMM_ATTRI dstComm;			// 串口属性 	
}LOCALSDK_PTZ_CFG;

// 串口配置
typedef struct 
{
	char sProtocolName[MAX_PROTOCOL_LENGTH];	// 串口协议:“Console” 
	int iPortNo;		// 端口号 
	LOCALSDK_COMM_ATTRI aCommAttri;		// 串口属性 
}LOCALSDK_CONFIG_COMM_X;

typedef struct
{
	LOCALSDK_CONFIG_COMM_X vCommConfig[COM_TYPES];
} LOCALSDK_COMM_CFG;


/// 录像模式种类
enum RecordModeTypes
{
	RECORD_MODE_CLOSED,		///< 关闭录像
	RECORD_MODE_MANUAL,		///< 手动录像
	RECORD_MODE_CONFIG,		///< 按配置录像
	RECORD_MODE_NR
};

///< 录像设置
typedef struct 
{
	int iPreRecord;			///< 预录时间，为零时表示关闭	
	bool bRedundancy;		///< 冗余开关
	int iPacketLength;		///< 录像打包长度（分钟）[1, 255]
	int iRecordMode;		///< 录像模式，见RecordModeTypes
	WORK_SHEET wcWorkSheet;				///< 录像时间段	
	unsigned int typeMask[LOCAL_N_WEEKS][LOCAL_N_TSECT];	///< 录像类型掩码，见enum RecordTypes
}LOCALSDK_RECORD_CFG;


/// 图片模式种类
enum SnapModeTypes
{
	SNAP_MODE_CLOSED,		///< 关闭抓图
	SNAP_MODE_MANUAL,		///< 手工抓图
	SNAP_MODE_CONFIG,		///< 按配置抓图
	SNAP_MODE_NR	
};

///< 图片设置
typedef struct 
{
	int iPreSnap;			///< 预抓图片数
	bool bRedundancy;		///< 冗余开关
	int iSnapMode;		///< 录像模式，见SnapModeTypes
	WORK_SHEET wcWorkSheet;				///< 录像时间段	
	unsigned int typeMask[LOCAL_N_WEEKS][LOCAL_N_TSECT];	///< 录像类型掩码，见enum RecordTypes
}LOCALSDK_SNAPSHOT_CFG;


///< 驱动器信息结构
typedef struct 
{
	int		iDriverType;		///< 驱动器类型
	bool	bIsCurrent;			///< 是否为当前工作盘
	unsigned int	uiTotalSpace;		///< 总容量，MB为单位
	unsigned int	uiRemainSpace;		///< 剩余容量，MB为单位
	int		iStatus;			///< 错误标志，文件系统初始化时被设置
	int		iLogicSerialNo;				///< 逻辑序号
	LOCALSDK_TIME  tmStartTimeNew;		///< 新录像时间段的开始时间
	LOCALSDK_TIME	 tmEndTimeNew;			///< 新录像时间段的结束时间
	LOCALSDK_TIME	 tmStartTimeOld;		///< 老录像时间段的开始时间
	LOCALSDK_TIME	 tmEndTimeOld;			///< 老录像时间段的结束时间
}LOCALSDK_DRIVER_INFO;

/// 磁盘信息结构
typedef struct 
{
	int		iPhysicalNo;		// 硬盘物理序号，从0开始
	int		iPartNumber;		// 分区数
	LOCALSDK_DRIVER_INFO diPartitions[LOCALSDK_MAX_DRIVER_PER_DISK];
}LOCALSDK_STORAGE_INFO;

typedef struct 
{
	int iDiskNumber;
	LOCALSDK_STORAGE_INFO vStorageDeviceInfoAll[LOCALSDK_MAX_DISK_PER_MACHINE];
}LOCALSDK_STORAGE_INFO_ALL;

/// 音频编码类型
enum AudioEncodeTypes
{
	AUDIO_ENCODE_NONE = 0,
	AUDIO_ENCODE_G729_8KBIT,
	AUDIO_ENCODE_G726_16KBIT,
	AUDIO_ENCODE_G726_24KBIT,
	AUDIO_ENCODE_G726_32KBIT,
	AUDIO_ENCODE_G726_40KBIT,
	AUDIO_ENCODE_PCM_8TO16BIT,
	AUDIO_ENCODE_PCM_ALAW,
	AUDIO_ENCODE_PCM_ULAW,
	AUDIO_ENCODE_ADPCM8K16BIT,
	AUDIO_ENCODE_ADPCM16K16BIT,
	AUDIO_ENCODE_G711_ALAW,
	AUDIO_ENCODE_MPEG2_LAYER1,
	AUDIO_ENCODE_AMR8K16BIT,
	AUDIO_ENCODE_G711_ULAW,
	AUDIO_ENCODE_IMA_ADPCM_8K16BIT,
	AUDIO_ENCODE_TYPES_NR
};

/// 音频输入格式，语音对讲用
typedef struct 
{
	int iBitRate;	   ///< 码流大小，kbps为单位，比如192kbps，128kbps
	int iSampleRate;   ///< 采样率，Hz为单位，比如44100Hz
	int iSampleBit;    ///< 采样的位深
	int iEncodeType;   ///< 编码方式，参照AudioEncodeTypes定义
}LOCALSDK_AUDIOIN_FORMAT;

enum LOACLSDK_PTZ_OPT_ACT
{
	PTZ_OPT_NONE = 0,
	PTZ_OPT_LEFTUP = 1,
	PTZ_OPT_UP,
	PTZ_OPT_RIGHTUP,
	PTZ_OPT_LEFT,
	PTZ_OPT_RIGHT,

	//6
	PTZ_OPT_LEFTDOWN,
	PTZ_OPT_DOWN,
	PTZ_OPT_RIGHTDOWN,
	PTZ_OPT_ZOOM_WIDE,
	PTZ_OPT_ZOOM_TELE,

	//11
	PTZ_OPT_FOCUS_FAR,
	PTZ_OPT_FOCUS_NEAR,
	PTZ_OPT_IRIS_LARGE,
	PTZ_OPT_IRIS_SMALL,
	PTZ_OPT_ALARM_SEARCH,

	//16
	PTZ_OPT_LIGHT_ON,
	PTZ_OPT_LIGHT_OFF,
	PTZ_OPT_SETPRESET,
	PTZ_OPT_CLEARPRESET,
	PTZ_OPT_GOTOPRESET,

	//21
	PTZ_OPT_AUTOPANON,
	PTZ_OPT_AUTOPANOFF,
	PTZ_OPT_SETLEFTLIMIT,
	PTZ_OPT_SETRIGHTLIMIT,
	PTZ_OPT_AUTOSCANON,

	//26
	PTZ_OPT_AUTOSCANOFF,
	PTZ_OPT_ADDTOUR,
	PTZ_OPT_DELTOUR,
	PTZ_OPT_STARTTOUR,
	PTZ_OPT_STOPTOUR,

	//31
	PTZ_OPT_CLEARTOUR,
	PTZ_OPT_SETPATTERNBEGIN,
	PTZ_OPT_SETPATTERNEND,
	PTZ_OPT_STARTPATTERN,
	PTZ_OPT_STOPPATTERN,

	//36
	PTZ_OPT_CLEARPATTERN,
	PTZ_OPT_POSITION,
	PTZ_OPT_AUXON,
	PTZ_OPT_AUXOFF,
	PTZ_OPT_MENU,

	//41
	PTZ_OPT_EXIT,
	PTZ_OPT_ENTER,
	PTZ_OPT_ESC,
	PTZ_OPT_MENUUP,
	PTZ_OPT_MENUDOWN,

	//46
	PTZ_OPT_MENULEFT,
	PTZ_OPT_MENURIGHT,
	PTZ_OPT_SWITCH,
	//47
	PTZ_OPT_FLIP,
	PTZ_OPT_RESET,
	PTZ_MATRIX_SWITCH,
	PTZ_LIGHT_CONTROLLER,

	//51
	PTZ_OPT_SETPRESETNAME,
	PTZ_OPT_ALARMPTZ,

	PTZ_OPT_STANDARD
};

typedef struct 
{
	int nCmd;						///< refer to LOACLSDK_PTZ_OPT_ACT
	int nArg1;						///< step(1~8), preset num, tour num and so on according to LOACLSDK_PTZ_OPT_ACT
	int nArg2;						///< add and del tour means to preset num, and PTZ_OPT_LEFTUP,PTZ_OPT_RIGHTUP,PTZ_OPT_LEFTDOWN,PTZ_OPT_RIGHTDOWN means horizontal step
	int nArg3;						///< no used
	int nArg4;						///< 0：start 1：stop
}LOCALSDK_PTZ_OPT;

/// 告警状态
typedef struct
{
	int iVideoMotion; ///< motion detect state,bit0 means channel 1 and so on, 1 - alarm 0 - normal
	int iVideoBlind; ///< blind state,bit0 means channel 1 and so on, 1 - alarm 0 - normal
	int iVideoLoss;	///< loss state,bit0 means channel 1 and so on, 1 - alarm 0 - normal
	int iAlarmIn;	///< alarm-in state,bit0 means channel 1 and so on, 1 - alarm 0 - normal
	int iAlarmOut;	///< alarm-out state,bit0 means channel 1 and so on, 1 - alarm 0 - normal
}ALARM_STATE;

typedef struct
{
	bool bRecord; ///< 1: in recording 0: no record
	int iBitrate;	///< bitrate
}CHANNEL_STATE;

typedef struct
{
	CHANNEL_STATE vChnState[MAX_CHANNEL_NUM];
	ALARM_STATE vAlarmState;
}LOCALSDK_WORKSTATE;

typedef struct
{
	uchar iVideoMotion[MAX_MSK_SIZE]; ///< 移动侦测状态,用掩码表示通道号,bit0代表通道一,以此类推 1: 有告警 0: 无告警
	uchar iVideoBlind[MAX_MSK_SIZE]; ///< 视频遮挡状态,用掩码表示通道号,bit0代表通道一,以此类推 1: 有告警 0: 无告警
	uchar iVideoLoss[MAX_MSK_SIZE];	///< 视频丢失状态,用掩码表示通道号,bit0代表通道一,以此类推 1: 有告警 0: 无告警
	uchar iAlarmIn[MAX_MSK_SIZE];	///< 告警输入状态,用掩码表示通道号,bit0代表通道一,以此类推 1: 有告警 0: 无告警
	uint iAlarmOut;	///< 告警输出状态,用掩码表示通道号,bit0代表通道一,以此类推 1: 有告警 0: 无告警
}ALARM_STATE_V2;

typedef struct
{
	CHANNEL_STATE* vChnState;
	ALARM_STATE_V2 vAlarmState;
	int maxChannelNum;
	int retChannelNum;
}LOCALSDK_WORKSTATE_V2;

typedef struct 
{
	char szFileName[MAX_FILE_NAME_LEN];
	char szResv[128];
}LOCALSDK_FILE_INFO;

typedef struct 
{
	/// 有动态检测发生的区域，数据的每一位对应一个矩形区域块，置1表示在该区域块
	/// 动态检测成功，置0表示该区域块动态检测失败。每一行用一个unsigned int表示，最左边
	/// 的块在最高位，从上到下的行对应的下标从0到17。支持的行列数之外对应的数据
	/// 位无效。
	unsigned int	win[18];
}MOTION_DETECT_RESULT;

enum OSD_PLACE
{
	OSD_LEFTUP,
	OSD_UP,
	OSD_RIGHTUP,
	OSD_LEFT,
	OSD_CENTER,
	OSD_RIGHT,
	OSD_LEFTDOWN,
	OSD_DOWN,
	OSD_DOWNRIGHT,
	OSD_NUM
};

enum OSD_PARAM
{
	OSD_OFF,	//关闭手动缩放功能，使用自动缩放，作为缺省值。
	OSD_SMAL,	//二分之一
	OSD_NOMAL,	//原始大小
	OSD_BIG2,	//两倍大小
	OSD_NR,
};

typedef struct 
{
	int nPlace;    //refer to OSD_PLACE
	char szOSDTitle[MAX_FILE_NAME_LEN - 4];  //end with '\0'
	int iParam;		//enum OSD_PARAM
}LOCALSDK_OSD_INFO;

typedef struct 
{
	int nX;    //坐标千分比
	int nY;
	char szOSDTitle[MAX_FILE_NAME_LEN - 4];  //end with '\0'
	int iParam;		//enum OSD_PARAM
}LOCALSDK_OSD_INFO_V2;




typedef struct
{
 	int nTimePlace;    //refer to OSD_PLACE  //时间信置
	int nEnableTime;    
	
 	int nCamNamePlace;    //refer to OSD_PLACE  //时间信置
	int nEnableCamName;    //是否允许显示
  
}LOCALSDK_OSD_CHANNELINFO;

// alarm type
enum ALARM_MSG_TYPE
{
	ALARM_IO = 0,
	ALARM_MOTION,
	ALARM_LOSS,
	ALARM_BLIND,
	ALARM_NET_ALARM,
	ALARM_STORAGE_ERROR,
	ALARM_STORAGE_LOW_SPACE,
	ALARM_NET_ABORT,
	ALARM_IP_CONFLICT,
	ALARM_VIDEO_ANALYZE,
	ALARM_SYS_UPGRADE = 10,
	ALARM_433,
	ALARM_433_AddResult,
	ALARM_HUMAN_DETECT,
	ALARM_IPC ,
	ALARM_NUM,
	ALARM_unKnow = 255
};
enum ALARM_SUB_MSG_TYPE
{
	ALARM_SUB_NOT_SUPPORT,
	ALARM_SUB_TYPE_PEA,
	ALARM_SUB_TYPE_OSC,
	ALARM_SUB_TYPE_AVD,
	ALARM_SUB_TYPE_CPC,
	ALARM_SUB_NUM,
};
// alarm msg info
typedef struct 
{
	int nChannel;           ///< channel
	short nType;		///< msg type see refer to ALARM_MSG_TYPE
	short nSubType;	///ALARM_SUB_MSG_TYPE
	int nStatus;            ///< status 1: start 0: stop
	LOCALSDK_TIME Time;     ///< happen time
}LOCAL_ALARM_MSG;

typedef struct 
{
	int nChannel;           ///< channel
	short nType;		///< msg type see refer to ALARM_MSG_TYPE
	short nSubType;	///ALARM_SUB_MSG_TYPE
	int nStatus;            ///< status 1: start 0: stop
	LOCALSDK_TIME Time;     ///< happen time
	char pExtInfo[128];  //附加信息，发送者和接收者对各种报警类型进行格式约定
}LOCAL_ALARM_INFO_V2;

enum MediaEventTypes
{
	F_COMMON	= 0x00,		///< (R)
	F_ALERT		= 0x01,		///< (A)
	F_DYNAMIC	= 0x02,		///< (M)
	F_CARD		= 0x03,		///< (C)
	F_HAND		= 0x04		///< (H)
};

enum MediaFileTypes
{
	F_JPEG = 0,				///< JPEG FILE
	F_AUDIO = 1,			///< audio file
	F_VIDEO = 2,			///< video file
	F_AUDIO_VIDEO = 3		///< audio and video file
};

typedef struct  
{
	unsigned char	chan;		//channel
	unsigned char	av;			//see refer to MediaFileTypes
	unsigned char	image;		//frame rate
	unsigned char	alarm;		//see refer to MediaEventTypes
	LOCALSDK_TIME	stime;		//record begin time
	LOCALSDK_TIME	etime;		//record end time
	unsigned int	len;		//file size, KB unit
	unsigned int	csize;		//cluter size, KB Unit
}LOCAL_FILE_ATTR;

typedef struct 
{
	int nEnable;
	char strSSID[32];            //SSID Number
	int nChannel;                   //channel
	char strNetType[32];         //Infra, Adhoc
	char strEncrypType[32];      //NONE, WEP, TKIP, AES
	char strAuth[32];            //OPEN, SHARED, WEPAUTO, WPAPSK, WPA2PSK, WPANONE, WPA, WPA2
	int  nKeyType;                  //0:Hex 1:ASCII
	char strKeys[32];
	LOCALSDK_IPADDR HostIP;		///< host ip
	LOCALSDK_IPADDR Submask;		///< netmask
	LOCALSDK_IPADDR Gateway;		///< gateway
}LOCALSDK_WIFI_CONFIG;


// RTSP
typedef  struct 
{
	bool bServer;
	bool bClient;
	LOCALSDK_REMOTE_CFG Server;		///< 服务器模式
	LOCALSDK_REMOTE_CFG Client;		///< 客户端模式
}LOCALSDK_RTSP_CONFIG;

///< NTP设置

typedef struct 
{
	///< 是否开启
	bool Enable;
	///< PPPOE服务器
	LOCALSDK_REMOTE_CFG Server;
	///< 更新周期
	int UpdatePeriod;
	///< 时区
	int TimeZone;
}LOCALSDK_NTP_CONFIG;

enum NetDHCPEthType
{
    DHCP_ETH0,  // 有线
    DHCP_ETH1,
    DHCP_ETH2,  // Wifi
    DHCP_ETH3,
    DHCP_BOND0,
    MAX_ETH_NUM // 5
};

//DHCP
typedef struct 
{
	bool bEnable;
	char ifName[32];
}LOCALSDK_DHCP_CONFIG;

/// 所有网卡的DHCP配置
typedef struct 
{
	LOCALSDK_DHCP_CONFIG vNetDHCPConfig[MAX_ETH_NUM]; //0网卡, 2Wifi
}LOCALSDK_DHCP_CONFIGALL;

///< NTP设置
typedef struct 
{
	LOCALSDK_IPADDR PrimaryDNS;
	LOCALSDK_IPADDR SecondaryDNS;
}LOCALSDK_DNS_CONFIG;

typedef struct
{
 	int Enable;
	int nISP;
	char sServerName[32];
	char ID[32];
	char sUserName[32];
	char sPassword[32];
	LOCALSDK_IPADDR HostIP;		///< host ip
	int port;
}LOCALSDK_NET_PLATFORM_CONFIG;
enum OSD_DOT_PARAM
{
	OSD_DOT_CH_TITLE,		//通道标题，用于兼容，所以把通道标题放在前面
	OSD_DOT_OTHERS,		//其他OSD叠加
	OSD_DOT_NR,
};
typedef struct
{
	int nLen;
	unsigned char vBuf[2996];
	int Param;		//enum OSD_DOT_PARAM
}LOCALSDK_OSD_DOT_MATRIX;

typedef struct
{
	int nLen;
	int Param;
	unsigned char *vBuf;
}LOCALSDK_OSD_DOT_MATRIX_V2;

enum manual_action
{
	ACT_STOP = 0,
	ACT_START
};
enum manual_kind
{
	MANUAL_RECORD=0,
	MANUAL_SNAP
};

typedef struct
{
	int nKind;    //类型 manual_kind
	int nAction;  //动作,见manual_action
}LOCALSDK_MANUAL_ACTION;

typedef struct
{
	int nPort; //内部端口
	int nExPort; //外部端口
	int nAct;   //0:添加  1删除  2获取
}LOCALSDK_UPNP;

typedef struct 
{
	LOCALSDK_VIDEO_FORMAT stMain;  // 主码流
	LOCALSDK_VIDEO_FORMAT stExtra; // 辅码流
}LOCALSDK_MANUAL_VIDEOENCODE;


///< ARSP(主动注册服务器)设置包括：乐泰DNS
struct NetARSPConfig
{
	bool bEnable;	///< 是否开启
	char sARSPKey[NAME_PASSWORD_LEN];	///< DNS类型名称
	int iInterval;	///< 保活间隔时间
	char sURL[NAME_PASSWORD_LEN];    ///< 本机域名
	LOCALSDK_REMOTE_CFG Server;		///< DDNS服务器
	int nHttpPort;                  ///< 服务器HTTP端口
};

typedef struct  
{
	NetARSPConfig vNetARSPConfigAll[LOCAL_MAX_ARSP_TYPE];
}LOCALSDK_ARSP_CONFIG;

///< 自动维护设置
typedef struct 
{
	int iAutoRebootDay;				///< 自动重启设置日期
	int iAutoRebootHour;			///< 重启整点时间	[0, 23]	
	int iAutoDeleteFilesDays;		///< 自动删除文件时间[0, 30] 指几天前的文件 0不删除
}LOCALSDK_AUTO_MAINTAIN_CONFIG;


/* 网络类型 */
enum LOCALSDK_WirelessType
{
	LOCALSDK_WIRELESS_TYPE_AUTOSEL=0,		/* 自动选择*/
	LOCALSDK_WIRELESS_TYPE_TD_SCDMA=1,	/* TD-SCDMA网络 */
	LOCALSDK_WIRELESS_TYPE_WCDMA=2,		/* WCDMA网络 */
	LOCALSDK_WIRELESS_TYPE_CDMA_1x=3,		/* CDMA 1.x网络 */
	LOCALSDK_WIRELESS_TYPE_EDGE=4,		/* GPRS网络 */
	LOCALSDK_WIRELESS_TYPE_EVDO=5,		/*EVDO网络*/
	LOCALSDK_WIRELESS_TYPE_4G=6,		/*EVDO网络*/
	LOCALSDK_WIRELESS_TYPE_MAX
};

// 无线设备
typedef struct //tagWIRELESS_DEVICE
{
	unsigned int	typeMask; ///< 支持的网络类型掩码，按位表示，每一位对应WirelessType类型
	char	info[32]; ///< 其他信息
	int reserved[23];
}LOCALSDK_WIRELESS_DEVICE;


typedef struct //tagWIRELESS_CONFIG
{
	int iEnable;					/* 无线模块使能标志 */
	LOCALSDK_WirelessType iNetType;			/* 无线网络类型*/
	char szAPN[32];		         	/* 接入点名称，字符串, 包括'\0'结束符共32bytes */
	char szDialNum[32];  			/* 拨号号码，字符串, 包括'\0'结束符共32bytes */
	char  szUserName[128];  		/* 拨号用户名，字符串, 包括'\0'结束符共16bytes */
	char szPWD[128];  				/* 拨号密码，字符串, 包括'\0'结束符共16bytes */
	char Reserved[8]; 				/* 预留8字节，供扩展 */
}LOCALSDK_WIRELESS_CONFIG;


// 无线网络状态
typedef struct //tagWIRELESS_STATUS
{
	int iAccessStat; /* 无线网络注册状态*/
	char szDevIP[16];   /* 前端设备拨号IP，字符串, 包括'\0'结束符共16byte*/
	char szSubNetMask [16];   /* 前端设备拨号子网掩码，字符串, 包括'\0'结束符共16byte */
	char szGateWay[16];  /* 前端设备拨号网关，字符串, 包括'\0'结束符共16byte */
	LOCALSDK_WirelessType iNetType : 8; /* 无线网络类型*/
	char Reserved1[3];   /* 保留字节，用于扩展*/
	char Reserved[4];   /* 保留字节，用于扩展*/
}LOCALSDK_WIRELESS_STATE;

typedef struct 
{
	bool bEnable;			  ///< 使能标志
	bool bState;              ///< 状态
	int iHTTPPort;			  ///< HTTP端口
	int iMediaPort;			  ///< 媒体端口
	int iMobliePort;		  ///< 手机监控端口
}LOCALSDK_NETUPNP_CONFIG;

//编码器静态参数
typedef struct  
{
//	char reserved[2];
	int profile;
	int level;
	int reserved1[4];
}LOCALSDK_ENCODESTATIC_CFG;

//网络摄像头参数
//曝光配置
struct ExposureParam
{
	int  level;    //曝光等级
	uint leastTime;//自动曝光时间下限或手动曝光时间，单位微秒
	uint mostTime; //自动曝光时间上限，单位微秒
};

//增益配置
struct GainParam
{
	int gain;    //自动增益上限(自动增益启用)或固定增益值
	int autoGain;//自动增益是否启用，0:不开启  1:开启
};

//网络摄像头参数
typedef struct CameraParam
{
	uint whiteBalance;          //白平衡
	uint dayNightColor;         //日夜模式，取值有彩色、自动切换和黑白
	int  elecLevel;             //参考电平值
	uint apertureMode;          //自动光圈模式
	uint BLCMode;               //背光补偿模式
	ExposureParam exposureParam;//曝光配置
	GainParam     gainParam;    //增益配置
	uint PictureFlip;		//图片上下翻转
	uint PictureMirror;	//图片左右翻转
	uint RejectFlicker;	//日光灯防闪功能
	uint EsShutter;		//电子慢快门功能
	int ircut_mode;		//IR-CUT切换 0 = 红外灯同步切换 1 = 自动切换
	int dnc_thr;			//日夜转换阈值
	int ae_sensitivity;	//ae灵敏度配置
	int Day_nfLevel;		//noise filter 等级，0-5,0不滤波，1-5 值越大滤波效果越明显
	int Night_nfLevel;
	int Ircut_swap;		//ircut 正常序= 0        反序= 1
}LOCALSDK_CAMERAPARA_CONFIG;

//海思IPC图像风格，不同风格之间画面饱和度之类参数不同，具体由底层控制
enum IMG_TYPE
{
	IMG_TYPE_DEFAULT,
	IMG_TYPE_1,
	IMG_TYPE_2,
	IMG_TYPE_NR,
};

//网络摄像头扩展参数
typedef struct CameraParamEx
{
	GainParam broadTrends;	//宽动态
	int style;	//enum IMG_TYPE
	uint exposureTime;//实际生效的曝光时间
	int res[62];	//冗余
}LOCALSDK_CAMERAPARA_EX_CONFIG;

//恢复默认配置
// 默认配置种类
typedef enum 
{
	DEFAULT_CFG_GENERAL,			// 普通配置
	DEFAULT_CFG_ENCODE,				// 编码配置
	DEFAULT_CFG_RECORD,				// 录像配置
	DEFAULT_CFG_NET_SERVICE,		// 网络服务
	DEFAULT_CFG_NET_COMMON,			// 通用网络
	DEFAULT_CFG_ALARM,				// 报警
	DEFAULT_CFG_PTZCOMM,			// 云台，串口
	DEFAULT_CFG_USERMANAGER,		// 用户管理
	DEFAULT_CFG_PREVIEW,			// 预览配置
	DEFAULT_CFG_CAMERA_PARAM,		// 网络摄像头配置
	DEFAULT_CFG_END
}DefaultConfigKinds;

//配置应用结果，按位表示，由配置的观察者设置，配置的提交者检查
typedef enum 
{
	CONFIG_APPLY_OK = 0,						/// 成功
	CONFIG_APPLY_REBOOT = 0x00000002,			/// 需要重启系统
	CONFIG_APPLY_FILE_ERROR = 0x00000004,		/// 写文件出错
	CONFIG_APPLY_CAPS_ERROR = 0x00000008,		/// 特性不支持
	CONFIG_APPLY_GATEWAY_ERROR = 0x00000010,	///网关设置失败,仍然归类为成功
	CONFIG_APPLY_VALIT_ERROR = 0x00000020,		/// 验证失败 
	CONFIG_APPLY_NOT_EXSIST = 0x00000040,		/// 配置不存在 
	CONFIG_APPLY_NOT_SAVE = 0x00000080,			/// 不要保存到文件
	CONFIG_APPLY_DELAY_SAVE = 0x00000100,		///延时保存..把位占掉取值为0x180的原因是在配置保存的时候需要互斥
}CONFIG_APPLY_RET;

typedef struct DefaultConfig
{
	DefaultConfigKinds enkind;//需要恢复的默认配置种类
	CONFIG_APPLY_RET  enResult;//恢复默认操作结果
}LOCALSDK_DEFAULT_CONFIG;

/***普通配置***/
/// 夏令时规则
enum DST_RULE_TYPES
{
	DST_RULE_OFF = 0,	///< 关闭夏令时规则
	DST_RULE_ON,		///< 启动夏令时规则
	DST_RULE_NR
};

///< 夏令时结构
struct GeneralDSTPoint
{
	int	iYear;
	int	iMonth;
	int	iWeek;		///<周1:first  to2 3 4 -1:last one   0:表示使用按日计算的方法[-1,4]
	int	iWeekDay;	///<weekday from sunday=0	[0, 6]
	int Hour;
	int Minute;
};

///<区域设置
typedef struct LocationConfig
{
	int iVideoFormat;		///< 视频制式
	int iLanguage;			///< 语言选择
	int iDateFormat;		///< 日期格式
	int iDateSeparator;		///< 日期分割符:“.”, “-”, “/” 
	int iTimeFormat;		///< 时间格式:“12”, “24” 
	int iDSTRule;			///< 夏令时规则 
	int iWorkDay;			///< 工作日
	GeneralDSTPoint dDSTStart;
	GeneralDSTPoint dDSTEnd;
}LOCALSDK_LOCATION_CONFIG;

typedef struct BeepConfig
{
	int Frequence;      
	int Duration;       //from 0 to 3000;other value is invalid.unit is millisecond
}LOCALSDK_BEEP_CONFIG;

//手机监控设置
typedef struct NetMobileConfig
{
	bool bEnable;	///< 是否开启
	LOCALSDK_REMOTE_CFG Server;		///< 服务器
}LOCALSDK_MOBILE_CONFIG;

//获取用户权限列表
#define MAX_USER_LENGTH 32
#define MAX_RIGTH_NUM 128
#define MAX_GROUP_NUM 32
#define PASS_WORD_LENGTH 32
#define MAX_USER_NUM 32

typedef struct _USER_INFO
{
	int			rigthNum;        //用户拥有的权限个数
	char		rights[MAX_RIGTH_NUM][MAX_USER_LENGTH];  
	char		Groupname[MAX_USER_LENGTH];
	char		memo[MAX_USER_LENGTH];
	char		name[MAX_USER_LENGTH];
	char		passWord[MAX_USER_LENGTH];
	bool		reserved;		 //是否保留
	bool		shareable;		 //本用户是否允许复用 1-复用，0-不复用
}LOCALSDK_USER_INFO;

//用户可以任意加组，
typedef struct _ATHORITY_INFO
{
	int         AthorityNum;
	char        GroupName[MAX_USER_LENGTH];
	char        Athority[MAX_RIGTH_NUM][MAX_USER_LENGTH];  
}LOCALSDK_ATHORITY_INFO;

//组列表
typedef struct _GROUP_LIST
{
	char        GroupList[MAX_GROUP_NUM][MAX_USER_LENGTH];
}LOCALSDK_GROUP_LIST;

/// 修改用户密码请求
typedef struct ModifyPassword
{
	char  UserName[MAX_USER_LENGTH];
	char  Password[PASS_WORD_LENGTH];
	char  NewPassword[PASS_WORD_LENGTH];
}LOCALSDK_MODIFY_PASSWORD;

//获取用户列表
typedef struct UserList
{
	char UserList[MAX_USER_NUM][MAX_USER_LENGTH];
}LOCALSDK_USER_LIST;

//强制I帧
// 捕获通道类型
typedef enum {
	SDK_CHL_MAIN_T = 0,		///< 主通道	－	主码流1 
	SDK_CHL_2END_T = 1,		///< 辅通道	－	出辅码流2 
	SDK_CHL_3IRD_T = 2,		///< 辅通道	－	出辅码流3 
	SDK_CHL_4RTH_T = 3,		///< 辅通道	－	出辅码流4 
	SDK_CHL_JPEG_T = 4,		///< 辅通道	－	出JPEG抓图 
	SDK_CHL_FUNCTION_NUM
}CHANNELTYPE;

typedef struct 
{
	int Channel;
	CHANNELTYPE dwStreamType;
}LOCALSDK_FORCE_I_FRAME;

typedef struct
{
	int Length;
	char DataBuffer[4 * 1024 * 10];
}LOCALSDK_UPGRADE_DATA;

typedef struct DASSerInfo
{
	bool enable;
 	char serAddr[NAME_PASSWORD_LEN];
	int  port;
	char userName[NAME_PASSWORD_LEN];
	char passwd[NAME_PASSWORD_LEN];
	char devID[NAME_PASSWORD_LEN];
}LOCALSDK_DAS_SER_INFO;

typedef struct VPNState
{
	int state;
	char sIPaddr[32];
	char mask[32];
}LOCALSDK_VPN_STATE;

typedef struct GUIInfo
{
	int Isshow;         //1.增加内容，0.删除内容
	int iChannel;       //相应通道
	int index;          //对应通道
	int nX;    			//坐标千分比
	int nY;
	char szGuiContent[MAX_FILE_NAME_LEN]; //OSD添加的内容
}LOCALSDK_ADD_GUI_INFO;

//温州凯能平台
/***用户信息***/ 
typedef union {	//IP addr
	unsigned char	c[4];
	unsigned short	s[2];
	uint	l;
}LOCALSDK_IPAddress;

struct CONFIG_KAINENG_USERINFO
{
	char szUserName[16];           //用户姓名：5个汉字或十个字母;
	char szUserAddr[128];           //用户地址：30个汉字或60个字母;
	char szUserAttr[16];           //用户性质：5个汉字或十个字母；
	
	char szBelongUnit[64];         //归管单位：15个汉字或30个字母;
	char szBelongUnitPhone[16];    //归管单位电话：14位数字；分机用"-" 隔离符;
	
	char szPoliceUnit[64];         //出警单位：15个汉字或30个字母;
	char szPoliceUnitAddr[128];     //出警单位地址：30个汉字或60个字母;
	char szPoliceUnitPhone[16];    //出警单位电话：14位数字；分机用"-"隔离符;
	
	char szPolicePhone[4][16];
	char szPolOrCenPhone[2][16];   //警联/中心电话1、2：14位数字；分机用"-" 隔离符;

	char szFTP[64];                //FTP：50个字符；
	char szEMail[64];              //E-Mail: 50个字符；
	LOCALSDK_IPAddress PoliceIP;            //接警中心IP地址
	int nHttpPort;                 //接警中心IP端口
};

/***设备信息***/
//闲置布防类型枚举
typedef enum GuardType
{
	FORBID = 0,      //禁止
	TEN_MIN,         //十分钟
	THIRTY_MIN,      //三十分钟
	SIXTY_MIN,       //六十分钟
	MAX,
}GUARDTYEP;

struct CONFIG_KAINENG_DEVINFO
{
	char ProbeNum1;                //条件布防：两个下拉框，每个下拉框从 1号到10号探测器选择 ；默认两个为 <1号探测器>
	char ProbeNum2;                //条件布防：两个下拉框，每个下拉框从 1号到10号探测器选择 ；默认两个为 <1号探测器>	
	char nProbeType;			   //条件：下拉框 可选择3种为 <禁止>  <AND>  <OR>  默认为 <禁止>
	char nGuardType;               //闲置布防, 参照GUARDTYEP

	int AlarmTime;                //警笛开启时间：三位数字从000-999  默认为：180
	int nGuardArea;               //有线防区类型：八位数字，每位最小0最大3，即00000000 到33333333;  默认：00000000；
	
	char szRemConAddr[4][4];       //遥控器1-4号地址 ：八位16进制字符允许空（即0-9  A-F），默认为空；
	char szLinDevAddr[6][4];       //联运设备地1-6地址：八位16进制字符允许空（即0-9  A-F），默认为空；
	char szProbeAddr[10][4];       //探测器1-10号地址：八位16进制字符允许空（即0-9  A-F），默认为空；
	
	//定时布撤防
	char szGuardTime[3][9];        //定时布撤防,每组都为char [8] 类型 即00 00 00 00，表达由开始时分到结束时分，不需要年月日期
};



/***联动设置***/
//联动动作枚举
typedef enum KNAlarmLink
{
	LINK_EMPTY = 0,
	LINK_SWITH,
	LINK_10S,
	LINK_30S,
	LINK_60S,
	LINK_200S,
	LINK_600S,
	LINK_METHOD_1,
	LINK_METHOD_2,
	LINK_MAX
}KNALARMLINK;

struct CONFIG_KAINENG_LINKINFO
{
	char eLink[10][7];        //联动设置,取值为联动动作枚举
};

//凯能定制，3个界面信息全在这里。
typedef struct CONFIG_KAINENG_INFO
{
	CONFIG_KAINENG_USERINFO UserInfo;
	CONFIG_KAINENG_DEVINFO DevInfo;
	CONFIG_KAINENG_LINKINFO LinkInfo;
}LOCALSDK_CONFIG_KAINENG_INFO;

//凯能回调结构体
typedef struct 
{	CONFIG_KAINENG_DEVINFO DevInfo;
	CONFIG_KAINENG_LINKINFO LinkInfo;
}LOCALSDK_CBCONFIG_KAINENG_INFO;

// VPN配置结构
typedef struct
{
	int		enable;			///< 是否启动VPN，0-不启动，1-启动
	char	serverip[32];	///< VPN服务器IP地址，带0结束符的字符串，点分形式，如192.168.0.6。
	char	username[64];	///< VPN服务用户名，带0结束符的字符串。
	char	password[64];	///< VPN服务密码，带0结束符的字符串。
}LOCALSDK_NET_VPN_CONFIG;

//凯能平台需求，获取预录数据
typedef struct PRERecord
{
	int iChannel;
	int iSecond;
}LOCALSDK_PRERECORD_INFO;

//北京的一个客户需要增加一些自定义信息设置和读取的接口
typedef struct
{
	int iIndex;        //范围0到9，存储10条信息
	char szContent[64];
}LOCALSDK_CUSTOMINFO;

typedef struct
{
	int iChannel;
	
}LOCALSDK_DIGCHANNELINFO;

//用户密码是否有效
typedef struct
{
	char	Username[64];   //用户名
	char	Password[64];	//密码
	char    IsValid;        //0无效，1有效。
}LOCALSDK_USERPW_ISVALID;

//安徽创世C7平台配置
typedef struct
{
	char pServerAddr[64];//平台地址，IP地址或域名
	int  servPort;       //平台端口
	char pPUID[32];      //PUID
	char pPassword[32];  //密码
}LOCALSDK_CHUANGSHI_INFO;

// 北京鸿泰恒平台配置
typedef struct
{
	bool bEnable;
	char ServerUrl[64]; //服务器地址
	char DevID[16];     //设备id
	char Password[16];  //设备验证码
	char DevModel[16];     //设备型号
	char SerialNo[20]; //设备序列号	
}LOCALSDK_BJHONGTAIHENG_INFO;

typedef struct 
{
	char ChannelNum;
	char ChannelTypes[32];     //ChannelTypes[0] = 1;,表示第一通道是模拟通道。ChannelTypes[1] = 2，表示第二通道是数字通道
}LOCALSDK_CHANNEL_INFO;
/// 捕获通道类型
enum capture_channel_t {
	CHL_MAIN_T = 0,		///< 主通道	－	主码流1 
	CHL_2END_T = 1,		///< 辅通道	－	出辅码流2 
	CHL_3IRD_T = 2,		///< 辅通道	－	出辅码流3 
	CHL_4RTH_T = 3,		///< 辅通道	－	出辅码流4 
	CHL_JPEG_T = 4,		///< 辅通道	－	出JPEG抓图 
	CHL_FUNCTION_NUM
};


typedef struct 
{
	char ChannelNum;
	char ChannelTypes[64];     //ChannelTypes[0] = 1;,表示第一通道是模拟通道。ChannelTypes[1] = 2，表示第二通道是数字通道
}LOCALSDK_CHANNEL_INFO_V2;

/*
typedef struct 
{
	char* ChannelTypes;     //ChannelTypes[0] = 1;,表示第一通道是模拟通道。ChannelTypes[1] = 2，表示第二通道是数字通道
	int maxChannelNum;
	int retChannelNum;
}LOCALSDK_CHANNEL_INFO_V2;*/
/// 捕获开始类型
enum capture_start_t {
	CAPTURE_START_VIDEO = 1,	///< 视频编码开。
	CAPTURE_START_AUDIO = 2		///< 音频编码开。
};

/// 捕获码流控制模式类型
enum capture_brc_t {
	CAPTURE_BRC_CBR,			///< 固定码流。
	CAPTURE_BRC_VBR,			///< 可变码流。
	CAPTURE_BRC_MBR,			///< 混合码流。
	CAPTURE_BRC_NR				///< 枚举的码流控制模式数目。
};

/// 捕获分辨率类型
/// \note更新下面这个结构时，要同步更新Record.cpp里验证编码能力
enum capture_size_t {
	LOCALSDK_CAPTURE_SIZE_D1,		///< 704*576(PAL)	704*480(NTSC)
	LOCALSDK_CAPTURE_SIZE_HD1,		///< 704*288(PAL)	704*240(NTSC)
	LOCALSDK_CAPTURE_SIZE_BCIF,		///< 352*576(PAL)	352*480(NTSC)
	LOCALSDK_CAPTURE_SIZE_CIF,		///< 352*288(PAL)	352*240(NTSC)
	LOCALSDK_CAPTURE_SIZE_QCIF,		///< 176*144(PAL)	176*120(NTSC)
	LOCALSDK_CAPTURE_SIZE_VGA,		///< 640*480(PAL)	640*480(NTSC)
	LOCALSDK_CAPTURE_SIZE_QVGA,		///< 320*240(PAL)	320*240(NTSC)
	LOCALSDK_CAPTURE_SIZE_SVCD,		///< 480*480(PAL)	480*480(NTSC)
	LOCALSDK_CAPTURE_SIZE_QQVGA,		///< 160*128(PAL)	160*128(NTSC)
	LOCALSDK_CAPTURE_SIZE_NR = 9,	///< 等临时处理等底层一起修改
	LOCALSDK_CAPTURE_SIZE_ND1 = 9,   ///< 240*192
	LOCALSDK_CAPTURE_SIZE_650TVL,    ///< 960*576
	LOCALSDK_CAPTURE_SIZE_720P,      ///< 1280*720
	LOCALSDK_CAPTURE_SIZE_1_3M,      ///< 1280*960
	LOCALSDK_CAPTURE_SIZE_UXGA ,     ///< 1600*1200
	LOCALSDK_CAPTURE_SIZE_1080P,     ///< 1920*1080
	LOCALSDK_CAPTURE_SIZE_WUXGA,     ///< 1920*1200
	LOCALSDK_CAPTURE_SIZE_2_5M,      ///< 1872*1408
	LOCALSDK_CAPTURE_SIZE_3M,        ///< 2048*1536
	LOCALSDK_CAPTURE_SIZE_5M,        ///< 3744*1408
	LOCALSDK_CAPTURE_SIZE_EXT_NR = 19,    ///< 枚举的图形大小种类的数目。
	LOCALSDK_CAPTURE_SIZE_1080N = 19,     ///< 960*1080
	LOCALSDK_CAPTURE_SIZE_4M,        ///< 2560×1440
	LOCALSDK_CAPTURE_SIZE_6M,		///< 3072×2048
	LOCALSDK_CAPTURE_SIZE_8M,		///< 3264×2448
	LOCALSDK_CAPTURE_SIZE_12M,		///< 4000*3000
	LOCALSDK_CAPTURE_SIZE_4K,		///< 4096 * 2160/3840*2160
	LOCALSDK_CAPTURE_SIZE_EXT_V2_NR = 25,    ///< 枚举的图形大小种类的数目。
	LOCALSDK_CAPTURE_SIZE_720N=25,     ///< 640*720
	LOCALSDK_CAPTURE_SIZE_RES1,		///< 0*0冗余
	LOCALSDK_CAPTURE_SIZE_RES2,		///< 0*0
	LOCALSDK_CAPTURE_SIZE_RES3,		///< 0*0
	LOCALSDK_CAPTURE_SIZE_RES4,		///< 0*0
	LOCALSDK_CAPTURE_SIZE_RES5,		///< 0*0
	LOCALSDK_CAPTURE_SIZE_RES6,		///< 0*0
	LOCALSDK_CAPTURE_SIZE_EXT_V3_NR
};

typedef struct 
{
	unsigned int uiImageSize;  ///< 每个通道支持的图像分辨率
	unsigned int uiExImageSize;  ///< 每个通道支持的辅码流图像分辨率
	unsigned int uiExImageSizeEx[LOCALSDK_CAPTURE_SIZE_EXT_NR]; ///< 指定主码流分辨率下每个通道的辅码流支持的图像分辨率
	unsigned int uiMaxPower;///<每个通道支持的总编码能力
	unsigned int uiExImageSizeEx2[LOCALSDK_CAPTURE_SIZE_EXT_V2_NR - LOCALSDK_CAPTURE_SIZE_EXT_NR]; ///< 指定主码流分辨率下每个通道的辅码流支持的图像分辨率	
	unsigned int res[11 - LOCALSDK_CAPTURE_SIZE_EXT_V2_NR + LOCALSDK_CAPTURE_SIZE_EXT_NR];
}LOCALSDK_CHANNEL_ABILITY;

typedef struct 
{
	unsigned int uiImageSize;  ///< 每个通道支持的图像分辨率
	unsigned int uiExImageSize;  ///< 每个通道支持的辅码流图像分辨率
	unsigned int uiExImageSizeEx[LOCALSDK_CAPTURE_SIZE_EXT_V3_NR]; ///< 指定主码流分辨率下每个通道的辅码流支持的图像分辨率
	unsigned int uiMaxPower;///<每个通道支持的总编码能力
	unsigned int res[8];
}LOCALSDK_CHANNEL_ABILITY_V2;

typedef struct LOCALSDK_CAPTURE_CAPS
{
	/// 支持的压缩标准的掩码，位序号对应枚举类型capture_comp_t的每一个值。
	/// 置1表示支持该枚举值对应的特性，置0表示不支持。
	unsigned int Compression;

	/// 支持的码流控制模式的掩码，位序号对应枚举类型capture_brc_t的每一个值。
	/// 置1表示支持该枚举值对应的特性，置0表示不支持。
	unsigned int BitRateControl;

	/// 支持的图像大小类型的掩码，位序号对应枚举类型capture_size_t的每一个值。
	/// 置1表示支持该枚举值对应的特性，置0表示不支持。
	/// 在V2版本接口中已经不在使用
	unsigned int ImageSize;

	/// 支持的媒体类型的掩码，与枚举类型capture_start_t的每一个值相与，得到对应
	/// 的特性。置1表示支持该枚举值对应的特性，置0表示不支持。
	unsigned int Media;

	/// 区域覆盖的支持的块数。
	int CoverBlocks;

	/// 保留。
	int Reserved;

	/// 支持最多的标题叠加的个数。
	int	TitleCount;

	/// 是否支持区域覆盖 0-不支持，1-支持实时的区域覆盖， 即设置区域时
	/// 速度较快，可以实时调用， 2-支持非实时的区域覆盖，即设置区域时速度较慢，
	/// 只能在用户确定区域后调用。  
	int	RegionCover;
} LOCALSDK_CAPTURE_CAPS;
enum EncStaticParam
{
	ENC_STATICPARAM_BASELINE = 0,	//支持baseline
	ENC_STATICPARAM_MAINPRO,	//支持main profile
	ENC_STATICPARAM_HIGHPRO,	//支持high profile
	ENC_STATICPARAM_NR
};
typedef struct
{
	LOCALSDK_CHANNEL_ABILITY DspAbility[32];
	LOCALSDK_CAPTURE_CAPS CapsAbility;	///<辅助能力
	unsigned int uiMaxEncodePower;      ///< 支持的总编码能力
	unsigned int uiMaxBps;              ///< 支持的总码率大小Kbps	
	unsigned int uEncStaticParam;	///静态编码能力掩码
	unsigned int uiResverd[13];
}LOCALSDK_CAPTURE_DSPINFO;

typedef struct
{
	LOCALSDK_CHANNEL_ABILITY_V2 DspAbility[32];
	LOCALSDK_CAPTURE_CAPS CapsAbility;	///<辅助能力
	unsigned int uiMaxEncodePower;      ///< 支持的总编码能力
	unsigned int uiMaxBps;              ///< 支持的总码率大小Kbps
	unsigned int uEncStaticParam;	///静态编码能力掩码
	unsigned int uiResverd[13];
}LOCALSDK_CAPTURE_DSPINFO_V2;

typedef struct
{
	LOCALSDK_CHANNEL_ABILITY* DspAbility;
	int maxsize;
	int retsize;
	unsigned int uiResverd[24];
}LOCALSDK_CAPTURE_DSPINFO_V3;

typedef struct
{
	char	ModuleName[64];		//模块名
	char	pInfo[256];			//信息
}LOCALSDK_INFO_FRAME;


#define  MAX_EMAIL_TITLE_LEN 64
#define  MAX_EMAIL_RECIEVERS  5
typedef struct
{
	///< 是否开启
	bool Enable;
	///< smtp 服务器地址使用字符串形式填充
	///< 可以填ip,也可以填域名
	LOCALSDK_REMOTE_CFG Server;
	bool bUseSSL;
	///< 发送地址
	char SendAddr[EMAIL_ADDR_LEN];
	///< 接收人地址
	char Recievers[MAX_EMAIL_RECIEVERS][EMAIL_ADDR_LEN];
	///< 邮件主题
	char Title[MAX_EMAIL_TITLE_LEN];
	///< email有效时间
	TIME_SECTION Schedule[N_MIN_TSECT];
}LOCALSDK_EMAIL_CFG;

enum DecChnState
{
	DecStaNone, 			//无用
	DecStaNoConfig,		//未配置
	DecStaNoLogin,		//未登陆
	DecStaNoConnect,		//未连接
	DecStaConnected,		//已连接
	decStaLoginFailed,	//登陆失败
};
	
typedef struct
{
	int chnstate; //enum DecChnState
	int res[8];
}LOCALSDK_DIGIT_CHN_STATE;

typedef struct LOCALSDK_ASB_NET_VSP_CFG
{
	bool		bCsEnable;				//使能标记
	char		szCsIP[64];				//服务器地址	(IP)
	short	sCsPort;					//服务器端口
	short	sUdpPort;				//本地服务器udp端口
	char		szServerNo[64];			//服务器编号
	char 		szServerDn[64];			//服务器域名(域名为28181编号)
	char		szDeviceNO[64];			//设备编号
	char 		szConnPass[64];			//设备注册密码
	int		iHsIntervalTime;			//心跳间隔时间,单位是秒
	int		iRsAgedTime;			//注册有效期，单位为秒
	char		Camreaid[MAX_CHANNEL_NUM][64];	//通道编号
	int		CamreaLevel[MAX_CHANNEL_NUM];	//报警级别
	char		Alarmid[MAX_CHANNEL_NUM][64];	//报警编号
	int		AlarmLevel[MAX_CHANNEL_NUM];	//报警级别
	uint		uiAlarmStateGpinEnable;	//外部报警使能
	uint		uiAlarmStateLoseEnable;	//视频丢失告警使能
	uint		uiAlarmStateMotionEnable;//视频动态侦测告警使能
	uint		uiAlarmStateBlindEnable;	//屏幕遮挡告警使能
	uint		uiAlarmStatePerformanceEnable;	//系统异常告警使能
	uint		uiAlarmStateConnectEnable;	//用户连接告警使能
}ASB_NET_VSP_CONFIG;

enum RSSI_SINGNAL
{
	RSSI_NO_SIGNAL,   //<= -90db
	RSSI_VERY_LOW,     //<= -81db
	RSSI_LOW,          //<= -71db
	RSSI_GOOD,         //<= -67db
	RSSI_VERY_GOOD,    //<= -57db
	RSSI_EXCELLENT     //>-57db
};

typedef struct 
{
	char SSID[36];
	int nRSSI;                          //SEE RSSI_SINGNAL
	int nChannel;
	char  NetType[32];	 //Infra, Adhoc
	char  Encode[32];   // NONE, WEP, TKIP, AES
	char  Auth[32];      // OPEN, SHARED, WEPAUTO, WPAPSK, WPA2PSK, WPANONE, WPA, WPA2
}LOCALSDK_WLAN_DEVICE;

typedef struct 
{
	LOCALSDK_WLAN_DEVICE *pWlanDev;
	int MaxDevNum;
	int RetDevNum;
}LOCALSDK_WLAN_DEVICE_LIST;

enum DiskFullPolicy
{	
	DISK_FULL_POLICY_STOP_RECORD,	///< 停止录像
	DISK_FULL_POLICY_OVERWRITE,		///< 覆盖
	DISK_FULL_POLICY_NR
};

///< 普通配置
typedef struct
{	
	int iLocalNo;				///< 本机编号:[0, 998] 	
	int iOverWrite;				///< 硬盘满时的处理策略DiskFullPolicy
	int iAutoLogout;			///< 本地菜单自动注销(分钟)	[0, 120]
}LOCALSDK_GENERAL_CONFIG;

enum TimeSynTypes
{
	TST_OFF = 0,
	TST_TIME_UTC,
	TST_TIME_LOCAL,
	TST_TIME_ALL,
	TST_NR
};

typedef struct
{
	int *pDigitTimeSync;
	int NumDigit;
}LOCALSDK_TIMESYNC_CONFIG;

typedef struct
{
	int left;
	int top;
	int right;
	int bottom;
}TRect;

typedef uint 			RGBA32;		//0x00bbggrr

/// 视频物件结构
struct  VIDEO_WIDGET
{
	RGBA32 rgbaFrontground;		///< 物件的前景MakeRGB，和透明度	
	RGBA32 rgbaBackground;		///< 物件的后景MakeRGB，和透明度	
	TRect	rcRelativePos;		///< 物件边距与整长的比例*8191
	bool	bShowInPreview;		///< 预览叠加	
	bool	bShowInEncode;		///< 编码叠加
};

//屏幕提示信息
typedef struct
{
	VIDEO_WIDGET OSDInfoWidget;
	char ppInfo[12][64];
}LOCALSDK_OSD_DOT_CFG;

enum WindowStyle
{
	split1,
	split2,	
	split4,	
	split8,
	split9,	
	split16,
	splitPIP,
	split6,
	split12,
	split25,
	split36,
	split5,
	split10,
	split20,
	splitNR,
};

typedef struct
{
	int spt;		//分割画面enum WindowStyle
	int sub;		//分割画面子模式
}LOCALSDK_SPLIT_MODEL;


///< ftp设置
typedef struct NetFtpServerConfig{
	bool bEnable;        ///< 服务器使能     
	LOCALSDK_REMOTE_CFG Server;	///< FTP服务器
	char cRemoteDir[MAX_DIRPATH_LENGTH];	///< 远程目录
	int iMaxFileLen;	///< 文件最大长度
}LOCALSDK_FTP_CFG;

/// 录像存储设备类型，以下类型的一种或者多种
typedef struct
{
	bool SATA_as;
	bool USB_as;
	bool SD_as;
	bool DVD_as;
}LOCALSDK_STORAGE_TYPE;

///<硬盘出错事件结构
typedef struct
{
	bool 	bEnable;
	EVENT_HANDLER hEvent;	///< 处理参数
	bool	bRebootEnable;		//系统重启使能
}LOCALSDK_STORAGE_FAIL_CFG;

///< 硬盘容量不足事件结构
typedef struct StorageLowSpaceConfig
{
	bool bEnable;
	int iLowerLimit;		///< 硬盘剩余容量下限
	EVENT_HANDLER hEvent;	///< 处理参数
	bool bRecordTimeEnable;
	int iRecordTime;		///<录像天数
	int Limittype;			///<剩余容量单位
}LOCALSDK_STORAGE_LOW_SPACE_CFG;

/// 视频物件设置
typedef struct 
{
	VIDEO_WIDGET	dstCovers[8];
	VIDEO_WIDGET	ChannelTitle;
	VIDEO_WIDGET	TimeTitle;
	struct 
	{
		char strName[64];
		unsigned long long iSerialNo;	
	} ChannelName;					/// 通道名称
	int				iCoverNum;		/// 当前该通道有几个叠加的区域
}LOCALSDK_VIDEOWIDGET_CFG;

typedef struct
{
	int index;
	bool bFlagSave;
	LOCALSDK_OSD_DOT_CFG OsdCfg;
}LOCALSDK_OSD_CFG_V2;
typedef struct
{
	int MsgLen;
	char pNum[32][32];//最多32个号码，每个号码最多32位
	char* pMsg;
}LOCALSDK_SMS;

typedef struct
{
	int EnStop;		//是否停止主码流
	int YH;			//y数据的高,uv数据高为y数据的1/2
	int yuvW;		//yuv数据的宽度
	char pFileName[256];//存放路径
}LOCALSDK_YUV;

enum WIRELESS_TYPE_AUTOSEL
{
	WIRELESS_TYPE_wcdma,			//联通
	WIRELESS_TYPE_td_scdma,		//移动
	WIRELESS_TYPE_evdo, 			//电信	
	WIRELESS_TYPE_cdma_1x,  
	WIRELESS_TYPE_edge, 
	WIRELESS_TYPE_net4g, ///4g信号强度
	WIRELESS_TYPE_NR
};
typedef struct  
{
	int type;			//运营商WIRELESS_TYPE_AUTOSEL
	int value;		//强度
	int reserved[4]; /*保留字节，用于扩展*/
}LOCALSDK_WIRELESS_RSSI;


typedef struct 
{
	char ProductType[128];
}LOCALSDK_OEM_VERSION;

// ------------------------------ 智能分析相关配置 ----------------------------------------
// from IMP header: 绊线数据结构
/** URP单绊线最多条数 */
#define IMP_URP_MAX_TRIPWIRE_CNT 8

/** 最大边界点数 */
#define IMP_MAX_BOUNDARY_POINT_NUM 8

typedef short IMP_S16;
typedef int IMP_S32;

/** 坐标点 */
typedef struct
{
	IMP_S16 s16X;					/**< x */
	IMP_S16 s16Y;					/**< y */
}URP_IMP_POINT_S;

/** 线结构 */
typedef struct
{
	URP_IMP_POINT_S stStartPt;			/**< 线段起点 */
	URP_IMP_POINT_S stEndPt;			/**< 线段终点 */
}URP_LINE_S;

/** URP单绊线结构 */
typedef struct
{
	IMP_S32     s32Valid;						/**< 是否有效 */
	IMP_S32     s32IsDoubleDirection;				/**< 表示该绊线是否为双向绊线(0: 否, 1: 是) */
	IMP_S32     s32ForbiddenDirection;				/**< 单绊线禁止方向角度(单位: 度) */
	URP_LINE_S	stLine;						/**< 单绊线位置 */
}URP_TRIPWIRE_S;

/** URP单绊线限制参数 */
typedef struct
{
	IMP_S32 s32MinDist;						/**< 最小像素距离 */
	IMP_S32 s32MinTime;						/**< 最短时间 */
}URP_TRIPWIRE_LIMIT_PARA_S;

/** URP单绊线规则参数 */
typedef struct
{
	IMP_S32 s32TypeLimit;						/**< 目标类型限制 */
	IMP_S32 s32TypeHuman;						/**< 目标类别：人(限定类型时使用) */
	IMP_S32 s32TypeVehicle;						/**< 目标类别：车 */
	URP_TRIPWIRE_S astLines[IMP_URP_MAX_TRIPWIRE_CNT];		/**< 单绊线 */
	URP_TRIPWIRE_LIMIT_PARA_S stLimitPara;				/**< 单绊线限制参数 */
} URP_TRIPWIRE_RULE_PARA_S;

/** URP周界检测边界点 */
typedef struct
{
	IMP_S32       s32BoundaryPtNum;					/**< 边界点数目 */
	URP_IMP_POINT_S   astBoundaryPts[IMP_MAX_BOUNDARY_POINT_NUM];	/**< 边界点集合 */
}URP_PERIMETER_LIMIT_BOUNDARY_S;

/** URP周界检测限制参数 */
typedef struct
{
	IMP_S32 s32MinDist;						/**< 最小像素距离 */
	IMP_S32 s32MinTime;						/**< 最短时间 */
	IMP_S32	 s32DirectionLimit;					/**< 是否做方向限制 */
	IMP_S32 s32ForbiddenDirection;					/**< 禁止方向角度(单位: 角度) */
	URP_PERIMETER_LIMIT_BOUNDARY_S stBoundary;			/**< 周界边界区域 */
}URP_PERIMETER_LIMIT_PARA_S;

/** URP周界检测规则参数 */
typedef struct
{
	IMP_S32 s32TypeLimit;						/**< 目标类型限制 */
	IMP_S32 s32TypeHuman;						/**< 目标类别：人(限定类型时使用) */
	IMP_S32 s32TypeVehicle;						/**< 目标类别：车 */
	IMP_S32 s32Mode;						/**< 周界模式 */
	URP_PERIMETER_LIMIT_PARA_S stLimitPara;				/**< 周界限制参数 */
} URP_PERIMETER_RULE_PARA_S;

typedef struct
{
	int iShowTrack;						/// 是否显示轨迹
	int iShowRule;						/// 是否显示规则
	int iLevel;						   /// 警戒级别2：低级；1：中级；0: 高级
	int iPerimeterEnable;					/// 周界规则使能
	//PERIMETER_RULE_S  stPerimeter;			/// 周界规则
	URP_PERIMETER_RULE_PARA_S	stPerimeterRulePara;	/**< 周界检测 */
	int iTripWireEnable;					/// 单绊线规则使能
	//TRIPWIRE_RULE_PARA_S  stTripWire;			/// 单绊线规则
	URP_TRIPWIRE_RULE_PARA_S	stTripwireRulePara;	/**< 单绊线检测 */
}PEA_RULE_S;

//一个通道的智能分析配置
typedef struct
{
	bool         bEnable;      //是否启用视频分析功能
	EVENT_HANDLER hEvent;  //联动事件
	PEA_RULE_S stRulePea;   //周界检测与绊线配置
}LOCALSDK_PEA_RULE_Config;

//一个通道的智能分析配置
typedef struct
{
	bool         bEnable;      //是否启用视频分析功能
	EventHandler_V2 hEvent;  //联动事件
	PEA_RULE_S stRulePea;   //周界检测与绊线配置
}LOCALSDK_PEA_RULE_Config_V2;

//---------------------------------------------------------------------------------------------------

//------------- 鱼眼模式配置 --------------------
typedef enum{
	FEC_APP_CEIL = 0, 
	FEC_APP_TABL = 1,
	FEC_APP_WALL = 2
}FEC_APP_TYPE_E;

typedef enum{
	FEC_SECENE_ORIG	= 0,
	FEC_SECENE_R = 1,
	FEC_SECENE_P180_ALL = 2,
	FEC_SECENE_P180_ONE = 3,
	FEC_SECENE_P180_TWO = 4,
	FEC_SECENE_P360_FULL = 5,
	FEC_SECENE_P360_SEPE = 6,
	FEC_SECENE_P360_HALF = 7,
	FEC_SECENE_ROP_R = 8,
	FEC_SECENE_RORR_R = 9,
	FEC_SECENE_RRRR_R = 10,
	FEC_SECENE_RRP_R = 11
}FEC_SECENE_E;

typedef struct{
	unsigned int appType;//FEC_APP_TYPE_E
	unsigned int secene;//FEC_SECENE_E
	unsigned int reserv[8];
}LOCALSDK_FishEye_Config;
//----------------------------------------------

typedef struct 
{
	int nChannel;			//通道
	int pDevType;		//报警设备类型
	int nDefenceType;	//防区类型
	char pDevID[16];		//报警设备ID，包括遥控器ID
	char pDevName[32];	//报警设备昵称
	bool bEnable;			// 动态检测开启  
	char res[8];
	EventHandler_V2 hEvent;	// 动态检测联动 
}LOCALSDK_Alarm433Dev;
//433设备列表,设备最大数32
typedef struct 
{
	int num;
	LOCALSDK_Alarm433Dev* pDevList; 
}LOCALSDK_433DevList;
enum ConsumerProCmd 
{
	CONSUMER_PRO_ADD_DEV = 0,		///< 添加报警设备
	CONSUMER_PRO_STOP_ADD_DEV = 1,	///< 停止添加报警设备
	CONSUMER_PRO_DEL_DEV = 2,		///< 删除已经添加的报警设备
//	CONSUMER_PRO_CHANGE_DEV_NAME = 3,	///< 修改已经添加设备的名称
//	CONSUMER_PRO_INQUIRY_STATUS = 4,	///< 获取传感器的电量和状态
	//CONSUMER_PRO_INQUIRY_INFO = 5,		///< 查询设备信息
//	CONSUMER_PRO_GET_DEV_LIST = 5,		///< 获取设备列表
//	CONSUMER_PRO_CHANGE_DEV_STATUS = 6,		///< 修改传感器的状态 open or close
//	CONSUMER_PRO_GET_DEVLIST_STATUS = 7,	///< 获取已经在线的设备列表
//	CONSUMER_PRO_CHANGE_MODE = 8,	///< 切换场景模式
//	CONSUMER_PRO_GET_MODE_CFG = 9,		///< 获取对应模式的状态
//	CONSUMER_PRO_GET_MODE_LIST = 10,	///< 获取模式列表
//	CONSUMER_PRO_CHANGE_MODE_NAME = 11,	///< 修改模式名字
	CONSUMER_PRO_GET_LINK_STATE = 12,	///< 判断设备是否在线
	CONSUMER_PRO_NR				///< 命令个数
};
struct LOCALSDK_ProOpr
{
	int  cmd;		//如枚举ConsumerProCmd
	char pArg1[20]; //CONSUMER_PRO_ADD_DEV ：要添加的报警设备类型
					//CONSUMER_PRO_DEL_DEV：设备的ID号
					//CONSUMER_PRO_MODE_DEV_NAME：设备的ID号
	char pArg2[32]; //CONSUMER_PRO_CHANGE_DEV_NAME：新的名称
	int Channel;
	char res[8];
};
struct LOCALSDK_Custom_cfg
{
	char pCfgName[32];//配置名称
	char* pInfo;
	int length;
};


typedef struct
{
	unsigned long  HostIP;    ///< 主机IP
	unsigned long  Submask;   ///< 子网掩码
	unsigned long  Gateway;   ///< 网关IP（保留，当前不可用）
} LOCALSDK_VIRTUAL_NET_COMM;

typedef struct LOCALSDK_ASB_NET_VSP_CFG_V2
{
	bool		bCsEnable;				//使能标记
	char		szCsIP[64];				//服务器地址	(IP)
	short	sCsPort;					//服务器端口
	short	sUdpPort;				//本地服务器udp端口
	char		szServerNo[64];			//服务器编号
	char 		szServerDn[64];			//服务器域名(域名为28181编号)
	char		szDeviceNO[64];			//设备编号
	char 		szConnPass[64];			//设备注册密码
	int		iHsIntervalTime;			//心跳间隔时间,单位是秒
	int		iRsAgedTime;			//注册有效期，单位为秒
	char		Camreaid[64][64];	//通道编号
	int		CamreaLevel[64];	//报警级别
	char		Alarmid[64][64];	//报警编号
	int		AlarmLevel[64];	//报警级别
	uint		uiAlarmStateGpinEnable;	//外部报警使能
	uint		uiAlarmStateLoseEnable;	//视频丢失告警使能
	uint		uiAlarmStateMotionEnable;//视频动态侦测告警使能
	uint		uiAlarmStateBlindEnable;	//屏幕遮挡告警使能
	uint		uiAlarmStatePerformanceEnable;	//系统异常告警使能
	uint		uiAlarmStateConnectEnable;	//用户连接告警使能
}ASB_NET_VSP_CONFIG_V2;


//$###########人形检测###########
#define LOCAL_XM_IA_PD_RULE_NUM_MAX           (4)         /* 规则人形算法的最大规则数 */

typedef struct
{
	short s16StartX;					/**< x */
	short s16StartY;					/**< y */
	short s16StopX;					/**< x */
	short s16StopY;					/**< y */
}LOCALSDK_XM_IA_LINE_S;

/* 线规则结构体 */
typedef struct
{
	LOCALSDK_XM_IA_LINE_S stLine;                /* 绊线位置X1,Y1为起始点,X2,Y2为终止点(百分比表示0~8192对应0～100%) */
	int eAlarmDirect;     /* 顺起始点终止点法线方向为正向,逆起始点终止点法线方向为反向 */

	unsigned int uiReserved[16];         /* 保留规则 */
}LOCALSDK_XM_IA_RULE_LINE_S;

typedef struct
{
	short s16X;					/**< x */
	short s16Y;					/**< y */
}LOCALSDK_XM_IA_POINT_S;

/* 区域规则结构体 */
typedef struct
{
	int iBoundaryPtsNum;                /* 禁区描点数 */
	LOCALSDK_XM_IA_POINT_S astBoundaryPts[8];    /* 禁区描点位置(百分比表示0~8192对应0～100%) */
	int eAlarmDirect;     /* 进入禁区为正向,离开禁区为反向 */

	unsigned int uiReserved[16];         /* 保留规则 */
}LOCALSDK_XM_IA_RULE_REGION_S;

/* 规则联合体 */
typedef struct
{
	LOCALSDK_XM_IA_RULE_LINE_S stRuleLine;       /* 绊线规则 */
	LOCALSDK_XM_IA_RULE_REGION_S stRuleRegion;   /* 禁区规则 */
}LOCALSDK_XM_IA_PD_RULE_U;

/* 所有规则结构体 */
typedef struct
{
	bool ucRuleEnable;         /* 规则是否使能:0不使能/1使能 */

	int eRuleType;             /* 规则类型 */
	LOCALSDK_XM_IA_PD_RULE_U unPdRule;           /* 具体规则 */

	unsigned int uiReserved[16];        /* 保留规则:使能 */
}LOCALSDK_XM_IA_PD_RULES_S;

/* 规则人形检测算法配置项结构体 */
typedef struct
{
	LOCALSDK_XM_IA_PD_RULES_S stPdRules[LOCAL_XM_IA_PD_RULE_NUM_MAX];         /* 人形检测规则 */
	unsigned int uiReserved[3];         /* 保留多种规则数 */
}LOCALSDK_XM_IA_PD_CONFIG_S;

//人形检测(报警联动项使用移动侦测的)
typedef struct 
{
	bool bEnable;
	int iShowTrack;//是否显示踪迹
	int iShowRule;//是否显示规则
	int eSensitivity;         /* 人形检测灵敏度 */
	int pedchoice;             /* 检测对象，0:人  0xFF:所有物体 */
	LOCALSDK_XM_IA_PD_CONFIG_S stPdCfg;
	unsigned int Reserved[4];
}LOCALSDK_HumanDectionCfg;
//$###########人形检测###########


//@@###########################@@B 平台

#define B_CODE_LEN 256
#define B_ID_LEN 64
#define B_IP_LEN 24

typedef struct{
    char Code[B_CODE_LEN];		//设备地址编码
    char Name[B_CODE_LEN];		//设备名称
    double Longitude;		//GPS经度
    double latitude;			//GPS纬度
    char Supplier[B_CODE_LEN];		//供应商名称
    char APN[B_CODE_LEN];			//APN名称
    char SIMCARDNo[32];	//流量卡号
    uint RSSI;				//信号强度
    char IPADDR[B_IP_LEN];		//IP地址
    uint MonConsum;		//本月已用
    char SIMStatus[B_CODE_LEN];	//流量卡状态
}LOCALSDK_firm_sim_info_t;

typedef struct{
    char Code[B_CODE_LEN];			//电源设备地址编码
    char Name[B_CODE_LEN];			//电源设备名称
    char SolarPanelStatus[B_CODE_LEN];	//太阳能板状态
    double PVArrayVoltage;		//光伏阵列电压
    double PVArrayCurrent;		//光伏阵列电源
    double BatteryVoltage;		//蓄电池电压
    double BatteryCurrent;		//蓄电池电流
    double DeviceVoltage;		//负载电压
    double DeviceCurrent;		//负载电流
    double BatteryLevel;		//电池剩余电量
    double BatteryTemp;		//电池温度
} LOCALSDK_firm_battery_info_t;

typedef struct{
	char ServerID[B_ID_LEN];		////平台编码
	char ServerIP[B_IP_LEN];		//平台IP
	uint  ServerPort;			//平台端口号
	char DeviceID[B_ID_LEN];		//设备编码
	char DeviceIP[B_IP_LEN];			//设备IP
	char ChannelID[MAX_ENCODER_NUM][B_ID_LEN];	//前端设备支持的通道信息
	char DevicePassword[B_ID_LEN];	//设备端密码
	uint ActChannelNum;		//实际通道个数
	int EnableB; 			//是否使能B接口 1:使能 0:关闭
}LOCALSDK_firm_enable_b_interface_t;

//@@###########################@@B 平台



#endif
