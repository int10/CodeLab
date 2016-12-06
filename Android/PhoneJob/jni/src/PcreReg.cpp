#include "PcreReg.h"

PcreReg::PcreReg()
{
}

PcreReg::PcreReg(string pattern)
{
	m_pattern = pattern;
	m_greedy = true;
	m_casesensitive = true;
}

PcreReg::~PcreReg()
{

}

int PcreReg::IndexIn(const string str, int pos)
{
	int index;
	pcre2_code *re;
	PCRE2_SPTR pattern;     /* PCRE2_SPTR is a pointer to unsigned code units of */
	PCRE2_SPTR subject;     /* the appropriate width (in this case, 8 bits). */

	m_capstrlist.clear();
	m_str = str;
	int errornumber;
	int i;
	int rc;

	PCRE2_SIZE erroroffset;
	PCRE2_SIZE *ovector;

	size_t subject_length;
	pcre2_match_data *match_data;

	pattern = (PCRE2_SPTR)m_pattern.data();
	subject = (PCRE2_SPTR)(m_str.data() + pos);
	subject_length = strlen((char *)subject);

	//compile pattern
	uint32_t option = PCRE2_DOTALL;
	if(!m_casesensitive)	option |= PCRE2_CASELESS;
	if(!m_greedy)				option |= PCRE2_UNGREEDY;

	re = pcre2_compile(pattern, PCRE2_ZERO_TERMINATED, option, &errornumber, &erroroffset, NULL);

	if (re == NULL)
	{
		//compile fail
		PCRE2_UCHAR buffer[256];
		pcre2_get_error_message(errornumber, buffer, sizeof(buffer));
		printf("PCRE2 compilation failed at offset %d: %s\n", (int)erroroffset,
			   buffer);
		return -1;
	}

	//match
	match_data = pcre2_match_data_create_from_pattern(re, NULL);
	rc = pcre2_match(re, subject, subject_length, 0, 0, match_data, NULL);

	//Matching failed: handle error cases
	if (rc < 0)
	{
		switch(rc)
		{
		case PCRE2_ERROR_NOMATCH:
			break;
		default:
			break;
		}
		pcre2_match_data_free(match_data);   /* Release memory used for the match */
		pcre2_code_free(re);                 /* data and the compiled pattern. */
		return -1;
	}

	ovector = pcre2_get_ovector_pointer(match_data);
	index = (int)ovector[0];
	//printf("Match succeeded at offset %d\n", (int)ovector[0]);

	if (rc == 0)	printf("ovector was not big enough for all the captured substrings\n");

	for (i = 0; i < rc; i++)
	{
		PCRE2_SPTR substring_start = subject + ovector[2*i];
		size_t substring_length = ovector[2*i+1] - ovector[2*i];
		string s;
		s.append((char *)substring_start, substring_length);
		m_capstrlist.push_back(s);
		//printf("%2d: %.*s\n", i, (int)substring_length, (char *)substring_start);
	}
	pcre2_match_data_free(match_data);   /* Release memory used for the match */
	pcre2_code_free(re);                 /* data and the compiled pattern. */
	return index + pos;
}

string PcreReg::Cap(int index)
{
	if(index >= m_capstrlist.size()){
		return "";
	}
	list<string>::iterator it = m_capstrlist.begin();
	while(index--) it++;
	return *it;

}

void PcreReg::SetGreedy(bool greedy)
{
	m_greedy = greedy;
}

void PcreReg::SetCaseSensitive(bool sen)
{
	m_casesensitive = sen;
}

