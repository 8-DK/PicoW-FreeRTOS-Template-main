/*
 * File: mainApp.h
 * Project: picoKey
 * File Created: Thursday, 13th January 2022 12:18:06 am
 * Author: Dhananjay Khairnar (khairnardm@gmail.com)
 * -----
 * Last Modified: Thursday, 13th January 2022 9:14:04 pm
 * Modified By: 8-DK (khairnardm@gmail.com>)
 * -----
 * Copyright 2021 - 2022 https://github.com/8-DK
 */

#ifndef __MAINAPP__
#define __MAINAPP__
#include "common.h"
#define EAddress 0
#define UnlocSeqAddr EAddress
#define ListCountAddr UnlocSeqAddr+1
#define ListDataAddr  ListCountAddr+1

#define MAX_COUNT_UNLOCK_SEQ 6
class MainApp
{
    static MainApp *instance;
    static uint8_t mUnlockSeq[MAX_COUNT_UNLOCK_SEQ];
    static uint8_t inUnlkSeq[MAX_COUNT_UNLOCK_SEQ];
    
    static uint8_t mListCount;
    static uint8_t * startAddress;
    static vector<string> userNameList;
    static vector<string> passwordList;

    static MAINAPP_STATS mainAppState;
    static TimerHandle_t xTimerCdcDataTimeout;

    static int currentKeyIndex;
    static int currentSeqInd;
    static int setNewUnlockSeq;
    static bool isDeviceUnlocked;

    static bool isCommandRcv;
    static uint8_t currentDataIndex;

public: 
    static auto *getInstance() {
        if (!instance)
            instance = new MainApp;
        return instance;
    }
    MainApp();
    ~MainApp();
    static void eraseEeprom();
    static void storeListInEeprom();
    static void readListFromEeprom();
    static void vCdcDataTimeoutCallback( TimerHandle_t xTimer );
    static void mainAppLoop();
    static void mainApp( void * pvParameters );
};


#endif