#ifndef SYNCRONOUSGRAB
#define SYNCRONOUSGRAB

#include <windows.h>
#include <VimbaC/Include/VimbaC.h>

VmbError_t	SynchronousGrab			( char* pCameraID, char* pFileName );
HANDLE		CreateRawFile			( LPTSTR pszFile, const VmbFrame_t* pFrame );
HANDLE		CreateBitmapFile		( LPTSTR pszFile, PBITMAPINFO pbi, const VmbFrame_t* pFrame );
PBITMAPINFO CreateBitmapInfoStruct	( HBITMAP hBmp );
HANDLE		CreateImageFile			( const VmbFrame_t* pFrame, const char* pFile );

#endif
