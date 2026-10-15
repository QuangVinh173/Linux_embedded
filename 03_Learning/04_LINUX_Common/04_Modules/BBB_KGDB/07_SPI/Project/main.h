#ifndef MAIN_H
#define MAIN_H

/* ################# [SPI0_TX] ################# */
/* USER CONTROL */
#define PRINT_DEBUG_TX 1 /* Print debug info print */

/* ################# [SPI1_RX] ################# */
/* USER CONTROL */
#define PRINT_DEBUG_RX 1 /* Print debug info print */


#define BUG_STRAY_BIT 0
/* 
    BUG_STRAY_BIT:
    - When terminate tx application, the re-run tx app with re-run rx app
    - When release and init, there 1 CLK edge and CS active low for a moment
    - RX is actively listenning during this moment, so 1 dirty bit is sent to shift register
    - When new 8-bit data comes, only this dirty bit (MSB) and 7-bits of data (LSBs) 
       is transfered from shift reg to data reg RX0

    SOLUTION:
    - Release CS (idle high) + DMA request
    - Remain EPOL/POL/PHA

*/

#define BUG_ONE_BYTE_RECEIVED 0
/* 
    BUG_ONE_BYTE_RECEIVED:
    - When run tx using DMA and rx using CPU
    - Because TX dma streams data very quickly, so RX FIFO is filled faster than
        got from CPU -> the num of bytes is always > threshold to generate IRQ
    - But IRQ is triggered by edge case interrupt (edge = from < threshold to > threshold)
        => IRQ only is triggered once 

    SOLUTION:
    - Create drained FIFO loop to continousely read data from FIFO as data is filled from
        TX DMA very quickly
    - On the same board, but one problem is raised here when running FIFO draining when TX CPU
    - As CPU is stuck at drained FIFO loop, there is no chance for CPU TX to transmit
        -> there is no transmit from TX -> stuck
        -> jiffies is useless here as the action in Hard IRQ, others (timer interrupt for jiffies)
            is not updated -> dead lock
        => SOLUTION:
          - Use empty reg to check whether when draining data is done
          - Add wait_event_interruptible_timeout to send SIGNAL or timeout efficiently

*/

#endif
