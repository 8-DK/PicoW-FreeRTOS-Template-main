/*
 * File: keyHelper.cpp
 * Project: picoKey
 * File Created: Tuesday, 11th January 2022 11:10:03 pm
 * Author: Dhananjay Khairnar (khairnardm@gmail.com)
 * -----
 * Last Modified: Wednesday, 12th January 2022 12:07:53 am
 * Modified By: 8-DK (khairnardm@gmail.com>)
 * -----
 * Copyright 2021 - 2022 https://github.com/8-DK
 */

#include "keyHelper.h"
#include "usbHelper.h"
//Singletone

KeyHelper *KeyHelper::instance = 0;
KEY_ID     KeyHelper::lastKeyPressed = KEY_INVALID;
KEY_ID     KeyHelper::lastKeyFromISR = KEY_INVALID;
bool       KeyHelper::isKeyPressed = false;
int        KeyHelper::pinNums[TOTAL_KEY_COUNT] = {KEY_1_PIN,KEY_2_PIN,KEY_3_PIN,KEY_4_PIN,KEY_5_PIN,KEY_6_PIN};
int        KeyHelper::keyPressed[TOTAL_KEY_COUNT] = {RELEASED};

void disableIRQ(uint8_t keyId,uint32_t events){gpio_set_irq_enabled( KeyHelper::pinNums[keyId],events,false);}
void enableIRQ(uint8_t keyId,uint32_t events){gpio_set_irq_enabled( KeyHelper::pinNums[keyId],events,true);}

void gpio_callback(uint gpio, uint32_t events) {
    // Put the GPIO event(s) that just happened into event_str
    // so we can print it
    // mPrintf("Button press %d,%d\r\n",gpio,events);
    if(events == GPIO_IRQ_EDGE_RISE)
    {
        if (gpio == KeyHelper::pinNums[P1]) //detect only rising edge
        {            
            KeyHelper::lastKeyFromISR = P1;
            KeyHelper::keyPressed[P1] = PRESSED;
            disableIRQ(P1,events);
        } 
        else if(gpio == KeyHelper::pinNums[P2])
        {
            KeyHelper::lastKeyFromISR = P2;
            KeyHelper::keyPressed[P2] = PRESSED;
            disableIRQ(P2,events);
        }   
        else if(gpio == KeyHelper::pinNums[P3])
        {
            KeyHelper::lastKeyFromISR = P3;
            KeyHelper::keyPressed[P3] = PRESSED;
            disableIRQ(P3,events);
        }   
        else if(gpio == KeyHelper::pinNums[P4])
        {
            KeyHelper::lastKeyFromISR = P4;
            KeyHelper::keyPressed[P4] = PRESSED;
            disableIRQ(P4,events);
        }   
        else if(gpio == KeyHelper::pinNums[P5])
        {
            KeyHelper::lastKeyFromISR = P5;
            KeyHelper::keyPressed[P5] = PRESSED;
            disableIRQ(P5,events);
        }   
        else if(gpio == KeyHelper::pinNums[P6])
        {
            KeyHelper::lastKeyFromISR = P6;
            KeyHelper::keyPressed[P6] = PRESSED;
            disableIRQ(P6,events);
        } 
        KeyHelper::isKeyPressed = true;  
    }
}


KeyHelper::KeyHelper()
{ 
}

KEY_ID KeyHelper::readKeyPress()
{   
    KEY_ID mKeyID = lastKeyPressed;
    lastKeyPressed = KEY_INVALID;
    return mKeyID;
}

void KeyHelper::initKey()
{
    for(int i=0 ; i<TOTAL_KEY_COUNT ;i++)
    {        
        gpio_init(pinNums[i]);
        gpio_set_dir(pinNums[i],GPIO_IN);                
        gpio_pull_up(pinNums[i]);        
        gpio_set_irq_enabled_with_callback(pinNums[i], GPIO_IRQ_EDGE_RISE , true, &gpio_callback);
    }  
}

void KeyHelper::keyTask( void * pvParameters )
{
    initKey();
    int m = 0;
    while(1)
    {                 
        if(isKeyPressed == true)
        {
            isKeyPressed = false; 
            KEY_ID mlastKey = lastKeyFromISR;            
            //debounce delay
            delay(300);                  
            while(gpio_get(pinNums[mlastKey]) == false);
            lastKeyPressed =lastKeyFromISR;
            mPrintf("Key released %d\n",lastKeyPressed);                  
        }
        for(int i = 0 ; i < TOTAL_KEY_COUNT ;i++)
        {
            if((keyPressed[i] == PRESSED))
            {                                        
                keyPressed[i] = RELEASED;
                gpio_set_irq_enabled_with_callback(pinNums[i], GPIO_IRQ_EDGE_RISE , true, &gpio_callback);                                
            }
        }  
        delay(1);
    }
}