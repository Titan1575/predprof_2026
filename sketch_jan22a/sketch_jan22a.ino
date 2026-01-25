#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 7
word VentPin = 3; // Используем uint8_t вместо word

Adafruit_SSD1306 display(OLED_RESET);

const int tachometerPin = 2;
volatile unsigned long pulseCount = 0;
const float pulsesPerRevolution = 2.0;
float rpm = 0.0;
unsigned long lastMeasureTime = 0;
const unsigned long measureInterval = 1000;

void countPulse() {
  pulseCount++;
}

void setup() {
  Wire.begin();
  pinMode(VentPin, OUTPUT);
  pwm25kHzBegin();

  Serial.begin(9600);
  Serial.println("Инициализация измерения RPM...");

  pinMode(tachometerPin, INPUT_PULLUP); // Включаем внутренний pullup-резистор
  attachInterrupt(digitalPinToInterrupt(tachometerPin), countPulse, RISING);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Инициализация дисплея в setup()
  display.clearDisplay(); // Очистка дисплея здесь же
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10, 10);

  pwm25kHzBegin();
  pwmDuty(22); // 25%

  lastMeasureTime = millis();
}

void loop() {
  if (millis() - lastMeasureTime >= measureInterval) { // Если интервал прошел
    noInterrupts(); // Запрещаем прерывания временно
    unsigned long currentPulseCount = pulseCount;
    pulseCount = 0; // Сбрасываем счетчик
    interrupts(); // Восстанавливаем прерывания

    unsigned long timeElapsed = millis() - lastMeasureTime;
    rpm = (currentPulseCount / pulsesPerRevolution) * (60000.0 / timeElapsed);
int rpm2 = 0;
rpm2=rpm;
    Serial.print("RPM: ");
    Serial.println(rpm2);
    
    display.clearDisplay(); // Очищаем дисплей перед обновлением данных
    display.setCursor(10, 10);
    display.print(rpm2); // Выводим значение RPM
    display.print(" RPM");
    display.display(); // Отображаем новое значение на дисплее

    lastMeasureTime = millis(); // Обновляем время последней проверки
  }
}

void pwm25kHzBegin() {
  TCCR2A = 0;
  TCCR2B = 0;
  TIMSK2 = 0;
  TIFR2 = 0;
  TCCR2A |= (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);
  TCCR2B |= (1 << WGM22) | (1 << CS21);
  OCR2A = 79;
  OCR2B = 0;
}

void pwmDuty(byte ocrb) {
  OCR2B = ocrb;
}