<?php
// 生成字符串格式UUID v7
$uuidStr = uuidv7_string();
echo "String: " . $uuidStr . PHP_EOL; // 输出：018f3a9d-7f3d-7...

// 生成二进制格式UUID v7（直接存MySQL BINARY(16)）
$uuidBin = uuidv7_binary();
echo "Binary: " . bin2hex($uuidBin) . PHP_EOL;

// MySQL插入示例（PDO）
// $pdo = new PDO("mysql:host=localhost;dbname=test", "user", "pass");
// $stmt = $pdo->prepare("INSERT INTO notifications (id) VALUES (?)");
// $stmt->bindParam(1, $uuidBin, PDO::PARAM_LOB);
// $stmt->execute();
?>