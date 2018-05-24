/*   
     AUTHOR: Sibyl System
     DATE: 2018-03-30
     DESC: new word extractor
*/
#ifndef NEW_WORD_EXTRACT_H_
#define NEW_WORD_EXTRACT_H_

#include <sys/time.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <wchar.h>
#include <json/json.h>
#include "exception.h"
#include "util.h"

using namespace std;

const int MAX_TOLERANCE =           5;                  // 新词最大长度
const int MAX_UNICODE_LEN =         MAX_TOLERANCE*2;    // 新词unicode最大长度
const int MAX_UTF8_LEN =            MAX_TOLERANCE*4;    // 新词utf8最大长度

const float FREE_LVL_THRESH =       1.4;                // 自由度阈值
const float SLD_LVL_THRESH =        40.0;               // 凝固度阈值
const int CONTENT_STREAM_MAX_LEN =  8000000;           // 40000000 一次处理的招聘信息集合最大字节数

const string EXISTED_WORDS_PATH =   "/home/caonimabi/develop/job_data_mining/jieba/dict.txt";           // 已存在词词库路径
const string NEW_WORDS_PATH =       "/home/caonimabi/develop/job_data_mining/cpp_new_word/new_words/";  // 新词结果存放路径

const int ERR_TOLERANCE_EXCCEDED  = 0002001;           // 超出新词最大长度
const int ERR_MAPPING_FAILURE  =    0002002;           // 查询映射关系失败
const int ERR_DATA_FORMAT  =        0002003;           // 数据格式错误
const int ERR_FILE_ERROR  =         0002004;           // 文件相关错误

/*重置集合字符串位置至字符ID之映射*/
inline void resetMapIdx2CharId(uint16_t* mapIdx2CharId);

class CNewWordExtract
{
    private:
        char *content_stream;                               // 一次处理的招聘信息集合字符串
        const char* content_stream_clean;                   // 用于重置上面字符串
        int loadOffset;                                     // 实际载入的招聘信息集合大小
        uint16_t *mapIdx2CharId;                            // 集合字符串位置至字符ID映射
        int totalWords;                                     // 汉语词数统计
        int fileCount;                                      // 招聘信息集合批次
        map<string,char> setExistedWords;                   // 已存在词汇集合
        map<string,uint16_t> mapChar2Id;                    // 字符至字符ID映射
        map<string,int> mapWord2Count;                      // 词至词频率映射
        map<string,map<uint16_t,int>*> mapWord2SideInfo;    // 词至词的左右信息映射
        map<string,float> mapWord2SldLvl;                   // 词至词的凝固度映射
        map<string,float> mapWord2FreeLvl;                  // 词至词的自由度映射
        Json::Value dictWord2Result;                        // 新词至新词的词频映射
        
    public:
        /*默认构造函数*/
        CNewWordExtract();
    
        /*析构函数*/
        ~CNewWordExtract();
        
        /*读取已经存在的词*/
        void getExistedWords();
        
        /*友元，用于重置成员变量*/
        friend inline void resetMapIdx2CharId(uint16_t* mapIdx2CharId);
        
        /*填装招聘信息集合字符串*/
        int loadContent(const char* content,int maxOffset);
        
        /*提取的新词存入本地*/
        void saveResult();
        
        /*主逻辑*/
        void run();
        
    private:
        /*释放成员变量*/
        void clean();
        
        /*重置成员变量*/
        void reset();
        
        /*构造字符->字符ID映射*/
        void buildCharIdMap();
        
        /*构造招聘信息集合字符串索引->字符ID映射*/
        void buildIdxCharIdMap();
        
        /*产生基本属性：词频与左右信息*/
        void genBaseProperty(int tolerance);
        
        /*计算自由度*/
        float calcFreeLvl(const string& inputStr);
        
        /*计算凝固度*/
        float calcSldLvl(const string& inputStr);
        
        /*产生高级属性：自由度与凝固度*/
        void genAdvanceProperty();
        
        /*过滤最终结果*/
        void refineResult();
};
#endif