# 서강대학교 시스템 프로그래밍 프로젝트
2020년 1학기 서강대 시스템 프로그래밍 프로젝트입니다. 
## _목표_
단일 컴퓨터 내에서의 주요 시스템 소프트웨어로서 Assembler, Linker, Loader 등을 설계하고, 구축해봄으로 시스템 소프트웨어에서 사용하는 기본 알고리즘과 시스템 구축 기법을 숙달한다.

## _구현_
1. Assembler, Linker, Loader을 실행할 shell
2. 컴파일을 통해 만든 object code가 load되고 실행될 memory
3. mnemonic(ADD, COMP, FLOAT, ...)을 opcode 값으로 변환하는 OPCODE table과 관련 명령어 구현
4. Assembler 구현(SIC/XE의 assembly program source 파일(2_5.asm)을 입력받아서 object 파일 생성. 교재(L. Beck, “System Software : An Introduction to Systems Programming”, 3rd Edition,
Addison-Wesley Publishing, 1997)의 Chap2.2까지 설명된 SIC/XE Assembler의 기능을 구현.)
5. assemble 명령을 통해서 생성된 object 파일을 link시켜 메모리에 올리고, 이를 실행한다.
