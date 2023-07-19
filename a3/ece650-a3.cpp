#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

#include <iostream>
#include <vector>

int main(int argc, char **argv) {
    int opt;
    int s = 10, n = 5, l = 5, c = 20;

    while ((opt = getopt(argc, argv, "-:s:n:l:c:")) != -1) {
        int t;
        switch (opt) {
            case 's':
                t = atoi(optarg);
                if (t < 2) {
                    fprintf(stderr, "Error: Option %c arg too small\n", optopt);
                    return 1;
                } else
                    s = t;
                break;
            case 'n':
                t = atoi(optarg);
                if (t < 1) {
                    fprintf(stderr, "Error: Option %c arg too small\n", optopt);
                    return 1;
                } else
                    n = t;
                break;
            case 'l':
                t = atoi(optarg);
                if (t < 5) {
                    fprintf(stderr, "Error: Option %c arg too small\n", optopt);
                    return 1;
                } else
                    l = t;
                break;
            case 'c':
                t = atoi(optarg);
                if (t < 1) {
                    fprintf(stderr, "Error: Option %c arg too small\n", optopt);
                    return 1;
                } else
                    c = t;
                break;
            case '?':
                fprintf(stderr, "Error: Unknown option: %c\n", optopt);
                return 1;
            case ':':
                fprintf(stderr, "Error: Missing arg for %c\n", optopt);
                return 1;
            case 1:
                fprintf(stderr, "Error: Non-option arg: %s\n", optarg);
                return 1;
        }
    }

    std::string ss = std::to_string(s);
    std::string nn = std::to_string(n);
    std::string ll = std::to_string(l);
    std::string cc = std::to_string(c);

    char *arg_rgen[10];
    arg_rgen[0] = (char *)"rgen";
    arg_rgen[1] = (char *)"-s";
    arg_rgen[2] = &*ss.begin();
    arg_rgen[3] = (char *)"-n";
    arg_rgen[4] = &*nn.begin();
    arg_rgen[5] = (char *)"-l";
    arg_rgen[6] = &*ll.begin();
    arg_rgen[7] = (char *)"-c";
    arg_rgen[8] = &*cc.begin();
    arg_rgen[9] = NULL;

    std::vector<pid_t> kids;
    int rgen_to_py[2];
    if(pipe(rgen_to_py)==-1){
        std::cerr << "Error: fail to pipe" << std::endl;
    }
    int py_to_a2[2];
    if(pipe(py_to_a2)==-1){
        std::cerr << "Error: fail to pipe" << std::endl;
    }

    pid_t a3_pid;
    a3_pid = fork();
    if (a3_pid == 0) {
        if (dup2(py_to_a2[0], STDIN_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        // freopen("output.txt","w",stdout);
        // std::string line;
        // while (std::getline(std::cin,line)){
        //     std::cout<<line<<std::endl;
        // }

        // std::string line;
        // while (true) {
        //     try {
        //         getline(std::cin, line);
        //         std::cout << line << std::endl;
        //         if (std::cin.eof()) {
        //             break;
        //         }
        //     } catch (...) {
        //         std::cerr << "Error: Incorrect Execution." << std::endl;
        //     }
        // }

        close(rgen_to_py[0]);
        close(rgen_to_py[1]);
        close(py_to_a2[0]);
        close(py_to_a2[1]);

        char *argv_a2[] = {(char *)"./ece650-a2", NULL};
        if (execv(argv_a2[0], argv_a2) < 0) {
            std::cerr << "Error: fail to execv a2" << std::endl;
            return 1;
        }

        return 0;
    } else if (a3_pid < 0) {
        std::cerr << "Error: could not fork\n";
        return 1;
    }

    kids.push_back(a3_pid);

    pid_t py_pid;
    py_pid = fork();
    if (py_pid == 0) {
        dup2(rgen_to_py[0], STDIN_FILENO);
        dup2(py_to_a2[1], STDOUT_FILENO);

        close(rgen_to_py[0]);
        close(rgen_to_py[1]);
        close(py_to_a2[0]);
        close(py_to_a2[1]);
        char *argv_py[3];
        argv_py[0] = (char *)"/usr/bin/python3";
        argv_py[1] = (char *)"./ece650-a1.py";
        argv_py[2] = nullptr;

        if (execv(argv_py[0], argv_py) < 0) {
            std::cerr << "Error: fail to execv py" << std::endl;
            return 1;
        }

    } else if (py_pid < 0) {
        std::cerr << "Error: could not fork\n";
        return 1;
    }
    kids.push_back(py_pid);

    pid_t rgen_pid;
    rgen_pid = fork();
    if (rgen_pid == 0) {
        close(STDIN_FILENO);
        dup2(rgen_to_py[1], STDOUT_FILENO);
        close(rgen_to_py[0]);
        close(rgen_to_py[1]);
        close(py_to_a2[0]);
        close(py_to_a2[1]);

        // char *arg_rgen[argc + 1];
        // arg_rgen[0] = (char *)"./rgen";
        // for (int i = 1; i < argc; i++) {
        //     arg_rgen[i] = argv[i];
        // }
        // arg_rgen[argc] = NULL;

        if (execv(arg_rgen[0], arg_rgen) < 0) {
            std::cerr << "Error: fail to execv rgen" << std::endl;
            return 1;
        }
        return 0;
    } else if (rgen_pid < 0) {
        std::cerr << "Error: could not fork\n";
        return 1;
    }
    kids.push_back(rgen_pid);

    //sleep(5);
    //dup2(py_to_a2[0], STDIN_FILENO);
    //write(py_to_a2[1], "s 1 2\n", sizeof("s 1 2\n"));

    // FILE *stream;
    // stream = fdopen(py_to_a2[1], "w");
    // fprintf(stream, "hello, world!\n");
    // fprintf(stream, "goodbye, world!\n");
    // fclose(stream);
    //sleep(10);
    dup2(py_to_a2[1], STDOUT_FILENO);
    close(rgen_to_py[0]);
    close(rgen_to_py[1]);
    close(py_to_a2[0]);
    close(py_to_a2[1]);
    std::string line;
    while (std::getline(std::cin, line)) {
        std::cout << line << std::endl;
    }

    for (pid_t k : kids) {
        int status;
        kill(k, SIGTERM);
        waitpid(k, &status, 0);
    }
    return 0;
}
