# 智能锁项目笔记

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

