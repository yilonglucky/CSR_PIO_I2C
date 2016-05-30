#define DELAY	10

/* pin definition */
#define SDA_PIN (1 << 12)
#define SCL_PIN (1 << 14)
#define RST_PIN (0x10000)

/* SDA input */
#define SET_SDA_IN() do \
{ \
  PioSetDir32(SDA_PIN, 0); \
  PioSet32(SDA_PIN, SDA_PIN); \
  PioSetStrongBias32(SDA_PIN, 0); \
}while(0)
#define GET_SDA() (PioGet32() & SDA_PIN) 
/* SDA output */
#define SET_SDA_OUT()   PioSetDir32(SDA_PIN, SDA_PIN)
#define SDA_HIGH()  PioSet32(SDA_PIN, SDA_PIN)
#define SDA_LOW()   PioSet32(SDA_PIN, 0)


/* SCL input */
#define SET_SCL_IN() do \
{ \
  PioSetDir32(SCL_PIN, 0); \
  PioSet32(SCL_PIN, SCL_PIN); \
  PioSetStrongBias32(SCL_PIN, 0); \
}while(0)
/* SCL output */
#define SET_SCL_OUT()   PioSetDir32(SCL_PIN, SCL_PIN)
#define SCL_HIGH()  PioSet32(SCL_PIN, SCL_PIN)
#define SCL_LOW()   PioSet32(SCL_PIN, 0)

/* RST output */
#define SET_RST_OUT()   PioSetDir32(RST_PIN, RST_PIN)
#define RST_HIGH()  PioSet32(RST_PIN, RST_PIN)
#define RST_LOW()   PioSet32(RST_PIN, 0)

/* function procotype */
void mysleep(int sec);
void msdelay(int ms);
void udelay(int us);
void i2c_start(void);
void i2c_stop(void);
void i2c_send_ack(uint8 ack);
uint8 i2c_receive_ack(void);
uint8 i2c_send_byte(uint8 send_byte);
void i2c_read_byte(uint8 *buffer, uint8 ack);

/*!
     @brief Allows transfer of data across the Bwave PIO simulated i2c interface.
     @param address The target address of the slave for this transfer.
     @param tx      A pointer to the data we wish to transmit. One octet per word.
     @param tx_len  The length of the data we wish to transmit.
     @param rx      A pointer to the memory we wish to read received data into. One octet per word.
     @param rx_len  The length of the data we wish to receive.
     @return The number of bytes acknowledged including the address bytes and the final data byte (which isn't strictly acknowledged).

     Perform a composite transfer consisting of a sequence of writes followed by a sequence of reads. These will be separated by a repeated start condition (Sr) and slave address if both reads and writes are performed.

     The general sequence of operations performed is:
     - Start condition (S).
     - Write slave address and direction byte (address | 0).
     - Write \e tx_len data bytes from the buffer at \e tx.
     - Repeated start condition (Sr) if \e tx_len and \e rx_len are non-zero.
     - Write slave address and direction byte (address | 1) if \e tx_len and \e rx_len are non-zero.
     - Read \e rx_len data bytes into the buffer at \e rx, acknowledging all but the final byte.
     - Stop condition (P).

     If \e tx_len is non-zero and \e rx_len is zero then the sequence reduces to:
     - Start condition (S).
     - Write slave address and direction byte (address | 0).
     - Write \e tx_len data bytes from the buffer at \e tx.
     - Stop condition (P).

     Alternatively, if \e tx_len is zero and \e rx_len is non-zero then the sequence reduces to:
     - Start condition (S).
     - Write slave address and direction byte (address | 1).
     - Read \e rx_len data bytes, acknowledging all but the final byte.
     - Stop condition (P).

     Finally, if both \e tx_len and \e rx_len are zero then the following minimal sequence is used:
     - Start condition (S).
     - Write slave address and direction byte (address | 1).
     - Stop condition (P).

     The transfer will be aborted if either the slave address or a byte being written is not acknowledged. The stop condition (P) will still be driven to reset the bus.

     Note also that the address is not shifted before being combined with the direction bit, i.e. the slave address should occupy bits 7 to 1 of address. This allows the R/W bit to be foerred to 1 for composite write/read transfers without a repeated start condition (Sr).

     If either \e tx_len or \e rx_len exceeds 64, failure may be returned or performance may be impaired; for instance, audio streaming may be disrupted.
*/
#if 1
uint16 I2cTransfer(uint16 address, const uint8 *tx, uint16 tx_len, uint8 *rx, uint16 rx_len);
#else
uint16 BwaveI2cTransfer(uint16 address, const uint8 *tx, uint16 tx_len, uint8 *rx, uint16 rx_len);
#endif
