#include "ActivationCode.h"

 bool ActivationCodeGen(char chMacAddr[12],unsigned long* ulActivationCode)
{
	BYTE abyMACAddress[6];
	DWORD dwDELTA = 0x9E3779B9;
	DWORD dwSum = 0;
	DWORD dwSecretKey[4] = { 5,7,100,200 };
	DWORD dwPlaintText[2];
	DWORD dwY, dwZ;
	DWORD dwLimit;
	UINT nlPassword;
	DWORD dwCipherText[2];

	for (int i = 0; i < 6; i++)
	{
		char chAddr[2];

		chAddr[0] = chMacAddr[i*2];
		chAddr[1] = chMacAddr[i*2 + 1]; 

		if(!Char2Hex(chAddr, &abyMACAddress[i]))
			return false;
	}

	dwPlaintText[0] = 0xAB << 24 | abyMACAddress[0]<<16 | abyMACAddress[1] << 8 | abyMACAddress[2];
	dwPlaintText[1] = abyMACAddress[3]<<24 | abyMACAddress[4]<<16 | abyMACAddress[5] << 8 | 0xCD;

	dwY = dwPlaintText[0];
	dwZ = dwPlaintText[1];

	dwLimit = dwDELTA * 32;
	
	while(dwSum != dwLimit)
	{
		dwY = dwY + ((dwZ<<4)^(dwZ>>5)) + (dwZ^dwSum) + dwSecretKey[dwSum & 3];
		dwSum = dwSum + dwDELTA;
		dwZ = dwZ + ((dwY << 4)^(dwY >> 5)) + (dwY^dwSum) + dwSecretKey[(dwSum>>11) & 3];
	};
	dwCipherText[0]= dwY; 
	dwCipherText[1] = dwZ;
	nlPassword = dwY^dwZ;
	
	*ulActivationCode = nlPassword;
	return true;
}

bool Char2Hex(char chMacString[2], BYTE* byHexValue)
{

	BYTE C;
	BYTE Hex;
	*byHexValue = 0;

	C = chMacString[0];
	if (!((C >= '0'&&C <= '9') || (C >= 'a'&&C <= 'f') || (C >= 'A'&&C <= 'F')))
		return false;
	if (C >= '0' && C <= '9')
		Hex = C - '0';
	else if (C >= 'A' && C <= 'Z')
		Hex = C - 'A' + 10;
	else if (C >= 'a' && C <= 'z')
		Hex = C - 'a' + 10;
	*byHexValue = Hex << 4;

	C = chMacString[1];
	if (!((C >= '0'&&C <= '9') || (C >= 'a'&&C <= 'f') || (C >= 'A'&&C <= 'F')))
		return false;
	if (C >= '0' && C <= '9')
		Hex = C - '0';
	else if (C >= 'A' && C <= 'Z')
		Hex = C - 'A' + 10;
	else if (C >= 'a' && C <= 'z')
		Hex = C - 'a' + 10;
	*byHexValue += Hex;

	return true;
}
