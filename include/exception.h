/**
 * COPYRIGHT: Tencent 
 *    AUTHOR: fishhu
 *      DATE: 2016-07-29
 *      DESC: 顶级异常类，业务侧将只能抛出顶级异常或顶级异常的派生异常
 */

#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include <string>
#include <exception>
#include <iostream>

using namespace std;

class CException : public exception
{
public:
    CException(int iErrCode, const char* szErrInfo, const char* szFile=NULL, const int iLine=0) throw() :
            m_iErrCode(iErrCode), m_sErrInfo(szErrInfo), m_sFile(szFile ? szFile : ""), m_iLine(iLine)
    {       
    }
			
    CException(int iErrCode, const string& sErrInfo, const char* szFile=NULL, const int iLine=0) throw() :
            m_iErrCode(iErrCode), m_sErrInfo(sErrInfo), m_sFile(szFile ? szFile : ""), m_iLine(iLine)
    {       
    }
			
    virtual ~CException() throw()
    {
    }

    inline const char* file() const throw()
    {
        return m_sFile.c_str();
    }

    inline int line() const throw()
    {
        return m_iLine;
    }

    inline int error() const throw()
    {
        return m_iErrCode;
    }

    inline const char* what() const throw()
    {
        return m_sErrInfo.c_str();
    }

    friend ostream& operator<<(ostream& os, const CException& e)
    {
		os << "[" << e.error() << ", " << e.what() << ", " << e.file() << ", " << e.line() << "]";
		return os;
    }

protected:
    int    m_iErrCode;  //错误码
    string m_sErrInfo;  //错误信息
    string m_sFile;     //错误产生的代码文件
    int    m_iLine;     //错误产生的代码行数
};

#endif // EXCEPTION_H_

