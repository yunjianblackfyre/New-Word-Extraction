/*   
     AUTHOR: Sibyl System
     DATE: 2018-03-29
     DESC: mysql client simple encapsulation
*/
#ifndef MYSQL_API_H_
#define MYSQL_API_H_

#include <stdlib.h>  
#include "exception.h"
#include <stdio.h>  
#include <sys/time.h>
#include <iostream>
#include <mysql/mysql.h>
#include <mysql/errmsg.h>
#include <mysql/mysqld_error.h>

using namespace std;

const int ERR_DB_INITIAL       = 0001001;           //mysql连接初始化失败
const int ERR_DB_LOST          = 0001002;           //mysql连接丢失
const int ERR_DB_DUP_ENTRY     = 0001003;           //key重复
const int ERR_DB_NULL_RESULT   = 0001004;           //结果集为空
const int ERR_DB_AFFECT_ROW    = 0001005;           //影响行数错误
const int ERR_DB_UNKNOW        = 0001006;           //未知错误

/*字符串转整数*/
int readDbInt(const char *value);

/*字符串转长整数*/
int64_t readDbLong(const char *value);

/*字符串转字符串对象*/
string readDbString(const char *value);
  
class CMySqlClient
{
    protected:
        string    m_strHost;         //IP
        string    m_strUser;         //用户名
        string    m_strPasswd;       //密码
        MYSQL*    m_SqlHandle;       //SQL操作句柄
        bool      m_bInTransaction;  //当前SQL是否处于事务中
        uint32_t  m_iPort;           // 连接端口
        uint32_t  m_iOverTime;       // 连接超时时长
        
    protected:
        /*关闭连接*/
        void close();   
        
        /*建立连接*/
        void connect();
        
        /*ping连接*/
        bool ping();
        
    public:
        /*默认构造函数*/
        CMySqlClient();
        
        /*带参构造函数*/
        CMySqlClient(const string& host, const string& user, const string& passwd, int iPort, int iOverTime);
    
        /*析构函数*/
        ~CMySqlClient();
        
        /*开始事务*/
        void begin();
        
        /*提交事务*/
        void commit();
        
        /*事务回滚*/
        void rollback();
        
        /*执行SQL*/
        void query(const string &query_sql);
        
        /*取结果集*/
        MYSQL_RES* fetchStoreResult();

        /*取结果集*/
        MYSQL_RES* fetchUseResult();
        
        /*释放读取的数据*/
        static void freeResult(MYSQL_RES **pRes);
        
        /*转义字符串*/
        static string escapeString(const string& str);

};
#endif