/*
 * cfg.h -- declarations for configuration file routines
 *
 * Lars Wirzenius
 */

#ifndef __publib_cfg_h_included
#define __publib_cfg_h_included

#include <publib/alloc.h>

enum cfg_type {
	CFG_LONG,
	CFG_LONG_EXPR,
	CFG_ULONG,
	CFG_ULONG_EXPR,
	CFG_DOUBLE,
	CFG_DOUBLE_EXPR,
	CFG_STRING,
	CFG_BOOLEAN
};

enum cfg_flags {
	cfg_read_only = 0x01,
	cfg_saved = 0x02,
	cfg_set_from_array = 0x04
};

struct cfg_variable {
	const char *cfg_name;
	enum cfg_type cfg_type;
	void *cfg_value;
	unsigned cfg_flags;
	int cfg_option;
#if 0
	/* these are ideas for new functionality */
	unsigned cfg_modified:1;
	int (*cfg_is_ok)(void *);
#endif
};

const char *cfg_error(void);
int cfg_from_array(struct cfg_variable *, char **);
int cfg_read_file(struct cfg_variable *, const char *);
int cfg_write_file(struct cfg_variable *, const char *);
int cfg_rewrite_file(struct cfg_variable *, const char *);

#endif
