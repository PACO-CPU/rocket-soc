/******************************************************************************
 * @file
 * @copyright Copyright 2015 GNSS Sensor Ltd. All right reserved.
 * @author    Sergey Khabarov - sergeykhbr@gmail.com
 * @brief     Boot procedure of copying FW image into SRAM with the debug
 *            signals.
******************************************************************************/

#include <string.h>
#include <stdio.h>
#include "axi_maps.h"
#include "encoding.h"

#define BLOCK_SIZE 8

static const int FW_IMAGE_SIZE_BYTES = 1 << 18;

void led_set(int output) {
    ((gpio_map *)ADDR_NASTI_SLAVE_GPIO)->led = output;
}

void print_uart(const char *buf, int sz) {
    uart_map *uart = (uart_map *)ADDR_NASTI_SLAVE_UART1;
    for (int i = 0; i < sz; i++) {
        while (uart->status & UART_STATUS_TX_FULL) {}
        uart->data = buf[i];
    }
}

void read_uart(char *buf, int sz) {
    uart_map *uart = (uart_map *)ADDR_NASTI_SLAVE_UART1;
    for (int i = 0; i < sz; i++) {
        while (uart->status & UART_STATUS_RX_FULL) {}
        buf[i]=(char)uart->data;
    }
}

void copy_image() { 
    uint32_t tech;
    uint64_t *fwrom = (uint64_t *)ADDR_NASTI_SLAVE_FWIMAGE;
    uint64_t *sram = (uint64_t *)ADDR_NASTI_SLAVE_SRAM;
    pnp_map *pnp = (pnp_map *)ADDR_NASTI_SLAVE_PNP;

    led_set(0xff);
    /** Speed-up RTL simulation by skipping coping stage: */
    tech = pnp->tech & 0xFF;
    if (tech != TECH_INFERRED) {
        memcpy(sram, fwrom, FW_IMAGE_SIZE_BYTES);
    }
    led_set(0x81);

#if 0
    /** Just to check access to DSU and read MCPUID via this slave device.
     *  Verification is made on time diagram (ModelSim), no other purposes of 
     *  these operations.
     *        DSU base address = 0x80080000: 
     *        CSR address: Addr[15:4] = 16 bytes alignment
     *  3296 ns - reading (iClkCnt = 409)
     *  3435 ns - writing (iClkCnt = 427)
     */
    uint64_t *arr_csrs = (uint64_t *)0x80080000;
    uint64_t x1 = arr_csrs[CSR_MCPUID<<1]; 
    pnp->fwdbg1 = x1;
    arr_csrs[CSR_MCPUID<<1] = x1;
#endif
}

static void _memclr(char *p,size_t n) {
  while(n--) *p++=0;
}

void load_uart_image() {
    uint32_t sz_total;
    size_t sz_block;
    uint32_t offs=0;
    char block[BLOCK_SIZE];
    char *sram = (char*)ADDR_NASTI_SLAVE_SRAM;
    
    read_uart((char*)&sz_total,sizeof(sz_total));
    
    while(sz_total>offs) {
        sz_block=
            ((sz_total-offs)>=BLOCK_SIZE)
            ? BLOCK_SIZE
            : sz_total;
        _memclr(block+sz_block,BLOCK_SIZE-sz_block);
        read_uart(block,sz_block);
        memcpy(sram+offs, block, BLOCK_SIZE); 
        offs+=sz_block;
    }
    

}


void _init() {
    uint32_t tech;
    

    pnp_map *pnp = (pnp_map *)ADDR_NASTI_SLAVE_PNP;
    uart_map *uart = (uart_map *)ADDR_NASTI_SLAVE_UART1;
    gpio_map *gpio = (gpio_map *)ADDR_NASTI_SLAVE_GPIO;
    // Half period of the uart = Fbus / 115200 / 2 = 70 MHz / 115200 / 2:
    uart->scaler = 304;
    
    if (gpio->dip&0x02) {
      print_uart("Boot (firmware) ..",18);
      copy_image();
      print_uart("OK\r\n",4);
    } else {
      print_uart("Boot (uart ready)\r\n",19);
      load_uart_image();
    }

    /** Check ADC detector that RF front-end is connected: */
    tech = (pnp->tech >> 24) & 0xff;
    if (tech != 0xFF) {
        print_uart("ADC clock not found. Enable DIP int_rf.\r\n", 41);
    }
}

/** Not used actually */
int main() {
    while (1) {}

    return 0;
}
