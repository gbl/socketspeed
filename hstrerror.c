#include <windows.h>
#include <stdio.h>

char *
hstrerror (int error_no)
{
  static char buf[500];
  DWORD ret;

  if (error_no == 0)
    error_no = WSAGetLastError ();

  ret = FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM |
                       FORMAT_MESSAGE_IGNORE_INSERTS,
                       NULL,
                       error_no,
                       0, /* choose most suitable language */
                       buf, sizeof (buf), NULL);

  while (ret > 0 && (buf[ret - 1] == '\n' ||
                     buf[ret - 1] == '\r' ))
      --ret;
  buf[ret] = '\0';
  if (!ret)
    sprintf (buf, "Winsock error %u", error_no);

  return buf;
}
