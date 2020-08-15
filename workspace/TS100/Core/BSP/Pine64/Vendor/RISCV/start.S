// See LICENSE for license details.

#include "riscv_encoding.h"

	.section .init

    .weak  eclic_msip_handler                 //弱定义函数，这些在编译为bin后可以看到将全部为0
    .weak  eclic_mtip_handler
    .weak  eclic_bwei_handler
    .weak  eclic_pmovi_handler
    .weak  WWDGT_IRQHandler
    .weak  LVD_IRQHandler
    .weak  TAMPER_IRQHandler
    .weak  RTC_IRQHandler
    .weak  FMC_IRQHandler
    .weak  RCU_IRQHandler
    .weak  EXTI0_IRQHandler
    .weak  EXTI1_IRQHandler
    .weak  EXTI2_IRQHandler
    .weak  EXTI3_IRQHandler
    .weak  EXTI4_IRQHandler
    .weak  DMA0_Channel0_IRQHandler
    .weak  DMA0_Channel1_IRQHandler
    .weak  DMA0_Channel2_IRQHandler
    .weak  DMA0_Channel3_IRQHandler
    .weak  DMA0_Channel4_IRQHandler
    .weak  DMA0_Channel5_IRQHandler
    .weak  DMA0_Channel6_IRQHandler
    .weak  ADC0_1_IRQHandler
    .weak  CAN0_TX_IRQHandler
    .weak  CAN0_RX0_IRQHandler
    .weak  CAN0_RX1_IRQHandler
    .weak  CAN0_EWMC_IRQHandler
    .weak  EXTI5_9_IRQHandler
    .weak  TIMER0_BRK_IRQHandler
    .weak  TIMER0_UP_IRQHandler
    .weak  TIMER0_TRG_CMT_IRQHandler
    .weak  TIMER0_Channel_IRQHandler
    .weak  TIMER1_IRQHandler
    .weak  TIMER2_IRQHandler
    .weak  TIMER3_IRQHandler
    .weak  I2C0_EV_IRQHandler
    .weak  I2C0_ER_IRQHandler
    .weak  I2C1_EV_IRQHandler
    .weak  I2C1_ER_IRQHandler
    .weak  SPI0_IRQHandler
    .weak  SPI1_IRQHandler
    .weak  USART0_IRQHandler
    .weak  USART1_IRQHandler
    .weak  USART2_IRQHandler
    .weak  EXTI10_15_IRQHandler
    .weak  RTC_Alarm_IRQHandler
    .weak  USBFS_WKUP_IRQHandler
    .weak  EXMC_IRQHandler
    .weak  TIMER4_IRQHandler
    .weak  SPI2_IRQHandler
    .weak  UART3_IRQHandler
    .weak  UART4_IRQHandler
    .weak  TIMER5_IRQHandler
    .weak  TIMER6_IRQHandler
    .weak  DMA1_Channel0_IRQHandler
    .weak  DMA1_Channel1_IRQHandler
    .weak  DMA1_Channel2_IRQHandler
    .weak  DMA1_Channel3_IRQHandler
    .weak  DMA1_Channel4_IRQHandler
    .weak  CAN1_TX_IRQHandler
    .weak  CAN1_RX0_IRQHandler
    .weak  CAN1_RX1_IRQHandler
    .weak  CAN1_EWMC_IRQHandler
    .weak  USBFS_IRQHandler

vector_base:                            //中断向量表
    j _start                            //第一条指令即跳转到_start处开始执行
    .align    2
    .word     0
    .word     0
    .word     eclic_msip_handler
    .word     0
    .word     0
    .word  	  0
    .word  	  eclic_mtip_handler
    .word  	  0
    .word  	  0
    .word  	  0
    .word  	  0
    .word  	  0
    .word  	  0
    .word  	  0
    .word 	  0
    .word  	  0
    .word  	  eclic_bwei_handler
    .word  	  eclic_pmovi_handler
    .word  	  WWDGT_IRQHandler
	.word     LVD_IRQHandler
	.word     TAMPER_IRQHandler
	.word     RTC_IRQHandler
	.word     FMC_IRQHandler
	.word     RCU_IRQHandler
	.word     EXTI0_IRQHandler
	.word     EXTI1_IRQHandler
	.word     EXTI2_IRQHandler
	.word     EXTI3_IRQHandler
	.word     EXTI4_IRQHandler
	.word     DMA0_Channel0_IRQHandler
	.word     DMA0_Channel1_IRQHandler
	.word     DMA0_Channel2_IRQHandler
	.word     DMA0_Channel3_IRQHandler
	.word     DMA0_Channel4_IRQHandler
	.word     DMA0_Channel5_IRQHandler
	.word     DMA0_Channel6_IRQHandler
	.word     ADC0_1_IRQHandler
	.word     CAN0_TX_IRQHandler
	.word     CAN0_RX0_IRQHandler
	.word     CAN0_RX1_IRQHandler
	.word     CAN0_EWMC_IRQHandler
	.word     EXTI5_9_IRQHandler
	.word     TIMER0_BRK_IRQHandler
	.word     TIMER0_UP_IRQHandler
	.word     TIMER0_TRG_CMT_IRQHandler
	.word     TIMER0_Channel_IRQHandler
	.word     TIMER1_IRQHandler
	.word     TIMER2_IRQHandler
	.word     TIMER3_IRQHandler
	.word     I2C0_EV_IRQHandler
	.word     I2C0_ER_IRQHandler
	.word     I2C1_EV_IRQHandler
	.word     I2C1_ER_IRQHandler
	.word     SPI0_IRQHandler
	.word     SPI1_IRQHandler
	.word     USART0_IRQHandler
	.word     USART1_IRQHandler
	.word     USART2_IRQHandler
	.word     EXTI10_15_IRQHandler
	.word     RTC_Alarm_IRQHandler
	.word     USBFS_WKUP_IRQHandler
    .word     0
    .word     0
    .word     0
    .word     0
	.word     0
	.word     EXMC_IRQHandler
	.word     0
	.word     TIMER4_IRQHandler
	.word     SPI2_IRQHandler
	.word     UART3_IRQHandler
	.word     UART4_IRQHandler
	.word     TIMER5_IRQHandler
	.word     TIMER6_IRQHandler
	.word     DMA1_Channel0_IRQHandler
	.word     DMA1_Channel1_IRQHandler
	.word     DMA1_Channel2_IRQHandler
	.word     DMA1_Channel3_IRQHandler
	.word     DMA1_Channel4_IRQHandler
    .word     0
    .word     0
	.word     CAN1_TX_IRQHandler
	.word     CAN1_RX0_IRQHandler
	.word     CAN1_RX1_IRQHandler
	.word     CAN1_EWMC_IRQHandler
	.word     USBFS_IRQHandler

	.globl _start
	.type _start,@function

_start:

	csrc CSR_MSTATUS, MSTATUS_MIE  //CSR_MSTATUS &= ~0x8 mstatus[3]:0屏蔽全部中断 1不屏蔽全部中断 （当然这里全部中断是除过不可屏蔽中断）
	/* Jump to logical address first to ensure correct operation of RAM region  */
    la		a0,	_start          //a0 = _start
    li		a1,	1               //a1 = 1
	slli	a1,	a1, 29          //a1 = 0x20000000 raw起始地址
    bleu	a1, a0, _start0800  //if( a1 <= a0 ) JUMP _start0800
    srli	a1,	a1, 2           //a1 = 0x08000000 flash起始地址
    bleu	a1, a0, _start0800  //if( a1 <= a0 ) JUMP _start0800
    la		a0,	_start0800      //a0 = _start0800
    add		a0, a0, a1          //a0 = a0+a1
	jr      a0                  //JUMP a0

_start0800:

    /* Set the the NMI base to share with mtvec by setting CSR_MMISC_CTL */
    li t0, 0x200              //t0 = 0x200
    csrs CSR_MMISC_CTL, t0    //mmisc_ctl |= 0x200 CSR_MMISC_CTL[9]：设置NMI的地址与mtvec相同且，mcause.EXCCODE = 0xfff
                              //cs开头的指令是特有的用来操作内核寄存器的的指令，内核寄存器是一组特有的12位地址

	/* Intial the mtvt*/
    la t0, vector_base        //t0 = vector_base 向量表地址
    csrw CSR_MTVT, t0         //mtvt = vector_base 中断向量表基地址

	/* Intial the mtvt2 and enable it*/
    la t0, irq_entry          //t0 = irq_entry     irq_entry定义在entry.S,有freertos操作系统情况下定义在portasm.S
    csrw CSR_MTVT2, t0        //mtvt2 = irq_entry  mtvt2[31:2]: 中断入口函数地址
    csrs CSR_MTVT2, 0x1       //mtvt2 |= 0x1       mtvt2[0]:    1配置mtvt2的配置有效，配0则为mtvec内地址

    /* Intial the CSR MTVEC for the Trap ane NMI base addr*/
    la t0, trap_entry         //t0 = trap_entry     trap_entry定义在entry.S,有freertos操作系统情况下定义在portasm.S
    csrw CSR_MTVEC, t0        //mtvec = trap_entry  mtvec[31:6]：异常入口地址
                              //                    mtvec[5:0]：0B00011 -- ECLIC中断模式 其他：默认模式
                              // 这里配置为默认模式
                              // trap_entry基本上可以理解为arm里的hard/mem/use/svc等fault了，
                              // 这里在移植freertos时就使用了 ecall 进入trap_entry里 相当于arm里的PendSVC

    /* OS启动前配置中断栈为FHEAP的end地址 */
	la t0, ucHeap
	csrw CSR_MSCRATCH, t0

#ifdef __riscv_flen          //浮点宏
	/* Enable FPU */
	li t0, MSTATUS_FS        //t0 = 0x6000
	csrs mstatus, t0         //mstatus |= 0x6000  mstatus[14:13]:非零值启用浮点单元，配置为1或2会在第一次使用浮点单元时变为3，这里直接配置为3
	csrw fcsr, x0            //fcsr = x0 = 0 ??这里x0是否指的是zero通用寄存器值? 这个寄存器0-4位是浮点异常状态标志，5-7位是浮点舍入模式配置
#endif

.option push
.option norelax
	la gp, __global_pointer$ //__global_pointer$定义在link连接脚本内，指向所有.data段后偏移0x800的地址,0x800为2K，其值源于下文解释
                             //gp 意为global pointer （全局指针）寄存器是一个解决方案，
                             //为了以进一步优化存储器访问单个4KB区域内。
                             //链接器使用__global_pointer$符号定义来比较内存地址，如果在范围内，
                             //则它将绝对/相对pc寻址替换为gp相对寻址，从而使代码更有效。这个过程通过-Wl,--no-relax选项使用。
                             //这里.option norelax起到-Wl,--no-relax作用，就不需要在编译选项添加了
                             //上文参考自：https://gnu-mcu-eclipse.github.io/arch/riscv/programmer/#the-gp-global-pointer-register
                             //要让 relaxing 优化起作用，编译时要加入 -msmall-data-limit=n 参数，有了这个参数，
                             //编译器会把内存空间小于 n 字节的静态变量放入 .sdata 或者 .sdata.* 节，
                             //然后链接器将这部分静态变量集中在 __global_pointer$ +/- 2K 的范围内。
                             //上文参考自：https://blog.csdn.net/zoomdy/article/details/100703451
.option pop
	la sp, _sp               //sp = 主栈栈顶地址 _sp定义在link连接脚本内

	/* Load data section */
	la a0, _data_lma         //a0 = data的Load Memory Address          _data_lma定义在link连接脚本内
	la a1, _data             //a1 = data的Run Memory Address           _data定义在link连接脚本内
	la a2, _edata            //a2 = data的Run Memory Address结束地址   _edata定义在link连接脚本内
	bgeu a1, a2, 2f          //if( a1 >= a2 ) JUMP 2f 原则上不会出现_data地址大于_edata地址
                             //如果出现了则直接跳转到 2f 即下一个2标签
1:
	lw t0, (a0)              //t0 = _data_lma
	sw t0, (a1)              //*_data = t0      即 *_data_lma，按word写入
	addi a0, a0, 4           //a0 = a0 + 4   下一个ward
	addi a1, a1, 4           //a1 = a1 + 4
	bltu a1, a2, 1b          //if( a1 < a2 ) JUMP 1b 如果未到达_edata则跳转到 1b 即上一个1标签，这里会完成一个循环
2:
	/* Clear bss section */
	la a0, __bss_start       //a0 = __bss_start 初值为0的全局变量段起始地址   __bss_start定义在link连接脚本内
	la a1, _end              //a1 = _end 初值为0的全局变量段结束地址          _end定义在link连接脚本内
	bgeu a0, a1, 2f          //if( a0 >= a1 ) JUMP 2f 原则上不会出现__bss_start地址大于_end地址
                             //如果出现了则直接跳转到 2f 即下一个2标签
1:
	sw zero, (a0)            //*__bss_start = zero = 0      bss段清除为0
	addi a0, a0, 4           //a0 = a0 + 4   下一个ward
	bltu a0, a1, 1b          //if( a0 < a1 ) JUMP 1b 如果未到达_end则跳转到 1b 即上一个1标签，这里会完成一个循环

                             //程序执行到这里全局变量就以及处理完毕了
2:
	/*enable mcycle_minstret*/
    csrci CSR_MCOUNTINHIBIT, 0x5       //CSR_MCOUNTINHIBIT &= ~0x5 这里清零了第0bit和第1bit，使能了mcycle计数和minstret计数
                                       //csrci这条指令处理立即数5bit，列如0x5只是0B00101，高位不处理
                                       /*
     * Call vendor defined SystemInit to
     * initialize the micro-controller system
     */
    call SystemInit
	/* Call global constructors */
	la a0, __libc_fini_array           //a0 = __libc_fini_array       newlib中atexit函数的参数
	call atexit                        //调用newlib中的 void atexit(void (*func)(void)) 函数
                                       //功能为注册main函数结束后调用的函数，这里是__libc_fini_array
	call __libc_init_array             //调用newlib中的 void __libc_init_array (void) 函数
                                       //这里要注意__libc_init_array函数会调用_init的钩子函数，用来做c环境初始化前的一些硬件初始化
                                       //列如时钟的配置等，这里_init定义在init.c

                             //程序执行到这里C/C++环境就初始化完成了，可以进入main函数入口了
	/* argc = argv = 0 */
	li a0, 0                 //a0 = 0 main函数参数argc = 0
	li a1, 0                 //a1 = 0 main函数参数argv = 0
	call main                //调用 int main(int argc,char **argv) 函数
	tail exit                //main返回后调用newlib的exit函数, tail指令应该是不会返回的调用函数了

1:
	j 1b                       //1b 即上一次1标签，即跳转到自己，程序到这里就死循环了，原则不会运行到这里

	.global disable_mcycle_minstret
disable_mcycle_minstret:
        csrsi CSR_MCOUNTINHIBIT, 0x5     //关闭了mcycle计数和minstret计数
	ret

	.global enable_mcycle_minstret
enable_mcycle_minstret:
        csrci CSR_MCOUNTINHIBIT, 0x5     //使能了mcycle计数和minstret计数
	ret
