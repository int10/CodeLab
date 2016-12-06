#ifndef PROCCMD_H
#define PROCCMD_H

#include <stdio.h>
#include <sys/types.h>
//#include <regex.h>
#include <memory.h>
#include <stdlib.h>
#include <string>
#include <list>
#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <sys/wait.h>
#include <fcntl.h>

using namespace std;

class ProcCmd
{
public:
	ProcCmd();
	~ProcCmd();
public:
	//run an external program
	//cmd: command string
	//waitms: wait timeout, set to -1 means wait until the process end , set to 0 meas don't wait..
	//return: return the string from stdout of child..
	//Ps: for easier code, I only read 100K data from stdout, if waitms == 0, you get nothing..
	string CmdRun(string cmdstr, int waitms);
	void Kill();
	static string Run(string cmdstr, int waitms);
private:
	void ParseCmd(string cmdstr, string &cmd, list<string> &argv);
private:
	pid_t m_pid;
};

#endif // PROCCMD_H
