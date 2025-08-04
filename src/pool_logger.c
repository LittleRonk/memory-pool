#include <pool_logger.h>

void poolEnableLogToFile(const char *file_name, logLevel level)
{
    logToFileEnable(file_name, level);
}

void poolEnableLogToStdout(logLevel level)
{
    logToStdoutEnable(level);
}

void poolDisableLogToFile(void)
{
    logToFileDisable();
}

void poolDisableLogToStdout(void)
{
    logToStdoutDisable();
}
