#include "localLog.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/time.h>

#include <string.h>
#include <errno.h>
#include <stdarg.h>

//
//获取时间相关函数
//
char * OI_GetShortDateStr(const time_t * mytime)
{
    static char s[50];
    struct tm curr = *localtime(mytime);

    if(curr.tm_year > 50)
        snprintf(s, sizeof(s), "%04d%02d%02d", curr.tm_year + 1900, curr.tm_mon + 1, curr.tm_mday);
    else
        snprintf(s, sizeof(s), "%04d%02d%02d", curr.tm_year + 2000, curr.tm_mon + 1, curr.tm_mday);

    return s;
}

static char* OI_GetDateTimeStr(const time_t * mytime)
{
    static char s[50];
    struct tm curr = *localtime(mytime);

    if(curr.tm_year > 50)
        snprintf(s, sizeof(s), "%04d-%02d-%02d %02d:%02d:%02d", curr.tm_year + 1900, curr.tm_mon + 1, curr.tm_mday, curr.tm_hour, curr.tm_min, curr.tm_sec);
    else
        snprintf(s, sizeof(s), "%04d-%02d-%02d %02d:%02d:%02d", curr.tm_year + 2000, curr.tm_mon + 1, curr.tm_mday, curr.tm_hour, curr.tm_min, curr.tm_sec);

    return s;
}

static char * OI_GetCurShortDateStr(void)
{
    time_t mytime = time(NULL);
    return OI_GetShortDateStr(&mytime);
}

static char * OI_GetCurDateTimeStr(void)
{
    time_t mytime = time(NULL);
    return OI_GetDateTimeStr(&mytime);
}


static int ShiftFiles(OI_LogFile * pstLogFile)
{
    struct stat stStat;
    char sLogFileName[300];
    char sNewLogFileName[300];
    int i;
    struct tm stLogTm, stShiftTm;

    if (pstLogFile->iShiftType == 6)
        return 0;

    snprintf(sLogFileName,sizeof(sLogFileName),"%s.log", pstLogFile->sBaseFileName);

    if(stat(sLogFileName, &stStat) < 0) 
    {
        if (errno == ENOENT) {
            FILE *pfile;
            if ((pfile = fopen(sLogFileName, "a+")) == NULL) 
                return -1;
            fclose(pfile);
            if (stat(sLogFileName, &stStat) < 0)
                return -1;
        } else {
            return -1;
        }
    }
    switch (pstLogFile->iShiftType)
    {
        case 0:
            if(stStat.st_size < pstLogFile->lMaxSize)
                return 0;
            break;
        case 2:
            if(stStat.st_mtime - pstLogFile->lLastShiftTime < pstLogFile->lMaxCount)
                return 0;
            break;
        case 3:
            if(pstLogFile->lLastShiftTime - stStat.st_mtime > 86400)
                break;
            memcpy(&stLogTm, localtime(&stStat.st_mtime), sizeof(stLogTm));
            memcpy(&stShiftTm, localtime(&pstLogFile->lLastShiftTime), sizeof(stShiftTm));
            if(stLogTm.tm_mday == stShiftTm.tm_mday)
                return 0;
            break;
        case 4:
            if(pstLogFile->lLastShiftTime - stStat.st_mtime > 3600)
                break;
            memcpy(&stLogTm, localtime(&stStat.st_mtime), sizeof(stLogTm));
            memcpy(&stShiftTm, localtime(&pstLogFile->lLastShiftTime), sizeof(stShiftTm));
            if(stLogTm.tm_hour == stShiftTm.tm_hour)
                return 0;
            break;
        case 5:
            if(pstLogFile->lLastShiftTime - stStat.st_mtime > 60)
                break;
            memcpy(&stLogTm, localtime(&stStat.st_mtime), sizeof(stLogTm));
            memcpy(&stShiftTm, localtime(&pstLogFile->lLastShiftTime), sizeof(stShiftTm));
            if(stLogTm.tm_min == stShiftTm.tm_min)
                return 0;
            break;
        default:
            if(pstLogFile->lLogCount < pstLogFile->lMaxCount)
                return 0;
            pstLogFile->lLogCount = 0;
    }

    for(i = pstLogFile->iMaxLogNum - 2; i >= 0; i--)
    {
        if(i == 0)
            snprintf(sLogFileName,sizeof(sLogFileName),"%s.log", pstLogFile->sBaseFileName);
        else
            snprintf(sLogFileName,sizeof(sLogFileName),"%s%d.log", pstLogFile->sBaseFileName, i);

        if(access(sLogFileName, F_OK) == 0)
        {
            snprintf(sNewLogFileName,sizeof(sNewLogFileName),"%s%d.log", pstLogFile->sBaseFileName, i + 1);
            if(rename(sLogFileName, sNewLogFileName) < 0)
            {
                return -1;
            }
        }
    }
    time(&pstLogFile->lLastShiftTime);
    return 0;
}

//@brief：初始化LOG对象
int OI_InitLogFile(OI_LogFile* pstLogFile, char *sLogBaseName, int iShiftType, int iMaxLogNum, int iMAX)
{
    memset(pstLogFile, 0, sizeof(OI_LogFile));
    strncat(pstLogFile->sLogFileName, sLogBaseName, sizeof(pstLogFile->sLogFileName) - 10);
    strcat(pstLogFile->sLogFileName, ".log");

    strncpy(pstLogFile->sBaseFileName, sLogBaseName, sizeof(pstLogFile->sBaseFileName) - 15);
    pstLogFile->iShiftType = iShiftType;
    pstLogFile->iMaxLogNum = iMaxLogNum;
    pstLogFile->lMaxSize = iMAX;
    pstLogFile->lMaxCount = iMAX;
    pstLogFile->lLogCount = iMAX;
    time(&pstLogFile->lLastShiftTime);

    return ShiftFiles(pstLogFile);
}

int OI_Log(OI_LogFile * pstLogFile, int iLogTime,const char *sFormat, ...)
{
    va_list ap;
    struct timeval stLogTv;

    if (pstLogFile->iShiftType == 6) {
        snprintf(pstLogFile->sLogFileName, sizeof(pstLogFile->sLogFileName), "%s_%s.log", 
            pstLogFile->sBaseFileName, OI_GetCurShortDateStr());
    }

    if((pstLogFile->pLogFile = fopen(pstLogFile->sLogFileName, "a+")) == NULL)
        return -1;
    va_start(ap, sFormat);
    if(iLogTime == 1)
    {
        fprintf(pstLogFile->pLogFile, "[%s] ", OI_GetCurDateTimeStr());
    }
    else if(iLogTime == 2)
    {
        gettimeofday(&stLogTv, NULL);
        fprintf(pstLogFile->pLogFile, "[%s.%.6d] ", 
            OI_GetDateTimeStr((const time_t *) &(stLogTv.tv_sec)), (int) stLogTv.tv_usec);
    }
    vfprintf(pstLogFile->pLogFile, sFormat, ap);
    fprintf(pstLogFile->pLogFile, "\n");
    va_end(ap);
    pstLogFile->lLogCount++;
    fclose(pstLogFile->pLogFile);
    return ShiftFiles(pstLogFile);
}