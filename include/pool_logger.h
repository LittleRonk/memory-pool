/**
 * @file pool_logger.h
 * @brief Use to enable/disable the logging
 */
#ifndef POOL_LOGGER_H
#define POLL_LOGGER_H

#include <logger.h>

/**
 * @brief Enable logging to file
 * @param file_name The name of the file to which logging
 * will be performed
 * @param Settable logging level
 */
void poolEnableLogToFile(const char *file_name, logLevel level);

/**
 * @brief Disable logging to file
 */
void poolDisableLogToFile(void);

/**
 * @brief Enable error messages to stdout
 * @param Settable logging level
 */
void poolEnableLogToStdout(logLevel level);

/**
 * @brief Disable error message to stdout
 */
void poolDisableLogToStdout(void);

#endif // POOL_LOGGER_H
