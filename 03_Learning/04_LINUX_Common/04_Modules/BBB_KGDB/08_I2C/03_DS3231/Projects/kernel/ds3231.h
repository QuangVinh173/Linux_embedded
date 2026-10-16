#ifndef BBB_DS3231_H
#define BBB_DS3231_H

/* *********** CONTROL >> ********** */
#define RX_TRIGGER 0
#define TX_TRIGGER 0
#define DELAY_MS 3000
 
#define IRQ_USED 1
/* *********** << CONTROL ********** */

#define BUG_CONTINUOUS_READ 0
/* 
    BUG_CONTINUOUS_READ:
    - When run read continuously 
    - I2C_DATA is not ready to be read from
        -> the data in there is not valid

    SOLUTION:
    - Add checking ARDY at repeated start
*/

#endif