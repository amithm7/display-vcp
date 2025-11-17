#ifndef DDCUTIL_WRAPPER_H
#define DDCUTIL_WRAPPER_H

#include <QString>

/**
 * @brief Get the VCP value using ddcutil
 *
 * @param vcpCode VCP code in hexadecimal format e.g. `"E2"`
 * @return set VCP value in decimal format
 */
short getVCPValue(QString vcpCode);

/**
 * @brief Set the VCP value using ddcutil
 *
 * @param vcpCode VCP code in hexadecimal format e.g. `"E2"`
 * @param value Value to set in decimal format
 * @return int Exit code of the process
 */
int setVCPValue(QString vcpCode, short value);

/**
 * @brief Get the VCP value asynchronously using ddcutil
 *
 * @param vcpCode VCP code in hexadecimal format e.g. `"E2"`
 * @param callback Function to call with the retrieved value
 */
void getVCPValueAsync(QString vcpCode, std::function<void(short)> callback);

/**
 * @brief Set the VCP value asynchronously using ddcutil
 *
 * @param vcpCode VCP code in hexadecimal format e.g. `"E2"`
 * @param value Value to set in decimal format
 * @param callback Function to call with the exit code
 */
void setVCPValueAsync(QString vcpCode, short value, std::function<void(int)> callback);

#endif