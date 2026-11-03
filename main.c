#define HALL_PIN PA0
#define CURRENT_PIN PA1
#define TEST_OUTPUT PA8

volatile unsigned long hallPulses = 0;

void hallISR()
{
  hallPulses++;
}

void setup()
{
  Serial.begin(115200);

  analogReadResolution(12);

  pinMode(HALL_PIN, INPUT_PULLDOWN);
  pinMode(TEST_OUTPUT, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(HALL_PIN), hallISR, RISING);

  Serial.println("=================================");
  Serial.println("EV MOTOR TELEMETRY TEST");
  Serial.println("RPM + ACS712 CURRENT");
  Serial.println("=================================");
}

void loop()
{
  // Generate test Hall signal
  digitalWrite(TEST_OUTPUT, HIGH);
  delayMicroseconds(5000);

  digitalWrite(TEST_OUTPUT, LOW);
  delayMicroseconds(5000);

  static unsigned long lastTime = 0;

  if (millis() - lastTime >= 1000)
  {
    lastTime = millis();

    noInterrupts();
    unsigned long pulses = hallPulses;
    hallPulses = 0;
    interrupts();

    // 1 pulse = 1 revolution
    float rpm = pulses * 60.0;

    // Read ACS712
    int adcRaw = analogRead(CURRENT_PIN);

    float adcVoltage = (adcRaw * 3.3) / 4095.0;

    // 10k + 15k voltage divider
    float sensorVoltage =
      adcVoltage * (10.0 + 15.0) / 15.0;

    // ACS712-05B ≈ 185 mV/A
    float current =
      (sensorVoltage - 2.5) / 0.185;

    Serial.print("Pulses = ");
    Serial.print(pulses);

    Serial.print(" | RPM = ");
    Serial.print(rpm, 1);

    Serial.print(" | ADC = ");
    Serial.print(adcRaw);

    Serial.print(" | ADC_V = ");
    Serial.print(adcVoltage, 3);

    Serial.print(" V | Sensor_V = ");
    Serial.print(sensorVoltage, 3);

    Serial.print(" V | Current = ");
    Serial.print(current, 2);

    Serial.println(" A");
  }
}