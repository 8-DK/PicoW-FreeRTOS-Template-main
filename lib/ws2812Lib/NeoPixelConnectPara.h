/*
 Copyright (c) 2020-2022 Alan Yorinks All rights reserved.

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU AFFERO GENERAL PUBLIC LICENSE
 Version 3 as published by the Free Software Foundation; either
 or (at your option) any later version.
 This library is distributed in the hope that it will be useful,f
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.

 You should have received a copy of the GNU AFFERO GENERAL PUBLIC LICENSE
 along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef NEOPIXEL_CONNECT_NEOPIXELCONNECTPARA_H
#define NEOPIXEL_CONNECT_NEOPIXELCONNECTPARA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

#include "pico/stdlib.h"
#include "pico/sem.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "ws2812.pio.h"

#define FRAC_BITS 4
#define NUM_PIXELS  1
#ifndef MAXIMUM_NUM_NEOPIXELS
#   define MAXIMUM_NUM_NEOPIXELS 1024
#endif

// Pixel buffer array offsets
#define RED 0
#define GREEN 1
#define BLUE 2

// bit plane content dma channel
#define DMA_CHANNEL 0
// chain channel for configuring main dma channel to output from disjoint 8 word fragments of memory
#define DMA_CB_CHANNEL 1

#define DMA_CHANNEL_MASK (1u << DMA_CHANNEL)
#define DMA_CB_CHANNEL_MASK (1u << DMA_CB_CHANNEL)
#define DMA_CHANNELS_MASK (DMA_CHANNEL_MASK | DMA_CB_CHANNEL_MASK)


#define VALUE_PLANE_COUNT (8 + FRAC_BITS)

// we store value (8 bits + fractional bits of a single color (R/G/B/W) value) for multiple
// strips of pixels, in bit planes. bit plane N has the Nth bit of each strip of pixels.
typedef struct {
    // stored MSB first
    uint32_t planes[VALUE_PLANE_COUNT];
} value_bits_t;


typedef struct {
    uint8_t *data;
    uint data_len;
    uint frac_brightness; // 256 = *1.0;
} strip_t;


class NeoPixelConnectPara
{
public:
    /// @brief Constructor
    /// @param pinNumber: GPIO pin that controls the NeoPixel string.
    /// @param numberOfPixels: Number of pixels in the string
    /// @param pio: pio selected - default = pio0. pio1 may be specified
    /// @param sm: state machine selected. Default = 0
    NeoPixelConnectPara(uint pinNumber);

    /// @brief Constructor
    /// @param pinNumber: GPIO pin that controls the NeoPixel string.
    /// @param numberOfPixels: Number of pixels in the string
    /// This constructor sets pio=pio0 and sm to 0
    NeoPixelConnectPara(uint pinNumber, PIO pio, uint sm);

    /// @brief Destructor
    virtual ~NeoPixelConnectPara(){};

    inline void put_pixel(uint32_t pixel_grb) ;
    inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b);
    void add_error(value_bits_t *d, const value_bits_t *s, const value_bits_t *e);
    void transform_strips(strip_t **strips, uint num_strips, value_bits_t *values, uint value_length,uint frac_brightness);
    void dither_values(const value_bits_t *colors, value_bits_t *state, const value_bits_t *old_state, uint value_length);
    void dma_init(PIO pio, uint sm);
    void output_strips_dma(value_bits_t *bits, uint value_length);
    int neoPixelInit(uint pinNumber) ;

        // start of each value fragment (+1 for NULL terminator)
    static uintptr_t fragment_start[NUM_PIXELS * 4 + 1];

    // posted when it is safe to output a new set of values
    static struct semaphore reset_delay_complete_sem;
    // alarm handle for handling delay
    static alarm_id_t reset_delay_alarm_id;

private:
    // pio - 0 or 1
    PIO pixelPio;

    // calculated program offset in memory
    uint pixelOffset;

    // pio state machine to use
    uint pixelSm;

    // number of pixels in the strip
    uint16_t actual_number_of_pixels;

    // a buffer that holds the color for each pixel
    uint8_t pixelBuffer[MAXIMUM_NUM_NEOPIXELS][3];

    // horrible temporary hack to avoid changing pattern code
    uint8_t *current_strip_out;
    bool current_strip_4color;

};


#endif //NEOPIXEL_CONNECT_NEOPIXELCONNECT_H
