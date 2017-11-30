# Standard log print in VP (virtual platform)

## Introduction
The log print is based sc_report API in systemc.There are four standard APIs from sc_report 
>  SC_REPORT_INFO  
>  SC_REPORT_WARNING  
>  SC_REPORT_ERROR  
>  SC_REPORT_FATAL  
 
Each API has different actions when called. 

For example, *SC_REPORT_FATAL* will display log info and call *sc_abort* and *SC_REPORT_INFO* just display the log info. At most time, we use *SC_REPORT_INFO* and there are different verbosity levels in it.
> SC_NONE = 0,  
> SC_LOW = 100,  
> SC_MEDIUM = 200,  
> SC_HIGH = 300,  
> SC_FULL = 400,  
> SC_DEBUG = 500  
 
 The default level is ***SC_MEDIUM*** which means *SC_HIGH/SC_FULL/SC_DEBUG* will not display in default configuration.
 
 The log print can be controlled by two ways.
 
> 1. ENV variable "SC_LOG"
> 2. Command line -s/--sc_log 

***The final controlled string will use the value from the env variable if the control string is in both places.***
 
## Control string usage:
 
	 "outfile:<log_file>;verbosity_level:<info_level>;<msg_string>:<report_level>"
	 <log_file>: 	log output file name -- Log file name string
	 <info_level>: 	info verbosity -- sc_none/sc_low/sc_medium/sc_high/sc_full/sc_debug
	 <msg_string>:	message string specified in sc_report.
	 <report_level>:sc report level --  info/warning/error/fatal/enable/disable
 
## Example for control string format:
 
	 [outfile:sc_log_file;verbosity_level:sc_debug;McAdaptor:enable]
	 outfile:		sc_log_file -- SC log will output to file sc_log_file.
	 verbosity_level:sc_debug -- All sc_info report will display.
	 McAdaptor:		enable -- All report level can display with msg string "McAdaptor"
 
 *The control string can be set by ENV variable "SC_LOG"*
 
 In bash shell
> export SC_LOG="outfile:sc_log_file;verbosity_level:sc_debug;McAdaptor:enable"
 
 In tcsh shell
 
>setenv SC_LOG "outfile:sc_log_file;verbosity_level:sc_debug;McAdaptor:enable"
 
It also can be set from command line:

> ./build/bin/aarch64_toplevel -c conf/aarch64_bare_mod.lua -s "outfile:sc_log_file;verbosity_level:sc_debug;McAdaptor:enable"
 
 In McAdaptor.cpp
 We can display some log with:
 
	 SC_REPORT_INFO_VERB("McAdaptor", "mc_socket created", SC_DEBUG);
 
 And the log will be like:
 
> Info: McAdaptor: mc_socket created
 
 Check the sc_log_file:
> 0 s: Info: McAdaptor: mc_socket created
 
 If we need to **disable** the log, there are some ways to use.
>  1. McAdaptor:disable -- disable all the log with "McAdaptor"
>  2. McAdaptor:warning -- raise the report level to SC_WARNING, so the SC_INFO will not enable
>  3. verbosity_level:sc_full -- raise the info level to sc_full, so SC_INFO with sc_full level will not enable
 
 