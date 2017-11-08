#include "USB.h"
#include <iostream>



using namespace std;


int flag = 0;


DWORD SendCallBack(SystemModel::DAL::USB::ASYNCRESULT* asyn)
{
	SystemModel::DAL::IUSB* usb = (SystemModel::DAL::USB*)(asyn->asyncState);

	//Finish written
	DWORD bytes = usb->EndWrite(asyn);
	//Close the port
	usb->Close();

	cout << "Write to USB: " << bytes << endl;
	flag = 1;

	return bytes;
}

DWORD RecieveCallBack(SystemModel::DAL::USB::ASYNCRESULT* asyn)
{
	SystemModel::DAL::IUSB* usb = (SystemModel::DAL::USB*)(asyn->asyncState);

	//Finish the read
	DWORD bytes = usb->EndRecieve(asyn);
	//Close the port
	usb->Close();

	cout << "Read from USB: " << bytes << endl;

	flag = 1;
	return bytes;
}


int main(int argc, char *argv[])
{
	//Create an usb
	SystemModel::DAL::IUSB* usb = new SystemModel::DAL::USB();



	//Open the usb
	usb->BeginOpen();

	//Configure the USB before send and recieve data with USB
	typedef SystemModel::DAL::IUSB USBENUM;
	UCHAR reqCode = 0xc0;
	int value = 0;
	usb->Configure(USBENUM::TARGET::USB_INTERFACE, USBENUM::DIRECTION::TO_DEVICE, USBENUM::REQTYPE::VENDOR,reqCode,value,0);

	//Begin write data to USB
	char data1[] = "I am listenning vedio now ";
	usb->BeginWrite(data1,strlen(data1),SendCallBack);

	cout << "Is writting to USB...\r\n" << endl;
	while (flag == 0);
	flag = 0;
	cout << "Write successfully\r\n" << endl;




	//Open the usb
	usb->BeginOpen();

	//Begin read data from USB
	char data[512] = { 0 };
	usb->BeginRead(data, sizeof(data), RecieveCallBack);

	cout << "Is reading data from USB..." << endl;
	while (flag == 0);
	cout << "Read successfully\r\n" << endl;

	cout << "Show data below:" << endl;
	int count = 0;
	for (int i = 0; i < 512; ++i)
	{
		printf("0x%x ", data[i]);
		++count;
		if (count == 16)
		{
			printf("\r\n");
			count = 0;
		}
	}






	//Open the USB
	usb->Open();

	//Write USB
	char writeData[] = "I am listenning vedio now ";
	int writeBytes = usb->Write(writeData, strlen(writeData));
	cout << "\r\nWrite Data:"<<writeData << endl;
	cout << "Write USB successfully." << endl;

	//Read USB
	char readData[512] = { 0 };
	int readBytes = usb->Read(readData, sizeof(readData));
	cout << "Read USB:" << endl;

	for (int i = 0; i < 512; ++i)
	{
		printf("0x%x ", readData[i]);
		++count;
		if (count == 16)
		{
		printf("\r\n");
		count = 0;
		}
	}

	//Close USB
	usb->Close();

	//Delete USB
	delete usb;


	return 0;
}
