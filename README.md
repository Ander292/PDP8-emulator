# What is this

A simple emulator for an old architecture based on old PDP-8, the so called "Elementary computer". Made it so I can run programs from a uni course.
The thing has many differences from the real PDP-8 so the repo name is a bit misleading.

# How to use

Just use assembler.exe to create a binary file that the emulator will run. Its not a full assembler as it doesnt support labels and stuff.

Code example:

      200 LDA 210
      201 CIL
      202 ADD 211
      203 INC
      204 STA 212
      205 HLT
      
      210 DEC 27  ;; a
      211 DEC 14  ;; b
      212 DEC 0   ;; y

I have not implemented IO operations yet so the only way to view the result of the code is to take a look at the memory dump at the end of the program.
Looking at binary memory dumps is anoying so I made it output a csv file as well.

Run the program argless to get what arguments are available and what do they do.

Currently the debugger doesnt work in linux for now