################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
build-161678513:
	@$(MAKE) -Onone -f TOOLS/subdir_rules.mk build-161678513-inproc

build-161678513-inproc: ../TOOLS/app_ble_uartlog.cfg
	@echo 'Building file: $<'
	@echo 'Invoking: XDCtools'
	"C:/ti/xdctools_3_32_00_06_core/xs" --xdcpath="C:/ti/tirtos_cc13xx_cc26xx_2_20_01_08/packages;C:/ti/tirtos_cc13xx_cc26xx_2_20_01_08/products/tidrivers_cc13xx_cc26xx_2_20_01_10/packages;C:/ti/tirtos_cc13xx_cc26xx_2_20_01_08/products/bios_6_46_01_38/packages;C:/ti/tirtos_cc13xx_cc26xx_2_20_01_08/products/uia_2_00_06_52/packages;C:/ti/ccsv7/ccs_base;" xdc.tools.configuro -o configPkg -t ti.targets.arm.elf.M3 -p ti.platforms.simplelink:CC2650F128 -r release -c "C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS" --compileOptions "-mv7M3 --code_state=16 -me -O4 --opt_for_speed=0 --include_path=\"C:/Users/Olivia/workspace_v7/emon_app_cc2650stk\" --include_path=\"C:/Users/Olivia/workspace_v7/emon_app_cc2650stk/Profiles\" --include_path=\"C:/Users/Olivia/workspace_v7/emon_app_cc2650stk/Application\" --include_path=\"C:/Users/Olivia/workspace_v7/emon_app_cc2650stk/ICallBLE\" --include_path=\"C:/Users/Olivia/workspace_v7/emon_app_cc2650stk/Startup\" --include_path=\"C:/Users/Olivia/workspace_v7/emon_app_cc2650stk/Board\" --include_path=\"c:/ti/simplelink_academy_01_11_00_0000/modules/projects/support_files/Components/uart_log\" --include_path=\"c:/ti/simplelink/ble_sdk_2_02_01_18/src/examples/simple_peripheral/cc26xx/app\" --include_path=\"c:/ti/simplelink/ble_sdk_2_02_01_18/src/inc\" --include_path=\"c:/ti/simplelink/ble_sdk_2_02_01_18/src/icall/inc\" --include_path=\"c:/ti/simplelink/ble_sdk_2_02_01_18/src/profiles/roles/cc26xx\" --include_path=\"c:/ti/simplelink/ble_sdk_2_02_01_18/src/profiles/roles\" --include_path=\"c:/ti/simplelink/ble_sdk_2_02_01_18/src/profiles/dev_info\" --include_path=\"c:/ti/simplelink/ble_sdk_2_02_01_18/src/profiles/simple_profile/cc26xx\" --include_path=\"c:/ti/simplelink/ble_sdk_2_02_01_18/src/profiles/simple_profile\" --include_path=\"c:/ti/simplelink/ble_sdk_2_02_01_18/src/common/cc26xx\" --include_path=\"c:/ti/simplelink/ble_sdk_2_02_01_18/src/components/heapmgr\" --include_path=\"c:/ti/simplelink/ble_sdk_2_02_01_18/src/controller/cc26xx/inc\" --include_path=\"c:/ti/simplelink/ble_sdk_2_02_01_18/src/components/hal/src/target/_common\" --include_path=\"c:/ti/simplelink/ble_sdk_2_02_01_18/src/components/hal/src/target\" --include_path=\"c:/ti/simplelink/ble_sdk_2_02_01_18/src/components/hal/src/target/_common/cc26xx\" --include_path=\"c:/ti/simplelink/ble_sdk_2_02_01_18/src/components/hal/src/inc\" --include_path=\"c:/ti/simplelink/ble_sdk_2_02_01_18/src/components/osal/src/inc\" --include_path=\"c:/ti/simplelink/ble_sdk_2_02_01_18/src/components/services/src/sdata\" --include_path=\"c:/ti/simplelink/ble_sdk_2_02_01_18/src/components/services/src/saddr\" --include_path=\"c:/ti/simplelink/ble_sdk_2_02_01_18/src/components/icall/src/inc\" --include_path=\"c:/ti/tirtos_cc13xx_cc26xx_2_20_01_08/products/cc26xxware_2_24_02_17393\" --include_path=\"C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/include\" --define=USE_ICALL --define=POWER_SAVING --define=SBP_TASK_STACK_SIZE=700 --define=GAPROLE_TASK_STACK_SIZE=520 --define=HEAPMGR_SIZE=0 --define=Display_DISABLE_ALL --define=BOARD_DISPLAY_EXCLUDE_UART --define=xBOARD_DISPLAY_EXCLUDE_LCD --define=ICALL_MAX_NUM_TASKS=3 --define=ICALL_MAX_NUM_ENTITIES=6 --define=xdc_runtime_Assert_DISABLE_ALL --define=Xxdc_runtime_Log_DISABLE_ALL --define=MAX_NUM_BLE_CONNS=1 --define=CC26XX --define=xdc_FILE=\"\"\"\" --define=UARTLOG_NUM_EVT_BUF=32 -g --c99 --gcc --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi  " "$<"
	@echo 'Finished building: $<'
	@echo ' '

configPkg/linker.cmd: build-161678513 ../TOOLS/app_ble_uartlog.cfg
configPkg/compiler.opt: build-161678513
configPkg/: build-161678513


