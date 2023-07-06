# gen_cli
generic CLI with easy configurabilty for windows and Linux made in C 

## Building

```shell
mkdir build 
cd build 

cmake ../
make

NOTE: for MIN GW in windows use the following command
cmake -G "MinGW Makefiles" ../
make

```
## API Usage

* To add a new command use the below macro   
`COMMAND(COMMAND_NAME, HELP_STRING)`  

* Each command must have its own command function which will be triggered when the command is entered by the user    
`CMDFUNC(COMMAND_NAME)`

* To add a new page use the below macro    
`PAGE(PAGE_NAME , PAGE_HELP_STRING,POINTER_TO_PAGE)`

* Check [example](https://github.com/ajaykrishna97/gen_cli/blob/main/CLI/src/main.c) for sample API usage.


![SAMPLE_IMAGE 1](https://github.com/ajaykrishna97/gen_cli/tree/main/images/EXAMPLE_CLI_HOME.png?raw=true)
![SAMPLE_IMAGE 2](https://github.com/ajaykrishna97/gen_cli/tree/main/images/EXAMPLE_CLI_CHANGE_PAGE.png?raw=true)

<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE` for more information.



