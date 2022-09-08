bool creatArch(fs::FS &fs, String nameArq){
  Serial.println("Entrou para Criar");
  File arq2OPen = SD.open("/" + nameArq ,"w+");
  if(arq2OPen){
    Serial.println("Arquivo Aberto/Criado");
    arq2OPen.close();
    return true;
  }
  Serial.println("Falha ao criar arquivo");
  return false;
}

void readArch(fs::FS &fs, String path){
  File rFile = SD.open(path, "r");
  if(rFile){
    Serial.println("Iniciando Leitura do Arquivo");
    while(rFile.available()){
      Serial.println(rFile.readStringUntil('\r'));
    }
    rFile.close();
  }
  else{
    Serial.println("Erro ao abrir arquivo TXT para leitura");
  }
}

String listFiles(fs::FS &fs,String nameDir){
  File root = fs.open(nameDir);
  File file = root.openNextFile();
  int countOnlyDirectory = 0;
  while(file){
    if(!file.isDirectory()){
      //Serial.println(file.name());
      return file.name();
      break;
    }
    else{
      countOnlyDirectory++;
      if(countOnlyDirectory == 5){return "";}
    }
  }
}

bool deleteFile(fs::FS &fs,String fileName){
  if(fs.remove(fileName)){
    return true;
  } 
  else {
    return false;
  }
}
