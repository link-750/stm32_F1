cmd /c " ilinkarm "USER\main.o" "modbus\mb.o" "modbus\ascii\mbascii.o" "modbus\rtu\mbcrc.o" "modbus\functions\mbfunccoils.o" "modbus\functions\mbfuncdiag.o" "modbus\functions\mbfuncdisc.o" "modbus\functions\mbfuncholding.o" "modbus\functions\mbfuncinput.o" "modbus\functions\mbfuncother.o" "modbus\rtu\mbrtu.o" "modbus\tcp\mbtcp.o" "modbus\functions\mbutils.o" "STM32F10x_StdPeriph_Driver\src\misc.o" "modbus\port\portevent.o" "modbus\port\portserial.o" "modbus\port\porttimer.o" "USER\startup_stm32f10x_md.o" "STM32F10x_StdPeriph_Driver\src\stm32f10x_adc.o" "STM32F10x_StdPeriph_Driver\src\stm32f10x_bkp.o" "STM32F10x_StdPeriph_Driver\src\stm32f10x_can.o" "STM32F10x_StdPeriph_Driver\src\stm32f10x_cec.o" "STM32F10x_StdPeriph_Driver\src\stm32f10x_crc.o" "STM32F10x_StdPeriph_Driver\src\stm32f10x_dac.o" "STM32F10x_StdPeriph_Driver\src\stm32f10x_dbgmcu.o" "STM32F10x_StdPeriph_Driver\src\stm32f10x_dma.o" "STM32F10x_StdPeriph_Driver\src\stm32f10x_exti.o" "STM32F10x_StdPeriph_Driver\src\stm32f10x_flash.o" "STM32F10x_StdPeriph_Driver\src\stm32f10x_fsmc.o" "STM32F10x_StdPeriph_Driver\src\stm32f10x_gpio.o" "STM32F10x_StdPeriph_Driver\src\stm32f10x_i2c.o" "USER\stm32f10x_it.o" "STM32F10x_StdPeriph_Driver\src\stm32f10x_iwdg.o" "STM32F10x_StdPeriph_Driver\src\stm32f10x_pwr.o" "STM32F10x_StdPeriph_Driver\src\stm32f10x_rcc.o" "STM32F10x_StdPeriph_Driver\src\stm32f10x_rtc.o" "STM32F10x_StdPeriph_Driver\src\stm32f10x_sdio.o" "STM32F10x_StdPeriph_Driver\src\stm32f10x_spi.o" "STM32F10x_StdPeriph_Driver\src\stm32f10x_tim.o" "STM32F10x_StdPeriph_Driver\src\stm32f10x_usart.o" "STM32F10x_StdPeriph_Driver\src\stm32f10x_wwdg.o" "USER\system_stm32f10x.o" -o "FREEMB_RBT6.out" --config "C:/Program Files (x86)/IAR Systems/Embedded Workbench 7.5/arm/config/linker/ST/stm32f103xB.icf" --no_wrap_diagnostics --semihosting --redirect __iar_sh_stdout=__iar_sh_stdout_swo  && ielftool --ihex --verbose "FREEMB_RBT6.out" "FREEMB_RBT6.hex"  "