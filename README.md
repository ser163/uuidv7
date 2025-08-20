# uuidv7

PHP extension for generating UUID v7 identifiers compliant with https://datatracker.ietf.org/doc/html/rfc9562

## 🐧 **1. Linux Compilation Steps**

### Generate Extension  
Execute in the PHP source directory to create extension skeleton files:  
```shell
cd /www/server/php/74/src/ext
php ext_skel.php --ext uuidv7
```

### Compile Extension  
```shell
phpize
./configure --enable-uuidv7 --with-php-config=/www/server/php/74/bin/php-config
make
sudo make install
```

### Modify `php.ini`  
```ini
extension=uuidv7.so
```

### Test Script  
```php
<?php
// uuid.php
$str = uuidv7_string();
$bin = uuidv7_binary();
echo "String: " . $str . PHP_EOL;
echo "Binary: " . bin2hex($bin) . PHP_EOL;
?>
```
**Output**:  
```
String: 0198c6d4-2b6f-7019-a535-c14fae293910
Binary: 0198c6d42b6f7019afbd8f34660b69f2
```

---

## 🛠️ **2. Windows Compilation Steps**
#### **Environment Setup**  
1. Install dependencies:  
   - https://windows.php.net/downloads/php-sdk/ (match VS version)  
   - https://slproweb.com/products/Win32OpenSSL.html (`Win64` version)  
   - Visual Studio (e.g., VS 2019)  

2. Configure environment variables:  
   ```bat
   set PHP_SDK=%CD%\php-sdk
   set OPENSSL_DIR=C:\OpenSSL-Win64   # Replace with actual path
   ```

#### **Compilation Commands (VS Developer Command Prompt)**  
```bat
:: 1. Generate extension skeleton
php-sdk-bind-tool -d
cd %PHP_SDK%\phpsrc
phpsdk_buildtree uuidv7

:: 2. Copy source to extension directory
copy uuidv7.c %PHP_SDK%\phpsrc\ext\uuidv7\
copy php_uuidv7.h %PHP_SDK%\phpsrc\ext\uuidv7\

:: 3. Configure build options
cd %PHP_SDK%\phpsrc\ext\uuidv7
phpsdk_deps -u
phpsdk_config --disable-all --enable-cli --enable-uuidv7=shared

:: 4. Modify config.w32 (add OpenSSL link)
echo 'ARG_WITH("uuidv7", "Enable UUID v7 support", "yes");' >> config.w32
echo 'if (PHP_UUIDV7 != "no") {' >> config.w32
echo '    ADD_EXTENSION_DEP("uuidv7", "openssl");' >> config.w32
echo '    ADD_SOURCES("uuidv7.c");' >> config.w32
echo '}' >> config.w32

:: 5. Compile extension
nmake
```

#### **Output**  
- After successful compilation, locate `php_uuidv7.dll` in `Release_TS` or `Debug_TS` directory  
- Copy this file to PHP extensions directory (e.g., `C:\php\ext\`)  

#### **Enable Extension**  
Add to `php.ini`:  
```ini
extension=php_uuidv7.dll
```

---

### ✅ **3. Functional Verification (Windows/Linux)**  
```php
<?php
// Generate binary format (for MySQL BINARY(16))
$bin = uuidv7_binary();
echo "Binary: " . bin2hex($bin) . PHP_EOL;

// Generate string format
$str = uuidv7_string();
echo "String: $str" . PHP_EOL;

// Verify timestamp (first 6 bytes = Unix millis)
$timestamp = hexdec(bin2hex(substr($bin, 0, 6)));
echo "Timestamp: " . date('Y-m-d H:i:s', $timestamp / 1000);
?>
```

---

### ⚠️ **Critical Considerations**  
1. **OpenSSL Dependency**:  
   - Windows: Manually install OpenSSL and set `OPENSSL_DIR`  
   - Linux: Install dev package via `sudo apt install libssl-dev`  

2. **Cross-Platform Differences**:  
   - Time acquisition: `GetSystemTimeAsFileTime` (Windows) vs. `gettimeofday` (Linux)  
   - Sleep function: `Sleep(ms)` (Windows) vs. `usleep(microseconds)` (Linux)  

3. **RFC 9562 Compliance**:  
   - **Timestamp layout**: First 6 bytes store Unix millis in big-endian  
   - **Version bits**: Upper 4 bits of byte 6 fixed as `0x7` (binary `0111`)  
   - **Variant bits**: Upper 2 bits of byte 8 fixed as `10` (binary `10xxxxxx`)  

4. **Performance Optimization**:  
   - 14-bit sequence counter for same-millisecond collisions  
   - 32-bit random expansion during clock rollback  

> Verify loading via `php -m | findstr uuidv7` (Windows) or `php -m | grep uuidv7` (Linux). If loading fails, check OpenSSL DLL paths (Windows) or file permissions (Linux).