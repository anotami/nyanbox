#pragma once

// ============================================================
//  NyanBox - Display Pin Configuration
//  Edit these values to match your wiring
// ============================================================

// Backlight pin (-1 = always on / hardware controlled)
#define TFT_BACKLIGHT_PIN   21

// Backlight PWM settings
#define BL_PWM_CHANNEL      0
#define BL_PWM_FREQ         5000
#define BL_PWM_RESOLUTION   8    // 8-bit: 0-255

// Test sequence timing (milliseconds)
#define TEST_STEP_DELAY     2000
#define ANIM_FRAME_DELAY    16   // ~60 fps

// Demo text
#define BOARD_NAME  "NyanBox ESP32"
#define FW_VERSION  "v1.0 Display Test"
