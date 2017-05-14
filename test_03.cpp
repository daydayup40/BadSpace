// test_03.cpp : 定义控制台应用程序的入口点。
//
/*

http://www.cnblogs.com/mwwf-blogs/archive/2015/05/04/4467687.html

*/
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

#include "ntfs.h"

#define DEF_SEC_BYTES		0x200	//512
#define DEF_CLU_BYTES		0x1000	//4096
#define DEF_NFTHEAD_BYTES	0x400	//1024

VOID	PauseShowError( LPCSTR lpszTitle ,DWORD dwError ) ;
DWORD	GetDiskData64( HANDLE hDisk , DWORD dwLen , LPBYTE lpOut , DWORD64 dw64Offset = ~0 ) ;
DWORD	GetDiskData32( HANDLE hDisk , DWORD dwLen , LPBYTE lpOut , DWORD dwOffset = ~0 ) ;
DWORD	ReadOneMFT32( HANDLE hDisk , DWORD dwOffset , LPBYTE * lplpOut , LPDWORD lpdwOut ) ;

HANDLE	g_hDisk = INVALID_HANDLE_VALUE ;
DWORD	g_dwBytesPerSec = DEF_SEC_BYTES ;
LPBYTE	g_lpbSecBuf = NULL ;

DWORD	g_dwBytesPerClu = DEF_CLU_BYTES ;
LPBYTE	g_lpbCluBuf = NULL ;

DWORD	g_dwBytePerMFTRecord = DEF_NFTHEAD_BYTES ;
LPBYTE	g_lpbMFTRecord = NULL ;

int _tmain(int argc, _TCHAR* argv[])
{
	DWORD	dwRead = 0 ;
	LPCSTR	lpszDrive = "\\\\.\\C:";
	//LPCSTR	lpszDrive = "\\\\.\\physicalDrive2" ;

	g_hDisk = CreateFileA(lpszDrive,GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);  
	if(g_hDisk == INVALID_HANDLE_VALUE)
		PauseShowError( "CreateFileA" , GetLastError() ) ;


	g_lpbSecBuf = new BYTE[g_dwBytesPerSec] ;
	dwRead = GetDiskData32( g_hDisk , g_dwBytesPerSec , g_lpbSecBuf , (DWORD)0 ) ;

	//获得BPB
	NTFS_BPB	stNtfsBpb = {0} ;
	memcpy( (LPBYTE)&stNtfsBpb , g_lpbSecBuf , sizeof(NTFS_BPB) ) ;

	//修正sec字节数
	g_dwBytesPerSec = stNtfsBpb.bytesPerSec ;
	delete [] g_lpbSecBuf ;	g_lpbSecBuf = new BYTE[g_dwBytesPerSec] ;

	MFT_HEADER	stMFTHeader[16] = {0} ;
	//获得第一条 MFT 项:  $MFT
	g_dwBytesPerClu = g_dwBytesPerSec * stNtfsBpb.SecsPerClu ;
	dwRead = GetDiskData64( g_hDisk , g_dwBytesPerSec , g_lpbSecBuf , g_dwBytesPerClu * stNtfsBpb.MFT_startClu ) ;
	memcpy( (LPBYTE)&(stMFTHeader[0]) , g_lpbSecBuf , sizeof(MFT_HEADER) ) ;

	g_dwBytePerMFTRecord = stMFTHeader[0].maxLen ;		//修正
	g_lpbMFTRecord = new BYTE[g_dwBytePerMFTRecord] ;

	DWORD	dwGetFileLen = 0 ;
	LPBYTE	lpGetFileData = NULL ;

	DWORD64	dw64Index = 0 ;
	dwRead = GetDiskData64( g_hDisk , g_dwBytePerMFTRecord , g_lpbMFTRecord , g_dwBytesPerClu * stNtfsBpb.MFT_startClu + dw64Index * DEF_SEC_BYTES ) ;	//0 MFT
	
	while ( dwRead == g_dwBytePerMFTRecord )	
	{
		if ( memcmp( g_lpbMFTRecord , (const LPBYTE)"FILE" , 4 ) == 0 )
		{
			LPBYTE				lpEnd = g_lpbMFTRecord + g_dwBytePerMFTRecord - sizeof(NTFS_ATTRIBUTES) ;
			PNTFS_ATTRIBUTES	pNtfsAttr = (PNTFS_ATTRIBUTES)(g_lpbMFTRecord + sizeof(MFT_HEADER)) ;
			BOOL				blNeedSam = FALSE ;
			BOOL				blNeedSystem = FALSE ;
			while ( (LPBYTE)pNtfsAttr <= lpEnd && pNtfsAttr->Type != 0xFFFFFFFF )
			{
				if ( pNtfsAttr->Type == 0x30 )	
				{
					Value0x30 * lpValue30 = (Value0x30 *)(((LPBYTE)pNtfsAttr) + pNtfsAttr->com.Resident.StreamOffset) ;
					LPWSTR	lpszName = (LPWSTR)((LPBYTE)lpValue30 + sizeof(Value0x30)) ;
					//if ( memcmp( (LPBYTE)lpszName , (const LPBYTE)(L"cdcdabc.txt")  , lpValue30->nameLength * 2 ) == 0 )
					if ( memcmp( (LPBYTE)lpszName , (const LPBYTE)(L"SAM")  , lpValue30->nameLength * 2 ) == 0 )
					{	
						blNeedSam = TRUE ;	
					}
					else	if ( memcmp( (LPBYTE)lpszName , (const LPBYTE)(L"SYSTEM")  , lpValue30->nameLength * 2 ) == 0 )
					{	
						blNeedSystem = TRUE ;	
					}
				}
				if ( (blNeedSam || blNeedSystem) && pNtfsAttr->Type == 0x80 )
				{
					if ( pNtfsAttr->NonResidentFiag == 1 )
					{
						LPBYTE	lpRunList = ((LPBYTE)pNtfsAttr) + pNtfsAttr->com.NonResident.RunListOffset ;
						DWORD	dwRunListLen = (lpRunList[0] & 0xf) + ((lpRunList[0]>>4) & 0xf) ;

						DWORD	dwUsedClusNum = *((LPDWORD)(lpRunList + 1)) ;
						dwUsedClusNum &= (((DWORD)(~0)) >> ((4-(lpRunList[0] & 0xf)) * 8) ) ;

						DWORD	dwStartClu = *((LPDWORD)(lpRunList + 1 + (lpRunList[0] & 0xf))) ;
						dwStartClu &= (((DWORD)(~0)) >> ((4-((lpRunList[0]>>4) & 0xf)) * 8) ) ;

						DWORD64	dw64GetOffset = (DWORD64)dwStartClu * (DWORD64)g_dwBytesPerClu ;
						dwGetFileLen = (DWORD64)dwUsedClusNum * (DWORD64)g_dwBytesPerClu ;
						lpGetFileData = new BYTE[dwGetFileLen] ;
						dwRead = GetDiskData64( g_hDisk , dwGetFileLen , lpGetFileData , dw64GetOffset ) ;

					}
					else
					{
						dwGetFileLen = pNtfsAttr->com.Resident.StreamLength ;
						lpGetFileData = new BYTE[dwGetFileLen] ;
						memcpy( lpGetFileData , ((LPBYTE)pNtfsAttr) + pNtfsAttr->com.Resident.StreamOffset , dwGetFileLen ) ;
					}

					//存文件
					CHAR	szFilePath[1024] = {0} ;
					if ( blNeedSam )	sprintf( szFilePath , "c:\\users\\public\\SAM_r%I64X" , dw64Index ) ;
					else	sprintf( szFilePath , "c:\\users\\public\\SYSTEM_r%I64X" , dw64Index ) ;

					FILE	* fp = fopen( szFilePath , "wb" ) ;
					fwrite( lpGetFileData , 1 , dwGetFileLen , fp ) ;
					fclose( fp ) ;

					delete [] lpGetFileData ;
				}

// 				if ( (((LPBYTE)pNtfsAttr) + pNtfsAttr->Length) > g_lpbMFTRecord + g_dwBytePerMFTRecord )
// 					break ;
				pNtfsAttr = (PNTFS_ATTRIBUTES)(((LPBYTE)pNtfsAttr) + pNtfsAttr->Length) ;
				
			}
			dw64Index += 1 ;
		}

		if ( memcmp( g_lpbMFTRecord , (const LPBYTE)"regf" , 4 ) == 0 )
		{
			dw64Index += 7 ;
		}

		dw64Index += 1 ;
		dwRead = GetDiskData64( g_hDisk , g_dwBytePerMFTRecord , g_lpbMFTRecord , g_dwBytesPerClu * stNtfsBpb.MFT_startClu + dw64Index * DEF_SEC_BYTES ) ;
		if ( dw64Index % 1024 == 0 )	{
			//printf( "\r%I64u / %I64u   " , dw64Index , stNtfsBpb.allSecsNum ) ;
			printf( "\r%I64X / %I64X   " , dw64Index , stNtfsBpb.allSecsNum ) ;
		}
	}

	CloseHandle( g_hDisk ) ;
	printf( "byebye\n" ) ;
	system( "pause" ) ;
	return 0;
}

/*
int _tmain(int argc, _TCHAR* argv[])
{
	DWORD	dwRead = 0 ;
	LPCSTR	lpszDrive = "\\\\.\\C:";
	//LPCSTR	lpszDrive = "\\\\.\\physicalDrive2" ;
	
	g_hDisk = CreateFileA(lpszDrive,GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);  
	if(g_hDisk == INVALID_HANDLE_VALUE)
		PauseShowError( "CreateFileA" , GetLastError() ) ;
	
	
	g_lpbSecBuf = new BYTE[g_dwBytesPerSec] ;
	dwRead = GetDiskData32( g_hDisk , g_dwBytesPerSec , g_lpbSecBuf , (DWORD)0 ) ;

	//获得BPB
	NTFS_BPB	stNtfsBpb = {0} ;
	memcpy( (LPBYTE)&stNtfsBpb , g_lpbSecBuf , sizeof(NTFS_BPB) ) ;
	
	//修正sec字节数
	g_dwBytesPerSec = stNtfsBpb.bytesPerSec ;
	delete [] g_lpbSecBuf ;	g_lpbSecBuf = new BYTE[g_dwBytesPerSec] ;

	MFT_HEADER	stMFTHeader[16] = {0} ;
	//获得第一条 MFT 项:  $MFT
	g_dwBytesPerClu = g_dwBytesPerSec * stNtfsBpb.SecsPerClu ;
	dwRead = GetDiskData64( g_hDisk , g_dwBytesPerSec , g_lpbSecBuf , g_dwBytesPerClu * stNtfsBpb.MFT_startClu ) ;
	memcpy( (LPBYTE)&(stMFTHeader[0]) , g_lpbSecBuf , sizeof(MFT_HEADER) ) ;
	g_dwBytePerMFTRecord = stMFTHeader[0].maxLen ;		//修正
	g_lpbMFTRecord = new BYTE[g_dwBytePerMFTRecord] ;
	dwRead = GetDiskData64( g_hDisk , g_dwBytePerMFTRecord , g_lpbMFTRecord , g_dwBytesPerClu * stNtfsBpb.MFT_startClu ) ;	//0 MFT
	dwRead = GetDiskData64( g_hDisk , g_dwBytePerMFTRecord , g_lpbMFTRecord ) ;	//1 MFTMirr
	dwRead = GetDiskData64( g_hDisk , g_dwBytePerMFTRecord , g_lpbMFTRecord ) ;	//2 LogFile
	dwRead = GetDiskData64( g_hDisk , g_dwBytePerMFTRecord , g_lpbMFTRecord ) ;	//3 Volume
	dwRead = GetDiskData64( g_hDisk , g_dwBytePerMFTRecord , g_lpbMFTRecord ) ;	//4 AttrDef
	dwRead = GetDiskData64( g_hDisk , g_dwBytePerMFTRecord , g_lpbMFTRecord ) ;	//5 根目录


	PNTFS_ATTRIBUTES	pNtfsAttr = (PNTFS_ATTRIBUTES)(g_lpbMFTRecord + sizeof(MFT_HEADER)) ;
	while ( pNtfsAttr->Type != 0xFFFFFFFF )	{
		printf( "type:%X\n" , pNtfsAttr->Type ) ;
// 		if ( pNtfsAttr->NonResidentFiag == 1 )	{	//非常驻属性
// 			//pNtfsAttr->CCommon.CNonResident.StartVCN ;
// 			//NTFS_ATTRIBUTES::CCommon::CNonResident*	lpCNonResident = (NTFS_ATTRIBUTES::CCommon::CNonResident*)(((LPBYTE)pNtfsAttr) + pNtfsAttr->ContentOffset) ;
// // 			printf( "aa\n" ) ;
// 		}
// 		else	{									//常驻属性
// 			//pNtfsAttr->CCommon.CResident.StreamLength ;
// 			//NTFS_ATTRIBUTES::CCommon::CResident*	lpCResident = (NTFS_ATTRIBUTES::CCommon::CResident*)(((LPBYTE)pNtfsAttr) + pNtfsAttr->ContentOffset) ;
// // 			printf( "aa\n" ) ;
// 		}

		pNtfsAttr = (PNTFS_ATTRIBUTES)(((LPBYTE)pNtfsAttr) + pNtfsAttr->Length) ;
	}

	CloseHandle( g_hDisk ) ;
	printf( "byebye\n" ) ;
	system( "pause" ) ;
	return 0;
}
*/
VOID	PauseShowError( LPCSTR lpszTitle ,DWORD dwError )
{
	printf( "[Error] %s : %d\n\n" , lpszTitle , dwError ) ;
	system( "pause" ) ;
	ExitProcess( 0x00 ) ;
}

DWORD	GetDiskData64( HANDLE hDisk , DWORD dwLen , LPBYTE lpOut , DWORD64 dw64Offset )
{
	if ( dw64Offset != ~0 ){
		LONG	lOffsetLow = (LONG)(dw64Offset & 0xFFFFFFFF) ;
		LONG	lOffsetHigh = (LONG)((dw64Offset >> 32) & 0xFFFFFFFF) ;
		SetFilePointer(hDisk, lOffsetLow, &lOffsetHigh, FILE_BEGIN);
	}

	DWORD	dwRead = 0 ;
	if( ReadFile(hDisk, lpOut, dwLen, &dwRead, NULL) == FALSE )
		dwRead = 0 ;

	return dwRead ;
}

DWORD	GetDiskData32( HANDLE hDisk , DWORD dwLen , LPBYTE lpOut , DWORD dwOffset )
{
	DWORD64	dw64Offset = ~0 ;
	if ( dwOffset != ~0 )
		dw64Offset = ((DWORD64)dwOffset) & 0xFFFFFFFF ;
	return	GetDiskData64( hDisk , dwLen , lpOut , dw64Offset ) ;
}