################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../src/main.c" \

C_SRCS += \
../src/main.c \

OBJS_OS_FORMAT += \
./src/main.o \

C_DEPS_QUOTED += \
"./src/main.d" \

OBJS += \
./src/main.o \

OBJS_QUOTED += \
"./src/main.o" \

C_DEPS += \
./src/main.d \


# Each subdirectory must supply rules for building sources it contributes
src/main.o: ../src/main.c
	@echo 'Building file: $<'
	@echo 'Executing target #4 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/main.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "src/main.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


