//这是个类CStringCoder (StringCoder.h文件)
#pragma once

#include <string>
#include <windows.h>
using namespace std;

class CStringCoder
{
public:
	static void UTF_8ToGB2312(string &str)
	{//utf_8转为gb2312
		int iLen = (int)str.length();
		char buf[4];
		const char *pText = str.c_str(); 
		char* rst = new char[iLen + (iLen >> 2) + 2];
		memset(buf,0,4);
		memset(rst,0,iLen + (iLen >> 2) + 2);

		int i =0;
		int j = 0;

		while(i < iLen)
		{
			if(*(pText + i) >= 0)
			{

				rst[j++] = pText[i++];
			}
			else                 
			{
				WCHAR Wtemp;

				UTF_8ToUnicode(&Wtemp,pText + i);

				UnicodeToGB2312(buf,Wtemp);

				unsigned short int tmp = 0;
				tmp = rst[j] = buf[0];
				tmp = rst[j+1] = buf[1];
				tmp = rst[j+2] = buf[2];

				i += 3;    
				j += 2;   
			}

		}
		rst[j]='\0';
		str = rst; 
		delete []rst;
	}

	static void GB2312ToUTF_8(string &str)
	{//gb2312 转utf_8
		int iLen = (int)str.length();
		const char *pText = str.c_str();
		char buf[4];
		string strOut;
		memset(buf,0,4);

		strOut.clear();

		int i = 0;
		while(i < iLen)
		{
			//如果是英文直接复制就可以
			if( pText[i] >= 0)
			{
				char asciistr[2]={0};
				asciistr[0] = (pText[i++]);
				strOut.append(asciistr);
			}
			else
			{
				WCHAR pbuffer;
				Gb2312ToUnicode(&pbuffer,pText+i);

				UnicodeToUTF_8(buf,&pbuffer);

				strOut.append(buf);

				i += 2;
			}
		}

		str = strOut;

		return;
	}

	// 把str编码为网页中的 GB2312 url encode ,英文不变，汉字双字节  如%3D%AE%88
	static void UrlGB2312(string &str)
	{//urlgb2312编码
		string dd;
		size_t len = str.length();
		for (size_t i=0;i<len;i++)
		{
			if(isalnum((BYTE)str[i]))
			{
				char tempbuff[2];
				sprintf_s(tempbuff,sizeof(tempbuff),"%c",str[i]);
				dd.append(tempbuff);
			}
			else if (isspace((BYTE)str[i]))
			{
				dd.append("+");
			}
			else
			{
				char tempbuff[4];
				sprintf_s(tempbuff,sizeof(tempbuff),"%%%X%X",((BYTE)str[i]) >>4,((BYTE)str[i] ) %16);
				dd.append(tempbuff);
			}

		}
		str = dd;
	}

	// 把str编码为网页中的 UTF-8 url encode ,英文不变，汉字三字节  如%3D%AE%88
	static void UrlUTF8(string &str)
	{//urlutf8 编码
		string tt;
		string dd;

		tt = str;
		GB2312ToUTF_8(tt);

		size_t len=tt.length();
		for (size_t i=0;i<len;i++)
		{
			if(isalnum((BYTE)tt.at(i)))
			{
				char tempbuff[2]={0};
				sprintf_s(tempbuff,sizeof(tempbuff),"%c",(BYTE)tt.at(i));
				dd.append(tempbuff);
			}
			else if (isspace((BYTE)tt.at(i)))
			{
				dd.append("+");
			}
			else
			{
				char tempbuff[4];
				sprintf_s(tempbuff,sizeof(tempbuff),"%%%X%X",((BYTE)tt.at(i)) >>4,((BYTE)tt.at(i)) %16);
				dd.append(tempbuff);
			}

		}
		str = dd;
	}

	static void UrlUTF8Decode(string &str)
	{//urlutf8解码

		UrlGB2312Decode(str);

		UTF_8ToGB2312(str);
	}

	// 把url GB2312解码
	static void UrlGB2312Decode(string &str)
	{//urlgb2312解码
		string output="";
		char tmp[2];
		int i=0,idx=0,len=(int)str.length();

		while(i<len){
			if(str[i]=='%'){
				tmp[0]=str[i+1];
				tmp[1]=str[i+2];
				output += StrToBin(tmp);
				i=i+3;
			}
			else if(str[i]=='+'){
				output+=' ';
				i++;
			}
			else{
				output+=str[i];
				i++;
			}
		}

		str = output;
	}

	static void Gb2312ToUnicode(WCHAR* pOut,const char *gbBuffer)
	{
		::MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,gbBuffer,2,pOut,1);
		return;
	}

	static void UTF_8ToUnicode(WCHAR* pOut,const char *pText)
	{
		char* uchar = (char *)pOut;

		uchar[1] = ((pText[0] & 0x0F) << 4) + ((pText[1] >> 2) & 0x0F);
		uchar[0] = ((pText[1] & 0x03) << 6) + (pText[2] & 0x3F);

		return;
	}

	static void UnicodeToUTF_8(char* pOut,WCHAR* pText)
	{
		// 注意 WCHAR高低字的顺序,低字节在前，高字节在后
		char* pchar = (char *)pText;

		pOut[0] = (0xE0 | ((pchar[1] & 0xF0) >> 4));
		pOut[1] = (0x80 | ((pchar[1] & 0x0F) << 2)) + ((pchar[0] & 0xC0) >> 6);
		pOut[2] = (0x80 | (pchar[0] & 0x3F));

		return;
	}

	static void UnicodeToGB2312(char* pOut,WCHAR uData)
	{
		WideCharToMultiByte(CP_ACP,NULL,&uData,1,pOut,sizeof(WCHAR),NULL,NULL);
		return;
	}

	static char  CharToInt(char ch)
	{
		if(ch>='0' && ch<='9')return (char)(ch-'0');
		if(ch>='a' && ch<='f')return (char)(ch-'a'+10);
		if(ch>='A' && ch<='F')return (char)(ch-'A'+10);
		return -1;
	}

	static char StrToBin(char *str)
	{
		char tempWord[2];
		char chn;

		tempWord[0] = CharToInt(str[0]);                         //make the B to 11 -- 00001011
		tempWord[1] = CharToInt(str[1]);                         //make the 0 to 0  -- 00000000

		chn = (tempWord[0] << 4) | tempWord[1];                //to change the BO to 10110000

		return chn;
	}

};