//------------------------------------------------------------------------------
/**
 * @file main.c
 * @author charles-park (charles.park@hardkernel.com)
 * @brief ODROID-C5 Weather station App.
 * @version 2.0
 * @date 2025-05-14
 *
 * @package apt install libcurl4-openssl-dev libcjson-dev
 *
 * @copyright Copyright (c) 2022
 *
 */
//------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//------------------------------------------------------------------------------
#include "lib_weather.h"

//------------------------------------------------------------------------------
int main(int argc, char *argv[]) {
    request_weather ((argc == 2) ? argv[1] : "");
    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

