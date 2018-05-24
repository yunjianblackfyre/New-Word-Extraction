/*   
     AUTHOR: Sibyl System
     DATE: 2018-04-03
     DESC: general resources
*/

#include "util.h"

int stringSplit(const std::string &inputStr, std::vector<std::string> &outputStrVector, char targetChar)
{
    size_t pos = inputStr.find( targetChar );
    size_t initialPos = 0;
    outputStrVector.clear();

    while ( pos != std::string::npos ) {
        outputStrVector.push_back( inputStr.substr( initialPos, pos - initialPos ) );
        initialPos = pos + 1;

        pos = inputStr.find( targetChar, initialPos );
    }

    outputStrVector.push_back( inputStr.substr( initialPos, min( pos, inputStr.size() ) - initialPos + 1 ) );
    return outputStrVector.size();
}

void coutStringHex(const std::string &str)
{
    if (str.size()!=0)
    {
        for (int i=0; i < str.size(); i++)
        {
            cout <<"element size: "<< sizeof(str[0]) <<": "<< hex << (unsigned int)(unsigned char)(str[i]) << endl;
        }
        cout << endl;
    }
}

void coutWStringHex(const std::wstring &str)
{
    if (str.size()!=0)
    {
        for (int i=0; i < str.size(); i++)
        {
            cout <<"element size: "<< sizeof(str[0]) <<": "<< hex << (unsigned int)(unsigned wchar_t)(str[i]) << endl;
        }
        cout << endl;
    }
}

void coutUnicodeArrayHex(const char* str, int num)
{
    if (str!=NULL)
    {
        int i = 0;
        while (true)
        {
            if ((i%2==0)&&(str[i]==0x00)&&(str[i+1]==0x00))
            {
                break;
            }
            else
            {
                if (i%2==0)
                {
                    cout <<hex<<(unsigned int)(unsigned char)str[i] << "|";
                }
                else
                {
                    cout <<hex<<(unsigned int)(unsigned char)str[i] << " ";
                }
                i++;
                if (num>0)
                {
                    if (i>num-1)
                    {
                        break;
                    }
                }
            }
        }
        cout << endl;
    }
}

void coutUnicodeUtf8(const string& str)
{
    if (str.size()>10)
    {
        cout << "Error:input string too long" <<endl;
        return;
    }
    char localUnicode[10] = {0};
    char localUtf8[20] = {0};
    const char* pStr = str.c_str();

    for (int i=0; i<str.size();i++)
    {
        localUnicode[i] = *(pStr+i);
    }
    
    code_convert((char*)"UNICODE",(char*)"UTF-8",localUnicode,10,localUtf8,20);
    cout << localUtf8 << endl;
}

int code_convert(char *from_charset,char *to_charset,char *inbuf, size_t inlen,char *outbuf, size_t outlen)
{
    iconv_t cd;
    char** pin = &inbuf;
    char** pout = &outbuf;
    
    cd = iconv_open(to_charset,from_charset);
    if (cd==0)
    {             
        return -1;
    }

    if (iconv(cd, pin, &inlen, pout, &outlen)==-1)
    {
        iconv_close(cd);
        return -1;
    }
    else
    {
        iconv_close(cd);
    }
    
    return 0;
}


