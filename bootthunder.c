#include <bitthunder.h>
#include <bt_config.h>
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

	BT_HANDLE hUART0 = BT_DeviceOpen("uart0", &Error);
	uart_config(hUART0);
	BT_UartEnable(hUART0);

	BT_HANDLE hUART1 = BT_DeviceOpen("uart1", &Error);
	uart_config(hUART1);
	BT_UartEnable(hUART1);

#if (BT_CONFIG_LIB_PRINTF_SUPPORT_MULTIPLE_STDOUT)
	BT_AddStdout(hUART0);
	BT_AddStdout(hUART1);
#else
	BT_SetStdout(hUART1);
#endif

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

	timeout = 2;
	do {
		BT_kPrint("Press any key to cancel boot (%d seconds remain)...\r", timeout);
		BT_ThreadSleep(1000);
		BT_s32 c = BT_GetC(hUART0, BT_FILE_NON_BLOCK, &Error);
		if(c >= 0) goto fallback_shell;
		c = BT_GetC(hUART1, BT_FILE_NON_BLOCK, &Error);
		if(c >= 0) goto fallback_shell;
	} while (--timeout > 0);

	Error = BT_ShellScript("/sd0/bootthunder.cfg");
	if(Error) {
		BT_kPrint("Could not open shell script, or shell script failed.");
	}

fallback_shell:

	// If we got here then script wasn't found, or didn't boot! Create a shell.
	//sprintf(buffer, "boot --core 0 %08x", jtag_loop);
	//BT_ShellCommand(buffer);

	BT_kPrint("Starting fallback shell!");

	BT_HANDLE hShell0 = BT_ShellCreate(hUART0, hUART0, "BootThunder>", BT_SHELL_FLAG_NON_BLOCK, NULL);

	BT_HANDLE hShell1 = BT_ShellCreate(hUART1, hUART1, "BootThunder>", BT_SHELL_FLAG_NON_BLOCK, NULL);

	while(1)
	{
		BT_Shell(hShell0);

		BT_Shell(hShell1);

		BT_ThreadSleep(1);
	}

	return 0;
}
