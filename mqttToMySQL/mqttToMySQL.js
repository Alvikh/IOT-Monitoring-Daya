const mqtt = require('mqtt');
const mysql = require('mysql');
// MQTT Configuration
const mqttServer = 'mqtt://broker.hivemq.com';
const mqttTopic = 'kelompok-5/sensor/2';
// MySQL Configuration
const mysqlConfig = {
  host: 'localhost',
  user: 'root',
  password: '',
  database: 'iot',
};
// Create MQTT and MySQL clients
const mqttClient = mqtt.connect(mqttServer);
const mysqlConnection = mysql.createConnection(mysqlConfig);
// Connect to MySQL
mysqlConnection.connect((err) => {
  if (err) {
    console.error('Error connecting to MySQL:', err);
    process.exit(1);
  }
  console.log('Connected to MySQL');
});
// MQTT Subscribe
mqttClient.on('connect', () => {
  console.log('Connected to MQTT');
  mqttClient.subscribe(mqttTopic);
});
mqttClient.on('message', (topic, message) => {
    // Assuming the message is a string
const dataString = message.toString();
// Log the received string
console.log(dataString);
// Extract numeric values using regular expressions
const matches = dataString.match(/-?\d+(\.\d+)?/g);
// Ensure that there are at least 4 numeric values
if (matches && matches.length >= 4) {
  // Extract the first four numeric values
  const [voltageRawValue, currentRawValue, voltageValue, currentVoltage] = matches.map(Number);
  // Calculate power and kWh
  const power = voltageValue * currentVoltage;
  const kWh = power / 1000.0;
  // Insert data into MySQL
  const query = 'INSERT INTO sensor_data (voltageRawValue, currentRawValue, voltageValue, currentValue, power, Kwh) VALUES (?, ?, ?, ?, ?, ?)';
  const values = [voltageRawValue, currentRawValue, voltageValue, currentVoltage, power, kWh];
  mysqlConnection.query(query, values, (err) => {
    if (err) {
      console.error('Error inserting into MySQL:', err);
    } else {
      console.log('Data inserted into MySQL');
    }
  });
} else {
  console.error('Invalid data format');
}
});
// Handle process termination
process.on('SIGINT', () => {
  mysqlConnection.end();
  mqttClient.end();
  process.exit();
});
