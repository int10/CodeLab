#include "Util.h"

string Util::DES3Decode(const string command, const string key, const string iv)
{
	int dlen = command.size();

	if(dlen % 8){
		dlen += 8 - dlen % 8;
	}

	unsigned char * ibuff = new unsigned char[dlen];
	unsigned char * obuff = new unsigned char[dlen];
	memset(ibuff, 0, dlen);
	memset(obuff, 0, dlen);


	unsigned char bkey[32];
	memset(bkey, 0, 32);
	memcpy(bkey, key.data(), key.size());

	des3_context deskey = {{0}, {0}};

	if(key.size() <= 16) des3_set_2keys(&deskey, bkey);
	else des3_set_3keys(&deskey, bkey);

	unsigned char biv[8];
	memset(biv, 0, 8);
	memcpy(biv, iv.data(), iv.size());

	string data;
	size_t olen;
	char *d;

	d = (char *)base64_decode((const unsigned char *)command.data(), command.size(), &olen);
	data.append(d, olen);
	free(d);

	memcpy(ibuff, data.data(), data.size());

	des3_cbc_decrypt(&deskey, biv, ibuff, obuff, data.size());

	string out;
	out.append((char *)obuff, data.size());

	delete[] ibuff;
	delete[] obuff;

	return out;
}

string Util::DES3Encode(const string code, const string key, const string iv)
{
	int dlen = code.size();

	if(dlen % 8){
		dlen += 8 - dlen % 8;
	}

	unsigned char * ibuff = new unsigned char[dlen];
	unsigned char * obuff = new unsigned char[dlen];
	memset(ibuff, 0, dlen);
	memset(obuff, 0, dlen);

	unsigned char bkey[32];
	memset(bkey, 0, 32);
	memcpy(bkey, key.data(), key.size());

	des3_context deskey = {{0}, {0}};

	if(key.length() <= 16) des3_set_2keys(&deskey, bkey);
	else des3_set_3keys(&deskey, bkey);

	unsigned char biv[8];
	memset(biv, 0, 8);
	memcpy(biv, iv.data(), iv.size());

	memcpy(ibuff, code.data(), code.length());

	des3_cbc_encrypt(&deskey, biv, ibuff, obuff, dlen);

	string data;
	data.append((char *)obuff, dlen);

	string out;
	size_t olen;
	char *d;

	d = (char *)base64_encode((const unsigned char *)data.data(), data.size(), &olen);
	out.append(d, olen);
	free(d);

	delete[] ibuff;
	delete[] obuff;

	return out;
}
