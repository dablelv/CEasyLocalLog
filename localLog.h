#ifndef _LOCALLOG_H_
#define _LOCALLOG_H_

#include <stdio.h>
#include <time.h>

typedef struct
{
    FILE *pLogFile;
    char sBaseFileName[80];	//log文件名前缀
    char sLogFileName[80];	//log文件名
    int iShiftType;    		//0 -> 大小,  1 -> 文件数, 2 -> shift by interval, 3 ->天, 4 -> 小时, 5 ->分钟
    int iMaxLogNum;			//log文件最大个数
    int lMaxSize;			//单个log文件最大大小，单位Byte
    int lMaxCount;			//单个log文件最大行数，单位line
    int lLogCount;			//当前log文件行数，单位line
    time_t lLastShiftTime;	//上一次切换时间，单位s
}OI_LogFile;

// @brief   OI_InitLogFile: 初始化log
// @param   pstLogFile: log结构指针
// @param   sLogBaseName: Log文件名的前缀字符串
// @param   iShiftType: log文件切换类型,
//      0->shift by size, when size > iMAX bytes
//      1->shift by LogCount, when logcount > iMAX lines
//      2->shift by interval, when elapse seconds > iMAX sec.
//      3->shift by day, 
//      4->shift by hour 
//      5->shift by minute
//      6->shift by day, log filename append with date.
//
// @param   iMaxLogNum: 切换log文件的最大个数. 当iShiftType=6时无意义。
// @param   iMAX: 如果iShiftType为0/1/2，那么表示当前log文件单位到达iMAX时进行切换，否则无意义。
// @return  成功返回0，失败 < 0
int OI_InitLogFile(OI_LogFile * pstLogFile, char *sLogBaseName, int iShiftType, int iMaxLogNum, int iMAX);

//
//@brief:	Log string to logfile.
//@param:	pstLogFile:log结构指针
//			iLogTime:0 = do not log time, 1 = log simple time, 2 = log detail time	
//			sFormat:C string pointed by format to the logfile,format may include format specifiers
//			...:可变参数
//@return:	<0 means failure
//@ps:__attribute__ ((format (printf, 3, 4))):提示编译器，对这个函数的调用需要像printf一样，用对应的format字符串来check可变参数的数据类型。3和4对应函数第3和第4个形参
int OI_Log(OI_LogFile * pstLogFile, int iLogTime, const char *sFormat, ...) __attribute__ ((format (printf, 3, 4)));

#endif