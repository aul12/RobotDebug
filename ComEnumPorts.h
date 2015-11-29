//*****************************************************************************
//*
//*
//*		ComEnumPorts.h
//*
//*
//*****************************************************************************
#ifndef 	__COMENUMPORTS_H__
#define 	__COMENUMPORTS_H__



typedef struct  
	{
    int 		bUsbDevice;				// Provided through a USB connection?
    char		cDevPath	 [128];		// Device path for use with CreateFile()
    char		cPortName    [128];		// Simple name (i.e. COM1)
    char		cFriendlyName[128];		// Full name to be displayed to a user
    char		cPortDesc	 [128];		// Friendly name without the COMx
	}ComInfo;

	int ComEnumPorts(ComInfo *pInfo,int iMax,int bIgnoreOpen=0);


#endif 	
