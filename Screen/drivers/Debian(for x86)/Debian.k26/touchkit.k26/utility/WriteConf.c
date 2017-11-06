
#include <stdio.h>
#include <sys/time.h>
#include "config.h"
#include "tpanel.h"

int main(int argc, char **argv)
{	FILE *file;

	if ( !(file=fopen(IDcat(PRJNAME, _CONF), "w+")) )
	{	perror("fopen");
		return 1;
	} 

	for ( argv++; argv[0]; argv++ )
	{	fprintf(file, "%s\n", argv[0]);
	}
	fclose(file);
	return 0;
}

