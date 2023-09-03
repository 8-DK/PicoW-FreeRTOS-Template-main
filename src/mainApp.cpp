/*
 * File: mainApp.cpp
 * Project: picoKey
 * File Created: Thursday, 13th January 2022 12:17:57 am
 * Author: Dhananjay Khairnar (khairnardm@gmail.com)
 * -----
 * Last Modified: Thursday, 13th January 2022 9:13:58 pm
 * Modified By: 8-DK (khairnardm@gmail.com>)
 * -----
 * Copyright 2021 - 2022 https://github.com/8-DK
 */

#include "mainApp.h"
#include "displayHelper.h"
#include "hardware/flash.h"
#include "usbHelper.h"
#include "keyHelper.h"
#include "cJSON.h"
#include "Storage.h"

MainApp *MainApp::instance = nullptr;
uint8_t MainApp::mUnlockSeq[MAX_COUNT_UNLOCK_SEQ] = {0};
uint8_t MainApp::inUnlkSeq[MAX_COUNT_UNLOCK_SEQ] = {0};
uint8_t MainApp::mListCount = 0;
MAINAPP_STATS MainApp::mainAppState = EM_MAINAPP_INIT;
vector<string> MainApp::userNameList;
vector<string> MainApp::passwordList;
TimerHandle_t MainApp::xTimerCdcDataTimeout;

int MainApp::currentKeyIndex = 0;
int MainApp::currentSeqInd = 0;
int MainApp::setNewUnlockSeq = 0;
bool MainApp::isDeviceUnlocked = false;
bool MainApp::isCommandRcv = false;
uint8_t MainApp::currentDataIndex = 0;

// uint32_t MainApp::startAddress = (uint32_t) (XIP_BASE+ 0x103ff000);
#define FLASH_TARGET_OFFSET (2044 * 1024)
uint8_t * MainApp::startAddress = (uint8_t *) (XIP_BASE + FLASH_TARGET_OFFSET);

MainApp::MainApp()
{
    if(mListCount == 0xFF)
    {
        mListCount = 0;
    }    
}

MainApp::~MainApp()
{

}

void MainApp::eraseEeprom()
{
    taskENTER_CRITICAL();
    flash_range_erase(FLASH_TARGET_OFFSET, 4096);
    taskEXIT_CRITICAL();
}

void MainApp::storeListInEeprom()
{
    taskENTER_CRITICAL();
    mListCount = 20;
    uint8_t buffer[4096];
    int j = 0;
    buffer[j++] = mListCount;
    for(int i = 0 ; i < MAX_COUNT_UNLOCK_SEQ ; i ++)
        buffer[j++] = mUnlockSeq[i];
    for(int i = 0 ; i < mListCount ; i++)
    {
        char listData[100] = {0};    
        sprintf(listData,"%d,Password",i);
        mPrintf("%s",listData);
        memcpy(buffer+j+(i*100),listData,100);
        delay(100);
    }
    flash_range_erase(FLASH_TARGET_OFFSET, 4096);
    flash_range_program(FLASH_TARGET_OFFSET, buffer, 4096);
    taskEXIT_CRITICAL();
}

void MainApp::readListFromEeprom()
{
    taskENTER_CRITICAL();
    mListCount = 0;    
    uint8_t* ptr;
    int j = 0;

    mListCount = startAddress[j++];
    if(mListCount > 35)
        mListCount = 0;

    for(int i = 0 ; i < MAX_COUNT_UNLOCK_SEQ ; i++)
    {
        mUnlockSeq[i] = startAddress[j+i];
        if((mUnlockSeq[i] != 0) && (mUnlockSeq[i] != 1))
            mUnlockSeq[i] = 0;
    }
    j = j + MAX_COUNT_UNLOCK_SEQ;
    vector<string> m_dispList;
    
    for(int i = 0 ; i < mListCount ; i++)
    {
        char listData[100] = {0};    
        memcpy(listData,startAddress+j+(i*100),100);
        m_dispList.push_back(listData);
    }
    DisplayHelper::getInstance()->updateList(m_dispList);
    taskEXIT_CRITICAL();
    mPrintf("Old unlock sequence : ");
    for(int i = 0 ; i < MAX_COUNT_UNLOCK_SEQ ; i++)
    {
        mPrintf("%d)[%d]\r\n",i,mUnlockSeq[i]);
    }       
    mPrintf("\n");
}

void MainApp::vCdcDataTimeoutCallback( TimerHandle_t xTimer )
{
    isCommandRcv = true;
    currentDataIndex = 0;
}

void MainApp::mainApp( void * pvParameters )
{    
    delay(10000);
    mPrintf("Main Task Started\n");
    Storage storage;
    storage.W25qxx_Init();

    char jsonBuffer[200] = {0}; 
    
    readListFromEeprom();
    // storeListInEeprom();
    xTimerCdcDataTimeout = xTimerCreate
                   ("cdcDataTmout",
                     toMs(500),
                     pdFALSE,
                     ( void * ) 0,
                     vCdcDataTimeoutCallback
                   );
    while (1)
    {                     
        const EventBits_t xBitsToWaitFor  = (KEY_TASK_KEY_PRESS_EVENT);
        EventBits_t xEventGroupValue;
        while(1)
        {
            KEY_ID mKey= KeyHelper::readKeyPress();
                    
            uint32_t const btn = board_button_read();
            if(btn)
            {
                const char buffer[] = "<style>\n*{background-color:#352e2e;font-family:monospace;color:#3cff01;padding:0}button,datalist{background-color:#555}input[type=text]{color:#b3ffb3;background-color:#665656;border:1px solid;border-color:#696 #363 #363 #696}#serialResults{font-family:monospace;white-space:pre;height:calc(100% - 120px);width:calc(100% - 20px);border-style:solid;overflow:scroll;background-color:#585c5c;padding:10px;margin:0}\n</style>\n<button onclick=\"connectSerial()\">Connect</button>\nBaud:\n<input type=\"text\" id=\"baud\" list=\"baudList\" style=\"width: 10ch;\" onclick=\"this.value = ''\"\n onchange=\"localStorage.baud = this.value\">\n<datalist id=\"baudList\">\n <option value=\"110\">110</option>\n <option value=\"300\">300</option>\n <option value=\"600\">600</option>\n <option value=\"1200\">1200</option>\n <option value=\"2400\">2400</option>\n <option value=\"4800\">4800</option>\n <option value=\"9600\">9600</option>\n <option value=\"14400\">14400</option>\n <option value=\"19200\">19200</option>\n <option value=\"38400\">38400</option>\n <option value=\"57600\">57600</option>\n <option value=\"115200\">115200</option>\n <option value=\"128000\">128000</option>\n <option value=\"256000\">256000</option>\n</datalist>\n<button onclick=\"serialResultsDiv.innerHTML = '';\">Clear</button>\n<br>\n<input type=\"text\" id=\"lineToSend\" style=\"width:calc(100% - 165px)\">\n<button onclick=\"sendSerialLine()\" style=\"width:45px\">Send</button>\n<button onclick=\"sendCharacterNumber()\" style=\"width:100px\">Send Char</button>\n<br>\n<input type=\"checkbox\" id=\"addLine\" onclick=\"localStorage.addLine = this.checked;\" checked>\n<label for=\"addLine\">send with /r/n</label>\n<input type=\"checkbox\" id=\"echoOn\" onclick=\"localStorage.echoOn = this.checked;\" checked>\n<label for=\"echoOn\">echo</label>\n<br>\n<div id=\"serialResults\">\n</div>\n<script>\nvar port,textEncoder,writableStreamClosed,writer;async function connectSerial(){try{port=await navigator.serial.requestPort(),await port.open({baudRate:document.getElementById(\"baud\").value}),listenToPort(),textEncoder=new TextEncoderStream,writableStreamClosed=textEncoder.readable.pipeTo(port.writable),writer=textEncoder.writable.getWriter()}catch{alert(\"Serial Connection Failed\")}}async function sendCharacterNumber(){document.getElementById(\"lineToSend\").value=String.fromCharCode(document.getElementById(\"lineToSend\").value)}async function sendSerialLine(){dataToSend=document.getElementById(\"lineToSend\").value,1==document.getElementById(\"addLine\").checked&&(dataToSend+=\"\r\n\"),1==document.getElementById(\"echoOn\").checked&&appendToTerminal(\"> \"+dataToSend),await writer.write(dataToSend),document.getElementById(\"lineToSend\").value=\"\"}async function listenToPort(){const e=new TextDecoderStream,t=(port.readable.pipeTo(e.writable),e.readable.getReader());for(;;){const{value:e,done:n}=await t.read();if(n)break;appendToTerminal(e)}}const serialResultsDiv=document.getElementById(\"serialResults\");async function appendToTerminal(e){serialResultsDiv.innerHTML+=e,serialResultsDiv.innerHTML.length>3e3&&(serialResultsDiv.innerHTML=serialResultsDiv.innerHTML.slice(serialResultsDiv.innerHTML.length-3e3)),serialResultsDiv.scrollTop=serialResultsDiv.scrollHeight}document.getElementById(\"lineToSend\").addEventListener(\"keyup\",async function(e){13===e.keyCode&&sendSerialLine()}),document.getElementById(\"baud\").value=null==localStorage.baud?9600:localStorage.baud,document.getElementById(\"addLine\").checked=\"false\"!=localStorage.addLine,document.getElementById(\"echoOn\").checked=\"false\"!=localStorage.echoOn;\n</script>\n";
                USBHelper::sendStringToKeyBoard(buffer);
            }       

            uint32_t rcvCnt = 0;
            char tempBuff[64] = {0};   
            if((rcvCnt = USBHelper::getVcomData((uint8_t*)tempBuff,64,1)) > 0)
            {
                if((rcvCnt+currentDataIndex) > sizeof(jsonBuffer))
                {
                    memset(jsonBuffer,0,sizeof(jsonBuffer));
                    currentDataIndex = 0;
                }
                memcpy(jsonBuffer+currentDataIndex,tempBuff,rcvCnt);
                currentDataIndex += rcvCnt;
                xTimerStart( xTimerCdcDataTimeout, 0 ); //restart timer                  
            }  
            if(isCommandRcv)
            {
                isCommandRcv = false;                    
                // mPrintf("JSON > %s\n",jsonBuffer);
                cJSON *root = cJSON_Parse(jsonBuffer);
                if(root != NULL) 
                {
                    cJSON *commandJsn = cJSON_GetObjectItemCaseSensitive(root, "command");
                    cJSON *dataJsn = cJSON_GetObjectItemCaseSensitive(root, "data");
                    cJSON *currentItemJsn = cJSON_GetObjectItemCaseSensitive(root, "currentItem");
                    cJSON *totalItemJsn = cJSON_GetObjectItemCaseSensitive(root, "totalItem");

                    mPrintf("command : %s, currentItem : %d, totalItem : %d, data : %s,\n",commandJsn->valuestring,currentItemJsn->valueint,totalItemJsn->valueint,dataJsn->valuestring);
                    delay(10);
                    if(commandJsn != NULL )free(commandJsn);
                    if(dataJsn != NULL )free(dataJsn);
                    if(currentItemJsn != NULL )free(currentItemJsn);
                    if(totalItemJsn != NULL )free(totalItemJsn);
                    free(root);
                }
                memset(jsonBuffer,0,sizeof(jsonBuffer));
            }                

            switch(mainAppState)
            {
                    case EM_MAINAPP_INIT:
                    DisplayHelper::displaySetState(EM_DISP_INIT);
                    mainAppState = EM_MAINAPP_WLCM;
                    break; 

                    case EM_MAINAPP_IDEAL:
                    {
                        {         
                            isDeviceUnlocked = false;                                                                                                                       
                            if(mKey == P1)
                            {                        
                                if(currentSeqInd >= MAX_COUNT_UNLOCK_SEQ)     
                                    break;   
                                inUnlkSeq[currentSeqInd++] = 1;
                                mPrintf("Key P1");
                                DisplayHelper::writeToDisp(0, (12*currentSeqInd)+12,4,(char*)"*", FONT_12x16,0,1);                                
                            }
                            else if(mKey == P2)
                            {                                
                                if(currentSeqInd >= MAX_COUNT_UNLOCK_SEQ)     
                                   break;
                                inUnlkSeq[currentSeqInd++] = 2; 
                                mPrintf("Key P2");
                                DisplayHelper::writeToDisp(0, (12*currentSeqInd)+12, 4,(char*)"*", FONT_12x16,0,1);                                                     
                            }
                            else if(mKey == P3) //ok key
                            {
                                if(setNewUnlockSeq == 1) //ok key
                                {
                                    if(currentSeqInd < MAX_COUNT_UNLOCK_SEQ-1)     
                                    {
                                        DisplayHelper::writeToDisp(0, 12, 4,(char*)"Need 6 digits", FONT_8x8,0,1);
                                        delay(2000);
                                        mainAppState = EM_MAINAPP_FIRSTTIME;
                                        break;     
                                    }
                                    memcpy(&mUnlockSeq,&inUnlkSeq,MAX_COUNT_UNLOCK_SEQ);
                                    mPrintf("new unlock sequence %x\n",mUnlockSeq);
                                    storeListInEeprom();
                                    setNewUnlockSeq = 0;
                                    DisplayHelper::writeToDisp(0, 12, 4,(char*)"Sequence Saved", FONT_8x8,0,1);
                                    delay(2000);
                                    mainAppState = EM_MAINAPP_LOCKED;
                                    break;
                                }
                                if(memcmp(&mUnlockSeq,&inUnlkSeq,MAX_COUNT_UNLOCK_SEQ) == 0)
                                {
                                    memset(inUnlkSeq,0,MAX_COUNT_UNLOCK_SEQ);
                                    currentSeqInd = 0;         
                                    mPrintf("Device unlocked\n");                                    
                                    DisplayHelper::displaySetState(EM_DISP_UNLOCKSCR);
                                    delay(2000);
                                    readListFromEeprom();    
                                    DisplayHelper::resetListInd();                    
                                    DisplayHelper::displaySetState(EM_DISP_LIST);
                                    mainAppState = EM_MAINAPP_UNLOCKED;
                                }
                                else
                                {
                                    mPrintf("Device locked\n");
                                    DisplayHelper::writeToDisp(0, 12, 4,(char*)"Invalid PIN", FONT_8x8,0,1);
                                    delay(2000);
                                    mainAppState = EM_MAINAPP_LOCKED;
                                }
                                mPrintf("unlock sequence : ");
                                for(int i = 0 ; i < MAX_COUNT_UNLOCK_SEQ ; i++)
                                    mPrintf("%d)[%d]==[%d]\r\n",i,mUnlockSeq[i],inUnlkSeq[i]);
                                mPrintf("\n");
                                memset(inUnlkSeq,0,MAX_COUNT_UNLOCK_SEQ);
                                currentSeqInd = 0;
                            }
                            else if(mKey == P4) //ok key
                            {
                                if(setNewUnlockSeq == 1) //ok key
                                {                                    
                                    mainAppState = EM_MAINAPP_FIRSTTIME;                                    
                                }
                                else
                                {
                                     mainAppState = EM_MAINAPP_LOCKED;
                                }
                            }
                        }
                    }
                    break; 
                    
                    case EM_MAINAPP_WLCM:
                        DisplayHelper::displaySetState(EM_DISP_WLCM);
                        delay(2000);
                        if(memcmp(mUnlockSeq,inUnlkSeq,MAX_COUNT_UNLOCK_SEQ) == 0)
                        {
                            setNewUnlockSeq = 1;
                            mainAppState = EM_MAINAPP_FIRSTTIME;
                        }
                        else
                            mainAppState = EM_MAINAPP_LOCKED;
                    break; 
                    
                    case EM_MAINAPP_FIRSTTIME:        
                        memset(inUnlkSeq,0,MAX_COUNT_UNLOCK_SEQ);
                        currentSeqInd = 0;         
                        DisplayHelper::displaySetState(EM_DISP_NEWLOCKSCR);
                        mainAppState = EM_MAINAPP_IDEAL;                    
                    break; 
                    
                    case EM_MAINAPP_LOCKED:
                        memset(inUnlkSeq,0,MAX_COUNT_UNLOCK_SEQ);
                        currentSeqInd = 0;         
                        DisplayHelper::displaySetState(EM_DISP_LOCKSCR);
                        mainAppState = EM_MAINAPP_IDEAL;
                    break; 
                    
                    case EM_MAINAPP_UNLOCKED:
                            isDeviceUnlocked = true;
                            if(mKey == P1)
                            {     
                                taskENTER_CRITICAL();
                                DisplayHelper::listSelUp();                   
                                taskEXIT_CRITICAL();
                            }
                            else if(mKey == P2)
                            {  
                                taskENTER_CRITICAL();
                                DisplayHelper::listSelDown();
                                taskEXIT_CRITICAL();
                            }
                            else if(mKey == P3) //ok key
                            {
                                DisplayHelper::displaySetState(EM_DISP_TYPE_USER);
                                mainAppState = EM_MAINAPP_SEND_USERNAME;
                            } 
                            else if(mKey == P4) //Open Menu
                            {
                                vector<string> optionList;
                                optionList.push_back("New unlock sequence");
                                optionList.push_back("Erase Device");
                                optionList.push_back("Add new entry");
                                optionList.push_back("Log out");  
                                taskENTER_CRITICAL(); 
                                DisplayHelper::resetListInd();                     
                                DisplayHelper::showlist2(optionList);
                                taskEXIT_CRITICAL();
                                mainAppState = EM_MAINAPP_SELECT;
                            }                       
                    break; 
                    
                    case EM_MAINAPP_SELECT:
                    {                        
                        if(mKey == P1)
                        {     
                            taskENTER_CRITICAL();
                            DisplayHelper::listSelUp();                   
                            taskEXIT_CRITICAL();
                        }
                        else if(mKey == P2)
                        {  
                            taskENTER_CRITICAL();
                            DisplayHelper::listSelDown();
                            taskEXIT_CRITICAL();
                        }
                        else if(mKey == P3) //ok key
                        {
                            if(DisplayHelper::getCurrOptInd() == 0)                            
                            {
                                setNewUnlockSeq = 1;
                                mainAppState = EM_MAINAPP_FIRSTTIME;
                            }
                            else if(DisplayHelper::getCurrOptInd() == 1)                            
                            {
                                eraseEeprom();
                                mainAppState = EM_MAINAPP_LOCKED;
                            }
                            else if(DisplayHelper::getCurrOptInd() == 2)                            
                            {
                                eraseEeprom();
                            }
                            else if(DisplayHelper::getCurrOptInd() == 3)                            
                            {
                                memset(inUnlkSeq,0,MAX_COUNT_UNLOCK_SEQ);
                                currentSeqInd = 0;
                                mainAppState = EM_MAINAPP_LOCKED;
                            }
                        } 
                        else if(mKey == P4) //back
                        {
                            mainAppState = EM_MAINAPP_UNLOCKED;
                        }
                    }
                    break; 
                    
                    case EM_MAINAPP_SEND_USERNAME:
                            if(mKey == P3) //ok key
                            {
                                DisplayHelper::displaySetState(EM_DISP_TYPE_PASS);
                                mainAppState = EM_MAINAPP_SEND_PASS;
                            }
                    break; 
                    
                    case EM_MAINAPP_SEND_PASS:
                            if(mKey == P3) //ok key
                            {
                                DisplayHelper::displaySetState(EM_DISP_LIST);
                                mainAppState = EM_MAINAPP_UNLOCKED;
                            }
                    break;                     
            }
            // delay(1);
        }
    }
    
}
