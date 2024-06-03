

/**
 点击按键BTNC，16位LED灯的低8位（LD[7:0]）即时显示对应低8位独立开关（sw[7:0]）的输出，高8位LED灯熄灭；
 点击按键BTNL一次，显示的LED灯串往左移一位，当移到最高位时，再次回到最低位显示；
 举例：开关为01100001时，LED[15:0]显示为灭灭灭灭灭灭灭灭 灭亮亮灭 灭灭灭亮；按L一次，LED灯整体左移一位
 */

#include <stdio.h>
#include "xil_printf.h"
#include "xil_io.h"
#include "xgpio_l.h"

// 16个LED和开关的定义
#define SWITCHES_BASEADDR XPAR_AXI_GPIO_0_BASEADDR
#define LEDS_BASEADDR XPAR_AXI_GPIO_0_BASEADDR

// 按钮的定义
#define BTNS_BASEADDR XPAR_AXI_GPIO_2_BASEADDR
#define BTNC_MASK 0x01 // BTNC
#define BTNL_MASK 0x04 // BTNL

int main() {
    unsigned short currentSwitchState;
    unsigned short led;  // LED显示
    unsigned short currentBtn, lastBtn, realBtn;

    // 初始化GPIO
    Xil_Out16(SWITCHES_BASEADDR + XGPIO_TRI_OFFSET, 0xffff);  // 设置开关为输入
    Xil_Out16(LEDS_BASEADDR + XGPIO_TRI2_OFFSET, 0x0);  // 设置LED为输出
    Xil_Out16(BTNS_BASEADDR + XGPIO_TRI_OFFSET, 0x1f);  // 设置按钮为输入

    while (1) {
        currentSwitchState = Xil_In16(SWITCHES_BASEADDR + XGPIO_DATA_OFFSET) & 0xffff;
        currentBtn = Xil_In8(BTNS_BASEADDR + XGPIO_DATA_OFFSET) & 0x1f;
        if (currentBtn) {  // 按钮被按下
            while (currentBtn) {  // 按钮未释放
                currentBtn = Xil_In8(BTNS_BASEADDR + XGPIO_DATA_OFFSET) & 0x1f;
                realBtn = (currentBtn ^ lastBtn) & lastBtn;  // 获取被按下的按钮
                lastBtn = currentBtn;
                if (realBtn) {
                    break;
                }
            }
            // 判断哪个按钮被按下
            switch (realBtn) {
                case BTNC_MASK:  // BTNC按下
                    led = currentSwitchState & 0x00FF;  // 低8位显示
                    break;

                case BTNL_MASK:  // BTNL按下
					 if (led & 0x8000) {  // 如果最高位有1
                         led = (led << 1) | 1;  // 左移一位，并将最低位置1
					} else {
						led = led << 1;  // 否则只左移一位
					}
					break;
            }
        }

        // 输出到LED
        Xil_Out16(LEDS_BASEADDR + XGPIO_DATA2_OFFSET, led);
    }

    return 0;
}






