#include <WiFi.h>
#include <SD.h>
#include <SPI.h>
#include "ESPAsyncWebServer.h"

//Credenciais de acesso ao ponto Wifi
const char* user     = "EspTransmiter";
const char* password = "123456789";

AsyncWebServer server(80);

//Informações para fixar o IP do transmissor 
IPAddress ip(192,168,1,120);    
IPAddress gateway(192,168,1,1); 
IPAddress subnet(255,255,255,0); 
IPAddress dns(192,168,1,1);     

int i = 0;
int time2CreateArq = 120000;

//Semaforo de acesso ao micro sd
SemaphoreHandle_t xSD_semaphore;

void setup() {
  Serial.begin(115200);
  
  if(!SD.begin(5)){
    Serial.println("Falha de reconhecimento do cartão");
    ESP.restart();
  }

  //O semáforo abaixo ajuda a não haver acessos simultâneos ao cartão SD
  xSD_semaphore = xSemaphoreCreateMutex();
  
  Serial.print("Setando acesso");
  WiFi.softAPConfig(ip,gateway,subnet);
  WiFi.softAP(user, password);

  //Recebeu mensagem para download do arquivo
  server.on("/download", [] (AsyncWebServerRequest *request){
    //Os arquivos serão listados e enviados de acordo com que são gerados
    xSemaphoreTake(xSD_semaphore, portMAX_DELAY);
    String nameArq2Send = listFiles(SD, "/");
    xSemaphoreGive(xSD_semaphore);
    if(nameArq2Send != ""){
      //Enviando Arquivo
      request->send(SD, nameArq2Send, "text/xhr");
    }
    else{
      //NPA: Não Possui Arquivo
      request->send(200, "text/plain", "NPA");
    }
  });
  
  //Caso o arquivo tenha sido validado pelo cliente, o mesmo retorna uma informação de deletar o arquivo enviado
  server.on("/delete", [] (AsyncWebServerRequest *request){
    if (request->hasParam("file")) {
      //Pègando o arquivo a ser deletado como parametro da url enviada ao servidor
      String file2Delete = request->getParam("file")->value();
      if(file2Delete != ""){
        xSemaphoreTake(xSD_semaphore, portMAX_DELAY);
        if(deleteFile(SD,"/" + file2Delete)){
          //AD: Arquivo Deletado
          request->send(200, "text/plain", "AD");
        }
        else{
          //FDA: Falha em deletar arquivo
          request->send(200, "text/plain", "FDA");
        }
        xSemaphoreGive(xSD_semaphore);
      }
      else{
        //FDA: Falha em encontrar arquivo
        request->send(200, "text/plain", "FEA");
      }
    } 
  });
  
  server.begin();
}

void loop(){
  //Criando arquivo de 500KB
  String name2CreateArq = "arqNumber" + String(i);
  xSemaphoreTake(xSD_semaphore, portMAX_DELAY);
  if(creatArch(SD, name2CreateArq)){ 
    File arq = SD.open("/" + name2CreateArq ,"w+");
    for(int a = 0; a < 50000; a++){
      //Delay abaixo é apenas uma segurança contra problemas de watchdog timer
      delay(2);
      arq.print("vou escrever qualquer coisa aqui para ser uma das linhas do arquivo de 500kb que deve ser enviado !!");
    }
  }
  xSemaphoreGive(xSD_semaphore);
  i += 1;
  delay(time2CreateArq);
}
