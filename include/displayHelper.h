/*
 * File: displayHelper.h
 * Project: picoKey
 * File Created: Wednesday, 12th January 2022 10:36:10 pm
 * Author: Dhananjay Khairnar (khairnardm@gmail.com)
 * -----
 * Last Modified: Wednesday, 12th January 2022 10:36:13 pm
 * Modified By: 8-DK (khairnardm@gmail.com>)
 * -----
 * Copyright 2021 - 2022 https://github.com/8-DK
 */

#ifndef __DISPLAYHLPR__
#define __DISPLAYHLPR__
#include "common.h"
#include "ss_oled.hpp"

#define SDA_PIN 4
#define SCL_PIN 5
#define PICO_I2C i2c0
#define I2C_SPEED 100 * 1000


#define OLED_WIDTH 128
#define OLED_HEIGHT 64

typedef void (*ListFunc)(vector<string> m_dispList);

class DisplayHelper{

    // static const unsigned char screen1;
    static uint8_t raspberry26x32[];

    static DisplayHelper *instance;
    static picoSSOLED myOled;    
    static uint8_t ucBuffer[1024];
    static vector<string> dispList;
    static int curretScrollerIndex;
    static int totalListCount;
    static DISP_STATS dispState;
    static ListFunc currListFunction;
public: 
    static auto *getInstance() {
        if (!instance)
            instance = new DisplayHelper;
        return instance;
    }

    DisplayHelper();
    static void displayTask( void * pvParameters );
    static void displaySetState(DISP_STATS mDispState);
    static void displayLoop();    
    static void scrnWithLockMsg(string message);
    static void showlist(string message);
    static void showlist1(vector<string> m_dispList);
    static void showlist2(vector<string> m_dispList);
    static void updateList(vector<string> m_dispList);

    static void writeToDisp(int iScrollX, int x, int y, char *szMsg, int iSize, bool bInvert, bool bRender);
    //disp controls
    static int getCurrOptInd();
    static void resetListInd(int newIndex = 0);
    static void listSelUp();
    static void listSelDown();
};

#endif
