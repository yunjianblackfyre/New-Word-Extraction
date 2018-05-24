/*   
     AUTHOR: Sibyl System
     DATE: 2018-03-30
     DESC: content preprocessing
*/

#include "content_preproc.h"

int Utf82Unicode(char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
    return code_convert((char*)"UTF-8",(char*)"UNICODE",inbuf,inlen,outbuf,outlen);
}

int Unicode2Utf8(char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
    return code_convert((char*)"UNICODE",(char*)"UTF-8",inbuf,inlen,outbuf,outlen);
}

int UnicodeToWUnicode(const char* inbuf, size_t inlen, char *outbuf, size_t outlen)
{
    if (inbuf==NULL||outbuf==NULL||inbuf==outbuf||\
        2*inlen!=outlen||inlen%2!=0||inlen==0)
    {
        cout << "base conditions" << endl;
        return -1;      // 长度不为零并且必须为偶数
    }
    else if (outbuf > inbuf && inbuf + inlen > outbuf)
    {
        cout << (size_t)(outbuf-inbuf) << endl;
        cout << "memory conflict, outbuf is higher" << endl;
        return -1;      // 内存块不能重合
    }
    else if (inbuf > outbuf && outbuf + outlen > inbuf)
    {
        cout << (size_t)(inbuf-outbuf) << endl;
        cout << "memory conflict, inbuf is higher" << endl;
        return -1;      // 内存块不能重合
    }

    for (int i=0,j=0; i<inlen-1; i+=2,j+=4)
    {
        outbuf[j] = inbuf[i];        // 低一位
        outbuf[j+1] = inbuf[i+1];    // 低二位
        outbuf[j+2] = 0;             // 高一位
        outbuf[j+3] = 0;             // 高二位
    }
    return 0;
}

int WUnicodeToUnicode(const char* inbuf, size_t inlen, char *outbuf, size_t outlen)
{
    if (inbuf==NULL||outbuf==NULL||inbuf==outbuf||\
        2*outlen!=inlen||outlen%2!=0||outlen==0)
    {
        cout << "base conditions" << endl;
        return -1;
    }
    else if (outbuf > inbuf && inbuf + inlen > outbuf)
    {
        cout << (size_t)(outbuf-inbuf) << endl;
        cout << "memory conflict, outbuf is higher" << endl;
        return -1;      // 内存块不能重合
    }
    else if (inbuf > outbuf && outbuf + outlen > inbuf)
    {
        cout << (size_t)(inbuf-outbuf) << endl;
        cout << "memory conflict, inbuf is higher" << endl;
        return -1;      // 内存块不能重合
    }

    for (int i=0,j=0; i<outlen-1; i+=2,j+=4)
    {
        if (i==0)
        {
            outbuf[i]   = 0x20;
            outbuf[i+1] = 0x00;
        }
        else
        {
            outbuf[i] = inbuf[j];
            outbuf[i+1] = inbuf[j+1];
        }
    }
    return 0;
}

int wStrBatchReplace(wchar_t* pContent, const wchar_t* pReplace, map<wstring,wstring>& strBatchMap)
{
    // 参数检测
    bool isEmpty = strBatchMap.empty();
    size_t repStrLen = wcslen(pReplace);
    if (isEmpty or repStrLen<=0 or repStrLen > MAX_REPSTR_LEN)
    {
        return -1;      // 替换字符串长度在1~5之间
    }
    
    // 初始化变量
    wchar_t* pCnt = pContent;
    const wchar_t* pTempRplc = NULL;
    map<wstring, wstring>::iterator  it_finder;
    
    while (*pCnt!=0)
    {
        pTempRplc = pReplace;
        wchar_t pCntPart[MAX_REPSTR_LEN]={0};
        wcsncpy(pCntPart,pCnt,repStrLen);
        
        it_finder = strBatchMap.find(wstring(pCntPart));
        if (it_finder != strBatchMap.end())
        {
            while ((*pCnt!=0)&&(*pTempRplc!=0))
            {
                (*pCnt) = (*pTempRplc);
                pCnt = pCnt + 1;
                pTempRplc = pTempRplc + 1;
            }
        }
        else
        {
            pCnt = pCnt + 1;
        }
    }
    return 0;
}

void wStrFastReplace(wchar_t *pContent, const wchar_t* pTarget, const wchar_t* pReplace)
{
    // 参数校验
    if (pContent == NULL || pTarget == NULL || pReplace == NULL)
    {
        return;
    }
    // 初始化变量
    wchar_t* pCnt = pContent;
    wchar_t* pTempCnt = NULL;
    const wchar_t* pTempTgt = NULL;
    const wchar_t* pTempRplc = NULL;
    bool equality = false;
    
    while (*pCnt!=0)
    {
        pTempCnt  = pCnt;
        pTempTgt  = pTarget;
        pTempRplc = pReplace;
        equality = true;
        
        // 查询字符串
        while (*pTempTgt!=0)
        {
            if (*pTempCnt==0)
            {
                equality = false;
                break;
            }
            else if (*pTempCnt!=*pTempTgt)
            {
                equality = false;
                break;
            }
            else
            {
                pTempCnt = pTempCnt + 1;
                pTempTgt = pTempTgt + 1;
            }
        }
        // 替换字符串
        if (equality)
        {
            while ((*pCnt!=0)&&(*pTempRplc!=0))
            {
                (*pCnt) = (*pTempRplc);
                pCnt = pCnt + 1;
                pTempRplc = pTempRplc + 1;
                
            }
        }
        else
        {
            pCnt = pCnt + 1;
        }
    }
}

CContentPreproc::CContentPreproc()
{
    m_utf8_chararray =              new char[UTF8_ARRAY_MAX_LEN]();
    m_unicode_chararray =           new char[UNICODE_ARRAY_MAX_LEN]();
    m_unicode_exchararray =         new char[WUNICODE_ARRAY_MAX_LEN]();
    m_utf8_chararray_clean =        new char[UTF8_ARRAY_MAX_LEN]();
    m_unicode_chararray_clean =     new char[UNICODE_ARRAY_MAX_LEN]();
    m_unicode_exchararray_clean =   new char[WUNICODE_ARRAY_MAX_LEN]();
}

CContentPreproc::~CContentPreproc()
{
    this->clean();
}

void CContentPreproc::clean()
{
    delete [] m_utf8_chararray;
    delete [] m_unicode_chararray;
    delete [] m_unicode_exchararray;
    delete [] m_utf8_chararray_clean;
    delete [] m_unicode_chararray_clean;
    delete [] m_unicode_exchararray_clean;
}

void CContentPreproc::reset()
{
    m_wcontent = L"";
    strncpy(m_utf8_chararray,m_utf8_chararray_clean,UTF8_ARRAY_MAX_LEN);
    strncpy(m_unicode_chararray,m_unicode_chararray_clean,UNICODE_ARRAY_MAX_LEN);
    strncpy(m_unicode_exchararray,m_unicode_exchararray_clean,WUNICODE_ARRAY_MAX_LEN);
}

int CContentPreproc::getResultLen()
{
    int resultLen = int(UNICODE_ARRAY_MAX_LEN);
    return resultLen;
}

void CContentPreproc::transCode(string &content)
{
    // 参数校验
    if (content.length() > CONTENT_MAX_LEN)
    {
        this->reset();
        throw CException(ERR_CONTENT_LENGTH,"content too long");
    }
    else if (content.length() == 0)
    {
        this->reset();
        throw CException(ERR_CONTENT_EMPTY,"content empty");
    }
    
    // 初始化变量
    int ret_transcoding = -1;
    
    // 清理成员变量
    this->reset();
    
    // 拷贝本次的源编码数据
    strcpy(m_utf8_chararray,content.c_str());
    
    // 字符串转码：utf8->unicode
    ret_transcoding = Utf82Unicode(m_utf8_chararray, UTF8_ARRAY_MAX_LEN, \
                                   m_unicode_chararray, UNICODE_ARRAY_MAX_LEN);
    if (ret_transcoding!=0)
    {
        this->reset();
        throw CException(ERR_TRANSCODING,"utf8 to unicode transcoding failed");
    }
    
    // 字符串转码：unicode->wunicode
    ret_transcoding = UnicodeToWUnicode(m_unicode_chararray,UNICODE_ARRAY_MAX_LEN, \
                                        m_unicode_exchararray,WUNICODE_ARRAY_MAX_LEN);
    if (ret_transcoding!=0)
    {
        this->reset();
        throw CException(ERR_TRANSCODING,"unicode to wunicode transcoding failed");
    }
    
}

bool CContentPreproc::contentFilter()
{
    // 初始化变量
    static double eng_ratio =   0.0;
    double eng_wordcount =      0.0;
    double chi_wordcount =      0.0;
    
    // 汉字计数
    boost::wregex chi_pattern(CHI_WORD);
    boost::wsmatch what_chi;
    std::wstring::const_iterator start =    m_wcontent.begin();
    std::wstring::const_iterator end =      m_wcontent.end();
    while ( boost::regex_search(start, end, what_chi, chi_pattern) )
    {  
        chi_wordcount+=1.0;
        start = what_chi[0].second;
    }
    
    // 英语单词计数
    boost::wregex eng_pattern(ENG_WORD);
    boost::wsmatch what_eng;
    start =     m_wcontent.begin();
    end =       m_wcontent.end();
    while ( boost::regex_search(start, end, what_eng, eng_pattern) )
    {  
        eng_wordcount+=1.0;
        start = what_eng[0].second;
    }
    
    // 阈值过滤
    eng_ratio = eng_wordcount/(eng_wordcount+chi_wordcount+0.001);
    if (eng_ratio > ENG_RATIO_THRESH)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void CContentPreproc::replacePattern()
{
    for (int i=0; i<REP_NO; i++)
    {
        wStrFastReplace((wchar_t*)m_unicode_exchararray, myReplacePatterns[i], myReplaceStrings[i]);
    }
}

void CContentPreproc::yieldResult()
{
    int ret_transcoding = WUnicodeToUnicode( m_unicode_exchararray,WUNICODE_ARRAY_MAX_LEN,\
                                         m_unicode_chararray,UNICODE_ARRAY_MAX_LEN);
    if (ret_transcoding!=0)
    {
        this->reset();
        throw CException(ERR_TRANSCODING,"wunicode to unicode transcoding failed");
    }
}

const char* CContentPreproc::run(string &content)
{
    char* yieldUnicodeChararray = NULL;
    try
    {
        this->transCode(content);
        if (!this->contentFilter())
        {
            this->replacePattern();
            this->yieldResult();

            yieldUnicodeChararray = m_unicode_chararray;
        }
        return yieldUnicodeChararray;
    }
    catch(const CException& e)
    {
         cout << e << endl;
         return yieldUnicodeChararray;
    }
    catch(const exception& e)
    {
         cout << e.what() << endl;
         return yieldUnicodeChararray;
    }
    catch(...)
    {
         cout << "unknown exception~~" << endl;
         return yieldUnicodeChararray;
    }
}