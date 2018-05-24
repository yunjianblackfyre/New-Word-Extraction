/*   
     AUTHOR: Sibyl System
     DATE: 2018-03-30
     DESC: content preprocessing
*/
#ifndef CONTENT_PROPROC_H_
#define CONTENT_PROPROC_H_

#include <sys/time.h>
#include <wchar.h>
#include <iostream>
#include <set>
#include <map>
#include <boost/regex.hpp>
#include "exception.h"
#include "util.h"

using namespace std;

const int CONTENT_MAX_LEN =             3500;                   // 招聘信息最大字节数
const size_t UTF8_ARRAY_MAX_LEN =       CONTENT_MAX_LEN;        // 招聘信息utf8编码最大字节数
const size_t UNICODE_ARRAY_MAX_LEN =    2*CONTENT_MAX_LEN;      // 招聘信息unicode编码最大字节数
const size_t WUNICODE_ARRAY_MAX_LEN =   4*CONTENT_MAX_LEN;      // 招聘信息宽字节unicode编码最大字节数
const wstring CHI_WORD =                L"[\u4e00-\u9fa0]";     // 汉字正则特征
const wstring ENG_WORD =                L"[a-zA-Z]{2,}";        // 英语单词正则特征
const size_t SUB_NO =                   4;                      // 替代词最大宽字节数
const size_t REP_NO =                   151;                    // 替代词个数
const double ENG_RATIO_THRESH =         0.25;                   // 招聘信息中单词所占比重阈值
const size_t MAX_REPSTR_LEN =           10;                     // 替换词最大字节数

/*替换目标词集合*/
const wchar_t myReplacePatterns[REP_NO][4] = {
 L"各类型",
 L"智能", L"个性", L"并发", L"并行", L"才能", L"不能", L"性能", L"功能", L"技能", L"拥有", L"具有", L"现有", 
 L"已有", L"以及", L"以上", L"以下", L"以前", L"以后", L"以往", L"较为", L"作为", L"各类", L"各种", L"各项", 
 L"各组", L"针对", L"整个", L"这个", L"多个", L"有个", L"一个", L"二个", L"两个", L"三个", L"五个", L"六个", 
 L"七个", L"八个", L"九个", L"一定", L"一种", L"一项", L"一起", L"一门", L"两种", L"两项", L"两起", L"两门", 
 L"我们", L"他们", L"你们", L"对接", L"对于", L"为人", L"或者", L"并且", L"可以", L"基于", L"善于", L"从业", 
 L"从事", L"其他", L"使用", L"快速", L"基本", L"至少", L"最少", L"如上", L"如下", L"日常", L"个大", L"个人", 
 L"例如", L"诸如", L"常用", L"各个", L"能够", L"能力", L"一年", L"两年", L"三年", L"四年", L"五年", L"六年", 
 L"七年", L"八年", L"九年", L"十年", L"1年",  L"2年",  L"3年",  L"4年",  L"5年",  L"6年",  L"7年",  L"8年",
 L"9年",  L"10年",
 L"的", L"能", L"有", L"和", L"及", L"与", L"且", L"或", L"对", L"等", L"为", L"各", L"以", L"可", L"者", L"如", L"并",
 L"!", L"(", L")", L"[", L"]", L"{", L"}",
 L";", L":", L"\"", L"\'", L"<", L">", L",", L".", L"?", L"`",
 L"！", L"（", L"）",  L"【", L"】", L"；", L"：", L"“", L"”", L"‘", L"’", L"《", L"》", L"，", L"。",
 L"？", L"、", L"·"
};

/*替换词集合*/
const wchar_t myReplaceStrings[REP_NO][4] = {
 L"   ",
 L"智慧", L"性格", L"霰发", L"霰行", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", 
 L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", 
 L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", 
 L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", 
 L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", 
 L"  ", L"  ", L"  ", L"  ",  L"  ",  L"  ",  L"  ",  L"  ",  L"  ",  L"  ",  L"  ", L"  ", L"  ", L"  ", 
 L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ", L"  ",
 
 L" ", L" ", L" ", L" ", L" ", L" ", L" ", L" ", L" ", L" ", L" ", L" ", L" ", L" ", L" ", L" ", L" ",
 L" ", L" ", L" ", L" ", L" ", L" ", L" ", L" ", L" ", L" ", L" ", L" ", L" ", L" ", L" ", L" ", L" ", L" ",
 L" ", L" ", L" ", L" ", L" ", L" ", L" ", L" ", L" ", L" ", L" ", L" ", L" ", L" ", L" ", L" ",  L" "
};

const int ERR_CONTENT_LENGTH    = 0002001;           // 超出招聘信息最大长度
const int ERR_CONTENT_EMPTY     = 0002002;           // 招聘信息为空
const int ERR_TRANSCODING       = 0002003;           // 转码失败

/*字符串转码：utf8->unicode*/
int Utf82Unicode(char *inbuf, size_t inlen, char *outbuf, size_t outlen);

/*字符串转码：unicode->utf8*/
int Unicode2Utf8(char *inbuf, size_t inlen, char *outbuf, size_t outlen);

/*字符串转码：unicode->wunicode*/
int UnicodeToWUnicode(const char* inbuf, size_t inlen, char *outbuf, size_t outlen);

/*字符串转码：wunicode->unicode*/
int WUnicodeToUnicode(const char* inbuf, size_t inlen, char *outbuf, size_t outlen);

/*字符串批量替换*/
int wStrBatchReplace(wchar_t* pContent, const wchar_t* pReplace, map<wstring,wstring>& strBatchMap);

/*字符串快速替换*/
void wStrFastReplace(wchar_t *pContent, const wchar_t* pTarget, const wchar_t* pReplace);

class CContentPreproc
{
    private:
        char* m_utf8_chararray;                     // 招聘信息缓存，编码：utf8
        char* m_unicode_chararray;                  // 招聘信息缓存，编码：unicode
        char* m_unicode_exchararray;                // 招聘信息缓存，编码：宽字节unicode
        const char* m_utf8_chararray_clean;         // 用于重置招聘信息缓存的空白字符串
        const char* m_unicode_chararray_clean;
        const char* m_unicode_exchararray_clean;
        std::wstring m_wcontent;                    // 招聘信息缓存，编码：宽字节unicode
        map<wstring,wstring> repMapSingle;
        map<wstring,wstring> repMapDouble;
        
    public:
        /*默认构造函数*/
        CContentPreproc();
        
        /*析构函数*/
        ~CContentPreproc();
        
        /*获取招聘信息最大长度*/
        int getResultLen();
        
        friend int UnicodeToWUnicode(const char* inbuf, size_t inlen, char *outbuf, size_t outlen);
        friend int Utf82Unicode(char *inbuf, size_t inlen, char *outbuf, size_t outlen);
        friend char *strncpy(char *dst, const char *src, size_t len);
        friend void wStrFastReplace(wchar_t *pContent, const wchar_t* pTarget, const wchar_t* pReplace);
        
        /*处理招聘信息，并将结果指针返回*/
        const char* run(string &content);
        
    private:
        /*释放成员变量*/
        void clean();
        
        /*重置成员变量*/
        void reset();
        
        /*招聘信息原始编码转为可以处理的编码*/
        void transCode(string &content);
        
        /*过滤不合格的招聘信息*/
        bool contentFilter();
        
        /*招聘信息预处理*/
        void replacePattern();
        
        /*返回处理结果*/
        void yieldResult();
};
#endif