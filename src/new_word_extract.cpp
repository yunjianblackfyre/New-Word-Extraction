/*   
     AUTHOR: Sibyl System
     DATE: 2018-03-30
     DESC: new word extractor
*/

#include "new_word_extract.h"

inline void resetMapIdx2CharId(uint16_t* mapIdx2CharId)
{
    for (int i=0; i<CONTENT_STREAM_MAX_LEN/2;i++)
    {
        mapIdx2CharId[i]=0;
    }
}

CNewWordExtract::CNewWordExtract()
{
    loadOffset = 0;
    totalWords = 0;
    fileCount =  0;
    mapIdx2CharId = new uint16_t[CONTENT_STREAM_MAX_LEN/2]();
    content_stream = new char[CONTENT_STREAM_MAX_LEN]();
    content_stream_clean = new char[CONTENT_STREAM_MAX_LEN]();
}

CNewWordExtract::~CNewWordExtract()
{
    this->clean();
}

void CNewWordExtract::clean()
{
    this->reset();
    delete [] content_stream;
    delete [] content_stream_clean;
    delete [] mapIdx2CharId;
}

void CNewWordExtract::reset()
{
    loadOffset = 0;
    totalWords = 0;
    strncpy(content_stream, content_stream_clean, CONTENT_STREAM_MAX_LEN);
    resetMapIdx2CharId(mapIdx2CharId);
    
    mapChar2Id.clear();
    mapWord2Count.clear();
    mapWord2SldLvl.clear();
    mapWord2FreeLvl.clear();
    dictWord2Result.clear();
    
    map<string,map<uint16_t,int>*>::iterator itrWordSideInfo = mapWord2SideInfo.begin();
    for (itrWordSideInfo=mapWord2SideInfo.begin();itrWordSideInfo!=mapWord2SideInfo.end();itrWordSideInfo++)
    {
        map<uint16_t,int>* tempMapPtr = itrWordSideInfo->second;
        map<uint16_t,int>* tempMapPtrNext = (itrWordSideInfo->second)+1;
        
        if (tempMapPtr!=NULL)
        {
            tempMapPtr->clear();
            tempMapPtrNext->clear();
            delete [] tempMapPtr;
        }
    }
    mapWord2SideInfo.clear();
}

void CNewWordExtract::getExistedWords()
{
    
    ifstream in(EXISTED_WORDS_PATH);
    vector<string> oneLineWords;
    if (!in) {
        this->reset();
        throw CException(ERR_FILE_ERROR,"Reading file failed");
    }
    char oneline[255];
    while (in)
    {
        in.getline(oneline, 255);
        if (in)
        {            
            oneLineWords.clear();
            stringSplit(string(oneline), oneLineWords, ' ');
            setExistedWords[oneLineWords[0]]=0x00;  // 赋值在此无关紧要，可随意
        }
    }
    in.close();
}

int CNewWordExtract::loadContent(const char* content, int maxOffset)
{
    const char* ptrContent = content;
    int last_location = loadOffset + maxOffset; // 最后两个字节留给空格
    
    if (last_location > CONTENT_STREAM_MAX_LEN)
    {
        cout << "This content can not be loaded for excceded stream capacity" << endl;
        return -1;
    }
    else if (ptrContent==NULL)
    {
        cout << "This content can not be loaded for null pointer" << endl;
        return -1;
    }
    
    for (int i=0; i<maxOffset; i++,ptrContent++)
    {
        if ((i%2==0)&&(*ptrContent==0x00)&&(*(ptrContent+1))==0x00)
        {
            break;
        }
        content_stream[loadOffset] = *ptrContent;
        loadOffset++;
    }
    return 0;
}

void CNewWordExtract::buildCharIdMap()
{
    mapChar2Id.clear();
    string strOperator;
    strOperator.push_back(0x00);
    strOperator.push_back(0x00);
    map<string,uint16_t>::iterator itr = mapChar2Id.begin();
    int trueIdx = 0;
    uint16_t CharId = 0;
    
    for (int i=0; i < loadOffset/2; i++)
    {
        trueIdx = 2*i;
        if (!((content_stream[trueIdx]==0x00) && (content_stream[trueIdx+1]==0x00)))
        {
            strOperator[0] = content_stream[trueIdx];
            strOperator[1] = content_stream[trueIdx+1];
            itr = mapChar2Id.find(strOperator);
            if (itr==mapChar2Id.end())
            {
                mapChar2Id.insert(pair<string,uint16_t>(strOperator,CharId));
                CharId++;
            }
        }
    }
}

void CNewWordExtract::buildIdxCharIdMap()
{
    resetMapIdx2CharId(mapIdx2CharId);
    string strOperator;
    strOperator.push_back(0x00);
    strOperator.push_back(0x00);
    int trueIdx = 0;
    map<string,uint16_t>::iterator itrCharId = mapChar2Id.begin();

    for (int i=0; i<(loadOffset/2); i++)
    {
        trueIdx = i*2;
        
        strOperator[0] = content_stream[trueIdx];
        strOperator[1] = content_stream[trueIdx+1];
        itrCharId = mapChar2Id.find(strOperator);

        if (itrCharId!=mapChar2Id.end())
        {
            mapIdx2CharId[i]=itrCharId->second;
        }
    }
}

void CNewWordExtract::genBaseProperty(int tolerance)
{
    //参数检测
    if (tolerance > MAX_TOLERANCE)
    {
        this->reset();
        throw CException(ERR_TOLERANCE_EXCCEDED,"tolerance too long");
    }
    
    //初始化变量
    int trueIdx = 0;
    int trueOffset = 0;
    int rightIdx = 0;
    int leftIdx = 0;
    
    string strOperator;
    for (int j=0; j<tolerance; j++)
    {
        strOperator.push_back(0x00);
        strOperator.push_back(0x00);
    }
    map<string,int>::iterator itrWordCount = mapWord2Count.begin();
    map<string,map<uint16_t,int>*>::iterator itrWordSideInfo = mapWord2SideInfo.begin();
    
    for (int i=0; i<loadOffset/2;i++) // loadOffset是当前有用字符串之数量，永远为偶数
    {
        trueIdx = i*2;
        rightIdx = (i+tolerance)*2;
        leftIdx = (i-1)*2;
        bool withNoneChi = false;
        uint16_t chiInt16 = 0;
        
        if (trueIdx + tolerance*2 > loadOffset)
        {
            break;
        }
        else if (content_stream[trueIdx]==0x00&&content_stream[trueIdx+1]==0x00)
        {
            continue;
        }
        for (int j=0; j<tolerance; j++)
        {
            trueOffset = j*2;
            chiInt16 = *((uint16_t*)(&content_stream[trueIdx+trueOffset]));
            if (chiInt16 < 19968||chiInt16 > 40959)      // 中文unicode范围4e00-9fff
            {
                withNoneChi = true;
                break;
            }
            strOperator[trueOffset] = content_stream[trueIdx+trueOffset];
            strOperator[trueOffset+1] = content_stream[trueIdx+trueOffset+1];
        }
        
        // 填装string->词频映射
        if (withNoneChi)
        {
            continue;
        }
        else
        {
            this->totalWords++;
        }
        
        itrWordCount = mapWord2Count.find(strOperator);
        if (itrWordCount==mapWord2Count.end())
        {
            mapWord2Count.insert(pair<string,int>(strOperator,1));
        }
        else
        {
            mapWord2Count[strOperator]++;
        }
        
        // 填装string->自由度info映射
        if (tolerance < 2)
        {
            continue;
        }
        itrWordSideInfo = mapWord2SideInfo.find(strOperator);
        if (itrWordSideInfo==mapWord2SideInfo.end())
        {
            map<uint16_t,int> *siteMaps = new map<uint16_t,int>[2]();
            mapWord2SideInfo.insert(pair<string,map<uint16_t,int>*>(strOperator,siteMaps));
            
            // 填装右词中字母ID与频率之映射
            if (rightIdx < loadOffset)
            {
                uint16_t charIdx = mapIdx2CharId[rightIdx/2];
                map<uint16_t,int>::iterator rightMapItr;
                map<uint16_t,int>* rightMap = mapWord2SideInfo[strOperator]+1;
                
                rightMapItr = rightMap->find(charIdx);
                if (rightMapItr==rightMap->end())
                {
                    rightMap->insert(pair<uint16_t,int>(charIdx,1));
                }
                else
                {
                    (*rightMap)[charIdx]++;
                }
            }
            // 填装左词中字母ID与频率之映射
            if (leftIdx>=0)
            {
                uint16_t charIdx = mapIdx2CharId[leftIdx/2];
                map<uint16_t,int>::iterator leftMapItr;
                map<uint16_t,int>* leftMap = mapWord2SideInfo[strOperator];
                
                leftMapItr = leftMap->find(charIdx);
                if (leftMapItr==leftMap->end())
                {
                    leftMap->insert(pair<uint16_t,int>(charIdx,1));
                }
                else
                {
                    (*leftMap)[charIdx]++;
                }
            }
        }
        else
        {
            // 填装右词中字母ID与频率之映射
            if (rightIdx < loadOffset)
            {
                uint16_t charIdx = mapIdx2CharId[rightIdx/2];
                map<uint16_t,int>::iterator rightMapItr;
                map<uint16_t,int>* rightMap = mapWord2SideInfo[strOperator]+1;
                
                rightMapItr = rightMap->find(charIdx);
                if (rightMapItr==rightMap->end())
                {
                    rightMap->insert(pair<uint16_t,int>(charIdx,1));
                }
                else
                {
                    (*rightMap)[charIdx]++;
                }
            }
            
            // 填装左词中字母ID与频率之映射
            if (leftIdx>=0)
            {
                uint16_t charIdx = mapIdx2CharId[leftIdx/2];
                map<uint16_t,int>::iterator leftMapItr;
                map<uint16_t,int>* leftMap = mapWord2SideInfo[strOperator];
                
                leftMapItr = leftMap->find(charIdx);
                if (leftMapItr==leftMap->end())
                {
                    leftMap->insert(pair<uint16_t,int>(charIdx,1));
                }
                else
                {
                    (*leftMap)[charIdx]++;
                }
            }
            
        }
    }
}

float CNewWordExtract::calcFreeLvl(const string& inputStr)
{
    map<string,map<uint16_t,int>*>::iterator itrWordSideInfo;
    
    itrWordSideInfo = mapWord2SideInfo.find(inputStr);
    if (itrWordSideInfo==mapWord2SideInfo.end())
    {
        this->reset();
        throw CException(ERR_MAPPING_FAILURE,"map look up failed");
    }
    
    int leftSideWordFreq = 0;
    int rightSideWordFreq = 0;
    float FreeLvl = 0.0;
    map<uint16_t,int>::iterator tempMapIter;
    
    // 计算分母，用于之后的信息熵计算
    map<uint16_t,int>* tempMapPtr = itrWordSideInfo->second;
    for (tempMapIter=tempMapPtr->begin();tempMapIter!=tempMapPtr->end();tempMapIter++)
    {
        leftSideWordFreq += tempMapIter->second;
    }

    tempMapPtr = tempMapPtr + 1;
    for (tempMapIter=tempMapPtr->begin();tempMapIter!=tempMapPtr->end();tempMapIter++)
    {
        rightSideWordFreq += tempMapIter->second;
    }
    
    // 计算自由度（左右两边最小信息熵）
    float fLeftSideWordFreq =   float(leftSideWordFreq);
    float fRightSideWordFreq =  float(rightSideWordFreq);
    float rightFreeLvl =        0.0;
    float leftFreeLvl =         0.0;
    float sideCount =           0.0;
    float fTemp =               0.0;

    // 计算右信息熵
    tempMapPtr = itrWordSideInfo->second;
    for (tempMapIter=tempMapPtr->begin();tempMapIter!=tempMapPtr->end();tempMapIter++)
    {
        sideCount = float(tempMapIter->second);
        fTemp = sideCount/fLeftSideWordFreq;
        rightFreeLvl -= fTemp*log(fTemp);
    }

    // 计算左信息熵
    tempMapPtr = tempMapPtr + 1;
    for (tempMapIter=tempMapPtr->begin();tempMapIter!=tempMapPtr->end();tempMapIter++)
    {
        sideCount = float(tempMapIter->second);
        fTemp = sideCount/fRightSideWordFreq;
        leftFreeLvl -= fTemp*log(fTemp);
    }
    return min(rightFreeLvl,leftFreeLvl);
}

float CNewWordExtract::calcSldLvl(const string& inputStr)
{
    int strSize = inputStr.size();
    if (strSize%2!=0 || strSize>MAX_TOLERANCE*2 || strSize<4)
    {
        this->reset();
        throw CException(ERR_DATA_FORMAT,"Input string size not qualified");
    }
    
    string rightPart;
    string leftPart;
    float inputStrProb;
    float leftProb;
    float rightProb;
    float tempSld;
    float minSld = 0.0;
    map<string,int>::iterator itrInputStrCount;
    map<string,int>::iterator itrLeftStrCount;
    map<string,int>::iterator itrRightStrCount;
    
    for (int i=2; i<strSize; i+=2)
    {
        rightPart.clear();
        leftPart.clear();
        for (int j=0; j<i; j++)
        {
            rightPart.push_back(inputStr[j]);
        }
        for (int j=i; j<strSize; j++)
        {
            leftPart.push_back(inputStr[j]);
        }
        
        itrInputStrCount =  mapWord2Count.find(inputStr);
        itrLeftStrCount =   mapWord2Count.find(leftPart);
        itrRightStrCount =  mapWord2Count.find(rightPart);
        
        if (itrInputStrCount==mapWord2Count.end()||itrLeftStrCount==mapWord2Count.end()||\
           itrRightStrCount==mapWord2Count.end())
        {
            this->reset();
            throw CException(ERR_MAPPING_FAILURE,"Map look up failed");
        }
        
        inputStrProb =  (float(itrInputStrCount->second))/(float(totalWords));
        leftProb =      (float(itrLeftStrCount->second))/(float(totalWords));
        rightProb =     (float(itrRightStrCount->second))/(float(totalWords));
        tempSld =       inputStrProb/(leftProb*rightProb);
        
        if (i==2)
        {
            minSld = tempSld;
        }
        else
        {
            minSld = min(minSld, tempSld);
        }
    }
    
    return minSld;
}

void CNewWordExtract::genAdvanceProperty()
{
    map<string,map<uint16_t,int>*>::iterator itrWordMap;
    float SldLvl = 0.0;
    float FreeLvl = 0.0;
    for (itrWordMap=mapWord2SideInfo.begin();itrWordMap!=mapWord2SideInfo.end();itrWordMap++)
    {
        SldLvl = calcSldLvl(itrWordMap->first);
        FreeLvl = calcFreeLvl(itrWordMap->first);
        
        mapWord2SldLvl.insert(pair<string,float>(itrWordMap->first,SldLvl));
        mapWord2FreeLvl.insert(pair<string,float>(itrWordMap->first,FreeLvl));
    }
}

void CNewWordExtract::refineResult()
{
    map<string,map<uint16_t,int>*>::iterator itrWordMap;
    map<string,float>::iterator itrFreeLvl;
    map<string,float>::iterator itrSldLvl;
    map<string,char>::iterator itrExist;
    map<string,int>::iterator itrCount;
    
    float SldLvl =  0.0;
    float FreeLvl = 0.0;

    char charUtf8[MAX_UTF8_LEN] =               {0};
    char charUtf8Clean[MAX_UTF8_LEN] =          {0};
    char charUnicode[MAX_UNICODE_LEN] =         {0};
    char charUnicodeClean[MAX_UNICODE_LEN] =    {0};
    
    for (itrWordMap=mapWord2SideInfo.begin();itrWordMap!=mapWord2SideInfo.end();itrWordMap++)
    {
        const string& searchWord = itrWordMap->first;
        itrSldLvl =     mapWord2SldLvl.find(searchWord);
        itrFreeLvl =    mapWord2FreeLvl.find(searchWord);
        itrCount =      mapWord2Count.find(searchWord);
        strncpy(charUtf8,charUtf8Clean,MAX_UTF8_LEN);
        strncpy(charUnicode,charUnicodeClean,MAX_UNICODE_LEN);
        
        if (itrSldLvl==mapWord2SldLvl.end()||itrFreeLvl==mapWord2FreeLvl.end()||\
           itrCount==mapWord2Count.end())
        {
            this->reset();
            throw CException(ERR_MAPPING_FAILURE,"Map look up failed");
        }
        const char* pSearchWord = searchWord.c_str();
        
        for (int i=0; i<searchWord.size();i++) // 这里不能用strncpy或者strcpy，因为单个空char不代表unicode字符结束
        {
            charUnicode[i] = *(pSearchWord+i);
        }
        code_convert((char*)"UNICODE",(char*)"UTF-8",charUnicode,MAX_UNICODE_LEN,charUtf8,MAX_UTF8_LEN);
        
        itrExist = setExistedWords.find(string(charUtf8));
        SldLvl = itrSldLvl->second;
        FreeLvl = itrFreeLvl->second;

        if ((SldLvl > SLD_LVL_THRESH) && (FreeLvl > FREE_LVL_THRESH) &&(itrExist==setExistedWords.end()))
        {
            dictWord2Result[string(charUtf8)] = itrCount->second;
        }
    }
}

void CNewWordExtract::saveResult()
{
    Json::FastWriter fWriter;
    char charFileCount[10] = {0};
    string resultStream = fWriter.write(dictWord2Result);
    
    fileCount++;
    
    string strFileCount = std::to_string(fileCount);
    stringstream filePathStream;
    filePathStream << NEW_WORDS_PATH << "new_words_" << strFileCount << ".json";
    
    string filePathStr = filePathStream.str();
    std::ofstream resultFile(filePathStr);
    resultFile << resultStream;
    resultFile.close();
}

void CNewWordExtract::run()
{
    try
    {
        cout << "Buid all sorts of map" << endl;
        this->buildCharIdMap();
        this->buildIdxCharIdMap();
        
        for (int i=1; i<5; i++)
        {
            this->genBaseProperty(i);
        }
        
        cout << "Gen Free lvl and Sld lvl" << endl;
        this->genAdvanceProperty();
        
        cout << "Refine result" << endl;
        this->refineResult();
        this->saveResult();
        
        cout << "Reset everything and start a new" << endl;
        this->reset();
    }
    catch(const CException& e)
    {
         cout << e << endl;
    }
    catch(const exception& e)
    {
         cout << e.what() << endl;
    }
    catch(...)
    {
         cout << "unknown exception~~" << endl;
    }
}