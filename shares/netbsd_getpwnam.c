#include "includes.h"
#include "dbutil.h"
#include "session.h"
#include "buffer.h"
#include "ssh.h"
#include "packet.h"
#include "auth.h"
#include "runopts.h"

#include <string.h>
#include <stdio.h>

int getpenv(FILE *fp, struct passwd *pw, char *buf, int len) 
{
	char *p, *idx;

	if (!fp) return -1;
	
	while (1) {
		idx = fgets(buf, len, fp);
		if (!idx)
			return -1;
		if (*idx == '#') continue;

		// get user name
		p = idx;
		if (!(idx = strchr(p, ':'))) continue;
		*idx++ = '\0';
		pw->pw_name = p;

		// get passwd
		p = idx;
		if (!(idx = strchr(p, ':'))) continue;
		*idx++ = '\0';
		pw->pw_passwd = p;
		
		// get uid
		p = idx;
		if (!(idx = strchr(p, ':'))) continue;
		*idx++ = '\0';
		pw->pw_uid = atoi(p);

		// get gid
		p = idx;
		if (!(idx = strchr(p, ':'))) continue;
		*idx++ = '\0';
		pw->pw_gid = atoi(p);

		// get gecos
		p = idx;
		if (!(idx = strchr(p, ':'))) continue;
		*idx++ = '\0';
		// pw->pw_gecos = p;

		// get dir
		p = idx;
		if (!(idx = strchr(p, ':'))) continue;
		*idx++ = '\0';
		pw->pw_dir = p;

		// get shell
		p = idx;
		idx = strlen(p) + p - 1;
		while (*idx == ' ' || *idx == '\t' || *idx == '\n' || *idx == '\r')
			*idx-- = '\0';
		pw->pw_shell = p;
		return 0;
	}
	return -1;
}

struct passwd* getpwnam(const char *login)
{
	static struct passwd pass;
	static char buffer[1024];
	FILE *fp;

    TRACE(("entering fake-getpwnam"));

	fp = fopen("/etc/passwd", "r");
	while (getpenv(fp, &pass, buffer, sizeof(buffer)) == 0) {
		if (!strcmp(pass.pw_name , login)) {
			return &pass;
		}
	}

	fclose(fp);
	
    TRACE(("leaving fake-getpwnam"));

    return NULL;

}

