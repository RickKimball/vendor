/*
   marching_ws281x_pixels.ino - DMA/TIMER based ws281x/neopixel pixel routines

   Generate a ws281x compatible pulse train using PWM timer and DMA
   provided duty cycle data.  Connect pin PA1 to the DIN of a ws281x
   led strip and watch as Blue Green Red White pixels march.

   This code written for a bluepill (stm32f103c8 china board) using
   the official STM core.

   Author: rick kimball

   Log:
   2018-11-25: modify to work with the STM core
   2018-07-11: tweaks to reduce size and style
   2018-07-10: converted to fabooh core and register only calls
               using stm32f103xb.h from stm32cube headers
   2018-07-07: simplified using RGB values, compute T0/T1 based on F_CPU
   2018-07-05: Initial version
*/

#define sizeofs(a) (sizeof(a)/sizeof(a[0]))

static const unsigned LED_CNT = 4;  // I'm using a 4 pixel led strip, change to suit

// pixel pulse constants
typedef uint8_t pulse_t;                              // pulse times are less than 255
static const uint32_t HZ = 800000;                    // ws281x pixel period is 800kHz
static const uint16_t T_PRE = (F_CPU / HZ / 255) + 1; // 1,  but we show the math here
static const uint16_t TT = (F_CPU / T_PRE / HZ);      // ~1.25us 800kHz 89 @ 72MHz
static const uint16_t T1 = (pulse_t)((float)TT*.56f); // ~700ns         50 @ 72MHz
static const uint16_t T0 = (pulse_t)((float)TT*.28f); // ~350ns         25 @ 72MHz

/*
    DMA_BUF_SIZE - LED_CNT*24+1

    one 8bit pwm pulse time for each RGB pixel bit (4 leds * 3 bytes for RGB * 8 bits) +
    one zero value pwm pulse to make sure the PA1 pins stays low at completion
*/
static const unsigned DMA_BUF_SIZE = (LED_CNT * 3 * 8) + 1;

//----------------------------------------------------------------------
// global variables

pulse_t dma_buffer[DMA_BUF_SIZE]; // pixel bit timing array

static const int led_pin = LED_BUILTIN;            // built-in PC13 pin of bluepill
static const int ws281x_pin = PA1;
static bool led_pin_state;

//----------------------------------------------------------------------
// rearrage the order of the red and green order to make ws281x happy

#define RGB(r,g,b) g,r,b

void setup() {
  ws281x_pin_alt_output(); // set PA1 to Push Pull Alternate IO

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LED_GREEN_OFF);
}

void loop() {
  // animation frames - marching B,W,R,G colors
  static const uint8_t frames[4][LED_CNT * 3] = {
    { RGB(0, 0, 0x1f), RGB(0x1f, 0x1f, 0x1f), RGB(0x1f, 0, 0), RGB(0, 0x1f, 0) },
    { RGB(0x1f, 0x1f, 0x1f), RGB(0x1f, 0, 0), RGB(0, 0x1f, 0), RGB(0, 0, 0x1f) },
    { RGB(0x1f, 0, 0), RGB(0, 0x1f, 0), RGB(0, 0, 0x1f), RGB(0x1f, 0x1f, 0x1f) },
    { RGB(0, 0x1f, 0), RGB(0, 0, 0x1f), RGB(0x1f, 0x1f, 0x1f), RGB(0x1f, 0, 0) }
  };

  // send each frame of pixel data to the led strip, and pause
  for (unsigned indx = 0; indx < sizeofs(frames); ++indx) {

    // fill the DMA pulse time array for each pixel bit using animation data
    init_dma_buffer(&frames[indx][0], dma_buffer, sizeofs(frames[0]));

    // kick off the timer, transfer a frame of pe
    dma_start();

    // wait for DMA completion flag or abort after 100msec timeout
    // this normally should take ~120 usecs for 4 leds
    const uint32_t m = millis();
    while ( !(DMA1->ISR & DMA_ISR_TCIF7) ) {
      if ((millis() - m) > 100) {
        break;
      }
    }

    // kill the timer, kill DMA
    on_dma_complete();

    // a > 50us delay causes the ws281x to load the pixels we just sent
    delay(250);
  }
}

//----------------------------------------------------------------------
//

void init_dma_buffer(const uint8_t * const src_pixel_data, pulse_t * dst_pulse_data, unsigned cnt)
{
  unsigned bufindx = 0;

  // take the led Green Red Blue values and turn convert them into
  // timer pwm pulse values depending on the 8 bit values for each color
  // store in the dma array we will transmit on the DIN ws281x pin from PA1
  for (unsigned led = 0; led < cnt ; ++led) {

    unsigned bindx = 8;
    do {
      dst_pulse_data[bufindx++] = (src_pixel_data[led] & (1 << --bindx)) ? T1 : T0;
    } while (bindx);
  }
}

//------------------------------------------------------
// enum for TIM2 DMA Control Register Base Address constants

enum {
  TIM_DCR_DBA_CR1,
  TIM_DCR_DBA_CR2,
  TIM_DCR_DBA_SMCR,
  TIM_DCR_DBA_DIER,
  TIM_DCR_DBA_SR,
  TIM_DCR_DBA_EGR,
  TIM_DCR_DBA_CCMR1,
  TIM_DCR_DBA_CCMR2,
  TIM_DCR_DBA_CCER,
  TIM_DCR_DBA_CNT,
  TIM_DCR_DBA_PSC,
  TIM_DCR_DBA_ARR,
  TIM_DCR_DBA_RCR,
  TIM_DCR_DBA_CCR1,
  TIM_DCR_DBA_CCR2,
  TIM_DCR_DBA_CCR3,
  TIM_DCR_DBA_CCR4,
  TIM_DCR_DBA_BDTR
};

//------------------------------------------------------
// DMA stuff using the STM32Cube device header only
// all baremetal register manipulation
//
// PA1 - TIM2 CH2, DMA CH7

void dma_start() {
  pulse_t *src = dma_buffer;
  volatile uint32_t * dst = &TIM2->DMAR;
  uint32_t cnt = DMA_BUF_SIZE;

  //------------------------------------------------------------------
  // PA1 - DMA CH7 setup

  // Disable so we can config it
  DMA1_Channel7->CCR = 0;

  DMA1_Channel7->CCR  |= 0
                         // Set DMA priority level to very high
                         | (0b11 << DMA_CCR_PL_Pos)
                         // Set memory transfer size to 8-bits
                         | ( sizeof(src[0]) == 1 ? (0b00 << DMA_CCR_MSIZE_Pos) : (0b01 << DMA_CCR_MSIZE_Pos))
                         // Set peripheral transfer size to 16-bits
                         | (0b01 << DMA_CCR_PSIZE_Pos)
                         // Enable memory increment mode
                         | DMA_CCR_MINC
                         // Data direction
                         //  1 - Read from memory
                         | (0b1 << DMA_CCR_DIR_Pos);

  // setup number of bytes to copy, will be zero when done
  DMA1_Channel7->CNDTR = cnt;

  // Peripheral address (destination)
  DMA1_Channel7->CPAR = (uint32_t)dst;

  // Memory address (source)
  DMA1_Channel7->CMAR = (uint32_t)src;

  // clear DMA1_Channel7 interrupt flags
  DMA1->IFCR |= DMA_IFCR_CGIF7 | DMA_IFCR_CTCIF7 | DMA_IFCR_CHTIF7 | DMA_IFCR_CTEIF7;

  //------------------------------------------------------------------
  // PA1 - TIM2 CH2 setup

  TIM2->CR1 = 0;                            // reset/disable timer
  TIM2->CR1 = TIM_CR1_ARPE | TIM_CR1_URS;   // preload ARR value

  TIM2->PSC = T_PRE - 1; // prescaler of 1
  TIM2->ARR = TT - 1;   // auto reload register period of ~1.25us
  TIM2->CCR2 = 0;       // capture compare channel 2 duty cycle of 0
  // this sets PA1 pin to LOW at start

  TIM2->CCMR1 |= 0
                 | TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 // (0b110) CCR2 PWM mode 1
                 | TIM_CCMR1_OC2PE;                    // (0b1)   CCR2 preload enabled

  // output capture compare channel 2 enabled, pulse is active high
  TIM2->CCER |= TIM_CCER_CC2E;

  TIM2->DIER |= TIM_DIER_CC2DE; // enable DMA request for channel 2

  TIM2->DCR |= 0
               | (1 - 1) << TIM_DCR_DBL_Pos // set DMA burst length to 1 16bits
               | TIM_DCR_DBA_CCR2;        // set DMA burst base addresss to CCR2

  // turn on DMA CH7
  DMA1_Channel7->CCR |= DMA_CCR_EN;

  // turn on the timer
  TIM2->CR1 |= TIM_CR1_CEN;
}

//----------------------------------------------------------------------
//

void on_dma_complete()
{
  // turn off the timer
  TIM2->CR1 &= ~TIM_CR1_CEN;

  // turn off DMA CH7
  DMA1_Channel7->CCR &= ~DMA_CCR_EN;

  led_pin_state ^= 1;
  
  digitalWrite(led_pin, (led_pin_state ? LED_GREEN_ON : LED_GREEN_OFF));
}

//----------------------------------------------------------------------
// configure PA1 as a TIMER with DMA as an Alternate PushPull Output

void ws281x_pin_alt_output() {
  // make sure all the clocks we are using are enabled
  // AFIO, PA, TIM2, DMA
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN;
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
  RCC->AHBENR  |= RCC_AHBENR_DMA1EN;
  
  GPIO_InitTypeDef GPIO_InitStruct;
  
  /**TIM2 GPIO Configuration
    PA1     ------> TIM2_CH2
  */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}
