#include <stdio.h>
#include <stdlib.h>
#include "publib/cfg.h"

static char *newsgroup_name = NULL;
static unsigned long expire_time = 7UL * 24 * 60 * 60;
static double double_expr = 0.0;
static double double_var = 0.0;
static int rewrite = 0;

static struct cfg_variable variables[] = {
	{ "newsgroup", CFG_STRING, &newsgroup_name, 0, 'n' },
	{ "expire-time", CFG_ULONG_EXPR, &expire_time, 0, 'e' },
	{ "double-expr", CFG_DOUBLE_EXPR, &double_expr, 0, 'd' },
	{ "double-var", CFG_DOUBLE, &double_var, 0, 'D' },
	{ "rewrite", CFG_BOOLEAN, &rewrite, 0, 'r' },
	{ NULL }
};


int main(int argc, char **argv) {
	int ret;

	if (cfg_from_array(variables, argv + 1) == -1) {
		fprintf(stderr, "error: %s\n", cfg_error());
		exit(EXIT_FAILURE);
	}
	
	if (argv[1] == NULL || argv[2] == NULL) {
		fprintf(stderr, "error: no config file named\n");
		exit(EXIT_FAILURE);
	}

	if (cfg_read_file(variables, argv[1]) == -1) {
		fprintf(stderr, "error: %s\n", cfg_error());
		exit(EXIT_FAILURE);
	}

	if (rewrite)
		ret = cfg_rewrite_file(variables, argv[2]);
	else
		ret = cfg_write_file(variables, argv[2]);
	if (ret == -1) {
		fprintf(stderr, "error: %s\n", cfg_error());
		exit(EXIT_FAILURE);
	}

	return 0;
}
