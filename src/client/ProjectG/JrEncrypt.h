// Original: d:\build\custom\temp\code\client\projectg\jrencrypt.h
#ifndef PG_CLIENT_PROJECTG_JRENCRYPT_H
#define PG_CLIENT_PROJECTG_JRENCRYPT_H

// Stream ciphers of the network packets (jrencrypt.cpp). The Alpha pair
// works in place (`source` == `dest`, as WSendPacket and WReceivedPacket call
// it) and also leaves its result in `source`.
void SimpleStreamEncrypt_Alpha(const char* source, char* dest, unsigned int length, unsigned int key);
void SimpleStreamDecrypt_Alpha(const char* source, char* dest, unsigned int length, unsigned int key);
void SimpleStreamEncrypt_Delta(const char* source, char* dest, unsigned int length, unsigned int key);
void SimpleStreamDecrypt_Delta(const char* source, char* dest, unsigned int length, unsigned int key);

#endif // PG_CLIENT_PROJECTG_JRENCRYPT_H
