#include "ZLibString.h"
#include <cstring>

//CZlibMgr* CZlibMgr::instance = NULL;

CZlibMgr::CZlibMgr() = default;

CZlibMgr::~CZlibMgr() = default;

bool CZlibMgr::Compress(const char *pcContentBuf, char *pcCompBuf, unsigned long &ulCompLen) {
    if (pcContentBuf == nullptr) {
        return false;
    }

    if (ulCompLen == 0) {
        return false;
    }

    memset(compr, 0, MAXBUFFERSIZE);

    uLong comprLen;
    int err;

    uLong len = ulCompLen;
    comprLen = sizeof(compr) / sizeof(compr[0]);

    err = compress(compr, &comprLen, (const Bytef *) pcContentBuf, len);
    if (err != Z_OK) {
        cout << "compess error: " << err << endl;
        return false;
    }
    //cout << "orignal size: " << len << " , compressed size : " << comprLen << endl;
    memcpy((Byte *) pcCompBuf, compr, comprLen);
    ulCompLen = comprLen;

    return true;
}

bool CZlibMgr::UnCompress(const char *pcCompBuf, unsigned long CompLen, char *pcUnCompBuf, unsigned long unCompLen) {
    if (pcCompBuf == nullptr || pcUnCompBuf == nullptr) {
        cout << __FUNCTION__ << "================> pcCompBuf is null please to check " << endl;
        return false;
    }

    if (CompLen == 0 || unCompLen == 0) {
        cout << __FUNCTION__ << "strlen(pcCompBuf) == 0  ========================> " << endl;
        return false;
    }

    int err;

    err = uncompress(reinterpret_cast<Bytef *>(pcUnCompBuf), &unCompLen, (const Bytef *) pcCompBuf, CompLen);
    if (err != Z_OK) {
        cout << "uncompess error: " << err << " length: ";
        cout << CompLen << " --> " << unCompLen << endl;
        cout << "wait for decode: " << std::string(pcCompBuf) << endl;
        return false;
    }

    return true;
}

