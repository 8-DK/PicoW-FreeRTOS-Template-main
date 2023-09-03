/*
 * File: usbHelper.cpp
 * Project: picoKey
 * File Created: Tuesday, 11th January 2022 9:51:11 pm
 * Author: Dhananjay Khairnar (khairnardm@gmail.com)
 * -----
 * Last Modified: Wednesday, 12th January 2022 12:08:32 am
 * Modified By: 8-DK (khairnardm@gmail.com>)
 * -----
 * Copyright 2021 - 2022 https://github.com/8-DK
 */

#include "usbHelper.h"


FILE __stdout; //STDOUT
FILE __stdin;  //STDIN


void mPrintf( const char * format, ... )
{
  char buffer[256];
  va_list args;
  va_start (args, format);
  vsprintf (buffer,format, args);
  USBHelper::sendToVcom (0,(uint8_t*)buffer,(uint32_t)strlen(buffer));
  va_end (args);
}


USBHelper *USBHelper::instance = nullptr;
uint32_t USBHelper::blink_interval_ms = BLINK_NOT_MOUNTED;

hid_keyboard_report_t USBHelper::keyboardData;
hid_mouse_report_t USBHelper::mouseData;
hid_gamepad_report_t USBHelper::gamepadData;    
uint16_t USBHelper::multimediKey;

int USBHelper::sendKeyBoardData = 0;
int USBHelper::sendMouseData = 0;    
int USBHelper::sendGamePadData = 0;
int USBHelper::sendMultimediaData = 0;

int USBHelper::sendNextData = 1;

USBHelper::USBHelper()
{
    tusb_init();
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

void USBHelper::send_hid_report(uint8_t report_id)
{
  // skip if hid is not ready yet
  if ( !tud_hid_ready() ) return;

  switch(report_id)
  {
    case REPORT_ID_KEYBOARD:
    {
      // use to avoid send multiple consecutive zero report for keyboard
      // static bool has_keyboard_key = false;

      // if ( btn )
      {
        mPrintf(">>>>Send keyboard\n");
        // uint8_t keycode[6] = { 0x41 };
        // keycode[0] = HID_KEY_A;

        tud_hid_keyboard_report(REPORT_ID_KEYBOARD, keyboardData.modifier, keyboardData.keycode);
        // has_keyboard_key = true;
      }
      // else
      // {
      //   // send empty key report if previously has key pressed
      //   if (has_keyboard_key) tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
      //   has_keyboard_key = false;
      // }
    }
    break;

    case REPORT_ID_MOUSE:
    {
      mPrintf(">>>>Send mouse\n");
      // int8_t const delta = 5;

      // no button, right + down, no scroll, no pan
      tud_hid_mouse_report(REPORT_ID_MOUSE, mouseData.buttons, mouseData.x, mouseData.y, 0, 0);
    }
    break;

    case REPORT_ID_CONSUMER_CONTROL:
    {
      mPrintf(">>>>Send media controll\n");
      // use to avoid send multiple consecutive zero report
      // static bool has_consumer_key = false;

      // if ( btn )
      {
        // volume down
        // uint16_t volume_down = HID_USAGE_CONSUMER_VOLUME_DECREMENT;
        tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &multimediKey, 2);
        // has_consumer_key = true;
      }
      // else
      // {
      //   // send empty key report (release key) if previously has key pressed
      //   uint16_t empty_key = 0;
      //   if (has_consumer_key) tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &empty_key, 2);
      //   has_consumer_key = false;
      // }
    }
    break;

    case REPORT_ID_GAMEPAD:
    {
      mPrintf(">>>>Send gamepad\n");
      // use to avoid send multiple consecutive zero report for keyboard
      // static bool has_gamepad_key = false;

      // hid_gamepad_report_t report =
      // {
      //   .x   = 0, .y = 0, .z = 0, .rz = 0, .rx = 0, .ry = 0,
      //   .hat = 0, .buttons = 0
      // };

      // if ( btn )
      {
        // report.hat = GAMEPAD_HAT_UP;
        // report.buttons = GAMEPAD_BUTTON_A;
        tud_hid_report(REPORT_ID_GAMEPAD, &gamepadData, sizeof(gamepadData));

        // has_gamepad_key = true;
      }
      // else
      // {
      //   report.hat = GAMEPAD_HAT_CENTERED;
      //   report.buttons = 0;
      //   if (has_gamepad_key) tud_hid_report(REPORT_ID_GAMEPAD, &report, sizeof(report));
      //   has_gamepad_key = false;
      // }
    }
    break;

    default: break;
  }
}
void USBHelper::usbMainTask( void * pvParameters )
{
    /* The parameter value is expected to be 1 as 1 is passed in the
    pvParameters value in the call to xTaskCreate() below. 
    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );
    */
    for( ;; )
    {
      USBHelper::usbLoop();
    }
}

//read serial port
uint32_t USBHelper::getVcomData(uint8_t *buff, uint16_t count,uint8_t itf)
{
  // for (itf = 0; itf < CFG_TUD_CDC; itf++)
  {
    // connected() check for DTR bit
    // Most but not all terminal client set this when making connection
    // if ( tud_cdc_n_connected(itf) )
    {
      uint32_t availableCnt = 0;
      if ( availableCnt = tud_cdc_n_available(itf) )
      {        
        uint32_t count = tud_cdc_n_read(itf, buff, availableCnt);
        return count;
      }
    }
  }
  return 0;
}

// echo to either Serial0 or Serial1
// with Serial0 as all lower case, Serial1 as all upper case
void USBHelper::sendToVcom(uint8_t itf, uint8_t *buf, uint32_t count)
{
  for(uint32_t i=0; i<count; i++)
  {
    tud_cdc_n_write_char(itf, buf[i]);
  }
  tud_cdc_n_write_flush(itf);
}

void USBHelper::sendStringToKeyBoard(const char* buffer,uint16_t len)
{
  uint8_t const conv_table[128][2] =  { HID_ASCII_TO_KEYCODE };
  uint8_t modifier = 0;
  if(len == 0)
    len = strlen(buffer);
  for(int i = 0 ; i < len; i++)
  {        
    modifier = 0;
    if ( conv_table[buffer[i]][0] ) modifier = KEYBOARD_MODIFIER_LEFTSHIFT;  
    sendKeyStroke(conv_table[buffer[i]][1],modifier);
    delay(20);
  }
}
// send key strokes with modifier
void USBHelper::sendKeyStroke(uint8_t keyCode, uint8_t modifier)
{
  memset(keyboardData.keycode,0x41,6);
  keyboardData.keycode[0] = keyCode;
  keyboardData.modifier = modifier;
//  sendKeyBoardData = 1;
  tud_hid_keyboard_report(REPORT_ID_KEYBOARD, keyboardData.modifier, keyboardData.keycode);
}

// send key strokes with modifier
void USBHelper::sendMouse(uint8_t x, uint8_t y,uint8_t buttons, uint8_t wheel, uint8_t pan)
{
  mouseData.x = x;
  mouseData.y = y;
  mouseData.buttons = buttons;  
  mouseData.wheel = wheel;
  mouseData.pan = pan;
  sendMouseData = 1;
}


// send key strokes with modifier
void USBHelper::sendMultimediaKey(uint16_t  keyCode)
{
  multimediKey = keyCode;
  sendMultimediaData = 1;
}


// send key strokes with modifier
void USBHelper::sendGamePad(uint8_t x, uint8_t y, uint8_t z, uint8_t rz,uint8_t rx,uint8_t ry,uint8_t hat, uint32_t buttons)
{
  gamepadData.x = x;
  gamepadData.y = y;
  gamepadData.z = z;
  gamepadData.rx =rx;
  gamepadData.ry =ry;
  gamepadData.rz =rz;
  gamepadData.hat = hat;
  gamepadData.buttons = buttons;
  sendGamePadData = 1;
}


// Every 10ms, we will sent 1 report for each HID profile (keyboard, mouse etc ..)
// tud_hid_report_complete_cb() is used to send the next report after previous one is complete
void USBHelper::usbLoop()
{
  while(1)
  {
    tud_task ();
    // Poll every 10ms
    const uint32_t interval_ms = 10;
    static uint32_t start_ms = 0;

    if ( board_millis() - start_ms < interval_ms) return; // not enough time
    start_ms += interval_ms;

    // Remote wakeup
    if (tud_suspended())
    {
      // Wake up host if we are in suspend mode
      // and REMOTE_WAKEUP feature is enabled by host
      tud_remote_wakeup();
    }
    else
    {    
      // Send the 1st of report chain, the rest will be sent by tud_hid_report_complete_cb()
      // send_hid_report(REPORT_ID_KEYBOARD, btn);
      if(sendKeyBoardData == 2) //release key here
      {
        taskENTER_CRITICAL();
        tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
        taskEXIT_CRITICAL();
        sendKeyBoardData = sendNextData = 0;
      }
      else if(sendGamePadData == 2)
      {
        gamepadData.hat = GAMEPAD_HAT_CENTERED;
        gamepadData.buttons = 0;
        taskENTER_CRITICAL();
        tud_hid_report(REPORT_ID_GAMEPAD, &gamepadData, sizeof(gamepadData));
        taskEXIT_CRITICAL();
        sendGamePadData = sendNextData = 0;
      }
      else if(sendMultimediaData == 2)
      {
        uint16_t empty_key = 0;
        taskENTER_CRITICAL();
        tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &empty_key, 2);
        taskEXIT_CRITICAL();
        sendMultimediaData = sendNextData = 0;
      }

      if(sendNextData == 1) // we are good to send
      {
        if(sendKeyBoardData == 1)
        {
          taskENTER_CRITICAL();
          tud_hid_keyboard_report(REPORT_ID_KEYBOARD, keyboardData.modifier, keyboardData.keycode);
          taskEXIT_CRITICAL();
          sendNextData = 0;
          sendKeyBoardData ++;
        }
        else if(sendMouseData == 1)
        {
          taskENTER_CRITICAL();
          tud_hid_mouse_report(REPORT_ID_MOUSE, mouseData.buttons, mouseData.x, mouseData.y, 0, 0);
          taskEXIT_CRITICAL();
          sendMouseData = sendNextData = 0;
        }
        else if(sendGamePadData == 1)
        {
          taskENTER_CRITICAL();
          tud_hid_report(REPORT_ID_GAMEPAD, &gamepadData, sizeof(gamepadData));
          taskEXIT_CRITICAL();
          sendGamePadData++;
          sendNextData = 0;
        }
        else if(sendMultimediaData == 1)
        {
          taskENTER_CRITICAL();
          tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &multimediKey, 2);
          taskEXIT_CRITICAL();
          sendMultimediaData++;
          sendNextData = 0;
        }
      }
      } //if(sendNextData == 1)        
  }
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint8_t len)
{
  (void) instance;
  (void) len;
  USBHelper::sendNextData = 1;  
  // uint8_t next_report_id = report[0] + 1;

  // if (next_report_id < REPORT_ID_COUNT)
  // {
  //   USBHelper::send_hid_report(next_report_id, board_button_read());
  // }
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  // TODO not Implemented
  (void) instance;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;

  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  (void) instance;

  if (report_type == HID_REPORT_TYPE_OUTPUT)
  {
    // Set keyboard LED e.g Capslock, Numlock etc...
    if (report_id == REPORT_ID_KEYBOARD)
    {
      // bufsize should be (at least) 1
      if ( bufsize < 1 ) return;

      uint8_t const kbd_leds = buffer[0];

      if (kbd_leds & KEYBOARD_LED_CAPSLOCK)
      {
        // Capslock On: disable blink, turn led on
        USBHelper::blink_interval_ms = 0;
        board_led_write(true);
      }else
      {
        // Caplocks Off: back to normal blink
        board_led_write(false);
        USBHelper::blink_interval_ms = BLINK_MOUNTED;
      }
    }
  }
}
