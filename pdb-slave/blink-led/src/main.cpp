#include <mbed.h>

// Serial pc(Tx, Rx, 128000);
DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);

int main(){
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