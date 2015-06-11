#ifndef PIPE_H_
#define PIPE_H_
//#define WIN32
const int LINE_INPUT_MAX_CHAR = 8192;
const int PATH_MAX_CHAR=1024;
const char PATH_SEPARATOR='/';

void open_pipe();
int line_input(char *line_str);

#endif
