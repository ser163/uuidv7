<?php
use PHPUnit\Framework\TestCase;

class UuidV7Test extends TestCase {
    // 测试字符串格式符合RFC 9562
    public function testStringFormat() {
        $uuid = uuidv7_string();
        $this->assertMatchesRegularExpression(
            '/^[0-9a-f]{8}-[0-9a-f]{4}-7[0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}$/i',
            $uuid
        );
    }

    // 测试二进制数据长度及版本位
    public function testBinaryStructure() {
        $bin = uuidv7_binary();
        $this->assertEquals(16, strlen($bin));
        
        // 检查版本位（第6字节高4位=0x7）
        $version = ord($bin[6]) >> 4;
        $this->assertEquals(0x7, $version);
        
        // 检查变体位（第8字节高2位=0b10）
        $variant = (ord($bin[8]) & 0xC0) >> 6;
        $this->assertEquals(0b10, $variant);
    }

    // 测试时间戳单调性（需连续生成）
    public function testTimestampOrdering() {
        $last = uuidv7_binary();
        for ($i = 0; $i < 100; $i++) {
            $current = uuidv7_binary();
            // 比较前6字节（时间戳部分）
            $cmp = strcmp(substr($last, 0, 6), substr($current, 0, 6));
            $this->assertLessThanOrEqual(0, $cmp);
            $last = $current;
        }
    }
}
?>