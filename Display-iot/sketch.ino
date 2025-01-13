#include <WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>

const char* ssid = "Wokwi-GUEST";
const char* senha = "";

unsigned int port = 1883;
const char* server = "broker.hivemq.com";
const char* usuarioBroker = "Mat-IoT";

WiFiClient client;
PubSubClient mqttClient(client);
LiquidCrystal_I2C lcd(0x27, 20, 4);


#define TOPICO_MENSAGEM "topic_mensagem/#"
char mensagem[81];
bool situacao = false;

void setup() {
  Serial.begin(115200);
  conectarWifiMqtt();
  delay(1250);
  iniciarLCD();
}

void loop() {
  delay(10);
  mqttClient.loop(); // Mantém a conexão MQTT ativa
}

void callbackMQTT(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida no tópico: ");
  Serial.println(topic);

  Serial.print("Conteúdo da mensagem: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    if (i < sizeof(mensagem) - 1) { // Evita estouro de buffer (isso aqui deu dor de cabeça)
      mensagem[i] = (char)payload[i];
      mensagem[i + 1] = '\0'; 
    }
  }
  Serial.println();
  printaLCD(mensagem);
}

/* ============ Funções =============*/

void conectarWifiMqtt() {
  WiFi.begin(ssid, senha);
  Serial.println("Conectando à rede.");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Conectado à rede Wi-Fi.");

  mqttClient.setServer(server, port);
  mqttClient.setCallback(callbackMQTT);
  Serial.println("Conectando ao MQTT.");

  while (!mqttClient.connected()) {
    if (mqttClient.connect(usuarioBroker)) {
      Serial.println("Conectado ao MQTT!");
      mqttClient.subscribe(TOPICO_MENSAGEM);
    } else {
      Serial.print("Falha na conexão MQTT, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Tentando reconectar em 5 segundos...");
      delay(5000);
    }
  }
}

void verificarConn() {
  Serial.print("");
}

void iniciarLCD() {
  lcd.init();
  lcd.backlight();
}

bool printaLCD(char* mensagem){
  lcd.clear();
  lcd.setCursor(0, 0);
  char partes[4][21]; 

  int tamanhoMensagem = strlen(mensagem);
  int numLinhas = tamanhoMensagem / 20 + 1; // Determina o numero de linhas necessárias
  
  if(numLinhas > 4){
    lcd.print(" A mensagem enviada ");
    lcd.setCursor(2, 1);
    lcd.print("exedeu o limite");
    lcd.setCursor(3, 2);
    lcd.print("de caracteres!");
    return false;
  }

  int i;
  for (i = 0; i < numLinhas; i++) {
    strncpy(partes[i], mensagem + (i * 20), 20); // Copia os próximos 20 caracteres para a parte atual.
    partes[i][20] = '\0'; // Adiciona um caractere nulo para terminar a string.

    if (partes[i][0] == ' ') {
      memmove(partes[i], partes[i] + 1, strlen(partes[i]));
    }
  }

  // Imprime as partes divididas nas linhas do LCD.
  for (i = 0; i < numLinhas; i++) {
    lcd.setCursor(0, i); 
    lcd.print(partes[i]); 
    delay(250); 
  }
  return true;
}


