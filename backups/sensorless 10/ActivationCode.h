#include "minwindef.h"

bool Char2Hex(char chMacString[2], BYTE* byHexValue);
bool ActivationCodeGen(char chMacAddr[12], unsigned long* ulActivationCode);

//chMacAddr[12]: Mac字符数组，如 00 80 F4 0B 8F 01，不区分大小写
//ulActivationCode：计算出的长整数激活码，如3923652430
//return：0；非法的Mac地址（不是字符0-F），1:无错误
