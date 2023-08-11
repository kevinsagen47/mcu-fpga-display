# mcu-fpga-display
<div id="top"></div>

## steps to add a closed loop variable display<->mcu<->fpga

### I. FPGA

  #### A. FPGA rx
  1. add address and variable to rx_handle.v module
     ```sh
      8'hC6	: force_set_rx		<= {address[3],address[2]};
      ```
  2. add output variable to rx_handle.v module
     ```sh
     output reg[15:0] force_set_rx,
     ```
  3. add variable to closed loop module
     ```sh
     wire [15:0]force_set_rx,
     ```
  4. add variable net to the desired module
     ```sh
     .force_set_rx(force_set_rx)
     ```
  5. add input variable to the desired module
     ```sh
     input [15:0] force_set_rx,
     ```
  
  #### B. FPGA tx
  1. add address length according to variable length to tx_handle_mcu.v
     ```sh
      parameter length= 58;
     ```
  2. add input variable
     ```sh
      input [15:0] force_set_rx,
     ```
  3. add input bytes to data_array
     ```sh
     8'h0B,8'h0C,//11 12 force set
     ```
  4. send input variable with included address number
     ```sh
     data_array[11] <= force_set_rx[12:8];
  	 data_array[12] <= force_set_rx[7:0];
     ```
### II.MCU
#### A. FPGA -> MCU rx (filename: dependencies.c)
  1. add address length
	  ```sh
     uint16_t FPGA_length=58;
     uint8_t FPGA_input[58] = {0};
      ```
     
  2. add display variable
	  ```sh
     unsigned int force_set_display
     ```
  3. add update display variable display variable = rx address
     ```sh
       force_set_display=(FPGA_input[11]<<8)|(FPGA_input[12]);
      ```
  4. add read display api function
      ```sh
        unsigned int read_force_set_display(void){return force_set_display;}
     ```
  5. create write variable and add write set api function
     ```sh
       force_set
       void write_force_set (unsigned int arg){force_set=arg;}
     ```
#### B. display -> mcu rx (filename: mcu_display_control.c)
  1. add receiving address from display and input received variable with write function
      ```sh
        case 0xca:
	  write_force_set((display_input_command[2]<<8)|(display_input_command[1]));
	  break;
      ```
     
#### C. mcu ->display (filename: mcu_display_control.c)

  1. add display variable name in hex format
     ```sh
      uint8_t display_page_2_F_set [7] = { 0x6E, 0x38,0x2E,0x76,0x61,0x6C,0x3D};
       //n8.val=
    	```
  2. send display variable name with hex format length
     ```sh
      UART_Write(UART1,display_page_2_F_set,7);
     ```
  3. use read api function, convert, and send  variable into bcd
     ```sh
      binary_to_bcd_array(read_force_set_display());
		  UART_Write(UART1,bcd_array,5);
     ```
  4. send header
     ```sh
      UART_Write(UART1,header,3);
     ```
#### D. mcu tx -> FPGA (filename: depedencies.c)
  1. check if set variable and display variable is different, if different, send set variable
     ```sh
       else if (force_set!=force_set_display){
			UART_WRITE(UART0,0xC6);
			UART_WRITE(UART0,force_set& ~(~0U << 8));
			UART_WRITE(UART0,force_set>>8);
			UART_WRITE(UART0,0xFF);
			PC6 = 1;
	    }
     ```
 
#### E. add functions to header file (filename: dependencies.h)
  1. add read and write API function
     ```sh
       unsigned int read_force_set_display(void);
       void write_force_set (unsigned int arg);
     ```
	
