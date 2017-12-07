/*
*@brief:C风格简易log系统demo
*@author:dablelv,1589276509@qq.com
*@date:20171207
*/

#include "localLog.h"

//定义三个级别LOG
OI_LogFile stErrorLog;		//错误日志
OI_LogFile stDebugLog;		//调试日志
OI_LogFile stEventLog;		//流水日志

//宏定义调用log函数
//ps:_args_...:GCC支持的可变参数名；"##对符号进行连接，如果_args_为空，则不进行连接，使函数宏可省略可变参数
#define ErrorLog(_fmt_, _args_...) do{ OI_Log(&stErrorLog, 2, _fmt_, ##_args_); } while (0)
#define DebugLog(_fmt_, _args_...) do{ OI_Log(&stDebugLog, 2, _fmt_, ##_args_); } while (0)
#define EventLog(_fmt_, _args_...) do{ OI_Log(&stEventLog, 2, _fmt_, ##_args_); } while (0)

int main(int argc,char* argv[])
{
	//初始化logFile
	//保留最多10个log文件，每个最大大小约为100MB
	OI_InitLogFile(&stErrorLog,"/home/dablelv/test/logSys/log/error",0,10,100000000);
	OI_InitLogFile(&stDebugLog,"/home/dablelv/test/logSys/log/debug",0,10,100000000);
	OI_InitLogFile(&stEventLog,"/home/dablelv/test/logSys/log/event",0,10,100000000);
	
	//向log文件写日志
	ErrorLog("this is error log");
	ErrorLog("this is error log whith %s","argument");
	DebugLog("this is debug log");
	DebugLog("this is debug log whith %s","argument");
	EventLog("this is event log");
	EventLog("this is event log whith %s","argument");
}