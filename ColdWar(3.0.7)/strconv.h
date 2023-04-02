
// harkon.kr@지메일닷컴, 07/4/8, vc 8

//
// 원95에서는 unicode to utf8 동작안함
// - 한글 윈도우 95에서 유니코드 문자열을 UTF8 문자열로 변환해 주는 다음 함수를 실행하면, 리턴되는 UTF8 버퍼에 아무런 값도 리턴 되지 않습니다.
// RFC2044에 명시된 스펙에 의거하여 문자열 변환하는 방법을 소개합니다.
// http://support.microsoft.com/kb/601368/ko
//

//#pragma once
#ifndef STRCONV_H
#define STRCONV_H

#include <string>
#include <mbctype.h> // _ismbslead
#include <atlcore.h> // unicode W2A, A2W macro

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define CONV_UTF8_MBCS(a)	(convUni2Mbcs(convUtf82Uni(a)).c_str())
#define CONV_MBCS_UTF8(a)	(convUni2Utf8(convMbcs2Uni(a)).c_str())
#define CONV_UNI_UTF8(a)	convUni2Utf8(a).c_str()


// ATL::_AtlGetConversionACP() to CP_ACP
#undef USES_CONVERSION
#ifndef _DEBUG
    #define USES_CONVERSION int _convert; (_convert); UINT _acp = CP_ACP; (_acp); LPCWSTR _lpw; (_lpw); LPCSTR _lpa; (_lpa)
#else
    #define USES_CONVERSION int _convert = 0; (_convert); UINT _acp = CP_ACP; (_acp); LPCWSTR _lpw = NULL; (_lpw); LPCSTR _lpa = NULL; (_lpa)
#endif

// utf8 convertion macro
#ifndef _DEBUG
    #define USES_UTF8_CONVERSION int _convert; (_convert); UINT _acp = CP_UTF8; (_acp); LPCWSTR _lpw; (_lpw); LPCSTR _lpa; (_lpa)
#else
    #define USES_UTF8_CONVERSION int _convert = 0; (_convert); UINT _acp = CP_UTF8; (_acp); LPCWSTR _lpw = NULL; (_lpw); LPCSTR _lpa = NULL; (_lpa)
#endif

#undef A2W
#define A2W(lpa) (\
    ((_lpa = lpa) == NULL) ? NULL : (\
        _convert = (lstrlenA(_lpa)+1),\
        ATLA2WHELPER((LPWSTR) alloca(_convert*2), _lpa, _convert, _acp)))


#undef W2A      // utf8 을 처리하기위해, (lstrlenW(_lpw)+1)*2 -> (lstrlenW(_lpw)+1)*3 으로 변경
#define W2A(lpw) (\
    ((_lpw = lpw) == NULL) ? NULL : (\
        _convert = (lstrlenW(_lpw)+1)*3,\
        ATLW2AHELPER((LPSTR) alloca(_convert), _lpw, _convert, _acp)))



inline
std::wstring convUtf82Uni(const std::string &ustr)
{
    USES_UTF8_CONVERSION;
    std::wstring wstr(A2W(ustr.c_str()));
    return wstr;
}

inline
std::string convUni2Utf8(const std::wstring &wstr)
{
    USES_UTF8_CONVERSION;
    std::string ustr(W2A(wstr.c_str()));
    return ustr;
}

inline
std::wstring convMbcs2Uni(const std::string &mstr)
{
    USES_CONVERSION;
    std::wstring wstr(A2W(mstr.c_str()));
    return wstr;
}

inline
std::string convUni2Mbcs(const std::wstring &wstr)
{
    USES_CONVERSION;
    std::string str(W2A(wstr.c_str()));
    return str;
}

inline
std::string convMbcs2Utf8(const std::string &mstr)
{
    return convUni2Utf8(convMbcs2Uni(mstr));
}

inline
std::string convUtf82Mbcs(const std::string &ustr)
{
    return convUni2Mbcs(convUtf82Uni(ustr));
}


inline char* ANSI_TO_UTF8( char *szANSI )
{
	int nLen = MultiByteToWideChar( CP_ACP, 0, szANSI, -1, NULL, NULL );
	LPWSTR lpwsz = new WCHAR[nLen];
	MultiByteToWideChar( CP_ACP, 0, szANSI, -1, lpwsz, nLen );
	int nLen1 = WideCharToMultiByte( CP_UTF8, 0, lpwsz, nLen, NULL, NULL, NULL, NULL );
	LPSTR lpsz = new CHAR[nLen1];
	WideCharToMultiByte(CP_UTF8, 0, lpwsz, nLen, lpsz, nLen1, NULL, NULL );
	delete []lpwsz;
	return lpsz;
}
	



/*
31..A7      33..167         Ogre::Font                  134

1100..11FF  4352..4607        Hangul Jamo                  255   한글 자모
3130..318F  12592..12687    Hangul Compatibility Jamo      95    한글 호환 자모
AC00..D7A3  44032..55203    Hangul Syllables              11171 한글

UCS-4                    UTF-8
0x00000000 - 0x0000007F    0xxxxxxx                        00..7F
0x00000080 - 0x000007FF    110xxxxx 10xxxxxx               C0..DF 80..DF
0x00000800 - 0x0000FFFF    1110xxxx 10xxxxxx 10xxxxxx      E0..EF

*/
inline
int lenUtf8Char(char c)
{
    unsigned char b = (unsigned char)c;
    if (b <= 0x7F) return 1;
    else if (b <= 0xDF) return 2;
    else if (b <= 0xEF) return 3;

    // over BMP (ucs-4?)
    return 1;
}

inline
int lenUniChar2Utf8(wchar_t c)
{
    if (c <= 0x7F) return 1;
    else if (c <= 0x7FF) return 2;
    
    return 3;
}

inline 
int lenMbcsChar(const char *cur, const char *base)
{
    return _ismbslead((unsigned char *)base, (unsigned char *)cur) == -1 ? 2 : 1;
}

#endif

 
  
