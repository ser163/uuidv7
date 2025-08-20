PHP_ARG_ENABLE(uuidv7, Enable uuidv7 support,
[ --enable-uuidv7   Enable UUID v7 extension support])

if test "$PHP_UUIDV7" != "no"; then
    PHP_REQUIRE_CXX()
    PHP_ADD_LIBRARY(ssl, 1, UUIDV7_SHARED_LIBADD) # 依赖 OpenSSL
    PHP_NEW_EXTENSION(uuidv7, uuidv7.c, $ext_shared)
fi