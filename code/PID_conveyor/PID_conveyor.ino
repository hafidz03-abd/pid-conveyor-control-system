#include "HX711.h"
#include <PID_v1.h>

// --- UPDATE DEFINISI PIN (MENGGUNAKAN PIN DIGITAL "D") ---
// Koneksi HX711 ke Pin Digital
const int LOADCELL_DOUT_PIN = 6; // Pin D2
const int LOADCELL_SCK_PIN = 5;  // Pin D3

// Koneksi Motor Driver L298N
const int IN1_PIN = 11;   // Pin D4
const int IN2_PIN = 10;   // Pin D5
const int ENA_PIN = 9;   // Pin D9 (Harus pin yang ada tanda '~' untuk PWM)

// --- Variabel PID & Setup Lainnya Tetap Sama ---
double Setpoint, Input, Output;
double Kp = 0.4, Ki = 0.05, Kd = 1.2;

HX711 scale;
// Setup PID: Reverse (Semakin Berat -> Motor Pelan) atau Direct (Semakin Berat -> Motor Cepat)
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, REVERSE); 

float calibration_factor = -208.00; 

void setup() {
  Serial.begin(9600);
  
  // Setup Pin Motor
  pinMode(ENA_PIN, OUTPUT);
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  
  // Set Arah Motor
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);

  // Setup HX711
  Serial.println("Menghubungkan ke Sensor Digital HX711...");
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor);
  scale.tare(); 

  // Setup PID
  Setpoint = 1000.0; // Target berat
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(60, 255); // Minimal 60 agar motor tidak macet
  myPID.SetSampleTime(100); 
}

void loop() {
  // 1. Baca Sensor
  if (scale.is_ready()) {
    Input = scale.get_units(3); 
    if(Input < 0) Input = 0;
  }

  // 2. Hitung PID
  myPID.Compute();

  // 3. Kirim Sinyal Digital PWM ke Motor
  analogWrite(ENA_PIN, Output); // analogWrite di Pin 9 menghasilkan sinyal PWM digital

  // 4. Monitoring
  Serial.print("Target:");
  Serial.print(Setpoint);
  Serial.print(" | Bacaan_Sensor:");
  Serial.print(Input);
  Serial.print(" | Kecepatan_PWM:");
  Serial.println(Output);
}