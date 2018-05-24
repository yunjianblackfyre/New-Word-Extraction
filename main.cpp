/*   
     AUTHOR: Sibyl System
     DATE: 2018-03-30
     DESC: main for new word extractor
*/

#include "content_preproc.h"
#include "new_word_extract.h"
#include "mysql_api.h"

/*
// json库测试
int main(int argc, char *argv[])
{
    string test="{\"id\":1,\"name\":\"test\"}";
    Json::Reader  reader;
    Json::Value   value;

    if (reader.parse(test,value))
    {
        if (!value["id"].isNull())
        {
            cout<<value["id"].asInt()<<endl;
            cout<<value["name"].asString()<<endl;
        }
    }
    return 0;
}
*/


int main(int argc, char *argv[]) {
    // 初始化变量
    char cSql[1000] =   {0};
    MYSQL_RES* pRes =   NULL;
    int oneTimeRead =   6000;
    int readStep =      6000;
    int readStart =     0;
    int readEnd =       oneTimeRead;
    
    // 开始主过程
    try
    {
        CMySqlClient mysql_client =         CMySqlClient();
        CContentPreproc preProcessor =      CContentPreproc();
        CNewWordExtract newWordExtractor =  CNewWordExtract();
        newWordExtractor.getExistedWords();
        mysql_client.begin();
        
        while (true)
        {
            snprintf(cSql, sizeof(cSql),
            "select Fauto_id, Fjob_summary from db_job.t_zhilian_detail where Fauto_id between %d and %d",
            readStart,readEnd       // 注意这里用between，limit后数字太大会引起mysql反应极慢
            );
            
            string sql = cSql;
            
            mysql_client.query(CMySqlClient::escapeString(sql));
            
            MYSQL_ROW row = NULL;
            pRes = mysql_client.fetchStoreResult();

            if (mysql_num_rows(pRes)==0)
            {
                break;
            }
            
            while ((row = mysql_fetch_row(pRes)))
            {
                string content_id = readDbString(row[0]);
                string content = readDbString(row[1]);
                newWordExtractor.loadContent(preProcessor.run(content),preProcessor.getResultLen());
            }
            newWordExtractor.run();
            CMySqlClient::freeResult(&pRes);
            mysql_client.commit();
            break;
            readStart+=readStep;
            readEnd += readStep;
            
        }
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

}