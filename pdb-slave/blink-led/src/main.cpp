#include <mbed.h>
#include <LPC1768_pindefs.h>

Serial pc(USBTX, USBRX, 9600);

DigitalOut led1(LPC_LED1);
DigitalOut led2(LPC_LED2);
DigitalOut led3(LPC_LED3);
DigitalOut led4(LPC_LED4);

int main(){
    pc.printf("Hello world!\r\n");
    led1 = true;
    led2 = true;
    led3 = true;
    led4 = true;
    while(1){
        wait(1);
        led1 = !led1;
        led2 = !led2;
        led3 = !led3;
        led4 = !led4;
    }
}