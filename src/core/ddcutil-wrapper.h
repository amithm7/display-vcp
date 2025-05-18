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

#endif