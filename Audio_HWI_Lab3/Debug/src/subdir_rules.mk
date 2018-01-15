################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
src/audio-3.obj: ../src/audio-3.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"E:/Program Files/TexasIns/ccsv5/tools/compiler/c6000_7.4.23/bin/cl6x" -mv64+ -g --include_path="E:/Program Files/TexasIns/ccsv5/tools/compiler/c6000_7.4.23/include" --include_path="C:/ti/boards/evmdm6437_v2/include" --include_path="F:/Documents/CCSworkspace/Audio_HWI_Lab3/Debug" --include_path="F:/Documents/CCSworkspace/Audio_HWI_Lab3/inc" --include_path="E:/Program Files/TexasIns/bios_5_42_01_09/packages/ti/bios/include" --include_path="E:/Program Files/TexasIns/bios_5_42_01_09/packages/ti/rtdx/include/c6000" --display_error_number --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="src/audio-3.pp" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/codec.obj: ../src/codec.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"E:/Program Files/TexasIns/ccsv5/tools/compiler/c6000_7.4.23/bin/cl6x" -mv64+ -g --include_path="E:/Program Files/TexasIns/ccsv5/tools/compiler/c6000_7.4.23/include" --include_path="C:/ti/boards/evmdm6437_v2/include" --include_path="F:/Documents/CCSworkspace/Audio_HWI_Lab3/Debug" --include_path="F:/Documents/CCSworkspace/Audio_HWI_Lab3/inc" --include_path="E:/Program Files/TexasIns/bios_5_42_01_09/packages/ti/bios/include" --include_path="E:/Program Files/TexasIns/bios_5_42_01_09/packages/ti/rtdx/include/c6000" --display_error_number --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="src/codec.pp" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/coeffs.obj: ../src/coeffs.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"E:/Program Files/TexasIns/ccsv5/tools/compiler/c6000_7.4.23/bin/cl6x" -mv64+ -g --include_path="E:/Program Files/TexasIns/ccsv5/tools/compiler/c6000_7.4.23/include" --include_path="C:/ti/boards/evmdm6437_v2/include" --include_path="F:/Documents/CCSworkspace/Audio_HWI_Lab3/Debug" --include_path="F:/Documents/CCSworkspace/Audio_HWI_Lab3/inc" --include_path="E:/Program Files/TexasIns/bios_5_42_01_09/packages/ti/bios/include" --include_path="E:/Program Files/TexasIns/bios_5_42_01_09/packages/ti/rtdx/include/c6000" --display_error_number --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="src/coeffs.pp" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/dipMonitor.obj: ../src/dipMonitor.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"E:/Program Files/TexasIns/ccsv5/tools/compiler/c6000_7.4.23/bin/cl6x" -mv64+ -g --include_path="E:/Program Files/TexasIns/ccsv5/tools/compiler/c6000_7.4.23/include" --include_path="C:/ti/boards/evmdm6437_v2/include" --include_path="F:/Documents/CCSworkspace/Audio_HWI_Lab3/Debug" --include_path="F:/Documents/CCSworkspace/Audio_HWI_Lab3/inc" --include_path="E:/Program Files/TexasIns/bios_5_42_01_09/packages/ti/bios/include" --include_path="E:/Program Files/TexasIns/bios_5_42_01_09/packages/ti/rtdx/include/c6000" --display_error_number --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="src/dipMonitor.pp" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/fir.obj: ../src/fir.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"E:/Program Files/TexasIns/ccsv5/tools/compiler/c6000_7.4.23/bin/cl6x" -mv64+ -g --include_path="E:/Program Files/TexasIns/ccsv5/tools/compiler/c6000_7.4.23/include" --include_path="C:/ti/boards/evmdm6437_v2/include" --include_path="F:/Documents/CCSworkspace/Audio_HWI_Lab3/Debug" --include_path="F:/Documents/CCSworkspace/Audio_HWI_Lab3/inc" --include_path="E:/Program Files/TexasIns/bios_5_42_01_09/packages/ti/bios/include" --include_path="E:/Program Files/TexasIns/bios_5_42_01_09/packages/ti/rtdx/include/c6000" --display_error_number --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="src/fir.pp" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/gen_twiddle_fft16x16.obj: ../src/gen_twiddle_fft16x16.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"E:/Program Files/TexasIns/ccsv5/tools/compiler/c6000_7.4.23/bin/cl6x" -mv64+ -g --include_path="E:/Program Files/TexasIns/ccsv5/tools/compiler/c6000_7.4.23/include" --include_path="C:/ti/boards/evmdm6437_v2/include" --include_path="F:/Documents/CCSworkspace/Audio_HWI_Lab3/Debug" --include_path="F:/Documents/CCSworkspace/Audio_HWI_Lab3/inc" --include_path="E:/Program Files/TexasIns/bios_5_42_01_09/packages/ti/bios/include" --include_path="E:/Program Files/TexasIns/bios_5_42_01_09/packages/ti/rtdx/include/c6000" --display_error_number --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="src/gen_twiddle_fft16x16.pp" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


