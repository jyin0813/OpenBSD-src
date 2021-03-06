#include <string.h>
#include <stat.h>
#include <unixlib.h>

int wrapped_stat (path, buffer)
const char *path;
struct stat *buffer;
{
  char statpath[1024];
  int rs;

  strcpy(statpath, path);
  strip_path(statpath);
  if(strcmp(statpath, ".") == 0)
     getwd(statpath);

  if ((rs = stat (statpath, buffer)) < 0)
      {
      /* If stat() fails try again after appending ".dir" to the filename
         this allows you to stat things like "bloogle/CVS" from VMS 6.1 */
      strcat(statpath, ".dir");
      rs = stat (statpath, buffer);
      }

  return rs;
}
