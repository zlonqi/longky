#ifndef __hhl__ZLibString__
#define __hhl__ZLibString__

#define  MAXBUFFERSIZE 20000
#include <iostream>
#include <zlib.h>

using namespace std;

class CZlibMgr
{
public:
    CZlibMgr();
    ~CZlibMgr();
    //static CZlibMgr* getInstance();
    
    bool Compress(const char* pcContentBuf, char* pcCompBuf, unsigned long& ulCompLen);  // 压缩，pcContentBuf 要压缩的内容 pcCompBuf 压缩后的内容 ulCompLen 得到压缩后的长度
    bool UnCompress(const char* pcCompBuf, unsigned long CompLen, char* pcUnCompBuf, unsigned long unCompLen); // 解压,pcCompBuf 压缩的内容, pcUnCompBuf 解压后的内容  CompLen 传入压缩内容的长度
    
private:
    Byte compr[MAXBUFFERSIZE];
    //Byte uncompr[MAXBUFFERSIZE];
    //单例
    //static CZlibMgr* instance;
    
};


#endif /* defined(__hhl__ZLibString__) */
