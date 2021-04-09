#include <math.h>
// 16 point FFT
#define N     16
#define PI    3.141
 
#define RCC_GPIO_ROW RCC_APB2Periph_GPIOA
#define RCC_GPIO_COL RCC_APB2Periph_GPIOB
#define GPIO_ROW GPIOA
#define GPIO_COL GPIOB
#define GPIO_PIN_ROW_0 GPIO_Pin_4
#define GPIO_PIN_ROW_1 GPIO_Pin_5
#define GPIO_PIN_ROW_2 GPIO_Pin_6
#define GPIO_PIN_ROW_3 GPIO_Pin_7
#define GPIO_PIN_ROW_4 GPIO_Pin_8
#define GPIO_PIN_ROW_5 GPIO_Pin_9
#define GPIO_PIN_ROW_6 GPIO_Pin_10
#define GPIO_PIN_ROW_7 GPIO_Pin_11
#define GPIO_PIN_COL_0 GPIO_Pin_8
#define GPIO_PIN_COL_1 GPIO_Pin_9
#define GPIO_PIN_COL_2 GPIO_Pin_10
#define GPIO_PIN_COL_3 GPIO_Pin_11
#define GPIO_PIN_COL_4 GPIO_Pin_12
#define GPIO_PIN_COL_5 GPIO_Pin_13
#define GPIO_PIN_COL_6 GPIO_Pin_14
#define GPIO_PIN_COL_7 GPIO_Pin_15
 
volatile uint16_t adc_value = 0;
volatile uint8_t n_count = 0;
volatile uint8_t n_done = 0;
int REX[N];
int IMX[N];
uint16_t MAG[N];
uint8_t led_buf[8];
 
void init_adc(void);
void init_timer(void);
void init_pwm(void);
void init_led_matrix(void);
uint16_t read_adc(void);
void write_pwm(uint16_t val);
void led_matrix_update(void);
void fft(void);
void mag_to_buf(void);

void TIM3_IRQHandler()
{
    static uint8_t s = 0;
    static uint8_t l = 0;
  
    // TIM3 interrupt at 35.15kHz
    if (TIM_GetITStatus(TIM3, TIM_IT_Update))
    {
        // Read ADC value (10-bit PWM)
        adc_value = read_adc() >> 2;
        // Write to PWM (audio loopback)
        write_pwm(adc_value);
 
        // Sampling N point FFT at 17.5kHz
        s++;
        if (s >= 2)
        {
            if (n_done == 0)
            {
                REX[n_count++] = adc_value;
 
                if (n_count >= N)
                {
                    n_done = 1;
                    n_count = 0;  
                }
            }
            s = 0;
        }
 
        // LED matrix scanning at 1kHz
        l++;
        if (l >= 35)
        {
            led_matrix_update();
            l = 0;
        }
 
        // Clears the TIM3 interrupt pending bit
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }
}
 
int main(void)
{
    init_adc();
    init_timer();
    init_pwm();
    init_led_matrix();
 
    while (1)
    {
        // Wait until sampling is done
        while (!n_done);
        fft();
        mag_to_buf();
        n_done = 0;
    }
}
