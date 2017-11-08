#include "USB.h"
#include "EmpString.h"
#include <string>
#pragma comment(lib, "setupapi")


using namespace std;
using namespace SystemModel::DAL;



USB::GUID USB::USBGUID = { 0xAE18AA60, 0x7F6A, 0x11D4, 0x97, 0xDD, 0x00, 0x01, 0x02, 0x29, 0xb9, 0x59 };
USB::STRING USB::VID_PID = "vid_0547&pid_1005";


CRITICAL_SECTION section;



//Recive CallBack function
USB::UDWORD WINAPI USB::AysnSend(ULPVOID para)
{
	ASYNCRESULT *asynResult = (ASYNCRESULT*)para;
	USB* usb = (USB*)asynResult->asyncState;

	memset(&(asynResult->overlapped), 0, sizeof(OVERLAPPED));
	asynResult->overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	EnterCriticalSection(&section);
	{
		usb->TransferData(asynResult);
	}
	LeaveCriticalSection(&section);
	return 0;
}

//Recive CallBack function
USB::UDWORD WINAPI USB::AysnRecieve(ULPVOID para)
{
	ASYNCRESULT *asynResult = (ASYNCRESULT*)para;
	USB* usb = (USB*)asynResult->asyncState;

	memset(&(asynResult->overlapped), 0, sizeof(OVERLAPPED));
	asynResult->overlapped.hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);

	EnterCriticalSection(&section);
	{
		usb->TransferData(asynResult);
	}
	LeaveCriticalSection(&section);
	return 0;
}






//Construct the USB
USB::USB(GUID guid)
{
	//Init the usb device
	InitUSB(guid);

	InitializeCriticalSection(&section);
}

//Destruct the USB
USB::~USB()
{
	//Destory the usb
	DestoryUSB();

	DeleteCriticalSection(&section);
}

//Initialize the Client
USB::NONE USB::InitUSB(GUID guid)
{
	//Create USB handle
	CreateUSBHandle();
	//Set GUID
	SetGUID(guid);
	//Set the closed
	SetClosed(false);
}

//Destory the Client
USB::NONE USB::DestoryUSB()
{
	if (!GetClosed())
	{
		SetClosed(true);
		//Destory the usb handle
		DestoryUSBHandle();
	}
}

//Create the handle
USB::NONE USB::CreateUSBHandle()
{
	SetUSBHandle(NULL);
}

//Destory the usb handle
USB::NONE USB::DestoryUSBHandle()
{
	if (GetUSBHandle())
	{
		::CloseHandle(GetUSBHandle());
		SetUSBHandle(NULL);
	}
}





//Get the device instance path here
USB::NONE USB::GetUSBDevicePath()
{
	char* pszDevicePath[24] = { NULL };
	int nCount = 0;

	HDEVINFO hDevInfoSet;
	SP_DEVICE_INTERFACE_DATA ifdata;
	PSP_DEVICE_INTERFACE_DETAIL_DATA pDetail;
	int bResult=0;

	GUID deviceGuid = guid;


	//Get device info bound to the GUID given
	hDevInfoSet = SetupDiGetClassDevs(&(GetGUID()), 
								NULL,						
								NULL,					 
								DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);	//Exist device now
	if (hDevInfoSet == INVALID_HANDLE_VALUE)
	{
		return;
	}
	//Allocate space for the device interface data section
	pDetail = (PSP_DEVICE_INTERFACE_DETAIL_DATA)::GlobalAlloc(LMEM_ZEROINIT, 1024);

	if (pDetail == NULL)
	{
		//Clear the device info
		SetupDiDestroyDeviceInfoList(hDevInfoSet);
		return;
	}
	//This size must be initialized
	pDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
	bResult = TRUE;

	//Search device interface from index 0
	while (bResult)
	{
		ifdata.cbSize = sizeof(ifdata);

		//Enum device interface owing the GUID
		bResult = ::SetupDiEnumDeviceInterfaces(
									hDevInfoSet,			// The para got from the return of SetupDiGetClassDevs
									NULL,						
									&deviceGuid,			//Provided GUID
									(ULONG)nCount,		//The index of device in Device info
									&ifdata);					//The interface info of device
		if (bResult)
		{
			//Get the interface detail info of the device(Get device instance path)
			bResult = SetupDiGetInterfaceDeviceDetail(
								hDevInfoSet,				//The para got from the return of SetupDiGetClassDevs
								&ifdata,						//The interface info of device
								pDetail,						//The interface detail info including device instance path
								1024,							//Output cache size
								NULL,							//No need
								NULL);							//No need extra device description
			if (bResult)
			{
				//Copy the device instance path to array
				pszDevicePath[nCount] = pDetail->DevicePath;
				//Add the index of device in Device info
				nCount++;
			}
		}
	}
	//Here, we get all device instance path which has the same GUID
	for (int i = 0; i<nCount; i++)
	{
		typedef SystemModel::DAL::EmpString EMPSTRING;
		EMPSTRING empStr;
		char forwardStr[1024] = { 0 };
		char backStr[1024] = { 0 };
		char backStr2[1024] = { 0 };
		empStr.SubString(pszDevicePath[i], '#', forwardStr, backStr);
		empStr.SubString(backStr, '#', forwardStr, backStr2);

		if (strcmp(forwardStr, USB::VID_PID) == 0)
		{
			memset(devicePath,0,USB::MAXLEN);
			memcpy(devicePath, pszDevicePath[i], strlen(pszDevicePath[i]));
			break;
		}
	}

	//Free the global memory
	GlobalFree(pDetail);
}

//Data transfer for asyn mode
USB::NONE USB::TransferData(ASYNCRESULT* asyn)
{
	BOOL success = false;

	USB* usb = (USB*)asyn->asyncState;

	asyn->transferPara.tempCacheHeadSize = sizeof(SINGLE_TRANSFER);
	int bufferSize = asyn->transferPara.tempCacheHeadSize + asyn->transferPara.transferSize;
	asyn->transferPara.tempCache = new char[bufferSize];
	memset(asyn->transferPara.tempCache, 0, bufferSize);

	PSINGLE_TRANSFER pSingleTransfer = (PSINGLE_TRANSFER)asyn->transferPara.tempCache;
	pSingleTransfer->ucEndpointAddress = asyn->transferPara.endPointAddr;
	pSingleTransfer->IsoPacketLength = 0;
	pSingleTransfer->BufferOffset = sizeof(SINGLE_TRANSFER);
	pSingleTransfer->BufferLength = asyn->transferPara.transferSize;

	ARRAYADDRESS p = asyn->transferPara.tempCache + pSingleTransfer->BufferOffset;
	memcpy(p, asyn->transferPara.transferArray, asyn->transferPara.transferSize);

	// attempt an synchronous read operation
	int bResult = DeviceIoControl(usb->GetUSBHandle(),
		asyn->transferPara.cmd,
		asyn->transferPara.tempCache,
		bufferSize,
		asyn->transferPara.tempCache,
		bufferSize,
		&(asyn->transferPara.returnSize),
		&(asyn->overlapped));
	if (!bResult)
	{
		if (::GetLastError() == ERROR_IO_PENDING)
		{
			DWORD ret = WaitForSingleObject(asyn->overlapped.hEvent, INFINITE);
			if (ret == WAIT_OBJECT_0)
			{
				if (asyn->callback)
				{
					asyn->callback(asyn);
				}
			}
		}
	}
}

//Data transfer 
USB::RESULT USB::TransferData(TRNASFERPARA* transferPara)
{
	int bufferSize = transferPara->tempCacheHeadSize + transferPara->transferSize;
	ARRAYADDRESS buffer = transferPara->tempCache;

	PSINGLE_TRANSFER pSingleTransfer = (PSINGLE_TRANSFER)buffer;
	pSingleTransfer->ucEndpointAddress = transferPara->endPointAddr;
	pSingleTransfer->IsoPacketLength = 0;
	pSingleTransfer->BufferOffset = sizeof(SINGLE_TRANSFER);
	pSingleTransfer->BufferLength = transferPara->transferSize;

	ARRAYADDRESS p = buffer + pSingleTransfer->BufferOffset;
	memcpy(p, transferPara->transferArray, transferPara->transferSize);

	// attempt an synchronous read operation
	int bResult = DeviceIoControl(GetUSBHandle(),
		transferPara->cmd,
		buffer,
		bufferSize,
		buffer,
		bufferSize,
		&(transferPara->returnSize),
		NULL);

	return bResult;
}











//Open the usb device
USB::STATUS USB::Open()
{
	STATUS status = STATUS::FAILED;

	if (GetUSBHandle() == NULL)
	{
		//Get the device instance path
		GetUSBDevicePath();

		//Open the USB device
		USBHANDLE usbHandle = CreateFile(devicePath,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);


		SetUSBHandle(usbHandle);
		status = STATUS::SUCCESS;
	}

	return status;
}

//Write Config to USB
USB::BOOL USB::Configure(TARGET target, DIRECTION direction, REQTYPE reqType, REQCODE reqCode, VALUE value, INDEX index)
{
	BOOL success = false;

	if (GetUSBHandle())
	{
		UCHAR buf[64] = { 0 };
		USHORT bytesSend = 64;

		int bufferSize = sizeof(SINGLE_TRANSFER)+bytesSend;
		ARRAYADDRESS buffer = new char[bufferSize];
		memset(buffer, 0, bufferSize);

		PSINGLE_TRANSFER pSingleTransfer = (PSINGLE_TRANSFER)buffer;
		pSingleTransfer->SetupPacket.bmReqType.Recipient = target;
		pSingleTransfer->SetupPacket.bmReqType.Type = reqType;
		pSingleTransfer->SetupPacket.bmReqType.Direction = direction;
		pSingleTransfer->SetupPacket.bRequest = reqCode;
		pSingleTransfer->SetupPacket.wValue = value;
		pSingleTransfer->SetupPacket.wIndex = index;
		pSingleTransfer->SetupPacket.wLength = bytesSend;
		pSingleTransfer->SetupPacket.ulTimeOut = 1000 / 1000;

		pSingleTransfer->ucEndpointAddress = USB::CONTROLENDPOINT;
		pSingleTransfer->IsoPacketLength = 0;
		pSingleTransfer->BufferOffset = sizeof(SINGLE_TRANSFER);
		pSingleTransfer->BufferLength = bytesSend;

		SIZE nBytesRead = 0;
		// attempt an synchronous read operation
		int bResult = DeviceIoControl(GetUSBHandle(),
			IOCTL_ADAPT_SEND_EP0_CONTROL_TRANSFER,
			buffer,
			bufferSize,
			buffer,
			bufferSize,
			&nBytesRead,
			NULL);
		if (!bResult)
		{
			nBytesRead = 0;
			success = false;
		}

		//ARRAYADDRESS p = buffer + pSingleTransfer->BufferOffset;
		//memcpy(buf, p, nBytesRead);
		delete buffer;

		success = true;
	}

	return success;
}

//Read data from the other side
USB::SIZE USB::Read(ARRAYADDRESS outArray, ARRAYSIZE outArraySize)
{
	SIZE nBytesRead=0;

	if (outArraySize == NULL || outArray == NULL)
	{
		SetLastError(ERROR_NOACCESS);
		return nBytesRead;
	}

	if (GetUSBHandle())
	{
		int bufferSize = sizeof(SINGLE_TRANSFER)+outArraySize;
		ARRAYADDRESS buffer = new char[bufferSize];
		memset(buffer, 0, bufferSize);

		TRNASFERPARA transferPara;
		transferPara.cmd = IOCTL_ADAPT_SEND_NON_EP0_TRANSFER;
		transferPara.endPointAddr = USB::BULKINENPOINT;
		transferPara.tempCache = buffer;
		transferPara.tempCacheHeadSize = sizeof(SINGLE_TRANSFER);
		transferPara.transferArray = outArray;
		transferPara.transferSize = outArraySize;
		

		int bResult = TransferData(&transferPara);
		if (bResult)
		{
			nBytesRead = transferPara.returnSize;
		}

		ARRAYADDRESS p1 = buffer + transferPara.tempCacheHeadSize;
		memcpy(outArray, p1, nBytesRead - transferPara.tempCacheHeadSize);

		delete buffer;
	}
	return nBytesRead;
}

//Write data to the other side
USB::SIZE USB::Write(ARRAYADDRESS inArray, ARRAYSIZE inArraySize)
{
	SIZE nBytesWrite = 0;

	if (inArray == NULL || inArraySize == NULL)
	{
		SetLastError(ERROR_NOACCESS);
		return nBytesWrite;
	}

	if (GetUSBHandle())
	{
		int bufferSize = sizeof(SINGLE_TRANSFER)+inArraySize;
		ARRAYADDRESS buffer = new char[bufferSize];
		memset(buffer, 0, bufferSize);

		TRNASFERPARA transferPara;
		transferPara.cmd = IOCTL_ADAPT_SEND_NON_EP0_TRANSFER;
		transferPara.endPointAddr = USB::BULKOUTENPOINT;
		transferPara.tempCache = buffer;
		transferPara.tempCacheHeadSize = sizeof(SINGLE_TRANSFER);
		transferPara.transferArray = inArray;
		transferPara.transferSize = inArraySize;
		

		int bResult = TransferData(&transferPara);
		if (bResult)
		{
			nBytesWrite = transferPara.returnSize;
		}
		delete buffer;
	}
	return nBytesWrite;
}

//Open the usb device
USB::STATUS USB::BeginOpen()
{
	STATUS status = STATUS::FAILED;

	if (GetUSBHandle() == NULL)
	{
		//Get the device instance path
		GetUSBDevicePath();

		//Open the USB device
		USBHANDLE usbHandle = CreateFile(devicePath,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
			NULL);


		SetUSBHandle(usbHandle);
		status = STATUS::SUCCESS;
	}
	return status;
}

//Read data from the other side
//CallBack function like (DWORD CallBack(ASYNCRESULT* asyn))
USB::STATUS USB::BeginRead(ARRAYADDRESS outArray, ARRAYSIZE outArraySize, ASYNCALLBACK callback)
{
	STATUS status = STATUS::FAILED;

	if (outArraySize == NULL || outArray == NULL)
	{
		SetLastError(ERROR_NOACCESS);
		return status;
	}
	if (GetUSBHandle())
	{
		ASYNCRESULT* asynResult = new ASYNCRESULT();
		asynResult->transferPara.cmd = IOCTL_ADAPT_SEND_NON_EP0_TRANSFER;
		asynResult->transferPara.endPointAddr = USB::BULKINENPOINT;
		asynResult->transferPara.transferArray = outArray;
		asynResult->transferPara.transferSize = outArraySize;
		asynResult->callback = callback;
		asynResult->asyncState = this;

		//Create a thread for event
		SetRecieveThread(::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AysnRecieve, (ULPVOID)asynResult, 0, NULL));

		status = STATUS::SUCCESS;
	}
	return status;
}

//Write data to the other side
//CallBack function like (DWORD CallBack(ASYNCRESULT* asyn))
USB::STATUS USB::BeginWrite(ARRAYADDRESS inArray, ARRAYSIZE inArraySize, ASYNCALLBACK callback)
{
	STATUS status = STATUS::FAILED;
	if (GetUSBHandle())
	{
		ASYNCRESULT* asynResult = new ASYNCRESULT();
		asynResult->transferPara.cmd = IOCTL_ADAPT_SEND_NON_EP0_TRANSFER;
		asynResult->transferPara.endPointAddr = USB::BULKOUTENPOINT;
		asynResult->transferPara.transferArray = inArray;
		asynResult->transferPara.transferSize = inArraySize;
		asynResult->callback = callback;
		asynResult->asyncState = this;

		//Create a thread for event
		SetSendThread(::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AysnSend, (ULPVOID)asynResult, 0, NULL));

		status = STATUS::SUCCESS;
	}
	return status;
}

//Get the bytes read in synchronous mode
USB::LENGTH USB::EndRecieve(ASYNCRESULT* result)
{
	LENGTH bytesRead = 0;

	//Copy the data the array
	bytesRead = result->transferPara.transferSize;
	ARRAYADDRESS p1 = result->transferPara.tempCache + result->transferPara.tempCacheHeadSize;
	memcpy(result->transferPara.transferArray, p1, result->transferPara.transferSize);
	//Delete the temp cache
	delete result->transferPara.tempCache;
	//Close the event
	ResetEvent(result->overlapped.hEvent);
	CloseHandle(result->overlapped.hEvent);
	delete result;

	return bytesRead;
}

//Get the bytes write in synchronous mode
USB::LENGTH USB::EndWrite(ASYNCRESULT* result)
{
	LENGTH bytesWrite = 0;

	//Record the return size of written
	bytesWrite = result->transferPara.transferSize;
	delete result->transferPara.tempCache;
	//Close the event
	ResetEvent(result->overlapped.hEvent);
	CloseHandle(result->overlapped.hEvent);
	delete result;

	return bytesWrite;
}

//Close the usb device
USB::NONE USB::Close()
{
	//Destory the usb handle
	DestoryUSBHandle();
}














//Get the closed flag
USB::BOOL USB::GetClosed() const
{
	return closed;
}

//Set the closed flag
USB::NONE USB::SetClosed(BOOL closed)
{
	this->closed = closed;
}

//Get the guid
USB::GUID USB::GetGUID() const
{
	return guid;
}

//Set the guid
USB::NONE USB::SetGUID(GUID gId)
{
	guid = gId;
}

//Get the usb handle
USB::USBHANDLE USB::GetUSBHandle() const
{
	return usbHandle;
}

//Set the usb handle
USB::NONE USB::SetUSBHandle(USBHANDLE h)
{
	this->usbHandle = h;
}

//Set the thread
USB::NONE USB::SetRecieveThread(THREADHANDLE handle)
{
	this->recieveThread = handle;
}

//Get the recieve thread
USB::THREADHANDLE USB::GetSendThread() const
{
	return this->sendThread;
}

//Set the recieve thread
USB::NONE USB::SetSendThread(THREADHANDLE handle)
{
	this->sendThread = handle;
}














