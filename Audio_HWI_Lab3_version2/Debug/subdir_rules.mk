################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
myWorkcfg.cmd: ../myWork.tcf
	@echo 'Building file: $<'
	@echo 'Invoking: TConf'
	"E:/Program Files/TexasIns/bios_5_42_01_09/xdctools/tconf" -b -Dconfig.importPath="E:/Program Files/TexasIns/bios_5_42_01_09/packages;" "$<"
	@echo 'Finished building: $<'
	@echo ' '

myWorkcfg.s??: | myWorkcfg.cmd
myWorkcfg_c.c: | myWorkcfg.cmd
myWorkcfg.h: | myWorkcfg.cmd
myWorkcfg.h??: | myWorkcfg.cmd
myWork.cdb: | myWorkcfg.cmd

myWorkcfg.obj: ./myWorkcfg.s?? $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"E:/Program Files/TexasIns/ccsv5/tools/compiler/c6000_7.4.23/bin/cl6x" -mv64+ -g --include_path="E:/Program Files/TexasIns/ccsv5/tools/compiler/c6000_7.4.23/include" --include_path="C:/ti/boards/evmdm6437_v2/include" --include_path="F:/Documents/CCSworkspace/Audio_HWI_Lab3/Debug" --include_path="F:/Documents/CCSworkspace/Audio_HWI_Lab3/inc" --include_path="E:/Program Files/TexasIns/bios_5_42_01_09/packages/ti/bios/include" --include_path="E:/Program Files/TexasIns/bios_5_42_01_09/packages/ti/rtdx/include/c6000" --display_error_number --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="myWorkcfg.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

myWorkcfg_c.obj: ./myWorkcfg_c.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"E:/Program Files/TexasIns/ccsv5/tools/compiler/c6000_7.4.23/bin/cl6x" -mv64+ -g --include_path="E:/Program Files/TexasIns/ccsv5/tools/compiler/c6000_7.4.23/include" --include_path="C:/ti/boards/evmdm6437_v2/include" --include_path="F:/Documents/CCSworkspace/Audio_HWI_Lab3/Debug" --include_path="F:/Documents/CCSworkspace/Audio_HWI_Lab3/inc" --include_path="E:/Program Files/TexasIns/bios_5_42_01_09/packages/ti/bios/include" --include_path="E:/Program Files/TexasIns/bios_5_42_01_09/packages/ti/rtdx/include/c6000" --display_error_number --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="myWorkcfg_c.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


