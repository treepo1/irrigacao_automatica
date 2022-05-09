#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>  
#include <ArduinoJson.h>


bool leituraSensor;
bool leituraAnterior;

const char* ssid     = "BETHO&CRIS";     // your network SSID (name of wifi network)
const char* password = "carvalhosobral5545"; // your network password


#define LED_VERMELHO 5
#define LED_AMARELO 6
#define LED_VERDE 7
#define SENSOR_UMIDADE 8
#define ATUADOR 12
#define BOTtoken "5117193859:AAECeNChQFSTdlW00K8P7iORndw0I3G125c"
#define CHAT_ID "1383689596"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);



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
    }
    if (text == "/on") {
      bot.sendMessage(chat_id, "Sistema de irrigação automática ligado!", "");
    }
     if (text == "/off") {
      bot.sendMessage(chat_id, "Sistema de irrigação automática desligado!", "");
    }

     if (text == "/check") {
      bot.sendMessage(chat_id, "A umidade atual do solo é de " , "");
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

 

  }


void loop() {
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
