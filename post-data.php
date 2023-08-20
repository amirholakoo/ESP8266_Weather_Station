<?php
$servername = "localhost";
$username = "admin";
$password = "XXX";
$dbname = "Weather";

$conn = new mysqli($servername, $username, $password, $dbname);

if ($conn->connect_error) {
  die("Connection failed: " . $conn->connect_error);
}

// Check if the table exists, and if not, create it
$table_check = "SHOW TABLES LIKE 'weather_data'";
$result = $conn->query($table_check);
if (!$result) {
    die("Query failed: " . $conn->error);
}

if ($result->num_rows == 0) {
  $create_table = "CREATE TABLE weather_data (
  id INT(11) UNSIGNED AUTO_INCREMENT PRIMARY KEY,
  temperature FLOAT NOT NULL,
  humidity FLOAT NOT NULL,
  pressure FLOAT NOT NULL,
  lux INT(11) NOT NULL,
  mq135 INT(11) NOT NULL,
  timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
)";


  if ($conn->query($create_table) === TRUE) {
    echo "Table 'weather_data' created successfully";
  } else {
    echo "Error creating table: " . $conn->error;
  }
}

$temperature = $_POST['temperature'];
$humidity = $_POST['humidity'];
$pressure = $_POST['pressure'];
$lux = $_POST['lux'];
$mq135 = $_POST['mq135'];

$sql = "INSERT INTO weather_data (temperature, humidity, pressure, lux, mq135) VALUES ('" . $temperature . "', '" . $humidity . "', '" . $pressure . "', '" . $lux . "', '" . $mq135 . "')";

if ($conn->query($sql) === TRUE) {
  echo "New record created successfully";
} else {
  echo "Error: " . $sql . "<br>" . $conn->error;
}

$conn->close();
?>

