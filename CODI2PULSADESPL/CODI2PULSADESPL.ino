//CÓDIGO PROYECTO ARQUITECTURA DOS PULSADORES CON DESPLAZAMIENTO DE PANTALLA
#include <WiFi.h>
#include <LiquidCrystal.h>
#include <WebSocketsServer.h>

IPAddress staticIP(172, 20, 10, 3);  // Dirección IP estática iPhone
IPAddress gateway(172, 20, 10, 1);    // Dirección IP de la puerta de enlace de la red 
IPAddress subnet(255, 255, 255, 0);   // Máscara de subred de la red
IPAddress dns(8, 8, 8, 8);           // Servidor DNS (se puede usar 8.8.8.8 de Google)

LiquidCrystal lcd(22, 23, 5, 18, 19, 21);
WebSocketsServer webSocket = WebSocketsServer(81);

const char* ssid = "iPhone";
const char* password = "0123456789";

const int pinPulsador1 = 2;
const int pinPulsador2 = 4; // Pin del segundo pulsador

int contador = 0;
unsigned long tiempoUltimaPulsacion = 0;
bool botonBloqueado = false;

unsigned long tiempoBloqueoBoton = 1000; // 1 segundos en milisegundos
unsigned long tiempoUltimoScroll = 0; // Tiempo de la última actualización del scroll
unsigned long intervaloScroll = 250; // Intervalo de desplazamiento en milisegundos

void conectarWiFi() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Conectando WiFi");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(250);
    }
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Conectado WiFi");
}

void reiniciarContador() {
    contador = 0;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("CUPOS SOLIC: ");
    lcd.print(contador);
    sendWebSocketData(); // Envía el valor actualizado a la página web
}

void eliminarDatos() {
    reiniciarContador(); // Reutilizamos la función reiniciarContador()
}

void sendWebSocketData() {
    String message = String(contador);
    webSocket.broadcastTXT(message);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    
}

void setup() {
    Serial.begin(115200);
    lcd.begin(16, 2);
    conectarWiFi();
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
    reiniciarContador();

    pinMode(pinPulsador1, INPUT_PULLUP);
    pinMode(pinPulsador2, INPUT_PULLUP); // Configura el pin del segundo pulsador como entrada
}

void loop() {
    webSocket.loop(); // Maneja eventos WebSocket

    unsigned long tiempoActual = millis();

    // Manejar la pulsación del primer pulsador
    if (!botonBloqueado && digitalRead(pinPulsador1) == HIGH) {
        botonBloqueado = true;
        tiempoUltimaPulsacion = tiempoActual;
        contador++;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("CUPOS SOLIC: ");
        lcd.print(contador);
        delay(250); // Pausa para evitar rebotes
        sendWebSocketData(); // Envía el valor actualizado a la página web
    }

    // Manejar la pulsación del segundo pulsador
    if (digitalRead(pinPulsador2) == HIGH) { // Comprueba si se presionó el segundo pulsador
        eliminarDatos(); // Llama a la función para eliminar los datos
    }

    // Desbloquear el botón después del tiempo de bloqueo
    if (botonBloqueado && tiempoActual - tiempoUltimaPulsacion >= tiempoBloqueoBoton) {
        botonBloqueado = false;
    }

    // Desplazar el texto en el LCD
    if (tiempoActual - tiempoUltimoScroll >= intervaloScroll) {
        lcd.scrollDisplayLeft();
        tiempoUltimoScroll = tiempoActual;
    }
}
