################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS_QUOTED += \
"../Project_Settings/Startup_Code/startup_S32K144.S" \

C_SRCS_QUOTED += \
"../Project_Settings/Startup_Code/startup.c" \
"../Project_Settings/Startup_Code/system_S32K144.c" \

S_UPPER_SRCS += \
../Project_Settings/Startup_Code/startup_S32K144.S \

C_SRCS += \
../Project_Settings/Startup_Code/startup.c \
../Project_Settings/Startup_Code/system_S32K144.c \

OBJS_OS_FORMAT += \
./Project_Settings/Startup_Code/startup.o \
./Project_Settings/Startup_Code/startup_S32K144.o \
./Project_Settings/Startup_Code/system_S32K144.o \

C_DEPS_QUOTED += \
"./Project_Settings/Startup_Code/startup.d" \
"./Project_Settings/Startup_Code/system_S32K144.d" \

OBJS += \
./Project_Settings/Startup_Code/startup.o \
./Project_Settings/Startup_Code/startup_S32K144.o \
./Project_Settings/Startup_Code/system_S32K144.o \

OBJS_QUOTED += \
"./Project_Settings/Startup_Code/startup.o" \
"./Project_Settings/Startup_Code/startup_S32K144.o" \
"./Project_Settings/Startup_Code/system_S32K144.o" \

C_DEPS += \
./Project_Settings/Startup_Code/startup.d \
./Project_Settings/Startup_Code/system_S32K144.d \


# Each subdirectory must supply rules for building sources it contributes
Project_Settings/Startup_Code/startup.o: ../Project_Settings/Startup_Code/startup.c
	@echo 'Building file: $<'
	@echo 'Executing target #1 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Project_Settings/Startup_Code/startup.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Project_Settings/Startup_Code/startup.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Project_Settings/Startup_Code/startup_S32K144.o: ../Project_Settings/Startup_Code/startup_S32K144.S
	@echo 'Building file: $<'
	@echo 'Executing target #2 $<'
	@echo 'Invoking: Standard S32DS Assembler'
	arm-none-eabi-gcc "@Project_Settings/Startup_Code/startup_S32K144.args" -o "Project_Settings/Startup_Code/startup_S32K144.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Project_Settings/Startup_Code/system_S32K144.o: ../Project_Settings/Startup_Code/system_S32K144.c
	@echo 'Building file: $<'
	@echo 'Executing target #3 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Project_Settings/Startup_Code/system_S32K144.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "Project_Settings/Startup_Code/system_S32K144.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


