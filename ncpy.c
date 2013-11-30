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
