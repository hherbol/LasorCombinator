/* combos.c
A code to generate all possible combinations of N distinguishable
particles in K spaces.

Given:
    - blocks - A list of characters indicating the different particles
    - k_spaces - The number of different spaces for which blocks can be in
Return:
    - combinations - A list of lists, each holding different possible permutations.
*/
#include <Python.h>
#include <malloc.h>
#include <stdlib.h>
#include <vector>
#include <string>

#define DEBUG 0

class Calculator{
private:
    // tmpT will hold the different combinations we can have
    char *tmpT;
    char *blocks;
    int lvl;

public:
    std::vector<char*> combos;
    std::vector<std::string> combo_strings;
    int numB, numO; // Number of blocks, and open spots

    Calculator(int number_of_open, int number_of_blocks, char* given_blocks){
        tmpT = (char*) malloc(number_of_open * sizeof(char));
        for (int i = 0; i < number_of_open; i++){
            tmpT[i] = 'o';
        }

        blocks = given_blocks;

        lvl = 0;
        numB = number_of_blocks;
        numO = number_of_open;
    }

    ~Calculator(){
        free(tmpT);
        for (int i = 0; i < combos.size(); i++)
            free(combos[i]);
    }

    void getCombo(int num, char* sub_part){ // This gets an array of all possibile board combinations
        if (lvl == 0 && sub_part == NULL){
            sub_part = (char*) malloc(numO * sizeof(char));
            for (int i = 0; i < numO; i++){
                sub_part[i] = 'o';
            }
        }
        char* sub_hold = (char*) malloc(numO * sizeof(char));
        for (int i = 0; i < numO; i++){
            sub_hold[i] = sub_part[i];
        }

        lvl++;
        for(int i = 0; i < numO; i++){
            if (sub_part[i] != 'o') continue; // Skip if we can't place a block

            for(int j = 0; j < numO; j++){
                sub_part[j] = sub_hold[j];
            }

            sub_part[i] = blocks[lvl - 1];

            if(lvl < numB)
                getCombo(i + 1, sub_part); // Make recursive until there are numB loops
            else {
                combos.push_back((char *) malloc(numO * sizeof(char)));
                int end = combos.size() - 1;
                for (int j = 0; j < numO; j++){
                    combos[end][j] = sub_part[j];
                }

// #if DEBUG
//                     FILE *debug2;
//                     debug2 = fopen("debug", "a");
//                     fprintf(debug2, ".%s.%d\n", combos.back(), lvl);
//                     fclose(debug2);
// #endif
            }
        }
        lvl--;
        if (lvl == 0){
            free(sub_part);
#if DEBUG
            FILE *debug2;
            debug2 = fopen("debug", "a");
            for (int i = 0; i < combos.size(); i++)
                fprintf(debug2, ".%s.%d\n", combos[i], lvl);
            fclose(debug2);
#endif
        }
        free(sub_hold);
    }

};

static PyObject* py_get_combos(PyObject* self, PyObject* args, PyObject* keywds) {
    // Initialize variables
    int N_combos, N_Blocks, N_Spaces;
    char *given_blocks;
    FILE *debug;

#if DEBUG
    debug = fopen("debug", "w");
    fprintf(debug, "Starting function\n");
    fclose(debug);
#endif

    // A list of all the python objects being input to c
    static char *kwlist[] = {"blocks", "N_Spaces", NULL};
    PyObject * py_blocks;
    double tmpD;

    // The python object to be returned to python
    PyObject * all_combos;
    PyObject * sub_combo;



    /* Ensure it is a list being passed
       NOTE! The format string "O!" says that I am reading in an object and requiring it to be a certain type
       Therefore, I give this function two variables.  The first is the type I want to read in and the second
       Is the memory location for the list itself.
       If keywords are used, separate by |.  Thus "O!d" would be "O!d|extra" where "extra" is "O!" or some other
       Identifiers.
       For more info, go here: https://docs.python.org/2/c-api/arg.html*/
    // !PyArg_ParseTupleAndKeywords( args, keywds, "O!d|O!O!", kwlist, &PyList_Type, &list_obj, &cutoff, &PyList_Type, &pbc, &PyList_Type, &origin_list)
    if ( !PyArg_ParseTupleAndKeywords( args, keywds, "O!d", kwlist, &PyList_Type, &py_blocks, &tmpD) ) return NULL;

    if (tmpD < 1) return NULL;
    N_Spaces = tmpD;

#if DEBUG
    debug = fopen("debug", "a");
    fprintf(debug, "Read in N_Spaces = %d\n", N_Spaces);
    fclose(debug);

    debug = fopen("debug", "a");
    if (PyList_Check(py_blocks))
        fprintf(debug, "blocks is read in as a PyList object\n");
    else
        fprintf(debug, "blocks was not read in as a PyList object\n");
    fclose(debug);
#endif


    N_Blocks = PyList_Size(py_blocks);

#if DEBUG
    debug = fopen("debug", "a");
    fprintf(debug, "Read in a list of length %d\n", N_Blocks);
    fclose(debug);
#endif

    // We make sure the number of blocks is less than or equal to the number of spaces
    if (N_Blocks > N_Spaces) return NULL;

    // Now, parse the list to c
    given_blocks = (char *) malloc(N_Blocks * sizeof(char));
    for (int i=0; i<N_Blocks; i++)
        given_blocks[i] = PyString_AsString(PyList_GetItem(py_blocks, i))[0];

#if DEBUG
    debug = fopen("debug", "a");
    fprintf(debug, "Parsed list\n");
    for (int i=0; i<N_Blocks; i++)
        fprintf(debug, "\t%c ", given_blocks[i]);
    fprintf(debug, "\n");
    fclose(debug);
#endif

    // Now, it's just c code.
    Calculator calc(N_Spaces, N_Blocks, given_blocks);

#if DEBUG
    debug = fopen("debug", "a");
    fprintf(debug, "Calculator has been made...\n");
    fclose(debug);
#endif

    calc.getCombo(0, NULL);

#if DEBUG
    debug = fopen("debug", "a");
    fprintf(debug, "All combinations have been found...\n");
    fclose(debug);
#endif

    // Now, convert calc.combos to pyobject and return
    N_combos = calc.combos.size();

    all_combos = PyList_New(N_combos);
    for (int i = 0; i < N_combos; i++){
        sub_combo = PyList_New(calc.numO);
        for (int j = 0; j < calc.numO; j++){
            PyList_SetItem(sub_combo, j, Py_BuildValue("c", calc.combos[i][j]));
        }
        PyList_SetItem(all_combos, i, sub_combo);
    }

    // Free memory
    free(given_blocks);

    return all_combos;
}

// This binds our c and python function names
static PyMethodDef combo_methods[] = {
  {"n_distinguish_in_k", (PyCFunction)py_get_combos, METH_VARARGS|METH_KEYWORDS},
  {NULL, NULL} // Sentinel, essentially look at what we're passing
};

// Python calls this to initialize module (essentially, this is our main function)
PyMODINIT_FUNC initcombos(void)
{
  (void) Py_InitModule("combos", combo_methods);
}
