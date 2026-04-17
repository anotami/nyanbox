#include <Arduino.h>
#include <TFT_eSPI.h>
#include "display_config.h"

TFT_eSPI tft = TFT_eSPI();

// ── helpers ──────────────────────────────────────────────────

void backlightInit() {
    ledcSetup(BL_PWM_CHANNEL, BL_PWM_FREQ, BL_PWM_RESOLUTION);
    ledcAttachPin(TFT_BACKLIGHT_PIN, BL_PWM_CHANNEL);
    ledcWrite(BL_PWM_CHANNEL, 255);
}

void backlightSet(uint8_t brightness) {
    ledcWrite(BL_PWM_CHANNEL, brightness);
}

uint16_t rainbow(float t) {
    // t in [0, 1] → full hue cycle
    float r = max(0.f, min(1.f,  2.f - fabsf(t * 6.f - 1.f) - fabsf(t * 6.f - 5.f)));
    float g = max(0.f, min(1.f,  2.f - fabsf(t * 6.f - 3.f)));
    float b = max(0.f, min(1.f,  2.f - fabsf(t * 6.f - 3.f) - fabsf(t * 6.f - 5.f)));
    // Proper hue: use HSV→RGB
    uint8_t h = (uint8_t)(t * 255);
    uint8_t s = 255, v = 255;
    uint8_t region = h / 43;
    uint8_t rem = (h - region * 43) * 6;
    uint8_t p = 0, q = (v * (255 - ((s * rem) >> 8))) >> 8;
    uint8_t t2 = (v * (255 - ((s * (255 - rem)) >> 8))) >> 8;
    (void)r; (void)g; (void)b; // suppress unused warning
    switch (region) {
        case 0: return tft.color565(v, t2, p);
        case 1: return tft.color565(q, v, p);
        case 2: return tft.color565(p, v, t2);
        case 3: return tft.color565(p, q, v);
        case 4: return tft.color565(t2, p, v);
        default:return tft.color565(v, p, q);
    }
}

// ── test stages ──────────────────────────────────────────────

void testSolidColors() {
    Serial.println("[TEST] Solid colors");
    const struct { uint16_t color; const char* name; } colors[] = {
        { TFT_RED,     "RED"     },
        { TFT_GREEN,   "GREEN"   },
        { TFT_BLUE,    "BLUE"    },
        { TFT_WHITE,   "WHITE"   },
        { TFT_BLACK,   "BLACK"   },
        { TFT_YELLOW,  "YELLOW"  },
        { TFT_CYAN,    "CYAN"    },
        { TFT_MAGENTA, "MAGENTA" },
    };
    for (auto& c : colors) {
        tft.fillScreen(c.color);
        tft.setTextColor(c.color == TFT_BLACK ? TFT_WHITE : TFT_BLACK);
        tft.setTextSize(2);
        tft.setTextDatum(MC_DATUM);
        tft.drawString(c.name, tft.width() / 2, tft.height() / 2);
        Serial.printf("  %s\n", c.name);
        delay(TEST_STEP_DELAY);
    }
}

void testColorBars() {
    Serial.println("[TEST] Color bars");
    int w = tft.width();
    int h = tft.height();
    int barW = w / 8;
    const uint16_t bars[] = {
        TFT_WHITE, TFT_YELLOW, TFT_CYAN, TFT_GREEN,
        TFT_MAGENTA, TFT_RED, TFT_BLUE, TFT_BLACK
    };
    for (int i = 0; i < 8; i++) {
        tft.fillRect(i * barW, 0, barW, h, bars[i]);
    }
    delay(TEST_STEP_DELAY);
}

void testGradient() {
    Serial.println("[TEST] Gradient");
    int w = tft.width();
    int h = tft.height();
    for (int x = 0; x < w; x++) {
        uint16_t col = rainbow((float)x / w);
        tft.drawFastVLine(x, 0, h, col);
    }
    delay(TEST_STEP_DELAY);
}

void testText() {
    Serial.println("[TEST] Text rendering");
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(TL_DATUM);

    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1);
    tft.drawString("NyanBox Display Test", 5, 5);

    tft.setTextSize(2);
    tft.setTextColor(TFT_CYAN);
    tft.drawString("ESP32-WROOM-32", 5, 20);

    tft.setTextSize(1);
    tft.setTextColor(TFT_YELLOW);
    tft.drawString("ABCDEFGHIJKLMNOPQRSTUVWXYZ", 5, 50);
    tft.drawString("abcdefghijklmnopqrstuvwxyz", 5, 62);
    tft.drawString("0123456789 !@#$%^&*()", 5, 74);

    tft.setTextColor(TFT_GREEN);
    tft.drawString("Resolution:", 5, 95);
    tft.drawString(String(tft.width()) + " x " + String(tft.height()), 5, 107);

    tft.setTextColor(TFT_MAGENTA);
    tft.drawString(FW_VERSION, 5, 128);

    delay(TEST_STEP_DELAY * 2);
}

void testShapes() {
    Serial.println("[TEST] Shapes");
    tft.fillScreen(TFT_BLACK);
    int cx = tft.width() / 2;
    int cy = tft.height() / 2;

    // Filled and outlined rectangles
    tft.fillRect(10, 10, 60, 40, TFT_BLUE);
    tft.drawRect(10, 10, 60, 40, TFT_WHITE);

    // Rounded rect
    tft.fillRoundRect(cx - 40, 10, 80, 40, 10, TFT_GREEN);
    tft.drawRoundRect(cx - 40, 10, 80, 40, 10, TFT_WHITE);

    // Circles
    tft.fillCircle(cx, cy, 50, TFT_RED);
    tft.drawCircle(cx, cy, 50, TFT_WHITE);
    tft.drawCircle(cx, cy, 35, TFT_YELLOW);
    tft.drawCircle(cx, cy, 20, TFT_CYAN);

    // Triangle
    tft.fillTriangle(10, tft.height() - 10,
                     70, tft.height() - 60,
                     130, tft.height() - 10,
                     TFT_MAGENTA);
    tft.drawTriangle(10, tft.height() - 10,
                     70, tft.height() - 60,
                     130, tft.height() - 10,
                     TFT_WHITE);

    // Lines fan
    for (int angle = 0; angle < 360; angle += 20) {
        float rad = angle * PI / 180.0f;
        int x2 = tft.width() - 40 + (int)(30 * cosf(rad));
        int y2 = tft.height() - 40 + (int)(30 * sinf(rad));
        tft.drawLine(tft.width() - 40, tft.height() - 40, x2, y2, rainbow((float)angle / 360.f));
    }

    delay(TEST_STEP_DELAY * 2);
}

void testCheckerboard() {
    Serial.println("[TEST] Checkerboard (pixel accuracy)");
    int w = tft.width();
    int h = tft.height();
    int cell = 8;
    for (int y = 0; y < h; y += cell) {
        for (int x = 0; x < w; x += cell) {
            bool white = ((x / cell + y / cell) % 2 == 0);
            tft.fillRect(x, y, cell, cell, white ? TFT_WHITE : TFT_BLACK);
        }
    }
    delay(TEST_STEP_DELAY);
}

void testAnimation() {
    Serial.println("[TEST] Bouncing ball animation (~5 s)");
    tft.fillScreen(TFT_BLACK);

    int w = tft.width();
    int h = tft.height();
    int r = 15;
    float x = w / 2, y = h / 2;
    float vx = 3.5f, vy = 2.3f;
    uint16_t ballColor = TFT_CYAN;
    uint16_t trailColor = TFT_BLACK;

    unsigned long end = millis() + 5000;
    int prevX = (int)x, prevY = (int)y;

    while (millis() < end) {
        tft.fillCircle(prevX, prevY, r, trailColor);

        x += vx;
        y += vy;

        if (x - r < 0)     { x = r;     vx = fabsf(vx); ballColor = rainbow(random(256) / 255.f); }
        if (x + r >= w)    { x = w-r-1; vx = -fabsf(vx); ballColor = rainbow(random(256) / 255.f); }
        if (y - r < 0)     { y = r;     vy = fabsf(vy); ballColor = rainbow(random(256) / 255.f); }
        if (y + r >= h)    { y = h-r-1; vy = -fabsf(vy); ballColor = rainbow(random(256) / 255.f); }

        tft.fillCircle((int)x, (int)y, r, ballColor);
        prevX = (int)x;
        prevY = (int)y;
        delay(ANIM_FRAME_DELAY);
    }
}

void testBacklight() {
    Serial.println("[TEST] Backlight PWM fade");
    tft.fillScreen(TFT_WHITE);
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_BLACK);
    tft.setTextSize(2);
    tft.drawString("Backlight Fade", tft.width()/2, tft.height()/2);

    // Fade out
    for (int b = 255; b >= 0; b -= 5) { backlightSet(b); delay(20); }
    delay(500);
    // Fade in
    for (int b = 0; b <= 255; b += 5)  { backlightSet(b); delay(20); }
    delay(500);
}

void testSummary() {
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);

    tft.setTextColor(TFT_GREEN);
    tft.setTextSize(2);
    tft.drawString("ALL TESTS DONE!", tft.width()/2, tft.height()/2 - 30);

    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1);
    tft.drawString(BOARD_NAME, tft.width()/2, tft.height()/2);
    tft.drawString(FW_VERSION, tft.width()/2, tft.height()/2 + 15);

    tft.setTextColor(TFT_CYAN);
    tft.drawString("Looping in 3s...", tft.width()/2, tft.height()/2 + 35);
    delay(3000);
}

// ── main ─────────────────────────────────────────────────────

void setup() {
    Serial.begin(115200);
    Serial.println("\n=== NyanBox Display Test ===");
    Serial.printf("Driver: %dx%d\n", TFT_WIDTH, TFT_HEIGHT);

    backlightInit();

    tft.init();
    tft.setRotation(0);   // 0=portrait, 1=landscape, 2=portrait-flip, 3=landscape-flip
    tft.fillScreen(TFT_BLACK);

    Serial.println("Display init OK");
}

void loop() {
    testSolidColors();
    testColorBars();
    testGradient();
    testText();
    testShapes();
    testCheckerboard();
    testAnimation();
    testBacklight();
    testSummary();
}
