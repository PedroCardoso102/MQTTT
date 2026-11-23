c
#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include "internet.h"
#include <ezTime.h>

int led = 2;

int bt = 0;

const char mqtt_server[] = "broker.emqx.io";
const int mqtt_port = 1883;
const char *mqtt_client_id = "senai134_esp_pedro";
const char *mqtt_topic_sub = "senai_duoKP/pub";
const char *mqtt_topic_pub = "senai134/DevKp/pub";

void conectaMqtt();
void retornomqtt(char *topic, byte *payload, unsigned int length);

WiFiClient espClient;
PubSubClient Client(espClient);

Timezone sp;

void setup()
{
  INPUT_PULLUP(bt);
  pinMode(led, OUTPUT);
  Serial.begin(115200);
  Serial.setTimeout(5000);

 conexaoWifi();

  Client.setServer(mqtt_server, mqtt_port);
  Client.setCallback(retornomqtt);

  waitForSync();
}

void loop()
{ 
  reconexaoWifi();

  Client.loop();

  if (!Client.connected())
  {
    conectaMqtt();
  }

  if (Serial.available() > 0)
  {
    String textoDigitado = Serial.readStringUntil('\n');
    textoDigitado.trim();

    if (textoDigitado.length() == 0)
    {
      Serial.println("msg vazia");
      return;
    }

    JsonDocument doc;

    doc["disp"] = "esp_pedro";
    doc["msg"] = textoDigitado;
    doc["Segundo"] = sp.second();
    doc["Minuto"] = sp.minute();
    doc["Hora"] = sp.hour();
    doc["Dia"] = sp.day();
    doc["Mes"] = sp.month();
    doc["Ano"] = sp.year();

    String stringJson;
    serializeJson(doc, stringJson);

    Client.publish(mqtt_topic_pub, stringJson.c_str());

    Serial.print("enviado: ");
    Serial.println(stringJson);
  }
}

void conectaMqtt()
{
  while (!Client.connected())
  {
    Serial.print("Conectando ao broker MQTT...");

    if (Client.connect(mqtt_client_id))
    {
      Serial.println("Conectado ao broker MQTT!");
      Client.subscribe(mqtt_topic_sub);
    }
    else
    {
      Serial.print("Falha na conexão MQTT. Código: ");
      Serial.println(Client.state());
      delay(5000);
    }
  }
}

void retornomqtt(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Mensagem recebida em ");
  Serial.print(topic);
  Serial.print(" : ");

  String msgRecebida = "";

  for (int i = 0; i < length; i++)
  {
    msgRecebida += (char)payload[i];
  }

  Serial.println(msgRecebida);

  JsonDocument doc;

  DeserializationError erro = deserializeJson(doc, msgRecebida);

  if (erro)
  {
    Serial.print("Falha ao decodificar JSON: ");
    Serial.println(erro.f_str());
    return;
  }

  const char *dispositivo = doc["disp"];
  const char *msg = doc["msg"];

  int segundo = doc["Segundo"];
  int minuto = doc["Minuto"];
  int hora = doc["Hora"];
  int dia = doc["Dia"];
  int mes = doc["Mes"];
  int ano = doc["Ano"];

  Serial.println("Dados decodificados");
  Serial.print("Esp: ");
  Serial.println(dispositivo);

  Serial.print("Mensagem: ");
  Serial.println(msg);

  Serial.println();

  Serial.printf("Hoje é %i do %i de %i, %i:%i:%i",
                dia, mes, ano, hora, minuto, segundo);

  Serial.println();
  Serial.println("----------------------------------");

  if (String(msg) == "acende led")
  {
    digitalWrite(led, HIGH);
  }
  else if (String(msg) == "apaga led")
  {
    digitalWrite(led, LOW);
  }
  
}
