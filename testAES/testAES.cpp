// testAES.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "AES.h"
#include <cstring>
#include <iostream>
#include "base64.h"
#include <Windows.h>



const char g_key[17] = "V9z8bkiQ21wWttWg";
const char g_iv[17] = "PTfwSA1aNPgzlZfN";
string EncryptionAES(const string& strSrc, const char* _key, const char* _iv) //AES加密
{
	size_t length = strSrc.length();
	int block_num = length / BLOCK_SIZE + 1;
	//明文
	char* szDataIn = new char[block_num * BLOCK_SIZE + 1];
	memset(szDataIn, 0x00, block_num * BLOCK_SIZE + 1);
	strcpy_s(szDataIn, block_num * BLOCK_SIZE + 1, strSrc.c_str());

	//进行PKCS7Padding填充。
	int k = length % BLOCK_SIZE;
	int j = length / BLOCK_SIZE;
	int padding = BLOCK_SIZE - k;
	for (int i = 0; i < padding; i++)
	{
		szDataIn[j * BLOCK_SIZE + k + i] = padding;
	}
	szDataIn[block_num * BLOCK_SIZE] = '\0';

	//加密后的密文
	char* szDataOut = new char[block_num * BLOCK_SIZE + 1];
	memset(szDataOut, 0, block_num * BLOCK_SIZE + 1);

	//进行进行AES的CBC模式加密
	AES aes;
	aes.MakeKey(_key, _iv, 16, 16);
	aes.Encrypt(szDataIn, szDataOut, block_num * BLOCK_SIZE, AES::CBC);
	char temp[1024] = { 0 };
	int n = base64_encode(temp, szDataOut, block_num * BLOCK_SIZE);
	string str = temp;
	delete[] szDataIn;
	delete[] szDataOut;
	return str;
}
string DecryptionAES(const string& strSrc, const char* _key, const char* _iv) //AES解密
{
	char temp[1024] = { 0 };
	int n = base64_decode(temp, strSrc.c_str(), strSrc.length());
	string strData(temp);
	size_t length = strData.length();
	//密文
	char* szDataIn = new char[length + 1];
	memcpy(szDataIn, strData.c_str(), length + 1);
	//明文
	char* szDataOut = new char[length + 1];
	memcpy(szDataOut, strData.c_str(), length + 1);

	//进行AES的CBC模式解密
	AES aes;
	aes.MakeKey(_key, _iv, 16, 16);
	aes.Decrypt(szDataIn, szDataOut, length, AES::CBC);

	//去PKCS7Padding填充
	if (0x00 < szDataOut[length - 1] <= 0x16)
	{
		int tmp = szDataOut[length - 1];
		for (int i = length - 1; i >= length - tmp; i--)
		{
			if (szDataOut[i] != tmp)
			{
				memset(szDataOut, 0, length);
				cout << "去填充失败！解密出错！！" << endl;
				break;
			}
			else
				szDataOut[i] = 0;
		}
	}
	string strDest(szDataOut);
	delete[] szDataIn;
	delete[] szDataOut;
	return strDest;
}

std::string string_To_UTF8(const std::string& str)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

	wchar_t* pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴 
	ZeroMemory(pwBuf, nwLen * 2 + 2);

	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char* pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);

	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr(pBuf);

	delete[]pwBuf;
	delete[]pBuf;

	pwBuf = NULL;
	pBuf = NULL;

	return retStr;
}

int main()
{
	{
		string str2 = "6oXSLfCpbacUKllvsOVuP9XZTNudfNqikVTPJpJ/QKlhc1D4PBaPaBgqdox/5EtXVBYOFJ0FjDYudgyfmSmQPA==";
		string str3 = DecryptionAES(str2, g_key, g_iv);
		cout << "解密后:" << str3 << endl << endl << endl << endl << endl;
	}


	string str1 = "qwertyuisfdlsajdxcvnkhsakfh1332487";
	cout << "加密前:" << str1 << endl;
	string str2 = EncryptionAES(str1, g_key, g_iv);
	cout << "加密后:" << str2 << endl;
	str2 = "6oXSLfCpbacUKllvsOVuP9XZTNudfNqikVTPJpJ/QKlhc1D4PBaPaBgqdox/5EtXVBYOFJ0FjDYudgyfmSmQPA==";
	string str3 = DecryptionAES(str2, g_key, g_iv);
	cout << "解密后:" << str3 << endl;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件

void print(unsigned char* state, int len)
{
	int i;
	for (i = 0; i < len; i++)
	{
		if (i % 16 == 0) printf("\n");
		//		printf("%s%X ",state[i]>15 ? "" : "0", state[i]);
		printf("%d  ", (int)(state[i] & 0xff));
	}
	printf("\n");
}

