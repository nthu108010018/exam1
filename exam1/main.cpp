#include "mbed.h"

#include "uLCD_4DGL.h"

using namespace std::chrono;

EventQueue queue_wave(32 * EVENTS_EVENT_SIZE);
EventQueue queue_menu(32 *EVENTS_EVENT_SIZE);
Thread thread;


uLCD_4DGL uLCD(D1, D0, D2); // serial tx, serial rx, reset pin;
InterruptIn up(D13);
InterruptIn down(D11);
InterruptIn confirm(D12);
DigitalOut out(D10);
AnalogOut Aout(PA_4);
AnalogIn Ain(A0);
Thread t;
void fup();

void fdown();

void flagchange();
void sampling(int rate);
void wave_gen(AnalogOut &aout, AnalogIn &ain, float adcData[], int stime, int choice);
void menu(int i);

int i = 0;
int flag = 0;
float ADCdata[128] = {0};

int main(){
    uLCD.printf("\nHello uLCD World\n");
    uLCD.printf("\n  Starting Demo...");
    out = 1;
    menu(i);
    
    while(1){
        /*switch(selections){
            case 0x4 : // down
              if(i >= 3){
                  i=0;
                  uLCD.cls();
                  menu(i);
              } 
              else{
                  i++;
                  uLCD.cls();
                  menu(i);
              }
              ThisThread::sleep_for(10ms);
              break;
            case 0x2 : flag =1 ;break;
            case 0x1 : // up
              if(i <= 0){
                  i = 3;
                  uLCD.cls();
                  menu(i);
              } 
              else{
                  i--;
                  uLCD.cls();
                  menu(i);
              }
              ThisThread::sleep_for(10ms);
              break;
        }*/
        ThisThread::sleep_for(10ms);
        t.start(callback(&queue_menu, &EventQueue::dispatch_forever));
        up.rise(fup);
        down.rise(fdown);
        confirm.rise(flagchange);
        
        if(flag ==1){
            queue_wave.call(&sampling, 128);
            Thread eventThread(osPriorityNormal);
            eventThread.start(callback(&queue_wave,&EventQueue::dispatch_forever));
            
            if(i == 0){
                wave_gen(Aout, Ain,  ADCdata, 800, 1);
            }
            else if(i == 1){
                wave_gen(Aout, Ain,  ADCdata, 400, 2);
            }
            else if(i == 2){
                wave_gen(Aout, Ain,  ADCdata, 200, 3);
            }
            else if(i == 3){
                wave_gen(Aout, Ain,  ADCdata, 100, 4);
            }
        }
    }
}
    


void menu(int i){
    uLCD.cls();
    uLCD.text_width(2);
    uLCD.text_height(2);
    const char *options[4] = { "1", "1/2", 
                             "1/4", "1/8" };
    for(int j = 0; j<4; j++){
        uLCD.locate(1, j+2);
        if(i == j){
            uLCD.color(RED);
            uLCD.printf(options[j]);
            uLCD.printf("\n");
            uLCD.color(GREEN);
        }
        else{
            uLCD.printf(options[j]);
            uLCD.printf("\n");
        }
    }
}

void wave_gen(AnalogOut &aout, AnalogIn &ain, float adcData[],  int stime, int choice){
    
    int w_time = 80 *choice;
    while(1){
        int i =0;
        
        while(i<91){
            aout = 0.01*i;
            i = i+1;
            wait_us(stime);
        }
        ThisThread::sleep_for(w_time*1ms);
        while(i>0){
            aout = 0.01*i;
            i = i-1;
            wait_us(stime);
        }
    }
   
}


void sampling(int rate){
    while(1){
        for(int i=0; i<128; i++){
            ADCdata[i] = Ain*3.3;
            ThisThread::sleep_for(1000ms/rate);
        }
        for(int i=0; i<128;i++){
            printf("%f\r\n", ADCdata[i]);
        }
        ThisThread::sleep_for(5000ms);
    }
}
void fup(){
    if(i >= 3){
        i=0;
        queue_menu.call(&menu, i);
} 
    else{
        i++;
        queue_menu.call(&menu, i);
    }
}

void fdown(){
    if(i <= 0){
        i = 3;
        queue_menu.call(&menu, i);
} 
    else{
        i--;
        queue_menu.call(&menu, i);
    }
}
void flagchange(){
    flag = 1;
}