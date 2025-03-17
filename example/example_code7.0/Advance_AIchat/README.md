# **AI real-time voice communication example**

we will integrate the CrowPanel Advance ESP32 display with a Large Language Model (LLM) and use a local computer running Python server code to process data. The processed data is then sent to the CrowPanel Advance AI display, which runs on Arduino code, for visualization. This setup enables seamless voice interaction with AI, showcasing the power of combining advanced hardware and software solutions.

***\*Note:\**** 

***\*ESP32 version 3.0.2\****

***\*Python version 3.10.6\****

 

### **How to use example**

You can find detailed video tutorials at the bottom of the page.

 

***\*First\****, identify the model you need to access and register an account on the model’s official website to obtain the access key.

***\*Then\**** uncomment the corresponding model code in the 'llm_openai.py' file and replace the placeholder with the key.

Ensure that your Python version is 3.10.6. Then, open the Command Prompt (or terminal) and navigate to the code folder. Run the following commands to create a virtual environment and install the related dependencies.

![10-1](https://github.com/user-attachments/assets/16182940-da76-47ef-91d3-2c2fdd04ca59)

cmd

```
python -m venv venv
.\venv\Scripts\activate
python -m pip install --upgrade pip
pip install requrements-i
pip install openai dashscope
```



After installing the dependencies, you can run the 'ws.py' script to execute the saerver code.

 

The next step is to modify the ESP32 device code. We only need to change the server's IP address to ensure the device is on the same LAN as the WiFi network.

![10-2](https://github.com/user-attachments/assets/4a3fa5a3-7258-435e-aa66-ce83e0b33d69)



***\*Configuration parameter：\****

![10-3](https://github.com/user-attachments/assets/37b94896-06c9-4370-a653-5296540483f7)





After burning successfully, the device will appear as connected on the server side and be ready to communicate. During communication, you can press the boot button to interrupt the current session and initiate a new one.

### **How to add Arduino libraries**

Replace the libraries file with the following path:

C:\Users\user name\Documents\Arduio\libraries

 

***\*Note\****: The 'user name' here is the same as the user computer account name.

 

## **Video tutorial link**：

 https://youtu.be/oIz41dyDqjo # **AI real-time voice communication example**

we will integrate the CrowPanel Advance ESP32 display with a Large Language Model (LLM) and use a local computer running Python server code to process data. The processed data is then sent to the CrowPanel Advance AI display, which runs on Arduino code, for visualization. This setup enables seamless voice interaction with AI, showcasing the power of combining advanced hardware and software solutions.

***\*Note:\**** 

***\*ESP32 version 3.0.2\****

***\*Python version 3.10.6\****

 

### **How to use example**

You can find detailed video tutorials at the bottom of the page.

 

***\*First\****, identify the model you need to access and register an account on the model’s official website to obtain the access key.

***\*Then\**** uncomment the corresponding model code in the 'llm_openai.py' file and replace the placeholder with the key.

Ensure that your Python version is 3.10.6. Then, open the Command Prompt (or terminal) and navigate to the code folder. Run the following commands to create a virtual environment and install the related dependencies.

![10-1](https://github.com/user-attachments/assets/16182940-da76-47ef-91d3-2c2fdd04ca59)

cmd

```
python -m venv venv
.\venv\Scripts\activate
python -m pip install --upgrade pip
pip install requrements-i
pip install openai dashscope
```



After installing the dependencies, you can run the 'ws.py' script to execute the saerver code.

 

The next step is to modify the ESP32 device code. We only need to change the server's IP address to ensure the device is on the same LAN as the WiFi network.

![10-2](https://github.com/user-attachments/assets/4a3fa5a3-7258-435e-aa66-ce83e0b33d69)



***\*Configuration parameter：\****

![10-3](https://github.com/user-attachments/assets/37b94896-06c9-4370-a653-5296540483f7)





After burning successfully, the device will appear as connected on the server side and be ready to communicate. During communication, you can press the boot button to interrupt the current session and initiate a new one.

### **How to add Arduino libraries**

Replace the libraries file with the following path:

C:\Users\user name\Documents\Arduio\libraries

 

***\*Note\****: The 'user name' here is the same as the user computer account name.

 

## **Video tutorial link**：

 https://youtu.be/oIz41dyDqjo 