/*
 * File contexts backend for labeling system
 *
 * Author : Eamon Walsh <ewalsh@tycho.nsa.gov>
 */

#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <regex.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "callbacks.h"
#include "label_internal.h"

/*
 * Internals, mostly moved over from matchpathcon.c
 */

/* A file security context specification. */
typedef struct spec {
	struct selabel_lookup_rec lr;	/* holds contexts for lookup result */
	char *regex_str;	/* regular expession string for diagnostics */
	char *type_str;		/* type string for diagnostic messages */
	regex_t regex;		/* compiled regular expression */
	char regcomp;           /* regex_str has been compiled to regex */
	mode_t mode;		/* mode format value */
	int matches;		/* number of matching pathnames */
	int hasMetaChars;	/* regular expression has meta-chars */
	int stem_id;		/* indicates which stem-compression item */
	size_t prefix_len;      /* length of fixed path prefix */
} spec_t;

/* A regular expression stem */
typedef struct stem {
	char *buf;
	int len;
} stem_t;

/* Our stored configuration */
struct saved_data {
	/*
	 * The array of specifications, initially in the same order as in 
	 * the specification file. Sorting occurs based on hasMetaChars.
	 */
	spec_t *spec_arr;
	unsigned int nspec;
	unsigned int ncomp;

	/*
	 * The array of regular expression stems.
	 */
	stem_t *stem_arr;
	int num_stems;
	int alloc_stems;
};


static void my_stats(struct selabel_handle *rec)
{
	struct saved_data *data = (struct saved_data *)rec->data;
	
	unsigned int i, nspec = data->nspec;
	
	spec_t *spec_arr = data->spec_arr;

	//fprintf(stderr, "selinux my_stats - nspec : %u\n", nspec);
	
	for (i = 0; i < nspec; i++) 
	{
		//fprintf(stderr, "selinux my_stats - spec_arr[%u].type_str : %x\n", i, spec_arr[i].type_str);
		
		if (spec_arr[i].type_str) 
		{			
			fprintf(stderr, "selinux_stats - type : %u (%s, %s)\n", 
				i,
			    spec_arr[i].regex_str,
			    //spec_arr[i].type_str,
			    spec_arr[i].lr.ctx_raw);
	
		} 
		else 
		{
			//fprintf(stderr, "selinux_stats - noty : %u (%s, %s)\n", \
				i,\
			    spec_arr[i].regex_str,\
			    //spec_arr[i].type_str,\
			    spec_arr[i].lr.ctx_raw);
		}
	}
}


/* Return the length of the text that can be considered the stem, returns 0
 * if there is no identifiable stem */
static int get_stem_from_spec(const char *const buf)
{
	const char *tmp = strchr(buf + 1, '/');
	const char *ind;

	if (!tmp)
		return 0;

	for (ind = buf; ind < tmp; ind++) {
		if (strchr(".^$?*+|[({", (int)*ind))
			return 0;
	}
	return tmp - buf;
}

/* return the length of the text that is the stem of a file name */
static int get_stem_from_file_name(const char *const buf)
{
	const char *tmp = strchr(buf + 1, '/');

	if (!tmp)
		return 0;
	return tmp - buf;
}

/* find the stem of a file spec, returns the index into stem_arr for a new
 * or existing stem, (or -1 if there is no possible stem - IE for a file in
 * the root directory or a regex that is too complex for us). */
static int find_stem_from_spec(struct saved_data *data, const char *buf)
{
	int i, num = data->num_stems;
	int stem_len = get_stem_from_spec(buf);

	if (!stem_len)
	{
		return -1;
	}
	
	for (i = 0; i < num; i++) 
	{
		if (stem_len == data->stem_arr[i].len && !strncmp(buf, data->stem_arr[i].buf, stem_len))
		{
			return i;
		}
	}
	
	if (data->alloc_stems == num) 
	{

stem_t *tmp_arr;
		
		data->alloc_stems = data->alloc_stems * 2 + 16;
		
		tmp_arr = (stem_t *) realloc(data->stem_arr,
				  sizeof(stem_t) * data->alloc_stems);
		
		if (!tmp_arr)
		{
			return -1;
		}
		
		data->stem_arr = tmp_arr;
	}
	
	data->stem_arr[num].len = stem_len;
	
	data->stem_arr[num].buf = (char *) malloc(stem_len + 1);
	
	if (!data->stem_arr[num].buf)
	{
		return -1;
	}
	
	memcpy(data->stem_arr[num].buf, buf, stem_len);
	
	data->stem_arr[num].buf[stem_len] = '\0';
	
	data->num_stems++;
	
	buf += stem_len;
	
	return num;
}

/* find the stem of a file name, returns the index into stem_arr (or -1 if
 * there is no match - IE for a file in the root directory or a regex that is
 * too complex for us).  Makes buf point to the text AFTER the stem. */
static int find_stem_from_file(struct saved_data *data, const char **buf)
{
	int i;
	
	int stem_len = get_stem_from_file_name(*buf);

	if (!stem_len)
	{
		return -1;
	}
	
	//fprintf(stderr, "selinux - find_stem_from_file stem_len : %d\n", stem_len);
	
	for (i = 0; i < data->num_stems; i++) 
	{
		if (stem_len == data->stem_arr[i].len && !strncmp(*buf, data->stem_arr[i].buf, stem_len)) 
		{
			*buf += stem_len;
			
			//fprintf(stderr, "selinux - find_stem_from_file *buf : %s\n", *buf);
			
			//fprintf(stderr, "selinux - find_stem_from_file data->stem_arr[i].buf : %s\n", data->stem_arr[i].buf);
			
			return i;
		}
	}
	
	return -1;
}

/*
 * Warn about duplicate specifications.
 */
static int nodups_specs(struct saved_data *data, const char *path)
{
	int rc = 0;
	unsigned int ii, jj;
	struct spec *curr_spec, *spec_arr = data->spec_arr;

	for (ii = 0; ii < data->nspec; ii++) {
		curr_spec = &spec_arr[ii];
		for (jj = ii + 1; jj < data->nspec; jj++) {
			if ((!strcmp
			     (spec_arr[jj].regex_str, curr_spec->regex_str))
			    && (!spec_arr[jj].mode || !curr_spec->mode
				|| spec_arr[jj].mode == curr_spec->mode)) {
				rc = -1;
				errno = EINVAL;
				if (strcmp
				    (spec_arr[jj].lr.ctx_raw,
				     curr_spec->lr.ctx_raw)) {
					selinux_log
						(SELINUX_ERROR,
						 "%s: Multiple different specifications for %s  (%s and %s).\n",
						 path, curr_spec->regex_str,
						 spec_arr[jj].lr.ctx_raw,
						 curr_spec->lr.ctx_raw);
				} else {
					selinux_log
						(SELINUX_ERROR,
						 "%s: Multiple same specifications for %s.\n",
						 path, curr_spec->regex_str);
				}
			}
		}
	}
	return rc;
}

/* Determine if the regular expression specification has any meta characters. */
static void spec_hasMetaChars(struct spec *spec)
{
	char *c;
	size_t len;
	char *end;

	c = spec->regex_str;
	len = strlen(spec->regex_str);
	end = c + len;

	spec->hasMetaChars = 0;
	spec->prefix_len = len;

	/* Look at each character in the RE specification string for a 
	 * meta character. Return when any meta character reached. */
	while (c != end) {
		switch (*c) {
		case '.':
		case '^':
		case '$':
		case '?':
		case '*':
		case '+':
		case '|':
		case '[':
		case '(':
		case '{':
			spec->hasMetaChars = 1;
			spec->prefix_len = c - spec->regex_str;
			return;
		case '\\':	/* skip the next character */
			c++;
			break;
		default:
			break;

		}
		c++;
	}
	return;
}

static int compile_regex(struct saved_data *data, spec_t *spec, char **errbuf)
{
	char *reg_buf, *anchored_regex, *cp;
	stem_t *stem_arr = data->stem_arr;
	size_t len;
	int regerr;

	//fprintf(stderr, "selinux compile_regex enter : spec->regex_str : %s\n", spec->regex_str);
	
	if (spec->regcomp)
	{
		return 0; /* already done */
	}

	data->ncomp++; /* how many compiled regexes required */

	/* Skip the fixed stem. */
	reg_buf = spec->regex_str;
	
	if (spec->stem_id >= 0)
	{
		reg_buf += stem_arr[spec->stem_id].len;
	}
	
	//fprintf(stderr, "selinux compile_regex reg_buf : %s\n", reg_buf);

	/* Anchor the regular expression. */
	len = strlen(reg_buf);
	cp = anchored_regex = (char *) malloc(len + 3);
	if (!anchored_regex)
		return -1;
	/* Create ^...$ regexp.  */
	*cp++ = '^';
	memcpy(cp, reg_buf, len);
	cp += len;
	*cp++ = '$';
	*cp = '\0';
	
	//fprintf(stderr, "selinux compile_regex after anchored_regex : %s\n", anchored_regex);

	/* Compile the regular expression. */
	regerr = regcomp(&spec->regex, anchored_regex, 
			 REG_EXTENDED | REG_NOSUB);
			 
	if (regerr != 0) 
	{
		size_t errsz = 0;
		
		errsz = regerror(regerr, &spec->regex, NULL, 0);
		
		if (errsz && errbuf)
		{
			//fprintf(stderr, "selinux compile_regex enter if (errsz && errbuf)\n");
			
			*errbuf = (char *) malloc(errsz);
		}
		
		//fprintf(stderr, "selinux compile_regex after if (errsz && errbuf)\n");
		
		
		if (errbuf && *errbuf)
		{
			//fprintf(stderr, "selinux compile_regex enter if (errbuf && *errbuf)\n");
			
			(void)regerror(regerr, &spec->regex,
				       *errbuf, errsz);
					   
					   			//fprintf(stderr, "selinux compile_regex - \
				regerror(regerr : %d, \
				reg_buf : %s, \
				errbuf : %s)\n",\
				regerr,\
				reg_buf,\
				*errbuf\
				);
		}
				
		//fprintf(stderr, "selinux compile_regex after if (errbuf && *errbuf)\n");

		free(anchored_regex);
		return -1;
	}
	
	//fprintf(stderr, "selinux compile_regex after if (regerr != 0) \n");
	
	free(anchored_regex);

	/* Done. */
	spec->regcomp = 1;

	return 0;
}

static int process_line(struct selabel_handle *rec,
			const char *path, const char *prefix,
			char *line_buf, int pass, unsigned lineno)
{
	int items, len;
	char buf1[BUFSIZ], buf2[BUFSIZ], buf3[BUFSIZ];
	char *buf_p, *regex = buf1, *type = buf2, *context = buf3;
	struct saved_data *data = (struct saved_data *)rec->data;
	spec_t *spec_arr = data->spec_arr;
	unsigned int nspec = data->nspec;

	//fprintf(stderr, "selinux - process_line - data->spec_arr %x\n", data->spec_arr);
	
	len = strlen(line_buf);
	if (line_buf[len - 1] == '\n')
		line_buf[len - 1] = 0;
	buf_p = line_buf;
	while (isspace(*buf_p))
		buf_p++;
	
	/* Skip comment lines and empty lines. */
	if (*buf_p == '#' || *buf_p == 0)
	{
		return 0;
	}

	//chao #define DELIM_CHAR '\t'
	//items = sscanf(line_buf, "%255s %255s %255s", regex, type, context);
	items = sscanf(line_buf, "%255[^\t]\t%255[^\t]\t%255s", regex, type, context);
	
	if (items < 2) {
		selinux_log(SELINUX_WARNING,
			    "%s:  line %d is missing fields, skipping\n", path,
			    lineno);
		return 0;
	} else if (items == 2) {
		/* The type field is optional. */
		//chao
		//context = type;
		items = sscanf(line_buf, "%255[^\t]\t%255s", regex, context);
		type = NULL;
	}

	//fprintf(stderr, "selinux - process_line - regex : |%s| \n", regex);
	
	//fprintf(stderr, "selinux - process_line - context : |%s| \n", context);
	
	//fprintf(stderr, "selinux - process_line - items : %d - regex : %s - context : %s\n", items, regex, context);

	len = get_stem_from_spec(regex);
	
	if (len && prefix && strncmp(prefix, regex, len)) 
	{
		/* Stem of regex does not match requested prefix, discard. */
		return 0;
	}
	
	//DEBUG
	if(!strcmp(regex, "/system/build.prop"))
	{
		//fprintf(stderr, "selinux - process_line before if(pass == 1)\n", regex);
	}

	if (pass == 1) 
	{
		/* On the second pass, process and store the specification in spec. */
		char *errbuf = NULL;
		
		spec_arr[nspec].stem_id = find_stem_from_spec(data, regex);
		
		spec_arr[nspec].regex_str = strdup(regex);
		
		//DEBUG
		if(!strcmp(regex, "/system/build.prop"))
		{
			//fprintf(stderr, "selinux - process_line pass1 nspec=%u\n", nspec);
		}
		
		if (!spec_arr[nspec].regex_str) 
		{
			selinux_log(SELINUX_WARNING,
				   "%s:  out of memory at line %d on regex %s\n",
				   path, lineno, regex);
			return -1;

		}
		
		if (rec->validating && compile_regex(data, &spec_arr[nspec], &errbuf)) {
			selinux_log(SELINUX_WARNING,
				   "%s:  line %d has invalid regex %s:  %s\n",
				   path, lineno, regex,
				   (errbuf ? errbuf : "out of memory"));
		}
		
		//DEBUG
		if(!strcmp(regex, "/system/build.prop"))
		{
			//fprintf(stderr, "selinux - pass1 - after validating\n");
		}

		/* Convert the type string to a mode format */
		spec_arr[nspec].mode = 0;
		
		if (!type)
		{
			goto skip_type;
		}
		
		//DEBUG
		if(!strcmp(regex, "/system/build.prop"))
		{
			//fprintf(stderr, "selinux - pass1 - after type\n");
		}
		
		spec_arr[nspec].type_str = strdup(type);
		
		len = strlen(type);
		
		if (type[0] != '-' || len != 2) 
		{
			selinux_log(SELINUX_WARNING,
				    "%s:  line %d has invalid file type %s\n",
				    path, lineno, type);
					
			return 0;
		}
		
		//DEBUG
		if(!strcmp(regex, "/system/build.prop"))
		{
			//fprintf(stderr, "selinux - pass1 - after len\n");
		}
		
		switch (type[1]) 
		{
		case 'b':
			spec_arr[nspec].mode = S_IFBLK;
			break;
		case 'c':
			spec_arr[nspec].mode = S_IFCHR;
			break;
		case 'd':
			spec_arr[nspec].mode = S_IFDIR;
			break;
		case 'p':
			spec_arr[nspec].mode = S_IFIFO;
			break;
		case 'l':
			spec_arr[nspec].mode = S_IFLNK;
			break;
		case 's':
			spec_arr[nspec].mode = S_IFSOCK;
			break;
		case '-':
			spec_arr[nspec].mode = S_IFREG;
			break;
		default:
			selinux_log(SELINUX_WARNING,
				    "%s:  line %d has invalid file type %s\n",
				    path, lineno, type);
			return 0;
		}

	skip_type:
	
		spec_arr[nspec].lr.ctx_raw = strdup(context);
		
		//DEBUG
		if(!strcmp(regex, "/system/build.prop"))
		{
			//fprintf(stderr, "selinux - pass1 - skip_type\n");
		}

		if (strcmp(context, "<<none>>") && rec->validating) 
		{
			if (selabel_validate(rec, &spec_arr[nspec].lr) < 0) {
				selinux_log(SELINUX_WARNING,
					    "%s:  line %d has invalid context %s\n",
					    path, lineno, spec_arr[nspec].lr.ctx_raw);
			}
		}

		/* Determine if specification has 
		 * any meta characters in the RE */
		spec_hasMetaChars(&spec_arr[nspec]);
	}

	
	//DEBUG
	
	////fprintf(stderr, "selinux - process_line before stats\n", regex);
	
	//fprintf(stderr, "\nmain - before stats\n");
	
	//my_stats(rec);
	
	////fprintf(stderr, "selinux - process_line after stats\n", regex);
	
	//fprintf(stderr, "\nmain - after stats\n");

	data->nspec = ++nspec;
	
	return 0;
}

static int init(struct selabel_handle *rec, const struct selinux_opt *opts,
		unsigned n)
{
	struct saved_data *data = (struct saved_data *)rec->data;
	const char *path = NULL;
	const char *prefix = NULL;
	FILE *fp;
	FILE *localfp = NULL;
	FILE *homedirfp = NULL;
	char local_path[PATH_MAX + 1];
	char homedir_path[PATH_MAX + 1];
	char line_buf[BUFSIZ];
	unsigned int lineno, pass, i, j, maxnspec;
	spec_t *spec_copy = NULL;
	int status = -1, baseonly = 0;
	struct stat sb;

	//fprintf(stderr, "selinux enter init\n");
	
	/* Process arguments */
	while (n--)
		switch(opts[n].type) {
		case SELABEL_OPT_PATH:
			path = opts[n].value;
			break;
		case SELABEL_OPT_SUBSET:
			prefix = opts[n].value;
			break;
		case SELABEL_OPT_BASEONLY:
			baseonly = !!opts[n].value;
			break;
		}

	/* Open the specification file. */
	if ((fp = fopen(path, "r")) == NULL)
		return -1;

	if (fstat(fileno(fp), &sb) < 0)
		return -1;
	if (!S_ISREG(sb.st_mode)) {
		errno = EINVAL;
		return -1;
	}

	if (!baseonly) {
		snprintf(homedir_path, sizeof(homedir_path), "%s.homedirs",
			 path);
		homedirfp = fopen(homedir_path, "r");

		snprintf(local_path, sizeof(local_path), "%s.local", path);
		localfp = fopen(local_path, "r");
	}

	//fprintf(stderr, "selinux init before two pass\n");
	
	/* 
	 * Perform two passes over the specification file.
	 * The first pass counts the number of specifications and
	 * performs simple validation of the input.  At the end
	 * of the first pass, the spec array is allocated.
	 * The second pass performs detailed validation of the input
	 * and fills in the spec array.
	 */
	maxnspec = UINT_MAX / sizeof(spec_t);
	
	//fprintf(stderr, "selinux init maxnspec: %u\n", maxnspec);
	
	for (pass = 0; pass < 2; pass++) 
	{
		lineno = 0;
		data->nspec = 0;
		data->ncomp = 0;
		
		//DEBUG
		//fprintf(stderr, "selinux init - before while\n");
		
		while (
			fgets(line_buf, sizeof line_buf - 1, fp) 
			&& 
			data->nspec < maxnspec ) 
		{
			//fprintf(stderr, "selinux init - in while sizeof line_buf - 1 : %u\n", sizeof line_buf - 1);
			
			//fprintf(stderr, "selinux init - in while data->nspec : %u\n", data->nspec);
			
			if (process_line(rec, path, prefix, line_buf, pass, ++lineno) != 0)
			{
				//fprintf(stderr, "selinux init - in while goto finish!\n");
				goto finish;
			}
			
			
			
		}
		
		//DEBUG
		//fprintf(stderr, "selinux init - after while\n");
		
		if (pass == 1) {
			status = nodups_specs(data, path);
			if (status)
				goto finish;
		}
		
		lineno = 0;
		
		//DEBUG
		//fprintf(stderr, "selinux init - before homedirfp\n");
		
		if (homedirfp)
		{
			while (fgets(line_buf, sizeof line_buf - 1, homedirfp)
			       && data->nspec < maxnspec) 
			{
				if (process_line
				    (rec, homedir_path, prefix,
				     line_buf, pass, ++lineno) != 0)
				{
					goto finish;
				}
			}
		}	

		lineno = 0;
		
		//DEBUG
		//fprintf(stderr, "selinux init - before localfp\n");
		
		if (localfp)
		{
			while (fgets(line_buf, sizeof line_buf - 1, localfp)
			       && data->nspec < maxnspec) {
				if (process_line
				    (rec, local_path, prefix, line_buf,
				     pass, ++lineno) != 0)
					goto finish;
			}
		}
		
		//DEBUG
		//fprintf(stderr, "selinux init - before if (pass == 0)\n");

		if (pass == 0) 
		{
			if (data->nspec == 0) 
			{
				status = 0;
				goto finish;
			}
			
			//DEBUG
			//fprintf(stderr, "selinux init - data->nspec : %u", data->nspec);
			
			if (NULL == (data->spec_arr =
				     (spec_t *) malloc(sizeof(spec_t) * data->nspec)))
			{
				goto finish;
			}
				
			memset(data->spec_arr, 0, sizeof(spec_t)*data->nspec);
			
			maxnspec = data->nspec;
			
			
			
			rewind(fp);
			
			if (homedirfp)
			{
				rewind(homedirfp);
			}
			if (localfp)
			{
				rewind(localfp);
			}
		}
		
		//DEBUG
		if(pass == 0)
		{
			//fprintf(stderr, "selinux init - data->nspec: %u", maxnspec);
		}
	}
	
	//debug
	
	//fprintf(stderr, "selinux init - before my_stats");
			
	my_stats(rec);
			
	//fprintf(stderr, "selinux init - after my_stats");

	/* Move exact pathname specifications to the end. */
	spec_copy = (spec_t *) malloc(sizeof(spec_t) * data->nspec);
	if (!spec_copy)
		goto finish;
	j = 0;
	for (i = 0; i < data->nspec; i++)
		if (data->spec_arr[i].hasMetaChars)
			memcpy(&spec_copy[j++],
			       &data->spec_arr[i], sizeof(spec_t));
	for (i = 0; i < data->nspec; i++)
		if (!data->spec_arr[i].hasMetaChars)
			memcpy(&spec_copy[j++],
			       &data->spec_arr[i], sizeof(spec_t));
	free(data->spec_arr);
	data->spec_arr = spec_copy;

	status = 0;
finish:
	fclose(fp);
	if (data->spec_arr != spec_copy)
		free(data->spec_arr);
	if (homedirfp)
		fclose(homedirfp);
	if (localfp)
		fclose(localfp);
	return status;
}

/*
 * Backend interface routines
 */
static void closef(struct selabel_handle *rec)
{
	struct saved_data *data = (struct saved_data *)rec->data;
	struct spec *spec;
	struct stem *stem;
	unsigned int i;

	for (i = 0; i < data->nspec; i++) {
		spec = &data->spec_arr[i];
		free(spec->regex_str);
		free(spec->type_str);
		free(spec->lr.ctx_raw);
		free(spec->lr.ctx_trans);
		if (spec->regcomp)
			regfree(&spec->regex);
	}

	for (i = 0; i < (unsigned int)data->num_stems; i++) {
		stem = &data->stem_arr[i];
		free(stem->buf);
	}

	if (data->spec_arr)
		free(data->spec_arr);
	if (data->stem_arr)
		free(data->stem_arr);
	
	free(data);
}

static spec_t *lookup_common(struct selabel_handle *rec,
			     const char *key,
			     int type,
			     bool partial)
{
	struct saved_data *data = (struct saved_data *)rec->data;
	spec_t *spec_arr = data->spec_arr;
	int i, rc, file_stem;
	mode_t mode = (mode_t)type;
	const char *buf;
	spec_t *ret = NULL;
	char *clean_key = NULL;
	const char *prev_slash, *next_slash;
	unsigned int sofar = 0;
	size_t keylen = strlen(key);

	if (!data->nspec) {
		errno = ENOENT;
		goto finish;
	}

	/* Remove duplicate slashes */
	if ((next_slash = strstr(key, "//"))) {
		clean_key = (char *) malloc(strlen(key) + 1);
		if (!clean_key)
			goto finish;
		prev_slash = key;
		while (next_slash) {
			memcpy(clean_key + sofar, prev_slash, next_slash - prev_slash);
			sofar += next_slash - prev_slash;
			prev_slash = next_slash + 1;
			next_slash = strstr(prev_slash, "//");
		}
		strcpy(clean_key + sofar, prev_slash);
		key = clean_key;
	}

	buf = key;
	
	//fprintf(stderr, "selinux - before find_stem_from_file\n");
	
	file_stem = find_stem_from_file(data, &buf);
	
	//fprintf(stderr, "selinux - after find_stem_from_file\n");
	
	mode &= S_IFMT;

	/* 
	 * Check for matching specifications in reverse order, so that
	 * the last matching specification is used.
	 */
	for (i = data->nspec - 1; i >= 0; i--) 
	{
		/* if the spec in question matches no stem or has the same
		 * stem as the file AND if the spec in question has no mode
		 * specified or if the mode matches the file mode then we do
		 * a regex check        */
		 
		//fprintf(stderr, \
		"selinux - lookup_common  **** key : %s - spec_arr[%d].regex_str : %s\n", \
		key, \
		i, \
		spec_arr[i].regex_str);
		 
		if ((spec_arr[i].stem_id == -1
		     || spec_arr[i].stem_id == file_stem)
		    && (!mode || !spec_arr[i].mode
			|| mode == spec_arr[i].mode)) 
		{
			char *error_buffer;
			
			//fprintf(stderr, "selinux - lookup_common  key : %s enter if\n", key); 

//			if (compile_regex(data, &spec_arr[i], NULL) < 0)
	
			if (compile_regex(data, &spec_arr[i], &error_buffer) < 0)
			{
				//fprintf(stderr, "selinux - lookup_common  error_buffer : %s\n", *error_buffer); 
								
				goto finish;
			}
			
			//fprintf(stderr, "selinux - lookup_common  key : %s after compile_regex\n", key); 
			
			if (spec_arr[i].stem_id == -1)
			{
				//fprintf(stderr, "selinux - lookup_common  spec_arr[i].stem_id == -1 key : %s\n", key); 
				
				rc = regexec(&spec_arr[i].regex, key, 0, 0, 0);
			}
			else
			{
				//fprintf(stderr, "selinux - lookup_common  spec_arr[i].stem_id != -1 buf : %s\n", buf); 
				
				rc = regexec(&spec_arr[i].regex, buf, 0, 0, 0);
			}
			
			//fprintf(stderr, "selinux - lookup_common  key : %s after if (spec_arr[i].stem_id == -1)\n", key); 
			
			//fprintf(stderr, "selinux - lookup_common  key : %s rc=%d\n", key, rc); 

			if (rc == 0) 
			{
				spec_arr[i].matches++;
				break;
			}
			
			//fprintf(stderr, "selinux - lookup_common  key : %s after if (rc == 0)\n", key); 

			if (partial) 
			{
				/*
				 * We already checked above to see if the
				 * key has any direct match.  Now we just need
				 * to check for partial matches.
				 * Since POSIX regex functions do not support
				 * partial match, we crudely approximate it
				 * via a prefix match.
				 * This is imprecise and could yield
				 * false positives or negatives but
				 * appears to work with our current set of
				 * regex strings.
				 * Convert to using pcre partial match
				 * if/when pcre becomes available in Android.
				 */
				if (spec_arr[i].prefix_len > 1 &&
				    !strncmp(key, spec_arr[i].regex_str,
					     keylen < spec_arr[i].prefix_len ?
					     keylen : spec_arr[i].prefix_len))
				{
					break;
				}
			}

			if (rc == REG_NOMATCH)
			{
				continue;
			}
			/* else it's an error */
			goto finish;
		}
	}

	if (i < 0 || strcmp(spec_arr[i].lr.ctx_raw, "<<none>>") == 0) {
		/* No matching specification. */
		errno = ENOENT;
		goto finish;
	}

	ret = &spec_arr[i];

finish:
	
	free(clean_key);
	
	return ret;
}

static struct selabel_lookup_rec *lookup(struct selabel_handle *rec,
					 const char *key, int type)
{
	spec_t *spec;
	spec = lookup_common(rec, key, type, false);
	if (spec)
		return &spec->lr;
	return NULL;
}

static bool partial_match(struct selabel_handle *rec, const char *key)
{
	return lookup_common(rec, key, 0, true) ? true : false;
}

static struct selabel_lookup_rec *lookup_best_match(struct selabel_handle *rec,
						    const char *key,
						    const char **aliases,
						    int type)
{
	size_t n, i;
	int best = -1;
	spec_t **specs;
	size_t prefix_len = 0;
	struct selabel_lookup_rec *lr = NULL;

	if (!aliases || !aliases[0])
		return lookup(rec, key, type);

	for (n = 0; aliases[n]; n++)
		;

	specs = calloc(n+1, sizeof(spec_t *));
	if (!specs)
		return NULL;
	specs[0] = lookup_common(rec, key, type, false);
	if (specs[0]) {
		if (!specs[0]->hasMetaChars) {
			/* exact match on key */
			lr = &specs[0]->lr;
			goto out;
		}
		best = 0;
		prefix_len = specs[0]->prefix_len;
	}
	for (i = 1; i <= n; i++) {
		specs[i] = lookup_common(rec, aliases[i-1], type, false);
		if (specs[i]) {
			if (!specs[i]->hasMetaChars) {
				/* exact match on alias */
				lr = &specs[i]->lr;
				goto out;
			}
			if (specs[i]->prefix_len > prefix_len) {
				best = i;
				prefix_len = specs[i]->prefix_len;
			}
		}
	}

	if (best >= 0) {
		/* longest fixed prefix match on key or alias */
		lr = &specs[best]->lr;
	}

out:
	free(specs);
	return lr;
}

static void stats(struct selabel_handle *rec)
{
	struct saved_data *data = (struct saved_data *)rec->data;
	
	unsigned int i, nspec = data->nspec;
	
	spec_t *spec_arr = data->spec_arr;

	//fprintf(stderr, "selinux stats - nspec : %u\n", nspec);
	
	for (i = 0; i < nspec; i++) 
	{
		if (spec_arr[i].matches == 0) 
		{
			if (spec_arr[i].type_str) 
			{
				
				//fprintf(stderr, "selinux stats - type : %u\n", nspec);
				
				selinux_log(SELINUX_WARNING,
				    "Warning!  No matches for (%s, %s, %s)\n",
				    spec_arr[i].regex_str,
				    spec_arr[i].type_str,
				    spec_arr[i].lr.ctx_raw);
					
					
			} 
			else 
			{
				selinux_log(SELINUX_WARNING,
				    "Warning!  No matches for (%s, %s)\n",
				    spec_arr[i].regex_str,
				    spec_arr[i].lr.ctx_raw);
			}
		}
	}
}

int selabel_file_init(struct selabel_handle *rec, const struct selinux_opt *opts,
		      unsigned nopts)
{
	
	struct saved_data *data;

	//fprintf(stderr, "selinux - selabel_file_init - enter\n");
	
	data = (struct saved_data *)malloc(sizeof(*data));
	if (!data)
		return -1;
	memset(data, 0, sizeof(*data));

	rec->data = data;
	rec->func_close = &closef;
	rec->func_stats = &stats;
	rec->func_lookup = &lookup;
	rec->func_partial_match = &partial_match;
	rec->func_lookup_best_match = &lookup_best_match;
	
	//fprintf(stderr, "selinux - selabel_file_init - before init\n");

	return init(rec, opts, nopts);
}
