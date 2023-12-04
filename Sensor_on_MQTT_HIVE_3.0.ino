#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <DHT.h>

//Informação de Parametros para comunicação - verificar com o seu broker
const char* ssid = "xxxxx";
const char* password = "xxxxx";
const char* mqtt_server = "xxxxx";
const int mqtt_port = 8888;
const char* mqtt_username = "xxxx";
const char* mqtt_password = "xxxxx";
const char *mqttTopic1 = "xxxxx";
const char *mqttTopic2 = "xxxxx";
const char *mqttTopic3 = "xxxxx";

#define DHTPIN 4  // Pinagem do SENSOR
#define DHTTYPE DHT11 //Definido o tipo de SENSOR

DHT dht(DHTPIN, DHTTYPE); //Informando dados para a biblioteca DHT

int Min_Anterior = 0;
char Temperatura[10];
char Umidade[10];

//O uso do certificado root_ca ajuda a garantir a autenticidade do servidor MQTT, protegendo a integridade e a confidencialidade da comunicação entre o seu dispositivo e o servidor MQTT.
static const char* root_ca PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)EOF";

//essas linhas de código estão configurando um cliente MQTT (client) para usar uma conexão segura (WiFiClientSecure) ao se comunicar com o servidor MQTT. 
//A instância client será usada para publicar mensagens, se inscrever em tópicos, etc., enquanto a instância espClient gerenciará a parte segura da conexão.
WiFiClientSecure espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client", mqtt_username, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void sensorHT(){
    // A leitura da temperatura e umidade 
  float h = dht.readHumidity();
  // Ler a temperatura como Celsius (o padrão)
  float t = dht.readTemperature();

  //Verificando se ocorreu erro na leitura do sensor
  if (isnan(t) || isnan(h)) {
    Serial.println("Erro ao ler sensor!");
    return;
  }
 
   // Formatar os dados MQTT para Temperatura
  sprintf(Temperatura,"%.2f C",t);  // Converte float para string com 2 casas decimais
  // Formatar os dados MQTT para Umidade
  sprintf(Umidade,"%.2f %%",h);  // Note que removi o espaço entre "%.2f" e "%%" para evitar espaços extras  
 
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  configTime(-3 * 3600, 0, "0.pool.ntp.org", "1.pool.ntp.org", "2.pool.ntp.org");
  setup_wifi();
  espClient.setCACert(root_ca);
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  delay(1000);

  //coleta dados sensor
  sensorHT();

  // Obter a hora atual
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) {
    struct tm timeinfo;
    Serial.println("Falha ao obter hora!");
    return;
  }

  // Formatar os dados da data para MQTT
  char Data[50];
  sprintf(Data, "[%02d/%02d/%02d-%02d:%02d]",
          timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year % 100,
          timeinfo.tm_hour, timeinfo.tm_min);

  int Min_Atual = timeinfo.tm_min;

  if (!client.connected()) {
    reconnect();
  }

//Comparação para envio dos dados a cada minuto.
  if (Min_Atual != Min_Anterior){
  client.publish(mqttTopic1, Data);
  client.publish(mqttTopic2, Temperatura);
  client.publish(mqttTopic3, Umidade);
  Serial.println("enviando:");
  Serial.print("Data/Hora:");
  Serial.println(Data);
  Serial.print("Temperatura:");
  Serial.println(Temperatura);
  Serial.print("Umidade:");
  Serial.println(Umidade);
  Serial.println("Dado enviado para o servidor MQTT");
  }
  Min_Anterior = Min_Atual; //armazena minuto para o proximo loop de comparação
  client.loop();
  // chamar client.loop() periodicamente em seu código é essencial para garantir que a comunicação MQTT seja eficiente e confiável. Geralmente, é comum incluir essa chamada dentro do loop principal do seu programa para que seja executada repetidamente. 
}


//MQTT(Message Queuing Telemetry Transport) é um protocolo de comunicação máquina para máquina (M2M - Machine to Machine) com foco em Internet of Things (IoT) que funciona em cima do protocolo TCP/IP. 
//Um sistema MQTT se baseia na comunicação entre cliente e servidor, em que o primeiro pode realizar tanto “postagens” quanto “captação” de informação e o segundo administra os dados a serem recebidos e enviados.


//Conexão MQTT
//Clientes e agentes começam a se comunicar usando uma conexão MQTT. 
//Os clientes iniciam a conexão enviando uma mensagem CONNECT ao agente MQTT. 
//O agente confirma que uma conexão foi estabelecida respondendo com uma mensagem CONNACK. 
//Tanto o cliente MQTT como o agente requerem uma pilha TCP/IP para se comunicarem. 
//Os clientes nunca se conectam entre si, apenas com o agente.