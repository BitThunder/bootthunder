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


int main(void) {

	BT_ERROR Error;

	BT_HANDLE hUART0 = BT_DeviceOpen(BT_CONFIG_BOOTTHUNDER_SHELL_0_DEVICE, &Error);
	uart_config(hUART0);
	BT_UartEnable(hUART0);

#ifdef BT_CONFIG_BOOTTHUNDER_SHELL_1_ENABLE
	BT_HANDLE hUART1 = BT_DeviceOpen("uart1", &Error);
	uart_config(hUART1);
	BT_UartEnable(hUART1);
#endif

#if (BT_CONFIG_LIB_PRINTF_SUPPORT_MULTIPLE_STDOUT)
	BT_AddStdout(hUART0);
#ifdef BT_CONFIG_BOOTTHUNDER_SHELL_1_ENABLE
	BT_AddStdout(hUART1);
#endif
#else
	BT_SetStdout(hUART0);
#endif

	BT_kPrint("BootThunder started...");

	signal_booted();

	BT_u32 timeout = 0;

	BT_HANDLE hVolume00 = BT_DeviceOpen(BT_CONFIG_BOOTTHUNDER_BOOT_VOLUME, &Error);
	while(!hVolume00) {
		timeout += 5;
		if(timeout >= 5000) {
			goto fallback_shell;
		}
		BT_ThreadSleep(5);
		hVolume00 = BT_DeviceOpen(BT_CONFIG_BOOTTHUNDER_BOOT_VOLUME, &Error);
	}

	BT_Mount(hVolume00, BT_CONFIG_BOOTTHUNDER_BOOT_MOUNTPOINT);

	timeout = BT_CONFIG_BOOTTHUNDER_TIMEOUT;
	do {
		BT_kPrint("Press any key to cancel boot (%d seconds remain)...\r", timeout);
		BT_ThreadSleep(1000);
		BT_s32 c = BT_GetC(hUART0, BT_FILE_NON_BLOCK, &Error);
		if(c >= 0) goto fallback_shell;
#ifdef BT_CONFIG_BOOTTHUNDER_SHELL_1_ENABLE
		c = BT_GetC(hUART1, BT_FILE_NON_BLOCK, &Error);
		if(c >= 0) goto fallback_shell;
#endif
	} while (--timeout > 0);

	BT_HANDLE hShell = BT_ShellCreate(BT_stdin, BT_stdout, "BootThunder>", 0, NULL);

	Error = BT_ShellScript(hShell, BT_CONFIG_BOOTTHUNDER_BOOT_SCRIPT_PATH);
	if(Error) {
		BT_kPrint("Could not open shell script, or shell script failed.");
	}

fallback_shell:

	BT_kPrint("Starting fallback shell!");

	BT_HANDLE hShell0 = BT_ShellCreate(hUART0, hUART0, "BootThunder>", BT_SHELL_FLAG_NON_BLOCK, NULL);

#ifdef BT_CONFIG_BOOTTHUNDER_SHELL_1_ENABLE
	BT_HANDLE hShell1 = BT_ShellCreate(hUART1, hUART1, "BootThunder>", BT_SHELL_FLAG_NON_BLOCK, NULL);
#endif

	while(1) {
		BT_Shell(hShell0);

#ifdef BT_CONFIG_BOOTTHUNDER_SHELL_1_ENABLE
		BT_Shell(hShell1);
#endif
		BT_ThreadSleep(1);
	}

	return 0;
}
