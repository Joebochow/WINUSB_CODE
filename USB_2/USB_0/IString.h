/****************************************************************
* COPYRIGHT(EMP),emperor electronic technology of ShenZhen
* FILENAME : IString.h
* AUTHOR : TangYingZhong
* VERSION : v1.0.0
* DATE : 2017/10/16
* DESCRIPTION : String interface
****************************************************************/
#ifndef		SYSTEMMODEL_DAL_ISTRING_H_
#define	SYSTEMMODEL_DAL_ISTRING_H_

#include <Windows.h>

namespace SystemModel
{
	namespace DAL
	{
		/*
		* CLASSNAME : IString
		* FUNC : Usb's interface
		* AUTHOR : Tangyz
		* DATE : 2017/10/16
		*/
		class IString
		{
		public:
			typedef void NONE;
			typedef bool BOOL;
			typedef char* STRING;
			typedef int	INDEX;
			typedef IString ISTRING;
			typedef int LENGTH;
			typedef char CHARACTOR;
			typedef char* PSTRING;

		public:
			virtual ~IString(){		}

		public:
			//Append string
			virtual ISTRING* Append(STRING str) = 0;

			//Get sub string
			virtual NONE SubString(STRING srcStr, CHARACTOR splitChar, STRING forwardStr, STRING backStr) = 0;

			//Identify wether there is string is same as special string
			virtual BOOL FindString(STRING str)=0;

			//Get string length(except '\0' charactor)
			virtual LENGTH Length()=0;

			//The value of string
			virtual STRING Value()=0;

		};
	}
}

#endif //SYSTEMMODEL_DAL_ISTRING_H_


