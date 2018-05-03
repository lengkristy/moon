#include "../cfg/environment.h"
#include "moon_string.h"
#include <math.h>
#ifdef MS_WINDOWS
#include <windows.h>
#endif
#include "moon_memory_pool.h"

#ifdef __cplusplus
extern "C" {
#endif

unsigned long moon_get_string_length(const char* str)/*get string length,with out '\0'*/
{
	unsigned long length = 0;
	if (NULL == str)
		return length;
	while (str[length++] != '\0');
	return --length;
}

//Remove the beginning and end Spaces of the string.
void moon_trim(char *strIn, char *strOut)
{
	char *start, *end, *temp;//
	temp = strIn;
	while (*temp == ' '){
	        ++temp;
	}
	start = temp; //
	temp = strIn + moon_get_string_length(strIn) - 1; //
	while (*temp == ' '){
	    --temp;
	}
	end = temp; //
	for(strIn = start; strIn <= end; ){
	    *strOut++ = *strIn++;
	}
	*strOut = '\0';
}

//Get rid of the beginning and end new lines of the string.
void moon_trim_line(char *strIn, char *strOut)
{
	char *start, *end, *temp;
	temp = strIn;
	while (*temp == '\n'){
		++temp;
	}
	start = temp; 
	temp = strIn + moon_get_string_length(strIn) - 1; //
	while (*temp == '\n'){
		--temp;
	}
	end = temp; //
	for(strIn = start; strIn <= end; ){
		*strOut++ = *strIn++;
	}
	*strOut = '\0';
}

/**
 * function desc:
 *      Convert the string to unsigned int
 * return:
 *      success return true,failed return false
 */
bool moon_string_parse_to_int(const char* str,unsigned int* outInt)
{
	bool bFlag = true;
	int length = 0;
	int i = 0;
	if (str == NULL)
	{
		return false;
	}
	length = moon_get_string_length(str);//get string length
	for (i = 0;i < length;i++)
	{
		if (str[i] < 48 || str[i] > 57) //return false if it's not digital
		{
			return false;
		}
		if(48 == str[i]) //0
		{
			*outInt += 0 * pow(10,(length - i - 1));
		}
		if(49 == str[i]) //1
		{
			*outInt += 1 * pow(10,(length - i - 1));
		}
		if(50 == str[i]) //2
		{
			*outInt += 2 * pow(10,(length - i - 1));
		}
		if(51 == str[i]) //3
		{
			*outInt += 3 * pow(10,(length - i - 1));
		}
		if(52 == str[i]) //4
		{
			*outInt += 4 * pow(10,(length - i - 1));
		}
		if(53 == str[i]) //5
		{
			*outInt += 5 * pow(10,(length - i - 1));
		}
		if(54 == str[i]) //6
		{
			*outInt += 6 * pow(10,(length - i - 1));
		}
		if(55 == str[i]) //7
		{
			*outInt += 7 * pow(10,(length - i - 1));
		}
		if(56 == str[i]) //8
		{
			*outInt += 8 * pow(10,(length - i - 1));
		}
		if(57 == str[i]) //9
		{
			*outInt += 9 * pow(10,(length - i - 1));
		}
	}
	return bFlag;
}

bool stringIsEmpty(char* str)//判断字符串是否为NULL
{
	if(str == NULL)
		return true;
	if(str[0] == '\0')
		return true;
	else
		return false;
}
#ifdef MS_WINDOWS
/**
 * function desc:
 *      Turn the ASCII string to the utf8 string.
 * params:
 *      asciiStr:ASCII string
 *      outUTF8:the converted utf-8 string
 * return:
 *      success returns the number of bytes that are actually converted,failure returns -1
 */
int moon_ms_windows_ascii_to_utf8(const char* asciiStr,wchar_t* outUTF8)
{
	int convertSize = 0;
	wchar_t* unicodeStr = NULL;
	int widesize = MultiByteToWideChar(CP_ACP,0,asciiStr,-1,NULL,0);
	if (asciiStr == NULL)
	{
		return 0;
	}
	if (widesize == 0)
	{
		return 0;
	}
	unicodeStr = moon_malloc(widesize);
	//ascii to unicode
	convertSize = moon_ms_windows_ascii_to_unicode(asciiStr,unicodeStr);
	if (convertSize == -1)
	{
		return convertSize;
	}
	//unicode to utf8
	convertSize = moon_ms_windows_unicode_to_utf8(unicodeStr,outUTF8);
	moon_free(unicodeStr);
	return convertSize;
}/**
 * function desc:
 *      Converts string lowercase to uppercase
 * params:
 *      srcStr:The original string
 *      pOutCapital:The converted string.
 * return:
 *      Returns the number of actual converted letters.
 */

/**
 * function desc:
 *      turn the ascii string to the unicode string.
 * params:
 *      asciiStr:ASCII string
 *      outUnicode:the converted unicode string
 * return:
 *      success returns the number of bytes that are actually converted,failure returns -1
 */
int moon_ms_windows_ascii_to_unicode(const char* asciiStr,wchar_t* outUnicode)
{
	int widesize = MultiByteToWideChar(CP_ACP,0,asciiStr,-1,NULL,0);
	int convertlength = 0;
	if (widesize == ERROR_NO_UNICODE_TRANSLATION)
	{
		return -1;
	}
	if (widesize == 0)
	{
		return 0;/**
 * function desc:
 *      Converts string lowercase to uppercase
 * params:
 *      srcStr:The original string
 *      pOutCapital:The converted string.
 * return:
 *      Returns the number of actual converted letters.
 */
	}
	convertlength = MultiByteToWideChar(CP_ACP,0,asciiStr,-1,outUnicode,widesize);
	if (widesize != convertlength)
	{
		memset(outUnicode,0,widesize);
		return -1;
	}
	return convertlength;
}

/**
 * function desc:
 *      turn the unicode string to the utf8 string.
 * params:
 *      unicodeStr:unicode string
 *      outUTF8:the converted utf8 string
 * return:
 *      success returns the number of bytes that are actually converted,failure returns -1
 */
int moon_ms_windows_unicode_to_utf8(const wchar_t* unicodeStr,wchar_t* outUTF8)
{
	int utf8size = WideCharToMultiByte(CP_UTF8,0,unicodeStr,-1,NULL,0,NULL,NULL);
	int convertSize = 0;
	if (utf8size == 0)
	{
		return -1;
	}
	convertSize = WideCharToMultiByte(CP_UTF8,0,unicodeStr,-1,outUTF8,utf8size,NULL,NULL);
	if (convertSize != utf8size)
	{
		memset(outUTF8,0,utf8size);
		return -1;
	}
	return convertSize;
}

/**
 * function desc:
 *      turn the unicode string to ascii string.
 * params:
 *      unicodeStr:unicode string
 *      outAscii:the converted ascii string
 * return:
 *      success returns the number of bytes that are actually converted,failure returns -1
 */
int moon_ms_windows_unicode_to_ascii(const moon_char* unicodeStr,char* outAscii)
{
	int convertSize = 0;
	int asciiSize = WideCharToMultiByte(CP_ACP,0,unicodeStr,-1,NULL,0,NULL,NULL);
	if (asciiSize == 0)
	{
		return -1;
	}
	convertSize = WideCharToMultiByte(CP_ACP,0,unicodeStr,-1,outAscii,asciiSize,NULL,NULL);
	if (convertSize != asciiSize)
	{
		memset(outAscii,0,asciiSize);
		return -1;
	}
	return convertSize;
}

/**
 * function desc:
 *      turn the utf8 string to unicode string.
 * params:
 *      utf8Str:utf8 string
 *      outUnicode:the converted unicode string
 * return:
 *      success returns the number of bytes that are actually converted,failure returns -1
 */
int moon_ms_windows_utf8_to_unicode(const moon_char* utf8Str,moon_char* outUnicode)
{
	int convertSize = 0;
	int unicodeSize = MultiByteToWideChar(CP_UTF8,0,utf8Str,-1,NULL,0);
	if (unicodeSize == 0)
	{
		return -1;
	}
	convertSize = MultiByteToWideChar(CP_UTF8,0,utf8Str,-1,outUnicode,unicodeSize);
	if (convertSize != unicodeSize)
	{
		memset(outUnicode,0,unicodeSize);
		return -1;
	}
	return convertSize;
}

/**
 * function desc:
 *      turn utf8 string to ascii string
 * params:
 *      utf8Str:utf8 string
 *      outAscii:the converted ascii string
 * return:
 *      success returns the number of bytes that are actually converted,failure returns -1
 */
int moon_ms_windows_utf8_to_ascii(const moon_char* utf8Str,char* outAscii)
{
	int convertSize = 0;
	wchar_t* unicodeStr = NULL;
	int unicodeSize = MultiByteToWideChar(CP_UTF8,0,utf8Str,-1,NULL,0);
	if (utf8Str == NULL)
	{
		return 0;
	}
	if (unicodeSize == 0)
	{
		return 0;
	}
	unicodeStr = moon_malloc(unicodeSize);
	convertSize = moon_ms_windows_utf8_to_unicode(utf8Str,unicodeStr);
	if (convertSize == -1)
	{
		return convertSize;
	}
	convertSize = moon_ms_windows_unicode_to_ascii(unicodeStr,outAscii);
	moon_free(unicodeStr);
	return convertSize;
}

#endif

/**
 * function desc:
 *      Converts string lowercase to uppercase
 * params:
 *      srcStr:The original string
 *      pOutCapital:The converted string.
 * return:
 *      Returns the number of actual converted letters.
 */
int moon_to_capital(char* srcStr,char* pOutCapital)
{
	int count = 0;
	int i = 0;
	int length = moon_get_string_length(srcStr);
	for (i = 0;i < length;i++)
	{
		if (srcStr[i] >= 'a' && srcStr[i] <= 'z')
		{
			pOutCapital[i] = srcStr[i] - 32;
			count++;
		}
		else
		{
			pOutCapital[i] = srcStr[i];
		}
	}
	return count;
}

#ifdef __cplusplus
}
#endif