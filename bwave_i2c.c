/*
  2.0C is ok
  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pio.h>
#include <print.h>
#include "bwave_i2c.h"



#if 1
uint16 I2cTransfer(uint16 address, const uint8 *tx, uint16 tx_len, uint8 *rx, uint16 rx_len)
#else
uint16 BwaveI2cTransfer(uint16 address, const uint8 *tx, uint16 tx_len, uint8 *rx, uint16 rx_len)
#endif
{
    uint16 ack_num = 0;
    uint16 i;

    uint8 dev_address = (uint8)(address); /* 7-bit address supported */

#ifdef DEBUG_IAP_CP_BWAVE
printf("wangyilong address=%x txlen=%d rxlen=%d\n", address, tx_len, rx_len);
#endif    
    
    i2c_start();
    
    if( 0 != tx_len)
    {
        if(0 == i2c_send_byte(dev_address & 0xfe))
        {
            ack_num ++;
        }
        for(i = 0; i < tx_len; i++)
        {
            if(0 == i2c_send_byte(tx[i]))
            {
                ack_num ++;
            }
        }
    }

    if( (0 != tx_len) && (0 != rx_len))
    {
        i2c_stop();
        i2c_start();
    }
    
    if( 0 != rx_len)
    {
        if(0 == i2c_send_byte(dev_address | 0x01))
        {
            ack_num ++;
        }
        for(i=0; i < rx_len; i++)
        {
            /*  !(len-i-1) means: If read N bytes, Byte1 ACK, Byte2 ACK, ..., ByteN NACK */
            i2c_read_byte((uint8*)&(rx[i]), !(rx_len-i-1));
        }
    }
    
    if( 0 == tx_len && 0 == rx_len)
    {
        if(0 == i2c_send_byte(dev_address))
        {
            ack_num ++;
        }
    }
    i2c_stop();
    
    return ack_num;
}
/******************************************************************************/
#if 0
int main(void)
{

    uint8 data[128]={0xff, 0x02, 0x04, 0xff};
    uint8 retry = 0;


    SET_SDA_OUT();    
    SET_SCL_OUT();

    SDA_HIGH();    
    SCL_HIGH();

    msdelay(40);

    SET_RST_OUT();
    RST_LOW();
    msdelay(40);
 #if 0
    RST_HIGH();
    msdelay(40);

#endif    
  
    while(1)
    {

#if 1
        /* case 1 */
        for(retry = 3; retry > 0; retry --)
        {
            if(BwaveI2cTransfer(0x20, NULL, 0, NULL, 0))
            {
                /* check whether salve alive */
                PRINT(("0x20 alive\n"));
                break;
            }
            else
            {
                PRINT(("\t\t\t0x20 dead\n"));
            }
        }
        data[0] = data[0];/* let gcc happy */
        mysleep(1);
        PRINT(("#########\n"));
#endif
#if 1
        /* case 2 write only */
        for(retry = 3; retry > 0; retry --)
        {
            memset((void*)data, 0x55, sizeof(data)/sizeof(data[0]));
            data[0] = 0x20; /* register address */
            if( 4 == BwaveI2cTransfer(0x20, (uint8 *)&data[0], 3, NULL, 0))
            {
                PRINT(("write bytes OK\n"));
                break;
            }
            else
            {
                PRINT(("\t\t\twrite bytes fail\n"));
            }
        }   
        
        mysleep(1);
        PRINT(("#########\n"));

#endif
#if 1
        /* case 3 write read */
        for(retry = 3; retry > 0; retry --)
        {
            memset((void*)data, 0xff, sizeof(data)/sizeof(data[0]));
            data[0] = 0x04; /* register address */
            if( 2 == BwaveI2cTransfer(0x20, (uint8 *)&data[0], 1, NULL, 0))
            {
                PRINT(("write OK\n"));
                break;
            }
            else
            {
                PRINT(("\t\t\twrite refore read fail\n"));
            }
        }
        for(retry = 3; retry > 0; retry --)
        {
            if(1 == BwaveI2cTransfer(0x20, NULL, 0, (uint8 *)&data[1], 4))
            {
                PRINT(("data: %x %x %x %x\n", data[1], data[2], data[3], data[4]));
                break;
            }
            else
            {
                PRINT(("\t\t\tread error""\n"));
            }
        }
        mysleep(1);
        PRINT(("#########\n"));
    #endif     
 
    }
}
#endif
/******************************************************************************/
/* START */
void i2c_start(void)
{	  
	SDA_HIGH();
	SCL_HIGH();
	udelay(DELAY);
    
	SDA_LOW();
    
	udelay(DELAY);
    
	SCL_LOW();
	udelay(DELAY);
}
/* STOP */
void i2c_stop(void)
{ 
	SCL_LOW();
	SDA_LOW();
    
	udelay(DELAY);
    
	SCL_HIGH();
	udelay(DELAY);
    
	SDA_HIGH();
	udelay(DELAY);
}

/* when ack 0 Send ack bit
   when ack 1 Don't send ack bit */
void i2c_send_ack(uint8 ack)
{
    SCL_LOW();
	if(ack)
		SDA_HIGH(); 
	else 
		SDA_LOW();
	udelay(DELAY);
	
	SCL_HIGH();
   	udelay(DELAY);
   	
	SCL_LOW();
	udelay(DELAY);	
}
/* try to get ack bit */
uint8 i2c_receive_ack(void)
{
	uint8 rc = 0;	

    SET_SDA_IN();
    udelay(DELAY);
	SCL_HIGH();
	udelay(DELAY);
    
	if(0 != GET_SDA())
    {
        /* nobody ack */
		rc = 1;
	}

    SCL_LOW();
    udelay(DELAY);
    
    SET_SDA_OUT();
	/*SDA_HIGH();*/
	return rc;
}
uint8 i2c_send_byte(uint8 send_byte)
{
	uint8 rc = 0;
	uint8 out_mask = 0x80;
	uint8 value;
	uint8 count = 8;
	
    for(count = 8; count > 0; count--, out_mask >>= 1)
    {
		value = ((send_byte & out_mask) ? 1 : 0);   
		if (1 == value)
		{    					      		
			SDA_HIGH();     
		}    
		else
		{      					     	
			SDA_LOW();
		}    
		udelay(DELAY);
		                          
		SCL_HIGH();   				    
		udelay(DELAY);
		             
		SCL_LOW();     
		udelay(DELAY);
	}
	
	/* SDA_HIGH();*/
	rc = i2c_receive_ack();
	return rc;
}
void i2c_read_byte(uint8 *buffer, uint8 ack)
{
	uint8 count = 0x08;
	uint8 data = 0x00;
	uint8 temp = 0;
	
    SET_SDA_IN();
	for(count = 8; count > 0; count--)
	{
		SCL_HIGH();
		udelay(DELAY);
		temp = GET_SDA();    	
		data <<= 1;
		if (temp)
		{
			data |= 0x01;
		}
		SCL_LOW();
		udelay(DELAY);
    }
    SET_SDA_OUT();
    /*SDA_HIGH();*/
	i2c_send_ack(ack);/*0 = ACK    1 = NACK */
	*buffer = data;
}
/******************************************************************************/
void mysleep(int sec)
{
    int i;
    uint32 a;
    for(i=0; i< sec; i++)
        for(a=0; a < 0xBC3C3B; a++)
            ;
    return; 
}

void msdelay(int ms)
{
    int i;
    uint32 a;
    for(i=0; i< ms; i++)
        for(a=0; a < 7273; a++)
            ;
    return; 
}

void udelay(int us)
{
    uint32 a;
        for(a=0; a < 100; a++)
            ;
    return; 
}





