#include <vector>
#include <fstream>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

int main(int argc, char **argv)
{
    vector<string> path, args;
    vector<vector<string> > params;
    int arg = 0, cnt = 0, copies;
    ifstream input("config.cfg");
    if(argc > 1) {
        copies = atoi(argv[1]);
        for(int i = 2; i < argc; ++i) {
            args.push_back(argv[i]);
        }
    } else {
        input >> copies;
        while(input.good()) {
            string tmp;
            input >> tmp;
            if(strlen(tmp.c_str()) > 0) {
                args.push_back(tmp);
            }
        }
    }
    while(arg < args.size()) {
        if(strcmp(args[arg].c_str(), "--path") == 0) {
            path.push_back(args[arg + 1]);
            params.push_back(vector<string>());
            cnt++;
            arg += 2;
        } else if(strcmp(args[arg].c_str(), "--name") == 0) {
            params[cnt - 1].push_back(args[arg + 1]);
            path[cnt - 1] += args[arg + 1];
            arg += 2;
        } else {
            params[cnt - 1].push_back(args[arg]);
            arg++;
        }
    }

    input.close();

    for(int copy = 0; copy < copies; ++copy) {
        char **argv_copy = new char * [params[copy].size() + 1];
        for(int i = 0; i < params[copy].size(); i++) {
            argv_copy[i] = new char[params[copy][i].length() + 1];
            strcpy(argv_copy[i], const_cast<char *>(params[copy][i].c_str()));
        }
        argv_copy[params[copy].size()] = NULL;

        pid_t pid = fork();
        if (pid == 0)
        {	    
	    // child process
            execv(path[copy].c_str(), argv_copy);
        }
        else if (pid < 0)
        {
            // fork failed
            printf("fork() %d failed!\n", copy);
            return 1;
        }
    }
    pid_t wpid;
    int status = 0;
    while(wait(&status) > 0);
    printf("all children finished.\n");
     
    return 0;
}
