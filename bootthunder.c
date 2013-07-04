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

	BT_kPrint("Bootloader started...");

	BT_HANDLE hVolume = BT_DeviceOpen("mmc00", &Error);
	while(!hVolume) {
		BT_ThreadSleep(5);
		hVolume = BT_DeviceOpen("mmc00", &Error);
	}

	BT_Mount(hVolume, "/sd0/");

	signal_booted();

	Error = BT_ShellScript("/sd0/boot.cfg");
	if(Error) {
		BT_kPrint("No kernel shell script found, jumping to terminal shell");
	}

	// If we got here then script wasn't found, or didn't boot! Create a shell.
	sprintf(buffer, "boot --core 0 %08x", jtag_loop);
	BT_ShellCommand(buffer);

	while(1) {
		BT_ThreadSleep(1000);
	}

	return 0;
}
