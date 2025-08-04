/**
 * @file logger.h
 * @brief Error logging
 */

#ifndef LOGGER_H
#define LOGGER_H

typedef enum {
    LOG_LEVEL_DEBUG = 0,    // Maximum information
    LOG_LEVEL_INFO,         // Statuses and events
    LOG_LEVEL_WARN,         // Suspicious but acceptable
    LOG_LEVEL_ERROR,        // Error, but the program will not crash
    LOG_LEVEL_FATAL         // Fatal error, the program will be forcibly crashed
} logLevel;

/**
 * @brief Logging the message
 * @param message The message to be logged
 * @param level Level of the message being transmitted
 */
void logging(const char *message, logLevel level);

/**
 * @brief Sends error messages to standart output
 * @param message Error message
 */
void stdoutLogging(const char *message);

/**
 * @brief Send error messagess to a file
 * @param message Error message
 */
void fileLogging(const char *message);

/**
 * @brief Enable sending error messages to standard output
 * @param level Settable logging level
 */
void logToStdoutEnable(logLevel level);

/**
 * @brief Disable sending error message to standart output
 */
void logToStdoutDisable(void);

/**
 * @brief Enable sending error messages to a file
 * @param file_name Path to the file to which messages should be written
 * @param level Settable logging level
 */
void logToFileEnable(const char *file_name, logLevel level);

/**
 * @brief Disable sending error messages to a file
 */
void logToFileDisable(void);

#endif // LOGGER_H
