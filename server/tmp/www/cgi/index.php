#!/usr/bin/php-cgi
<?php
header("Content-Type: text/html");

echo "<h1>Hello from PHP CGI!</h1>";
echo "<p>You sent:</p>";
echo "<pre>";
foreach ($_REQUEST as $key => $value) {
    echo "$key = $value\n";
}
echo "</pre>";
?>
