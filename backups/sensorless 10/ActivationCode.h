#include "minwindef.h"

bool Char2Hex(char chMacString[2], BYTE* byHexValue);
bool ActivationCodeGen(char chMacAddr[12], unsigned long* ulActivationCode);

//chMacAddr[12]: Mac�ַ����飬�� 00 80 F4 0B 8F 01�������ִ�Сд
//ulActivationCode��������ĳ����������룬��3923652430
//return��0���Ƿ���Mac��ַ�������ַ�0-F����1:�޴���
