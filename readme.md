# 智能锁项目笔记

## 目录

[前置](##前置)

[其他](##其他)



## 前置

### **嵌入式项目中，逐个功能实现和搭建框架后再逐个功能实现的差别，不搭建框架的话，会时刻影响前面的代码，耦合度太高？**

**直接逐个功能实现的方式：**

1. 容易造成代码高度耦合。比如实现串口功能时直接写死了数据处理逻辑，后续添加其他功能时可能需要大量修改原有代码。
2. 代码复用性差。类似的功能可能会在不同地方重复编写，导致代码冗余且难以维护。
3. 扩展性受限。新增功能时经常需要修改现有代码，容易引入bug。

**先搭建框架再实现功能的方式：**

1. 通过良好的架构设计(如分层架构)，实现各模块解耦。比如：
   - 硬件抽象层(HAL)处理底层硬件
   - **驱动层处理具体外设**
   - **应用层实现业务逻辑**
2. 定义清晰的接口，使各模块之间通过标准接口通信，而不是直接调用。
3. 便于功能扩展和维护：
   - 新增功能时只需实现相应接口
   - 修改某个模块不会影响其他模块
   - 可以方便地复用已有模块

举个例子：假设要实现一个带显示功能的数据采集系统。

不使用框架的做法可能是：

```
void main() {
    while(1) {
        // 直接在一起写采集和显示逻辑
        data = adc_read();
        lcd_display(data);
    }
}
```

使用框架的做法：

```c
// 数据采集模块
typedef struct {
    void (*init)(void);
    uint16_t (*read)(void);
} Sensor_t;

// 显示模块
typedef struct {
    void (*init)(void);
    void (*display)(uint16_t);
} Display_t;

void main() {
    Sensor_t sensor;
    Display_t display;
    
    // 初始化各模块
    sensor.init();
    display.init();
    
    while(1) {
        uint16_t data = sensor.read();
        display.display(data);
    }
}
```

**经过搭建框架，可以实现分层结构？驱动层和应用层？两者的代码不一样吗？**

驱动层(Driver Layer):

1. 主要负责硬件的直接操作
2. 提供硬件的基础功能接口
3. 代码特点:

```c
// 驱动层示例 - UART驱动
typedef struct {
    void (*init)(uint32_t baudrate);
    void (*send_byte)(uint8_t data);
    uint8_t (*receive_byte)(void);
    void (*enable_rx_interrupt)(void);
} UART_Driver_t;

// 具体实现
void UART_Init(uint32_t baudrate) {
    // 配置GPIO引脚
    GPIO_InitTypeDef gpio_init;
    gpio_init.Mode = GPIO_MODE_AF_PP;
    HAL_GPIO_Init(...);
    
    // 配置UART寄存器
    USART1->BRR = SystemCoreClock/baudrate;
    USART1->CR1 |= USART_CR1_UE | USART_CR1_TE;
}
```

应用层(Application Layer):

1. 实现具体的业务逻辑
2. 调用驱动层提供的接口
3. 代码特点:

```c
// 应用层示例 - 温度监控应用
typedef struct {
    float temp_threshold;
    void (*process_temp)(float temp);
    void (*alarm_handler)(void);
} TempMonitor_t;

// 具体实现
void TempMonitor_Process(float temp) {
    // 使用驱动层的接口发送数据
    char buf[32];
    sprintf(buf, "Temperature: %.1f\r\n", temp);
    
    // 通过UART发送温度数据
    for(int i = 0; buf[i]; i++) {
        uart.send_byte(buf[i]);
    }
    
    // 业务逻辑处理
    if(temp > temp_threshold) {
        alarm_handler();
    }
}
```

主要区别:

1. 关注点不同
   - **驱动层关注如何操作硬件(寄存器配置等)**
   - **应用层关注业务流程(数据处理、控制逻辑等)**
2. 依赖关系
   - 驱动层依赖硬件抽象层(HAL)
   - 应用层依赖驱动层提供的接口
3. 可移植性
   - 驱动层与硬件平台紧密相关
   - 应用层相对独立，更容易移植

这样的分层设计有助于:

- 代码复用(不同产品可以复用相同的驱动)
- 便于测试(可以单独测试各层功能)
- 提高可维护性(修改某层代码不影响其他层)

### 现在嵌入式系统开发方式有三种，寄存器，标准库，hal库他们之间有什么区别？

寄存器级开发：

```c
// 直接操作寄存器的例子 - 配置GPIO
GPIOA->MODER |= (1 << 10);  // 设置PA5为输出模式
GPIOA->ODR |= (1 << 5);     // PA5输出高电平
```

特点：

- 直接操作硬件寄存器，**执行效率最高**
- **代码量小，程序空间占用少**
- 需要深入理解硬件架构和寄存器功能
- **可移植性差，不同芯片的寄存器定义不同**
- 开发周期长，**容易出错**

标准库开发：

```c
// 使用标准库的例子 - 配置GPIO
GPIO_InitTypeDef GPIO_InitStructure;
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
GPIO_Init(GPIOA, &GPIO_InitStructure);
GPIO_SetBits(GPIOA, GPIO_Pin_5);
```

特点：

- 对寄存器操作进行了封装
- 相比寄存器级开发更容易理解和使用
- **仍需要了解部分底层细节**
- **可移植性一般，同系列芯片可以复用**
- **执行效率好，代码量适中**

HAL库开发：

```c
// 使用HAL库的例子 - 配置GPIO
GPIO_InitTypeDef GPIO_InitStruct = {0};
GPIO_InitStruct.Pin = GPIO_PIN_5;
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
GPIO_InitStruct.Pull = GPIO_NOPULL;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
```

特点：

- 最高层次的抽象，**屏蔽了硬件细节**
- 代码可读性好，容易上手
- **可移植性最好，跨平台开发方便**
- 执行效率较低，代码量大
- **适合快速开发，不太注重性能的场合**

选择建议：

1. 性能要求高的项目（如高速数据采集）：选择寄存器级开发
2. 平衡开发效率和性能：选择标准库开发
3. 注重开发效率和可移植性：选择HAL库开发

实际应用中可以混合使用：

```c
// 混合使用的例子
void Init_Device(void) {
    // 普通GPIO用HAL库初始化
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // 高速处理的部分用寄存器直接操作
    GPIOB->BSRR = GPIO_PIN_5;
}
```

### 库函数的学习重点

- 程序实现思路很重要，其他都可以使用库函数实现。

- 如果不能理解库函数参数的含义，需要知道根据库函数溯源到对应的寄存器位，根据手册知道具体的含义。
- 比如常见的串口配置思路：   
  - 串口初始化： 
    - GPIO控制器配置 
    - 串口控制器配置 
    - 用于中断接收的NVIC配置
  - 串口控制器的DMA使能可以在对应DMA功能函数时单独开启，这样就不用修改原来的代码了。

## 更新字库后想通过软件复位重新加载字库

在 STM32F401（以及其他基于 Cortex-M 的 STM32 系列）中，软件复位一般是通过系统控制寄存器（SCB，System Control Block）实现的。SCB 寄存器是 Cortex-M 内核的一部分，而不是外设寄存器，因此在参考手册中找不到专门的复位寄存器。

你可以通过调用 CMSIS 提供的内核函数，或者直接操作 SCB 寄存器来实现复位。

### **使用 CMSIS 内核函数**

CMSIS 提供了标准化的函数，直接调用即可完成复位操作。这个函数是：

```c
#include "stm32f4xx.h"

void NVIC_SystemReset(void);
```

调用方式：

```c
int main(void) {
    // 初始化代码...

    // 执行软件复位
    NVIC_SystemReset();

    while (1) {
        // 不会执行到这里，因为系统已复位
    }
}
```

原理

`NVIC_SystemReset()` 函数底层操作 SCB 寄存器中的 `AIRCR`（Application Interrupt and Reset Control Register），设置复位位（`SYSRESETREQ` 位）来触发系统复位。

### 直接操作 SCB 寄存器（手册上没有，也不好配置）

如果不使用 CMSIS 函数，也可以直接手动配置 `SCB->AIRCR` 来实现复位。操作步骤如下：

```c
#include "stm32f4xx.h"

#define AIRCR_VECTKEY_MASK    ((uint32_t)0x05FA0000) // 写入密钥值

void software_reset(void) {
    SCB->AIRCR = AIRCR_VECTKEY_MASK | (1 << SCB_AIRCR_SYSRESETREQ_Pos);
    while (1); // 等待复位
}

int main(void) {
    // 初始化代码...

    // 执行软件复位
    software_reset();

    while (1) {
        // 不会执行到这里，因为系统已复位
    }
}
```

注意

1. `AIRCR_VECTKEY_MASK` 是一个写保护密钥，写入时必须使用 `0x5FA`，否则写入无效。
2. 设置 `SYSRESETREQ` 位 (`1 << SCB_AIRCR_SYSRESETREQ_Pos`) 即触发软件复位。
3. 复位后，所有外设和系统状态都会恢复为初始状态，类似于硬件复位。

------

总结

如果你使用的是官方的 STM32 HAL 库或标准库，直接调用 `NVIC_SystemReset()` 是最简单和推荐的方式。如果你想更底层地操作，可以直接配置 SCB 寄存器。两种方法效果完全相同。

## 中途修改看门狗的重载值

```c
void iwdg_init(void)
{
    //时钟源配置-开启低速时钟
    RCC->CSR |= (1U<<0);
    //等待时钟稳定
    while(!(RCC->CSR & (1U<<1)));
    //解除写保护
    IWDG->KR = 0x5555;
    //设置分频为128分频,根据具体情况确定时间，32 kHz 低速内部 RC (LSI RC)，该 RC 用于驱动独立看门狗
    //250次/s
    IWDG->PR = 5;     //直接赋值更清晰
    //重装载值设置为2500 (超时时间约10秒)
    IWDG->RLR = 2500;
    //重载到计数器,任何时候将关键字 0xAAAA 写到 IWWDG_KR 寄存器中， 
    //IWDG_RLR 的值就会被重载到计数器，从而避免产生看门狗复位。
    IWDG->KR = 0xAAAA;//此时写保护再次开启
    //启动看门狗
    IWDG->KR = 0xCCCC;
    
    printf("看门狗启动\r\n");
}


//独立喂狗程序
void iwdg_feed(void)
{
    IWDG->KR = 0xAAAA;
}


//若要更改重载值， IWDG_SR 中的 RVU 位必须为 0。
//RVU： 看门狗计数器重载值更新 (Watchdog counter reload value update)
//可通过硬件将该位置 1 以指示重载值正在更新。当在 VDD 电压域下完成重载值更新操作后
//（需要多达 5 个 RC 40 kHz 周期）毫秒级，会通过硬件将该位复位。
//重载值只有在 RVU 位为 0 时才可更新。直接修改重载值可能会失败，因为没有确认上一次更新是否完成
//IWDG_PR 和 IWDG_RLR 寄存器具有写访问保护。若要修改寄存器，必须首先对 IWDG_KR
//寄存器写入代码 0x5555。而写入其他值则会破坏该序列，从而使寄存器访问保护再次生效。
//这意味着重装载操作（即写入 0xAAAA）也会启动写保护功能。   
//解除写保护
IWDG->KR = 0x5555;
while(IWDG->SR & (1U<<1));    // 等待之前的重载值加载完成，避免新重载值被覆盖或者错乱
IWDG->RLR = 250;
IWDG->KR = 0xAAAA;
```

## 两种串口发送判断的标志的区别

标志位区别:

- `usart6_send_byte`: 使用 TC (Transmission Complete) 标志位
- `USART6_SendChar`: 使用 TXE (Transmit Data Register Empty) 标志位

检查时机:

- TC: 检查整个数据帧(包括起始位、数据位、停止位)是否完全发送完成
- TXE: 只检查发送数据寄存器是否为空,可以写入新数据

各自优势:

使用TC标志 (`usart6_send_byte`):

- 可靠性更高,确保数据完全发送
- 适合在以下场景使用:
  - UART关闭前的最后发送
  - 更改波特率前
  - 进入低功耗模式前
  - 对时序要求严格的场合

使用TXE标志 (`USART6_SendChar`):

- 发送效率更高,尤其是连续发送时
- 适合在以下场景使用:
  - 普通数据传输
  - 需要高速连续发送
  - 对实时性要求高的场合

在实际应用中,如无特殊要求,推荐使用TXE标志的方式,可以获得更好的传输效率。

## 其他

### 时钟总线上的外设复位寄存器

在很多硬件设计中，**复位寄存器在触发复位之后，硬件会自动将其恢复为默认值 `0`**，因此无需手动再写一次 `0`（不复位）。这种设计简化了复位操作流程，同时避免了冗余的写操作。

#### 为什么库函数只需要“先写 `0` 再写 `1`”？

1. **触发复位后，硬件自动清零：**
   - 复位信号在触发后，硬件通常会自动将复位寄存器恢复为默认状态（`0`），以表明复位已经结束。
   - 这意味着写 `1` 触发复位时，无需手动再写 `0`，因为硬件本身会完成复位信号的清除。
2. **复位完成后，外设状态即退出复位：**
   - 复位寄存器的主要任务是产生一个复位信号，并不需要长时间保持复位状态。
   - 写 `1` 后复位触发，寄存器会自动清零，从而让外设退出复位，回到工作状态。
3. **硬件设计简化了用户操作：**
   - 如果每次复位都需要写 `1` -> 写 `0`，不仅增加了代码复杂性，还可能导致用户误操作（比如漏写 `0`）。
   - 自动清零的机制让复位逻辑对用户透明，降低了使用的复杂性。

#### 为什么还需要写 `0` 再写 `1`？

尽管复位寄存器自动清零，但**库函数仍然需要先写 `0` 再写 `1`，原因如下：**

1. **清除可能的遗留状态：**
   - 如果某次复位操作失败，或者某些原因导致复位位未被正确清零，直接写 `1` 可能会导致复位逻辑失效。
   - 通过显式先写 `0`，可以清除任何可能的残留复位状态。
2. **确保复位信号完整：**
   - 某些硬件逻辑需要完整的 `0` -> `1` 的上升沿来触发复位。如果不先写 `0`，可能导致复位逻辑无法正常触发。



### 库函数开发的注意事项

**事项1**

- 还是先有程序思路再写代码，因为有一些库函数也没有实现，就只能手动寄存器实现。
  - 比如`USART_ClearITPendingBit(USART1, USART_IT_IDLE);`清除不了，没有这样的参数。
  - 需要手动实现
- 配置时有时候会跨函数，比如串口，GPIO口，串口，NVIC等，没有思路容易乱。

**事项2**

- 串口通信时GPIO配置复用功能时，要在GPIO基本设置的前面，外设总线时钟使能的后面，否则会偷发一个数据

### 库函数的通用配置思路

#### **比如定时器的延迟函数**

```c
void tim5Delay_Ms(u16 ms)
{
    // 使能TIM5的时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
    
    // 禁用定时器以进行配置
    TIM_Cmd(TIM5, DISABLE);
    
    // 配置TIM6的预分频器和自动重载值
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};
    //默认一个结构体配置
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    
    //只需要配置需要的参数，其他就用默认的
    TIM_TimeBaseStructure.TIM_Prescaler = 8400 - 1;
    TIM_TimeBaseStructure.TIM_Period = ms * 10 - 1;
    // 选择向上计数模式
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    //这里面也会产生一个更新事件，不用再写一个更新事件。
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
    
    
    // 清除更新标志位
    TIM_ClearFlag(TIM5, TIM_FLAG_Update);
    
    // 使能定时器
    TIM_Cmd(TIM5, ENABLE);
    
    // 等待更新事件完成
    while(!TIM_GetFlagStatus(TIM5, TIM_FLAG_Update));
    
    // 可选：清除更新标志位
    TIM_ClearFlag(TIM5, TIM_FLAG_Update);
    // 禁用定时器以进行配置，关闭定时器或者设置单次计数模式
    TIM_Cmd(TIM5, DISABLE);
}
```

**先整理好配置思路，按照思路调用对应函数即可。不相关参数，可以不赋值（结构体），也可以通过先默认初始化配置结构体，再根据需求修改特定参数的配置思路，可以使代码更简洁、易读，并减少配置错误的风险。这种方法适用于所有外设的配置，推荐在开发过程中广泛使用。**

#### 是否可以使用“|”一起配置？

建议都分开写避免错误，但是分开写时必须保证每配置一个都是被赋值到寄存器中的，不然会被覆盖。

```c
GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
GPIO_Init(GPIOA, &GPIO_InitStruct); // 只配置Pin7
```















指纹模块函数封装

```
GPIO口初始化
usart6串口初始化
接收中断配置
串口发送一个指令函数
解析收到的数据包函数

```

