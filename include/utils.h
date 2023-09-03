/*
 * File: utils.h
 * Project: picoKey
 * File Created: Thursday, 20th January 2022 11:48:14 pm
 * Author: Dhananjay Khairnar (khairnardm@gmail.com)
 * -----
 * Last Modified: Thursday, 10th February 2022 12:02:56 am
 * Modified By: Dhananjay-Khairnar (khairnardm@gmail.com>)
 * -----
 * Copyright 2021 - 2022 https://github.com/8-DK
 */

#ifndef __UTILS_H_
#define __UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <pico/time.h>
#include <FreeRTOS.h>
#include <task.h>
#include <math.h>

#ifdef true
#undef true
#endif
#define true ((bool)1)

#ifdef false
#undef false
#endif
#define false ((bool)0)

#define toMs(ms) (ms / portTICK_PERIOD_MS)
#define delay(ms) vTaskDelay(ms / portTICK_PERIOD_MS);
#define getRand(lower,upper) ((rand() % (upper - lower + 1)) + lower)
//#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

class UtilsClass
{

    static UtilsClass *instance;
public: 
    static auto *getInstance() {
        if (!instance)
            instance = new UtilsClass;
        return instance;
    }
	uint8_t constrain(uint8_t amt,uint8_t low,uint8_t high)
	{
		if(amt < low)
		{
			return low;
		}
		else if(amt == low)
		{
			return low;
		}
		else if(amt == high)
		{
			return high;
		}
		else if(amt > high)
		{
			return high;
		}
		return amt;
	}

	float constraind(double amt,double low,double high){
		return ((amt)<(low)?(low):((amt)>(high)?(high):(amt)));
	}

	float constrainf(float amt,float low,float high){
		return ((amt)<(low)?(low):((amt)>(high)?(high):(amt)));
	}

	int isOutOfrange(int inValue, int setPoint,int plusOrMinus);
	float map(float x, float in_min, float in_max, float out_min, float out_max) ;
	uint8_t mapU8(uint8_t x, uint8_t in_min, uint8_t in_max, uint8_t out_min, uint8_t out_max);
	int mapInt(int x, int in_min, int in_max, int out_min, int out_max);
	int ascciToDecimal(char a);
	void getRandoms(uint8_t* arr,int lower, int upper,int count);
	uint32_t millis();
	uint32_t micros();
};

extern UtilsClass Utils;

#endif