# gen_cli
generic CLI with easy configurabilty for windows and Linux made in C 

## Building

Steps to build on windows

```shell
mkdir build 
cd build 

gcc -I..\CLI\include -I..\helper\include  -O0 -g3 -c -fmessage-length=0  -o main.o ..\CLI\src\main.c
gcc -I..\CLI\include -I..\helper\include  -O0 -g3 -c -fmessage-length=0  -o helper.o ..\helper\src\helper.c
gcc -I..\CLI\include -I..\helper\include  -O0 -g3 -c -fmessage-length=0  -o cli.o ..\CLI\src\cli.c 
gcc -o gen_CLI.exe cli.o main.o helper.o

```

Steps to build on linux

```shell
TBD
```
