/****************************************************************
* COPYRIGHT(EMP),emperor electronic technology of ShenZhen
* FILENAME : IUSB.h
* AUTHOR : TangYingZhong
* VERSION : v1.0.0
* DATE : 2017/9/4
* DESCRIPTION : USB interface
****************************************************************/
#ifndef		SYSTEMMODEL_DAL_IUSB_H_
#define	SYSTEMMODEL_DAL_IUSB_H_


#include "USBPara.h"

namespace SystemModel
{
	namespace DAL
	{
		/*
		* CLASSNAME : IUSB
		* FUNC : Usb's interface
		* AUTHOR : Tangyz
		* DATE : 2017/9/4
		*/
		class IUSB
		{
		public:
			typedef void NONE;
			typedef bool BOOL;
			typedef HANDLE USBHANDLE;
			typedef DWORD  USBDEVICEINDX;
			typedef char*	 ARRAYADDRESS;
			typedef DWORD*  PLENGTH;
			typedef DWORD ARRAYSIZE;
			typedef DWORD  DELAYMS;
			typedef DWORD LENGTH;
			typedef UCHAR REQCODE;
			typedef int VALUE;
			typedef int INDEX;

			typedef USBPara::STATUS STATUS;
			typedef USBPara::ASYNCALLBACK ASYNCALLBACK;
			typedef USBPara::AsyncResult ASYNCRESULT;
			typedef USBPara::DWORD UDWORD;
			typedef USBPara::LPVOID ULPVOID;
			typedef USBPara::SIZE SIZE;
			typedef USBPara::CMD CMD;
			typedef USBPara::TARGET TARGET;
			typedef USBPara::REQTYPE REQTYPE;
			typedef USBPara::DIRECTION DIRECTION;
			typedef USBPara::TRNASFERPARA TRNASFERPARA;

		public:
			virtual ~IUSB(){		}

		public:
			//Open the usb device
			virtual STATUS Open() = 0;

			//Write Config to USB
			virtual BOOL Configure(TARGET target, DIRECTION direction, REQTYPE reqType, REQCODE reqCode, VALUE value, INDEX index) = 0;

			//Read data from the other side
			virtual SIZE Read(ARRAYADDRESS outArray, ARRAYSIZE outArraySize) = 0;

			//Write data to the other side
			virtual SIZE Write(ARRAYADDRESS inArray, ARRAYSIZE inArraySize) = 0;

			//Open the usb device
			virtual STATUS BeginOpen() = 0;

			//Read data from the other side
			//CallBack function like (DWORD CallBack(ASYNCRESULT* asyn))
			virtual STATUS BeginRead(ARRAYADDRESS outArray, ARRAYSIZE outArraySize, ASYNCALLBACK callback) = 0;

			//Write data to the other side
			//CallBack function like (DWORD CallBack(ASYNCRESULT* asyn))
			virtual STATUS BeginWrite(ARRAYADDRESS inArray, ARRAYSIZE inArraySize, ASYNCALLBACK callback) = 0;

			//Get the bytes read in synchronous mode
			virtual LENGTH EndRecieve(ASYNCRESULT* result) = 0;

			//Get the bytes write in synchronous mode
			virtual LENGTH EndWrite(ASYNCRESULT* result) = 0;

			//Close the usb device
			virtual NONE Close() = 0;
		};
	}
}

#endif //SYSTEMMODEL_DAL_IUSB_H_


