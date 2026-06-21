# What is this

A simple emulator for an old architecture based on old PDP-8, the so called "Elementary computer". Made it so I can run programs for an uni course.
The thing has many differences from the real PDP-8 so the repo name is a bit misleading.

# How to use

Just use assembler.exe to create a binary file that the emulator will run. Its not a full assembler as it doesnt support labels and stuff for now.

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

Input output operations are explained in some examples inside assets folder. Interrupt routine is NOT implemented and will crash the program. IO operations are available only by using the IO instructions directly.

Linux version might not be completely stable.
