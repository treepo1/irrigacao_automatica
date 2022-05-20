#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>  
#include <ArduinoJson.h>



bool modoAuto;
int leituraSensorIntervalo = 20; //86400 (24 horas)
long ultimaLeituraSensor = 0;
int leituraSensor = 0;

const char* ssid     = "BETHO&CRIS";     // your network SSID (name of wifi network)
const char* password = "carvalhosobral5545"; // your network password





#define SENSOR_UMIDADE 32
#define ATUADOR 12
#define BOTtoken "5117193859:AAECeNChQFSTdlW00K8P7iORndw0I3G125c"
#define CHAT_ID "1383689596"

#define valorCriticoUmidade 2000

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);


int readSensor(){
  int readed = analogRead(SENSOR_UMIDADE);
  Serial.print("*** Nível de umidade (0-4095): ");
  Serial.println(4095 - readed);
  return 4095 - readed;
}


void handle_irrig_auto(){

        if (leituraSensor < valorCriticoUmidade) {
        
        String res = "Umidade abaixo do limite, ativando irrigador\n";
        res+="Nível de umidade [0-4095]: ";
        res+=String(leituraSensor);
        bot.sendMessage(CHAT_ID, res,""); // informando via telegram sobre a umidade
        delay(5000);
        while(readSensor() < valorCriticoUmidade && modoAuto){
        digitalWrite(ATUADOR, HIGH); //ativando irrigacao
        delay(500);
        digitalWrite(ATUADOR, LOW); //desativando irrigacao

        delay(10000); //esperando pela mudança da umidade
    }
    bot.sendMessage(CHAT_ID, "Irrigador desativado :)","");
        }
  
  }

void handle_irrig(){

  ultimaLeituraSensor = millis();
      
         String res = "Nível de umidade [0-4095]: ";
         res+=String(leituraSensor);
         if (leituraSensor < valorCriticoUmidade){
         res+=" Me regue :( ";
         }
         bot.sendMessage(CHAT_ID, res,""); // informando via telegram sobre a umidade
  
  
}



void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));


  for (int i=0; i<numNewMessages; i++){
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Usuário não autorizado", "");
      continue;
    }

    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start"){
      String welcome = "Olá, " + from_name + ".\n";
      welcome+= "Use os seguintes comandos para controlar o seu jardim.\n\n";
      welcome+= "/on para ativar o controle automático de irrigação\n";
      welcome+= "/off para desativar o controle automático de irrigação\n";
      welcome+= "/check para checar a umidade atual do solo\n";
      bot.sendMessage(chat_id, welcome, "");
      modoAuto = false;
    }
    if (text == "/on") {
      bot.sendMessage(chat_id, "Sistema de irrigação automática ligado!", "");
      modoAuto = true;
  }
    
     if (text == "/off") {
      bot.sendMessage(chat_id, "Sistema de irrigação automática desligado!", "");
      modoAuto = false;
    }

     if (text == "/check") {
      String res = "Nível de umidade [0-4095]: ";
      int umidadeAtual = readSensor();
       res+=String(umidadeAtual);
       if(umidadeAtual < valorCriticoUmidade){
        res+=" Me regue! :(";
       }
      bot.sendMessage(chat_id, res , "");
      
    }
    }
  }
  


//checando novas mensagens a cada 1 segundo
int botRequestDelay = 1000;
unsigned long lastTimeBotCheck;

void setup() {
  
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  delay(100);

  Serial.print("Tentando conexão com SSID: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    // wait 1 second for re-trying
    delay(1000);
  }

  Serial.print("Conectado a ");
  Serial.println(ssid);

  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);

  pinMode(ATUADOR, OUTPUT);

 

  }


void loop() {
    leituraSensor = readSensor();
    
    if (!modoAuto && millis()> ultimaLeituraSensor + ((leituraSensorIntervalo)*1000) or ultimaLeituraSensor == 0) {handle_irrig();}
    if (modoAuto ){handle_irrig_auto();}

    
  if (millis() > lastTimeBotCheck + botRequestDelay) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages){
      Serial.println("resposta aceita");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotCheck = millis();
}
}
