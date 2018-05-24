/*   
     AUTHOR: Sibyl System
     DATE: 2018-04-03
     DESC: general resources
*/
#ifndef UTIL_H_
#define UTIL_H_

#include <iconv.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

/*分割字符串，结果填装入vector*/
int stringSplit(const std::string &inputStr, std::vector<std::string> &outputStrVector, char targetChar);

/*string字符单元以16进制输出*/
void coutStringHex(const std::string &str);

/*wstring字符单元以16进制输出*/
void coutWStringHex(const std::wstring &str);

/*长unicode字符串按照存储顺序以16进制输出*/
void coutUnicodeArrayHex(const char* str,int num=0);

/*unicode词语输出为utf8新式，词语不超过5个汉字*/
void coutUnicodeUtf8(const string &str);

/*字符串转码*/
int code_convert(char *from_charset,char *to_charset,char *inbuf, size_t inlen,char *outbuf, size_t outlen);

#endif