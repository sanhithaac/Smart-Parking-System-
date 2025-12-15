#include "stm32f4xx.h"                   // Device header

// Global Variables
uint32_t adc_value = 0; // Variable to hold ADC value

// Flags to track gate state
uint8_t gate1_opened = 0;  // Flag for Gate 1 (Sensor 0)
uint8_t gate2_opened = 0;  // Flag for Gate 2 (Sensor 1)

void GPIO_Config(void);
void Timer4_PWM_Init(void);
void Set_Servo_Angle(uint8_t angle);
void Delay_ms(uint32_t ms);
void ADC_Config(void);
uint32_t ADC_Read(uint8_t channel);
void GPIOC_Init(void);
void GPIOA_Init(void);
void BluetoothUSART_Init(void);
char BluetoothUSART_Read(void);
void BluetoothUSART_Write(char c);
void BluetoothUSART_SendString(char *str);
uint8_t stable_read_pa_bit(uint32_t bitmask);

int main(void) {
    GPIO_Config();    
    Timer4_PWM_Init();
    ADC_Config(); // Configure ADC
    GPIOC_Init(); // Initialize GPIO for LED
    GPIOA_Init(); // Initialize GPIO for USART2 TX and RX
    BluetoothUSART_Init(); // Initialize USART2 for Bluetooth

    uint8_t current_angle = 0; // Track current servo angle (gate position)
    uint8_t sensor0_state = stable_read_pa_bit(GPIO_IDR_ID0); // Initialize to current state of PA0 (incoming)
    uint8_t sensor1_state = stable_read_pa_bit(GPIO_IDR_ID1); // Initialize to current state of PA1 (outgoing)

    while (1) {
        // Read the state of each sensor
        uint8_t new_sensor0_state = stable_read_pa_bit(GPIO_IDR_ID0); // Read PA0 for incoming
        uint8_t new_sensor1_state = stable_read_pa_bit(GPIO_IDR_ID1); // Read PA1 for outgoing

        // Logic for handling Sensor 0 (incoming cars)
        if (new_sensor0_state != sensor0_state) { // Only act on state change
            sensor0_state = new_sensor0_state; // Update the state

            if (sensor0_state) { // If Sensor 0 detects motion (incoming car)
                if (!gate1_opened) { // Check if Gate 1 has not been opened
                    if (current_angle != 90) {
                        Set_Servo_Angle(90); // Lift Gate 1 for incoming
                        current_angle = 90;
                    }
                    BluetoothUSART_SendString("Gate 1 closed.\r\n");
                    gate1_opened = 1;  // Set the flag to indicate Gate 1 is opened
                }
            } else { // If no motion detected by Sensor 0 (incoming car cleared)
                if (gate1_opened) {  // If Gate 1 was opened previously
                    if (current_angle == 90) {
                        Set_Servo_Angle(0); // Drop Gate 1
                        current_angle = 0;
                    }
                    BluetoothUSART_SendString("Gate 1 opened for outgoing car.\r\n");
                    gate1_opened = 0;  // Reset the flag to indicate Gate 1 is closed
                }
            }
        }

        // Logic for handling Sensor 1 (outgoing cars)
        if (new_sensor1_state != sensor1_state) { // Only act on state change
            sensor1_state = new_sensor1_state; // Update the state

            if (sensor1_state) { // If Sensor 1 detects motion (outgoing car)
                if (!gate2_opened) { // Check if Gate 2 has not been opened
                    if (current_angle != 90) {
                        Set_Servo_Angle(90); // Lift Gate 2 for outgoing
                        current_angle = 90;
                    }
                    BluetoothUSART_SendString("Gate 2 closed .\r\n");
                    gate2_opened = 1;  // Set the flag to indicate Gate 2 is opened
                }
            } else { // If no motion detected by Sensor 1 (outgoing car cleared)
                if (gate2_opened) {  // If Gate 2 was opened previously
                    if (current_angle == 90) {
                        Set_Servo_Angle(0); // Drop Gate 2
                        current_angle = 0;
                    }
                    BluetoothUSART_SendString("Gate 2 opened for outgoing car.\r\n");
                    gate2_opened = 0;  // Reset the flag to indicate Gate 2 is closed
                }
            }
        }

        // If both sensors detect objects, keep the gate open
        if (sensor0_state && sensor1_state) {
            if (current_angle != 90) {
                Set_Servo_Angle(90); // Keep the gate open if both sensors are triggered
                current_angle = 90;
            }
            BluetoothUSART_SendString("Both gates closed for incoming and outgoing cars.\r\n");
            gate1_opened = 1; // Mark both gates as open
            gate2_opened = 1;
        }

        // Read ADC value (from a connected analog sensor)
        adc_value = ADC_Read(10); // Read from ADC channel 10 (PC0 or any configured ADC channel)

        Delay_ms(100); // Small delay to prevent rapid toggling
    }
}


void GPIO_Config(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // Enable GPIOA clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; // Enable GPIOC clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; // Enable GPIOB clock

    // Configure PA0 and PA1 as input for IR sensors (Gate 1 and Gate 2)
    GPIOA->MODER &= ~GPIO_MODER_MODER0; // Set PA0 as input (Gate 1)
    GPIOA->MODER &= ~GPIO_MODER_MODER1; // Set PA1 as input (Gate 2)
    
    // Enable pull-up resistors for PA0 and PA1
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR0 | GPIO_PUPDR_PUPDR1);
    GPIOA->PUPDR |=  (GPIO_PUPDR_PUPDR0_0 | GPIO_PUPDR_PUPDR1_0); // pull-up on PA0, PA1

    // Configure PC13 as output for LED indication
    GPIOC->MODER &= ~GPIO_MODER_MODER13; // Clear mode bits for PC13
    GPIOC->MODER |= GPIO_MODER_MODER13_0; // Set PC13 as output

    // Set PC13 as Push-Pull and No Pull-Up/Down
    GPIOC->OTYPER &= ~GPIO_OTYPER_OT13;
    GPIOC->PUPDR &= ~GPIO_PUPDR_PUPDR13;

    // Configure PB6 as alternate function for PWM (TIM4_CH1)
    GPIOB->MODER &= ~GPIO_MODER_MODER6; // Clear mode bits for PB6
    GPIOB->MODER |= GPIO_MODER_MODER6_1; // Set PB6 to alternate function mode
    GPIOB->AFR[0] |= (2 << 24); // Set AF2 (TIM4) for PB6
}

void Timer4_PWM_Init(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; // Enable TIM4 clock

    // Set prescaler and auto-reload values for 50Hz PWM frequency
    TIM4->PSC = 15; // Prescaler (for 1 MHz timer frequency)
    TIM4->ARR = 20000 - 1; // Auto-reload for 20 ms period (50Hz PWM)

    // Configure PWM mode for Channel 1 (PB6)
    TIM4->CCMR1 |= (6 << 4); // Set OC1M to PWM mode 1 (bits 5:4)
    TIM4->CCMR1 |= TIM_CCMR1_OC1PE; // Enable preload for channel 1

    // Enable output for channel 1
    TIM4->CCER |= TIM_CCER_CC1E; // Enable output for channel 1

    TIM4->CR1 |= TIM_CR1_CEN; // Enable TIM4
}

void ADC_Config(void) {
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; // Enable ADC1 clock

    // Configure ADC1
    ADC1->CR1 &= ~ADC_CR1_RES; // Set resolution to 12 bits
    ADC1->SQR3 = 10; // Set first conversion to channel 10 (PC0)
    ADC1->CR2 |= ADC_CR2_ADON; // Enable ADC
    Delay_ms(1); // Delay for ADC stabilization
}

uint32_t ADC_Read(uint8_t channel) {
    ADC1->SQR3 = channel; // Set the desired channel
    ADC1->CR2 |= ADC_CR2_SWSTART; // Start the conversion

    while (!(ADC1->SR & ADC_SR_EOC)); // Wait for conversion to complete
    return ADC1->DR; // Return the ADC value
}

void Set_Servo_Angle(uint8_t angle) {
    uint32_t pulseWidth = (angle * 2000 / 180) + 500; // Calculate pulse width for servo (500-2500us)

    TIM4->CCR1 = pulseWidth; // Set duty cycle for servo
    Delay_ms(20); // Allow time for servo to reach position
}
    
void Delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms; i++) {
        for (uint32_t j = 0; j < 2500; j++) { // Reduced from 4000 to 2500 for shorter delays
            __NOP(); // NOP instruction for delay
        }
    }
}

void GPIOC_Init(void) {
    RCC->AHB1ENR |= (1 << 2);  // Enable clock for GPIOC
    GPIOC->MODER &= ~(3 << (13 * 2));  // Clear mode bits for PC13
    GPIOC->MODER |= (1 << (13 * 2));   // Set PC13 to output mode
}

void GPIOA_Init(void) {
    RCC->AHB1ENR |= (1 << 0); // Enable GPIOA clock
    GPIOA->MODER &= ~(3 << 4); // Clear mode bits for PA2 and PA3
    GPIOA->MODER |= (2 << 4); // Set PA2 to alternate function mode
    GPIOA->MODER |= (2 << 6); // Set PA3 to alternate function mode
    GPIOA->AFR[0] |= (7 << 8) | (7 << 12); // Set PA2 and PA3 to AF7 (USART2)
}

void BluetoothUSART_Init(void) {
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN; // Enable USART2 clock
    USART2->BRR = 0x0683; // 9600 baud rate for 16 MHz clock
    USART2->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE; // Enable TX, RX, USART
}

char BluetoothUSART_Read(void) {
    while (!(USART2->SR & USART_SR_RXNE)); // Wait for RXNE to be set
    return USART2->DR; // Return received data
}

void BluetoothUSART_Write(char c) {
    while (!(USART2->SR & USART_SR_TXE)); // Wait for TXE to be set
    USART2->DR = c; // Send character
}

void BluetoothUSART_SendString(char *str) {
    while (*str) {
        BluetoothUSART_Write(*str++);
    }
}

uint8_t stable_read_pa_bit(uint32_t bitmask) {
    uint8_t v1 = (GPIOA->IDR & bitmask) ? 1 : 0;
    Delay_ms(3);
    uint8_t v2 = (GPIOA->IDR & bitmask) ? 1 : 0;
    return (v1 == v2) ? v1 : ((GPIOA->IDR & bitmask) ? 1 : 0);
}
