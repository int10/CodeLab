#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *str_replace(char *orig, char *rep, char *with) 
{
    char *result; // the return string
    char *ins;    // the next insert point
    char *tmp;    // varies
    int len_rep;  // length of rep
    int len_with; // length of with
    int len_front; // distance between rep and end of last rep
    int count;    // number of replacements

    if (!orig)
        return NULL;
    if (!rep)
        rep = "";
    len_rep = strlen(rep);
    if (!with)
        with = "";
    len_with = strlen(with);

    ins = orig;
    for (count = 0; tmp = strstr(ins, rep); ++count) {
        ins = tmp + len_rep;
    }

    // first time through the loop, all the variable are set correctly
    // from here on,
    //    tmp points to the end of the result string
    //    ins points to the next occurrence of rep in orig
    //    orig points to the remainder of orig after "end of rep"
    tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

    if (!result)
        return NULL;

    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep; // move to next "end of rep"
    }
    strcpy(tmp, orig);
    return result;
}


char* make_reg_free_string(char *path)
{
	/*
	path.replace(QString("+"), QString("\\+"));
    path.replace("-","\\-");
    path.replace("^","\\^");
    path.replace("$","\\$");
    path.replace("*","\\*");
    path.replace("?","\\?");
    path.replace("{","\\{");
    path.replace("}","\\}");
    path.replace(".","\\.");
    path.replace("(","\\(");
    path.replace(")","\\)");
    path.replace(":","\\:");
    path.replace("=","\\=");
    path.replace("!","\\!");
    path.replace("|","\\|");
    path.replace("[","\\[");
    path.replace("]","\\]");
	*/
	
	char *result = NULL;
	
	char* result_1 = NULL;
	char* result_2 = NULL;
	char* result_3 = NULL;
	char* result_4 = NULL;
	char* result_5 = NULL;
	char* result_6 = NULL;
	char* result_7 = NULL;
	char* result_8 = NULL;
	char* result_9 = NULL;
	char* result_10 = NULL;
	char* result_11 = NULL;
	char* result_12 = NULL;
	char* result_13 = NULL;
	char* result_14 = NULL;
	char* result_15 = NULL;
	char* result_16 = NULL;
	
	//fprintf(stderr, "make_reg_free_string : %s\n", path);
	
	
	//
	result = str_replace(path, "+", "\\+");
	
	if(result == NULL)
	{
		return NULL;
	}
	
	path = result;
	
	//用于释放内存
	result_1 = result;
	
	//fprintf(stderr, "make_reg_free_string : %s\n", result);
	
	//
	result = str_replace(path, "-", "\\-"); 
	
	if(result == NULL)
	{
		return NULL;
	}
	
	path = result;
	
	result_2 = result;
	
	//fprintf(stderr, "make_reg_free_string : %s\n", result);
	
	//
	result = str_replace(path, "^", "\\^"); 
	
	if(result == NULL)
	{
		return NULL;
	}
	
	path = result;
	
	result_3 = result;
	
	//fprintf(stderr, "make_reg_free_string : %s\n", result);
	
	//
	result = str_replace(path, "$", "\\$"); 
	
	if(result == NULL)
	{
		return NULL;
	}
	
	path = result;
	
	result_4 = result;
	
	//fprintf(stderr, "make_reg_free_string : %s\n", result);
	
	//
	result = str_replace(path, "*", "\\*"); 
	
	if(result == NULL)
	{
		return NULL;
	}
	
	path = result;
	
	result_5 = result;
	
	//fprintf(stderr, "make_reg_free_string : %s\n", result);
	
	//
	result = str_replace(path, "?", "\\?"); 
	
	if(result == NULL)
	{
		return NULL;
	}
	
	path = result;
	
	result_6 = result;
	
	//fprintf(stderr, "make_reg_free_string : %s\n", result);
	
	//
	result = str_replace(path, "{", "\\{"); 
	
	if(result == NULL)
	{
		return NULL;
	}
	
	path = result;
	
	result_7 = result;
	
	//fprintf(stderr, "make_reg_free_string : %s\n", result);
	
	//
	result = str_replace(path, "}", "\\}");
	
	if(result == NULL)
	{
		return NULL;
	}
	
	path = result;
	
	result_8 = result;
	
	//fprintf(stderr, "make_reg_free_string : %s\n", result);
	
	//
	result = str_replace(path, ".", "\\.");
	
	if(result == NULL)
	{
		return NULL;
	}
	
	path = result;
	
	result_9 = result;
	
	//fprintf(stderr, "make_reg_free_string : %s\n", result);
		
	//
	result = str_replace(path, "(", "\\(");
	
	if(result == NULL)
	{
		return NULL;
	}
	
	path = result;
	
	result_10 = result;
	
	//fprintf(stderr, "make_reg_free_string : %s\n", result);

	//
	result = str_replace(path, ")", "\\)");
	
	if(result == NULL)
	{
		return NULL;
	}
	
	path = result;
	
	result_11= result;
	
	//fprintf(stderr, "make_reg_free_string : %s\n", result);
	
	//
	result = str_replace(path, ":", "\\:");
	
	if(result == NULL)
	{
		return NULL;
	}
	
	path = result;
	
	result_12 = result;
	
	//fprintf(stderr, "make_reg_free_string : %s\n", result);
	
	//
	result = str_replace(path, "=", "\\=");
	
	if(result == NULL)
	{
		return NULL;
	}
	
	path = result;
	
	result_13 = result;
	
	//fprintf(stderr, "make_reg_free_string : %s\n", result);
	
	//
	result = str_replace(path, "!", "\\!");
	
	if(result == NULL)
	{
		return NULL;
	}
	
	path = result;
	
	result_14 = result;
	
	//fprintf(stderr, "make_reg_free_string : %s\n", result);
	
	//
	result = str_replace(path, "|", "\\|");
	
	if(result == NULL)
	{
		return NULL;
	}
	
	path = result;
	
	result_15 = result;
	
	//fprintf(stderr, "make_reg_free_string : %s\n", result);
	
	//
	result = str_replace(path, "[", "\\[");
	
	if(result == NULL)
	{
		return NULL;
	}
	
	path = result;
	
	result_16 = result;
	
	//fprintf(stderr, "make_reg_free_string : %s\n", result);
		
	//
	result = str_replace(path, "]", "\\]");
	
	if(result == NULL)
	{
		return NULL;
	}

	//fprintf(stderr, "make_reg_free_string : %s\n", result);	

	free(result_1);
	free(result_2);
	free(result_3);
	free(result_4);
	free(result_5);
	free(result_6);
	free(result_7);
	free(result_8);
	free(result_9);
	free(result_10);
	free(result_11);
	free(result_12);
	free(result_13);
	free(result_14);
	free(result_15);
	free(result_16);

	return result;
}

