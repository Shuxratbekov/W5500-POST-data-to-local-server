#include "SPI.h"
//  Подключаем стандартную библиотеку для работы с Ethernet
#include "Ethernet.h"
#include "ArduinoJson.h"

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(10, 10, 10, 80);
IPAddress myDns(1, 1, 1, 1);
IPAddress gateway(10,10,10,1);
byte subnet[] = {255, 255, 248, 0};

EthernetClient client;

//  Указываем адрес сервера
char server[] = "quiz.digital-city.uz";
IPAddress server1(10,10,10,12);

unsigned long lastConnectionTime = 0;
const unsigned long postingInterval = 10 * 1000;

void setup() {
  Serial.begin(9600);
  Serial.println("Initialize Ethernet with DHCP:");

  //  Если соединение с динамической адресацией не было установлено, то
  if (Ethernet.begin(mac) == 0) {
    //  Выводим сообщение об этом в монитор последовательного порта и
    Serial.println("Failed to configure Ethernet using DHCP");
    //  проверяем наличие самого Shield'а
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      //  Если Shield не найден, то выводим соответствующее сообщение в монитор порта
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :( ");
      //  Ничего больше не выполняется
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    //  Проверяем наличие соединения
    if (Ethernet.linkStatus() == LinkOFF) {
      //  Если соеднинение не установлено, то выводим соответствующее сообщение в монитор порта
      Serial.println("Ethernet cable is not connected.");
    }
    // Пробуем установить соединение, используя статический IP-адрес
    Ethernet.begin(mac, ip, myDns, gateway, subnet);
  }
  //  Если соединение с динамической адресацией было установлено, то
  else {
    //  Выводим в монитор порта соответствующее сообщение об этом и выводим назначенный устройству IP-адрес
    Ethernet.begin(mac, ip, myDns, gateway, subnet);
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }
}

void loop() {
  //  Если получены данные, то
  if (client.available()) {
    //  читаем их в переменную и выводим в виде бинарного кода в монитор порта
    char c = client.read();
    Serial.write(c);
  }
  //  Если время между отправкой запросов истекло, то отправляется новый запрос
  if (millis() - lastConnectionTime > postingInterval) {
    httpRequest();
  }
}

//  Функция отправки запроса серверу
void httpRequest() {
  client.stop();

  //  Устанавливаем новое соединение с сервером через 80 порт, и если соединение установлено, то
  if (client.connect(server1, 8000)) {
    Serial.println("connecting...");

    StaticJsonDocument<200> doc;
    doc["name"] = "John Doe";
    doc["age"] = 123;

    String jsonString;
    serializeJson(doc, jsonString);

    String json = "{\"name\":\"John Smith\",\"age\":35}";
    //  Делаем GET-запрос на сервер
    String apiURL = "/api/posts/";
    client.println("POST /api/posts/ HTTP/1.1");
    client.print("Host: ");
    client.println(server1);
    client.println("Content-type: application/json");
    client.print("Content-Length: ");
    client.println(jsonString.length());
    client.println();
    client.println(jsonString);

    lastConnectionTime = millis();
  }
}