#include <WiFi.h>
#include <SD.h>
#include <HTTPClient.h>

//Credenciais de acesso ao ponto Wifi
const char* user     = "EspTransmiter";
const char* password = "123456789";

//URL de acesso para solicitação do arquivo
const char* urlAcessArq= "http://192.168.1.120/download";

unsigned long timePass;
int time2requestArq = 30000;

void setup() {
  Serial.begin(115200);
  //Acesso ao ponto de wifi gerado pelo servidor
  WiFi.begin(user, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("waiting connection");
  }
}

void loop(){
  //Delay abaixo é apenas uma segurança contra problemas de watchdog timer
  delay(2);
  if(abs(millis() - timePass) >= time2requestArq){
    testConection();
    Serial.println("Solicitando Arquivo");
    request2Server(urlAcessArq);
    timePass = millis();
  }
}

void testConection(){
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(user, password);
    delay(500);
    Serial.println("waiting connection");
  }
}

void request2Server(const char* url) {
  HTTPClient http;

  //Iniciando comunicação com servidor
  http.begin(url);
  
  //PEgando o codigo de resposta
  int responseCode = http.GET();
  
  String arqString; 
  
  if (responseCode == HTTP_CODE_OK) {
    arqString = http.getString();
  }
  else{
    Serial.print("Error code: ");
    Serial.println(responseCode);
  }

  //Finalizando comunicação
  http.end();
}
