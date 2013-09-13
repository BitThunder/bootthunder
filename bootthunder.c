#include <bitthunder.h>
#include <lib/putc.h>
#include <fs/bt_fs.h>

void signal_booted() {
	BT_GpioSetDirection(9, BT_GPIO_DIR_OUTPUT);
	BT_GpioSet(9, BT_TRUE);
}

void uart_config(BT_HANDLE hUART) {
	BT_UART_CONFIG oConfig;

	oConfig.eMode 		= BT_UART_MODE_POLLED;
	oConfig.ulBaudrate 	= 115200;
	oConfig.ucParity    = BT_UART_PARITY_NONE;
	oConfig.ucStopBits  = BT_UART_ONE_STOP_BIT;
	oConfig.ucDataBits  = BT_UART_8_DATABITS;

	BT_UartSetConfiguration(hUART, &oConfig);
}

void jtag_loop() {
	while(1) {
		__asm("");
	}
}

char buffer[1024];

int main(void) {

	BT_ERROR Error;

	BT_HANDLE hUART = BT_DeviceOpen("uart1", &Error);

	uart_config(hUART);

	BT_UartEnable(hUART);
	BT_SetStandardHandle(hUART);

	BT_kPrint("BootThunder started...");

	signal_booted();

	BT_u32 timeout = 0;
	while(timeout < 3) {
		BT_kPrint("Press any key to cancel boot (%d seconds)...\r", timeout++);
		timeout++;
		BT_ThreadSleep(1000);
	}

	timeout = 0;
	BT_HANDLE hVolume = BT_DeviceOpen("mmc00", &Error);
	while(!hVolume) {
		timeout += 5;
		if(timeout >= 5000) {
			goto fallback_shell;
		}
		BT_ThreadSleep(5);
		hVolume = BT_DeviceOpen("mmc00", &Error);
	}

	BT_Mount(hVolume, "/sd0/");

	Error = BT_ShellScript("/sd0/bootthunder.cfg");
	if(Error) {
		BT_kPrint("Could not open shell script, or shell script failed.");
	}

fallback_shell:

	// If we got here then script wasn't found, or didn't boot! Create a shell.
	//sprintf(buffer, "boot --core 0 %08x", jtag_loop);
	//BT_ShellCommand(buffer);

	BT_kPrint("Starting fallback shell!");

	/*char c;
	BT_Read(hUART, 0, 1, &c, &Error);*/

	while(1) {
		BT_u32 i = 0;

		bt_printf("BootThunder>");

		do {
			BT_s32 c = BT_GetC(hUART, 0, &Error);
			buffer[i] = c;

			if(c == '\r' || c == '\n') {
				buffer[i] = '\0';
				break;
			}
			BT_PutC(hUART, 0, buffer[i]);
		} while(++i < 1024);

		BT_PutC(hUART, 0, '\n');
		BT_ShellCommand(buffer);
	}

	return 0;
}
