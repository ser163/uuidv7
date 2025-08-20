# uuidv7

直接生成php扩展 UUID v7 id 符合（RFC9562）[https://datatracker.ietf.org/doc/html/rfc9562]

## 🐧 **1. Linux 编译步骤**

### 生成扩展

进入源码目录利用`ext_skel.php` 生成扩展示例文件.

```shell
/www/server/php/74/src/ext
php ext_skel.php --ext uuidv7
```
### 编译扩展
```shell
phpize
./configure --enable-uuidv7 --with-php-config=/www/server/php/74/bin/php-config
make
sudo make install

```

### 修改`php.ini`文件
```ini
extension=uuidv7.so
```

### 测试文件
```php
[root@Book2 ~]# php uuid.php
String: 0198c6d4-2b6f-7019-a535-c14fae293910
Binary: 0198c6d42b6f7019afbd8f34660b69f2
```

---

## 🛠️ **2. Windows 编译步骤**
#### **环境准备**
1. 安装依赖：
   - https://windows.php.net/downloads/php-sdk/（选择匹配的 VS 版本）
   - https://slproweb.com/products/Win32OpenSSL.html（选择 `Win64` 版）
   - Visual Studio（如 VS 2019）

2. 配置环境变量：
   ```bat
   set PHP_SDK=%CD%\php-sdk
   set OPENSSL_DIR=C:\OpenSSL-Win64   # 替换为实际路径
   ```

#### **编译命令（VS 开发者命令提示符）**
```bat
:: 1. 生成扩展骨架
php-sdk-bind-tool -d
cd %PHP_SDK%\phpsrc
phpsdk_buildtree uuidv7

:: 2. 复制源码到扩展目录
copy uuidv7.c %PHP_SDK%\phpsrc\ext\uuidv7\
copy php_uuidv7.h %PHP_SDK%\phpsrc\ext\uuidv7\

:: 3. 配置编译选项
cd %PHP_SDK%\phpsrc\ext\uuidv7
phpsdk_deps -u
phpsdk_config --disable-all --enable-cli --enable-uuidv7=shared

:: 4. 修改 config.w32（添加 OpenSSL 链接）
echo 'ARG_WITH("uuidv7", "Enable UUID v7 support", "yes");' >> config.w32
echo 'if (PHP_UUIDV7 != "no") {' >> config.w32
echo '    ADD_EXTENSION_DEP("uuidv7", "openssl");' >> config.w32
echo '    ADD_SOURCES("uuidv7.c");' >> config.w32
echo '}' >> config.w32

:: 5. 编译扩展
nmake
```

#### **输出结果**
- 编译成功后，在 `Release_TS` 或 `Debug_TS` 目录生成 `php_uuidv7.dll`
- 复制此文件到 PHP 扩展目录（如 `C:\php\ext\`）

#### **启用扩展**
在 `php.ini` 中添加：
```ini
extension=php_uuidv7.dll
```

---

### ✅ **4. 功能验证（Windows/Linux 通用）**
```php
<?php
// 二进制格式（直接存数据库 BINARY(16)）
$bin = uuidv7_binary();
echo "Binary: " . bin2hex($bin) . PHP_EOL;

// 字符串格式
$str = uuidv7_string();
echo "String: $str" . PHP_EOL;

// 时间戳验证（前 6 字节为毫秒时间戳）
$timestamp = hexdec(bin2hex(substr($bin, 0, 6)));
echo "Timestamp: " . date('Y-m-d H:i:s', $timestamp / 1000);
?>
```

---

### ⚠️ **关键注意事项**
1. **OpenSSL 依赖**：
   - Windows 需手动下载 OpenSSL 并设置 `OPENSSL_DIR` 环境变量
   - Linux 安装开发包：`sudo apt install libssl-dev`

2. **跨平台差异**：
   - 时间获取：Windows 用 `GetSystemTimeAsFileTime`，Linux 用 `gettimeofday`
   - 休眠函数：Windows 用 `Sleep(ms)`，Linux 用 `usleep(microseconds)`

3. **RFC 9562 合规性**：
   - **时间戳布局**：前 6 字节大端序存储 Unix 毫秒时间戳
   - **版本位**：第 7 字节高 4 位固定为 `0x7`（二进制 `0111`）
   - **变体位**：第 9 字节高 2 位固定为 `10`（二进制 `10xxxxxx`）

4. **性能优化**：
   - 同一毫秒内使用 14 位序列计数器（防冲突）
   - 时钟回拨时扩展随机位（32 位），避免 ID 重复

> 通过 `php -m | findstr uuidv7`（Windows）或 `php -m | grep uuidv7`（Linux）验证扩展加载状态。若遇加载失败，检查 OpenSSL DLL 路径（Windows）或文件权限（Linux）。

