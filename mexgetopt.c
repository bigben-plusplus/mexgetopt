#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <getopt.h>

#include "mex.h"

int check_short_options(const char* short_options) {
    const char* p = short_options;

    //mexPrintf("short_options: %s\n", short_options);

    int flag = 1;
    while (*p) {
        if (!((isalnum(*p)) || (*p == ':'))) {
            flag = 0;
            break;
        }
        p++;
    }

    // mexPrintf("flag: %d\n", flag);
    return flag;
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

    if (nrhs < 1 || nlhs > 2) {
        mexErrMsgTxt("mexgetopt:SyntaxError: [opts, args] = mexgetopt(arg_string, option_specs)\n");
    }

    opterr = 1;
    optind = 0;

    int    argc          = 0;
    char** argv          = NULL;
    char*  short_options = NULL;

    if (mxIsCell(prhs[0])) {
        const mxArray* arg_string = prhs[0];

        argc = mxGetNumberOfElements(arg_string) + 1;
        argv = (char **) malloc(argc * sizeof(char *));
        argv[0] = NULL;

        for (size_t i = 1; i < argc; ++i) {
            argv[i] = mxArrayToString(mxGetCell(arg_string, i - 1));
        }

        if (nrhs > 1) {
            short_options = mxArrayToString(prhs[1]);
            if (0 == check_short_options(short_options)) {
                free(argv);
                mexErrMsgTxt("mexgetopt:ValueError: invalid short options specs\n");
            }
        } else {
            free(argv);
            mexErrMsgTxt("mexgetopt:SyntaxError: mexgetopt(arg_string, option_specs)\n");
        }

    } else {
        if (mxIsChar(prhs[nrhs-1])) {
            short_options = mxArrayToString(prhs[nrhs-1]);
        }

        if (0 == check_short_options(short_options)) {
            mexErrMsgTxt("mexgetopt:ValueError: invalid short options specs\n");
        }

        argc = nrhs - 1 + 1;
        argv = NULL;
        if (argc > 0) {
            argv = (char **) malloc(argc * sizeof(char *));
        }
        argv[0] = NULL;

        for (size_t i = 1; i < argc; ++i) {
            argv[i] = mxArrayToString(prhs[i-1]);
        }
    }

    // for debug
    #ifdef DEBUG
    mexPrintf("short_options = %s\n", short_options);
    mexPrintf("argc = %d\n", argc);
    for (size_t i = 0; i < argc; ++i) {
        mexPrintf("argv[%d] = %s\n", i, argv[i]);
    }
    #endif

    int count = 0;    

    char ch;
    while ((ch = getopt(argc, argv, short_options)) != EOF) {
        if (ch == '?') {
            mexErrMsgTxt("mexgetopt:ValueError: undefined option\n");
        }
        count++;
    }

    optind = 0;
    if (nlhs > 0) {
        char* field_names[] = {"opt", "arg"};
        plhs[0] = mxCreateStructMatrix(1, count, 2, (const char**) field_names);

        int i = 0;
        char buf[10];
        while ((ch = getopt(argc, argv, short_options)) != EOF) {
            buf[0] = '-';
            buf[1] = optopt;
            buf[2] = 0;
            mxSetField(plhs[0], i, "opt", mxCreateString(buf));
            mxSetField(plhs[0], i, "arg", mxCreateString(optarg));
            i++;
        }
        assert(i == count);
    }

    if (nlhs > 1) {
        plhs[1] = mxCreateCellMatrix(1, argc - optind);
        for (size_t i = optind; i < argc; ++i) {
            mxSetCell(plhs[1], i - optind, mxCreateString(argv[i]));
        }

    }

    free(argv);
}
