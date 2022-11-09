#include "secrets.hpp"

#include <jni.h>

#include "sha256.hpp"
#include "sha256.cpp"

/* Copyright (c) 2020-present Klaxit SAS
*
* Permission is hereby granted, free of charge, to any person
* obtaining a copy of this software and associated documentation
* files (the "Software"), to deal in the Software without
* restriction, including without limitation the rights to use,
* copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following
* conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
* HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*/

char *customDecode(char *str) {
    /* Add your own logic here
    * To improve your key security you can encode it before to integrate it in the app.
    * And then decode it with your own logic in this function.
    */
    return str;
}

jstring getOriginalKey(
        char *obfuscatedSecret,
        int obfuscatedSecretSize,
        jstring obfuscatingJStr,
        JNIEnv *pEnv) {

    // Get the obfuscating string SHA256 as the obfuscator
    const char *obfuscatingStr = pEnv->GetStringUTFChars(obfuscatingJStr, NULL);
    char buffer[2 * SHA256::DIGEST_SIZE + 1];

    sha256(obfuscatingStr, buffer);
    const char *obfuscator = buffer;

    // Apply a XOR between the obfuscated key and the obfuscating string to get original string
    char out[obfuscatedSecretSize + 1];
    for (int i = 0; i < obfuscatedSecretSize; i++) {
        out[i] = obfuscatedSecret[i] ^ obfuscator[i % strlen(obfuscator)];
    }

    // Add string terminal delimiter
    out[obfuscatedSecretSize] = 0x0;

    // (Optional) To improve key security
    return pEnv->NewStringUTF(customDecode(out));
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_greatfire_wikiunblocked_fdroid_Secrets_getdotAddr(
        JNIEnv *pEnv,
        jobject pThis,
        jstring packageName) {
    char obfuscatedSecret[] = {  };
    return getOriginalKey(obfuscatedSecret, sizeof(obfuscatedSecret), packageName, pEnv);
}
extern "C"
JNIEXPORT jstring JNICALL
Java_org_greatfire_wikiunblocked_fdroid_Secrets_getdohUrl(
        JNIEnv* pEnv,
        jobject pThis,
        jstring packageName) {
     char obfuscatedSecret[] = { 0x5f, 0x4d, 0x15, 0x45, 0x47, 0xe, 0x16, 0x4e, 0x5d, 0x5d, 0x4f, 0xf, 0x5e, 0x58, 0x2, 0x1e, 0x1, 0x5b, 0xe, 0x4d, 0x1, 0x54, 0xe, 0x52, 0x42, 0x52, 0x4b, 0x5c, 0x5e, 0x40, 0x1b, 0x53, 0x5e, 0x5a, 0x49, 0x51, 0x56, 0x11, 0x4f, 0x44, 0x47, 0x1, 0x42, 0x18 };
     return getOriginalKey(obfuscatedSecret, sizeof(obfuscatedSecret), packageName, pEnv);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_greatfire_wikiunblocked_fdroid_Secrets_gethystCert(
        JNIEnv* pEnv,
        jobject pThis,
        jstring packageName) {
     char obfuscatedSecret[] = { 0x7a, 0x70, 0x28, 0x70, 0x4e, 0x5e, 0x7a, 0x22, 0x71, 0x48, 0x54, 0x1, 0x73, 0x43, 0x2a, 0x72, 0x23, 0x50, 0x28, 0x6a, 0x24, 0x7b, 0x15, 0x76, 0x1b, 0x5a, 0x54, 0x7c, 0x1b, 0x2, 0x43, 0x46, 0x4b, 0x67, 0x3c, 0x54, 0x6b, 0x2e, 0x8, 0x1a, 0x53, 0x53, 0x69, 0x16, 0x70, 0x66, 0x38, 0x7b, 0x7f, 0xc, 0x6e, 0x28, 0xc, 0x41, 0x2, 0x28, 0x23, 0x66, 0x26, 0x2e, 0x75, 0x34, 0x23, 0x44, 0x1b, 0x5f, 0x1b, 0x70, 0x51, 0x79, 0x7b, 0x16, 0x77, 0x73, 0x4, 0x33, 0x77, 0x77, 0xb, 0x7d, 0x34, 0x55, 0x36, 0x4c, 0xf, 0x68, 0x3a, 0x79, 0x0, 0x7e, 0x21, 0x6a, 0x5c, 0x57, 0x58, 0x66, 0x42, 0x55, 0x55, 0x77, 0x4c, 0x38, 0x35, 0x0, 0x2, 0x2d, 0x75, 0x2f, 0x76, 0x7a, 0x32, 0x5e, 0x43, 0x28, 0x75, 0x38, 0x20, 0x61, 0x30, 0x37, 0x2e, 0x73, 0x20, 0x24, 0x4d, 0x3c, 0x50, 0x2, 0x76, 0x15, 0x3, 0x61, 0x72, 0x43, 0x5a, 0xc, 0x8, 0x47, 0x57, 0x21, 0xb, 0x5e, 0x3a, 0x67, 0x15, 0x50, 0x2a, 0x7e, 0x2b, 0x2, 0x38, 0x6b, 0x6a, 0x5b, 0x4, 0x51, 0x72, 0x7d, 0x6c, 0x0, 0x63, 0x41, 0x4, 0x58, 0x7e, 0x11, 0x38, 0x76, 0x59, 0x1c, 0x7d, 0x35, 0x75, 0x41, 0x23, 0x56, 0x7a, 0x35, 0x76, 0x20, 0x29, 0x7a, 0x2a, 0x21, 0x53, 0x45, 0x3a, 0x50, 0x61, 0x1c, 0x6, 0x70, 0x75, 0x43, 0x4d, 0x6c, 0x6, 0x5, 0x78, 0x3, 0x64, 0x74, 0x42, 0x5, 0x5f, 0xc, 0x16, 0x52, 0x25, 0xe, 0xb, 0x61, 0x32, 0x45, 0x5, 0x78, 0x76, 0x79, 0x56, 0x62, 0x68, 0x69, 0x59, 0x52, 0x58, 0x75, 0x28, 0x6c, 0x8, 0x30, 0x14, 0x57, 0x5f, 0x22, 0x43, 0x3b, 0x77, 0x5c, 0x16, 0x79, 0x5c, 0x0, 0x7a, 0x2c, 0xe, 0x7e, 0x16, 0x2b, 0x36, 0x7e, 0x50, 0x2f, 0x73, 0x20, 0x52, 0x7d, 0x63, 0x68, 0x54, 0x19, 0x63, 0x5c, 0x5a, 0x2f, 0x7d, 0x48, 0x7c, 0x11, 0x7f, 0x60, 0x2a, 0x3, 0x2f, 0x73, 0x24, 0x8, 0x2b, 0x66, 0x33, 0x6, 0x67, 0x5d, 0x24, 0x17, 0x7d, 0x61, 0x1, 0x47, 0x79, 0x76, 0x3f, 0x71, 0x6e, 0x33, 0x33, 0x7e, 0x77, 0x1c, 0x66, 0x15, 0x55, 0x5, 0x2f, 0x5d, 0x57, 0xd, 0x65, 0x6, 0x3e, 0x70, 0x37, 0x54, 0x38, 0x60, 0x1b, 0x14, 0x54, 0x22, 0x53, 0x5f, 0x55, 0x57, 0x30, 0x52, 0x18, 0x65, 0x9, 0x24, 0x48, 0x79, 0x5f, 0x27, 0x5d, 0x76, 0x4, 0x7e, 0x34, 0x75, 0x20, 0x4b, 0x28, 0x7b, 0x3a, 0x7d, 0x5a, 0x55, 0x33, 0x7a, 0x44, 0x7e, 0x6d, 0x72, 0x48, 0x55, 0x1f, 0x0, 0x4b, 0x0, 0x50, 0x7b, 0x5a, 0x6, 0x73, 0x20, 0x5b, 0x62, 0x52, 0x63, 0x58, 0x7, 0x59, 0x37, 0x11, 0x7e, 0x24, 0x57, 0x8, 0x65, 0x24, 0x5b, 0x42, 0x3c, 0x35, 0x4b, 0x5c, 0x72, 0x35, 0x70, 0x4c, 0x18, 0x74, 0x22, 0x8, 0x75, 0x74, 0x57, 0x67, 0x71, 0x22, 0x47, 0x15, 0x58, 0x3, 0x6f, 0x11, 0x58, 0x38, 0x6b, 0x7a, 0x7, 0x2f, 0x70, 0x7e, 0x59, 0x57, 0x65, 0x73, 0x43, 0x2b, 0x6d, 0x7a, 0x1b, 0x0, 0x4c, 0x7, 0x17, 0x52, 0x53, 0x7a, 0x5f, 0x3, 0x72, 0x75, 0xc, 0x61, 0x52, 0x36, 0x5b, 0x5, 0xb, 0x34, 0x42, 0x2a, 0x27, 0x6, 0xf, 0x30, 0x74, 0xe, 0x4c, 0x38, 0x62, 0x4c, 0x5f, 0x15, 0x2a, 0x64, 0x71, 0x76, 0x27, 0x53, 0x7d, 0x14, 0x74, 0x33, 0x6e, 0x2b, 0x73, 0xa, 0x68, 0x2b, 0x5b, 0x46, 0x54, 0x28, 0x79, 0x61, 0x76, 0x77, 0x72, 0x60, 0x76, 0x22, 0x52, 0x5f, 0x25, 0x32, 0x74, 0x76, 0x27, 0x73, 0x20, 0x6d, 0x58, 0x22, 0x56, 0x53, 0x24, 0x76, 0x20, 0x2a, 0x53, 0x4a, 0xb, 0x2f, 0x74, 0x5a, 0x9, 0x66, 0x32, 0x15, 0x7b, 0x1c, 0x51, 0x4a, 0x1e, 0x42, 0x59, 0x6a, 0x4d, 0x7b, 0x59, 0x44, 0x10, 0x3, 0x4c, 0x6, 0x52, 0x2a, 0x7c, 0xf, 0x5b, 0x2e, 0x66, 0x4d, 0x79, 0x71, 0x76, 0x14, 0xe, 0x5c, 0x71, 0x72, 0x1f, 0x7e, 0xe, 0x32, 0x3, 0x6e, 0x52, 0x29, 0x70, 0x68, 0x30, 0x57, 0x2c, 0x51, 0x61, 0x34, 0x5, 0x6c, 0x24, 0x0, 0x22, 0x56, 0x74, 0x37, 0x36, 0x30, 0x4d, 0x1, 0x1, 0x52, 0x24, 0x26, 0x60, 0x63, 0x77, 0x52, 0x3, 0x41, 0x6, 0x52, 0x53, 0x1c, 0x19, 0x61, 0x9, 0x75, 0x51, 0x33, 0x6, 0x4, 0x72, 0x2, 0x17, 0x16, 0x48, 0x55, 0x60, 0x74, 0x6, 0x33, 0x5e, 0x1f, 0x6b, 0x40, 0x6, 0x50, 0x6d, 0x25, 0x47, 0x7d, 0x17, 0x17, 0x7e, 0xa, 0x5, 0x78, 0x2, 0x5c, 0x54, 0xf, 0x3, 0x1f, 0x21, 0x53, 0x2, 0x32, 0x2, 0x9, 0x3, 0xb, 0x7c, 0x2a, 0x12, 0x66, 0x22, 0x2a, 0x65, 0x5d, 0x7f, 0x1b, 0x76, 0x53, 0x51, 0x43, 0x58, 0x7, 0x1e, 0x42, 0x3e, 0x56, 0x57, 0x2d, 0x5f, 0x15, 0x52, 0x13, 0x48, 0x32, 0x62, 0x56, 0x1, 0x67, 0x7c, 0x53, 0x41, 0x5a, 0x1, 0x50, 0x3, 0x1a, 0x61, 0x2d, 0x77, 0x57, 0x8, 0x2a, 0x5a, 0x47, 0x12, 0x63, 0x23, 0x46, 0x63, 0xb, 0x2, 0x71, 0x2, 0x6d, 0x20, 0xa, 0x2, 0xf, 0x37, 0x2e, 0x5e, 0x2a, 0x12, 0x5e, 0x52, 0x38, 0x42, 0x7f, 0x54, 0x10, 0x2, 0x7a, 0x5b, 0x4b, 0x2e, 0x58, 0x61, 0x19, 0xa, 0x56, 0x55, 0x20, 0x7b, 0x2d, 0x1, 0x15, 0x48, 0x5d, 0x7e, 0x30, 0x62, 0x68, 0x7, 0x3e, 0x9, 0x3, 0x75, 0x79, 0x0, 0x5e, 0xf, 0xe, 0x7b, 0x72, 0x0, 0x55, 0x52, 0x68, 0x11, 0x63, 0x38, 0x4c, 0x43, 0x52, 0x7f, 0x4e, 0xc, 0x64, 0x51, 0x3e, 0x74, 0x4, 0x2d, 0x4, 0x53, 0x5a, 0x2e, 0x56, 0x52, 0x56, 0x3, 0xe, 0x4c, 0x51, 0x6f, 0x76, 0x61, 0x4c, 0x34, 0x42, 0x65, 0x59, 0x4a, 0x59, 0x4, 0x52, 0x57, 0x32, 0x5f, 0x4a, 0xe, 0x36, 0x43, 0x10, 0x45, 0x43, 0x46, 0x35, 0x5e, 0x3, 0x72, 0x5b, 0x40, 0x49, 0x41, 0xa, 0x43, 0x6d, 0x4, 0x12, 0x58, 0x3, 0x1, 0x9, 0x2d, 0x52, 0x62, 0x3b, 0x54, 0x4, 0x30, 0x1b, 0x2b, 0x55, 0x78, 0x38, 0x29, 0x9, 0x71, 0x51, 0x33, 0x53, 0x34, 0x49, 0x44, 0x5b, 0x43, 0x29, 0x6f, 0x47, 0x6d, 0x40, 0x19, 0x6a, 0x0, 0x40, 0x5, 0x1e, 0x5f, 0x31, 0x67, 0x2e, 0x6e, 0x3, 0x49, 0x3d, 0x59, 0x24, 0xa, 0x3, 0x6f, 0x55, 0x77, 0x2, 0x7b, 0x5, 0x63, 0x5a, 0x5d, 0x3f, 0x7e, 0x7a, 0x51, 0x32, 0x73, 0x79, 0x7, 0x67, 0x2f, 0x51, 0x62, 0x5, 0x45, 0x7, 0x2f, 0x7e, 0x55, 0x8, 0x79, 0x13, 0xf, 0x11, 0x6f, 0x48, 0x30, 0x1c, 0x2f, 0x36, 0x66, 0x1c, 0xd, 0x39, 0x45, 0x73, 0x6d, 0x43, 0xf, 0x5e, 0x7b, 0x50, 0x4, 0x74, 0x18, 0x4c, 0x7a, 0x16, 0x18, 0x34, 0x1, 0x4, 0x74, 0x9, 0x56, 0x5e, 0x5c, 0x23, 0xb, 0x7a, 0x7b, 0x4c, 0x56, 0x54, 0xe, 0x35, 0x71, 0x5d, 0x6, 0x23, 0x44, 0x67, 0x32, 0x7f, 0x58, 0x5e, 0x1, 0x39, 0x76, 0x66, 0x25, 0x67, 0x2a, 0x51, 0x7b, 0x14, 0x9, 0x38, 0x44, 0x3b, 0xd, 0x72, 0x14, 0x10, 0x42, 0x6e, 0xa, 0x25, 0x6d, 0x56, 0x61, 0x63, 0x35, 0x43, 0x68, 0x57, 0xd, 0x68, 0x6, 0x4f, 0x7e, 0x34, 0x43, 0xc, 0x75, 0x4e, 0xb, 0x4d, 0x51, 0x53, 0x42, 0x24, 0x40, 0x74, 0x54, 0x77, 0x48, 0x64, 0x70, 0x8, 0x43, 0x68, 0x2e, 0x30, 0x54, 0x7a, 0x2b, 0x9, 0x38, 0x7, 0x45, 0xa, 0x5b, 0x57, 0x48, 0xc, 0x30, 0x6, 0x3, 0x51, 0xf, 0xb, 0x55, 0x34, 0x5a, 0x66, 0x2c, 0x26, 0x72, 0x66, 0x78, 0x23, 0x5a, 0x4, 0x61, 0x4e, 0x30, 0x4a, 0x73, 0x7a, 0x24, 0x55, 0x7a, 0x35, 0x1c, 0x2a, 0x66, 0x59, 0x7a, 0x24, 0x54, 0x5a, 0x76, 0x72, 0x76, 0x2b, 0x7b, 0x71, 0x54, 0x64, 0x47, 0x74, 0x50, 0x3f, 0x71, 0x6e, 0x30, 0x52, 0x61, 0x73, 0x35, 0x78, 0x4e, 0x76, 0x76, 0x6, 0x46, 0x76, 0x4, 0x71, 0x23, 0x4b, 0x40, 0x28, 0x24, 0x23, 0x73, 0x22, 0x6, 0x75, 0x2, 0x2c, 0x65, 0x7f, 0x68, 0x55, 0x70, 0x72, 0x53, 0x68, 0x34, 0x49, 0x53, 0x72, 0x21, 0x0, 0x65, 0x30, 0x5c, 0x4e, 0x59, 0x13, 0xb, 0x33, 0x41, 0x2d, 0x63, 0x54, 0x1c, 0x51, 0x7d, 0x47, 0x55, 0x4d, 0x63, 0x78, 0x66, 0x51, 0x60, 0x4f, 0x26, 0x33, 0x6c, 0x78, 0x2f, 0x5f, 0x3b, 0x7d, 0x5f, 0x17, 0x52, 0x7a, 0x22, 0x65, 0x24, 0x28, 0x75, 0x30, 0x27, 0x26, 0x50, 0x4, 0x25, 0x75, 0x24, 0x23, 0xa, 0x0, 0x58, 0x9, 0x6, 0x5b, 0x1, 0x7c, 0x34, 0x47, 0x4b, 0x1a, 0x2a, 0x7c, 0x67, 0x8, 0x63, 0x29, 0x1b, 0x2c, 0x7d, 0x3c, 0x60, 0x27, 0x47, 0x6a, 0x51, 0x3c, 0x48, 0x6, 0x5c, 0x4f, 0x4, 0x78, 0x5d, 0x22, 0x78, 0x7b, 0x29, 0x2c, 0xc, 0x74, 0x2f, 0x44, 0x2a, 0x57, 0x46, 0xd, 0x41, 0x56, 0x1a, 0x7e, 0x37, 0x8, 0x4d, 0x4e, 0x7, 0xa, 0x7e, 0x36, 0x5b, 0x18, 0x34, 0x26, 0x42, 0x74, 0x72, 0x2, 0x54, 0x75, 0x7e, 0x6f, 0x2d, 0x5d, 0x60, 0x0, 0x51, 0x54, 0x6e, 0x4c, 0x41, 0xb, 0x58, 0x4d, 0x70, 0x2b, 0x63, 0x1, 0x5e, 0x1, 0x4e, 0x10, 0x79, 0x6, 0x67, 0x59, 0x40, 0x43, 0x59, 0x11, 0x78, 0x70, 0x2c, 0x16, 0x65, 0x7d, 0x57, 0x53, 0x19, 0x47, 0x5e, 0x50, 0x41, 0x4, 0x27, 0x3, 0x24, 0xb, 0x51, 0x20, 0x1f, 0x52, 0x58, 0x22, 0x1, 0x65, 0x15, 0x51, 0x7d, 0x50, 0x6d, 0x2e, 0x62, 0x44, 0x6c, 0xe, 0x1b, 0x64, 0x42, 0x51, 0x54, 0x42, 0x7a, 0x2a, 0x45, 0x26, 0x4e, 0x57, 0x14, 0x9, 0x5f, 0x32, 0x2, 0x59, 0x75, 0xd, 0x6d, 0x48, 0x55, 0x6d, 0x5f, 0x43, 0x76, 0x28, 0x1e, 0x75, 0x30, 0x53, 0x66, 0x48, 0x21, 0x67, 0x38, 0x65, 0x59, 0x53, 0x5a, 0x7, 0xb, 0x57, 0x29, 0x1, 0x44, 0x13, 0x10, 0x36, 0x4d, 0x12, 0x25, 0x6d, 0x8, 0x1, 0x65, 0x4e, 0x6b, 0x25, 0x5c, 0x5c, 0x78, 0x6e, 0x5, 0x7, 0x50, 0x61, 0x3, 0x5b, 0x4c, 0x24, 0x7f, 0x57, 0x4f, 0x59, 0x4f, 0x49, 0x54, 0x2c, 0x7d, 0x67, 0x63, 0x31, 0x13, 0x63, 0x57, 0x3, 0x44, 0x0, 0x41, 0x36, 0x63, 0x6a, 0x49, 0x13, 0x77, 0x45, 0x37, 0x7c, 0x26, 0x61, 0x7c, 0x2c, 0x49, 0x5b, 0x35, 0x51, 0x4, 0xa, 0x56, 0x31, 0x5e, 0x32, 0x6f, 0xd, 0x54, 0x42, 0x50, 0x20, 0x77, 0x4d, 0x77, 0xa, 0x42, 0x6e, 0x7f, 0x68, 0x2c, 0x67, 0x63, 0x4f, 0x20, 0x5e, 0x40, 0x48, 0x74, 0x33, 0x7b, 0x57, 0x9, 0x1f, 0x1e, 0x54, 0x47, 0x5, 0x78, 0x33, 0xf, 0x3, 0x70, 0x6c, 0x57, 0x7b, 0x0, 0x3e, 0x7c, 0x73, 0x14, 0x29, 0x7b, 0x19, 0x1, 0x76, 0x2e, 0x73, 0xe, 0xd, 0x47, 0x6d, 0x4, 0x58, 0xe, 0x5c, 0x7b, 0x18, 0x22, 0x28, 0x0, 0x57, 0x33, 0x55, 0x1f, 0xc, 0x65, 0x5f, 0x16, 0x29, 0x56, 0x43, 0x4e, 0x49, 0x12, 0x7, 0x46, 0x6, 0x37, 0x70, 0x1b, 0x30, 0x7, 0x33, 0x6, 0x8, 0x55, 0x10, 0x57, 0x4e, 0x4, 0x5e, 0x4, 0xe, 0x71, 0x7e, 0x43, 0x4, 0x77, 0x46, 0x73, 0x1, 0x63, 0x55, 0x24, 0x9, 0x5e, 0x2, 0xb, 0x79, 0x26, 0xc, 0x1c, 0x12, 0x4, 0x4e, 0x56, 0x0, 0x9, 0x1c, 0x74, 0x28, 0x27, 0x20, 0x50, 0x34, 0x38, 0x44, 0x27, 0x10, 0x1, 0x41, 0x4d, 0x26, 0x79, 0x42, 0x5a, 0xa, 0x4a, 0x48, 0x77, 0x71, 0x1, 0x74, 0x40, 0x31, 0x43, 0x3b, 0xe, 0x2f, 0xc, 0x35, 0x66, 0x2d, 0x1f, 0x60, 0x65, 0x2e, 0x49, 0x1f, 0x1c, 0x77, 0x78, 0x47, 0x63, 0xc, 0x73, 0x4c, 0x5b, 0x12, 0x42, 0x68, 0x17, 0x5, 0xa, 0x1f, 0x72, 0x23, 0x44, 0xd, 0x28, 0x7, 0x28, 0x54, 0x60, 0x3b, 0x11, 0x50, 0x67, 0x21, 0x35, 0x5f, 0x1f, 0x2e, 0x5a, 0x7b, 0x78, 0x56, 0x65, 0x50, 0x5f, 0x7d, 0x8, 0x74, 0x44, 0x65, 0x11, 0x7, 0x47, 0x29, 0x67, 0x5a, 0x7, 0x17, 0x77, 0x33, 0x5d, 0x5a, 0x44, 0x1c, 0x5b, 0x32, 0x71, 0x5d, 0xa, 0x1e, 0x5c, 0x49, 0x5d, 0x54, 0x61, 0x59, 0x7, 0xb, 0x44, 0x51, 0x34, 0x51, 0x37, 0x66, 0x75, 0x34, 0x76, 0x3, 0x0, 0x59, 0x28, 0x1c, 0x1c, 0x8, 0x33, 0x24, 0x67, 0xd, 0x12, 0x43, 0x11, 0xc, 0x43, 0xf, 0x6a, 0x17, 0x67, 0x63, 0x58, 0x68, 0x5c, 0xd };
     return getOriginalKey(obfuscatedSecret, sizeof(obfuscatedSecret), packageName, pEnv);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_greatfire_wikiunblocked_fdroid_Secrets_getdnsttdomain(
        JNIEnv* pEnv,
        jobject pThis,
        jstring packageName) {
     char obfuscatedSecret[] = { 0x43, 0x17, 0x16, 0x5c, 0x5a, 0x50, 0x56, 0x16, 0x52, 0x40, 0x5a, 0x13, 0x55, 0x5c, 0x17, 0x53, 0x3, 0x45, 0x13, 0x41, 0x4b, 0x51, 0xd, 0x5e };
     return getOriginalKey(obfuscatedSecret, sizeof(obfuscatedSecret), packageName, pEnv);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_greatfire_wikiunblocked_fdroid_Secrets_getdnsttpath(
        JNIEnv* pEnv,
        jobject pThis,
        jstring packageName) {
     char obfuscatedSecret[] = { 0x18, 0x4e, 0x8, 0x5e, 0x5d, 0x41, 0x57, 0x3, 0x5c, 0x5d, 0x56, 0xd, 0x57, 0x50, 0x4c, 0x53, 0xd, 0x59, 0x7, 0x51, 0x2, 0x1c, 0x8, 0x40, 0x5f, 0x59 };
     return getOriginalKey(obfuscatedSecret, sizeof(obfuscatedSecret), packageName, pEnv);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_greatfire_wikiunblocked_fdroid_Secrets_getdnsttkey(
        JNIEnv* pEnv,
        jobject pThis,
        jstring packageName) {
     char obfuscatedSecret[] = { 0x56, 0x58, 0x56, 0x51, 0x7, 0x6, 0xc, 0x55, 0x8, 0x56, 0x6, 0x51, 0x3, 0x7, 0x1, 0x54, 0x1, 0xf, 0x2, 0xb, 0x5c, 0x50, 0x1, 0x55, 0x4, 0x56, 0x53, 0x9, 0x2, 0x5, 0x3, 0x53, 0x0, 0x53, 0x5, 0x3, 0x9, 0x5a, 0x1, 0x3, 0x2, 0x51, 0x51, 0x59, 0x57, 0x0, 0x55, 0x0, 0x56, 0x53, 0x7, 0x2, 0x5d, 0x5, 0x50, 0x5f, 0x52, 0x51, 0x7, 0x51, 0xf, 0x4, 0x50, 0x6 };
     return getOriginalKey(obfuscatedSecret, sizeof(obfuscatedSecret), packageName, pEnv);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_org_greatfire_wikiunblocked_fdroid_Secrets_getdefProxy(
        JNIEnv* pEnv,
        jobject pThis,
        jstring packageName) {
     char obfuscatedSecret[] = { 0x5f, 0x4d, 0x15, 0x45, 0x47, 0xe, 0x16, 0x4e, 0x47, 0x5b, 0x5e, 0xf, 0x1c, 0x51, 0x13, 0x5f, 0x1, 0x5f, 0x3, 0x5d, 0x9, 0x46, 0x4c, 0x50, 0x5f, 0x5a, 0x49, 0x4f, 0x59, 0x58, 0x5c, 0x40, 0x54, 0x53, 0xf, 0x54, 0x14, 0x11, 0x11, 0xf, 0x1d, 0x4b, 0x69, 0x53, 0x5c, 0x5f, 0x38, 0x3, 0x5c, 0xb, 0x79, 0xb, 0x25, 0x43, 0x0, 0x31, 0x34, 0x7, 0x39, 0xb, 0x6, 0x12, 0x0, 0x1, 0x4f, 0xc, 0x2c, 0x61, 0x79, 0x43, 0x77, 0x35, 0x40, 0x42, 0x6f, 0x3e, 0x7c, 0x42, 0x2, 0x78, 0x38, 0x41, 0x2e, 0x70, 0xd, 0x3, 0x2c, 0x5e, 0x9, 0x58, 0x31, 0x6f, 0x9, 0x4a, 0x54, 0x67, 0x64, 0x4e, 0x2, 0x72, 0x50, 0xa, 0x38, 0x62, 0x5a, 0x12, 0x51, 0x27, 0x76, 0x58, 0x2e, 0x77, 0x66, 0xc, 0x6d, 0x30, 0x59, 0xa, 0x21, 0x57, 0x55, 0x5, 0x4d, 0x53, 0x7, 0x51, 0x4c, 0x2, 0x1, 0xa, 0x4f, 0x0, 0x0, 0xe, 0x1, 0x52, 0x8, 0xa, 0x1a, 0x4a, 0x5a, 0x4d, 0x10, 0x44, 0x7, 0x45, 0x8, 0x59, 0x5f, 0x1d, 0x4d, 0x2, 0x7, 0x5, 0x48, 0x9, 0x0, 0x7, 0x1b, 0x1, 0x7, 0x4, 0x48, 0x0, 0xc, 0x58, 0x51, 0x7, 0x1, 0x56, 0x3, 0x5e, 0x5b, 0x55, 0x7, 0x42, 0x9, 0x16, 0x64, 0x0, 0x55, 0x50, 0x0, 0x14, 0x56, 0x70, 0x16, 0x1, 0x52, 0x50, 0xd, 0x5c, 0x43, 0x5c, 0x18, 0x5c, 0x41, 0x40, 0x51, 0x8, 0x55, 0x5, 0x46, 0x9, 0x43, 0x41, 0x56, 0x7d, 0x17, 0x1b, 0x17, 0xa, 0x12, 0x41, 0x58, 0x1c, 0x1f, 0x40, 0xf, 0x53, 0x59, 0x1d, 0x50, 0x40, 0x5e, 0x54, 0xe, 0x57, 0x5d, 0xe, 0x16, 0x1b, 0x51, 0xb, 0x5d, 0x5b, 0x0, 0x3, 0x52, 0xe, 0x44, 0x2, 0x40, 0x9, 0x59, 0x5a, 0x26, 0x5f, 0x37, 0x11, 0xa, 0x6, 0xa, 0x7, 0x6, 0x6, 0x55, 0xc, 0x58, 0x51, 0x7, 0x19, 0x58, 0x52, 0x5, 0x51, 0x18, 0x52, 0x3, 0x3, 0x5, 0x1d, 0x5a, 0x51, 0x53, 0x8, 0x48, 0x2, 0x5a, 0x4, 0x6, 0x52, 0x52, 0x5a, 0x5, 0x52, 0xc, 0x51, 0x50, 0x1b, 0x10, 0x7, 0x4b, 0x10, 0x16, 0x45, 0x8, 0x4b, 0x1f, 0x50, 0x3, 0x5, 0x4f, 0x0, 0x4, 0x57, 0x1a, 0x50, 0x52, 0x4, 0x4f, 0x53, 0x56, 0xd, 0x52, 0x52, 0x7, 0x55, 0x50, 0xc, 0x5e, 0x5d, 0x5c, 0x57, 0x50, 0x1, 0x5b, 0x54, 0x9, 0x56, 0x6, 0x4b, 0x53, 0x7, 0x56, 0x53, 0x4f, 0x3, 0x50, 0xf, 0x3, 0x1f, 0x5a, 0x55, 0x2, 0x7, 0x4b, 0x8, 0x8, 0x4, 0x3, 0x55, 0x5, 0x55, 0x53, 0x54, 0x1, 0x3, 0x3, 0x19, 0x44, 0x56, 0x47, 0x4, 0x57, 0x5f, 0x0, 0x45, 0xe, 0x4c, 0x1b, 0x50, 0x53, 0x5, 0x4f, 0x57, 0x52, 0x3, 0x4d, 0x53, 0x1, 0x56, 0x4c, 0x6, 0x3, 0x3, 0x50, 0x3, 0x6, 0xc, 0xc, 0x5e, 0x59, 0x56, 0x8, 0x4, 0x56, 0x1, 0x1, 0x5, 0x5b, 0x53, 0x52, 0x15, 0x4, 0x1, 0x57, 0x50, 0x1d, 0x3, 0x57, 0xf, 0x56, 0x1e, 0xd, 0x56, 0x3, 0x7, 0x4b, 0x5, 0x0, 0x55, 0x54, 0x50, 0x6, 0x6, 0x5, 0x0, 0xd, 0x56, 0x0 };
     return getOriginalKey(obfuscatedSecret, sizeof(obfuscatedSecret), packageName, pEnv);
}
