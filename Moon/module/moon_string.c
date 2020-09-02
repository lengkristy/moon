#include "../cfg/environment.h"
#include "moon_string.h"
#include <math.h>

#include "moon_memory_pool.h"


#ifdef __cplusplus
extern "C" {
#endif

/*get string length,with out '\0'*/
unsigned long moon_get_string_length(const char* str)
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

/**
 * 函数说明：
 *     将int32转成4字节的字符串，只能转4位数及其以下的低于四位数，高位用0补齐
 * 参数：
 *     num：32位数字
 *     outCh：输出转换的字符
 */
void moon_int32_to_4byte(const int num,char outCh[5])
{
	if(num > 9999) return;
	if(num > 999)
		sprintf(outCh,"%d",num);
	else if (num > 99)
		sprintf(outCh,"0%d",num);
	else if (num > 9)
		sprintf(outCh,"00%d",num);
	else if (num > 0)
		sprintf(outCh,"000%d",num);
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

/**
 * function desc:
 *      Determines whether the string is NULL.
 * params:
 *      string
 * return:
 *      null return true,not null return false
 */
bool moon_string_is_empty(moon_char* str)
{
	if(str == NULL)
	{
		return true;
	}
	if(str[0] == '\0')
	{
		return true;
	}
	else
	{
		return false;
	}
}

/**
 * 函数说明：
 *   查询字符串，查询findStr在sourceStr中第一次出现的位置
 * 参数：
 *   sourceStr:被查询的源字符串
 *   findStr：被查询的字符串
 * 返回值：
 *   如果查询到，则返回第一次出现的位置，如果没有查询到则返回-1
 */
int index_of(const char* sourceStr,const char* findStr)
{
	char *p=sourceStr;  
	int i=0;  
	p=strstr(sourceStr,findStr);  
	if(p==NULL)  
		return -1;  
	else
	{  
		while(sourceStr!=p)  
		{  
			sourceStr++;  
			i++;  
		}  
	}  
	return i;
}

/**
 * 函数说明：
 *   查询字符串，查询findStr在sourceStr中第一次出现的位置
 * 参数：
 *   sourceStr:被查询的源字符串
 *   findStr：被查询的字符串
 * 返回值：
 *   如果查询到，则返回第一次出现的位置，如果没有查询到则返回-1
 */
int moon_char_index_of(const moon_char* sourceStr,const moon_char* findStr)
{
	return index_of((char*)sourceStr,(char*)findStr);
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
int moon_ms_windows_ascii_to_utf8(const char* asciiStr,char* outUTF8)
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
	unicodeStr = moon_malloc(widesize * sizeof(wchar_t));
	//ascii to unicode
	convertSize = moon_ms_windows_ascii_to_unicode(asciiStr,unicodeStr);
	if (convertSize == -1)
	{
		return convertSize;
	}
	//unicode to utf8
	convertSize = moon_ms_windows_unicode_to_utf8(unicodeStr,outUTF8);
	if (unicodeStr != NULL)
	{
		moon_free(unicodeStr);
	}
	return convertSize;
}

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
	int widesize = 0;
	int convertlength = 0;
	if(asciiStr == NULL)
	{
		return -1;
	}
	if(outUnicode == NULL)
	{
		return -1;
	}
	widesize = MultiByteToWideChar(CP_ACP,0,asciiStr,-1,NULL,0);
	if (widesize == ERROR_NO_UNICODE_TRANSLATION)
	{
		return -1;
	}
	if (widesize == 0)
	{
		return 0;
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
 *      moonchar:unicode string
 *      outUTF8:the converted utf8 string
 * return:
 *      success returns the number of bytes that are actually converted,failure returns -1
 */
int moon_ms_windows_unicode_to_utf8(const wchar_t* moonchar,char* outUTF8)
{
	int utf8size = WideCharToMultiByte(CP_UTF8,0,moonchar,-1,NULL,0,NULL,NULL);
	int convertSize = 0;
	if (utf8size == 0)
	{
		return -1;
	}
	convertSize = WideCharToMultiByte(CP_UTF8,0,moonchar,-1,outUTF8,utf8size,NULL,NULL);
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
int moon_ms_windows_unicode_to_ascii(const wchar_t* unicodeStr,char* outAscii)
{
	int convertSize = 0;
	int asciiSize = WideCharToMultiByte(CP_OEMCP,0,unicodeStr,-1,NULL,0,NULL,NULL);
	if (asciiSize == 0)
	{
		return -1;
	}
	convertSize = WideCharToMultiByte(CP_OEMCP,0,unicodeStr,-1,outAscii,asciiSize,NULL,NULL);
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
int moon_ms_windows_utf8_to_unicode(const char* utf8Str,wchar_t* outUnicode)
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
int moon_ms_windows_utf8_to_ascii(const char* utf8Str,char* outAscii)
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

	unicodeStr = (wchar_t*)moon_malloc(3 * unicodeSize * sizeof(wchar_t));
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

/**
 * function desc:
 *      get moon_char length
 * params:
 *      str:the moon_char string
 * return:
 *      return length
 */
int moon_char_length(const moon_char* str)
{
	int len = 0;
	len = strlen((char*)str);
	return len;
}

/**
 * 函数说明：
 *   获取字符串占用内存的大小
 * 参数：
 *   str：moon_char
 * 返回值：
 *   返回字符串占用内存大小
 */
int moon_char_memory_size(const moon_char* str)
{
	char* p = (char*) str;
	while (*p++);
	return p - str;
}

/**
 * function desc:
 *	create 32bit uuid
 * params:
 *	p_out_uuid:return uuid
 */
void moon_create_32uuid(_out_ moon_char* p_out_uuid)
{
	const char *c = "89ab";
	char buf[40] = {0};
	char *p = buf;
	int n;  

	for( n = 0; n < 16; ++n )  
	{  
		int b = rand()%255;  

		switch( n )  
		{  
		case 6:  
			sprintf(  
				p,  
				"4%x",  
				b%15 );  
			break;  
		case 8:  
			sprintf(  
				p,  
				"%c%x",  
				c[rand()%strlen( c )],  
				b%15 );  
			break;  
		default:  
			sprintf(  
				p,  
				"%02x",  
				b );  
			break;  
		}  

		p += 2;
	}  
	*p = 0;

	char_to_moon_char(buf,p_out_uuid);
}

#ifdef __cplusplus
}
#endif