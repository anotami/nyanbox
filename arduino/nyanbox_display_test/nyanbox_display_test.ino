/*
 * NyanBox - Firmware de prueba de pantalla
 * Placa  : ESP32-WROOM-32
 * Display: RCmall ILI9341 2.8" 240x320 TFT LCD SPI
 *
 * ANTES DE COMPILAR:
 *   1. Instala la librería "TFT_eSPI" de Bodmer (Gestor de librerías)
 *   2. Abre el archivo:
 *        Documents/Arduino/libraries/TFT_eSPI/User_Setup.h
 *   3. Comenta todos los drivers y descomenta solo:
 *        #define ILI9341_DRIVER
 *   4. Ajusta los pines en ese mismo archivo (o déjalos si coinciden):
 *        #define TFT_MOSI  23
 *        #define TFT_SCLK  18
 *        #define TFT_CS     5
 *        #define TFT_DC     2
 *        #define TFT_RST    4
 *        #define TFT_BL    21
 *
 * CONEXIÓN DISPLAY → ESP32:
 *   VCC  → 3.3V
 *   GND  → GND
 *   CS   → GPIO 5
 *   RESET→ GPIO 4
 *   DC   → GPIO 2
 *   MOSI → GPIO 23
 *   SCK  → GPIO 18
 *   LED  → GPIO 21  (o 3.3V para siempre encendido)
 *   MISO → GPIO 19  (opcional, solo lectura)
 */

#include <TFT_eSPI.h>
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();

#define BL_PIN          21
#define BL_PWM_CH       0
#define TEST_DELAY      2000
#define ANIM_DURATION   5000

// ── backlight PWM ─────────────────────────────────────────────

void blInit() {
    ledcSetup(BL_PWM_CH, 5000, 8);
    ledcAttachPin(BL_PIN, BL_PWM_CH);
    ledcWrite(BL_PWM_CH, 255);
}

void blSet(uint8_t b) { ledcWrite(BL_PWM_CH, b); }

// ── color helpers ─────────────────────────────────────────────

uint16_t hsvColor(uint8_t h) {
    uint8_t region = h / 43;
    uint8_t rem    = (h - region * 43) * 6;
    uint8_t q = (255 * (255 - rem)) >> 8;
    uint8_t t = (255 * rem) >> 8;
    switch (region) {
        case 0: return tft.color565(255, t,   0);
        case 1: return tft.color565(q,   255, 0);
        case 2: return tft.color565(0,   255, t);
        case 3: return tft.color565(0,   q,   255);
        case 4: return tft.color565(t,   0,   255);
        default:return tft.color565(255, 0,   q);
    }
}

// ── pruebas ───────────────────────────────────────────────────

void prueba_colores_solidos() {
    Serial.println("PRUEBA: Colores solidos");
    struct { uint16_t c; const char* n; } cols[] = {
        {TFT_RED,     "ROJO"},    {TFT_GREEN,  "VERDE"},
        {TFT_BLUE,    "AZUL"},    {TFT_WHITE,  "BLANCO"},
        {TFT_BLACK,   "NEGRO"},   {TFT_YELLOW, "AMARILLO"},
        {TFT_CYAN,    "CYAN"},    {TFT_MAGENTA,"MAGENTA"},
    };
    for (auto& c : cols) {
        tft.fillScreen(c.c);
        tft.setTextDatum(MC_DATUM);
        tft.setTextColor(c.c == TFT_BLACK ? TFT_WHITE : TFT_BLACK);
        tft.setTextSize(2);
        tft.drawString(c.n, tft.width()/2, tft.height()/2);
        delay(TEST_DELAY);
    }
}

void prueba_barras() {
    Serial.println("PRUEBA: Barras de color");
    int bw = tft.width() / 8;
    uint16_t bars[] = {TFT_WHITE, TFT_YELLOW, TFT_CYAN, TFT_GREEN,
                       TFT_MAGENTA, TFT_RED, TFT_BLUE, TFT_BLACK};
    for (int i = 0; i < 8; i++)
        tft.fillRect(i * bw, 0, bw, tft.height(), bars[i]);
    delay(TEST_DELAY);
}

void prueba_gradiente() {
    Serial.println("PRUEBA: Gradiente arcoiris");
    for (int x = 0; x < tft.width(); x++)
        tft.drawFastVLine(x, 0, tft.height(), hsvColor(map(x, 0, tft.width()-1, 0, 255)));
    delay(TEST_DELAY);
}

void prueba_texto() {
    Serial.println("PRUEBA: Texto");
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(TL_DATUM);

    tft.setTextColor(TFT_WHITE);   tft.setTextSize(1);
    tft.drawString("NyanBox Display Test", 5, 5);

    tft.setTextColor(TFT_CYAN);    tft.setTextSize(2);
    tft.drawString("ESP32-WROOM-32", 5, 20);

    tft.setTextColor(TFT_YELLOW);  tft.setTextSize(1);
    tft.drawString("ABCDEFGHIJKLMNOPQRSTUVWXYZ", 5, 50);
    tft.drawString("abcdefghijklmnopqrstuvwxyz", 5, 62);
    tft.drawString("0123456789 !@#$%&*()", 5, 74);

    tft.setTextColor(TFT_GREEN);
    tft.drawString("Resolucion:", 5, 95);
    tft.drawString(String(tft.width()) + " x " + String(tft.height()), 5, 107);

    tft.setTextColor(TFT_MAGENTA);
    tft.drawString("ILI9341 2.8\" TFT LCD", 5, 128);

    delay(TEST_DELAY * 2);
}

void prueba_formas() {
    Serial.println("PRUEBA: Formas geometricas");
    tft.fillScreen(TFT_BLACK);
    int cx = tft.width() / 2, cy = tft.height() / 2;

    tft.fillRect(10, 10, 60, 40, TFT_BLUE);
    tft.drawRect(10, 10, 60, 40, TFT_WHITE);

    tft.fillRoundRect(cx - 40, 10, 80, 40, 10, TFT_GREEN);
    tft.drawRoundRect(cx - 40, 10, 80, 40, 10, TFT_WHITE);

    tft.fillCircle(cx, cy, 55, TFT_RED);
    tft.drawCircle(cx, cy, 55, TFT_WHITE);
    tft.drawCircle(cx, cy, 40, TFT_YELLOW);
    tft.drawCircle(cx, cy, 25, TFT_CYAN);

    tft.fillTriangle(10, tft.height()-10, 70, tft.height()-70, 130, tft.height()-10, TFT_MAGENTA);
    tft.drawTriangle(10, tft.height()-10, 70, tft.height()-70, 130, tft.height()-10, TFT_WHITE);

    for (int a = 0; a < 360; a += 20) {
        float rad = a * PI / 180.0f;
        tft.drawLine(tft.width()-45, tft.height()-45,
                     tft.width()-45 + (int)(35*cos(rad)),
                     tft.height()-45 + (int)(35*sin(rad)),
                     hsvColor(map(a, 0, 359, 0, 255)));
    }
    delay(TEST_DELAY * 2);
}

void prueba_tablero() {
    Serial.println("PRUEBA: Tablero ajedrez (pixeles)");
    int cell = 10;
    for (int y = 0; y < tft.height(); y += cell)
        for (int x = 0; x < tft.width(); x += cell)
            tft.fillRect(x, y, cell, cell,
                         ((x/cell + y/cell) % 2) ? TFT_WHITE : TFT_BLACK);
    delay(TEST_DELAY);
}

void prueba_animacion() {
    Serial.println("PRUEBA: Animacion pelota");
    tft.fillScreen(TFT_BLACK);
    int w = tft.width(), h = tft.height(), r = 18;
    float x = w/2, y = h/2, vx = 4.0f, vy = 2.7f;
    uint16_t col = TFT_CYAN;
    int px = x, py = y;
    unsigned long fin = millis() + ANIM_DURATION;
    while (millis() < fin) {
        tft.fillCircle(px, py, r, TFT_BLACK);
        x += vx; y += vy;
        if (x-r < 0)    { x=r;    vx= fabsf(vx); col=hsvColor(random(256)); }
        if (x+r >= w)   { x=w-r-1;vx=-fabsf(vx); col=hsvColor(random(256)); }
        if (y-r < 0)    { y=r;    vy= fabsf(vy); col=hsvColor(random(256)); }
        if (y+r >= h)   { y=h-r-1;vy=-fabsf(vy); col=hsvColor(random(256)); }
        tft.fillCircle((int)x, (int)y, r, col);
        px=(int)x; py=(int)y;
        delay(16);
    }
}

void prueba_backlight() {
    Serial.println("PRUEBA: Fade backlight");
    tft.fillScreen(TFT_WHITE);
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_BLACK);
    tft.setTextSize(2);
    tft.drawString("Fade backlight", tft.width()/2, tft.height()/2);
    for (int b = 255; b >= 0; b -= 5)  { blSet(b); delay(18); }
    delay(400);
    for (int b = 0; b <= 255; b += 5)  { blSet(b); delay(18); }
    delay(500);
}

void prueba_resumen() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_GREEN);   tft.setTextSize(2);
    tft.drawString("PRUEBAS OK!", tft.width()/2, tft.height()/2 - 40);
    tft.setTextColor(TFT_WHITE);   tft.setTextSize(1);
    tft.drawString("NyanBox ESP32", tft.width()/2, tft.height()/2);
    tft.drawString("ILI9341 240x320", tft.width()/2, tft.height()/2 + 15);
    tft.setTextColor(TFT_CYAN);
    tft.drawString("Repitiendo en 3s...", tft.width()/2, tft.height()/2 + 35);
    delay(3000);
}

// ── setup / loop ──────────────────────────────────────────────

void setup() {
    Serial.begin(115200);
    Serial.println("\n=== NyanBox Display Test ===");

    blInit();
    tft.init();
    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);

    Serial.printf("Display: %dx%d OK\n", tft.width(), tft.height());
}

void loop() {
    prueba_colores_solidos();
    prueba_barras();
    prueba_gradiente();
    prueba_texto();
    prueba_formas();
    prueba_tablero();
    prueba_animacion();
    prueba_backlight();
    prueba_resumen();
}
