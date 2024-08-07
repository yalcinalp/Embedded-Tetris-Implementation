// ============================ //
// Do not edit this part!!!!    //
// ============================ //
// 0x300001 - CONFIG1H
#pragma config OSC = HSPLL      // Oscillator Selection bits (HS oscillator,
// PLL enabled (Clock Frequency = 4 x FOSC1))
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit
// (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit
// (Oscillator Switchover mode disabled)
// 0x300002 - CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bits (Brown-out
// Reset disabled in hardware and software)
// 0x300003 - CONFIG1H
#pragma config WDT = OFF        // Watchdog Timer Enable bit
// (WDT disabled (control is placed on the SWDTEN bit))
// 0x300004 - CONFIG3L
// 0x300005 - CONFIG3H
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator Enable bit
// (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled;
// RE3 input pin disabled)
// 0x300006 - CONFIG4L
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply
// ICSP disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit
// (Instruction set extension and Indexed
// Addressing mode disabled (Legacy mode))

#pragma config DEBUG = OFF      // Disable In-Circuit Debugger

#define KHZ 1000UL
#define MHZ (KHZ * KHZ)
#define _XTAL_FREQ (40UL * MHZ)

// ============================ //
//             End              //
// ============================ //

// ============================ //
// Creators                     //
// Alp Eren Yalcin (2522126)    //
// Batuhan Akcan   (2580181)    //
// Erencan Ceyhan  (2521342)    //
// ============================ //

#include <xc.h>

#define INTEGERIZE(arr) (unsigned char) ((arr[0] << 0) | (arr[1] << 1) | (arr[2] << 2) | (arr[3] << 3) | (arr[4] << 4) | (arr[5] << 5) | (arr[6] << 6) | (arr[7] << 7))
#define BITOF(variable, bit) ((variable) & (1 << (bit)))
#define TMR0L_INIT_VAL 0xCE
#define TMR0H_INIT_VAL 0x3C
#define SEGMENT7DELAY 3000
#ifndef bool // stdbool.h
#define bool unsigned char
#define true 1
#define false 0
#endif

#define LONG_MAX 4294967299

typedef enum State {
    LED_OFF,
    LED_ON,
} State;

enum Keys {
    KEY_RIGHT = 0,
    KEY_UP = 1,
    KEY_DOWN = 2,
    KEY_LEFT = 3,
    ARROW_KEY_COUNT = 4,
    KEY_ROTATE = 5,
    KEY_SUBMIT = 6,
    KEY_COUNT = 6,
};

typedef enum Type {
    POINT,
    SQUARE,
    L_SHAPED,
} Type;

typedef struct Position {
    unsigned char x;
    unsigned char y;
} Position;

typedef enum Rotation {
    BOTTOM_LEFT,
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_RIGHT,
} Rotation;

State grid[4][8];
bool keys[KEY_COUNT];
char counter250ms;
short blink_counter;
Position position;
Rotation rotation;
Type type;
bool PORTG_previous[ARROW_KEY_COUNT];
bool PORTG_current[ARROW_KEY_COUNT];
bool will_down;
unsigned char score;
unsigned char scoreDigit1;
unsigned char scoreDigit2;

void busy_wait();
void blink();
void init();
bool not_intersects();
void draw();
void rotate();
void submit();
bool canMove(Position position);
void move(Position newPosition);
void move_left();
void move_down();
void move_up();
void move_right();
void poll_g();
void reset();

void busy_wait() {
    for (long int i = 0; i <= 454544; i++); // 1 sec
    INTCON |= (1 << 7);
}

bool blink_on = true;

void blink() {
    counter250ms += 1;
    if (counter250ms == 8) {
        counter250ms = 0;
        will_down = true;
    }
    blink_counter = 0;
    blink_on = !blink_on;
}

bool prev_submit = false;
bool prev_rotate = false;

__interrupt(high_priority)
void HandleInterrupt() {
    __delay_ms(1);

    if (INTCONbits.T0IF) {
        INTCONbits.T0IF = 0;

        blink_counter += 1;
        if (blink_counter == 50) {
            blink();
        }

        TMR0L = TMR0L_INIT_VAL;
        TMR0H = TMR0H_INIT_VAL;
    }

    if (INTCONbits.RBIF) {
        INTCONbits.RBIF = 0;

        if (!prev_rotate && PORTBbits.RB5)
            rotate();
        if (!prev_submit && PORTBbits.RB6) {
            submit();
        }

        prev_rotate = PORTBbits.RB5;
        prev_submit = PORTBbits.RB6;
    }

    __delay_ms(1);
}

void init() {
    INTCON = (1 << 5) | (1 << 3);
    T0CON = (1 << 3) | (1 << 7);

    ADCON1 = 0x0F;

    TRISG = 0b00011101;
    TRISB = 0b01100000;
    TRISC = 0x00;
    TRISD = 0x00;
    TRISE = 0x00;
    TRISF = 0x00;
    TRISH = 0x00;
    TRISJ = 0x00;

    LATG = 0;
    LATB = 0;
    LATC = 0;
    LATD = 0;
    LATE = 0;
    LATF = 0;
    LATH = 0b00001111;
    LATJ = 0b00111111;

    TMR0L = TMR0L_INIT_VAL;
    TMR0H = TMR0H_INIT_VAL;

    reset();
}

bool not_intersects() {
    bool intersects = false;
    switch (type) {
        case POINT:
        {
            intersects = intersects || grid[position.x][position.y];
            break;
        }
        case SQUARE:
        {
            intersects = intersects || grid[position.x][position.y];
            intersects = intersects || grid[position.x + 1][position.y];
            intersects = intersects || grid[position.x][position.y + 1];
            intersects = intersects || grid[position.x + 1][position.y + 1];
            break;
        }
        case L_SHAPED:
        {
            switch (rotation) {
                case BOTTOM_LEFT:
                {
                    intersects = intersects || grid[position.x][position.y];
                    intersects = intersects || grid[position.x + 1][position.y];
                    intersects = intersects || grid[position.x + 1][position.y + 1];
                    break;
                }
                case TOP_LEFT:
                {
                    intersects = intersects || grid[position.x + 1][position.y];
                    intersects = intersects || grid[position.x][position.y + 1];
                    intersects = intersects || grid[position.x + 1][position.y + 1];
                    break;
                }
                case TOP_RIGHT:
                {
                    intersects = intersects || grid[position.x][position.y];
                    intersects = intersects || grid[position.x][position.y + 1];
                    intersects = intersects || grid[position.x + 1][position.y + 1];
                    break;
                }
                case BOTTOM_RIGHT:
                {
                    intersects = intersects || grid[position.x][position.y];
                    intersects = intersects || grid[position.x + 1][position.y];
                    intersects = intersects || grid[position.x][position.y + 1];
                    break;
                }
            }
            break;
        }
    }

    return !intersects;
}

void setLed(Position pos, bool value) {
    switch (pos.x) {
        case 0:
            if (value)
                LATC |= (1 << pos.y);
            else
                LATC &= ~(1 << pos.y);
            break;
        case 1:
            if (value)
                LATD |= (1 << pos.y);
            else
                LATD &= ~(1 << pos.y);
            break;
        case 2:
            if (value)
                LATE |= (1 << pos.y);
            else
                LATE &= ~(1 << pos.y);
            break;
        case 3:
            if (value)
                LATF |= (1 << pos.y);
            else
                LATF &= ~(1 << pos.y);
            break;
    }
}

void draw() {
    if (will_down) {
        move_down();
        will_down = false;
    }

    LATC = INTEGERIZE(grid[0]);
    LATD = INTEGERIZE(grid[1]);
    LATE = INTEGERIZE(grid[2]);
    LATF = INTEGERIZE(grid[3]);

    switch (type) {
        case POINT:
        {

            setLed((Position) {
                position.x, position.y
            }, blink_on);
            break;
        }
        case SQUARE:
        {

            setLed((Position) {
                position.x, position.y
            }, blink_on);

            setLed((Position) {
                position.x + 1, position.y
            }, blink_on);

            setLed((Position) {
                position.x, position.y + 1
            }, blink_on);

            setLed((Position) {
                position.x + 1, position.y + 1
            }, blink_on);
            break;
        }
        case L_SHAPED:
        {
            switch (rotation) {

                case BOTTOM_LEFT:
                    setLed((Position) {
                        position.x, position.y
                    }, blink_on);

                    setLed((Position) {
                        position.x + 1, position.y
                    }, blink_on);

                    setLed((Position) {
                        position.x + 1, position.y + 1
                    }, blink_on);
                    break;

                case BOTTOM_RIGHT:
                    setLed((Position) {
                        position.x, position.y
                    }, blink_on);

                    setLed((Position) {
                        position.x + 1, position.y
                    }, blink_on);

                    setLed((Position) {
                        position.x, position.y + 1
                    }, blink_on);
                    break;

                case TOP_LEFT:
                    setLed((Position) {
                        position.x + 1, position.y
                    }, blink_on);

                    setLed((Position) {
                        position.x, position.y + 1
                    }, blink_on);

                    setLed((Position) {
                        position.x + 1, position.y + 1
                    }, blink_on);
                    break;

                case TOP_RIGHT:
                    setLed((Position) {
                        position.x, position.y
                    }, blink_on);

                    setLed((Position) {
                        position.x, position.y + 1
                    }, blink_on);

                    setLed((Position) {
                        position.x + 1, position.y + 1
                    }, blink_on);
                    break;
            }
            break;
        }
    }
}

void rotate() {
    rotation = (rotation + 1) % 4;
}

void submit() {
    if (not_intersects()) {
        switch (type) {
            case POINT:
            {
                grid[position.x][position.y] = 1;
                score++;
                scoreDigit1 = score % 10;
                scoreDigit2 = (score / 10) % 10;
                break;
            }
            case SQUARE:
            {
                grid[position.x][position.y] = 1;
                grid[position.x + 1][position.y] = 1;
                grid[position.x][position.y + 1] = 1;
                grid[position.x + 1][position.y + 1] = 1;
                score += 4;
                scoreDigit1 = score % 10;
                scoreDigit2 = (score / 10) % 10;
                break;
            }
            case L_SHAPED:
            {
                switch (rotation) {
                    case BOTTOM_LEFT:
                    {
                        grid[position.x][position.y] = 1;
                        grid[position.x + 1][position.y] = 1;
                        grid[position.x + 1][position.y + 1] = 1;
                        break;
                    }
                    case TOP_LEFT:
                    {
                        grid[position.x + 1][position.y] = 1;
                        grid[position.x][position.y + 1] = 1;
                        grid[position.x + 1][position.y + 1] = 1;
                        break;
                    }
                    case TOP_RIGHT:
                    {
                        grid[position.x][position.y] = 1;
                        grid[position.x][position.y + 1] = 1;
                        grid[position.x + 1][position.y + 1] = 1;
                        break;
                    }
                    case BOTTOM_RIGHT:
                    {
                        grid[position.x][position.y] = 1;
                        grid[position.x + 1][position.y] = 1;
                        grid[position.x][position.y + 1] = 1;
                        break;
                    }
                }
                score += 3;
                scoreDigit1 = score % 10;
                scoreDigit2 = (score / 10) % 10;
                break;
            }
        }

        type = (type + 1) % 3;
        position = (Position){0, 0};
        rotation = 0;
    }
}

bool canMove(Position position) {
    unsigned char left, right, top, bot;
    switch (type) {
        case POINT:
        {
            left = right = position.x;
            top = bot = position.y;
            break;
        }
        case SQUARE:
        case L_SHAPED:
        {
            left = position.x;
            right = position.x + 1;
            top = position.y;
            bot = position.y + 1;
            break;
        }
    }
    return (left < 4 && right < 4 && top < 8 && bot < 8);
}

void move(Position newPosition) {
    if (canMove(newPosition)) {
        position = newPosition;
    }
}

void move_left() {
    Position newPosition = {position.x - 1, position.y};
    move(newPosition);
}

void move_right() {
    Position newPosition = {position.x + 1, position.y};
    move(newPosition);
}

void move_up() {
    Position newPosition = {position.x, position.y - 1};
    move(newPosition);
}

void move_down() {
    Position newPosition = {position.x, position.y + 1};
    move(newPosition);
}

void poll_g() {
    PORTG_current[KEY_RIGHT] = PORTGbits.RG0;
    PORTG_current[KEY_UP] = PORTGbits.RG2;
    PORTG_current[KEY_DOWN] = PORTGbits.RG3;
    PORTG_current[KEY_LEFT] = PORTGbits.RG4;

    if (PORTG_current[KEY_RIGHT] && !PORTG_previous[KEY_RIGHT])
        move_right();
    if (PORTG_current[KEY_UP] && !PORTG_previous[KEY_UP])
        move_up();
    if (PORTG_current[KEY_DOWN] && !PORTG_previous[KEY_DOWN])
        move_down();
    if (PORTG_current[KEY_LEFT] && !PORTG_previous[KEY_LEFT])
        move_left();

    for (char i = 0; i < ARROW_KEY_COUNT; i++) {
        PORTG_previous[i] = PORTG_current[i];
    }
}

void display_7segment() {
    switch (scoreDigit1) {
        case 0: LATJ = 0b00111111;
            break;
        case 1: LATJ = 0b00000110;
            break;
        case 2: LATJ = 0b01011011;
            break;
        case 3: LATJ = 0b01001111;
            break;
        case 4: LATJ = 0b01100110;
            break;
        case 5: LATJ = 0b01101101;
            break;
        case 6: LATJ = 0b01111101;
            break;
        case 7: LATJ = 0b00000111;
            break;
        case 8: LATJ = 0b01111111;
            break;
        case 9: LATJ = 0b01101111;
            break;
    }
    LATH = 1 << 3;
    _delay(SEGMENT7DELAY);

    switch (scoreDigit2) {
        case 0: LATJ = 0b00111111;
            break;
        case 1: LATJ = 0b00000110;
            break;
        case 2: LATJ = 0b01011011;
            break;
        case 3: LATJ = 0b01001111;
            break;
        case 4: LATJ = 0b01100110;
            break;
        case 5: LATJ = 0b01101101;
            break;
        case 6: LATJ = 0b01111101;
            break;
        case 7: LATJ = 0b00000111;
            break;
        case 8: LATJ = 0b01111111;
            break;
        case 9: LATJ = 0b01101111;
            break;
    }
    LATH = 1 << 2;
    _delay(SEGMENT7DELAY);


    LATJ = 0b00111111; // 0
    LATH = 1 << 0;
    _delay(SEGMENT7DELAY);

    LATJ = 0b00111111; // 1
    LATH = 1 << 1;
    _delay(SEGMENT7DELAY);
    LATH = 0b00000000;
}

void reset() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 8; j++) {
            grid[i][j] = LED_OFF;
        }
    }

    type = 0;
    position = (Position){0, 0};
    rotation = 0;

    for (int i = 0; i < ARROW_KEY_COUNT; i++) {
        PORTG_previous[i] = 0;
        PORTG_current[i] = 0;
    }

    score = 0;
    scoreDigit1 = 0;
    scoreDigit2 = 0;
    blink_counter = 0;
    counter250ms = 0;
    will_down = false;
}

// ============================ //
//            MAIN              //
// ============================ //

void main() {
    init();
    __delay_us(999860); // 140us delay from init()
    INTCON |= (1 << 7);

    while (1) {
        display_7segment();
        poll_g();
        display_7segment();
        draw();
        display_7segment();

        if (score == 32)
            reset();
        display_7segment();
    }
}
