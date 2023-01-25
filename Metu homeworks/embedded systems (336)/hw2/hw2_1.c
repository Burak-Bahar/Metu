/* 
 * File:   hw2_1.c
 * Author: hobbits_to_isengard
 *
 * Created on 04 May?s 2022 Çar?amba, 17:05
 */

#pragma config OSC = HSPLL
#include <xc.h>
#include <stdint.h>
#define _XTAL_FREQ 40000000
// Preload value to generate a 1KHz overflow for 16bit Timer0
int level_init = 104;// This is 65536-39062,5  //for 500ms 180-76
//for 400 ms 247-61 // for 300 ms 57-45
                       // 500ms 104-256       //400 ms 12-127  // for
//300ms  73-127
int count_cap = 256;
int t_count =0;
int level;
int note_count;
int health;
unsigned int initial_timer1 = 0x0000;
unsigned int high;
unsigned int low;
int wait;
int exit_flag = 0;
unsigned char L = 0x38;
unsigned char O = 0x3F; //0
unsigned char S = 0x6D; //5
unsigned char E = 0x79;
unsigned char n = 0x54;
unsigned char d = 0x5E;

unsigned int randomed = 0x0000;
unsigned int ander =0x0007;
int light = 0x00;

unsigned char health_points
[]={0x3F,0x30,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F}; // global
#define digit1 0x01
#define digit2 0x02
#define digit3 0x04
#define digit4 0x08

int input_compare_flag = 0;
int input_G = 0x00;
int note_generate_flag;
int RA = 0x00;
int decrease_health_flag = 0 ;
int start_flag;

void init_ports() {
    TRISA = 0x00; // Port A is all OUTPUTS
    TRISB = 0x00;
    TRISC = 0x01;
    TRISD = 0x00;
    TRISE = 0x00;
    TRISF = 0x00;
    TRISG = 0x0F;
    TRISJ = 0x00; //define PORTD as a output pin
    PORTJ =0X00;   // initialize PORTD pins to active low
    TRISH =0X00;
    PORTA = 0x00; // Output 0 for now.
    PORTB = 0x00;
    PORTC = 0x00;
    LATC = 0x00;
    PORTD = 0x00;
    PORTE = 0x00;
    PORTF = 0x00;
    PORTG = 0x00;    LATG = 0x00;
}

void iterate_notes(){
    PORTF = LATE;
    PORTE = LATD;
    PORTD = LATC;
    PORTC = LATB;
    PORTB = LATA;
    LATA = RA;
}

void tmr0_int() {
    INTCONbits.T0IF = 0; // Acknowledge interrupt
    iterate_notes();
    TMR0 = level_init & 0xff;
    //if(health <= 0){ exit_flag =1;}
    if(note_count != 0){ note_count--;} //{generate_note();}
    else{ PORTA = 0x00;}
    ADCON0bits.GO = 1; // Start next acquisition
}
int timer_flag = 0;
int health_check = 0;
void __interrupt(high_priority) highPriorityISR(void) {
    if(INTCONbits.T0IE) {
        if(t_count < count_cap){
            t_count++;
            TMR0 = level_init & 0xff;
            INTCONbits.T0IF = 0;
            ADCON0bits.GO = 1;

        }
        else{
            t_count=0;
            if(start_flag == 1){
                if(LATF > 0 && input_G == 0x00 ){
                    health -=1;
                }
                tmr0_int();
                input_compare_flag = 1;

                input_G = 0x00;
                note_generate_flag = 1;
            }
        }
    }
}
void __interrupt(low_priority) lowPriorityISR(void) {}


void init_timer() {
      // 8 bits, 128 prescaler. Timer will be turned on after interrupts are
    // initialized
    T0CON = 0x46; //0000 0110 -- 128 prescaler
    TMR0 = level_init & 0xff;
    T1CON = 0x00;
}
void init_interrupts() {
    INTCON = 0x00;  // 1010 0000
    INTCONbits.TMR0IE = 1;// Enable TMR0 interrupt
    INTCONbits.GIE = 1;
}



void DELAY_ms(unsigned int ms_Count)
{
    unsigned int i,j;
    for(i=0;i<ms_Count;i++)
    {
        for(j=0;j<10;j++);
    }
}


int l2_over = 0;


void set_timer_task(){
    if(level == 1){
            if(note_count == 0){
                wait=1;
            }
            if(LATF == 0x00 && LATA == 0x00 && wait == 1 && note_count == 0){
              level = 2;}
        }
        if (level == 2){
            if(LATF == 0x00 && LATA == 0x00 && l2_over == 1 && note_count == 0){
              level = 3;
            }
            if(LATF == 0x00 && LATA == 0x00 && wait == 1 && l2_over == 0){
                note_count = 10;
                wait = 0;
                level_init = 12;
                count_cap = 127;
            }
            if( wait == 0 && note_count == 0){
                wait = 1; l2_over = 1;}
        }
        if (level == 3 ){
            if(LATF == 0x00 && LATA == 0x00 && wait == 1){
                note_count = 15;
                wait = 0;
                level_init = 73;
            }

            if( wait == 0 && note_count == 0 && LATF == 0x00 && LATA == 0x00){
               health= -2;
               //exit_flag = 1;
            } //game finished
        }

}



void compare_results(){
    if((input_G ^ LATF) == 0x00){
        decrease_health_flag = 0;

    }
    else{
        decrease_health_flag = 1;
    }
}
void input_task(){
    if(PORTG != 0x00){
        input_G = PORTG;
        if(input_compare_flag){
            compare_results();
            if(decrease_health_flag && health > 0){
                health -=1;
                decrease_health_flag = 0;
            }
        input_compare_flag = 0;
        }
    }
}

int a = 0x0000;
void rightRotate(unsigned int n)
{
    if(n%2==1){
       initial_timer1 = (n / 2)+ 0x8000;
    }
    else{
       initial_timer1 = n / 2;
    }

}
void read_tmr1(){
    low = TMR1L;
    high = TMR1H;
    initial_timer1 = high;
    initial_timer1 = (initial_timer1<<8)|low;
}


void note_generate_task(){
    if(note_generate_flag){
        note_generate_flag = 0;
            randomed = initial_timer1 & ander;
            light = randomed % 5;
            switch(light){
                case 0:
                    RA = 1;
                    break;
                case 1:
                    RA = 0x02;

                    break;
                case 2:
                    RA= 0x04;

                    break;
                case 3:
                    RA= 0x08;

                    break;
                case 4:
                    RA = 0x10;

                    break;
            }
            rightRotate(initial_timer1);
            if(level == 2){rightRotate(initial_timer1);rightRotate(initial_timer1);}
            if(level == 3){rightRotate(initial_timer1);rightRotate(initial_timer1);rightRotate(initial_timer1);rightRotate(initial_timer1);}
        }
}



void display_task(){
    switch(health){
            case(9):
                PORTH = digit1;
                PORTJ = health_points[9];
                if(level == 1){
                    DELAY_ms(1);
                    PORTH = digit4;
                    PORTJ = health_points[1];
                }
                if(level == 2){
                    DELAY_ms(1);
                    PORTH = digit4;
                    PORTJ = health_points[2];
                }
                if(level == 3){
                    DELAY_ms(1);
                    PORTH = digit4;
                    PORTJ = health_points[3];
                }
                break;
            case(8):
                PORTH = digit1;
                PORTJ = health_points[8];
                if(level == 1){
                    DELAY_ms(1);
                    PORTH = digit4;
                    PORTJ = health_points[1];
                }
                if(level == 2){
                    DELAY_ms(1);
                    PORTH = digit4;
                    PORTJ = health_points[2];
                }
                if(level == 3){
                    DELAY_ms(1);
                    PORTH = digit4;
                    PORTJ = health_points[3];
                }
                break;
            case(7):
                PORTH = digit1;
                PORTJ = health_points[7];

                if(level == 1){
                    DELAY_ms(1);
                    PORTH = digit4;
                    PORTJ = health_points[1];
                }
                if(level == 2){
                    DELAY_ms(1);
                    PORTH = digit4;
                    PORTJ = health_points[2];
                }
                if(level == 3){
                    DELAY_ms(1);
                    PORTH = digit4;
                    PORTJ = health_points[3];
                }
                break;
            case(6):
                PORTH = digit1;
                PORTJ = health_points[6];
                if(level == 1){
                    DELAY_ms(1);
                    PORTH = digit4;
                    PORTJ = health_points[1];
                }
                if(level == 2){
                    DELAY_ms(1);
                    PORTH = digit4;
                    PORTJ = health_points[2];
                }
                if(level == 3){
                    DELAY_ms(1);
                    PORTH = digit4;
                    PORTJ = health_points[3];
                }
                break;
            case(5):
                PORTH = digit1;
                PORTJ = health_points[5];
                if(level == 1){
                    DELAY_ms(1);
                    PORTH = digit4;
                    PORTJ = health_points[1];
                }
                if(level == 2){
                    DELAY_ms(1);
                    PORTH = digit4;
                    PORTJ = health_points[2];
                }
                if(level == 3){
                    DELAY_ms(1);
                    PORTH = digit4;
                    PORTJ = health_points[3];
                }
                break;
            case(4):
                PORTH = digit1;
                PORTJ = health_points[4];
                if(level == 1){
                    DELAY_ms(1);
                    PORTH = digit4;
                    PORTJ = health_points[1];
                }
                if(level == 2){
                    DELAY_ms(1);
                    PORTH = digit4;
                    PORTJ = health_points[2];
                }
                if(level == 3){
                    DELAY_ms(1);
                    PORTH = digit4;
                    PORTJ = health_points[3];
                }
                break;
            case(3):
                PORTH = digit1;
                PORTJ = health_points[3];
                if(level == 1){
                    DELAY_ms(1);
                    PORTH = digit4;
                    PORTJ = health_points[1];
                }
                if(level == 2){
                    DELAY_ms(1);
                    PORTH = digit4;
                    PORTJ = health_points[2];
                }
                if(level == 3){
                    DELAY_ms(1);
                    PORTH = digit4;
                    PORTJ = health_points[3];
                }
                break;
            case(2):
                PORTH = digit1;
                PORTJ = health_points[2];
                if(level == 1){
                    DELAY_ms(1);
                    PORTH = digit4;
                    PORTJ = health_points[1];
                }
                if(level == 2){
                    DELAY_ms(1);
                    PORTH = digit4;
                    PORTJ = health_points[2];
                }
                if(level == 3){
                    DELAY_ms(1);
                    PORTH = digit4;
                    PORTJ = health_points[3];
                }
                break;
            case(1):
                PORTH = digit1;
                PORTJ = health_points[1];
                if(level == 1){
                    DELAY_ms(1);
                    PORTH = digit4;
                    PORTJ = health_points[1];
                }
                if(level == 2){
                    DELAY_ms(1);
                    PORTH = digit4;
                    PORTJ = health_points[2];
                }
                if(level == 3){
                    DELAY_ms(1);
                    PORTH = digit4;
                    PORTJ = health_points[3];
                }
                break;
            case(0): 
                PORTH = digit1;
                PORTJ = L;
                DELAY_ms(1);
                PORTH = digit2;
                PORTJ = O;
                DELAY_ms(1);
                PORTH = digit3;
                PORTJ = S;
                DELAY_ms(1);
                PORTH = digit4;
                PORTJ = E;
                DELAY_ms(1);
                break;
            case(-1): //lose the game
                PORTH = digit1;
                PORTJ = L;
                DELAY_ms(1);
                PORTH = digit2;
                PORTJ = O;
                DELAY_ms(1);
                PORTH = digit3;
                PORTJ = S;
                DELAY_ms(1);
                PORTH = digit4;
                PORTJ = E;
                DELAY_ms(1);
                break;
            case(-2): //win the game so the end //can be written where win
                PORTH = digit1;
                PORTJ = E;
                DELAY_ms(1);
                PORTH = digit2;
                PORTJ = n;
                DELAY_ms(1);
                PORTH = digit3;
                PORTJ = d;
                DELAY_ms(1);
                break;
           case(10): 
               break;
               
}
}


void start_system() {
    INTCONbits.GIE = 1;// Enable global interrupts
    //T0CONbits.TMR0ON = 1; // Start Timer 0 after bla bla is
    T1CONbits.TMR1ON = 1;
    health = 10;
    start_flag = 0;
}

void check_rc0(){
    if(PORTC == 0x01 && start_flag == 0){
        health = 9;
        note_count = 5;
        level = 1;
        wait = 0;
        note_generate_flag =1;
        read_tmr1();
        start_flag = 1;
        TRISC = 0x00;
        T0CONbits.TMR0ON = 1;
    }
}
void init_all(){

    init_ports();
    init_timer();
    init_interrupts();
    start_system();

}
int main() {
    init_all();
    //
    while(1){//buralar biryerlere ba?langiÃ§ inputunu al
        check_rc0();
        if(start_flag == 1){
            set_timer_task();
            note_generate_task();
            input_task();
            display_task();
            if (health <= 0){ // end de health'i -2 yapt?m, -1 ve 0 da lose oluyor
                start_flag = 0;
                LATA = 0x00; // Output 0 for now.
                LATB = 0x00;
                LATC = 0x00;
                LATD = 0x00; // Output 0 for now.
                LATE = 0x00;
                LATF = 0x00;
            }
        }
        else{ // program ended until RC0 pressed again display end or lose
            display_task();
            TRISC = 0x01;
            PORTC = 0x00;
        }

    }
    return 0;

}
