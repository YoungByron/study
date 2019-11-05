#include "stdafx.h"
#include "TCalcFuncSets.h"
#define WM_TRADEMSG WM_USER+1
#include <stdio.h>  
#include<math.h>
#include <stdarg.h>  
#include<string>
#include<sstream>
//#include "winsock.h"
#include "mysql.h"//ͷ�ļ�˳���ܵߵ�
MYSQL mysql; //���ݿ����Ӿ��
BOOL bConnectMysql = FALSE;

void TraceEx(const wchar_t *strOutputString, ...)  
{  
	va_list vlArgs = NULL;  
	va_start(vlArgs, strOutputString);  
	size_t nLen = _vscwprintf(strOutputString, vlArgs) + 1;  
	wchar_t *strBuffer = new wchar_t[nLen];  
	_vsnwprintf_s(strBuffer, nLen, nLen, strOutputString, vlArgs);  
	va_end(vlArgs);  
	OutputDebugStringW(strBuffer);  
	delete [] strBuffer;  
}  

void TraceEx(const char *strOutputString, ...)  
{  
	va_list vlArgs = NULL;  
	va_start(vlArgs, strOutputString);  
	size_t nLen = _vscprintf(strOutputString, vlArgs) + 1;  
	char *strBuffer = new char[nLen];  
	_vsnprintf_s(strBuffer, nLen, nLen, strOutputString, vlArgs);  
	va_end(vlArgs);  
	OutputDebugStringA(strBuffer);  
	delete [] strBuffer;  
}  
//test  
// 	TraceEx(L"DEBUG_INFO | %d %s\r\n", 1234, L"this is test by kandy(wchar_t*)");  
// 	TraceEx("DEBUG_INFO | %d %s\r\n", 5678, "this is test by kandy(char*)");  
// 	
double round(double r)
{
	return (r > 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
}
void OnScreenfont(char* szBuffer, int x, int y, COLORREF newClr)
{
	HFONT		font;
	HDC			hdc;
	HWND		hwnd;
	COLORREF	clr;
	//TCHAR		szBuffer[] = TEXT("CPU�¶ȳ���...>89��C");

	//��ȡ���洰�ھ��
	hwnd = GetDesktopWindow();
	//��ȡ���洰��DC
	hdc = GetWindowDC(hwnd);
	//����ɫ͸��
	SetBkMode(hdc, TRANSPARENT);

	font=CreateFont(
		120,48,0,0, FW_BOLD,0,0,0, ANSI_CHARSET ,OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,NULL,(LPCTSTR)"΢���ź�");
	SelectObject(hdc,font);

	//������ɫ���������
	clr = SetTextColor(hdc, newClr);
	TextOut(hdc, x, y, szBuffer, strlen(szBuffer));

	//��ԭ��ɫ
	SetTextColor(hdc, clr);

	//�ͷž��DC
	DeleteObject(font); 
	ReleaseDC(hwnd, hdc);
}


//1������mysql
int ConnectMysql(LPCTSTR sip,LPCTSTR suser,LPCTSTR spass,LPCTSTR sdbname,int sport)
{
	if( mysql_init(&mysql) == NULL )
	{
		::OutputDebugString("��ʼ��mysqlʧ��!");
		return FALSE;
	}
	//ʵ��mysql_real_connect(&mysql,"127.0.0.1","root","","zkeco_db",17770,NULL,0)
	if(!mysql_real_connect(&mysql,(const char *)sip,(const char *)suser,(const char *)spass,(const char *)sdbname,sport,NULL,0))
	{
		//TRACE("����mysqlʧ��:%s",mysql_error(&mysql)); 
		return FALSE;
	}
	else
	{
		//Trace2Logger("���ݿ����ӳɹ�");
		return TRUE;
	}
}

BOOL bLastData = FALSE;
void ShowTable(float* fOut, float a1,float a2,float a3,float a4 )
{
	//��ʾ����
	MYSQL_RES *result=NULL;
	//if(0==mysql_query(&mysql,"SELECT * FROM stock.399006_1min LIMIT 0, 100"))
/*
SELECT * FROM stock.399006_1min_result where 
min1a=floor(round(-0.12,2)*2)/2 and 
min2a=floor(round(-0.22,2)*2)/2 and 
min3a=floor(round(-0.32,2)*2)/2 and 
min4a=floor(round(-0.49,2)*2)/2;
 **/

	//OutputDebugString("111*********");
	TCHAR strSQL[1024] = {0};
	sprintf(strSQL, "SELECT * FROM stock.399006_1min_result where min1a=floor(round(%.2f,2)*2)/2 and min2a=floor(round(%.2f,2)*2)/2 and min3a=floor(round(%.2f,2)*2)/2 and min4a=floor(round(%.2f,2)*2)/2", a1, a2, a3, a4);
	if (bLastData)
	{
		::OutputDebugString("\r\n");
		::OutputDebugString(strSQL);
		::OutputDebugString("\r\n");
	}
	if(0==mysql_query(&mysql,strSQL))
	{
		//OutputDebugString("222");
		std::string loginfo;
		//loginfo="mysql_query() select data succeed";
		//MessageBox(loginfo);
		//һ����ȡ�����ݼ�
		result=mysql_store_result(&mysql);

		//ȡ�ò���ӡ����
		LONG64 rowcount=mysql_num_rows(result);
		/*loginfo.Format("row count=%d",rowcount);
		MessageBox(loginfo);       */

		//ȡ�ò���ӡ���ֶε�����
		unsigned int fieldcount=mysql_num_fields(result);
		MYSQL_FIELD *field=NULL;
		//loginfo=loginfo+"\r\n";
		//for(unsigned int i=0;i<fieldcount;i++)
		//{
		//	field=mysql_fetch_field_direct(result,i);
		//	loginfo=loginfo+(field->name)+",";
		//}
		//loginfo=loginfo+"\r\n";
		//::OutputDebugString(loginfo.c_str());       

		//OutputDebugString("333");
		//��ӡ����
		MYSQL_ROW row=NULL;
		row=mysql_fetch_row(result);
		//row=NULL;
		loginfo="\r\n[TDX]DB==> ";
		while(NULL!=row)
		{
			for(int i=0; i<fieldcount;i++)
			{
				//loginfo=loginfo+(row[i])+",";

				field=mysql_fetch_field_direct(result,i);
				if (strstr(field->name, "ZEROs") <= 0)
				{
					loginfo=loginfo+(field->name)+"= ["+(row[i])+"]";
					loginfo=loginfo+" ,";
					if ((i+1)==fieldcount)
					{
						loginfo=loginfo+"ӯռ�ȣ�";

						*fOut = (atof(row[4]))/((atof(row[4]) + atof(row[6])));
						TCHAR tmp[1024] = {0};
						sprintf(tmp, "%d��",(int)round((*fOut) * 100));

						if (bLastData)
						{
							OnScreenfont(row[4], 400, 10, RGB(255, 0, 0));
							OnScreenfont(row[6], 600, 10, RGB(0, 255, 0));
							OnScreenfont(tmp, 800, 10, RGB(0, 0, 255));
						}
						loginfo=loginfo+tmp;
					}
				}
			}
			loginfo=loginfo+";";
			//OutputDebugString("444");
			row=mysql_fetch_row(result);
		}
		loginfo=loginfo+"\r\n";
		if (bLastData)
		{
			::OutputDebugString(loginfo.c_str());
			bLastData = FALSE;
		}
	}   
	else {
		//MessageBox("mysql_query() select data failed");       
	}
	//OutputDebugString("111Done*********");

}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) 
{ 
	DWORD dwCurProcessId = *((DWORD*)lParam); 
	DWORD dwProcessId = 0; 

	GetWindowThreadProcessId(hwnd, &dwProcessId); 
	if(dwProcessId == dwCurProcessId && GetParent(hwnd) == NULL)
	{ 
		*((HWND *)lParam) = hwnd;
		return FALSE; 
	} 
	return TRUE; 
} 


HWND GetMainWindow() 
{ 
	DWORD dwCurrentProcessId = GetCurrentProcessId();
	if(!EnumWindows(EnumWindowsProc, (LPARAM)&dwCurrentProcessId)) 
	{     
		return (HWND)dwCurrentProcessId; 
	} 
	return NULL; 
} 

//���ɵ�dll���������dll�뿽����ͨ���Ű�װĿ¼��T0002/dlls/����,���ڹ�ʽ���������а�
float fMaxLost = 0;
float fWinRate = 0;
float fTotalAsset = 0;
float fStartPrice = 0;
float fEndPrice = 0;
int iSaleCount = 0;
int iWinCount = 0;
float fFirstDay = 0;;
float fLastDay = 0;;

void PostMsgToTradeWnd(LPARAM lParam)
{
	TCHAR sTradeWndTitle[256] = "�Զ������׿�������";
	HWND hTradeWnd = NULL;
	hTradeWnd = ::FindWindow(NULL, sTradeWndTitle);
	if( hTradeWnd != NULL) 
	{ 
		::PostMessage(hTradeWnd,WM_TRADEMSG,0,lParam); 
	}  
}
//��һ�汾��T+1 bug�޷��������������ڶ��汾
// void TestPlugin1(int DataLen,float* pfOUT,float* buyPrice,float* salePrice,float* closePrice)
// { /*TDXDLL2(1,BUYP,SALEP,CCC)*/
// 	//OutputDebugString("\r\nTestPlugin1");
// 	BOOL bBuy = FALSE;
// 	//float fFirstBuy = 0;
// 	float fLastSale = 0;
// 	//float fBuy = 0;
// 	//float fWin = 0;
// 	float fStockCounts = 0;
// 
// 	float fHighWin = 0;
// 
// 	fMaxLost = 0;
// 	fWinRate = 0;
// 	fTotalAsset = 0;
// 	fStartPrice = 0;
// 	iSaleCount = 0;
// 	iWinCount = 0;
// 
// 	if (DataLen>0)
// 	{
// 		fTotalAsset = closePrice[0];//��ʼ�ʲ�=��һ������̼�
// 		fStartPrice = closePrice[0];
// 
// 		for(int i=0;i<DataLen;i++)
// 		{
// 			pfOUT[i]=fTotalAsset;//��������ʲ�
// 			if (bBuy)//�ֹ���
// 			{
// 				pfOUT[i] = closePrice[i] * fStockCounts;
// 
// 				//�������س� fMaxLost
// 				if (pfOUT[i] > fHighWin)
// 				{
// 					fHighWin = pfOUT[i];
// 				}
// 				else
// 				{
// 					float ftempLost = (fHighWin - pfOUT[i])/ fHighWin * 100;
// 					if (ftempLost > fMaxLost)
// 					{
// 						fMaxLost = ftempLost;
// 					}
// 				}
// 				//�������س�����
// 			}
// 			if(buyPrice[i] > 0)//����
// 			{
// 				if (!bBuy)
// 				{
// 					bBuy = TRUE;
// 
// 					fStockCounts = fTotalAsset / buyPrice[i];
// 				}
// 			}
// 			if(salePrice[i] > 0)//����
// 			{
// 				if (bBuy)
// 				{
// 					bBuy = FALSE;
// 					fLastSale = salePrice[i];
// 					float fCurrMoney = 0;
// 					fCurrMoney = salePrice[i] * fStockCounts * 0.9995/*�۳� ETF���� ������2.5��������*/;
// 					if (fCurrMoney > fTotalAsset)
// 					{
// 						iWinCount++;
// 					}
// 					fTotalAsset = fCurrMoney;
// 					iSaleCount++;
// 
// // 					TCHAR bbb[256] = {0};
// // 					sprintf(bbb, "����,%.0f,����,%.0f,����,%.0f,С��,%.0f,���ʲ�,%.0f,", fBuy, fLastSale, fWin, fTotalAsset, fTotalAsset+fFirstBuy);
// // 					OutputDebugString(bbb);
// 				}
// 			}
// 
// 		}
// 
// 		fWinRate = (fTotalAsset - closePrice[0]) / closePrice[0] * 100; //����������
// 
// 	}
// }

//�ڶ��汾
void CalcWin(int DataLen,float* pfOUT,float* currPrice,float* fAction,float* currDay)
{
	/*TDXDLL2(1,Price,Action,DAY)*/
	//OutputDebugString("\r\nTestPlugin1");
	BOOL bBought = FALSE;
	BOOL bLastSale = FALSE;
	//float fFirstBuy = 0;
	//float fLastSale = 0;
	//float fBuy = 0;
	//float fWin = 0;
	float fStockCounts = 0;

	float fHighWin = 0;

	fMaxLost = 0;
	fWinRate = 0;
	fTotalAsset = 0;
	fStartPrice = 0;
	fEndPrice = 0;
	iSaleCount = 0;
	iWinCount = 0;
	float fBuyDay = 0; 
	fFirstDay = 0;
	fLastDay = 0;

	if (DataLen>0)
	{
		TraceEx("\r\n[TDX]==>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n");
		//fTotalAsset = currPrice[0];//��ʼ�ʲ�=��һ������̼�
		//fStartPrice = currPrice[0];
		fEndPrice = currPrice[DataLen-1];
		fFirstDay = currDay[0];
		fLastDay = currDay[DataLen-1];

		for(int i=0;i<DataLen;i++)
		{
			pfOUT[i]=fTotalAsset;//��������ʲ�
			if (bBought)//�ֹ���
			{
				pfOUT[i] = currPrice[i] * fStockCounts;

				//�������س� fMaxLost
				if (pfOUT[i] > fHighWin)
				{
					fHighWin = pfOUT[i];
				}
				else
				{
					float ftempLost = (fHighWin - pfOUT[i])/ fHighWin * 100;
					if (ftempLost > fMaxLost)
					{
						fMaxLost = ftempLost;
					}
				}
				//�������س�����

				if ((fAction[i] == 0.0))
				{
					if ((fBuyDay != currDay[i]))
					{
						if (bLastSale == TRUE)
						{
							bLastSale = FALSE;
							fAction[i] = 2.0;//�ֹ��У����ڱ仯����һK����
						}
					}
				}
			}
			if(fAction[i] == 1.0)//����
			{
				if (!bBought)
				{
					bBought = TRUE;
					fBuyDay = currDay[i];

					if (fStartPrice == 0.0)
					{
						fTotalAsset = currPrice[i];//��ʼ�ʲ�=��һBUY�����̼�
						fStartPrice = currPrice[i];

						for (int xx = 0; xx <= i; xx++)
						{
							pfOUT[xx] = fTotalAsset;//�����һ��֮ǰ�����ʲ�
						}
					}

					fStockCounts = fTotalAsset / currPrice[i];
//#ifdef _DEBUG
					TCHAR bbb[256] = { 0 };
					sprintf(bbb, ">>����day,%.0f,�۸�,%.3f,���ʲ�,%.3f,", fBuyDay-1000000, currPrice[i], fTotalAsset);
					OutputDebugString(bbb);
//#endif
				}
				bLastSale = FALSE;
			}
			else if((fAction[i] == 2.0))
			{
				if ((fBuyDay != currDay[i]))//����
				{
					if (bBought)
					{
						bBought = FALSE;
						//fLastSale = fAction[i];
						float fCurrMoney = 0;
						fCurrMoney = currPrice[i] * fStockCounts * 0.9995/*�۳� ETF���� ������2.5��������*/;
						if (fCurrMoney > fTotalAsset)
						{
							iWinCount++;
						}
						fTotalAsset = fCurrMoney;
						iSaleCount++;

	//#ifdef _DEBUG
						TCHAR bbb[256] = { 0 };
						sprintf(bbb, "====����day,%.0f,�۸�,%.3f,���ʲ�,%.3f,[%d]��", currDay[i]-1000000, currPrice[i], fTotalAsset, iSaleCount);
						OutputDebugString(bbb);
	//#endif
					}
				}			
				else
				{
					bLastSale = TRUE;
				}

			}

		}

		if (fStartPrice > 0.0)
		{
			fWinRate = (fTotalAsset - fStartPrice) / fStartPrice * 100; //����������
		}

	}
}

void getReal_BS(int DataLen,float* pfOUT,float* currDay,float* currTime,float* fAction)
{
	//ʵ������:TDXDLL2(5,DATE,TIME,IF(ȫ���,1,IF(ȫ����,2,0)));

	BOOL bBought = FALSE;
	BOOL bNextDaySale = FALSE;
	//float fFirstBuy = 0;
	//float fLastSale = 0;
	//float fBuy = 0;
	//float fWin = 0;
	float fBuyDay = 0; 
	fFirstDay = 0;
	fLastDay = 0;

	if (DataLen>0)
	{
		TraceEx("\r\n[TDX]==>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n");
		fFirstDay = currDay[0];
		fLastDay = currDay[DataLen-1];

		for(int i=0;i<DataLen;i++)
		{
			pfOUT[i]=0;//���

			if (currTime[i] >= 1457) //����ǰ���Ͼ���ʱ��
			{
				if (fAction[i] > 0.0f)//�ж���
				{
					if ((i+1)<DataLen)//����һ����
					{
						if (fAction[i+1] == 0.0f)//��һ�����޶���
						{
							fAction[i+1] = fAction[i];//��ǰ����������һ����
							fAction[i] = 0.0f;//��ǰ�������
						}
					}
				}

				continue;
			}

			if (bBought)//�ֹ���
			{
				if ((fAction[i] == 0.0f))
				{
					if ((fBuyDay != currDay[i]))
					{
						if (bNextDaySale == TRUE)
						{
							bNextDaySale = FALSE;
							fAction[i] = 2.0f;//�ֹ��У����������źţ����ڱ仯����һK����
						}
					}
				}
			}

			if(fAction[i] == 1.0f)//����
			{
				if (!bBought)
				{
					bBought = TRUE;
					fBuyDay = currDay[i];
					pfOUT[i] = 1.0f;//���
				}
				bNextDaySale = FALSE;
			}
			else if((fAction[i] == 2.0f))
			{
				if ((fBuyDay != currDay[i]))//����
				{
					if (bBought)
					{
						bBought = FALSE;
						pfOUT[i] = 2.0f;//���
					}
				}			
				else
				{
					bNextDaySale = TRUE;
				}

			}

		}

	}
}

//std::vector<float>
//Ema(std::vector<float> &X, int N)
//{
//	std::vector<float> vec;
//	int nLen = X.size();
//	if (nLen >= 1)
//	{
//		if (N > nLen) N = nLen;
//
//		vec.resize(nLen);
//		//vec.reserve(nLen);
//		vec[0] = X[0];
//		for (int i = 1; i < nLen; i++)
//		{
//			vec[i] = (2 * X[i] + (N - 1) * vec[i - 1]) / (N + 1);
//		}
//	}
//	return vec;
//}

/*
DIF:EMA(CLOSE,SHORT)-EMA(CLOSE,LONG);
DEA:EMA(DIF,MID);
MACD:(DIF-DEA)*2,COLORSTICK;
*/

void testEma(int DataLen, float* pfOUT, float* pfINa, float* pfINb, float* pfINc)
{
	if ((DataLen > 0) )
	{
		int N = pfINb[0];
		if (N > DataLen) N = DataLen;
		pfOUT[0] = pfINa[0];

		for (int i = 1; i < DataLen; i++)
		{
			pfOUT[i]= (2 * pfINa[i] + (N - 1) * pfOUT[i - 1]) / (N + 1);	//EMA���㹫ʽ
			//if (pfINa[i] != 0.0f && pfINb[i] != 0.0f && pfINc[i] != 0.0f)
			//{
			//	TraceEx("\r\n[TDX]=- \t%.3f\t%.3f\t%.3f", pfINa[i], pfINb[i], pfINc[i]);
			//}
		}
	}
}

void OutputN(int DataLen, float* pfOUT, float* pfINa, float* pfINb, float* pfINc)
{
	if ((DataLen > 0) )
	{
		for (int i = 0; i < DataLen; i++)
		{
			if (pfINa[i] != 0.0f && pfINb[i] != 0.0f && pfINc[i] != 0.0f)
			{
				TraceEx("\r\n[TDX]=- \t%.3f\t%.3f\t%.3f", pfINa[i], pfINb[i], pfINc[i]);
			}
		}
	}
}

void OutputP(int DataLen, float* pfOUT, float* pfINa, float* pfINb, float* pfINc)
{
	if ((DataLen > 0) )
	{
		for (int i = 0; i < DataLen; i++)
		{
			if (pfINa[i]!=0.0f && pfINb[i] != 0.0f && pfINc[i] != 0.0f )
			{
				TraceEx("\r\n[TDX]=+ \t%.3f\t%.3f\t%.3f", pfINa[i], pfINb[i], pfINc[i]);
			}
		}
	}
}

void returnMaxLost(int DataLen,float* pfOUT,float* pfINa,float* pfINb,float* pfINc)
{
	if ((DataLen > 0) && (fMaxLost > 0))
	{
		//Log ���س� fMaxLost
		TCHAR bbb[256] = {0};
		TraceEx("\r\n[TDX]��(%.0f_%.0f_%.0f),%2.2f��%2.2f, ��(%.3f��%.3f), ��:%.3f, ӯ\t%.3f\t��|��-%.2f��|��%.2f��, %d��|%dӯ|��%.2f��", pfINa[0], pfINb[0], pfINc[0], 190000.0+(fFirstDay/100), 190000.0+(fLastDay/100), fStartPrice, fEndPrice, fTotalAsset, fWinRate,fMaxLost, 100*fWinRate/fMaxLost, iSaleCount,iWinCount,(100.0*iWinCount/iSaleCount));
		//OutputDebugString(bbb);
	}

	//�������س� fMaxLost
	for(int i=0;i<DataLen;i++)
	{
		pfOUT[i]=fMaxLost;
	}
}

void returnWinRate(int DataLen,float* pfOUT,float* pfINa,float* pfINb,float* pfINc)
{
	//������������ fTotalAsset
	//OutputDebugString("TestPlugin3");
	for(int i=0;i<DataLen;i++)
	{
		pfOUT[i]=fWinRate;
	}
}


void WaitCorrectActiveWnd() 
{
	//��ҵ��ָ(399006) 2017��01��20�� ������ PageUp/Down:ǰ���� �ո��:���� ͨ����(R)
	char   chText[MAX_PATH];//�������洰�ڵ�Text   
	ZeroMemory(chText,MAX_PATH); // ZeroMemory����0�����һ���ڴ����� 
	while(!((strstr(chText, "399006") > 0) &&
		(strstr(chText, "PageUp/Down:ǰ����") > 0) &&
		(strstr(chText, "�ո��") > 0)))
	{
		Sleep(100);
		GetWindowText(::GetActiveWindow(),chText,MAX_PATH);//��ȡ���ڵı���
	}
}

#define SIZE 2

int m_iLastDate = 0;

int convertDateToFile( int curDate ) 
{
	int Date = curDate+19000000;
	int Y = Date/10000;
	int M = (Date % 10000)/100;
	int D = (Date % 100);
	int DateToFile = (Y-2004)*0x800+M*100+D;
	return DateToFile;
}

void GetminFilefullPath(TCHAR * pBuf) 
{
	GetModuleFileName(NULL,pBuf,MAX_PATH);
	TCHAR * tmp = pBuf;
	while (strstr(tmp, "\\"))
	{
		tmp = strstr(tmp, "\\")+1;
	}
	tmp[0] = '\0';
	//TraceEx(pBuf);
	strcat(pBuf, "vipdoc\\sz\\minline\\sz399006.lc1");
}

int readLastDateInFile() 
{
	FILE* pFile;  
	int DateInFile = 0;
	//���ļ�
	//pFile = fopen("N:\\Xmoney\\new_tdx\\vipdoc\\sz\\minline\\sz399006.lc1" , "rb"); // ���ļ�  
	TCHAR  pBuf[MAX_PATH];
	GetminFilefullPath(pBuf);
	pFile = fopen(pBuf, "rb"); // ���ļ�  
	if (pFile)
	{
		fseek(pFile, 0, SEEK_END);
		int iFsize = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);
		if(iFsize>=32)
		{
			//��ȡ���һ������
			fseek(pFile,iFsize-32,SEEK_SET);//��fpָ���˻ص����ļ���β32�ֽڴ���
			if(fread(&DateInFile,2,1,pFile)==1)
			{//��ȡ�ɹ�
			}

		}
		fclose(pFile); // �ر��ļ�  
	}
	return DateInFile;
}

void dataExport1Minute(int DataLen,float* pfOUT,float* currPrice,float* curDate,float* currTime)
{
//Todo:��ǰ��ҳ���߼������ж�
	if (DataLen > 0)
	{
		//ת�����ݸ�ʽ
		int DateToFile = convertDateToFile((int)(curDate[0]));
		int DateInFile = 0;
		TraceEx("[TDX]( DateToFile=%X curDate[0]=%X", DateToFile, (int)(curDate[0]));
		if (m_iLastDate == 0)
		{
			DateInFile = readLastDateInFile();

			if (DateInFile >= DateToFile)
			{	
				//�ϴ�����������ڵ�ǰ���ݵĵ�һ��
				//���ҳ
// 				TraceEx("[TDX](��ҳaaa DateInFile=%X, DateToFile=%X",DateInFile, DateToFile);
// 				WaitCorrectActiveWnd();
// 				::PostMessage(::GetActiveWindow(),WM_KEYDOWN,VK_NEXT,NULL);
// 				::PostMessage(::GetActiveWindow(),WM_KEYUP,VK_NEXT,NULL);
				return;
			}		
		}
		else if (m_iLastDate >= DateToFile)
		{
			//�ϴ�����������ڵ�ǰ���ݵĵ�һ��
			//���ҳ
// 			TraceEx("[TDX](��ҳbbb m_iLastDate=%X, DateToFile=%X",m_iLastDate, DateToFile);
// 			WaitCorrectActiveWnd();
// 			::PostMessage(::GetActiveWindow(),WM_KEYDOWN,VK_NEXT,NULL);
// 			::PostMessage(::GetActiveWindow(),WM_KEYUP,VK_NEXT,NULL);
			return;
		}
	}
// 	//

	FILE* pFile;  
	TCHAR  pBuf[MAX_PATH];
	GetminFilefullPath(pBuf);

	pFile = fopen(pBuf , "ab+"); // ���ļ�׷��д����  
	if (pFile)
	{
		int outDate = 0;
		fseek(pFile, 0, SEEK_END);
		for(int i=0;i<DataLen;i++)
		{
			TraceEx("[TDX]( ��������A�� %d, %d, %f, %x\r\n", (int)(curDate[i]+19000000), (int)(currTime[i]), currPrice[i], *(int *)&currPrice[i]);
			/*
			(int)(curDate[i]+19000000)	20170105	int
			(int)currTime[i]			93200	int
			currPrice[i]				1988.6200	float
			*/
			//ת�����ݸ�ʽ
			int Date = (int)(curDate[i]+19000000);
			int Y = Date/10000;
			int M = (Date % 10000)/100;
			int D = (Date % 100);
			outDate = (Y-2004)*0x800+M*100+D;
			int hm = (int)(currTime[i])/100;
			int h = hm/100;
			int m = hm % 100;
			int outTime = h*60+m;
			int outZero = 0;
			int outD = outTime * 0x10000 + outDate;
			int xPrice = *(int *)&currPrice[i];
			/*		 outDate outTime
			00000000h: FE 5B 3F 02	8F 32 13 45		A4 A0 14 45		8F 32 13 45 
			00000010h: 00 EC 13 45	4A F2 0F 50		CB 02 C2 05		28 01 6D 00 

			��2355.159912109375
			��2378.0400390625
			��2355.159912109375
			��2366.75
			��9660082176.0
			��96600779
			��7143720
			*/
			//׷��д��1����TDX�ѻ��ļ���
			TraceEx("%d",fwrite(&outD , 4 , 1 , pFile)); // 
			TraceEx("%d",fwrite(&xPrice , 4 , 1 , pFile)); // 
			TraceEx("%d",fwrite(&xPrice , 4 , 1 , pFile)); // 
			TraceEx("%d",fwrite(&xPrice , 4 , 1 , pFile)); // 
			TraceEx("%d",fwrite(&xPrice , 4 , 1 , pFile)); // 
			TraceEx("%d",fwrite(&outZero , 4 , 1 , pFile)); // 
			TraceEx("%d",fwrite(&outZero , 4 , 1 , pFile)); // 
			TraceEx("%d",fwrite(&outZero , 4 , 1 , pFile)); // 
		}
		fclose(pFile); // �ر��ļ�  
		if (DataLen > 0)
		{
			m_iLastDate = outDate;

			//WaitCorrectActiveWnd();
			::PostMessage(::GetActiveWindow(),WM_KEYDOWN,VK_NEXT,NULL);
			::PostMessage(::GetActiveWindow(),WM_KEYUP,VK_NEXT,NULL);
		}
	}
}
//��ʱֹͣд�˴��룬����TDX�ĵ�������Ϊcsv��������ָͬ��һ�𵼳�
//ֻ��3�������������ҪTDX��ε��ú��������д���ļ�
//"2013-06-05 16:34:18",1.562,0.5,0.6,0.7,0.71,0.81,0.82,0.9,0.91
//void dataExport2CSV1(int DataLen,float* pfOUT,float* o,float* h,float* l)
//{
// 	for(int i=0;i<DataLen;i++)
// 	{
// 		TraceEx("[TDX]( ��ʱ����B�� %.3f, %.3f, %.3f\r\n", o, h, l);
// 	}
//}

void SmoothBSCalc(int DataLen,float* pfOUT,float* currPrice,float* fAction,float* currDay)
{

	//�����λ���ƺ�Ľ��
/*
Dll�м���Ŀǰ��λ��
fAction>0 ��ʾ�����źż���λ�ٷֱ�  0.088015������λ0.88�������λ0.15��
fAction<0 ��ʾ�����źż���λ�ٷֱ� -0.088015������λ0.88��������λ0.15��
fAction=0 ��ʾ���ź�

*/
	float fMoney = 0;
	float fStockSellable = 0;
	float fStockLocked = 0;
	float fNewDay = 0;
	fStartPrice = 0;
	fEndPrice = 0;

	float fHighWin = 0;
	fTotalAsset = 0;
	fMaxLost = 0;
	fWinRate = 0;
	iSaleCount = 0;
	iWinCount = 0;
	fFirstDay = 0;
	fLastDay = 0;

	if (DataLen>0)
	{
		fTotalAsset = currPrice[0];//��ʼ�ʲ�=��һ������̼�
		fStartPrice = currPrice[0];
		fEndPrice = currPrice[DataLen-1];
		fFirstDay = currDay[0];
		fLastDay = currDay[DataLen-1];

		for(int i=0;i<DataLen;i++)
		{
			if (fNewDay != currDay[i])
			{//��һ�죬T+1������
				fNewDay = currDay[i];
				fStockSellable += fStockLocked;
				fStockLocked = 0;
			}

			fTotalAsset = fMoney + (currPrice[i] * (fStockSellable + fStockLocked));
			pfOUT[i] = fTotalAsset;

			if (fAction[i] > 0)//�����ź�
			{
				if ((fMoney == 0) && (fStockSellable == 0) && (fStockLocked == 0)) 
				{//��һ�������źţ���ʼ�ʽ���Ϊ��ǰ�۸�
					fMoney = currPrice[i];
					fTotalAsset = fMoney;
				}
				float fMaxPercent = float(int(fAction[i]*1000))/100;//����λ�ٷֱ�

				float fBuyPercent = (fAction[i]*1000);//���β�λ�ٷֱ�
				fBuyPercent = (float)((int)(fBuyPercent*1000)%1000)/100;//���β�λ�ٷֱ�


				float fCurPercent = ((currPrice[i] * (fStockSellable + fStockLocked))/fTotalAsset);
				TraceEx("\r\n[TDX]( ��λ=%.2f��B [%.0f��| ��=%.2f| �޲�=%.2f| �Ӳ�=%.2f| �ʲ�=%.2f| Ǯ=%.2f| T0Ʊ=%.2f | T1Ʊ=%.2f  ����=%d ]***\r\n", fCurPercent, currDay[i], currPrice[i], fMaxPercent, fBuyPercent, fTotalAsset, fMoney, fStockSellable, fStockLocked, DataLen);
				if ((fMaxPercent - fCurPercent) >= 0.01)
				{
					float fNeedMoney = 0;
					if ((fMaxPercent - fCurPercent) >= fBuyPercent)
					{
						//�����λ�ٷֱ�fBuyPercent
						fNeedMoney = (fBuyPercent * fTotalAsset);
					} 
					else//��λ�����ﵽ�������ֵ
					{
						//�����λ�ٷֱ�(fMaxPercent - fCurPercent)
						fNeedMoney = ((fMaxPercent - fCurPercent) * fTotalAsset);
					}
					if (fMoney >= fNeedMoney)
					{//��Ǯ��
						fStockLocked += (fNeedMoney/currPrice[i]);
						fMoney -= fNeedMoney;
					} 
					else if (fMoney > 0)
					{//��Ǯ����������
						fStockLocked += fMoney / currPrice[i];
						fMoney = 0;
					}//��Ǯ=0
				} 
				fCurPercent = ((currPrice[i] * (fStockSellable + fStockLocked))/fTotalAsset);
				TraceEx("\r\n[TDX]( ��λ=%.2f B��[%.0f��| ��=%.2f| �޲�=%.2f| �Ӳ�=%.2f| �ʲ�=%.2f| Ǯ=%.2f| T0Ʊ=%.2f | T1Ʊ=%.2f  ����=%d ]***\r\n", fCurPercent, currDay[i], currPrice[i], fMaxPercent, fBuyPercent, fTotalAsset, fMoney, fStockSellable, fStockLocked, DataLen);
			}
			else if (fAction[i] < 0)//�����ź�
			{
				if (fTotalAsset > 0)
				{
					float fMaxPercent = float(int(fAction[i]*(-1)*1000))/100;//����λ�ٷֱ�

					float fSellPercent = (fAction[i]*(-1)*1000);//���β�λ�ٷֱ�
					fSellPercent = (float)((int)(fSellPercent*1000)%1000)/100;//���β�λ�ٷֱ�

					float fCurPercent = ((currPrice[i] * (fStockSellable + fStockLocked))/fTotalAsset);
					TraceEx("\r\n[TDX]( ��λ=%.2f��S [%.0f��| ��=%.2f| �޲�=%.2f| �Ӳ�=%.2f| �ʲ�=%.2f| Ǯ=%.2f| T0Ʊ=%.2f | T1Ʊ=%.2f  ����=%d ]***\r\n", fCurPercent, currDay[i], currPrice[i], fMaxPercent, fSellPercent, fTotalAsset, fMoney, fStockSellable, fStockLocked, DataLen);

					//������λ�ٷֱ�fSellPercent
					float fNeedStock = (fSellPercent * fTotalAsset)/currPrice[i];
					if (fStockSellable >= fNeedStock)
					{//����Ʊ��
						fStockSellable -= fNeedStock;
						fMoney += fNeedStock * currPrice[i] * 0.9995/*�۳� ETF���� ������2.5��������*/;;

						iSaleCount++;
					} 
					else if (fStockSellable > 0)
					{//����Ʊ����������
						fMoney += fStockSellable * currPrice[i] * 0.9995/*�۳� ETF���� ������2.5��������*/;;
						fStockSellable = 0;
						iSaleCount++;
					}//����Ʊ=0

					fCurPercent = ((currPrice[i] * (fStockSellable + fStockLocked))/fTotalAsset);
					TraceEx("\r\n[TDX]( ��λ=%.2f S��[%.0f��| ��=%.2f| �޲�=%.2f| �Ӳ�=%.2f| �ʲ�=%.2f| Ǯ=%.2f| T0Ʊ=%.2f | T1Ʊ=%.2f  ����=%d ]***\r\n", fCurPercent, currDay[i], currPrice[i], fMaxPercent, fSellPercent, fTotalAsset, fMoney, fStockSellable, fStockLocked, DataLen);
				} 
				else
				{
					TraceEx("\r\n[TDX]( ��λ=0 [S][%.0f��| ��=%.2f| �ʲ�=%.2f| Ǯ=%.2f| T0Ʊ=%.2f | T1Ʊ=%.2f  ����=%d ]***\r\n", currDay[i], currPrice[i], fTotalAsset, fMoney, fStockSellable, fStockLocked, DataLen);
				}
			}
			else
			{
				//TraceEx("\r\n[TDX]( ��λ����[ ��=%f| Ǯ=%f| ����Ʊ=%f | ������=%f \r\n", fTotalAsset, fMoney, fStockSellable, fStockLocked);
				//�������س� fMaxLost
				if (pfOUT[i] > fHighWin)
				{
					fHighWin = pfOUT[i];
				}
				else
				{
					float ftempLost = (fHighWin - pfOUT[i])/ fHighWin * 100;
					if (ftempLost > fMaxLost)
					{
						fMaxLost = ftempLost;
					}
				}
				//�������س�����

			}
		}
		fWinRate = (fTotalAsset - currPrice[0]) / currPrice[0] * 100; //����������

	}
}

void PostMsgOf_BS(int DataLen, float* pfOUT, float* pfINa, float* pfINb, float* pfINc) //���룬����������
{
/*	for(int i=0;i<DataLen;i++)
	{
		TCHAR bbb[256] = {0};
		sprintf(bbb, "\r\npfINa[%d]=%.0f,pfINb[%d]=%.0f,", i, pfINa[i], i, pfINb[i]);
		OutputDebugString(bbb);
	}
*/
	if (DataLen > 0) 
	{
		int iTrade=0;
		TCHAR aaa[512] = {0};
		TCHAR bbb[512] = {0};
		::GetWindowText(GetMainWindow(), bbb, 512);
		if(pfINa[DataLen-1] > 0)
		{
			iTrade=1;
			strcat(bbb, "  ָ�++++++++����++++++++");
		}
		else if(pfINb[DataLen-1] > 0)
		{
			iTrade=2;
			strcat(bbb, "  ָ�-------����------");
		}
		else 
		{
			iTrade=0;
			strcat(bbb, "  ָ�**********�޲���**********");
		}

		if (pfINc[DataLen-1] == 1)//{ֻ����5�������ڵ������źţ�PERIOD=1 }
		{
			PostMsgToTradeWnd(iTrade);
			sprintf(aaa, "\r\n[TDX]ָ��,(%.0f_%.0f_���ڣ�%.0f), %s,", pfINa[DataLen-1], pfINb[DataLen-1], pfINc[DataLen-1], bbb);
			OutputDebugString(aaa);
		}

	}
}
void BuyOne(int DataLen,float* pfOUT,float* pfINa,float* pfINb,float* pfINc)
{
	OutputDebugString("BuyOne");
}

void Test99(int DataLen,float* pfOUT,float* pfINa,float* pfINb,float* pfINc)
{
	OutputDebugString("Test99");
}

void QueryDB(int DataLen,float* pfOUT,float* pfINa,float* pfINb,float* pfINc)
{
	if (bConnectMysql == FALSE)
	{
		bConnectMysql = ConnectMysql("127.0.0.1", "root", "admin777", "stock", 3306);
	}

	//OutputDebugString("QueryDB");
	/*for (int i = 0; i<DataLen; i++)
	{
		ShowTable(pfINa[i]/100.00, pfINb[i]/100.00, (int(pfINc[i]))/100.00, (pfINc[i]-(int(pfINc[i])))*100.00); 
	}*/
	if (DataLen > 0)
	{
		//int i = DataLen - 1;
		for (int i = 0; i<DataLen; i++)
		{
			if (i == DataLen - 1)
			{
				bLastData = TRUE;
			}
			ShowTable(&(pfOUT[i]), pfINa[i]/100.00, pfINb[i]/100.00, (round(pfINc[i]) )/100.00, (pfINc[i]-(round(pfINc[i])))*100.00); 
			bLastData = FALSE;

		}
	}	
	//mysql_close(&mysql);//���д��OnDestroy()������

}
//���صĺ���
PluginTCalcFuncInfo g_CalcFuncSets[] = 
{
	// 
	//{0,NULL},
	{1,(pPluginFUNC)&CalcWin},
	{2,(pPluginFUNC)&returnMaxLost},
	{3,(pPluginFUNC)&returnWinRate},
	{4,(pPluginFUNC)&SmoothBSCalc},
	//{5,(pPluginFUNC)&dataExport1Minute},
	{5,(pPluginFUNC)&getReal_BS},//���� ȫ����/ȫ��㣬���T+1��ʵ�ʵ���Ч�����㣬���ظ�TDX��ʽ
	{6,(pPluginFUNC)&QueryDB},

	{8,(pPluginFUNC)&PostMsgOf_BS},//�����̨��������ָ��//(���룬����������)
	{9,(pPluginFUNC)&testEma},

	{ 10,(pPluginFUNC)&Test99 },
	{ 11,(pPluginFUNC)&Test99 },
	{ 12,(pPluginFUNC)&Test99 },
	{ 13,(pPluginFUNC)&Test99 },
	{ 88,(pPluginFUNC)&OutputN }, //Log������������3����ֵ or 3��0.0f
	{ 89,(pPluginFUNC)&OutputP }, //Log�������ӯ����3����ֵ or 3��0.0f
	{0,NULL}
};

//������TCalc��ע�ắ��
BOOL RegisterTdxFunc(PluginTCalcFuncInfo** pFun)
{
	if(*pFun==NULL)
	{
		//ConnectMysql("127.0.0.1","root","admin777","stock",3306);
		(*pFun)=g_CalcFuncSets;
		return TRUE;
	}
	return FALSE;
}
