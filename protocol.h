#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QtGlobal>

namespace Protocol {
    const char* const CMD_REG = "reg";
    const char* const CMD_AUTH = "auth";
    const char* const CMD_SHA256 = "sha256";
    const char* const CMD_SPLINE = "spline";
    const char* const CMD_GD = "gd";
    const char* const CMD_USERS = "users";
    
    const char* const RSP_REG_OK = "REG_OK:";
    const char* const RSP_REG_ERR = "REG_ERR:";
    const char* const RSP_AUTH_OK = "AUTH_OK:";
    const char* const RSP_AUTH_ERR = "AUTH_ERR:";
    const char* const RSP_SHA256_OK = "SHA256_OK:";
    const char* const RSP_SHA256_ERR = "SHA256_ERR:";
    const char* const RSP_SPLINE_OK = "SPLINE_OK:";
    const char* const RSP_SPLINE_ERR = "SPLINE_ERR:";
    const char* const RSP_GD_OK = "GD_OK:";
    const char* const RSP_GD_ERR = "GD_ERR:";
    const char* const RSP_USERS_OK = "USERS_OK:";
    const char* const RSP_USERS_ERR = "USERS_ERR:";
    const char* const RSP_ERR = "ERR:";
    
    const char DELIM_CMD = '&';
    const char DELIM_ARGS = ',';
    const quint16 SERVER_PORT = 33333;
}

#endif
