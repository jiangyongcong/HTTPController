#pragma once
#include <string>

namespace UTF
{
	//void ConvertGBKToUtf8(CString& strGBK) {
	//	int len=MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)strGBK, -1, NULL,0);
	//	unsigned short * wszUtf8 = new unsigned short[len+1];
	//	memset(wszUtf8, 0, len * 2 + 2);
	//	MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)strGBK, -1, wszUtf8, len);

	//	len = WideCharToMultiByte(CP_UTF8, 0, wszUtf8, -1, NULL, 0, NULL, NULL); 
	//	char *szUtf8=new char[len + 1];
	//	memset(szUtf8, 0, len + 1);
	//	WideCharToMultiByte (CP_UTF8, 0, wszUtf8, -1, szUtf8, len, NULL,NULL);

	//	strGBK = szUtf8;
	//	delete[] szUtf8;
	//	delete[] wszUtf8;
	//}

	//void ConvertUtf8ToGBK(CString& strUtf8) {
	//	int len=MultiByteToWideChar(CP_UTF8, 0, (LPCTSTR)strUtf8, -1, NULL,0);
	//	unsigned short * wszGBK = new unsigned short[len+1];
	//	memset(wszGBK, 0, len * 2 + 2);
	//	MultiByteToWideChar(CP_UTF8, 0, (LPCTSTR)strUtf8, -1, wszGBK, len);

	//	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL); 
	//	char *szGBK=new char[len + 1];
	//	memset(szGBK, 0, len + 1);
	//	WideCharToMultiByte (CP_ACP, 0, wszGBK, -1, szGBK, len, NULL,NULL);

	//	strUtf8 = szGBK;
	//	delete[] szGBK;
	//	delete[] wszGBK;
	//}

	class Convert
	{
	public:
		static std::string GBToUTF8(const char* str)
		{
			std::string result;
			WCHAR *strSrc;
			TCHAR *szRes;

			//获得临时变量的大小
			int i = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
			strSrc = new WCHAR[i+1];
			MultiByteToWideChar(CP_ACP, 0, str, -1, strSrc, i);

			//获得临时变量的大小
			i = WideCharToMultiByte(CP_UTF8, 0, strSrc, -1, NULL, 0, NULL, NULL);
			szRes = new TCHAR[i+1];
			int j=WideCharToMultiByte(CP_UTF8, 0, strSrc, -1, (LPSTR) szRes, i, NULL, NULL);

			result = szRes;
			delete []strSrc;
			delete []szRes;

			return result;
		}

		static std::string UTF8ToGB(const char* str)
		{
			std::string result;
			WCHAR *strSrc;
			TCHAR *szRes;

			//获得临时变量的大小
			int i = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
			strSrc = new WCHAR[i+1];
			MultiByteToWideChar(CP_UTF8, 0, str, -1, strSrc, i);

			//获得临时变量的大小
			i = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);
			szRes = new TCHAR[i+1];
			WideCharToMultiByte(CP_ACP, 0, strSrc, -1,  (LPSTR) szRes, i, NULL, NULL);

			result = szRes;
			delete []strSrc;
			delete []szRes;

			return result;
		}	

		static std::string ChineseToUrlCode(const char* str)
		{
			std::string result;

			std::string cstr = GBToUTF8( str );
			char *pBuff = new char[cstr.length()*4+10];
			LPCSTR pStr = cstr.c_str();
			int iPos = 0;
			for ( int i=0;i<(int)cstr.length();i++ )
			{
				pBuff[iPos++] = '%';
				::sprintf( pBuff+iPos,"%02x",(unsigned char)pStr[i] );
				iPos += 2;
			}

			pBuff[iPos++] = 0;
			result = pBuff;

			delete []pBuff;

			return result;
		}
	protected:
	private:
	};


}
using namespace UTF;

