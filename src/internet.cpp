#include <Arduino.h>
#include <ArduinoJson.h>
#include "internet.h"
#include "senha.h"
#include <WiFi.h>

const unsigned long TempoConexao = 10000;
const unsigned long TempoReconexao = 5000;
unsigned long tempoUltimaconexao = 0;


void conexaoWifi()
{
  Serial.printf("Conectando-se a rede Wi-Fi: %s", SSID);
  WiFi.begin(SSID, SENHA);

  unsigned long tempoWiFi = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - tempoWiFi < TempoConexao)
  {
    Serial.print(".");
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\nWi-Fi funcionando!");
    Serial.print("Endereco IP: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("\nWi-Fi nao funcionando.");
  }
}

void reconexaoWifi()
{
  unsigned long tempoAtual = millis();

  if (tempoAtual - tempoUltimaconexao >= TempoReconexao)
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("Conexao Wi-Fi perdida!");
      conexaoWifi();
    }

    tempoUltimaconexao = tempoAtual;
  }
}