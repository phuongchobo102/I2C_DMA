/*
 * Copyright 2015 Naver Corp.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __CRC16_IBM_H
#define __CRC16_IBM_H

#include "stdint.h"
//#include "stm32f7xx_hal.h"
#include <stdio.h>
//#include "b64.h"
#include "aes.h"
#include "math.h"


uint16_t EnCryptAes128(uint8_t *data_in, uint8_t *data_out, uint16_t length, uint8_t *pass);
uint16_t DeCryptAes128(uint8_t *data_in, uint8_t *data_out, uint16_t length, uint8_t *pass);
unsigned short crc16 (const char *buf, int len, unsigned short sd);
static uint16_t ComputeChecksum(uint8_t *bytes,uint16_t len);
uint16_t CreateLoginMsg(uint8_t *input,uint8_t *output, uint8_t *pass);
uint16_t CreateHeartBitMsg(uint8_t *input, uint8_t *output, uint8_t *key);
uint16_t ReplyMsg(uint8_t *input, uint8_t *output, uint8_t *key);
#endif
