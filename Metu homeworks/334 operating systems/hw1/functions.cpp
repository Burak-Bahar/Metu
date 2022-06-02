#include "parser.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <array>
#include <map>
using namespace std;

void out_one_process(char *outfilename, char **argu)
{

    pid_t p_id = fork();
    int child_status;
    if (p_id < 0)
    {
        perror("out failed");
    }
    else if (p_id == 0)
    { // child
        int file = open(outfilename, O_WRONLY | O_APPEND | O_CREAT, 0666);
        dup2(file, STDOUT_FILENO);
        close(file);
        execvp(argu[0], argu);
    }
    else
    {
        waitpid(p_id, &child_status, 0);
    }
}

void in_one_process(char *infilename, char **argu)
{
    pid_t p_id = fork();
    int child_status;
    if (p_id < 0)
    {
        perror("out failed");
    }
    else if (p_id == 0)
    { // child
        int file = open(infilename, O_RDONLY);
        dup2(file, STDIN_FILENO);
        close(file);
        execvp(argu[0], argu);
    }
    else
    {
        waitpid(p_id, &child_status, 0);
    }
}

void out_more_process(char *outfilename, const vector<char **> &args_of_processes)
{
    int count = args_of_processes.size();
    vector<pid_t> pid(count);
    int child_status;
    for (int i = 0; i < count; i++)
    {
        pid[i] = fork();
        if (pid[i] < 0)
        {
            perror("out failed");
        }
        else if (pid[i] == 0)
        { // child
            int file = open(outfilename, O_WRONLY | O_APPEND | O_CREAT, 0666);
            dup2(file, STDOUT_FILENO);
            close(file);
            execvp(args_of_processes[i][0], args_of_processes[i]);
        }
    }
    for (int i = 0; i < count; i++)
    {
        waitpid(pid[i], NULL, 0);
    }
}

void in_more_process(char *infilename, const vector<char **> &args_of_processes)
{
    int count = args_of_processes.size();
    vector<pid_t> pid(count);
    int child_status;
    for (int i = 0; i < count; i++)
    {
        pid[i] = fork();
        if (pid[i] < 0)
        {
            perror("out failed");
        }
        else if (pid[i] == 0)
        { // child
            int file = open(infilename, O_RDONLY);
            dup2(file, STDIN_FILENO);
            close(file);
            execvp(args_of_processes[i][0], args_of_processes[i]);
        }
    }
    for (int i = 0; i < count; i++)
    {
        waitpid(pid[i], NULL, 0);
    }
}

int firstpipeout(char **argu, char *infilename)
{ // one process
    int pip[2];
    pipe(pip);
    int child_status;
    pid_t p_id = fork();
    if (p_id < 0)
    {
        perror("out failed");
    }
    else if (p_id == 0)
    {

        dup2(pip[1], STDOUT_FILENO);

        if (infilename != NULL)
        {
            int file = open(infilename, O_RDONLY);
            dup2(file, STDIN_FILENO);
            close(file);
        }
        close(pip[1]);
        execvp(argu[0], argu);
    }
    else
    {
        waitpid(p_id, &child_status, 0);
        close(pip[1]);
    }
    return pip[0];
}

int middlepipeout(int pipein, char **argu)
{ // one process middle one
    int pip[2];
    pipe(pip);
    int child_status;
    pid_t p_id = fork();
    if (p_id < 0)
    {
        perror("out failed");
    }
    else if (p_id == 0)
    {
        dup2(pipein, STDIN_FILENO);
        dup2(pip[1], STDOUT_FILENO);
        close(pip[1]);
        close(pipein);
        execvp(argu[0], argu);
    }
    else
    {
        waitpid(p_id, &child_status, 0);
        close(pip[1]);
    }
    return pip[0];
}

void lastpipeout(int pipein, char **argu, char *outfilename)
{ // one process out

    int child_status;
    pid_t p_id = fork();
    if (p_id < 0)
    {
        perror("out failed");
    }
    else if (p_id == 0)
    {

        dup2(pipein, STDIN_FILENO);
        close(pipein);
        if (outfilename != NULL)
        {
            int file = open(outfilename, O_WRONLY | O_APPEND | O_CREAT, 0666);
            dup2(file, STDOUT_FILENO);
            close(file);
        }
        execvp(argu[0], argu);
    }
    else
    {
        waitpid(p_id, &child_status, 0);
    }
}

int first_multi_process_pipe(char *infilename, const vector<char **> &args_of_processes)
{

    int pip[2];
    pipe(pip);

    int count = args_of_processes.size();
    vector<pid_t> pid(count);

    int child_status;

    for (int i = 0; i < count; i++)
    {
        pid[i] = fork();
        if (pid[i] < 0)
        {
            perror("out failed");
        }
        else if (pid[i] == 0)
        {
            dup2(pip[1], STDOUT_FILENO);

            if (infilename != NULL)
            {
                int file = open(infilename, O_RDONLY);
                dup2(file, STDIN_FILENO);
                close(file);
            }
            close(pip[1]);
            execvp(args_of_processes[i][0], args_of_processes[i]);
        }
    }
    close(pip[1]);
    for (int i = 0; i < count; i++)
    {
        waitpid(pid[i], NULL, 0);
    }
    return pip[0];
}

int middle_multi_process_pipe(int pipein, const vector<char **> &args_of_processes)
{
    int pip[2];
    pipe(pip);

    int count = args_of_processes.size();
    vector<pid_t> pid(count);
    vector<int> in_fds(count);
    char *buff = new char[65535];
    int how_much = read(pipein, buff, 65535);
    close(pipein);
    for (size_t i = 0; i < count; i++)
    {
        int temp[2];
        pipe(temp);
        write(temp[1], buff, how_much);
        close(temp[1]);
        in_fds[i] = temp[0];
    }

    int child_status;
    for (int i = 0; i < count; i++)
    {
        pid[i] = fork();
        if (pid[i] < 0)
        {
            perror("out failed");
        }
        else if (pid[i] == 0)
        {
            dup2(in_fds[i], STDIN_FILENO);
            close(in_fds[i]);
            dup2(pip[1], STDOUT_FILENO);
            close(pip[1]);
            execvp(args_of_processes[i][0], args_of_processes[i]);
        }
    }
    for (int i = 0; i < count; i++)
    {
        close(in_fds[i]);
        waitpid(pid[i], NULL, 0);
    }
    return pip[0];
}

void last_multi_process_pipe(int pipein, char *outfilename, const vector<char **> &args_of_processes)
{

    int count = args_of_processes.size();
    vector<pid_t> pid(count);
    vector<int> in_fds(count);
    char *buff = new char[65535];
    int how_much = read(pipein, buff, 65535);
    close(pipein);
    for (size_t i = 0; i < count; i++)
    {
        int temp[2];
        pipe(temp);
        write(temp[1], buff, how_much);
        close(temp[1]);
        in_fds[i] = temp[0];
    }
    int child_status;

    for (int i = 0; i < count; i++)
    {
        pid[i] = fork();
        if (pid[i] < 0)
        {
            perror("out failed");
        }
        else if (pid[i] == 0)
        {
            dup2(in_fds[i], STDIN_FILENO);
            close(in_fds[i]);
            if (outfilename != NULL)
            {
                int file = open(outfilename, O_WRONLY | O_APPEND | O_CREAT, 0666);
                dup2(file, STDOUT_FILENO);
                close(file);
            }
            execvp(args_of_processes[i][0], args_of_processes[i]);
        }
    }
    for (int i = 0; i < count; i++)
    {
        close(in_fds[i]);
        waitpid(pid[i], NULL, 0);
    }
}

struct b_b
{
    char name[255];
    vector<char **> argvs;
};

    int main()
    {
        int dur;
        int b_count = 0;
        int create = 0;
        parsed_input pars;
        map<string, vector<b_b>::iterator> m_bund;
        vector<b_b>::iterator current_creation_iterator;
        vector<b_b> b_bunds;
        b_bunds.reserve(128);
        while (true)
        {
            char *buff = NULL;
            size_t _ = 0;
            ssize_t characters = getline(&buff, &_, stdin);
            dur = parse(buff, create, &pars);
            if (!dur && pars.command.type == PROCESS_BUNDLE_CREATE)
            {
                create = 1;
                b_b bund;
                strcpy(bund.name, pars.command.bundle_name);
                b_bunds.push_back(bund);
                current_creation_iterator = b_bunds.end() - 1;
                m_bund.emplace(pars.command.bundle_name, current_creation_iterator);
            }
            else if (!dur && pars.command.type == QUIT)
            {
                return 0;
            }
            else if (dur && pars.command.type == PROCESS_BUNDLE_STOP)
            {
                b_count++;
                create = 0;
            }

            else if (!dur && pars.command.type == PROCESS_BUNDLE_EXECUTION)
            {
                if (pars.command.bundle_count == 1)
                { // just one bundle
                    vector<b_b>::iterator b = m_bund.find(pars.command.bundles[0].name)->second;

                    if (b->argvs.size() == 1) // bundle has one process?

                    { // one process
                        if (pars.command.bundles[0].input)
                        {
                            in_one_process(pars.command.bundles[0].input, b->argvs[0]);
                        }
                        else if (pars.command.bundles[0].output)
                        {
                            out_one_process(pars.command.bundles[0].output, b->argvs[0]);
                        }
                    }
                    else
                    // more than one process
                    {

                        if (pars.command.bundles[0].input)
                        {
                            in_more_process(pars.command.bundles[0].input, b->argvs);
                        }
                        else if (pars.command.bundles[0].output)
                        {
                            out_more_process(pars.command.bundles[0].output, b->argvs);
                        }
                    }
                }
                else
                {
                    vector<b_b>::iterator b = m_bund.find(pars.command.bundles[0].name)->second;
                    int in_pipe;
                    if (b->argvs.size() == 1) // bundle has one process?

                    { // one process
                        in_pipe = firstpipeout(b->argvs[0], pars.command.bundles[0].input);
                    }
                    else
                    // more than one process
                    {
                        in_pipe = first_multi_process_pipe(pars.command.bundles[0].input, b->argvs);
                    }
                    for (size_t i = 1; i < pars.command.bundle_count - 1; i++)
                    {
                        vector<b_b>::iterator b = m_bund.find(pars.command.bundles[i].name)->second;

                        if (b->argvs.size() == 1) // bundle has one process?

                        { // one process
                            in_pipe = middlepipeout(in_pipe, b->argvs[0]);
                        }
                        else
                        // more than one process
                        {
                            in_pipe = middle_multi_process_pipe(in_pipe, b->argvs);
                        }
                    }
                    b = m_bund.find(pars.command.bundles[pars.command.bundle_count - 1].name)->second;
                    if (b->argvs.size() == 1) // bundle has one process?

                    { // one process
                        lastpipeout(in_pipe, b->argvs[0], pars.command.bundles[pars.command.bundle_count - 1].output);
                    }
                    else
                    // more than one process
                    {
                        last_multi_process_pipe(in_pipe, pars.command.bundles[pars.command.bundle_count - 1].output, b->argvs);
                    }
                }
            }
            else
            {
                current_creation_iterator->argvs.push_back(pars.argv);
            }
        }
        return 0;
    }
