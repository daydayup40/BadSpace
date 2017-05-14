#pragma once

#pragma pack (1)
//��������
typedef struct tag_ntfs_bpb{//	��cmd ���� fsutil fsinfo ntfsinfo d: ��ѯ NTFS ��Ϣ
	BYTE jmpCmd[3];
	BYTE s_ntfs[8];			//	"NTFS    " ��־
	// 0x0B
	WORD bytesPerSec;		//	0x0200����������С��512B
	BYTE SecsPerClu;		//	0x08����  ÿ����������4KB
	WORD rsvSecs;			//	��������
	BYTE noUse01[5];
	// 0x15
	BYTE driveDscrp;		//	0xF8     ���̽��� -- Ӳ��
	BYTE noUse02[2];
	// 0x18
	WORD SecsPerTrack;		//	0x003F ��ÿ�������� 63
	WORD Headers;			//	0x00FF ��ͷ��
	DWORD secsHide;			//	0x3F������������
	BYTE noUse03[8];
	// 0x28
	UINT64 allSecsNum;		//	����������, ��λ��ǰ, ��λ�ں�
	// 0x30
	UINT64 MFT_startClu;	//	MFT ��ʼ��
	UINT64 MFTMirr_startClu;//	MTF ���� MFTMirr λ��
	//0x40
	DWORD cluPerMFT;		//	ÿ��¼���� 0xF6
	DWORD cluPerIdx;		//	ÿ��������
	//0x48
	UINT64 SerialNum;		//	�����к�
	UINT64 checkSum;		//	У���
}NTFS_BPB,*PNTFS_BPB;

typedef struct tag_bft_header{
	BYTE    mark[4];		//"FILE" ��־ 
	WORD    UsnOffset;		//�������к�ƫ�� ��������30 00
	WORD    usnSize;		//�������������С+1 �� 03 00
	DWORD64    LSN;			//��־�ļ����к�(ÿ�μ�¼�޸ĺ�ı�)  58 8E 0F 34 00 00 00 00
	// 0x10
	WORD	SN;				//���к� �����ļ����¼���ô���������
	WORD    linkNum;		//Ӳ������ (����Ŀ¼ָ����ļ�) 01 00
	WORD    firstAttr;		//��һ�����Ե�ƫ�ơ���38 00
	WORD    flags;			//0��ɾ�� 1�����ļ� 2��ɾ��Ŀ¼ 3Ŀ¼��ʹ��
	// 0x18
	DWORD    MftUseLen;		//��¼��Ч����   ��A8 01 00 00
	DWORD    maxLen;			//��¼ռ�ó��� �� 00 04 00 00
	// 0x20
	DWORD64    baseRecordNum;	//����������¼, ����ǻ�����¼��Ϊ0
	WORD    nextAttrId;		//��һ����Id����07 00
	WORD    border;
	DWORD    xpRecordNum;	//����xp, ��¼��
	// 0x30
	DWORD64    USN;			//�������к�(2B) �� ������������
}MFT_HEADER, *PMFT_HEADER;

//------------------  ����ͷͨ�ýṹ - ��פ���Ժͷǳ�פ���ԵĹ������� ----
union CCommon
{
	//---- ���������Ϊ ��פ ����ʱʹ�øýṹ ----
	struct CResident
	{
		DWORD StreamLength;		//����ֵ�ĳ���, �����Ծ������ݵĳ��ȡ�"48 00 00 00"
		WORD StreamOffset;		//����ֵ��ʼƫ����  "18 00"
		WORD IndexFiag;			//�����Ƿ�����������������������һ������(��Ŀ¼)�Ļ������  00 00
	}Resident;
	//------- ���������Ϊ �ǳ�פ ����ʱʹ�øýṹ ----
	struct CNonResident
	{
		DWORD64 StartVCN;			//��ʼ�� VCN ֵ(����غţ���һ���ļ��е��ڲ��ر��,0��
		DWORD64 LastVCN;			//���� VCN ֵ
		WORD RunListOffset;		//�����б��ƫ����
		WORD CompressEngineIndex;	//ѹ�����������ֵ��ָѹ��ʱʹ�õľ������档
		DWORD Reserved;
		DWORD64 StreamAiiocSize;		//Ϊ����ֵ����Ŀռ� ����λΪB��ѹ���ļ�����ֵС��ʵ��ֵ
		DWORD64 StreamRealSize;		//����ֵʵ��ʹ�õĿռ䣬��λΪB
		DWORD64 StreamCompressedSize;	//����ֵ����ѹ����Ĵ�С, ��δѹ��, ��ֵΪʵ��ֵ
	}NonResident;
};

//------------------  ����ͷͨ�ýṹ ----
typedef struct tag_ntfs_attribute	//����ƫ������Ϊ������������� Type ��ƫ����
{
	DWORD Type;				//�������� 0x10, 0x20, 0x30, 0x40,...,0xF0,0x100
	DWORD Length;			//���Եĳ��� 
	BYTE NonResidentFiag;		//�Ƿ��Ƿǳ�פ���ԣ�l Ϊ�ǳ�פ���ԣ�0 Ϊ��פ���� 00
	BYTE NameLength;			//�������Ƴ��ȣ�������������ƣ���ֵΪ 00
	WORD ContentOffset;		//�������ݵ�ƫ����  18 00
	WORD CompressedFiag;	//���ļ���¼��ʾ���ļ������Ƿ�ѹ���� 00 00
	WORD Identify;			//ʶ���־  00 00
	//--- 0ffset: 0x10 ---
	//--------  ��פ���Ժͷǳ�פ���ԵĹ������� ----
	CCommon com ;
}NTFS_ATTRIBUTES,*PNTFS_ATTRIBUTES;


struct Value0x30
{
	DWORD64 parentFileNum;		//��Ŀ¼���ļ��ο���,ǰ6B
	DWORD64 createTime;		//�ļ�����ʱ��
	DWORD64 changeTime;		//�ļ��޸�ʱ��
	DWORD64 MFTchangeTime;		//MFT �޸�ʱ�� 
	DWORD64 lastVisitTime;		//���һ�η���ʱ�� 
	DWORD64 AllocSize;			//�ļ������С 
	DWORD64 realSize;			//ʵ�ʴ�С 
	DWORD fileFlag;			//�ļ���־��ϵͳ ���� ѹ���ȵ� 
	DWORD EAflags;			//EA��չ���Ժ��ؽ�����
	BYTE nameLength;			//�ļ����� 
	BYTE nameSpace;			//�ļ������ռ� 
	//----- Name (Unicode����) ����Ϊ 2 * nameLength ---- 
};
#pragma pack ()