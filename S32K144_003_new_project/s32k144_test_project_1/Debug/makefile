################################################################################
# Automatically-generated file. Do not edit!
################################################################################

#Progress monitor hint: 4
first : all
-include ../makefile.init

# This file contains definitions of environment variables used in the makefiles and .args files if exist.
-include makefile.local

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include Project_Settings/Linker_Files/subdir.mk
-include Project_Settings/Startup_Code/subdir.mk
-include src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
s32k144_test_project_1.elf \

EXECUTABLES_QUOTED += \
"s32k144_test_project_1.elf" \

EXECUTABLES_OS_FORMAT += \
s32k144_test_project_1.elf \

SECONDARY_SIZE += \
s32k144_test_project_1.siz \

SECONDARY_SIZE_QUOTED += \
"s32k144_test_project_1.siz" \

SECONDARY_SIZE_OS_FORMAT += \
s32k144_test_project_1.siz \


# All Target
all: warning-messages explicit-dependencies s32k144_test_project_1.elf secondary-outputs

# Tool invocations
s32k144_test_project_1.elf: $(OBJS) D:/Users/Administrator/workspaceS32DS.ARM.2018.R1/s32k144_test_project_1/Project_Settings/Linker_Files/S32K144_64_flash.ld $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Executing target #5 $@'
	@echo 'Invoking: Standard S32DS C Linker'
	arm-none-eabi-gcc -o "s32k144_test_project_1.elf" "@s32k144_test_project_1.args"  $(USER_OBJS_QUOTED) $(LIBS_QUOTED)
	@echo 'Finished building target: $@'
	@echo ' '

s32k144_test_project_1.siz: $(EXECUTABLES)
	@echo 'Executing target #6 $@'
	@echo 'Invoking: Standard S32DS Print Size'
	arm-none-eabi-size --format=berkeley $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) ./*/*.d ./*/*/*.d  ./*/*.o ./*/*/*.o  $(SECONDARY_SIZE_QUOTED) $(EXECUTABLES_QUOTED)
	-@echo ' '

secondary-outputs: $(SECONDARY_SIZE)

.PHONY: all clean dependents explicit-dependencies warning-messages
.SECONDARY:

-include ../makefile.targets

explicit-dependencies: 

warning-messages: 

