#include <nn.h>
#include <reqrep.h>

#include "ncpy.h"

#include "io.h"
#include "client.h"
#include "server.h"


int display_usage()
{
  printf("ncpy -c ADDRESS\n");
  printf("ncpy -s FILENAME\n");
  return 1;
}


int main(int argc, char* argv[])
{
  if (argc == 3 && strcmp(argv[1], "-c") == 0)
  {
    return execute_client(argv[2]);
  }
  
  if (argc == 3 && strcmp(argv[1], "-s") == 0)
  {
    return execute_server(PORT, argv[2]);
  }

  return display_usage();
}
