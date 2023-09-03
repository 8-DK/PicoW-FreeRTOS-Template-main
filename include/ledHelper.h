/*
 * File: ledHelper.h
 * Project: picoKey
 * File Created: Wednesday, 12th January 2022 11:30:27 pm
 * Author: Dhananjay Khairnar (khairnardm@gmail.com)
 * -----
 * Last Modified: Wednesday, 12th January 2022 11:30:28 pm
 * Modified By: 8-DK (khairnardm@gmail.com>)
 * -----
 * Copyright 2021 - 2022 https://github.com/8-DK
 */
#ifndef __LEDHELPER__
#define __LEDHELPER__

#include "common.h"
#define LED_PORT 0
#define LED_1_PIN 25
#define LED_2_PIN 11
#define LED_3_PIN 12

#define TOTAL_LED_COUNT 3

class LedHelper
{
    static LedHelper *instance;
public: 
    static auto *getInstance() {
        if (!instance)
        instance = new LedHelper;
        return instance;
    }
    static int pinNums[TOTAL_LED_COUNT];
    static pico_cpp::GPIO_Pin *m_leds[TOTAL_LED_COUNT];

    LedHelper(/* args */);
    ~LedHelper();
    static void initLed();
    static void ledTask( void * pvParameters );
};


#endif

