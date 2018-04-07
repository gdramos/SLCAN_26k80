/* 
 * File:                CAN.h
 * Author:              Gerson Ramos
 * Comments:            CAN stuff
 * Revision history:    Stop asking for this
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef CAN_H
#define	CAN_H

#include <xc.h> 
#include <stdint.h>
#include <stdbool.h>

#define         STD         0
#define         EXT         1

/* CANCON */
#define     CANCON_REQOP_CONFIG_MODE            0b10000000
#define     CANCON_REQOP_LISTEN_MODE            0b01100000
#define     CANCON_REQOP_LPBACK_MODE            0b01000000
#define     CANCON_REQOP_SLEEP_MODE             0b00100000
#define     CANCON_REQOP_NORMAL_MODE            0b00000000

#define     CANCON_ABAT                         (uint8_t)(1 << 4)

#define     CANCON_WIN_2                        (uint8_t)(1 << 3)
#define     CANCON_WIN_1                        (uint8_t)(1 << 2)
#define     CANCON_WIN_0                        (uint8_t)(1 << 1)

#define     CANCON_FP_3                         (uint8_t)(1 << 3)
#define     CANCON_FP_2                         (uint8_t)(1 << 2)
#define     CANCON_FP_1                         (uint8_t)(1 << 1)
#define     CANCON_FP_0                         (uint8_t)(1 << 0)

/* CANSTAT */
#define     CANSTAT_OPMODE_CONFIG_MODE          0b10000000
#define     CANSTAT_OPMODE_LISTEN_MODE          0b01100000
#define     CANSTAT_OPMODE_LPBACK_MODE          0b01000000
#define     CANSTAT_OPMODE_SLEEP_MODE           0b00100000
#define     CANSTAT_OPMODE_NORMAL_MODE          0b00000000

/* ECANCON */
#define     ECANCON_MDSEL_MODE_0                (uint8_t)(0)
#define     ECANCON_MDSEL_MODE_1                (uint8_t)(1 << 6)
#define     ECANCON_MDSEL_MODE_2                (uint8_t)(1 << 7)  


/* BRGCON1 */
#define     BRGCON1_1000kbps                    0x40
#define     BRGCON1_500kbps                     0x41
#define     BRGCON1_250kbps                     0x43
#define     BRGCON1_125kbps                     0x47

/* BRGCON2 */
#define     BRGCON2_1000kbps                    0x90
#define     BRGCON2_500kbps                     0x90
#define     BRGCON2_250kbps                     0x90
#define     BRGCON2_125kbps                     0x90

/* BRGCON3 */
#define     BRGCON3_1000kbps                    0x82
#define     BRGCON3_500kbps                     0x82
#define     BRGCON3_250kbps                     0x82
#define     BRGCON3_125kbps                     0x82

/* PIR5 */
#define     PIR5_IRXIF                          (uint8_t)(1 << 7)
#define     PIR5_WAKIF                          (uint8_t)(1 << 6)
#define     PIR5_ERRIF                          (uint8_t)(1 << 5)
#define     PIR5_TXB2IF                         (uint8_t)(1 << 4)
#define     PIR5_TXB1IF                         (uint8_t)(1 << 3)
#define     PIR5_TXB0IF                         (uint8_t)(1 << 2)
#define     PIR5_RXB1IF                         (uint8_t)(1 << 1)
#define     PIR5_RXB0IF                         (uint8_t)(1 << 0)

/* RXBnSIDL */
#define     RXBnSIDL_SRR                        (uint8_t)(1 << 4)
#define     RXBnSIDL_EXID                       (uint8_t)(1 << 3)

/* RXBnDLC */
#define     RXBnDLC_RXRTR                       (uint8_t)(1 << 6)

/* RXBnCON */
#define     RXBnCON_RXFUL                       (uint8_t)(1 << 7)

/* TXBnCON */
#define     TXBnCON_TXBIF                       (uint8_t)(1 << 7)
#define     TXBnCON_TXABT                       (uint8_t)(1 << 6)
#define     TXBnCON_TXLARB                      (uint8_t)(1 << 5)
#define     TXBnCON_TXERR                       (uint8_t)(1 << 4)
#define     TXBnCON_TXREQ                       (uint8_t)(1 << 3)

/* TXBnSIDL */
#define     TXBnSIDL_EXIDE                      (uint8_t)(1 << 3)

/* TXBnDLC */
#define     TXBnDLC_TXRTR                       (uint8_t)(1 << 6)

/* RXB0CON Mode 0 */
#define     RXB0CON_RXFUL                       (uint8_t)(1 << 8)
#define     RXB0CON_RXM1                        (uint8_t)(1 << 7)
#define     RXB0CON_RXM0                        (uint8_t)(1 << 6)
#define     RXB0CON_RXRTRRO                     (uint8_t)(1 << 3)
#define     RXB0CON_RXB0DBEN                    (uint8_t)(1 << 2)
#define     RXB0CON_JTOFF                       (uint8_t)(1 << 1)
#define     RXB0CON_FILHIT0                     (uint8_t)(1 << 0)


/* RXB1CON Mode 0 */
#define     RXB1CON_RXFUL                       (uint8_t)(1 << 8)
#define     RXB1CON_RXM1                        (uint8_t)(1 << 7)
#define     RXB1CON_RXM0                        (uint8_t)(1 << 6)
#define     RXB1CON_RXRTRRO                     (uint8_t)(1 << 3)
#define     RXB1CON_FILHIT2                     (uint8_t)(1 << 2)
#define     RXB1CON_FILHIT1                     (uint8_t)(1 << 1)
#define     RXB1CON_FILHIT0                     (uint8_t)(1 << 0)

/* CAN Operation Mode */
typedef enum
{
    Configuration,
    Listen_Only,
    Loopback,
    Sleep,
    Normal
}CAN_OPMODE_t;

/* CAN Peripheral Mode */
typedef enum
{
    Legacy,
    Enhanced_Legacy,
    Enhanced_FIFO
}CAN_MODE_t;

/* CAN Baud Rate Type */
typedef enum
{
    Baud_10kbps,
    Baud_20kbps,
    Baud_50kbps,
    Baud_100kbps,
    Baud_125kbps,
    Baud_250kbps,
    Baud_500kbps,
    Baud_800kbps,
    Baud_1000kbps
}CAN_Baud_t;

/* CAN Configuration Type*/
typedef struct
{
    CAN_MODE_t      Mode;
    CAN_OPMODE_t    OpMode;
    CAN_Baud_t      Baudrate;
}CAN_Configuration_t;

/* CAN Message Type*/
typedef struct
{
    uint16_t    Timestamp;
    bool        IsExtended;
    bool        IsRTR;
    uint32_t    ID;
    uint8_t     DLC;
    uint8_t     Data[8];
}CAN_Message_t;

/* CAN Filter Type */
typedef struct 
{
    uint8_t     Number;
    bool        Extended;
    uint32_t    ID;
}CAN_Filter_t;

/* CAN Mask Type */
typedef struct 
{
    uint8_t     Number;
    bool        Extended;
    uint32_t    ID;
}CAN_Mask_t;


bool CAN_Receive(CAN_Message_t *message);

bool CAN_Transmit(CAN_Message_t *message);

bool CAN_Initialize(CAN_Configuration_t *configuration);

bool CAN_ChangeBaudrate(CAN_Baud_t baud);

bool CAN_SetBaudrateRegisters(CAN_Baud_t baud);

bool CAN_SetOpMode(CAN_OPMODE_t opmode);

bool CAN_SetCANMode(CAN_MODE_t mode);

bool CAN_IsTxBufferAvailable(void);

bool CAN_SetMask(CAN_Mask_t *mask);

bool CAN_SetFilter(CAN_Filter_t *filter);

bool CAN_ReadMask(CAN_Mask_t *mask);

bool CAN_ReadFilter(CAN_Filter_t *filter);

#endif	/* XC_HEADER_TEMPLATE_H */

