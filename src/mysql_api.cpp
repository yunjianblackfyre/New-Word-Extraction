/*   
     AUTHOR: Sibyl System
     DATE: 2018-03-29
     DESC: mysql client simple encapsulation
*/

#include "mysql_api.h"

CMySqlClient::CMySqlClient():
    m_strHost("localhost"),
    m_strUser("root"),
    m_strPasswd("caonimabi"),
    m_iPort(0),
    m_iOverTime(3600),
    m_SqlHandle(NULL),
    m_bInTransaction(false)
{
}

CMySqlClient::CMySqlClient(const string& host, const string& user, const string& passwd, int iPort, int iOverTime)
{
    m_strHost = host;
    m_strUser = user;
    m_strPasswd = passwd;
    m_iPort = iPort;
    m_iOverTime = iOverTime;

    m_SqlHandle = NULL;
    m_bInTransaction = false;
}

CMySqlClient::~CMySqlClient()
{
    close();
}

void CMySqlClient::close()
{
    if(m_SqlHandle)
    {
        mysql_close(m_SqlHandle);
        free(m_SqlHandle);
        m_SqlHandle = NULL;
        m_bInTransaction = false;
    }
}

bool CMySqlClient::ping()
{
    return mysql_ping(m_SqlHandle) == 0;
}

void CMySqlClient::connect()
{
    //初始化连接句柄
    if(m_SqlHandle == NULL)
    {
        m_SqlHandle =(MYSQL *)malloc(sizeof(MYSQL));
        m_bInTransaction = false;
    }

    //初始化
    mysql_init(m_SqlHandle);

    //初始化失败
    if(m_SqlHandle == NULL)
    {
        throw CException(ERR_DB_INITIAL, "Failed to allocate mysql handle in mysql_init");
    }
    
    //设置选项
    mysql_options(m_SqlHandle, MYSQL_OPT_CONNECT_TIMEOUT, (char *)&m_iOverTime); //超时时长
    mysql_options(m_SqlHandle, MYSQL_SET_CHARSET_NAME, "utf8");   //字符集

    //建立连接
    if(!mysql_real_connect(m_SqlHandle, m_strHost.c_str(), m_strUser.c_str(), m_strPasswd.c_str(), NULL, m_iPort, NULL, 0))
    {
        string strError = string("Failed to connetct db :") + string(mysql_error(m_SqlHandle));
        close();
        throw CException(ERR_DB_INITIAL, strError.c_str(), __FILE__, __LINE__);
    }
    else
    {
        cout << "Connection success" << endl;
        query("SET AUTOCOMMIT=0"); // 关闭自动提交
    }
}

void CMySqlClient::begin()
{
    query("BEGIN");
    m_bInTransaction = true;
}

void CMySqlClient::commit()
{
    query("COMMIT");
    m_bInTransaction = false;
}

void CMySqlClient::rollback()
{
    query("ROLLBACK");
    m_bInTransaction = false;
}

void CMySqlClient::query(const string &query_sql)
{
    struct timeval start, end;

    //若处于断连状态，则进行重连
    if(m_SqlHandle == NULL)
    {
        connect();
    }

    //获取调用起始时间
    gettimeofday(&start, NULL);   

    //执行查询
    int iRet = mysql_real_query(m_SqlHandle, query_sql.c_str(), query_sql.length());

    // 若SQL处于非事务当中，重做一次
    if(iRet!=0 && !m_bInTransaction)
    {
        //重连一次
        ping();

        //重新执行SQL
        iRet = mysql_real_query(m_SqlHandle, query_sql.c_str(), query_sql.length());
    }

    //获取调用结束时间
    gettimeofday(&end, NULL);   

    //记录日志
    cout << "|sql|" << (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec)/1000
            << "|" << iRet << "|" << query_sql << endl;
    
    if(iRet != 0)
    {
        //保存错误信息
        string strError = mysql_error(m_SqlHandle);

        cout << "mysql_real_query failed.mysql_errno:" << mysql_errno(m_SqlHandle)
                << "|mysql_error:" << strError << endl;

        //判断是否为数据库断连
        if(mysql_errno(m_SqlHandle)==CR_SERVER_LOST || mysql_errno(m_SqlHandle)==CR_SERVER_GONE_ERROR)
        {
            close();// 关闭连接
            throw CException(ERR_DB_LOST, strError.c_str(), __FILE__, __LINE__);
        }
        else if(mysql_errno(m_SqlHandle) == ER_DUP_ENTRY)
        {
            throw CException(ERR_DB_DUP_ENTRY, strError.c_str(), __FILE__, __LINE__);
        }
        else
        { 
            throw CException(ERR_DB_UNKNOW, strError.c_str(), __FILE__, __LINE__);
        }
    }
}

MYSQL_RES* CMySqlClient::fetchStoreResult()
{
    MYSQL_RES* pRes = mysql_store_result(m_SqlHandle);
    if(pRes == NULL)
    {
        throw CException(ERR_DB_NULL_RESULT, "Fetch result for non-select statement", __FILE__, __LINE__);
    }
    
    return pRes;
}

MYSQL_RES* CMySqlClient::fetchUseResult()
{
    MYSQL_RES* pRes = mysql_use_result(m_SqlHandle);
    if(pRes == NULL)
    {
        throw CException(ERR_DB_NULL_RESULT, "Fetch result for non-select statement", __FILE__, __LINE__);
    }
    
    return pRes;
}

void CMySqlClient::freeResult(MYSQL_RES **pRes)
{
    if(pRes!=NULL && *pRes!=NULL)
    {
        mysql_free_result(*pRes);
        *pRes = NULL;
    }
}

string CMySqlClient::escapeString(const string& str)
{
    char szTmp[10240] = {0};
    mysql_escape_string(szTmp, str.c_str(), str.length());
    return szTmp;
}

int readDbInt(const char *value)
{
    return value ? atoi(value) : 0;
}

int64_t readDbLong(const char *value)
{
    return value ? atoll(value) : 0;
}

string readDbString(const char *value)
{
    return value ? value : "";
}

/*
int main(int argc, char *argv[]) {  

    MYSQL_RES* pRes = NULL;
    try
    {
        CMySqlClient mysql_client = CMySqlClient();
        mysql_client.begin();
        // string sql = "insert into test.t_test_1 (Ftitle,Fcontent) values ('兄贵666','王の摔跤');";
        string sql = "select * from test.t_test_1 limit 20";
        
        mysql_client.query(CMySqlClient::escapeString(sql));
        
        // 循环读取数据
        MYSQL_ROW row = NULL;
        pRes = mysql_client.fetchStoreResult();
        while((row = mysql_fetch_row(pRes)))
        {
            //获取结果
            int row_i = 0;
            int auto_id    = readDbInt(row[row_i++]);
            string title   = readDbString(row[row_i++]);
            string content = readDbString(row[row_i++]);
            cout <<"auto_id:"<< auto_id <<" title:"<< title << " content:" << content << endl;
        }
        CMySqlClient::freeResult(&pRes);
        mysql_client.commit();
    }
    catch(const CException& e)
    {
        cout << e << endl;
        CMySqlClient::freeResult(&pRes);
    }
    catch(const exception& e)
    {
        cout << e.what() << endl;
        CMySqlClient::freeResult(&pRes);
    }
    catch(...)
    {
        cout << "UnKown Error Detected" << endl;
        CMySqlClient::freeResult(&pRes);
    }

}*/