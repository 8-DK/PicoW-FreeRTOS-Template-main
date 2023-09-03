/*
 * File: EEPROM.h
 * Project: picoKey
 * File Created: Saturday, 15th January 2022 12:47:56 am
 * Author: Dhananjay Khairnar (khairnardm@gmail.com)
 * -----
 * Last Modified: Thursday, 10th February 2022 12:02:05 am
 * Modified By: Dhananjay-Khairnar (khairnardm@gmail.com>)
 * -----
 * Copyright 2021 - 2022 https://github.com/8-DK
 */

#ifndef __EEPROM__
#define __EEPROM__

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <hardware/sync.h>
#include <stack>
#include <map>
// Support nested IRQ disable/re-enable
static std::stack<uint32_t> _irqStack[2];

class EEPROMClass {
public:
    EEPROMClass(void);

    void begin(size_t size);
    uint8_t read(int const address);
    void write(int const address, uint8_t const val);
    bool commit();
    bool end();

    uint8_t * getDataPtr();
    uint8_t const * getConstDataPtr() const;

    template<typename T>
    T &get(int const address, T &t) {
        if (address < 0 || address + sizeof(T) > _size) {
            return t;
        }

        memcpy((uint8_t*) &t, _data + address, sizeof(T));
        return t;
    }

    template<typename T>
    const T &put(int const address, const T &t) {
        if (address < 0 || address + sizeof(T) > _size) {
            return t;
        }
        if (memcmp(_data + address, (const uint8_t*)&t, sizeof(T)) != 0) {
            _dirty = true;
            memcpy(_data + address, (const uint8_t*)&t, sizeof(T));
        }

        return t;
    }

    size_t length() {
        return _size;
    }

    uint8_t& operator[](int const address) {
        return getDataPtr()[address];
    }
    uint8_t const & operator[](int const address) const {
        return getConstDataPtr()[address];
    }

protected:
    uint8_t* _sector;
    uint8_t* _data = nullptr;
    size_t _size = 0;
    bool _dirty = false;
};

extern EEPROMClass EEPROM;

#endif
