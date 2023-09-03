/*
 * File: usbHelper.h
 * Project: picoKey
 * File Created: Tuesday, 11th January 2022 9:51:37 pm
 * Author: Dhananjay Khairnar (khairnardm@gmail.com)
 * -----
 * Last Modified: Wednesday, 12th January 2022 12:08:37 am
 * Modified By: 8-DK (khairnardm@gmail.com>)
 * -----
 * Copyright 2021 - 2022 https://github.com/8-DK
 */

#ifndef __USBHELPER__
#define __USBHELPER__

#include "common.h"
#include <stdio.h>
#include <stdarg.h>

using namespace std;

#include "usb_descriptors.h"

/* Blink pattern
 * - 250 ms  : device not mounted
 * - 1000 ms : device mounted
 * - 2500 ms : device is suspended
 */
enum  {
  BLINK_NOT_MOUNTED = 250,
  BLINK_MOUNTED = 1000,
  BLINK_SUSPENDED = 2500,
};

void mPrintf( const char * format, ... );

class USBHelper
{
    static USBHelper *instance;

    static hid_keyboard_report_t keyboardData;
    static hid_mouse_report_t mouseData;
    static hid_gamepad_report_t gamepadData;    
    static uint16_t multimediKey;

    static int sendKeyBoardData;
    static int sendMouseData;    
    static int sendGamePadData;
    static int sendMultimediaData;
public: 
    static int sendNextData;

    static auto *getInstance() {
        if (!instance)
            instance = new USBHelper;
        return instance;
    }
    static uint32_t blink_interval_ms;
    
    USBHelper();
    static void send_hid_report(uint8_t report_id);
    static void usbMainTask( void * pvParameters );
    static uint32_t getVcomData(uint8_t *buff, uint16_t count = 1,uint8_t itf = 0);
    static void sendToVcom(uint8_t itf, uint8_t *buf, uint32_t count);
    static void sendStringToKeyBoard(const char* buffer,uint16_t len = 0);
    static void sendKeyStroke(uint8_t keyCode, uint8_t modifier = 0);
    static void sendMouse(uint8_t x, uint8_t y,uint8_t buttons = 0, uint8_t wheel = 0, uint8_t pan = 0);
    static void sendMultimediaKey(uint16_t  keyCode);
    static void sendGamePad(uint8_t x, uint8_t y, uint8_t z = 0, uint8_t rz = 0,uint8_t rx = 0 ,uint8_t ry = 0,uint8_t hat = 0, uint32_t buttons = 0);
    static void usbLoop();
};


#endif