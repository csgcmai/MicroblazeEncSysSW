# MicroblazeEncSysSW
This is the code for one of my project almost four years ago. This project is about an public key chip design with control system design. I am responsible for the system design excluding the encryption HDL code, where the encryption we use is the Rainbow, one of the MPKC. The code here, would including 
- The UART command, which is the foundation of the GPRS module
- Some of the AT-Command for GPRS TCP/IP connection and two simple HTTP protocol over it, i.e., the get and post. 
- The SMBUS reading of MLX90614 vis IIC provided by Xilinx

Please to be kindly noted that this work was done in my year2 of B.SC in SCUT. The system is not installed on the MCU, i.e., microblaze. And I use the stupid dead loop for some of the function. 

As the project and competition has been finished for a few years, some of the links and accounts in the document may no longer accessible. 
