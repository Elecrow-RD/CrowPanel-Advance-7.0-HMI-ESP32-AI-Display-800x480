## Wirless Module H2/C6 Single Board Burn-in Method

### 1.Tools to be prepared

|      | Tools                      | quantities |
| ---- | -------------------------- | ---------- |
| 1    | Single Female DuPont Cable | 4          |
| 2    | Serial Module Tools        | 1          |
| 3    | Wirless Module H2/C6       | 1          |
| 4    | Soldering Iron & Wire      |            |

 Single Female DuPont Cable
 
![1](https://github.com/user-attachments/assets/eaec3064-b89f-493b-8b93-12a9dbb2f7dc)



Wirless Module H2/C6

![2](https://github.com/user-attachments/assets/a8fa48b8-56b8-4b15-8312-6d496cb194b3)



Serial modules (CH340/CP2102, etc. are fine)

![3](https://github.com/user-attachments/assets/8fbcc7b0-2959-4898-8eea-895ba3edc79d)




 

### 2.Weld line.

The burn-in pins for the Wirless Module H2/C6 are the same, and the four wires that need to be soldered are located as follows：

（Arrow pointing up）

| RX   | TX   |
| ---- | ---- |
|      |      |
|      |      |
|      |      |
| 3V3  |      |
| GND  |      |
|      |      |

Module Pin Definition

The finished soldering is shown below.

![4](https://github.com/user-attachments/assets/8d711279-4d25-4057-9e8d-fadda9c70a0a)

![5](https://github.com/user-attachments/assets/47befec5-00ff-4639-9327-f997d809d3e8)

 

### 3.Burning

RX is connected to TX of the serial module;

TX connection to the RX of the serial module;

3V3 connection to the 3V3 of the serial module;

GND connection to the GND of the serial module.

Connect the serial module to the computer and find the corresponding serial device number (e.g. COM7) in the Task Manager.

![6](https://github.com/user-attachments/assets/6eaab326-cc9a-4a39-9f3e-b009f8ef1d32)


Open Arduino or open the corresponding serial port under any serial port tool.

![7](https://github.com/user-attachments/assets/475d212e-7c78-440a-ab17-4e65658dc8e0)


Press and hold the BOOT key and then press the RST key to enter burn mode.

![8](https://github.com/user-attachments/assets/9f0fea5e-4f69-4f04-a210-d4a6a68bc5b6)


The serial monitor prints to enter burn-in mode.
![9](https://github.com/user-attachments/assets/c743e703-ce6f-473b-ab7a-dcd3981a7efa)



 
