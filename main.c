#include <pic18f26k80.h>

#include "main.h"
#include "can.h"

CAN_Message_t inbox;
uint16_t Milliseconds;
bool MainLoopTick = false;

void IO_Initialize(void)
{   
    /* slew rate */
    SLRCONbits.SLRB = 0;
    
    /* initial values */
    LATBbits.LATB4 = 0;
    
    LATBbits.LATB6 = 0;
    LATBbits.LATB7 = 0;
    
    /* data direction */
    TRISBbits.TRISB7 = 0;   /* OUTPUT */
    TRISBbits.TRISB6 = 0;   /* OUTPUT */
    TRISBbits.TRISB4 = 0;   /* CAN_STB */
    
    /* weak pull-ups */
    INTCON2bits.nRBPU = 0;
    
    WPUBbits.WPUB7 = 0;
    WPUBbits.WPUB6 = 1;     /* Enable pull up for +12V DETR */
    
    
}

bool IO_Read(uint8_t* frame)
{
    bool status = true;
    
    status = false;
    /*
    if(*(frame + 2) != 0x0D || *(frame + 1) != '0')
    {
        status = false;
    }
    else
    {
        switch(*(frame + 1))
        {
            case '0':
            {
                printf("A");
                if(PORTBbits.RB6 == 0)
                {
                    printf("0");
                }
                else
                {
                    printf("1");
                }
                break;
            }
            default:
            {
                status = false;
            }
        }
    }
    */
    
    if(status == true)
    {
        ReplySuccess();
    }
    else
    {
        ReplyFailure();
    }
    
    return status;
}

bool IO_Write(uint8_t* frame)
{
    bool status = true;
    if(*(frame + 3) != 0x0D || (*(frame + 2) != '1' && *(frame + 2) != '0'))
    {
        status = false;
    }
    else
    {
        switch(*(frame + 1))
        {
            case '0':
            {
                LATBbits.LATB6 = *(frame + 2) - 0x30;
                break;
            }
            case '1':
            {
                LATBbits.LATB7 = *(frame + 2) - 0x30;
                break;
            }
            default:
            {
                status = false;
            }
        }
    }
    if(status == true)
    {
        ReplySuccess();
    }
    else
    {
        ReplyFailure();
    }
    
    return status;
}

void Milliseconds_Tick(void)
{
    if(++Milliseconds >= 60000)
    {
        Milliseconds = 0;
    }
    MainLoopTick = true;
}

uint16_t Milliseconds_Read(void)
{
    uint16_t ms;
    //INTERRUPT_GlobalInterruptDisable();
    ms = Milliseconds;
    //INTERRUPT_GlobalInterruptEnable();
    return ms;
}

void Oscillator_Initialize(void)
{
    OSCCON = 0b01110000;
    
    OSCCON2 = 0b00000000;
    
    OSCTUNE = 0b01000000;
    
    REFOCON = 0b00000000;
}

void interrupt System_ISR(void)
{
    if(PIE1bits.RC1IE == 1 && PIR1bits.RC1IF == 1)
    {
        UART1_ISR();
    }
    if(PIR5bits.RXB0IF == 1 || PIR5bits.RXB1IF == 1)
    {
        CAN_Receive(&inbox);
        inbox.Timestamp = Milliseconds_Read();
        CAN_FrameReceived(&inbox);
    }
    if(INTCONbits.TMR0IE == 1 && INTCONbits.TMR0IF == 1)
    {
        TMR0_ISR();
    }
}

void Interrupts_Initialize(void)
{
    //Serial Reception Interrupt
    PIR1bits.RCIF = 0;
    PIE1bits.RCIE = 1;
    
    //CAN Receive interrupts
    PIR5bits.RXB0IF = 0;
    PIR5bits.RXB1IF = 0;
    
    PIE5bits.RXB0IE = 1;
    PIE5bits.RXB1IE = 1;
    
    RCONbits.IPEN = 0;          //No interrupts priority
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
}

void Serial_FrameReceived(void)
{
    if(SerialFrames.Pending >= SERIAL_QUEUE_SIZE)
    {
        return;
    }
    
    SerialFrames.Pending++;
    
    uint8_t* bufferPtr = SerialFrameBuffer;
    
    int i = 0;
    do
    {
        SerialFrames.Queue[SerialFrames.Tail].Data[i++] = *bufferPtr;
        
    }while(*bufferPtr++ != 0x0D);
    
    SerialFrames.Queue[SerialFrames.Tail].Pending = true;
    
    if(SerialFrames.Tail >= SERIAL_QUEUE_SIZE - 1)
    {
        SerialFrames.Tail = 0;
    }
    else
    {
        SerialFrames.Tail++;
    }
}

void CAN_FrameReceived(CAN_Message_t* inbox)
{
    if(CAN_Frames.Pending < CAN_QUEUE_SIZE)
    {
        CAN_Frames.Pending++;

        CAN_Frames.Queue[CAN_Frames.Tail].Timestamp = inbox->Timestamp;
        CAN_Frames.Queue[CAN_Frames.Tail].IsRTR = inbox->IsRTR;
        CAN_Frames.Queue[CAN_Frames.Tail].IsExtended = inbox->IsExtended;
        CAN_Frames.Queue[CAN_Frames.Tail].ID = inbox->ID;
        CAN_Frames.Queue[CAN_Frames.Tail].DLC = inbox->DLC;

        for(int i = 0; i < CAN_Frames.Queue[CAN_Frames.Tail].DLC; i++)
        {
            CAN_Frames.Queue[CAN_Frames.Tail].Data[i] = inbox->Data[i];
        }

        if(CAN_Frames.Tail >= CAN_QUEUE_SIZE - 1)
        {
            CAN_Frames.Tail = 0;
        }
        else
        {
            CAN_Frames.Tail++;
        }
    }
}

void System_Initialize(void)
{
    CAN_Configuration_t config;
    
    SerialFrames.Pending = 0;
    SerialFrames.Head = 0;
    SerialFrames.Tail = 0;
    
    CAN_Frames.Pending = 0;
    CAN_Frames.Head = 0;
    CAN_Frames.Tail = 0;
   
    /* Oscillator */
    Oscillator_Initialize();
    
    /* Serial Communications */
    UART1_Initialize(BAUD_115200);
    UART1_SetBuffer(SerialFrameBuffer);
    UART1_SetFrameReceivedHandler(Serial_FrameReceived);
    
    /* CAN Communications */
    TRISBbits.TRISB3 = 1;   
    TRISBbits.TRISB2 = 0;
    config.Baudrate = Baud_500kbps;
    config.Mode = Legacy;
    config.OpMode = Normal;
    CAN_Initialize(&config);
    
    /* Eeprom Memory */
    Eeprom_Initialize();
    
    /* System Timer */
    TMR0_Initialize();
    TMR0_SetInterruptHandler(Milliseconds_Tick);
    
    /* Interrupts */
    Interrupts_Initialize();
    
    /* IO pins */
    IO_Initialize();
}

void Bootload(uint8_t* frame)
{
    if(*(frame + 1) == '9' && *(frame + 2) == '5')
    {
        Eeprom_write8(~APP_MODE, BOOT_KEY);
        printf("Boot Jump\r");
        NOP();
        NOP();
        RESET();
    }
}

void main(void) 
{
    System_Initialize();
    uint8_t* frame;
    
    while(1)
    {  
        if(MainLoopTick == true)
        {
            if(CAN_Frames.Pending > 0)
            {
                SLCAN_Print(&CAN_Frames.Queue[CAN_Frames.Head]);

                if(CAN_Frames.Head >= CAN_QUEUE_SIZE - 1)
                {
                    CAN_Frames.Head = 0;
                }
                else
                {
                    CAN_Frames.Head++;
                }

                INTCONbits.GIE = 0;
                CAN_Frames.Pending--;
                INTCONbits.GIE = 1;
            }

            if(SerialFrames.Pending > 0)
            {
                frame = &SerialFrames.Queue[SerialFrames.Head].Data[0];
                switch(*(frame + 0))
                {
                    case 'B':
                    {
                        Bootload(frame);
                        break;
                    }
                    case 'S':        /* Setup CAN bitrate: Sn[CR] */
                    {
                        SLCAN_SetCANBitrate(frame);
                        break;
                    }
                    case 's':        /* Setup BTR0/BTR1 bitrate registers: Sxxyy[CR] */
                    {
                        SLCAN_SetCANBitrateRegisters();
                        break;
                    }
                    case 'O':        /* Open the CAN channel: O[CR] */
                    {
                        SLCAN_OpenCANChannel();
                        break;
                    }
                    case 'C':        /* Close the CAN channel: C[CR] */
                    {
                        SLCAN_CloseCANChannel();
                        break;
                    }
                    case 't':        /* Transmit a standard (11bit) CAN frame: tiiildd...[CR] */
                    {
                        SLCAN_TxStandardFrame(frame);
                        break;
                    }
                    case 'T':        /* Transmit an extended CAN frame: Tiiiiiiiildd...[CR] */
                    {
                        SLCAN_TxExtendedFrame(frame);
                        break;
                    }
                    case 'r':        /* Transmit a standard RTR (11bit) CAN frame: riiil[CR] */
                    {
                        SLCAN_TxStandardRTR(frame);
                        break;
                    }
                    case 'R':        /* Transmit an extended RTR (29bit) CAN frame: Riiiiiiiil[CR] */
                    {
                        SLCAN_TxExtendedRTR(frame);
                        break;
                    }
                    case 'F':        /* Read Status Flags: F[CR] */
                    {
                        SLCAN_ReadFlags();
                        break;
                    }
                    case 'M':        /* Set Acceptance Code Register: Mxxxxxxxx[CR] */
                    {
                        SLCAN_SetFilterCode(frame);
                        break;
                    }
                    case 'm':        /* Set Accpetance Mask Register: mxxxxxxxx[CR] */
                    {
                        SLCAN_SetFilterMask(frame);
                        break;
                    }
                    case 'V':        /* Get Version Number: V[CR] */
                    case 'v':
                    {
                        SLCAN_PrintVersion();
                        break;
                    }
                    case 'N':        /* Get Serial Number: N[CR] */
                    case 'n':
                    {
                        SLCAN_PrintSerialNumber();
                        break;
                    }
                    case 'Z':        /* Sets Time Stamp ON/OFF for received frames: Zn[CR] */
                    {
                        SLCAN_EnableRxTimestamp(frame);
                        break;
                    }
                    case 'K':       /* Sets the ECAN module Mask specified in the received frame */
                    case 'k':
                    {
                        SLCAN_SetECANMask(frame);
                        break;
                    }
                    case 'L':       /* Sets the ECAN module Filter specified in the received frame */
                    case 'l':
                    {
                        SLCAN_SetECANFilter(frame);
                        break;
                    }
                    case 'U':       /* Sets the baudrate of the Serial Communications  */
                    {
                        SLCAN_SetSerialBaudrate(frame);
                        break;
                    }
                    case 'G':       /* Reads the requested item(Mask or Filter) back to the PC */
                    {
                        SLCAN_GetItem(frame);
                        break;
                    }
                    case 'W':       /* Writes the specified IO pin to the given state */
                    case 'w':
                    {
                        IO_Write(frame);
                        break;
                    }
                    case 'A':       /* Reads the state of the specified IO pin */
                    case 'a':
                    {
                        IO_Read(frame);
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }

                if(SerialFrames.Head >= SERIAL_QUEUE_SIZE - 1)
                {
                    SerialFrames.Head = 0;
                }
                else
                {
                    SerialFrames.Head++;
                }

                INTCONbits.GIE = 0;
                SerialFrames.Pending--;
                INTCONbits.GIE = 1;

                SerialFrames.Queue[SerialFrames.Head].Pending = false;
            }
        }
    }
}
