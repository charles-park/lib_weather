//------------------------------------------------------------------------------
/**
 * @file weather.c
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
#include <ctype.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <locale.h>
#include <math.h>
#include <time.h>

//------------------------------------------------------------------------------
#include "lib_weather.h"

//------------------------------------------------------------------------------
char KorString [WTTR_DATA_SIZE];

//------------------------------------------------------------------------------
// wttr.in data 구조
//------------------------------------------------------------------------------
const char *SubClass[] = {
    "current_condition",
    "nearest_area",
    "request",
    "weather",
};

/* wttr.in request data struct */
wttr_data_t WttrData [] = {
    /* SubClass : current_condition */
    { eWTTR_TEMP_FEEL,  &SubClass[0], "FeelsLikeC",      "0" },  /* 체감온도: "FeelsLikeC": "29" */
    { eWTTR_CLOUD,      &SubClass[0], "cloudcover",      "0" },  /* 운고: "cloudcover": "75" */
    { eWTTR_HUMIDUTY,   &SubClass[0], "humidity",        "0" },  /* 습도: "humidity": "71" */
    { eWTTR_LOBS_DATE,  &SubClass[0], "localObsDateTime","0" },  /* local 측정 시간: "localObsDateTime": "2025-05-20 12:14 PM" */
    { eWTTR_PRECIPI,    &SubClass[0], "precipMM",        "0" },  /* 강수: "precipMM": "0.0" */
    { eWTTR_PRESSURE,   &SubClass[0], "pressure",        "0" },  /* 기압: "pressure": "1010" */
    { eWTTR_TEMP,       &SubClass[0], "temp_C",          "0" },  /* 온도: "temp_C": "27" */
    { eWTTR_UV,         &SubClass[0], "uvIndex",         "0" },  /* 자외선 강도: "uvIndex": "6" */
    { eWTTR_VISIVILITY, &SubClass[0], "visibility",      "0" },  /* 시정: "visibility": "16" */
    { eWTTR_W_CODE,     &SubClass[0], "weatherCode",     "0" },  /* 날씨 코드: "weatherCode": "116" */
    { eWTTR_WIND_DIR,   &SubClass[0], "winddirDegree",   "0" },  /* 풍향: "winddirDegree": "209" */
    { eWTTR_WIND_SPEED, &SubClass[0], "windspeedKmph",   "0" },  /* 풍속: "windspeedKmph": "15" */

    /* SubClass : nearest_area */
    { eWTTR_LATITUDE,   &SubClass[1], "latitude",    "0" },  /* 위도: "latitude": "37.266" */
    { eWTTR_LONGITUDE,  &SubClass[1], "longitude",   "0" },  /* 경도: "longitude": "127.048" */
    { eWTTR_AREA_NAME,  &SubClass[1], "areaName",    "0" },  /* 지역이름: "areaName": [ { "value": "Seryudong" } ] */
    { eWTTR_COUNTRY,    &SubClass[1], "country",     "0" },  /* 국가: "country": [ { "value": "South Korea" } ] */
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// 풍향 Degree → 한글
//------------------------------------------------------------------------------
const char* translate_wind_degree (const char *degree, int is_kor) {
    const char* directions[] = {
        "북", "북북동", "북동", "동북동", "동", "동남동", "남동", "남남동",
        "남", "남남서", "남서", "서남서", "서", "서북서", "북서", "북북서"
    };
    const char* directions_en[] = {
        "N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE",
        "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW"
    };
    int i_degree = atoi(degree);

    // 360도를 16방위로 나누면 22.5도씩
    int index = (int)((i_degree + 11.25) / 22.5) % 16;
    return is_kor ? directions[index] : directions_en[index];
}

//------------------------------------------------------------------------------
// 날씨 코드 변환
//------------------------------------------------------------------------------
const char* translate_weather_code(const char* code, int is_kor) {
    if (strcmp(code, "113") == 0) return is_kor ? "맑음" : "Clear";
    if (strcmp(code, "116") == 0) return is_kor ? "부분적으로 흐림" : "Partly Cloudy";
    if (strcmp(code, "119") == 0) return is_kor ? "흐림" : "Cloudy";
    if (strcmp(code, "122") == 0) return is_kor ? "매우 흐림" : "Overcast";
    if (strcmp(code, "143") == 0) return is_kor ? "안개" : "Mist";
    if (strcmp(code, "176") == 0) return is_kor ? "가벼운 비" : "Patchy rain";
    if (strcmp(code, "179") == 0) return is_kor ? "가벼운 눈비" : "Patchy snow";
    if (strcmp(code, "182") == 0) return is_kor ? "가벼운 비와 눈" : "Patchy sleet";
    if (strcmp(code, "185") == 0) return is_kor ? "얕은 비" : "Patchy freezing drizzle";
    if (strcmp(code, "200") == 0) return is_kor ? "천둥" : "Thundery outbreaks";
    if (strcmp(code, "227") == 0) return is_kor ? "가벼운 눈" : "Blowing snow";
    if (strcmp(code, "230") == 0) return is_kor ? "강한 눈" : "Blizzard";
    if (strcmp(code, "248") == 0) return is_kor ? "안개" : "Fog";
    if (strcmp(code, "260") == 0) return is_kor ? "서리 낀 안개" : "Freezing fog";
    if (strcmp(code, "263") == 0) return is_kor ? "가벼운 이슬비" : "Patchy light drizzle";
    if (strcmp(code, "266") == 0) return is_kor ? "약한 이슬비" : "Light drizzle";
    if (strcmp(code, "281") == 0) return is_kor ? "얕은 이슬비" : "Freezing drizzle";
    if (strcmp(code, "284") == 0) return is_kor ? "강한 이슬비" : "Heavy freezing drizzle";
    if (strcmp(code, "293") == 0) return is_kor ? "가벼운 비" : "Patchy light rain";
    if (strcmp(code, "296") == 0) return is_kor ? "약한 비" : "Light rain";
    if (strcmp(code, "299") == 0) return is_kor ? "가벼운 비" : "Moderate rain at times";
    if (strcmp(code, "302") == 0) return is_kor ? "강한 비" : "Moderate rain";
    if (strcmp(code, "305") == 0) return is_kor ? "소나기" : "Heavy rain at times";
    if (strcmp(code, "308") == 0) return is_kor ? "강한 소나기" : "Heavy rain";
    if (strcmp(code, "311") == 0) return is_kor ? "가벼운 비와 눈" : "Light freezing rain";
    if (strcmp(code, "314") == 0) return is_kor ? "강한 비와 눈" : "Moderate or Heavy freezing rain";
    if (strcmp(code, "317") == 0) return is_kor ? "비와 진눈깨비" : "Light sleet";
    if (strcmp(code, "320") == 0) return is_kor ? "가벼운 진눈깨비" : "Moderate or Heavy sleet";
    if (strcmp(code, "323") == 0) return is_kor ? "가벼운 눈" : "Patchy light snow";
    if (strcmp(code, "326") == 0) return is_kor ? "가끔 눈" : "Light snow";
    if (strcmp(code, "329") == 0) return is_kor ? "많은 눈" : "Patchy moderate snow";
    if (strcmp(code, "332") == 0) return is_kor ? "강한 눈" : "Moderate snow";
    if (strcmp(code, "335") == 0) return is_kor ? "눈보라" : "Patchy heavy snow";
    if (strcmp(code, "338") == 0) return is_kor ? "강한 눈보라" : "Heavy snow";
    if (strcmp(code, "350") == 0) return is_kor ? "우박" : "Ice pellets";
    if (strcmp(code, "353") == 0) return is_kor ? "약한 소나기" : "Light rain shower";
    if (strcmp(code, "356") == 0) return is_kor ? "강한 소나기" : "Moderate or heavy rain shower";
    if (strcmp(code, "359") == 0) return is_kor ? "매우 강한 소나기" : "Torrential rain shower";
    if (strcmp(code, "362") == 0) return is_kor ? "소나기와 눈" : "Light sleet showers";
    if (strcmp(code, "365") == 0) return is_kor ? "강한 소나기와 눈" : "Moderate or heavy sleet showers";
    if (strcmp(code, "368") == 0) return is_kor ? "가끔 눈" : "Light snow showers";
    if (strcmp(code, "371") == 0) return is_kor ? "강한 눈" : "Moderate or heavy snow showers";
    if (strcmp(code, "374") == 0) return is_kor ? "눈 소나기" : "Light showers of ice pellets";
    if (strcmp(code, "377") == 0) return is_kor ? "진눈깨비" : "Showers of ice pellets";
    if (strcmp(code, "386") == 0) return is_kor ? "약한 천둥" : "Patchy light rain with thunder";
    if (strcmp(code, "389") == 0) return is_kor ? "천둥과 비" : "Moderate or heavy rain with thunder";
    if (strcmp(code, "392") == 0) return is_kor ? "천둥과 눈" : "Patchy light snow with thunder";
    if (strcmp(code, "395") == 0) return is_kor ? "강한 천둥과 눈" : "Moderate or heavy snow with thunder";
    return is_kor ? "알 수 없음" : "Unknown";
}

//------------------------------------------------------------------------------
const char* translate_uv_index (const char* index, int is_kor) {
    int i_index = atoi(index);
    switch (i_index) {
        case 0 ... 2:   return is_kor ? "낮음" : "Low";
        case 3 ... 5:   return is_kor ? "보통" : "Moderate";
        case 6 ... 7:   return is_kor ? "높음" : "High";
        case 8 ... 10:  return is_kor ? "매우 높음" : " Very High";
        default:        return is_kor ? "매우 위험함" : "Extreme";
    }
}

//------------------------------------------------------------------------------
// 지역을 한글로 입력시 인코딩(영어도 사용가능)
//------------------------------------------------------------------------------
char *url_encode (const char *str) {
    char *enc = malloc(strlen(str) * 3 + 1);  // 최악의 경우 모든 문자가 %XX로 인코딩됨
    char *penc = enc;

    if (!enc) return NULL;

    while (*str) {
        unsigned char c = (unsigned char)*str;
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            *penc++ = c;
        } else {
            sprintf(penc, "%%%02X", c);
            penc += 3;
        }
        str++;
    }
    *penc = '\0';
    return enc;
}

//------------------------------------------------------------------------------
// 숫자를 한글로 출력
//------------------------------------------------------------------------------
void int_to_kor_buf (int num, char* output) {
    const char *digits[] = { "영", "일", "이", "삼", "사", "오", "육", "칠", "팔", "구" };
    char buffer[64] = "", temp[8];
    int digit, pos;

    sprintf(buffer, "%d", num);
    if (output) memset (output, 0, WTTR_DATA_SIZE);

    if (!num)   {
        sprintf (output, "%s", digits[0]);
        return;
    }

    for (size_t i = 0, place = strlen(buffer); i < strlen(buffer); i++, place--) {
        digit = buffer[i] - '0';
        temp[0] = '\0';

        if (digit) {
            pos = (digit == 1) ? 0 : sprintf(temp, "%s", digits[digit]);
            switch (place) {
                case 5: sprintf(&temp[pos], "%s", "만");  break;
                case 4: sprintf(&temp[pos], "%s", "천");  break;
                case 3: sprintf(&temp[pos], "%s", "백");  break;
                case 2: sprintf(&temp[pos], "%s", "십");  break;
            }
            strcat(output, temp);
        }
    }
}

const char *int_to_kor (int num) {
    const char *digits[] = { "영", "일", "이", "삼", "사", "오", "육", "칠", "팔", "구" };
    char buffer[64] = "", temp[8];
    int digit, pos;

    sprintf(buffer, "%d", num);
    memset (KorString, 0, WTTR_DATA_SIZE);

    if (!num)   {
        sprintf (KorString, "%s", digits[0]);
        return (const char *)&KorString[0];
    }

    for (size_t i = 0, place = strlen(buffer); i < strlen(buffer); i++, place--) {
        digit = buffer[i] - '0';
        temp[0] = '\0';

        if (digit) {
            pos = (digit == 1) ? 0 : sprintf(temp, "%s", digits[digit]);
            switch (place) {
                case 5: sprintf(&temp[pos], "%s", "만");  break;
                case 4: sprintf(&temp[pos], "%s", "천");  break;
                case 3: sprintf(&temp[pos], "%s", "백");  break;
                case 2: sprintf(&temp[pos], "%s", "십");  break;
            }
            strcat(KorString, temp);
        }
    }
    return (const char *)&KorString[0];
}

//------------------------------------------------------------------------------
// 현재시간이나 입력되어진 시간중 원하는 필드의 한글 값을 얻어온다.
//------------------------------------------------------------------------------
void date_to_kor_buf (enum eDayItem d_item, void *i_time, char *k_str)
{
    struct tm *lt;
    time_t t = time(NULL);
    const char *weekday_korean[] = { "일", "월", "화", "수", "목", "금", "토" };
    const char *hour_korean[] = {"영", "한", "두", "세", "네", "다섯", "여섯", "일곱", "여덟", "아홉", "열", "열한", "열두"};

    setenv("TZ", "Asia/Seoul", 1);  // 타임존 설정

    if (i_time == NULL) lt = localtime(&t);
    else                lt = (struct tm *)i_time;

    if (k_str)  memset (k_str, 0, WTTR_DATA_SIZE);

    switch (d_item) {
        case eDAY_AM_PM:
            strncpy (k_str, (lt->tm_hour < 12) ? "오전" : "오후", strlen ("오전"));
            break;

        case eDAY_SEC:      int_to_kor_buf (lt->tm_sec, k_str);           break;
        case eDAY_MIN:      int_to_kor_buf (lt->tm_min, k_str);           break;

        case eDAY_HOUR:
            int hour = (lt->tm_hour == 12) ? 12 : lt->tm_hour % 12;
            strncpy (k_str, hour_korean[hour], strlen (hour_korean[hour]));
            break;

        case eDAY_W_DAY:
            strncpy (k_str, weekday_korean[lt->tm_wday], strlen (weekday_korean[lt->tm_wday]));
            break;

        case eDAY_DAY:      int_to_kor_buf (lt->tm_mday, k_str);          break;
        case eDAY_MONTH:    int_to_kor_buf (lt->tm_mon  + 1,    k_str);   break;
        case eDAY_YEAR:     int_to_kor_buf (lt->tm_year + 1900, k_str);   break;
        default :
            break;
    }
}

const char *date_to_kor (enum eDayItem d_item, void *i_time)
{
    struct tm *lt;
    time_t t = time(NULL);
    const char *weekday_korean[] = { "일", "월", "화", "수", "목", "금", "토" };
    const char *hour_korean[] = {"영", "한", "두", "세", "네", "다섯", "여섯", "일곱", "여덟", "아홉", "열", "열한", "열두"};

    setenv("TZ", "Asia/Seoul", 1);  // 타임존 설정

    memset (KorString, 0, WTTR_DATA_SIZE);

    if (i_time == NULL) lt = localtime(&t);
    else                lt = (struct tm *)i_time;

    switch (d_item) {
        case eDAY_AM_PM:
            strncpy (KorString, (lt->tm_hour < 12) ? "오전" : "오후", strlen ("오전"));
            break;

        case eDAY_SEC:      int_to_kor_buf (lt->tm_sec, KorString);   break;
        case eDAY_MIN:      int_to_kor_buf (lt->tm_min, KorString);   break;

        case eDAY_HOUR:
            int hour = (lt->tm_hour == 12) ? 12 : lt->tm_hour % 12;
            strncpy (KorString, hour_korean[hour], strlen (hour_korean[hour]));
            break;

        case eDAY_W_DAY:
            strncpy (KorString, weekday_korean[lt->tm_wday], strlen (weekday_korean[lt->tm_wday]));
            break;

        case eDAY_DAY:      int_to_kor_buf (lt->tm_mday, KorString);          break;
        case eDAY_MONTH:    int_to_kor_buf (lt->tm_mon  + 1,    KorString);   break;
        case eDAY_YEAR:     int_to_kor_buf (lt->tm_year + 1900, KorString);   break;
        default :
            break;
    }
    return (const char *)&KorString[0];
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// 메모리 버퍼
//------------------------------------------------------------------------------
struct MemoryStruct {
    char *memory;
    size_t size;
};

//------------------------------------------------------------------------------
// cURL 콜백
//------------------------------------------------------------------------------
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (!ptr) return 0;

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

//------------------------------------------------------------------------------
// 위,경도에 위치한 도시/지역 요청
//------------------------------------------------------------------------------
void get_location_json (double lat, double lon, char *g_city, char *g_country, int is_kor)
{
    CURL *curl;
    CURLcode res;
    struct MemoryStruct chunk = {malloc(1), 0};
    char url[512];

    #if defined (__LIB_WEATHER_DEBUG__)
        printf("lat = %f, lon = %f, is_kor = %d\n", lat, lon, is_kor);
    #endif

    snprintf (url, sizeof(url), is_kor ? LOCATION_URL_FORMAT_KR : LOCATION_URL_FORMAT_EN, lat, lon);

    curl_global_init(CURL_GLOBAL_ALL);
    if (!(curl = curl_easy_init())) return;

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "C-Geocoder/1.0");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

    if ((res = curl_easy_perform(curl)) != CURLE_OK) {
        fprintf(stderr, "curl 요청 실패: %s\n", curl_easy_strerror(res));
    } else {
        cJSON *json = cJSON_Parse(chunk.memory);
        if (json) {
            cJSON *address = cJSON_GetObjectItemCaseSensitive(json, "address");
            if (address) {
                const char *city = NULL, *state = NULL, *country = NULL;

                cJSON *fields_city[] = {
                    cJSON_GetObjectItemCaseSensitive(address, "city"),
                    cJSON_GetObjectItemCaseSensitive(address, "town"),
                    cJSON_GetObjectItemCaseSensitive(address, "village"),
                    cJSON_GetObjectItemCaseSensitive(address, "county")
                };
                for (int i = 0; i < 4; i++) {
                    if (fields_city[i]) {
                        city = fields_city[i]->valuestring;
                        break;
                    }
                }

                state = cJSON_GetObjectItemCaseSensitive(address, "state") ?
                        cJSON_GetObjectItemCaseSensitive(address, "state")->valuestring : NULL;

                country = cJSON_GetObjectItemCaseSensitive(address, "country") ?
                          cJSON_GetObjectItemCaseSensitive(address, "country")->valuestring : NULL;

                #if defined (__LIB_WEATHER_DEBUG__)
                    printf("\n[위치 정보]\n");
                    printf("도시:   %s\n", city ? city : "(정보 없음)");
                    printf("지역:   %s\n", state ? state : "(정보 없음)");
                    printf("국가:   %s\n", country ? country : "(정보 없음)");
                #endif

                memset  (g_city,       0,    strlen(city)+1);
                strncpy (g_city,    city,    strlen(city));
                memset  (g_country,    0,    strlen(country)+1);
                strncpy (g_country, country, strlen(country));

            } else {
                fprintf(stderr, "주소 정보 없음\n");
                memset  (g_city,       0,    1);
                memset  (g_country,    0,    1);
            }
            cJSON_Delete(json);
        } else {
            fprintf(stderr, "JSON 파싱 실패\n");
        }
    }
    free(chunk.memory);
    curl_easy_cleanup(curl);
}

//------------------------------------------------------------------------------
// HTTP 요청 (location = 지역명(한글/영어), "위도,경도")
//------------------------------------------------------------------------------
char *get_weather_json (const char *location)
{
    CURL *curl;
    CURLcode res;
    struct MemoryStruct chunk = {malloc(1), 0};

    #if defined (__LIB_WEATHER_DEBUG__)
        printf("입력지역: %s\n", location[0] ? location : "현위치");
    #endif

    // location 인코딩 (한글/영문 지역 사용가능)
    char *encoded_location = url_encode(location && strlen(location) > 0 ? location : "");

    char url[512];
    snprintf(url, sizeof(url), WEATHER_URL_FORMAT, encoded_location);
    free(encoded_location);

    curl_global_init(CURL_GLOBAL_ALL);
    if (!(curl = curl_easy_init())) return NULL;

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    if ((res = curl_easy_perform(curl)) != CURLE_OK) {
        free(chunk.memory);
        curl_easy_cleanup(curl);
        return NULL;
    }

    curl_easy_cleanup(curl);
    return chunk.memory;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Json 날씨데이터 파싱 및 저장(WttrData Struct)
//------------------------------------------------------------------------------
void parse_weather(const char *json)
{
    cJSON *root = cJSON_Parse(json);
    if (!root) {
        fprintf(stderr, "JSON 파싱 실패\n");
        return;
    }

    for (size_t i = 0; i < (sizeof (WttrData) / sizeof (WttrData[0])); i++) {
        cJSON *current = cJSON_GetObjectItem(root, *WttrData[i].sub_class);

        if (!cJSON_IsArray(current) || cJSON_GetArraySize(current) == 0) {
            cJSON_Delete(root);
            fprintf(stderr, "날씨 정보 없음\n");
            return;
        }

        cJSON *info = cJSON_GetArrayItem(current, 0);

        if  ((strncmp(WttrData[i].item_str, "areaName", strlen("areaName")) == 0) ||
             (strncmp(WttrData[i].item_str, "country" , strlen("country"))  == 0) )  {
            const cJSON *arr = cJSON_GetObjectItem(info, WttrData[i].item_str);
            const cJSON *obj = cJSON_GetArrayItem (arr, 0);

            strncpy (
                WttrData[i].data_str,
                cJSON_GetObjectItem(obj, "value")->valuestring,
                strlen(cJSON_GetObjectItem(obj, "value")->valuestring)
            );
        }
        else {
            /* Get weather data from json arrary */
            strncpy (
                WttrData[i].data_str,
                cJSON_GetObjectItem(info, WttrData[i].item_str)->valuestring,
                strlen(cJSON_GetObjectItem(info, WttrData[i].item_str)->valuestring)
            );
        }
        #if defined (__LIB_WEATHER_DEBUG__)
            /* Data print */
            printf ("%s : %s, %s, %s\n", __func__,
                *WttrData[i].sub_class, WttrData[i].item_str, WttrData[i].data_str);
        #endif
    }
}

//------------------------------------------------------------------------------
// 측정시간[eWTTR_LOBS_DATE] (WttrData struct) 데이터 변환
//------------------------------------------------------------------------------
void get_wttr_date (const char *obs_data, struct tm *t)
{
    // Local Obs Data : "2025-05-20 12:14 PM"
    strptime (obs_data, "%Y-%m-%d %I:%M %p", t);
    mktime (t);  // 자동으로 overflow 조정

    #if defined (__LIB_WEATHER_DEBUG__)
        printf("관측 시각: %d년 %d월 %d일 %d시 %d분 %d요일 %d일/356일\n",
            t->tm_year + 1900,
            t->tm_mon + 1,
            t->tm_mday,
            t->tm_hour,
            t->tm_min,
            t->tm_wday,
            t->tm_yday + 1
        );
    #endif
}

//------------------------------------------------------------------------------
// WttrData Struct data 요청
//------------------------------------------------------------------------------
const char *get_wttr_data (enum eWttrItem id)
{
    size_t cnt;

    for (cnt = 0; cnt < sizeof (WttrData)/sizeof (WttrData[0]); cnt++) {
        if (WttrData[cnt].id == id) return WttrData[cnt].data_str;
    }
    return NULL;
}

//------------------------------------------------------------------------------
// location = 지역명 (한글/영어)
//------------------------------------------------------------------------------
int update_weather_data (const char *location)
{
    char *json = get_weather_json (location);

    if (!json) {
        fprintf (stderr, "날씨 정보를 가져올 수 없습니다.\n");
        return 0;
    }
    #if defined (__LIB_WEATHER_DEBUG__)
        printf ("서버 응답 내용:\n%s\n", json);
    #endif

    parse_weather(json);
    free(json);

    return 1;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
