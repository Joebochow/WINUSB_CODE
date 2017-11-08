#include "EmpString.h"

using namespace SystemModel::DAL;


//Construct the EmpString
EmpString::EmpString()
{
	//Create the EmpString
	Create();
}

//Destruct the EmpString
EmpString::~EmpString()
{
	//Destory the EmpString
	Destory();
}

//Copy construct EmpString
EmpString::EmpString(EmpString& other)
{
	operator=(other);
}

//Assignment
EmpString& EmpString::operator=(EmpString& other)
{
	if (&other != this)
	{
		LENGTH len = other.Length() + 1;
		//Malloc a space for EmpString
		SetString(Malloc(len));
		InitMemory(GetString(), len);
		//Copy the EmpString
		Copy(GetString(), len, other.GetString(), other.Length());
	}
	return *this;
}

//Create the EmpString
EmpString::NONE EmpString::Create()
{
	//Malloc EmpString
	MallocStr();
	//Set closed 
	SetClosed(false);
}

//Destory the EmpString
EmpString::NONE EmpString::Destory()
{
	if (!GetClosed())
	{
		SetClosed(true);
		//Destory EmpString
		DestoryStr();
	}
}

//Malloc EmpString
EmpString::NONE EmpString::MallocStr()
{
	SetString(Malloc(EmpString::STRSIZE));
	InitMemory(GetString(), EmpString::STRSIZE);
}

//Destroy the EmpString
EmpString::NONE EmpString::DestoryStr()
{
	if (GetString())
	{
		Free(GetString());
		SetString(NULL);
	}
}








//Malloc the EmpString space
EmpString::STRING EmpString::Malloc(SIZE size)
{
	return ((STRING)malloc(sizeof(char)*size));
}

//Free the space
EmpString::NONE EmpString::Free(STRING pStr)
{
	free(pStr);
}

//Resize the space
EmpString::STRING EmpString::ReSize(SIZE size)
{
	//Malloc a new size
	return (Malloc(size));
}

//Back up the EmpString 
EmpString::NONE EmpString::BackUp(STRING* current)
{
	if (GetString())
	{
		*current = GetString();
		return;
	}
	*current = NULL;
}

//Copy the EmpString 
EmpString::NONE EmpString::Copy(STRING dest, LENGTH destLen, STRING src, LENGTH srcLen)
{
	if (dest != NULL || src != NULL || destLen != 0 || srcLen != 0)
	{
		if (destLen > srcLen)
		{
			memcpy_s(dest, destLen, src, srcLen);
		}
	}
}

//Check buffer size
EmpString::STATUS EmpString::EnoughBuffer(STRING newStr)
{
	if (newStr == NULL)
	{
		return STATUS::EMPTY;
	}
	else
	{
		LENGTH lenLeft = STRSIZE - this->Length();
		LENGTH lenNewStr = strlen(newStr)+1;

		if (lenLeft < lenNewStr)
		{
			return STATUS::MEM_OVERFLOW;
		}

		return STATUS::NORMAL;
	}
}

//Update the EmpString pointer
EmpString::PSTRING EmpString::UpdateBufferPointer()
{
	PSTRING pStr = GetString();

	pStr = pStr + this->Length();

	return pStr;
}

//Zero the memory
EmpString::NONE EmpString::InitMemory(STRING pStr, LENGTH len)
{
	memset(pStr,0,len);
}

//Directly append
EmpString::NONE EmpString::DirectAppend(STRING srcStr)
{
	//Enough and set the buffer pointer
	STRING pStr = UpdateBufferPointer();
	//Copy the valid EmpString to buffer
	Copy(pStr, EmpString::STRSIZE - this->Length(), srcStr, strlen(srcStr) + 1);
}

//Resize and append
EmpString::NONE EmpString::ReSizeAndAppend(STRING srcStr)
{
	//Not enough,resize the buffer size
	LENGTH strLen = strlen(srcStr) + 1;
	STRING pStr = NULL;
	//Back up the old buffer
	BackUp(&pStr);
	//Set the new size (100: extra size)
	SIZE newSize = strLen + strlen(pStr) + 10;
	//Set the new buffer 
	SetString(ReSize(newSize));
	InitMemory(GetString(), newSize);
	if (*pStr != 0)
	{
		//Copy the valid EmpString to buffer
		STRING pBuffer0 = UpdateBufferPointer();
		Copy(pBuffer0, newSize, pStr, strlen(pStr) + 1);
	}
	//Update the buffer pointer
	STRING pBuffer = UpdateBufferPointer();
	//Copy the new EmpString
	Copy(pBuffer, newSize - strlen(pStr) - 1, srcStr, strlen(srcStr) + 1);
	Free(pStr);
}









//Append EmpString
EmpString::ISTRING* EmpString::Append(STRING str)
{
	//Check buffer size
	STATUS status = EnoughBuffer(str);
	if (status == STATUS::EMPTY)
	{
		return NULL;
	}
	else if (status == STATUS::NORMAL)
	{
		//Direct append with the current space molloced
		DirectAppend(str);

		return this;
	}
	else
	{
		//ReSize the space for new EmpString and append to the old one
		ReSizeAndAppend(str);

		return this;
	}
}

//Get sub EmpString 
EmpString::NONE EmpString::SubString(STRING srcStr, CHARACTOR splitChar, STRING forwardStr, STRING backStr)
{
	//Check the input
	if (srcStr == NULL || forwardStr == NULL || backStr == NULL)
	{
		return;
	}
	STRING src = srcStr;
	LENGTH srcLen = strlen(src);
	LENGTH count = 0;

	while (*src != splitChar)
	{
		++src;
		++count;
	}
	//Update forward EmpString
	memcpy(forwardStr, srcStr, count);
	//Update back EmpString
	memcpy(backStr, src + 1, srcLen - count - 1);
}

//Identify wether there is EmpString is same as special EmpString
EmpString::BOOL EmpString::FindString(STRING str)
{
	return false;
}

//Get EmpString length(except '\0' charactor)
EmpString::LENGTH EmpString::Length()
{
	if (GetString())
	{
		LENGTH len = strlen(GetString());
		return len;
	}
	return 0;
}

//The value of EmpString
EmpString::STRING EmpString::Value()
{
	if (GetString())
	{
		return GetString();
	}
	return NULL;
}










//Get the EmpString
EmpString::STRING EmpString::GetString() const
{
	return this->string;
}

//Set the EmpString
EmpString::NONE EmpString::SetString(STRING str)
{
	this->string = str;
}

//Get closed status
EmpString::BOOL EmpString::GetClosed() const
{
	return this->closed;
}

//Set closed status
EmpString::NONE EmpString::SetClosed(BOOL closed)
{
	this->closed = closed;
}









