#include "crc16_ibm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "b64.h"

int crc16_table[256];

uint16_t EnCryptAes128(uint8_t *data_in, uint8_t *data_out, uint16_t length, uint8_t *pass)
{
  unsigned int n;
  uint16_t i;
  uint8_t buf[400];
  uint8_t count=0;
  unsigned char aeskey[17];
  memset(aeskey, 0, 17);
  memcpy(buf,data_in,length+1);
  memcpy((char *)aeskey, (const char *)pass, 16);
  if ((length%16)==0) 
  {
      count = 16;
      for(i = 0;i<16;i++)
          buf[length + i] = 1;
  }
  else  
  {
    n = length%16;
    count = 16-n;
    for(i= 0;i < count;i++)
    {			
        buf[length + i] = count;
//	        D_printf(huart1,">>>: %d \r\n",count);
    }
  } 
//	for(i = 0; i < (length +count);i++)
//	D_printf(huart1,"%02x ",buf[i]);
//		D_printf(huart1,"length:  %d \r\n",length);
  for( i=0; i< (length + 2) ;i +=16)
  {
      AES_ECB_encrypt(&buf[i], aeskey, &data_out[i],16); 
//		D_printf(huart1,"Here :  %s,     %d,          %s\r\n",&buf[i], i, &data_out[i]);
//		HAL_Delay(2000);
  }
  data_out[length +count] ='\0';
//	D_printf(huart1,"i>>>:  %d \r\n",i);
      return i;
}

uint16_t DeCryptAes128(uint8_t *data_in, uint8_t *data_out, uint16_t length, uint8_t *pass)
{
    uint8_t buf[256];
    uint8_t key[16];
    uint8_t i;
    memcpy((uint8_t *)key,pass,16);
    memcpy(buf,data_in,length);
    for(i = 0;i < length; i += 16)
    {
        AES_ECB_decrypt(&buf[i], key, &data_out[i], length);
    }
    for(int j = 0; j < length;j++)
        if (data_out[j] < 16)  data_out[j]=0;
    return i;
}

unsigned short crc16 (const char *buf, int len, unsigned short sd)
{
  int counter;
  unsigned short crc = sd;
  for (counter = 0; counter < len; counter++)
    crc = (crc >> 8) ^ crc16_table[((crc ) ^ *buf++) & 0x00FF];
  return crc;
}


static uint16_t ComputeChecksum(uint8_t *data,uint16_t data_len)
{
	uint16_t crc = 0x0000; 
  size_t j; 
  int i; 
  // Note: 0xA001 is the reflection of 0x8005 
  for (j=data_len; j>0; j--) { 
    crc ^= *data++; 
    for (i=0; i<8; i++) { 
      if (crc & 1) crc = (crc >> 1) ^ 0xA001; 
      else crc >>= 1; 
    } 
  } 
  return (crc); 
}

//uint8_t check_crc16( uint8_t *data, size_t lenght)
//{
//	uint16_t crc_check;
//	
//}
uint16_t CreateLoginMsg(uint8_t *input, uint8_t *output, uint8_t *pass)
{
    uint16_t len,crc;
    char buf[256];
    char *pt;
    uint8_t key[16];
    memcpy((uint8_t *)key,pass,16);
    memset((uint8_t *)output,0,sizeof(output));
    len = EnCryptAes128(input,output, strlen((const char *)input), key);
    pt = b64_encode(output, len);
    crc = ComputeChecksum((uint8_t *)pt, strlen(pt));
//	for(i = 0; i < (length +count +1 );i++)
//	D_printf(huart1,"len:  %d \r\n",len);
    output[0] = '{';
    output[1] = 0;
    strcat((char *)output,pt);
    sprintf(buf,"%05d}",crc);
    strcat((char *)output,buf);
    free(pt);
    return strlen((char *)output);
}

uint16_t CreateHeartBitMsg(uint8_t *input, uint8_t *output, uint8_t *key)
{
    uint16_t len,crc;
    char buf[256];
    char *pt;
    uint8_t devicekey[17];
    memset(devicekey, 0 ,17);
    memset((void*)output,0,sizeof(output));
    memcpy((uint8_t *)devicekey,key,16);
    len = EnCryptAes128(input,output, strlen((const char *)input), devicekey);
    pt = b64_encode(output,len);
    crc = ComputeChecksum((uint8_t *)pt,strlen(pt));
//	D_printf(huart1,"len:  %d \r\n",len);
    output[0] = '[';
    output[1] = 0;
    strcat((char *)output,pt);
    sprintf(buf,"%05d]",crc);
    strcat((char *)output,buf);
    free(pt);
    return strlen((char *)output);
}


uint16_t ReplyMsg(uint8_t *input, uint8_t *output, uint8_t *key)
{
    uint16_t len,crc;
    char buf[256], crc_check[5], crc_msg[5];
    char *pt, *pt2;
    uint8_t devicekey[17];
    memset(devicekey, 0 ,17);
    memset(crc_msg, 0 ,5);
    memcpy((uint8_t *)devicekey,key,16);
    if ((((pt = strchr((const char *)input, '[')) != NULL) ||((pt = strchr((const char *)input, '{')) != NULL)) &&
        (((pt2 = strchr((const char *)input, ']')) != NULL)||((pt2 = strchr((const char *)input, '}')) != NULL))) 
    {
        memcpy((void *)buf,pt+1, pt2 - pt -1-5);
        memcpy((void *)crc_msg, pt2 -5,5);
    }
    else return 0;
    crc = ComputeChecksum((uint8_t *)buf, strlen((const char *)buf));
    sprintf(crc_check,"%05d",crc);
    if ((strcmp(crc_msg, crc_check)) == 0) 
    {
        pt = b64_decode((const char *)buf, strlen((const char *)buf));
        len = DeCryptAes128((uint8_t *)pt,output, strlen((const char *)pt), devicekey);
//		HAL_Delay(2000);
    }
    return strlen((char *)output);
}

