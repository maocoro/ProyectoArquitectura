#include <WiFi.h>
#include <LiquidCrystal.h>
#include <WiFiClientSecure.h>

WiFiClientSecure client;
const char* host = "tu-servidor-websocket"; // Reemplaza con tu servidor WebSocket
const int port = 443; // Puerto seguro por defecto

// Resto del código de Arduino

void reiniciarContador() {
  contador = 0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CUPOS SOLIC: ");
  lcd.print(contador);
  tiempoUltimaPulsacion = millis();

  // Envía el nuevo valor del contador al servidor WebSocket
  if (client.connect(host, port)) {
    String mensaje = "{\"tipo\":\"actualizar-cupos\",\"valor\":" + String(contador) + "}";
    client.println("POST / HTTP/1.1");
    client.println("Host: " + String(host));
    client.println("Content-Type: application/json");
    client.println("Content-Length: " + String(mensaje.length()));
    client.println();
    client.println(mensaje);
    delay(10);
  }
}
