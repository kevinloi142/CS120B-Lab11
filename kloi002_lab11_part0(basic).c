/*      Author: lab
 *  Partner(s) Name: Kevin Loi
 *      Lab Section:
 *      Assignment: Lab #  Exercise #
 *      Exercise Description: [optional - include for your own benefit]
 *
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 *
 *      Demo Link: 
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

unsigned char pattern1 = 0x00;
unsigned char row1 = 0x00;
unsigned char pattern2 = 0x00;
unsigned char row2 = 0x00;
unsigned char pattern3 = 0x20;
unsigned char row3 = 0xFB;
unsigned char pad1Pos[3] = {0x00, 0x00, 0x00};
unsigned char pad2Pos[3] = {0x00, 0x00, 0x00};
unsigned char movement = 1;

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerSet(unsigned long M){
        _avr_timer_M = M;
        _avr_timer_cntcurr = _avr_timer_M;
}

void TimerOn(){
        TCCR1B = 0x0B;
        OCR1A = 125;
        TIMSK1 = 0x02;
        TCNT1 = 0;

        _avr_timer_cntcurr = _avr_timer_M;

        SREG |= 0x80;
}

void TimerOff(){
        TCCR1B = 0x00;
}

void TimerISR(){
        TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect){
        _avr_timer_cntcurr--;
        if(_avr_timer_cntcurr == 0){
                TimerISR();
                _avr_timer_cntcurr = _avr_timer_M;
        }
}

typedef struct task {
        signed char state;
        unsigned long int period;
        unsigned long int elapsedTime;
        int (*TickFct)(int);
} task;

enum PaddleU_States{ ini, wait, left, right};
int PaddleUser(int state){
        unsigned char press = PINA & 0x03;
        unsigned char reset = ~PINA & 0x04;
        switch(state){
                case ini:
                        pattern1 = 0x01;
                        row1 = 0xF1;
                        pad1Pos[0] = 0xF7;
                        pad1Pos[1] = 0xFB;
                        pad1Pos[2] = 0xFD;
                        state = wait;
                        break;
                case wait:
                        if(press == 0x00 || press == 0x03){
                                state = wait;
                        } else if(press == 0x02){
                                state = right;
                        } else {
                                state = left;
                        }
                        break;
                case left:
                        if(press == 0x01){
                                state = left;
                        } else if(press == 0x02){
                                state = right;
                        } else {
                                state = wait;
                        }
                        break;
                case right:
                        if(press == 0x02){
                                state = right;
                        } else if(press == 0x01){
                                state = left;
                        } else {
                                state = wait;
                        }
                        break;
                default:
                        state = wait;
                        break;
        }
        switch(state){
                case ini:
                        break;
                case wait:
                        break;
                case left:
                        if(row1 != 0xE3){
                                if(row1 == 0xF8){
                                        pattern1 = 0x01;
                                        row1 = 0xF1;
                                        pad1Pos[0] = 0xF7;
                                        pad1Pos[1] = 0xFB;
                                        pad1Pos[2] = 0xFD;
                                } else {
                                        pattern1 = 0x01;
                                        row1 = 0xE3;
                                        pad1Pos[0] = 0xEF;
                                        pad1Pos[1] = 0xF7;
                                        pad2Pos[2] = 0xFB;
                                }
                        }
                        break;
                case right:
                        if(row1 != 0xF8){
                                if(row1 == 0xE3){
                                        pattern1 = 0x01;
                                        row1 = 0xF1;
                                        pad1Pos[0] = 0xF7;
                                        pad1Pos[1] = 0xFB;
                                        pad1Pos[2] = 0xFD;
                                } else {
                                        pattern1 = 0x01;
                                        row1 = 0xF8;
                                        pad1Pos[0] = 0xFB;
                                        pad1Pos[1] = 0xFD;
                                        pad1Pos[2] = 0xFE;
                                }
                        }
                        break;
                default:
                        break;
        }
        return state;
}

enum PaddleC_States{ init, AI }; //add more later
int PaddleComp(int state){
        unsigned char reset = ~PINA & 0x04;
        switch(state){
                case init:
                        pattern2 = 0x80;
                        row2 = 0xF1;
                        pad2Pos[0] = 0xF7;
                        pad2Pos[1] = 0xFB;
                        pad2Pos[2] = 0xFD;
                        state = AI;
                        break;
                case AI:
                        if((rand() % 4 + 1 > 2)){
                                if(row3 == 0xEF){
                                        if(row2 != 0xE3){
                                                if(row2 == 0xF1){
                                                        row2 = 0xE3;
                                                        pad2Pos[0] = 0xEF;
                                                        pad2Pos[1] = 0xF7;
                                                        pad2Pos[2] = 0xFB;
                                                } else {
                                                        row2 = 0xF1;
                                                        pad2Pos[0] = 0xF7;
                                                        pad2Pos[1] = 0xFB;
                                                        pad2Pos[2] = 0xFD;
                                                }
                                        }
                                } else if(row3 == 0xF7){
                                        if(row2 != 0xE3){
                                                if(row2 == 0xF1){
                                                        row2 = 0xE3;
                                                        pad2Pos[0] = 0xEF;
                                                        pad2Pos[1] = 0xF7;
                                                        pad2Pos[2] = 0xFB;
                                                } else {
                                                        row2 = 0xF1;
                                                        pad2Pos[0] = 0xF7;
                                                        pad2Pos[1] = 0xFB;
                                                        pad2Pos[2] = 0xFD;
                                                }
                                        }

                                } else if(row3 == 0xFB){
                                        if(row2 != 0xF1){
                                                if(row2 == 0xE3){
                                                        row2 = 0xF1;
                                                        pad2Pos[0] = 0xF7;
                                                        pad2Pos[1] = 0xFB;
                                                        pad2Pos[2] = 0xFD;
                                                }
                                        }

                                } else if(row3 == 0xFD){
                                        if(row2 != 0xF8){
                                                if(row2 == 0xF1){
                                                        row2 = 0xF8;
                                                        pad2Pos[0] = 0xFB;
                                                        pad2Pos[1] = 0xFD;
                                                        pad2Pos[2] = 0xFE;
                                                } else {
                                                        row2 = 0xF1;
                                                        pad2Pos[0] = 0xF7;
                                                        pad2Pos[1] = 0xFB;
                                                        pad2Pos[2] = 0xFD;
                                                }
                                        }
                                } else {
                                        if(row2 != 0xF8){
                                                if(row2 == 0xF1){
                                                        row2 = 0xF8;
                                                        pad2Pos[0] = 0xFB;
                                                        pad2Pos[1] = 0xFD;
                                                        pad2Pos[2] = 0xFE;
                                                } else {
                                                        row2 = 0xF1;
                                                        pad2Pos[0] = 0xF7;
                                                        pad2Pos[1] = 0xFB;
                                                        pad2Pos[2] = 0xFD;
                                                }
                                        }
                                }
                                state = AI;
                                break;
                        }
                default:
                        state = init;
                        break;
        }
        switch(state){
                case init:
                        break;
                default:
                        break;
        }
        return state;
}

//pad1Pos
//pad2Po2

unsigned char isLeft = 0x00;
unsigned char isRight = 0x01;
unsigned char isUpRight = 0x00;
unsigned char isUpLeft = 0x00;
unsigned char isDownRight = 0x00;
unsigned char isDownLeft = 0x00;
enum Ball_State{ move };
int Ball_Move(int state){
        unsigned char nextPat = 0x00;
        unsigned char nextRow = 0x00;
        unsigned char reset = ~PINA & 0x04;
        switch(state){
                case move:
                        state = move;
                        break;
                default:
                        state = move;
                        break;
        }
        switch(state){
                case move:
                        if(isLeft == 0x01){
                                nextPat = pattern3 << 1;
                                nextRow = row3;
                                if(nextPat == 0x80 && row2 == 0xF1){
                                        isLeft = 0x00;
                                        isRight = 0x01;
                                } else if(nextPat == 0x80 && row2 == 0xE3){
                                        isLeft = 0x00;
                                        isDownRight = 0x01;
                                        row3 = (row3 << 1) | 0x01;
                                } else if(nextPat == 0x80 && row2 == 0xF8){
                                        isLeft = 0x00;
                                        isUpRight = 0x01;
                                        row3 = (row3 >> 1) | 0x10;
                                } else {
                                        pattern3 <<= 1;
                                }
                        } else if(isRight == 0x01){
                                nextPat = pattern3 >> 1;
                                nextRow = row3;
                                if(nextPat == 0x01 && row1 == 0xF1){
                                        isLeft = 0x01;
                                        isRight = 0x00;
                                } else if(nextPat == 0x01 && row1 == 0xE3){
                                        isRight = 0x00;
                                        isDownLeft = 0x01;
                                        row3 = (row3 << 1) | 0x01;
                                } else if(nextPat == 0x01 && row1 == 0xF8){
                                        isRight = 0x00;
                                        isUpLeft = 0x01;
                                        row3 = (row3 >> 1) | 0x10;
                                } else {
                                        pattern3 >>= 1;
                                }
                        } else if(isUpRight == 0x01){
                                if(row3 == 0x1E){
                                        isUpRight = 0x00;
                                        nextRow = (row3 << 1) | 0x01;
                                        nextPat = pattern3 >> 1;
                                        if(nextPat == 0x01 && row1 == 0xF1){
                                                isUpLeft = 0x01;
                                        } else if(nextPat == 0x01 && row1 == 0xE3){
                                                isDownLeft = 0x01;
                                                row3 = (row3 << 2) | 0x03;
                                        } else {
                                                isDownRight = 0x01;
                                        }
                                } else {
                                        pattern3 >>= 1;
                                        row3 = (row3 >> 1) | 0x10;
                                        if(pattern3 == 0x01){
                                                if(row3 == pad1Pos[0] || row3 == pad1Pos[2]){
                                                        isUpRight = 0x00;
                                                        isDownLeft = 0x01;
                                                } else if(row3 == pad1Pos[1]){
                                                        isUpRight = 0x00;
                                                        isUpLeft = 0x01;
                                                } else {
                                                        row3 = 0x00;
                                                        pattern3 = 0x00;
                                                }
                                        }
                                        row3 = row3 & 0x1F;
                                }
                        } else if(isUpLeft == 0x01){
                                if(row3 == 0x1E){
                                        isUpLeft = 0x00;
                                        nextRow = (row3 << 1) | 0x01;
                                        nextPat = pattern3 << 1;
                                        if(nextPat == 0x80 && row2 == 0xF1){
                                                isUpRight = 0x01;
                                        } else if(nextPat == 0x80 && row2 == 0xE3){
                                                isDownRight = 0x01;
                                                row3 = (row3 << 2) | 0x03;
                                        } else {
                                                isDownLeft = 0x01;
                                        }
                                } else {
                                        row3 = (row3 >> 1) | 0x10;
                                        pattern3 <<= 1;
                                        if(pattern3 == 0x80){
                                                if(row3 == pad2Pos[0] || row3 == pad2Pos[2]){
                                                        isUpLeft = 0x00;
                                                        isDownRight = 0x01;
                                                } else if(row3 == pad2Pos[1]){
                                                        isUpLeft = 0x00;
                                                        isUpRight = 0x01;
                                                } else {
                                                        row3 = 0x00;
                                                        pattern3 = 0x00;
                                                }
                                        }
                                }
                                row3 = row3 & 0x1F;
                        } else if(isDownLeft == 0x01){
                                if(row3 == 0xEF){
                                        isDownLeft = 0x00;
                                        nextRow = (row3 >> 1) | 0x10;
                                        nextPat = pattern3 << 1;
                                        if(nextPat == 0x80 && row2 == 0xF1){
                                                isDownRight = 0x01;
                                        } else if(nextPat == 0x80 && row2 == 0xF8){
                                                isDownRight = 0x01;
                                        } else {
                                                isUpLeft = 0x01;
                                        }
                                } else {
                                        row3 = (row3 << 1) | 0x01;
                                        pattern3 <<= 1;
                                        if(pattern3 == 0x80){
                                                if(row3 == pad2Pos[0] || row3 == pad2Pos[2]){
                                                        isDownLeft = 0x00;
                                                        isUpRight = 0x01;
                                                } else if(row3 == pad2Pos[1]){
                                                        isDownLeft = 0x00;
                                                        isDownRight = 0x01;
                                                } else {
                                                        row3 = 0x00;
                                                        pattern3 = 0x00;
                                                }
                                        }
                                }
                        } else if(isDownRight == 0x01){
                                if(row3 == 0xEF){
                                        isDownRight = 0x00;
                                        nextRow = (row3 >> 1) | 0x10;
                                        nextPat = pattern3 >> 1;
                                        if(nextPat == 0x01 && row1 == 0xF1){
                                                isDownLeft = 0x01;
                                        } else if(nextPat == 0x01 && row1 == 0xF8){
                                                isUpLeft = 0x01;
                                                row3 = (row3 >> 1) | 0x10;
                                                row3 = (row3 >> 1) | 0x10;
                                        } else {
                                                isUpRight = 0x01;
                                        }
                                } else {
                                        row3 = (row3 << 1) | 0x01;
                                        pattern3 >>= 1;
                                        if(pattern3 == 0x01){
                                                if(row3 == pad1Pos[0] || row3 == pad1Pos[2]){
                                                        isDownRight = 0x00;
                                                        isUpLeft = 0x01;
                                                } else if(row3 == pad1Pos[1]){
                                                        isDownRight = 0x00;
                                                        isDownLeft = 0x01;
                                                } else {
                                                        row3 = 0x00;
                                                        pattern3 = 0x00;
                                                }
                                        }
                                }
                        } else {
                        }
                        break;
                default:
                        break;
        }
        return state;
}



enum Draw_State {  draw1, draw2, draw3 };
int Draw_Task(int state){
        switch(state){
                case draw1:
                        state = draw2;
                        break;
                case draw2:
                        state = draw3;
                        break;
                case draw3:
                        state = draw1;
                        break;
                default:
                        state = draw1;
                        break;
        }
        switch(state){
                case draw1:
                        PORTC = pattern1;
                        PORTD = row1;
                        break;
                case draw2:
                        PORTC = pattern2;
                        PORTD = row2;
                        break;
                case draw3:
                        PORTC = pattern3;
                        PORTD = row3;
                        break;
                default:
                        break;
        }
        return state;
}





int main(void){
    /* Insert DDR and PORT initializations */
        DDRA = 0x00; PORTA = 0xFF;
        DDRB = 0x00; PORTB = 0xFF;
        DDRC = 0xFF; PORTC = 0x00;
        DDRD = 0xFF; PORTD = 0x00;
    /* Insert your solution below */
        static task task1, task2, task3, task4;
        task *tasks[] = { &task1 , &task2, &task3, &task4};
        const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

        const char start = -1;

        task1.state = ini;
        task1.period = 250;
        task1.elapsedTime = task1.period;
        task1.TickFct = &PaddleUser;

        task2.state = init;
        task2.period = 250;
        task2.elapsedTime = task2.period;
        task2.TickFct = &PaddleComp;

        task3.state = move;
        task3.period = 250;
        task3.elapsedTime = task3.period;
        task3.TickFct = &Ball_Move;

        task4.state = draw1;
        task4.period = 2;
        task4.elapsedTime = task4.period;
        task4.TickFct = &Draw_Task;

        TimerSet(1);
        TimerOn();

        unsigned short i;
    while (1) {
        for( i = 0; i < numTasks; ++i){
                if(tasks[i]->elapsedTime == tasks[i]->period){
                        tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
                        tasks[i]->elapsedTime = 0;
                }
                tasks[i]->elapsedTime += 1;
        }
        while(!TimerFlag);
        TimerFlag = 0;
    }
    return 0;
}
