@256
D=A
@SP
M=D
@TRANSLATOR_RETURN0
D=A
@SP
A=M
M=D
@SP
M=M+1
@LCL
D=M
@SP
A=M
M=D
@SP
M=M+1
@ARG
D=M
@SP
A=M
M=D
@SP
M=M+1
@THIS
D=M
@SP
A=M
M=D
@SP
M=M+1
@THAT
D=M
@SP
A=M
M=D
@SP
M=M+1
@SP
D=M
@LCL
M=D
@5
D=D-A
@ARG
M=D
@Sys.init
0;JMP
(TRANSLATOR_RETURN0)
(Main.fibonacci)
@ARG
D=M
@0
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
@2
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
AM=M-1
D=M
A=A-1
D=M-D
@TRANSLATOR_CMP_BEGIN0
D;JLT
D=0
@TRANSLATOR_CMP_END0
0;JMP
(TRANSLATOR_CMP_BEGIN0)
D=-1
(TRANSLATOR_CMP_END0)
@SP
A=M-1
M=D
@SP
AM=M-1
D=M
@IF_TRUE
D;JNE
@IF_FALSE
0;JMP
(IF_TRUE)
@ARG
D=M
@0
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
@LCL
D=M
@TRANSLATOR_frame
M=D
@5
A=D-A
D=M
@TRANSLATOR_retAddr
M=D
@0
D=A
@ARG
M=D+M
@SP
AM=M-1
D=M
@ARG
A=M
M=D
@0
D=A
@ARG
M=M-D
@ARG
D=M
@SP
M=D+1
@TRANSLATOR_frame
AM=M-1
D=M
@THAT
M=D
@TRANSLATOR_frame
AM=M-1
D=M
@THIS
M=D
@TRANSLATOR_frame
AM=M-1
D=M
@ARG
M=D
@TRANSLATOR_frame
AM=M-1
D=M
@LCL
M=D
@TRANSLATOR_retAddr
A=M
0;JMP
(IF_FALSE)
@ARG
D=M
@0
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
@2
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
AM=M-1
D=M
A=A-1
M=M-D
@TRANSLATOR_RETURN1
D=A
@SP
A=M
M=D
@SP
M=M+1
@LCL
D=M
@SP
A=M
M=D
@SP
M=M+1
@ARG
D=M
@SP
A=M
M=D
@SP
M=M+1
@THIS
D=M
@SP
A=M
M=D
@SP
M=M+1
@THAT
D=M
@SP
A=M
M=D
@SP
M=M+1
@SP
D=M
@LCL
M=D
@6
D=D-A
@ARG
M=D
@Main.fibonacci
0;JMP
(TRANSLATOR_RETURN1)
@ARG
D=M
@0
A=D+A
D=M
@SP
A=M
M=D
@SP
M=M+1
@1
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
AM=M-1
D=M
A=A-1
M=M-D
@TRANSLATOR_RETURN2
D=A
@SP
A=M
M=D
@SP
M=M+1
@LCL
D=M
@SP
A=M
M=D
@SP
M=M+1
@ARG
D=M
@SP
A=M
M=D
@SP
M=M+1
@THIS
D=M
@SP
A=M
M=D
@SP
M=M+1
@THAT
D=M
@SP
A=M
M=D
@SP
M=M+1
@SP
D=M
@LCL
M=D
@6
D=D-A
@ARG
M=D
@Main.fibonacci
0;JMP
(TRANSLATOR_RETURN2)
@SP
AM=M-1
D=M
A=A-1
M=D+M
@LCL
D=M
@TRANSLATOR_frame
M=D
@5
A=D-A
D=M
@TRANSLATOR_retAddr
M=D
@0
D=A
@ARG
M=D+M
@SP
AM=M-1
D=M
@ARG
A=M
M=D
@0
D=A
@ARG
M=M-D
@ARG
D=M
@SP
M=D+1
@TRANSLATOR_frame
AM=M-1
D=M
@THAT
M=D
@TRANSLATOR_frame
AM=M-1
D=M
@THIS
M=D
@TRANSLATOR_frame
AM=M-1
D=M
@ARG
M=D
@TRANSLATOR_frame
AM=M-1
D=M
@LCL
M=D
@TRANSLATOR_retAddr
A=M
0;JMP
(Sys.init)
@4
D=A
@SP
A=M
M=D
@SP
M=M+1
@TRANSLATOR_RETURN3
D=A
@SP
A=M
M=D
@SP
M=M+1
@LCL
D=M
@SP
A=M
M=D
@SP
M=M+1
@ARG
D=M
@SP
A=M
M=D
@SP
M=M+1
@THIS
D=M
@SP
A=M
M=D
@SP
M=M+1
@THAT
D=M
@SP
A=M
M=D
@SP
M=M+1
@SP
D=M
@LCL
M=D
@6
D=D-A
@ARG
M=D
@Main.fibonacci
0;JMP
(TRANSLATOR_RETURN3)
(WHILE)
@WHILE
0;JMP
