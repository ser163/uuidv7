#ifndef PHP_UUIDV7_H
#define PHP_UUIDV7_H

extern zend_module_entry uuidv7_module_entry;
#define phpext_uuidv7_ptr &uuidv7_module_entry

#define PHP_UUIDV7_VERSION "1.0.0"

#ifdef PHP_WIN32
#   define PHP_UUIDV7_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#   define PHP_UUIDV7_API __attribute__ ((visibility("default")))
#else
#   define PHP_UUIDV7_API
#endif

#ifdef ZTS
#   include "TSRM.h"
#endif

PHP_FUNCTION(uuidv7_string);
PHP_FUNCTION(uuidv7_binary);

#endif /* PHP_UUIDV7_H */