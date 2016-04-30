/********************* (C) COPYRIGHT 2015 e-Design Co.,Ltd. ********************
 S100 APP Ver 30.10 startup                                      Author : bure
*******************************************************************************/

    MODULE  ?cstartup

        SECTION CSTACK:DATA:NOROOT(3)
        SECTION INTVEC:CODE:NOROOT(2)

        EXTERN  __iar_program_start

        PUBLIC  __vector_table

        DATA
__vector_table
        DCD     sfe(CSTACK)
        DCD     Reset_Handler             ; Reset Handler
        DCD     NMI_Handler               ; NMI Handler
        DCD     HardFault_Handler         ; Hard Fault Handler
        DCD     MemManage_Handler         ; MPU Fault Handler
        DCD     BusFault_Handler          ; Bus Fault Handler
        DCD     UsageFault_Handler        ; Usage Fault Handler
        DCD     0                         ; Reserved
        DCD     0                         ; Reserved
        DCD     0                         ; Reserved
        DCD     0                         ; Reserved
        DCD     SVC_Handler               ; SVCall Handler
        DCD     DebugMon_Handler          ; Debug Monitor Handler
        DCD     0                         ; Reserved
        DCD     PendSV_Handler            ; PendSV Handler
        DCD     SysTick_Handler           ; SysTick Handler

        THUMB
        EXPORT  DFU_Func_ptr
DFU_Func_ptr
        B.W     0;LCD_Init
        B.W     0;ClrScrn
        B.W     0;SetBlock
        B.W     0;SetPosi
        B.W     0;SetPixel
        B.W     0;ReadPixel
        B.W     0;SetPixels
        B.W     0;SendPixels

        B.W     0;Direction
        B.W     0;GetChar8x14
        B.W     0;DispLogo
        B.W     0;DispStr
        B.W     0;Dev_SN
        B.W     0;Lic_Gen
        B.W     0;Ident
        B.W     0;Chk_SYS

        B.W     0;Lic_ok
        B.W     0;DeviceInfo
        B.W     0;Delay_mS

        DATA
        DCD     USB_HP_CAN1_TX_IRQHandler  ; USB High Priority or CAN1 TX
        DCD     USB_LP_CAN1_RX0_IRQHandler ; USB Low  Priority or CAN1 RX0
        DCD     DEFAULT_ISR;//CAN_RX1_IRQHandler;
        DCD     DEFAULT_ISR;
        DCD     DEFAULT_ISR;
        DCD     DEFAULT_ISR;
        DCD     DEFAULT_ISR;
        DCD     DEFAULT_ISR;
        DCD     DEFAULT_ISR;
        DCD     DEFAULT_ISR;
        DCD     DEFAULT_ISR;
        DCD     DEFAULT_ISR;
        DCD     DEFAULT_ISR;
        DCD     I2C1_EV_IRQHandler;
        DCD     I2C1_ER_IRQHandler;
        
        THUMB

        IMPORT  RCC_Config
        IMPORT  CTR_HP
        IMPORT  USB_Istr
        IMPORT  DEFAULT_ISR
        IMPORT  I2C1_EV_IRQHandler
        IMPORT  I2C1_ER_IRQHandler
        PUBWEAK Reset_Handler
        SECTION .text:CODE:REORDER(2)
Reset_Handler
        LDR     R0, =RCC_Config
        BLX     R0
        LDR     R0, =__iar_program_start
        BX      R0

        PUBWEAK NMI_Handler
        SECTION .text:CODE:REORDER(1)
NMI_Handler
        B NMI_Handler

        PUBWEAK HardFault_Handler
        SECTION .text:CODE:REORDER(1)
HardFault_Handler
        B HardFault_Handler

        PUBWEAK MemManage_Handler
        SECTION .text:CODE:REORDER(1)
MemManage_Handler
        B MemManage_Handler

        PUBWEAK BusFault_Handler
        SECTION .text:CODE:REORDER(1)
BusFault_Handler
        B BusFault_Handler

        PUBWEAK UsageFault_Handler
        SECTION .text:CODE:REORDER(1)
UsageFault_Handler
        B UsageFault_Handler

        PUBWEAK SVC_Handler
        SECTION .text:CODE:REORDER(1)
SVC_Handler
        B SVC_Handler

        PUBWEAK DebugMon_Handler
        SECTION .text:CODE:REORDER(1)
DebugMon_Handler
        B DebugMon_Handler

        PUBWEAK PendSV_Handler
        SECTION .text:CODE:REORDER(1)
PendSV_Handler
        B PendSV_Handler

        PUBWEAK SysTick_Handler
        SECTION .text:CODE:REORDER(1)
SysTick_Handler
        B SysTick_Handler

        PUBWEAK USB_HP_CAN1_TX_IRQHandler
        SECTION .text:CODE:REORDER(1)
USB_HP_CAN1_TX_IRQHandler
        B     CTR_HP ; USB_HP_CAN1_TX_IRQHandler

        PUBWEAK USB_LP_CAN1_RX0_IRQHandler
        SECTION .text:CODE:REORDER(1)
USB_LP_CAN1_RX0_IRQHandler
        B     USB_Istr ; USB_LP_CAN1_RX0_IRQHandler



    

        END

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
