/** @file common.h
*
* @brief common includes
*
* @author Alvaro Prieto
*/
#ifndef _COMMON_H
#define _COMMON_H

#include <io.h>
#include <stdint.h>

#define debug(x) uart_write( x, strlen(x) )

#endif /* _COMMON_H */\

