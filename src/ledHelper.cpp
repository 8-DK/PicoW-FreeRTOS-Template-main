/*
 * File: ledHelper.cpp
 * Project: picoKey
 * File Created: Wednesday, 12th January 2022 11:30:35 pm
 * Author: Dhananjay Khairnar (khairnardm@gmail.com)
 * -----
 * Last Modified: Wednesday, 12th January 2022 11:30:37 pm
 * Modified By: 8-DK (khairnardm@gmail.com>)
 * -----
 * Copyright 2021 - 2022 https://github.com/8-DK
 */
#include "ledHelper.h"
#include "NeoPixelConnect.h"
LedHelper *LedHelper::instance = 0;
int LedHelper::pinNums[TOTAL_LED_COUNT] = {LED_1_PIN,LED_2_PIN,LED_3_PIN};
pico_cpp::GPIO_Pin *LedHelper::m_leds[TOTAL_LED_COUNT];

LedHelper::LedHelper()
{
}

LedHelper::~LedHelper()
{
}


void LedHelper::initLed()
{
    for(int i=0 ; i<TOTAL_LED_COUNT ;i++)
    {
        m_leds[i] = new pico_cpp::GPIO_Pin(pinNums[i],pico_cpp::PinType::Output);
    } 
}

void LedHelper::ledTask( void * pvParameters )
{
    NeoPixelConnect p(16, 1, pio0, 0);
    while(1)
    {
        p.neoPixelFill(255, 0, 0, true);
        delay(1000);
        p.neoPixelClear(true);
        delay(1000);

        p.neoPixelFill(0, 255, 0, true);
        delay(1000);
        p.neoPixelClear(true);
        delay(1000);

        p.neoPixelFill(0, 0, 255, true);
        delay(1000);
        p.neoPixelClear(true);
        delay(1000);
    }
    initLed();
    while(1)
    {
           m_leds[0]->set_high();
           vTaskDelay(50 / portTICK_PERIOD_MS);
           m_leds[0]->set_low();
           vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}