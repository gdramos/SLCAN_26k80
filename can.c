#include "can.h"

bool CAN_Receive(CAN_Message_t *message)
{
    uint8_t *ptr;
    uint8_t *bufPtr;
    uint8_t flagToClear;
    
    /* determine which receive buffer */
    if(PIR5 & PIR5_RXB1IF)
    {
        bufPtr = &RXB1CON;
        ptr = &RXB1SIDH;
        flagToClear = PIR5_RXB1IF;
    }
    else if(PIR5 & PIR5_RXB0IF)
    {
        bufPtr = &RXB0CON;
        ptr = &RXB0SIDH;
        flagToClear = PIR5_RXB0IF;
    }
    else
    {
        return false;
    }
    
    /* check if Remote Request */
    if(*(ptr + 4) & RXBnDLC_RXRTR)
    {
        message->IsRTR = true;
    }
    else
    {
        message->IsRTR = false;
    }
    
    /* check if Extended Frame and get ID */
    if(*(ptr + 1) & RXBnSIDL_EXID)
    {
        message->IsExtended = true;
        message->ID = *ptr++;
        message->ID = (message->ID << 3) | ((*ptr >> 5) & 0x07);
        message->ID = (message->ID << 2) | (*ptr++ & 0x03); 
        message->ID = (message->ID << 8) | *ptr++;
        message->ID = (message->ID << 8) | *ptr++;
    }
    else
    {
        message->IsExtended = false;
        message->ID = *ptr++;
        message->ID = message->ID << 8;
        message->ID = message->ID | *ptr++;
        message->ID = message->ID >> 5;
        *ptr++;
        *ptr++;
    }
    
    /* Data Length Code */
    message->DLC = *ptr++ & 0x0F;
    
    /* message data */
    for(int i = 0; i < message->DLC; i++)
    {
        message->Data[i] = *ptr++;
    }
    
    /* clear flag */
    PIR5 &= ~flagToClear;
    
    /* release receive buffer */
    *bufPtr &= ~RXBnCON_RXFUL;
    
    return true;
}

bool CAN_IsTxBufferAvailable(void)
{
    if((TXB0CON & TXBnCON_TXREQ) != TXBnCON_TXREQ || 
       (TXB1CON & TXBnCON_TXREQ) != TXBnCON_TXREQ ||
       (TXB2CON & TXBnCON_TXREQ) != TXBnCON_TXREQ)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool CAN_Transmit(CAN_Message_t *message)
{
    if(CAN_IsTxBufferAvailable() == false)
        return false;
    
    uint8_t *bufPtr;
    uint8_t *ptr;
    
    /* determine which transmit buffer to use */
    if((TXB0CON & TXBnCON_TXREQ) != TXBnCON_TXREQ)
    {
        bufPtr = &TXB0CON;
        ptr = &TXB0SIDH;
    }
    else if((TXB1CON & TXBnCON_TXREQ) != TXBnCON_TXREQ)
    {
        bufPtr = &TXB1CON;
        ptr = &TXB1SIDH;
    }
    else if((TXB2CON & TXBnCON_TXREQ) != TXBnCON_TXREQ)
    {
        bufPtr = &TXB2CON;
        ptr = &TXB2SIDH;
    }
    else
    {
        return false;
    }
    
    /* load message ID */
    if(message->IsExtended)
    {
        *ptr++ = (message->ID>>21) & 0x00FF;
        *ptr = ((message->ID>>16) & 0x0003) | 0X08;
        *ptr = *ptr | ((message->ID >> 13) & 0xE0);
        *ptr |= TXBnSIDL_EXIDE;
        *ptr++;
        *ptr++ = (message->ID>>8) & 0x00FF;
        *ptr++ = message->ID & 0x00FF;
    }
    else
    {
        *ptr++ = (message->ID>>3) & 0x00FF;
        *ptr++ = ((message->ID<<5) & 0x00E0) & ~TXBnSIDL_EXIDE;
        *ptr++;
        *ptr++;
    }
    
    /* check if Remote Transmit Request */
    *ptr = message->DLC & 0x0F;
    
    if(message->IsRTR)
    {
        *ptr |= TXBnDLC_TXRTR;
    }
    *ptr++;
    
    /* load data */
    for(int i = 0; i < message->DLC; i++)
    {
        *ptr++ = message->Data[i];
    }
    
    /* request transmission */
    *bufPtr |= TXBnCON_TXREQ;
    
    return true;
}

bool CAN_SetOpMode(CAN_OPMODE_t opmode)
{
    uint8_t opModeCode;
    
    if(opmode != Configuration && (CANSTAT & 0xE0 != CANSTAT_OPMODE_CONFIG_MODE))
    {
        /* request configuration mode */
        CANCON = CANCON_REQOP_CONFIG_MODE;

        /* wait for module to enter configuration mode */
        while((CANSTAT & 0xE0) != CANCON_REQOP_CONFIG_MODE);
    }
    
    switch(opmode)
    {
        case Configuration:
        {
            opModeCode = CANCON_REQOP_CONFIG_MODE;
            break;
        }
        case Listen_Only:
        {
            opModeCode = CANCON_REQOP_LISTEN_MODE;
            break;
        }
        case Loopback:
        {
            opModeCode = CANCON_REQOP_LPBACK_MODE;
            break;
        }
        case Sleep:
        {
            opModeCode = CANCON_REQOP_SLEEP_MODE;
            break;
        }
        case Normal:
        {
            opModeCode = CANCON_REQOP_NORMAL_MODE;
            break;
        }
        default:
        {
            return false;
        }
    }
    
    /* request operation mode */
    CANCON = opModeCode;
    
    /* wait for module to enter operation mode */
    while((CANSTAT & 0xE0) != opModeCode);
    
    return true;
}

bool CAN_SetCANMode(CAN_MODE_t mode)
{
    switch(mode)
    {
        case Legacy:
        {
            ECANCON = ECANCON_MDSEL_MODE_0;
            break;
        }
        case Enhanced_Legacy:
        {
            ECANCON = ECANCON_MDSEL_MODE_1;
            break;
        }
        case Enhanced_FIFO:
        {
            ECANCON = ECANCON_MDSEL_MODE_2;
            break;
        }
        default:  
        {
            return false;
        }
    }
    return true;
}

bool CAN_SetBaudrateRegisters(CAN_Baud_t baud)
{
    switch(baud)      
    {
        case Baud_125kbps:
        {
            BRGCON1 = BRGCON1_125kbps;
            BRGCON2 = BRGCON2_125kbps;
            BRGCON3 = BRGCON3_125kbps;
            break;
        }
        case Baud_250kbps:  
        {
            BRGCON1 = BRGCON1_250kbps;
            BRGCON2 = BRGCON2_250kbps;
            BRGCON3 = BRGCON3_250kbps;
            break;
        }
        case Baud_500kbps:  
        {
            BRGCON1 = BRGCON1_500kbps;
            BRGCON2 = BRGCON2_500kbps;
            BRGCON3 = BRGCON3_500kbps;
            break;
        }
        case Baud_1000kbps:
        {
            BRGCON1 = BRGCON1_1000kbps;
            BRGCON2 = BRGCON2_1000kbps;
            BRGCON3 = BRGCON3_1000kbps;
            break;
        }
        default:
        {
            return false;
        }
    }
    return true;
}

bool CAN_Initialize(CAN_Configuration_t *configuration)
{   
    CAN_Mask_t mask;
    CAN_Filter_t filter;
    
    /* put module in configuration mode */
    if(CAN_SetOpMode(Configuration) == false)
    {
        return false;
    }
    
    /* set can mode */
    if(CAN_SetCANMode(configuration->Mode) == false)
    {
        return false;
    }
    
    /* set baud rate registers */
    CAN_SetBaudrateRegisters(configuration->Baudrate);
    
    /* reset critical register values */
    TXERRCNT = 0x00;                                /* transmit error count */
    RXERRCNT = 0x00;                                /* receive error count */
    
    RXB0CON = 0x00;                                 /* Filters enabled */
    RXB1CON = 0x00;                                 /* Filters enabled */
    
    /* set up default Masking */
    mask.Extended = true;
    mask.ID = (uint32_t)0;
    
    mask.Number = 0;
    CAN_SetMask(&mask);
    
    mask.Number = 1;
    CAN_SetMask(&mask);
    
    /* set up default Filtering */
    filter.ID = (uint32_t)0;
    
    filter.Number = 0;
    filter.Extended = true;
    CAN_SetFilter(&filter);
    
    filter.Number = 1;
    filter.Extended = false;
    CAN_SetFilter(&filter);
    
    filter.Number = 2;
    filter.Extended = true;
    CAN_SetFilter(&filter);
    
    filter.Number = 3;
    filter.Extended = true;
    CAN_SetFilter(&filter);
    
    filter.Number = 4;
    filter.Extended = false;
    CAN_SetFilter(&filter);
    
    filter.Number = 5;
    filter.Extended = false;
    CAN_SetFilter(&filter);
    
    /* select module clock */
    CIOCONbits.CLKSEL = 1;                          /* Not use PLL output as the clock for CAN */

    /* clear interrupt flags */
    PIR5 = 0x00;                                    /* {IRXIF, WAKIF, ERRIF, TXB2IF, TXB1IF, TXB0IF, RXB1IF, RXB0IF} */
    PIE5 = 0x00;                                    /* {IRXIE, WAKIE, ERRIE, TXB2IE, TXB1IE, TXB0IE, RXB1IE, RXB0IE} */

    /* disable transmit interrupts */
    TXBIE = 0x00;                                   /* transmit buffer interrupt enables */
    
    /* set requested operation mode */
    if(CAN_SetOpMode(configuration->OpMode) == false)
    {
        return false;
    }
    
    return true;
}

bool CAN_ChangeBaudrate(CAN_Baud_t baud)
{
    uint8_t currentMode = CANSTAT & 0xE0;
    
    CAN_SetOpMode(Configuration);
    
    CAN_SetBaudrateRegisters(baud);
    
    CAN_SetOpMode(currentMode);
    
    return true;
}

bool CAN_SetMask(CAN_Mask_t *mask)
{
    uint8_t *ptr;
    
    /* only two masks available: 0 & 1*/
    if(mask->Number > 1)
    {
        return false;
    }
    
    ptr = &RXM0SIDH + mask->Number * 4;
    
    if(mask->Extended)
    {
        *ptr++ = (mask->ID>>21) & 0x00FF;
        *ptr = ((mask->ID>>16) & 0x0003) | 0X08;
        *ptr = *ptr | ((mask->ID >> 13) & 0xE0);
        *ptr |= TXBnSIDL_EXIDE;
        *ptr++;
        *ptr++ = (mask->ID>>8) & 0x00FF;
        *ptr++ = mask->ID & 0x00FF;
    }
    else
    {
        *ptr++ = (mask->ID>>3) & 0x00FF;
        *ptr++ = ((mask->ID<<5) & 0x00E0) & ~TXBnSIDL_EXIDE;
        *ptr++;
        *ptr++;
    }
    
    return true;
}

bool CAN_SetFilter(CAN_Filter_t *filter)
{
    uint8_t *ptr;
    
    /* only 6 filters available in mode 0 */
    /* mask 0 <> filters 0, 1 */
    /* mask 1 <> filters 2, 3, 4, 5*/
    if(filter->Number  > 5)
    {
        return false;
    }
    
    ptr = &RXF0SIDH + filter->Number * 4;
    
    if(filter->Extended)
    {
        *ptr++ = (filter->ID>>21) & 0x00FF;
        *ptr = ((filter->ID>>16) & 0x0003) | 0X08;
        *ptr = *ptr | ((filter->ID >> 13) & 0xE0);
        *ptr |= TXBnSIDL_EXIDE;
        *ptr++;
        *ptr++ = (filter->ID>>8) & 0x00FF;
        *ptr++ = filter->ID & 0x00FF;
    }
    else
    {
        *ptr++ = (filter->ID>>3) & 0x00FF;
        *ptr++ = ((filter->ID<<5) & 0x00E0) & ~TXBnSIDL_EXIDE;
        *ptr++;
        *ptr++;
    }
    
    return true;
}

bool CAN_ReadMask(CAN_Mask_t *mask)
{
    uint8_t *ptr;
    
    /* only two masks available: 0 & 1*/
    if(mask->Number > 1)
    {
        return false;
    }
    
    ptr = &RXM0SIDH + mask->Number * 4;
    
    /* check if Extended Frame and get ID */
    if(*(ptr + 1) & RXBnSIDL_EXID)
    {
        mask->Extended = true;
        mask->ID = *ptr++;
        mask->ID = (mask->ID << 3) | ((*ptr >> 5) & 0x07);
        mask->ID = (mask->ID << 2) | (*ptr++ & 0x03); 
        mask->ID = (mask->ID << 8) | *ptr++;
        mask->ID = (mask->ID << 8) | *ptr++;
    }
    else
    {
        mask->Extended = false;
        mask->ID = *ptr++;
        mask->ID = mask->ID << 8;
        mask->ID = mask->ID | *ptr++;
        mask->ID = mask->ID >> 5;
        *ptr++;
        *ptr++;
    }
    
    return true;
}

bool CAN_ReadFilter(CAN_Filter_t *filter)
{
    uint8_t *ptr;
    
    /* only 6 filters available in mode 0 */
    /* mask 0 <> filters 0, 1 */
    /* mask 1 <> filters 2, 3, 4, 5*/
    if(filter->Number  > 5)
    {
        return false;
    }
    
    ptr = &RXF0SIDH + filter->Number * 4;
    
    if(*(ptr + 1) & RXBnSIDL_EXID)
    {
        filter->Extended = true;
        filter->ID = *ptr++;
        filter->ID = (filter->ID << 3) | ((*ptr >> 5) & 0x07);
        filter->ID = (filter->ID << 2) | (*ptr++ & 0x03); 
        filter->ID = (filter->ID << 8) | *ptr++;
        filter->ID = (filter->ID << 8) | *ptr++;
    }
    else
    {
        filter->Extended = false;
        filter->ID = *ptr++;
        filter->ID = filter->ID << 8;
        filter->ID = filter->ID | *ptr++;
        filter->ID = filter->ID >> 5;
        *ptr++;
        *ptr++;
    }
    
    return true;
}