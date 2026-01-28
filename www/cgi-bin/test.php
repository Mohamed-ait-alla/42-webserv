#!/usr/bin/php-cgi
<?php
// ---- CGI HEADERS ----
echo "Content-Type: text/html\r\n";
echo "\r\n"; // IMPORTANT: end of headers
?>

<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <title>PHP CGI Test</title>
  <style>
    body {
      background: #1a2238;
      color: white;
      font-family: system-ui, sans-serif;
      padding: 20px;
    }
    h1 {
      color: #facc15;
    }
    pre {
      background: #0f1629;
      padding: 15px;
      border-radius: 8px;
      overflow-x: auto;
    }
    .section {
      margin-top: 30px;
    }
  </style>
</head>
<body>

<h1>Hello from PHP CGI 👋</h1>
<p>If you see this page, PHP CGI is working correctly.</p>

<div class="section">
  <h2>Environment Variables</h2>
  <pre>
<?php
foreach ($_SERVER as $key => $value) {
    echo htmlspecialchars($key . "=" . $value) . "\n";
}
?>
  </pre>
</div>

<div class="section">
  <h2>Request Method</h2>
  <pre>
<?php
echo htmlspecialchars($_SERVER['REQUEST_METHOD'] ?? 'UNKNOWN');
?>
  </pre>
</div>

<?php if (!empty($_GET)): ?>
<div class="section">
  <h2>GET Parameters</h2>
  <pre><?php print_r($_GET); ?></pre>
</div>
<?php endif; ?>

<?php if (!empty($_POST)): ?>
<div class="section">
  <h2>POST Parameters (form-encoded)</h2>
  <pre><?php print_r($_POST); ?></pre>
</div>
<?php endif; ?>

<?php
$rawBody = file_get_contents("php://input");
if (!empty($rawBody)):
?>
<div class="section">
  <h2>Raw POST Body</h2>
  <pre><?php echo htmlspecialchars($rawBody); ?></pre>
</div>
<?php endif; ?>

</body>
</html>
