/****************************************************************
* COPYRIGHT(EMP),emperor electronic technology of ShenZhen
* FILENAME : EmpString.h
* AUTHOR : TangYingZhong
* VERSION : v1.0.0
* DATE : 2017/10/16
* DESCRIPTION : String's operation such as append ,find,get length
****************************************************************/
#ifndef		SYSTEMMODEL_DAL_EMPSTRING_H_
#define	SYSTEMMODEL_DAL_EMPSTRING_H_

#include  "IString.h"


namespace SystemModel
{
	namespace DAL
	{
		/*
		* CLASSNAME : EmpString
		* FUNC : SQLDB
		* AUTHOR : Tangyz
		* DATE : 2017/10/16
		*/
		class EmpString :public IString
		{
		public:
			typedef int SIZE;
			typedef char* PSTRING;

			typedef enum
			{
				NORMAL=0,
				MEM_OVERFLOW,
				EMPTY,
			}STATUS;

		public:
			//Construct the String
			EmpString();

			//Destruct the String
			~EmpString();

			//Copy construct String
			EmpString(EmpString& other);

			//Assignment
			EmpString& operator=(EmpString& other);

		public:
			//Append string
			virtual ISTRING* Append(STRING str);

			//Get sub string
			virtual NONE SubString(STRING srcStr, CHARACTOR splitChar, STRING forwardStr, STRING backStr);

			//Identify wether there is string is same as special string
			virtual BOOL FindString(STRING str);

			//Get string length(except '\0' charactor)
			virtual LENGTH Length();

			//The value of string
			virtual STRING Value();

		private:
			//Create the string
			NONE Create();

			//Destory the string
			NONE Destory();

			//Malloc string
			NONE MallocStr();

			//Destroy the string
			NONE DestoryStr();

		private:
			//Malloc the string space
			STRING Malloc(SIZE size);

			//Free the space
			NONE Free(STRING pStr);

			//Resize the space
			STRING ReSize(SIZE size);

			//Back up the string
			NONE BackUp(STRING* current);

			//Copy the string 
			NONE Copy(STRING dest,LENGTH destLen, STRING src,LENGTH srcLen);

			//Check buffer size
			STATUS EnoughBuffer(STRING newStr);

			//Update the string pointer
			PSTRING UpdateBufferPointer();

			//Zero the memory
			NONE InitMemory(STRING pStr,LENGTH len);

			//Directly append
			NONE DirectAppend(PSTRING srcStr);

			//Resize and append
			NONE ReSizeAndAppend(PSTRING srcStr);

		private:
			//Get the string
			STRING GetString() const;

			//Set the string
			NONE SetString(STRING str);

			//Get closed status
			BOOL GetClosed() const;

			//Set closed status
			NONE SetClosed(BOOL closed);

		private:
			//Size of primary
			static const LENGTH STRSIZE = 100;

			//Char * string
			STRING string;

			//Closed
			BOOL closed;
		};
	}
}





#endif //SYSTEMMODEL_DAL_STRING_H_


