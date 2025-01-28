# EmbeddedAssignment
A repository for Embedded systems programming course assignment done with NUCLEO-STM32F411RE

- Assignment-folder has the whole project
- ./Assignment/main.c is the file from which the program is ran
- ./Assignment/external has separated components of the program in their respective files
-  The following files are relevant for the program structure:
    - **ISR.c**: Interrupt routines
    - **PI_controller.c**: Routines for running the PI-controller/converter model
    - **setup.c**: Routines for setting up different parts of the device
    - **task.c**: Routines for the task scheduling
    - All relevant header-files for the above files.
