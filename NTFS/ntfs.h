#pragma once

#pragma pack (1)
//引导扇区
typedef struct tag_ntfs_bpb{//	在cmd 输入 fsutil fsinfo ntfsinfo d: 查询 NTFS 信息
	BYTE jmpCmd[3];
	BYTE s_ntfs[8];			//	"NTFS    " 标志
	// 0x0B
	WORD bytesPerSec;		//	0x0200　　扇区大小，512B
	BYTE SecsPerClu;		//	0x08　　  每簇扇区数，4KB
	WORD rsvSecs;			//	保留扇区
	BYTE noUse01[5];
	// 0x15
	BYTE driveDscrp;		//	0xF8     磁盘介质 -- 硬盘
	BYTE noUse02[2];
	// 0x18
	WORD SecsPerTrack;		//	0x003F 　每道扇区数 63
	WORD Headers;			//	0x00FF 磁头数
	DWORD secsHide;			//	0x3F　　隐藏扇区
	BYTE noUse03[8];
	// 0x28
	UINT64 allSecsNum;		//	卷总扇区数, 高位在前, 低位在后
	// 0x30
	UINT64 MFT_startClu;	//	MFT 起始簇
	UINT64 MFTMirr_startClu;//	MTF 备份 MFTMirr 位置
	//0x40
	DWORD cluPerMFT;		//	每记录簇数 0xF6
	DWORD cluPerIdx;		//	每索引簇数
	//0x48
	UINT64 SerialNum;		//	卷序列号
	UINT64 checkSum;		//	校验和
}NTFS_BPB,*PNTFS_BPB;

typedef struct tag_bft_header{
	BYTE    mark[4];		//"FILE" 标志 
	WORD    UsnOffset;		//更新序列号偏移 　　　　30 00
	WORD    usnSize;		//更新序列数组大小+1 　 03 00
	DWORD64    LSN;			//日志文件序列号(每次记录修改后改变)  58 8E 0F 34 00 00 00 00
	// 0x10
	WORD	SN;				//序列号 随主文件表记录重用次数而增加
	WORD    linkNum;		//硬连接数 (多少目录指向该文件) 01 00
	WORD    firstAttr;		//第一个属性的偏移　　38 00
	WORD    flags;			//0已删除 1正常文件 2已删除目录 3目录正使用
	// 0x18
	DWORD    MftUseLen;		//记录有效长度   　A8 01 00 00
	DWORD    maxLen;			//记录占用长度 　 00 04 00 00
	// 0x20
	DWORD64    baseRecordNum;	//索引基本记录, 如果是基本记录则为0
	WORD    nextAttrId;		//下一属性Id　　07 00
	WORD    border;
	DWORD    xpRecordNum;	//用于xp, 记录号
	// 0x30
	DWORD64    USN;			//更新序列号(2B) 和 更新序列数组
}MFT_HEADER, *PMFT_HEADER;

//------------------  属性头通用结构 - 常驻属性和非常驻属性的公共部分 ----
union CCommon
{
	//---- 如果该属性为 常驻 属性时使用该结构 ----
	struct CResident
	{
		DWORD StreamLength;		//属性值的长度, 即属性具体内容的长度。"48 00 00 00"
		WORD StreamOffset;		//属性值起始偏移量  "18 00"
		WORD IndexFiag;			//属性是否被索引项所索引，索引项是一个索引(如目录)的基本组成  00 00
	}Resident;
	//------- 如果该属性为 非常驻 属性时使用该结构 ----
	struct CNonResident
	{
		DWORD64 StartVCN;			//起始的 VCN 值(虚拟簇号：在一个文件中的内部簇编号,0起）
		DWORD64 LastVCN;			//最后的 VCN 值
		WORD RunListOffset;		//运行列表的偏移量
		WORD CompressEngineIndex;	//压缩引擎的索引值，指压缩时使用的具体引擎。
		DWORD Reserved;
		DWORD64 StreamAiiocSize;		//为属性值分配的空间 ，单位为B，压缩文件分配值小于实际值
		DWORD64 StreamRealSize;		//属性值实际使用的空间，单位为B
		DWORD64 StreamCompressedSize;	//属性值经过压缩后的大小, 如未压缩, 其值为实际值
	}NonResident;
};

//------------------  属性头通用结构 ----
typedef struct tag_ntfs_attribute	//所有偏移量均为相对于属性类型 Type 的偏移量
{
	DWORD Type;				//属性类型 0x10, 0x20, 0x30, 0x40,...,0xF0,0x100
	DWORD Length;			//属性的长度 
	BYTE NonResidentFiag;		//是否是非常驻属性，l 为非常驻属性，0 为常驻属性 00
	BYTE NameLength;			//属性名称长度，如果无属性名称，该值为 00
	WORD ContentOffset;		//属性内容的偏移量  18 00
	WORD CompressedFiag;	//该文件记录表示的文件数据是否被压缩过 00 00
	WORD Identify;			//识别标志  00 00
	//--- 0ffset: 0x10 ---
	//--------  常驻属性和非常驻属性的公共部分 ----
	CCommon com ;
}NTFS_ATTRIBUTES,*PNTFS_ATTRIBUTES;


struct Value0x30
{
	DWORD64 parentFileNum;		//父目录的文件参考号,前6B
	DWORD64 createTime;		//文件创建时间
	DWORD64 changeTime;		//文件修改时间
	DWORD64 MFTchangeTime;		//MFT 修改时间 
	DWORD64 lastVisitTime;		//最后一次访问时间 
	DWORD64 AllocSize;			//文件分配大小 
	DWORD64 realSize;			//实际大小 
	DWORD fileFlag;			//文件标志，系统 隐藏 压缩等等 
	DWORD EAflags;			//EA扩展属性和重解析点
	BYTE nameLength;			//文件名长 
	BYTE nameSpace;			//文件命名空间 
	//----- Name (Unicode编码) 长度为 2 * nameLength ---- 
};
#pragma pack ()
