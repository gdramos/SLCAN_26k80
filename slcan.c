#include "slcan.h"

uint8_t atonx(uint8_t byte)
{   
    if(isxdigit(byte))
    {
        if(byte >= '0' && byte <= '9')
        {
            byte -= 0x30;
        }
        else if(byte >= 'A' && byte <= 'F')
        {
            byte -= 0x37;
        }
        else if(byte >= 'a' && byte <= 'f')
        {
            byte -= 0x57;
        }
    }
    
    return (byte & 0x0F);
}

uint8_t atox_byte(uint8_t *b)
{
    uint8_t result;
    
    result = atonx(*b++);
    result = (result << 4) | atonx(*b++);
    
    return result;
}

uint32_t atox_sid(uint8_t *id)
{
    uint32_t result;
    
    result = atonx(*id++) << 8;
    result |= (uint32_t)(atonx(*id++) << 4);
    result |= (uint32_t)atonx(*id++);
    
    return (result & 0x7FF);
}

uint32_t atox_xid(uint8_t *id)
{
    uint32_t result;
    
    result = atonx(*id++);
    result = (result << 4) | atonx(*id++);
    result = (result << 4) | atonx(*id++);
    result = (result << 4) | atonx(*id++);
    result = (result << 4) | atonx(*id++);
    result = (result << 4) | atonx(*id++);
    result = (result << 4) | atonx(*id++);
    result = (result << 4) | atonx(*id++);
    
    result &= 0x1FFFFFFF;
    
    return result;
}

void SLCAN_Print(CAN_Message_t *inbox)
{
    if(inbox->IsExtended == EXT)
    {
        if(inbox->IsRTR)
        {
            printf("R");
        }
        else
        {
            printf("T");
        }
        printf("%2.2X", (uint8_t)(inbox->ID>>24));
        printf("%2.2X", (uint8_t)(inbox->ID>>16));
        printf("%2.2X", (uint8_t)(inbox->ID>>8));
        printf("%2.2X", (uint8_t)(inbox->ID>>0));
        printf("%1.1X", inbox->DLC);
    }
    else
    {
        if(inbox->IsRTR)
        {
            printf("r");
        }
        else
        {
            printf("t");
        }
        printf("%1.1X", (uint8_t)(inbox->ID>>8));
        printf("%2.2X", (uint8_t)(inbox->ID>>0));
        printf("%1.1X", inbox->DLC);
    }

    for(int i = 0; i < inbox->DLC; i++)
        printf("%2.2X", inbox->Data[i]);
    
    if(SLCAN_TimestampEnable == true)
    {
        printf("%2.2X", (uint8_t)(inbox->Timestamp>>8));
        printf("%2.2X", (uint8_t)(inbox->Timestamp>>0));
    }
    
    printf("\r");
}

void SLCAN_SetCANBitrate(uint8_t* frame)
{
    if((CANSTAT & CANSTAT_OPMODE_SLEEP_MODE) != CANSTAT_OPMODE_SLEEP_MODE || *(frame + 2) != 0x0D)
    {
        ReplyFailure();
    }
    else
    {
        if(*(frame + 1) < 0x39 && *(frame + 1) > 0x2A)
        {
            CAN_ChangeBaudrate(*(frame + 1) - 0x30);
            ReplySuccess();
        }
        else
        {
            ReplyFailure();
        }
    }
}

void SLCAN_SetCANBitrateRegisters(void)
{
    ReplyFailure();
}

void SLCAN_OpenCANChannel(void)
{
    CAN_SetOpMode(Normal);
    
    ReplySuccess();
}

void SLCAN_CloseCANChannel(void)
{
    CAN_SetOpMode(Sleep);
    
    ReplySuccess();
}

void SLCAN_TxStandardRTR(uint8_t* frame)
{
    CAN_Message_t outbox;

    outbox.ID = atox_sid(frame + 1);
    outbox.DLC = atonx(*(frame + 4));
    outbox.IsExtended = false;
    outbox.IsRTR = true;

    if(CAN_Transmit(&outbox) == true)
    {
        ReplySuccess();
    }
    else
    {
        ReplyFailure();
    }
}

void SLCAN_TxExtendedRTR(uint8_t* frame)
{
    CAN_Message_t outbox;

    outbox.ID = atox_xid(frame + 1);
    outbox.DLC = atonx(*(frame + 9));
    outbox.IsExtended = true;
    outbox.IsRTR = true;

    if(CAN_Transmit(&outbox) == true)
    {
        ReplySuccess();
    }
    else
    {
        ReplyFailure();
    }
}

void SLCAN_TxStandardFrame(uint8_t* frame)
{
    CAN_Message_t outbox;
    
    if(*(frame + 4) == 0 && (*(frame + 5) == 'R' || *(frame + 5) == 'r'))
    {
        SLCAN_TxStandardRTR(frame);
    }

    outbox.ID = atox_sid(frame + 1);
    outbox.DLC = atonx(*(frame + 4));
    outbox.IsExtended = false;
    outbox.IsRTR = false;
    
    for(int i = 0; i < outbox.DLC; i++)
    {
        outbox.Data[i] = atox_byte(frame + 5 + i*2);
    }

    if(CAN_Transmit(&outbox) == true)
    {
        ReplySuccess();
    }
    else
    {
        ReplyFailure();
    }
}

void SLCAN_TxExtendedFrame(uint8_t* frame)
{
    CAN_Message_t outbox;
    
    if(*(frame + 9) == 0 && (*(frame + 10) == 'R' || *(frame + 10) == 'r'))
    {
        SLCAN_TxExtendedRTR(frame);
    }

    outbox.ID = atox_xid(frame + 1);
    outbox.DLC = atonx(*(frame + 9));
    outbox.IsExtended = true;
    outbox.IsRTR = false;
    
    for(int i = 0; i < outbox.DLC; i++)
    {
        outbox.Data[i] = atox_byte(frame + 10 + i*2);
    }

    if(CAN_Transmit(&outbox) == true)
    {
        ReplySuccess();
    }
    else
    {
        ReplyFailure();
    }
}

void SLCAN_ReadFlags(void)
{
    printf("00");
    ReplySuccess();
}

void SLCAN_SetFilterCode(uint8_t* frame)
{   
    ReplySuccess();
}

void SLCAN_SetFilterMask(uint8_t* frame)
{
    ReplySuccess();
}

void SLCAN_PrintVersion(void)
{
    printf("V");
    printf(FIRMWARE_VERSION);
    ReplySuccess();
}

void SLCAN_PrintSerialNumber(void)
{
    printf(DUMMY_SERIAL_NUMBER);
    ReplySuccess();
}

void SLCAN_EnableRxTimestamp(uint8_t* frame)
{
    if(*(frame + 1) == '0' && *(frame + 2) == 0x0D)
    {
        SLCAN_TimestampEnable = false;
    }
    else if(*(frame + 1) == '1' && *(frame + 2) == 0x0D)
    {
        SLCAN_TimestampEnable = true;
    }
    else
    {
        ReplyFailure();
        return;
    }
    
    ReplySuccess();
}

void SLCAN_SetECANMask(uint8_t* frame)
{
    CAN_Mask_t mask;
    
    if(*(frame + 0) == 'K' && *(frame + 10) == 0x0D)
    {
        mask.Extended = true;
        mask.ID = atox_xid(frame + 2);
        mask.Number = atonx(*(frame + 1));
    }
    else if(*(frame + 0) == 'k' && *(frame + 5) == 0x0D)
    {
        mask.Extended = false;
        mask.ID = atox_sid(frame + 2);
        mask.Number = atonx(*(frame + 1));
    }
    else
    {
        ReplyFailure();
        return;
    }
    
    if(CAN_SetMask(&mask) == true)
    {
        ReplySuccess();
    }
    else
    {
        ReplyFailure();
    }
}

void SLCAN_SetECANFilter(uint8_t* frame)
{
    CAN_Filter_t filter;
    
    if(*(frame + 0) == 'L' && *(frame + 10) == 0x0D)
    {
        filter.Extended = true;
        filter.ID = atox_xid(frame + 2);
        filter.Number = atonx(*(frame + 1));
    }
    else if(*(frame + 0) == 'l' && *(frame + 5) == 0x0D)
    {
        filter.Extended = false;
        filter.ID = atox_sid(frame + 2);
        filter.Number = atonx(*(frame + 1));
    }
    else
    {
        ReplyFailure();
        return;
    }
    
    if(CAN_SetFilter(&filter) == true)
    {
        ReplySuccess();
    }
    else
    {
        ReplyFailure();
    }
}

void SLCAN_ReadMask(uint8_t* frame)
{
    CAN_Mask_t mask;
    
    mask.Number = atonx(*(frame + 2));
    
    if(CAN_ReadMask(&mask) == true)
    {
        if(mask.Extended == true)
        {
            printf("K");
            printf("%1.1X", (uint8_t)(mask.Number));
            printf("%2.2X", (uint8_t)(mask.ID>>24));
            printf("%2.2X", (uint8_t)(mask.ID>>16));
            printf("%2.2X", (uint8_t)(mask.ID>>8));
            printf("%2.2X", (uint8_t)(mask.ID>>0));
        }
        else
        {
            printf("k");
            printf("%1.1X", (uint8_t)(mask.Number));
            printf("%1.1X", (uint8_t)(mask.ID>>8));
            printf("%2.2X", (uint8_t)(mask.ID>>0));
        }
        ReplySuccess();
    }
    else
    {
        ReplyFailure();
    }
}

void SLCAN_ReadFilter(uint8_t* frame)
{
    CAN_Filter_t filter;
    
    filter.Number = atonx(*(frame + 2));
    
    if(CAN_ReadFilter(&filter) == true)
    {
        if(filter.Extended == true)
        {
            printf("L");
            printf("%1.1X", (uint8_t)(filter.Number));
            printf("%2.2X", (uint8_t)(filter.ID>>24));
            printf("%2.2X", (uint8_t)(filter.ID>>16));
            printf("%2.2X", (uint8_t)(filter.ID>>8));
            printf("%2.2X", (uint8_t)(filter.ID>>0));
        }
        else
        {
            printf("l");
            printf("%1.1X", (uint8_t)(filter.Number));
            printf("%1.1X", (uint8_t)(filter.ID>>8));
            printf("%2.2X", (uint8_t)(filter.ID>>0));
        }
        ReplySuccess();
    }
    else
    {
        ReplyFailure();
    }
}

void SLCAN_GetItem(uint8_t* frame)
{   
    switch(*(frame + 1))
    {
        case 'K':
        case 'k':
        {
            SLCAN_ReadMask(frame);
            break;
        }
        case 'L':
        case 'l':
        {
            SLCAN_ReadFilter(frame);
            break;
        }
        default:
        {
            ReplyFailure();
            break;
        }
    }
}

void SLCAN_SetSerialBaudrate(uint8_t* frame)
{
    uint16_t baud;
    
    switch(*(frame + 1))
    {
        case 0:
        {
            baud = BAUD_230400;
            break;
        }
        case 1:
        {
            baud = BAUD_115200;
            break;
        }
        case 2:
        {
            baud = BAUD_57600;
            break;
        }
        case 3:
        {
            baud = BAUD_38400;
            break;
        }
        case 4:
        {
            baud = BAUD_19200;
            break;
        }
        case 5:
        {
            baud = BAUD_9600;
            break;
        }
        case 6:
        {
            baud = BAUD_2400;
            break;
        }
        case 7:
        {
            baud = BAUD_460800;
            break;
        }
        case 8:
        {
            baud = BAUD_921600;
        }
        default:
        {
            ReplyFailure();
            break;
        }
    }
    
    ReplySuccess();
    UART1_Initialize((UART1_Baud_t)baud);
}