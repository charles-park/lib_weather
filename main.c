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
#if defined(__LIB_WEATHER_APP__)
//------------------------------------------------------------------------------
int main(int argc, char *argv[]) {

    char location [WTTR_DATA_SIZE];

    memset (location, 0, sizeof(location));

    switch (argc) {
        /* 지역명 (한글, 영어 사용가능) */
        case 2:     sprintf (location, "%s", strlen(argv[1]) ? argv[1] : "");   break;
        /* 위치명 (위도, 경도 입력) */
        case 3:     sprintf (location, "%s,%s", argv[1], argv[2]);              break;
        /* 현 위치 (IP위치 검색) */
        default :   sprintf (location, "%s", "");                               break;
    }

    if (update_weather_data (location)) {
        char city[WTTR_DATA_SIZE], country[WTTR_DATA_SIZE];

        /* 측정되어진 wttr 좌표 데이터*/
        printf ("Lati : %s, Longi : %s\n", get_wttr_data (eWTTR_LATITUDE), get_wttr_data (eWTTR_LONGITUDE));

        /* 좌표기준으로 위치 검색 */
        get_location_json (
            atof(get_wttr_data (eWTTR_LATITUDE)),
            atof(get_wttr_data (eWTTR_LONGITUDE)),
            city, country, 1);

        printf ("Korean : city(%s), country(%s)\n", city, country);

        get_location_json (
            atof(get_wttr_data (eWTTR_LATITUDE)),
            atof(get_wttr_data (eWTTR_LONGITUDE)),
            city, country, 0);

        printf ("English : city(%s), country(%s)\n", city, country);
    }
    return 0;
}
//------------------------------------------------------------------------------
#endif  // __LIB_WEATHER_APP__
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

