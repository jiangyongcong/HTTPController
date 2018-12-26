#pragma once

#define MAX_META_COUNT_PER_FRAME 150
#define MAX_LIVE_BROADCASE_TIME 5 //秒

namespace nsProtocol
{
	enum _enDevControlType  // 设备控制类型
	{
		enClient_Crl = 0x01,  // 客户端控制流
		enDataNode_Crl = 0x02, // 数据节点控制流
		enClient_Dt = 0x03,  // 客户端数据流
		enDataNode_Dt = 0x04, // 数据节点数据流
	};
	enum _enControl_Client
	{
		enClientLogin = 0x01, // 客户端登录
		enUploadMetaReq = 0x02, // 文件上传元数据请求
		enDownloadMetaReq = 0x03, // 文件下载元数据请求
		enMasterProBroadcast = 0x41, // 主节点属性播送
		enUploadMetaRsp = 0x42, // 文件上传元数据应答
		enDownloadMetaRsp = 0x43, // 文件下载元数据应答
	};

	enum _enControl_DataNode
	{
		enStatusBro = 0x01, // 数据节点状态播送
		enNodeMeatBro = 0x02, // 节点元数据播送
		enFileUploadMetaBro = 0x03, // 文件上传元数据播送
		enDownloadFinBro = 0x04, // 文件下载完成播送
		enRegistRsp = 0x41,// 注册应答
		enFileCopy = 0x42, // 文件复制
	};

	enum _enData_Client
	{
		enFileBaseInfo = 0x01, // 文件基本信息
		enFileDataInfo = 0x02, // 文件数据信息
		enDownloadDataReq = 0x03, // 下载数据请求
		enUploadDataRsp = 0x41,// 上传数据应答
		enWebUrlRsp = 0x42, // 下载Url应答
	};

	enum _enNodeProp
	{
		enUnknow = 0,
		enReadWrite = 1,		// 读写
		enReadOnly  = 2			// 只读
	};
	
#pragma  pack( push, 1 )

	typedef struct _tagSingleFileMetaData
	{
		unsigned short m_usNodeId;
		unsigned long m_uiFileRecordNo;
		unsigned long m_uiFileTS;
		unsigned short m_usFileLenK;
		unsigned char  m_ui8FileType;
	}CSingleFileMetaData;

	typedef struct _tagSingleMetaData
	{
		unsigned long m_uiFileRecordNo;
		unsigned long m_uiFileTS;
		unsigned short m_usFileLenK;
		unsigned char  m_ui8FileType;
	}CSingleMetaData;

	typedef struct _tagFileWriteMetaData
	{
		unsigned long m_uiDataNodeIp;
		unsigned short m_usDataNodePort;
		unsigned long m_uiFileRecordNo;
		unsigned long m_uiFileTS;
		unsigned short m_usFileLenK;
	}CFileWriteMetaData,CFileReadMetaData;

	enum _enStatusProp
	{
		enReg = 0, // 数据节点注册包
		enAlive = 1, // 数据节点生存包
		enMasterAlive = 2, // 主节点生存包
	};
	typedef struct _tagDataNodeRegist
	{
		unsigned short	m_ui16ListenPort;	// 监听端口
		unsigned char   m_ui8Prop;			// 属性，是注册还是心跳
		unsigned long  m_uiIndexCount;		// 元数据索引数
		unsigned char   m_ui8NodeProp;		// 数据节点属性
		unsigned long  m_uiDiskRate;		// 磁盘利用率
		unsigned long  m_uiMemUsed;		// 内存占用
		unsigned long  m_uiLoadValue;		// 负载值
	}CDataNodeStatusBro;

	typedef struct _tagDataNodeRegistRsp
	{
		unsigned short  m_usNodeId; // 节点ID
	}CDataNodeRegistRsp; // 数据节点注册应答

	enum _enMetaProp
	{
		enAddMeta = 0, // 添加元数据
		enDelMeta = 1, // 删除元数据
	};
	typedef struct _tagMetaDataBroadcast
	{
		unsigned char   m_ui8Prop; // 类型（增/删）
		unsigned long  m_uiCount; // 数量
		CSingleMetaData m_ArySingleMetaDatas[MAX_META_COUNT_PER_FRAME]; // 元数据数组
	}CMetaDataBroadcast;

	typedef struct _tagClientWriteMetaReq
	{
		unsigned long m_uiSN; // 流水号
		unsigned long m_uiFileLen; // 文件大小
	}CClientWriteMetaReq; // 文件上传请求

	typedef struct _tagIpAdressPort
	{
		unsigned long m_uiDataNodeIPAddrs;		// 节点IP列表
		unsigned short m_uiDataNodePorts;   // 节点端口列表
	}CIpAdressPort;

	typedef struct _tagMasterMetaRsp
	{
		unsigned long m_uiSN; // 流水号
		unsigned long m_uiFileRecordNo; // 记录号
		unsigned long m_uiFileTS; // 时间戳
		unsigned char  m_ui8DataNodeNum; // 节点数
		CIpAdressPort m_lstDataNodeAdress[8]; // 节点列表
	}CMasterMetaRsp; // 主节点元数据应答

	typedef struct _tagFileBaseInfo
	{
		unsigned long m_uiSN; // 流水号
		unsigned long m_uiFileRecordNo; // 记录号
		unsigned long m_uiTotalFileLen; // 文件总大小
		unsigned char  m_ui8FileType; // 文件类型
		unsigned long m_uiFileTS; // 文件时间戳
	}CFileBaseInfo; // 文件基本信息

	typedef struct _tagFileDataInfo
	{
		unsigned long m_uiSN; // 流水号
		unsigned short m_usFrameNo; // 帧序号
		unsigned short m_usFrameDataLen; // 本帧文件数据长度
		char   m_szFrameData[4096];
	}CFileDataInfo; // 文件数据信息
	
	typedef struct _tagFileUploadMetaBro
	{
		unsigned char  m_ui8Ret; // 文件上传结果
		unsigned short m_usFileLenK; // 以K为单位的文件大小
		unsigned long m_uiFileRecordNo; // 文件记录号
		unsigned long m_uiFileTS; // 文件时间戳
		unsigned char  m_ui8FileType; // 文件类型
	}CFileUploadMetaBro; // 文件上传结果元数据播送

	typedef struct _tagDownloadFinBro
	{
		unsigned short m_usFileLenK; // 以K为单位的文件大小
	}CDownloadFinBro; // 文件下载完成结果播送

	typedef struct _tagUploadDataRsp
	{
		unsigned long m_uiSN; // 流水号
		unsigned char  m_ui8Ret; // 上传结果
		unsigned char  m_ui8FileNameLen; //文件名
		char   m_szFileName[MAX_PATH]; 
	}CUploadDataRsp; // 文件上传结果应答

	typedef struct _tagDownloadMetaReq
	{
		unsigned long m_uiSN; // 流水号
		unsigned long m_uiFileRecordNo; // 文件记录号
		unsigned long m_uiFileTS; // 文件时间戳
		unsigned short m_usFileLenK; // 以K为单位的文件大小
	}CClientReadMetaReq; // 文件下载元数据请求

	typedef struct _tagFileDataDownloadReq
	{
		unsigned long m_uiSN; // 流水号
		unsigned long m_uiFileTS; // 时间戳
		unsigned short m_usFileLenK; // 以K为的单位的文件大小
		unsigned char  m_ui8DownloadType; // 下载类型
		unsigned char  m_ui8FileProp; // 文件属性（原图/缩略图）
		unsigned char  m_ui8FileNameLen; // 文件名
		char   m_szFileName[MAX_PATH];
	}CFileDataDownloadReq; // 文件下载数据请求

	typedef struct _tagFileCopyCommand
	{
		unsigned char  m_ui8DstDataNodeNum; // 目标数据节点数
		CIpAdressPort m_lstAryNodeIpAddr[8];// 目标数据节点地址数组
		unsigned long m_uiFileTS; // 文件时间戳
		unsigned long m_uiFileRecordNo; // 文件记录号
		unsigned short m_usFileLenK; //以K为单位的文件大小
		unsigned char  m_ui8FileType; // 文件类型
	}CFileCopyCommand; // 文件复制指令

	const int PROTOCAL_HEAD_LEN = 2; // 协议头长度
	const float READ_WRITE_RATE = 5.0; // 磁盘读写比例因子
	const int DATA_FRAMR_LEN = 1800; // 单帧文件数据长度
	const int MAX_ALIVE_TIMER = 15; // 业务生存时间

#pragma  pack( pop )
}

using namespace nsProtocol;