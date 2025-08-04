#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <logger.h>

static unsigned char log_to_file = 0;
static unsigned char log_to_stdout = 0;
static logLevel file_log_level = LOG_LEVEL_FATAL;
static logLevel stdout_log_level = LOG_LEVEL_FATAL;
static FILE *log_file = NULL;

void logToStdoutEnable(logLevel level)
{
    log_to_stdout = 1;
    stdout_log_level = level;
}

void logToStdoutDisable(void)
{
    log_to_stdout = 0;
    stdout_log_level = LOG_LEVEL_FATAL;
}

void logToFileEnable(const char *file_name, logLevel level)
{
    if (!file_name)
        return;

    if ((log_file = fopen(file_name, "a")) == NULL)
    {
        fprintf(stderr, "Failed to open file: %s\n", file_name);
        return;
    }
    log_to_file = 1;
    file_log_level = level;

}

void logToFileDisable(void)
{
    if (log_file)
    {
        fclose(log_file);
        log_file = NULL;
    }

    log_to_file = 0;
    file_log_level = LOG_LEVEL_FATAL;
}

void stdoutLogging(const char *message)
{
    time_t m_time = time(NULL);
    printf("%s %s\n", ctime(&m_time), message);
}

void fileLogging(const char *message)
{
    time_t m_time = time(NULL);
    fprintf(log_file, "%s %s\n", ctime(&m_time), message);
}

void logging(const char *message, logLevel level)
{
    if (log_to_file && level >= file_log_level)
        fileLogging(message);

    if (log_to_stdout && level >= stdout_log_level)
        stdoutLogging(message);
}
