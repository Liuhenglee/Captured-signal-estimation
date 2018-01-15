################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
evmdm6437_v2/tests/norflash/flash_test.obj: ../evmdm6437_v2/tests/norflash/flash_test.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"E:/Program Files/TexasIns/ccsv5/tools/compiler/c6000_7.4.23/bin/cl6x" -mv64+ -g --include_path="E:/Program Files/TexasIns/ccsv5/tools/compiler/c6000_7.4.23/include" --include_path="C:/ti/boards/evmdm6437_v2/include" --include_path="F:/Documents/CCSworkspace/Audio_HWI_Lab3/Debug" --include_path="F:/Documents/CCSworkspace/Audio_HWI_Lab3/inc" --include_path="E:/Program Files/TexasIns/bios_5_42_01_09/packages/ti/bios/include" --include_path="E:/Program Files/TexasIns/bios_5_42_01_09/packages/ti/rtdx/include/c6000" --display_error_number --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="evmdm6437_v2/tests/norflash/flash_test.pp" --obj_directory="evmdm6437_v2/tests/norflash" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

evmdm6437_v2/tests/norflash/main.obj: ../evmdm6437_v2/tests/norflash/main.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"E:/Program Files/TexasIns/ccsv5/tools/compiler/c6000_7.4.23/bin/cl6x" -mv64+ -g --include_path="E:/Program Files/TexasIns/ccsv5/tools/compiler/c6000_7.4.23/include" --include_path="C:/ti/boards/evmdm6437_v2/include" --include_path="F:/Documents/CCSworkspace/Audio_HWI_Lab3/Debug" --include_path="F:/Documents/CCSworkspace/Audio_HWI_Lab3/inc" --include_path="E:/Program Files/TexasIns/bios_5_42_01_09/packages/ti/bios/include" --include_path="E:/Program Files/TexasIns/bios_5_42_01_09/packages/ti/rtdx/include/c6000" --display_error_number --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="evmdm6437_v2/tests/norflash/main.pp" --obj_directory="evmdm6437_v2/tests/norflash" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


