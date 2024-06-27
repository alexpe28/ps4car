#include <Arduino.h>
#include <PS4Controller.h>
#include <ESP32Servo.h>

// Определяем пины для ШИМ и сервопривода
const int PWM_FORWARD_PIN = 16;
const int PWM_BACKWARD_PIN = 17;
const int SERVO_PIN = 18;

// Определяем каналы ШИМ
const int PWM_CHANNEL_FORWARD = 1;
const int PWM_CHANNEL_BACKWARD = 2;
// const int PWM_CHANNEL_SERVO = 2;

// Определяем частоту и разрешение ШИМ
const int PWM_FREQUENCY = 5000;
const int PWM_RESOLUTION = 8;

// Определяем угол поворота сервопривода
const int SERVO_LEFT_ANGLE = 70;  // Влево
const int SERVO_CENTER_ANGLE = 90; // Центр
const int SERVO_RIGHT_ANGLE = 110; // Вправо

// Создаем объект для управления сервоприводом
Servo steeringServo;

// Функция, вызываемая при подключении PS4 контроллера
void onConnect() {
  Serial.println("PS4 controller connected");
}

void setup() {
  // Настраиваем серийный порт для отладки
  Serial.begin(115200);

  // Настраиваем пины для ШИМ управления двигателем
  ledcSetup(PWM_CHANNEL_FORWARD, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttachPin(PWM_FORWARD_PIN, PWM_CHANNEL_FORWARD);
  
  ledcSetup(PWM_CHANNEL_BACKWARD, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttachPin(PWM_BACKWARD_PIN, PWM_CHANNEL_BACKWARD);

  // Инициализируем сервопривод
  steeringServo.setPeriodHertz(50); // Частота сервопривода
  steeringServo.attach(SERVO_PIN, 500, 2400); // Минимум и максимум для SG90
  steeringServo.write(SERVO_CENTER_ANGLE);

  // Инициализируем контроллер PS4
  PS4.attach(onConnect);
  PS4.begin("XX:XX:XX:XX:XX:XX"); // Замените на MAC адрес вашего контроллера
}

void loop() {
  if (PS4.isConnected()) {
    // Обрабатываем поворот колес
    int leftX = PS4.LStickX();
    if (leftX < -20) {
      steeringServo.write(SERVO_LEFT_ANGLE);
    } else if (leftX > 20) {
      steeringServo.write(SERVO_RIGHT_ANGLE);
    } else {
      steeringServo.write(SERVO_CENTER_ANGLE);
    }

    // Обрабатываем движение вперед/назад
    int rightY = PS4.RStickY();
    if (rightY < -20) {
      int speed = map(abs(rightY), 0, 128, 0, 255);
      ledcWrite(PWM_CHANNEL_FORWARD, speed);
      ledcWrite(PWM_CHANNEL_BACKWARD, 0);
    } else if (rightY > 20) {
      int speed = map(rightY, 0, 128, 0, 255);
      ledcWrite(PWM_CHANNEL_FORWARD, 0);
      ledcWrite(PWM_CHANNEL_BACKWARD, speed);
    } else {
      ledcWrite(PWM_CHANNEL_FORWARD, 0);
      ledcWrite(PWM_CHANNEL_BACKWARD, 0);
    }
  }
}
