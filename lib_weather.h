//------------------------------------------------------------------------------
/**
 * @file weather.h
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
//------------------------------------------------------------------------------
#define WEATHER_URL_FORMAT      "http://wttr.in/%s?format=j1"
#define LOCATION_URL_FORMAT_KR  "https://nominatim.openstreetmap.org/reverse?format=json&lat=%f&lon=%f&zoom=10&accept-language=ko"
#define LOCATION_URL_FORMAT_EN  "https://nominatim.openstreetmap.org/reverse?format=json&lat=%f&lon=%f&zoom=10&accept-language=en"
#define DEFAULT_LOCATION ""

//------------------------------------------------------------------------------
// 현재시간이나 입력되어진 시간중 원하는 필드의 한글 값을 얻어온다.
//------------------------------------------------------------------------------
enum eDayItem {
    eDAY_AM_PM,
    eDAY_SEC,
    eDAY_MIN,
    eDAY_HOUR,
    eDAY_W_DAY,
    eDAY_DAY,
    eDAY_MONTH,
    eDAY_YEAR,
    eDAY_END
};

//------------------------------------------------------------------------------
// wttr.in data list
//------------------------------------------------------------------------------
enum eWttrItem {
    /* current_condition */
    eWTTR_TEMP_FEEL = 0, /* 체감온도: "FeelsLikeC": "29" */
    eWTTR_CLOUD,         /* 운고: "cloudcover": "75" */
    eWTTR_HUMIDUTY,      /* 습도: "humidity": "71" */
    eWTTR_LOBS_DATE,     /* local 측정 시간: "localObsDateTime": "2025-05-20 12:14 PM" */
    eWTTR_PRECIPI,       /* 강수: "precipMM": "0.0" */
    eWTTR_PRESSURE,      /* 기압: "pressure": "1010" */
    eWTTR_TEMP,          /* 온도: "temp_C": "27" */
    eWTTR_UV,            /* 자외선 강도: "uvIndex": "6" */
    eWTTR_VISIVILITY,    /* 시정: "visibility": "16" */
    eWTTR_W_CODE,        /* 날씨 코드: "weatherCode": "116" */
    eWTTR_WIND_DIR,      /* 풍향: "winddirDegree": "209" */
    eWTTR_WIND_SPEED,    /* 풍속: "windspeedKmph": "15" */

    /* nearest_area */
    eWTTR_AREA_NAME = 100,  /* 지역이름: "areaName": [ { "value": "Seryudong" } ] */
    eWTTR_COUNTRY,          /* 국가: "country": [ { "value": "South Korea" } ] */
    eWTTR_LATITUDE,         /* 위도: "latitude": "37.266" */
    eWTTR_LONGITUDE,        /* 경도: "longitude": "127.048" */

    /* request */
    eWTTR_REQUEST = 200,

    /* weather */
    eWTTR_WEATHER = 300,
    eWTTR_END
};

#define WTTR_DATA_SIZE   32

typedef struct wttr_data__t {
    enum eWttrItem id;
    const char **sub_class;
    const char *item_str;
    char data_str [WTTR_DATA_SIZE];

}   wttr_data_t;

//------------------------------------------------------------------------------
#if 0
서버 응답 내용:
{
    "current_condition": [
        {
            "FeelsLikeC": "29",
            "FeelsLikeF": "85",
            "cloudcover": "75",
            "humidity": "71",
            "localObsDateTime": "2025-05-20 12:14 PM",
            "observation_time": "03:14 AM",
            "precipInches": "0.0",
            "precipMM": "0.0",
            "pressure": "1010",
            "pressureInches": "30",
            "temp_C": "27",
            "temp_F": "80",
            "uvIndex": "6",
            "visibility": "16",
            "visibilityMiles": "9",
            "weatherCode": "116",
            "weatherDesc": [
                {
                    "value": "Partly cloudy"
                }
            ],
            "weatherIconUrl": [
                {
                    "value": ""
                }
            ],
            "winddir16Point": "SSW",
            "winddirDegree": "209",
            "windspeedKmph": "15",
            "windspeedMiles": "10"
        }
    ],
    "nearest_area": [
        {
            "areaName": [
                {
                    "value": "Seryudong"
                }
            ],
            "country": [
                {
                    "value": "South Korea"
                }
            ],
            "latitude": "37.266",
            "longitude": "127.048",
            "population": "0",
            "region": [
                {
                    "value": ""
                }
            ],
            "weatherUrl": [
                {
                    "value": ""
                }
            ]
        }
    ],
#endif

//------------------------------------------------------------------------------
extern void         get_location_json   (double lat, double lon, char *g_city, char *g_country, int is_kor);
extern const char   *get_wttr_data      (enum eWttrItem id);
//------------------------------------------------------------------------------
// location = 지역명 (한글/영어)
//------------------------------------------------------------------------------
extern int          update_weather_data (const char *location);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
