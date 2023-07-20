Exemption:

1. In the forums, some teachers say that we don't need to check if the macro is written properly, while others say that we do need to check certain things, 
   such as making sure that there is an empty line after the macro name and at the end. 
   I have implemented some checks that I thought were important, excluding checks for code grammar as requested in Maman 14.

2. removeExtraWhitespace" function removes all white spaces before the first word, eliminates extra white spaces, replaces the remaining ones with regular spaces, 
   and finally removes all white spaces at the end of the line. 
   This function is crucial to my code, and therefore, I have not included any checks for white space elsewhere. 
   Some students have asked in the forums if we can make these kinds of changes during the macro stage, and the answer is yes. 
   Consequently, I have implemented the necessary modifications.

3. Based on my understanding of Maman 14 and the answers in the forums, errors will point to the line in the .am file after macro unpacking.

4. Only one error will be shown per line of code (source: questions in forum).

5. Code flow: assembler -> demacro -> assembler -> phaseOne -> phaseTwo -> fileMaker -> assembler

6. PhaseOne checks for proper grammar in data and instructions. 
   That's why PhaseTwo only handles labels inside instructions, which updates the addresses of all lines where ERA = 2 (.....10).

7. Under the test1 folder, we have the file ps1.as and its outputs. 
   This file is a copy of the code provided in Maman 14's PS file (table on line 35). 
   The file does not have .entry/.extern directives. 
   I used the table on page 35 as a reference to translate the code.

   The table on page 35 has some differences from my output. 
   Firstly, on line 0114 of the page 36 table, it is written 00001010000100 while it should be 00001010001000, since bne is a jump command, the target needs to be 2 and not 1. 
   Secondly, on line 0109, Maman 14 constantly changes the register number at least two or three times throughout the entire explanation, therefore, I used r4 and r5, as the table on page 35 does.

8. "Under the test2 folder, we have the file ps2.as and its outputs. 
   This file is a copy of the code provided in Maman 14's PS file (table on line 41). 
   The file does have .entry/.extern directives. 
   I used the table on page 41 as a reference to translate the code.

9. Under the test3 folder, we have the file et.as (error test) and a picture of the error screen. 
   It's the ps2.as file, but I changed every line so it has a different error.