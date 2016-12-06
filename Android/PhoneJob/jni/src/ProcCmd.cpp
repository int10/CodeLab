#include "ProcCmd.h"

ProcCmd::ProcCmd()
{
	m_pid = 0;
}

ProcCmd::~ProcCmd()
{
	Kill();
}


string ProcCmd::CmdRun(string cmdstr, int waitms)
{
	int pipes[2];
	string result = "";
	pid_t childpid;

	if (pipe(pipes) == 0) {
		childpid = fork();
		if (childpid == (pid_t) - 1) {
			cout << "warning:fork fail!" << endl;
		}

		if(childpid == (pid_t)0) {
			//child
			//merge stdout and stderr to pipes[1]
			close(1);
			close(2);
			dup(pipes[1]);
			dup(pipes[1]);
			close(pipes[0]);
			close(pipes[1]);

			list<string> argvlist;
			string cmd;
			ParseCmd(cmdstr, cmd, argvlist);

			char **argv;
			argv = new char *[argvlist.size() + 1];
			list<string>::iterator it;
			int argcount = 0;
			for(it = argvlist.begin(); it != argvlist.end(); it++) {
				argv[argcount] = (char *)(*it).data();
				argcount++;
			}
			argv[argcount] = 0;

			//go go go
			execvp(cmd.data(), argv);

			//in fact , it should not reach here;
			delete []argv;
			argv = 0;
		} else {
			//parent
			close(pipes[1]);

			//wait for the child end;
			if(0 != waitms){
				int count;
				int status;

				if(-1 == waitms) count = -1;
				else count = waitms / 100;

				while(count) {
					if (0 == waitpid(childpid, &status, WNOHANG)) {
						if(count > 0) count--;
						usleep(100 * 1000);	//sleep 100ms;
					} else {
						//already end
						break;
					}
				}
				//whether child end or not, read the output;
				int datasize = 100*1024;
				char *buf;
				buf = new char[datasize];
				int flags = fcntl(pipes[0], F_GETFL);
				fcntl(pipes[0], F_SETFL, flags|O_NONBLOCK);	//don't block at read.
				datasize = read(pipes[0], buf, datasize);
				close(pipes[0]);
				result = string(buf);

				delete []buf;
			}
		}
	}
	m_pid = childpid;
	return result;
}

void ProcCmd::ParseCmd(string cmdstr, string &cmd, list<string> &argv)
{
	//replace all \\ to /
	int index = 0;
	for(;;){
		index = cmdstr.find('\\', index);
		if(-1 == index)	break;
		cmdstr.replace(index, 1, "/");
	}

	//spit the arg...
	int i, startpos = 0, endpos;
	string arg = "";
	for(i = 0; i < cmdstr.size();) {
		char c = cmdstr.at(i);
		if(c == '\"'){
			//match \", find another \" and pick the middle char as an arg..
			startpos = i + 1;
			endpos = cmdstr.find('\"',  startpos);
			if(endpos != -1){
				argv.push_back(cmdstr.substr(startpos, endpos - startpos));
				i = endpos + 1;
				startpos = endpos + 1;
				arg = "";
			} else {
				argv.push_back(cmdstr.substr(startpos));
				break;
			}
		} else if(c == ' ') {
			//match ' ', means get an arg.
			if(arg != "")	argv.push_back(arg);
			arg = "";
			i = cmdstr.find_first_not_of(" ", i);
		} else {
			arg += c;
			i++;
		}
	}
	//handle last string;
	if(arg != "") argv.push_back(arg);

	//pick cmd and correct arg0;arg0 should be the cmd without path info;
	list<string>::iterator it;
	it = argv.begin();
	cmd = *it;
	index = cmd.find_last_of("/");
	if(-1 != index) *it = cmd.substr(index + 1);

	//cout << cmd << endl;
	//for(it = argv.begin(); it != argv.end(); it++) {
	//	cout << *it<<endl;
	//}
}

void ProcCmd::Kill()
{
	int ret;
	if(m_pid != 0){
		ret = kill(m_pid, SIGKILL);
		if (ret){
			//kill fail;
		}
	}
	m_pid = 0;
}

string ProcCmd::Run(string cmdstr, int waitms)
{
	string result;
	ProcCmd p;
	result = p.CmdRun(cmdstr, waitms);
	p.Kill();
	return result;
}

