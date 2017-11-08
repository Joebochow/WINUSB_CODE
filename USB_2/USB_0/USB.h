/****************************************************************
* COPYRIGHT(EMP),emperor electronic technology of ShenZhen
* FILENAME : USB.h
* AUTHOR : TangYingZhong
* VERSION : v1.0.0
* DATE : 2017/9/18
* DESCRIPTION : USB driver
****************************************************************/
#ifndef		SYSTEMMODEL_DAL_USB_H_
#define	SYSTEMMODEL_DAL_USB_H_

#include "IUSB.h"
#include <SetupAPI.h>
#include "cyioctl.h"
#include <iostream>

namespace SystemModel
{
	namespace DAL
	{
		/*
		* CLASSNAME : USB
		* FUNC : USB application driver
		* AUTHOR : Tangyz
		* DATE : 2017/9/18
		*/
		class USB :public IUSB
		{
		public:
			typedef bool BOOL;
			typedef int RESULT;
			typedef GUID  GUID;
			typedef char DEVICEPATH;
			typedef char* STRING;
			typedef int	MAXPATHLENGTH;
			typedef HDEVINFO USBDEVICEINFO;
			typedef SP_DEVICE_INTERFACE_DATA		INTERFACEDATA;
			typedef PSP_DEVICE_INTERFACE_DETAIL_DATA	INTERFACEDETAILDATA;
			typedef int	NUMBER;
			typedef int	INDEX;
			typedef HANDLE USBTHREAD;
			typedef HANDLE	 THREADHANDLE;

		public:
			//Construct the USB
			USB(GUID guid = USBGUID);

			//Destruct the USB
			~USB();

		private:
			//Copy construct USB
			USB(USB& other){		}

			//Assignment
			USB& operator=(USB& other){	}

		public:
			//Open the usb device
			virtual STATUS Open();

			//Write Config to USB
			virtual BOOL Configure(TARGET target, DIRECTION direction, REQTYPE reqType, REQCODE reqCode, VALUE value, INDEX index);

			//Read data from the other side
			virtual SIZE Read(ARRAYADDRESS outArray, ARRAYSIZE outArraySize);

			//Write data to the other side
			virtual SIZE Write(ARRAYADDRESS inArray, ARRAYSIZE inArraySize);

			//Open the usb device
			virtual STATUS BeginOpen();

			//Read data from the other side
			//CallBack function like (DWORD CallBack(ASYNCRESULT* asyn))
			virtual STATUS BeginRead(ARRAYADDRESS outArray, ARRAYSIZE outArraySize, ASYNCALLBACK callback);

			//Write data to the other side
			//CallBack function like (DWORD CallBack(ASYNCRESULT* asyn))
			virtual STATUS BeginWrite(ARRAYADDRESS inArray, ARRAYSIZE inArraySize, ASYNCALLBACK callback);

			//Get the bytes read in synchronous mode
			virtual LENGTH EndRecieve(ASYNCRESULT* result);

			//Get the bytes write in synchronous mode
			virtual LENGTH EndWrite(ASYNCRESULT* result);

			//Close the usb device
			virtual NONE Close();

		private:
			//Initialize the Client
			NONE InitUSB(GUID guid);

			//Destory the Client
			NONE DestoryUSB();

			//Create the handle
			NONE CreateUSBHandle();

			//Destory the usb handle
			NONE DestoryUSBHandle();

		private:
			//Recieve CallBack function
			static UDWORD WINAPI AysnRecieve(ULPVOID para);

			//Send CallBack function
			static UDWORD WINAPI AysnSend(ULPVOID para);

			//Get device instance path
			NONE GetUSBDevicePath();

			//Data transfer for asyn mode
			NONE TransferData(ASYNCRESULT* asyn);

			//Data transfer 
			RESULT TransferData(TRNASFERPARA* transferPara);

		private:
			//Set the usb handle
			NONE SetUSBHandle(USBHANDLE h);

			//Get the usb handle
			USBHANDLE GetUSBHandle() const;

			//Get the recieve thread
			THREADHANDLE GetRecieveThread() const;

			//Set the recieve thread
			NONE SetRecieveThread(THREADHANDLE handle);

			//Get the recieve thread
			THREADHANDLE GetSendThread() const;

			//Set the recieve thread
			NONE SetSendThread(THREADHANDLE handle);

			//Get the guid
			GUID GetGUID() const;

			//Set the guid
			NONE SetGUID(GUID gId);

			//Get the closed flag
			BOOL GetClosed() const;

			//Set the closed flag
			NONE SetClosed(BOOL closed);

		private:
			//Vendor ID Product ID
			static  STRING VID_PID;

			//GUID
			static  GUID USBGUID;

			//Max device path length
			static const MAXPATHLENGTH MAXLEN = 250;

			//Max USB device on the host
			static const NUMBER MAXUSBDEVICE = 127;

			//Bulk in endpoint address
			static const UCHAR BULKINENPOINT = 0x86;

			//Bulk out endpoint address
			static const UCHAR BULKOUTENPOINT = 0x02;

			//Control endpoint address
			static const UCHAR CONTROLENDPOINT = 0x00;

			//USB device path
			DEVICEPATH devicePath[MAXLEN];

			//USB handle
			USBHANDLE usbHandle;

			//Thread of reciving data
			THREADHANDLE recieveThread;

			//Thread of sending data
			THREADHANDLE sendThread;

			//GUID of USB
			GUID guid;

			//USB closed
			BOOL closed;

		};
	}
}





#endif //SYSTEMMODEL_DAL_USBSERVICE_H_


