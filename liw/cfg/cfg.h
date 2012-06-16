/*
 * cfg.h -- declarations for configuration file routines
 *
 * Lars Wirzenius
 */

#ifndef __publib_cfg_h_included
#define __publib_cfg_h_included

#include <publib/alloc.h>
#include <stdio.h>

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
	cfg_set_from_array = 0x04,
	cfg_removed = 0x08,
	cfg_userflag1 = 0x100,
	cfg_userflag2 = 0x200,
	cfg_userflag3 = 0x400,
	cfg_userflag4 = 0x800,
	cfg_userflag5 = 0x1000,
	cfg_userflag6 = 0x2000,
	cfg_userflag7 = 0x4000,
	cfg_userflag8 = 0x8000
};

struct cfg_unknown_variable {
	char *cfg_name;
	char *cfg_value;
	unsigned cfg_flags;
	struct cfg_unknown_variable *cfg_next;
};

struct cfg_variable {
	const char *cfg_name;
	enum cfg_type cfg_type;
	void *cfg_value;
	unsigned cfg_flags;
	int cfg_option;
};

const char *cfg_error(void);
int cfg_from_array(struct cfg_variable *, char **);
int cfg_read_file(struct cfg_variable *, const char *);
int cfg_read_open_file(struct cfg_variable *, const char *, FILE *);
int cfg_write_file(struct cfg_variable *, const char *);
int cfg_write_open_file(struct cfg_variable *, const char *, FILE *);
int cfg_rewrite_file(struct cfg_variable *, const char *);

struct cfg_unknown_variable *cfg_find_unknown(struct cfg_variable *,
						const char *);
void cfg_reset_some_flags(struct cfg_variable *, unsigned);

#endif
