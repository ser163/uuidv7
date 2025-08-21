#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_uuidv7.h"
#include <time.h>
#include <openssl/rand.h>

#ifdef _WIN32
#include <windows.h>
#include <winsock2.h>
#else
#include <sys/time.h>
#endif

ZEND_BEGIN_ARG_INFO(arginfo_void, 0)
ZEND_END_ARG_INFO()

const zend_function_entry uuidv7_functions[] = {
    PHP_FE(uuidv7_string, arginfo_void)
    PHP_FE(uuidv7_binary, arginfo_void)
    PHP_FE_END
};

zend_module_entry uuidv7_module_entry = {
    STANDARD_MODULE_HEADER,
    "uuidv7",                    
    uuidv7_functions,            
    NULL,                        
    NULL,                       
    NULL,                       
    NULL,                       
    NULL,                        
    PHP_UUIDV7_VERSION,         
    STANDARD_MODULE_PROPERTIES
};

ZEND_GET_MODULE(uuidv7)

static uint64_t last_timestamp = 0;
static uint16_t sequence_counter = 0;

static uint64_t get_current_timestamp() {
#ifdef _WIN32
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    uint64_t timestamp = ((uint64_t)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
    return (timestamp / 10000) - 11644473600000ULL; 
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
#endif
}

static int generate_uuidv7_bytes(unsigned char *bytes) {
    uint64_t timestamp = get_current_timestamp();

    if (timestamp < last_timestamp) {
        uint64_t delta = last_timestamp - timestamp;
        if (delta > 10000) { 
            php_error_docref(NULL, E_ERROR, "Clock moved backwards by %llums", delta);
            return 0;
        }
        timestamp = last_timestamp;
        RAND_bytes(bytes + 12, 4);
    }


    if (timestamp == last_timestamp) {
        sequence_counter++;
        if (sequence_counter >= 0x3FFF) { 
            php_error_docref(NULL, E_WARNING, "Sequence counter overflow");
#ifdef _WIN32
            Sleep(1);
#else
            usleep(1000);
#endif
            timestamp = get_current_timestamp();
            sequence_counter = 0;
        }
    } else {
        sequence_counter = 0;
    }
    last_timestamp = timestamp;


    bytes[0] = (timestamp >> 40) & 0xFF;
    bytes[1] = (timestamp >> 32) & 0xFF;
    bytes[2] = (timestamp >> 24) & 0xFF;
    bytes[3] = (timestamp >> 16) & 0xFF;
    bytes[4] = (timestamp >> 8) & 0xFF;
    bytes[5] = timestamp & 0xFF; 

    bytes[6] = 0x70 | ((timestamp >> 44) & 0x0F);


    bytes[7] = sequence_counter & 0xFF;
    bytes[8] = 0x80 | ((sequence_counter >> 8) & 0x3F);


    int retry = 0;
    while (retry < 3) {
        if (RAND_bytes(bytes + 9, 7) == 1) {
            return 1; 
        }
        php_error_docref(NULL, E_WARNING, "RAND_bytes failed, retry %d/3", retry + 1);
        retry++;
#ifdef _WIN32
        Sleep(retry); 
#else
        usleep(retry * 1000);
#endif
    }
    php_error_docref(NULL, E_ERROR, "RAND_bytes failed after 3 attempts");
    return 0;
}


PHP_FUNCTION(uuidv7_binary) {
    unsigned char bytes[16];
    if (!generate_uuidv7_bytes(bytes)) {
        RETURN_FALSE;
    }
    RETURN_STRINGL((char *)bytes, 16);
}


PHP_FUNCTION(uuidv7_string) {
    unsigned char bytes[16];
    if (!generate_uuidv7_bytes(bytes)) {
        RETURN_FALSE;
    }
    char uuid_str[37];
    snprintf(uuid_str, sizeof(uuid_str),
        "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
        bytes[0], bytes[1], bytes[2], bytes[3], bytes[4], bytes[5],
        bytes[6], bytes[7], bytes[8], bytes[9], bytes[10], bytes[11],
        bytes[12], bytes[13], bytes[14], bytes[15]
    );
    RETURN_STRING(uuid_str);
}