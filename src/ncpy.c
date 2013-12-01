/*
 Copyright (c) 2013 Matthew Howlett

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal 
 in the Software without restriction, including without limitation the rights 
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*/


#include <nn.h>
#include <reqrep.h>

#include "ncpy.h"

#include "io.h"
#include "client.h"
#include "server.h"


int display_usage()
{
  printf("ncpy -r REMOTE-ADDRESS-TO-RECEIVE-FROM\n");
  printf("ncpy -s PATH-OF-FILE-TO-SEND\n");
  return 1;
}


int main(int argc, char* argv[])
{
  if (argc == 3 && 
      (strcmp(argv[1], "receive") == 0 || strcmp(argv[1], "-r") == 0))
  {
    return execute_client(argv[2]);
  }
  
  if (argc == 3 && 
      (strcmp(argv[1], "send") == 0 || strcmp(argv[1], "-s") == 0))
  {
    return execute_server(PORT, argv[2]);
  }

  return display_usage();
}
