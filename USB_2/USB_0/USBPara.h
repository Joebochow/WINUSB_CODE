/****************************************************************
* COPYRIGHT(EMP),emperor electronic technology of ShenZhen
* FILENAME : USBPara.h
* AUTHOR : TangYingZhong
* VERSION : v1.0.0
* DATE : 2017/10/9
* DESCRIPTION : USB configure data
****************************************************************/
#ifndef 	SYSTEMMODEL_DAL_USBPARA_H_
#define	SYSTEMMODEL_DAL_USBPARA_H_


#include <Windows.h>

namespace SystemModel
{
	namespace DAL
	{
		/*
		* CLASSNAME : USBPara
		* FUNC : The configure para of serial port
		* AUTHOR : TangYingZhong
		* DATE : 2017/10/9
		*/
		class USBPara
		{
		public:
			//Status of operation
			typedef enum
			{
				FAILED=-1,
				SUCCESS=0
			}STATUS;

			typedef char*	ARRAY;
			typedef int  ARRAYSIZE;
			typedef DWORD	SIZE;
			typedef DWORD LENGTH;
			typedef DWORD CMD;

			//USB transfer para
			typedef struct
			{
				CMD cmd;
				ARRAY transferArray;
				ARRAYSIZE transferSize;
				ARRAY tempCache;
				ARRAYSIZE tempCacheHeadSize;
				SIZE returnSize;
				UCHAR endPointAddr;
			}TRNASFERPARA;

			//Asynchronous para
			typedef struct asyn
			{
				TRNASFERPARA transferPara;
				OVERLAPPED overlapped;
				typedef DWORD(*AsyncCallback)(struct asyn* ar);
				typedef LPVOID AsyncPara;
				AsyncCallback callback;
				AsyncPara asyncState;
			}AsyncResult;

			typedef AsyncResult::AsyncCallback ASYNCALLBACK;
			typedef DWORD	 DWORD;
			typedef LPVOID	 LPVOID;

			//USB target
			typedef enum
			{
				USB_DEVICE=0,
				USB_INTERFACE,
				USB_ENDPOINT,
				OTHER
			}TARGET;

			//USB request type
			typedef enum
			{
				STANDARD=0,
				CLASS,
				VENDOR
			}REQTYPE;

			//USB transfer direction
			typedef enum
			{
				TO_DEVICE=0,
				FROM_DEVICE
			}DIRECTION;


		};
	}
}

#endif //SYSTEMMODEL_DAL_USBPARA_H_


