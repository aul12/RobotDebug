//*****************************************************************************
//*
//*
//*		ComEnumPorts.cpp
//*
//*
//*****************************************************************************
//
//	(C) Copyright Anton Zechner 2007
//
#include	<Windows.h>
#include	<string>
#include	<cstdio>
#include	<setupapi.h>
#include	"ComEnumPorts.h"


static GUID	GUID_CLASS_COMPORT_EX={ 0x86e0d1e0L, 0x8089, 0x11d0, 0x9c, 0xe4, 0x08, 0x00, 0x3e, 0x30, 0x1f, 0x73 };

static int	EnumPortsWdm   (ComInfo *pInfo,int iMax);
static int	EnumPortsWNt4  (ComInfo *pInfo,int iMax);
static int	EnumPortsW9x   (ComInfo *pInfo,int iMax);
static void SearchPnpKeyW9x(HKEY hPnp,BOOL bUsbDevice,ComInfo *pInfo,int iMax,int &iCount);



typedef HDEVINFO(__stdcall *TypeSetupDiGetClassDevs)				(const GUID FAR*, PCSTR, HWND, DWORD);
typedef HKEY	(__stdcall *TypeSetupDiOpenDevRegKey)				(HDEVINFO, PSP_DEVINFO_DATA, DWORD,DWORD,DWORD, REGSAM);
typedef BOOL	(__stdcall *TypeSetupDiEnumDeviceInterfaces)		(HDEVINFO, PSP_DEVINFO_DATA, const GUID FAR*, DWORD, PSP_DEVICE_INTERFACE_DATA);
typedef BOOL	(__stdcall *TypeSetupDiGetDeviceInterfaceDetail)	(HDEVINFO, PSP_DEVICE_INTERFACE_DATA, PSP_DEVICE_INTERFACE_DETAIL_DATA, DWORD, DWORD FAR*, PSP_DEVINFO_DATA);
typedef BOOL	(__stdcall *TypeSetupDiGetDeviceRegistryProperty)	(HDEVINFO, PSP_DEVINFO_DATA, DWORD, DWORD FAR*, BYTE FAR*, DWORD, DWORD FAR*);
typedef BOOL	(__stdcall *TypeSetupDiDestroyDeviceInfoList)		(HDEVINFO);


static TypeSetupDiGetClassDevs				PSetupDiGetClassDevs=0;
static TypeSetupDiOpenDevRegKey				PSetupDiOpenDevRegKey=0;
static TypeSetupDiEnumDeviceInterfaces		PSetupDiEnumDeviceInterfaces=0;
static TypeSetupDiGetDeviceInterfaceDetail	PSetupDiGetDeviceInterfaceDetail=0;
static TypeSetupDiGetDeviceRegistryProperty	PSetupDiGetDeviceRegistryProperty=0;
static TypeSetupDiDestroyDeviceInfoList		PSetupDiDestroyDeviceInfoList=0;

static HINSTANCE hDll=0;


//*****************************************************************************
//*
//*		ComEnumPorts
//*
//*****************************************************************************
//	Enumerates all COM ports. The results are writen to pInfo
//	pInfo		: array with received the port infos
//	iMax		: size of the pInfo array, if zero detect the port count
//	bIgnoreOpen	: don't enum open ports
//	Returns the number of dedected ports
int ComEnumPorts(ComInfo *pInfo,int iMax,int bIgnoreOpen)
{
OSVERSIONINFO	sOsInfo;
ComInfo		   *pCurrent;
char		   *pFirst,*pLast;
int				i,iLen,iFound;
HANDLE			hCom;


	if(iMax<0)return 0;

	memset(pInfo,0,sizeof(ComInfo)*iMax);	// Clear the output array
	sOsInfo.dwOSVersionInfoSize = sizeof(sOsInfo);

	if(!GetVersionEx(&sOsInfo))return 0;


	if (sOsInfo.dwMajorVersion<5)			// Windows 9x and NT4
		{
		if (sOsInfo.dwPlatformId==VER_PLATFORM_WIN32_NT)
				iFound=EnumPortsWNt4(pInfo,iMax);
		else	iFound=EnumPortsW9x (pInfo,iMax);
		}
	else{									// Win2k and later
				iFound=EnumPortsWdm (pInfo,iMax);
		}

	if(iMax==0)return iFound;				// Only enum port count


	for(i=0;i<iFound;i++)
		{
		pCurrent = pInfo+i;

		if(bIgnoreOpen)						// Only display ports that can be opened
			{
			    hCom = CreateFile(pCurrent->cDevPath,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);
			if (hCom == INVALID_HANDLE_VALUE)
				{
				memmove(pInfo+i,pInfo+i+1,sizeof(ComInfo)*(iFound-i-1));
				iFound--;
				i--;
				continue;
				}

			CloseHandle(hCom);
			}

		if(pCurrent->cFriendlyName[0]==0)	// If there is no friendly name, use the port name.
			{
			strncpy(pCurrent->cFriendlyName,
					pCurrent->cPortName,
					sizeof(pCurrent->cFriendlyName));
			}

		if(pCurrent->cPortDesc[0]==0)		// Strip off the " (COM?)"
			{
			pFirst = strchr(pCurrent->cPortDesc,'(');
			pLast  = strchr(pCurrent->cPortDesc,')');
			iLen   = strlen(pCurrent->cPortDesc);

			if(pFirst && pLast)
			if(pFirst <  pLast)
			if(pLast-pCurrent->cPortDesc == iLen-1)
				{
				pFirst[0]=0;
				}
			}
		}


return iFound;
}


//*****************************************************************************
//*
//*		EnumPortsWdm
//*
//*****************************************************************************
//	Enumeration for Win2000 and WinXP
static int EnumPortsWdm(ComInfo *pInfo,int iMax)
{
SP_DEVINFO_DATA					 sDevData;
SP_DEVICE_INTERFACE_DATA		 sIfcData;
SP_DEVICE_INTERFACE_DETAIL_DATA *pDetData;
GUID						    *pGuidDev;
DWORD							 dwDetDataSize;
DWORD							 dwError;
DWORD							 dwLength;
HDEVINFO						 hDevInfo;
HKEY							 hDrvSoftInfo;
TCHAR							 cDevPath[256];
TCHAR							 cLocInfo[256];
TCHAR							 cFname[256];
TCHAR							 cDesc[256];
BOOL							 bUsbDevice;
BOOL							 bSuccess;
BOOL							 bOk;
int								 i,iCount=0;


	if(!hDll)
		{
			hDll=LoadLibrary("setupapi.dll");
		if(!hDll)return 0;

		PSetupDiGetClassDevs				= (TypeSetupDiGetClassDevs			   )GetProcAddress(hDll,"SetupDiGetClassDevsA"				);
		PSetupDiOpenDevRegKey				= (TypeSetupDiOpenDevRegKey			   )GetProcAddress(hDll,"SetupDiOpenDevRegKey"				);
		PSetupDiEnumDeviceInterfaces		= (TypeSetupDiEnumDeviceInterfaces	   )GetProcAddress(hDll,"SetupDiEnumDeviceInterfaces"		);
		PSetupDiGetDeviceInterfaceDetail	= (TypeSetupDiGetDeviceInterfaceDetail )GetProcAddress(hDll,"SetupDiGetDeviceInterfaceDetailA"	);
		PSetupDiGetDeviceRegistryProperty	= (TypeSetupDiGetDeviceRegistryProperty)GetProcAddress(hDll,"SetupDiGetDeviceRegistryPropertyA"	);
		PSetupDiDestroyDeviceInfoList		= (TypeSetupDiDestroyDeviceInfoList	   )GetProcAddress(hDll,"SetupDiDestroyDeviceInfoList"		);
		}

	if(!PSetupDiGetClassDevs				)return 0;
	if(!PSetupDiOpenDevRegKey				)return 0;
	if(!PSetupDiEnumDeviceInterfaces		)return 0;
	if(!PSetupDiGetDeviceInterfaceDetail	)return 0;
	if(!PSetupDiGetDeviceRegistryProperty	)return 0;
	if(!PSetupDiDestroyDeviceInfoList		)return 0;



	pGuidDev = (GUID*) &GUID_CLASS_COMPORT_EX;

	// Create a device information set that will be the container for
	// the device interfaces.


	hDevInfo = INVALID_HANDLE_VALUE;
	pDetData = NULL;

	hDevInfo = PSetupDiGetClassDevs(pGuidDev,NULL,NULL,DIGCF_PRESENT|DIGCF_DEVICEINTERFACE);

	if(hDevInfo==INVALID_HANDLE_VALUE)
		{
		return 0;
		}


	// Enumerate the serial ports
	bOk					= TRUE;
	dwDetDataSize		= sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA) + 256;
	pDetData		    = (SP_DEVICE_INTERFACE_DETAIL_DATA*) new char[dwDetDataSize];
	pDetData->cbSize	= sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
	sIfcData.cbSize		= sizeof(SP_DEVICE_INTERFACE_DATA);

	for(i=0;bOk;i++)
		{
		   bOk = PSetupDiEnumDeviceInterfaces(hDevInfo,NULL,pGuidDev,i,&sIfcData);
		if(bOk)
			{
			sDevData.cbSize = sizeof(SP_DEVINFO_DATA);
			bOk = PSetupDiGetDeviceInterfaceDetail(hDevInfo,&sIfcData,pDetData,dwDetDataSize, NULL,&sDevData);

			if(bOk)
				{
				strncpy(cDevPath,pDetData->DevicePath,sizeof(cDevPath));
				// Got a path to the device. Try to get some more info.
				bSuccess   = PSetupDiGetDeviceRegistryProperty(hDevInfo,&sDevData,SPDRP_FRIENDLYNAME,NULL,(PBYTE)cFname,sizeof(cFname),NULL);
				bSuccess   = bSuccess && PSetupDiGetDeviceRegistryProperty(hDevInfo,&sDevData,SPDRP_DEVICEDESC,NULL,(PBYTE)cDesc,sizeof(cDesc),NULL);
				bUsbDevice = FALSE;

				if(PSetupDiGetDeviceRegistryProperty(hDevInfo,&sDevData,SPDRP_LOCATION_INFORMATION,NULL,(PBYTE)cLocInfo,sizeof(cLocInfo),NULL))
					{
					// Just check the first three characters to determine
					// if the port is connected to the USB bus. This isn't
					// an infallible method; it would be better to use the
					// BUS GUID. Currently, Windows doesn't let you query
					// that though (SPDRP_BUSTYPEGUID seems to exist in
					// documentation only).
					bUsbDevice = (strncmp(cLocInfo,"USB",3)==0);
					}

				if(!bUsbDevice && !strnicmp(cDevPath,"\\\\?\\usb",7))
					{
					bUsbDevice=1;
					}

				if(bSuccess)
					{
					if(iMax>0)
						{
						   hDrvSoftInfo =PSetupDiOpenDevRegKey( hDevInfo,&sDevData,DICS_FLAG_GLOBAL,0,DIREG_DEV,KEY_READ);
						if(hDrvSoftInfo!=INVALID_HANDLE_VALUE)
							{
							dwLength = sizeof(pInfo[iCount].cPortName)-1;
							RegQueryValueEx(hDrvSoftInfo,"PortName",NULL,NULL,(BYTE*)pInfo[iCount].cPortName,&dwLength);
							RegCloseKey(hDrvSoftInfo);
							}

						strncpy(pInfo[iCount].cDevPath,cDevPath		,sizeof(pInfo[iCount].cDevPath)-1);
						strncpy(pInfo[iCount].cFriendlyName,cFname	,sizeof(pInfo[iCount].cFriendlyName)-1);
						strncpy(pInfo[iCount].cPortDesc,cDesc		,sizeof(pInfo[iCount].cPortDesc)-1);
						pInfo[iCount].bUsbDevice = bUsbDevice;
						}
					iCount++;
					}
				}
			else{
				break;
				}
			}
		else{
			   dwError = GetLastError();
			if(dwError != ERROR_NO_MORE_ITEMS)
				{
				break;
				}
			}
		}


	if(pDetData!=NULL)delete pDetData;
	if(hDevInfo!=INVALID_HANDLE_VALUE)
		{
		PSetupDiDestroyDeviceInfoList(hDevInfo);
		}


return iCount;
}


//*****************************************************************************
//*
//*		EnumPortsWNt4
//*
//*****************************************************************************
//	Enumeration for Win2000 and WinXP
static int	EnumPortsWNt4(ComInfo *pInfo,int iMax)
{
int		i,iPos;
char	cName[]="\\\\.\\COM1";
HANDLE	hCom;


	iPos = 0;

	for(i=0;i<16;i++)
		{
		if(iMax!=0 && i>=iMax)break;


		sprintf(cName+7,"%i",i+1);
		hCom=CreateFile(cName,GENERIC_WRITE|GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
		if(hCom==INVALID_HANDLE_VALUE)
 			{
			if(GetLastError()==ERROR_FILE_NOT_FOUND)continue;
			}
		else{
			CloseHandle(hCom);
			}

		if(iMax)
			{
			sprintf(pInfo[iPos].cDevPath,"\\\\.\\COM%i",i+1);
			sprintf(pInfo[iPos].cPortName,      "COM%i",i+1);
			}

		iPos++;
		}



return iPos;
}


//*****************************************************************************
//*
//*		EnumPortsW9x
//*
//*****************************************************************************
//	Look at all keys in HKLM\Enum, searching for subkeys named
//	*PNP0500 and *PNP0501. Within these subkeys, search for
//	sub-subkeys containing value entries with the name "PORTNAME"
//	Search all subkeys of HKLM\Enum\USBPORTS for PORTNAME entries.
static int	EnumPortsW9x(ComInfo *pInfo,int iMax)
{
HKEY	hEnum			= NULL;
HKEY	hSubEnum		= NULL;
HKEY	hSubSubEnum		= NULL;
char	cSubEnum[128];
char	cSubSubEnum[128];
DWORD	dwSubSubEnumIndex;
DWORD	dwSubEnumIndex;
DWORD	dwSize;
BOOL	bUsbDevice;
BOOL	bMatch;
int		iCount=0;



	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,"Enum",0,KEY_READ,&hEnum) != ERROR_SUCCESS)
		{
		return 0;
		}


	dwSubEnumIndex = 0;
	dwSize = sizeof(cSubEnum);

	// Enumerate the subkeys of HKLM\Enum
	while(RegEnumKeyEx(hEnum,dwSubEnumIndex,cSubEnum,&dwSize,NULL,NULL,NULL,NULL)==ERROR_SUCCESS)
		{
		dwSubEnumIndex++;

		if(RegOpenKeyEx(hEnum,cSubEnum,0,KEY_READ,&hSubEnum)!=ERROR_SUCCESS)
			{
			continue;
			}


		// Enumerate the subkeys of HKLM\Enum\*\, looking for keys
		// named *PNP0500 and *PNP0501 (or anything in USBPORTS)

		bUsbDevice = strcmp(cSubEnum,"USBPORTS")? 0:1;
		dwSize	   = sizeof(cSubSubEnum);
		dwSubSubEnumIndex = 0;

		while(RegEnumKeyEx(hSubEnum,dwSubSubEnumIndex,cSubSubEnum,&dwSize,NULL,NULL,NULL,NULL)==ERROR_SUCCESS)
			{
			dwSubSubEnumIndex++;
			bMatch = (strcmp(cSubSubEnum,"*PNP0500")==0 ||
					  strcmp(cSubSubEnum,"*PNP0501")==0 ||
					  bUsbDevice);

			if(bMatch)
				{
				hSubSubEnum = NULL;

				if(RegOpenKeyEx(hSubEnum,cSubSubEnum,0,KEY_READ,&hSubSubEnum)!=ERROR_SUCCESS)
					{
					continue;
					}

				SearchPnpKeyW9x(hSubSubEnum, bUsbDevice, pInfo,iMax,iCount);
				RegCloseKey(hSubSubEnum);
				}

			dwSize = sizeof(cSubSubEnum);
			}

		RegCloseKey(hSubEnum);
		dwSize   = sizeof(cSubEnum);
		hSubEnum = NULL;
		}

	RegCloseKey(hEnum);

return iCount;
}


//*****************************************************************************
//*
//*		SearchPnpKeyW9x
//*
//*****************************************************************************
//	Enumerate the subkeys of the given PNP key, looking for values with
//	the name "PORTNAME" First, open HKLM\Enum
void SearchPnpKeyW9x(HKEY hPnp,BOOL bUsbDevice,ComInfo *pInfo,int iMax,int &iCount)
{
char	cSubPnp[128];
char	cPortName[128];
char	cFriendlyName[128];
DWORD	dwSubPnpIndex = 0;
DWORD	dwSize = sizeof(cSubPnp);
HKEY	hSubPnp = NULL;
BOOL	bDup;
int		i;


	// Enumerate the subkeys of HKLM\Enum\*\PNP050[01]
	dwSubPnpIndex = 0;
	dwSize = sizeof(cSubPnp);

	while(RegEnumKeyEx(hPnp,dwSubPnpIndex,cSubPnp,&dwSize,NULL,NULL,NULL,NULL)==ERROR_SUCCESS)
		{
		dwSubPnpIndex++;

		if(RegOpenKeyEx(hPnp,cSubPnp,0,KEY_READ,&hSubPnp)!=ERROR_SUCCESS)
			{
			continue;
			}

		// Look for the PORTNAME value
		dwSize = sizeof(cPortName)-1;

		if(RegQueryValueEx(hSubPnp,"PORTNAME",NULL,NULL,(BYTE*)cPortName,&dwSize)==ERROR_SUCCESS)
			{
			cPortName[dwSize]=0;
			cFriendlyName[0]=0;
			dwSize = sizeof(cFriendlyName)-1;

			if(RegQueryValueEx(hSubPnp,"FRIENDLYNAME",NULL,NULL,(BYTE*)cFriendlyName,&dwSize)==ERROR_SUCCESS)
				{
				cFriendlyName[dwSize]=0;
				}


			if(iMax==0)
				{
				iCount++;
				continue;
				}

			bDup = FALSE;

			for(i=0;i<iCount;i++)				// Overwrite duplicates
				{
				if(strcmp(pInfo[i].cPortName,cPortName))continue;
				bDup = TRUE;
				break;
				}

			sprintf(pInfo[i].cDevPath,"\\\\.\\%s",cPortName);
			strncpy(pInfo[i].cPortName,cPortName,sizeof(pInfo[i].cPortName));
			strncpy(pInfo[i].cFriendlyName,cFriendlyName,sizeof(pInfo[i].cFriendlyName));
			pInfo[i].bUsbDevice=bUsbDevice;

			if(!bDup)iCount++;
			}

		RegCloseKey(hSubPnp);
		hSubPnp = NULL;
		dwSize = sizeof(cSubPnp);				// restore the buffer size
		}

}
