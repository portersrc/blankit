/*BEGIN_LEGAL
Intel Open Source License

Copyright (c) 2002-2017 Intel Corporation. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.  Redistributions
in binary form must reproduce the above copyright notice, this list of
conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.  Neither the name of
the Intel Corporation nor the names of its contributors may be used to
endorse or promote products derived from this software without
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL OR
ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
END_LEGAL */
/*! @file
*/

#include "pin.H"
#include <iostream>
#include <fstream>
#include <string>
#include <sys/mman.h>
#include <fcntl.h>
#include <deque>
#include <set>
#include "/home/rudy/debloat/pin_probe_debloat_experiments/blankit/blankit.h"

using namespace std;

#define RWX_PERM (PROT_READ | PROT_WRITE | PROT_EXEC)
#define RX_PERM  (PROT_READ | PROT_EXEC)


#define APP_NAME "/home/rudy/debloat/pin_probe_debloat_experiments/example_app/example_app"
//#define APP_NAME "/home/rudy/debloat/pin_probe_debloat_experiments/basicmath/basicmath_small"


// https://software.intel.com/sites/landingpage/pintool/docs/71313/Pin/html/
// A probe may be up to 14 bytes long.
#define MAGIC_OFFSET 14



typedef enum{
    E_BLANK_STATE_INACTIVE     = 0, 
    E_BLANK_STATE_ACTIVE       = 1, 
    E_BLANK_STATE_MISPREDICTED = 2, 
}blank_state_e;



// https://www.google.com/search?q=magic+gif&client=ubuntu&hs=fx6&channel=fs&tbm=isch&source=iu&ictx=1&fir=RlfimoK3CK8v1M%253A%252C35N61XzR0TwJqM%252C_&usg=__3LZm8zPuqAjNhKzvNYMY7H2C3ms%3D&sa=X&ved=0ahUKEwjy3O_R66DaAhVJulMKHR5UCucQ9QEIKzAA#imgrc=RlfimoK3CK8v1M:
string the_magic_list[] = {
    ".text",
    //"_init",
    "__libc_start_main",
    //"__ctype_init",
    //"__sigsetjmp",
    //"atoi",
    //"exit",
    //"__cxa_atexit",
    //"__cxa_finalize",
    //"__call_tls_dtors",
    //"_IO_vfprintf",
    //"_IO_printf",
    //"sprintf",
    //"_IO_file_doallocate",
    //"_IO_puts",
    //"vsprintf",
    //"_IO_file_sync",
    "bzero", // 536
};



int page_size;
blank_state_e blank_state = E_BLANK_STATE_INACTIVE;
map<char *, unsigned int> copied_funcs;


const char *called_funcs[] = {"malloc", "pow"};
int called_funcs_length = sizeof(called_funcs)/sizeof(*called_funcs);
const char *app_funcs[] = {"fib", "Summ"};
int app_funcs_length = sizeof(app_funcs)/sizeof(*app_funcs);
set<string> ignored_funcs(the_magic_list,
                          the_magic_list
                           + sizeof(the_magic_list) / sizeof(*the_magic_list));
deque<string> pred_q;
map<char *, string> addr_to_name;
map<string, set<string> > statically_reachable_funcs;
int num_no_predicts      = 0;
int num_mispredicts      = 0;
int num_underpredicts    = 0;
int num_overpredicts     = 0;
int num_correct_predicts = 0;
int num_correct_full_chain_predicts = 0; // understimation due to our
                                         // implementation with probe_blank not
                                         // always getting called at the end







template<typename Out>
void split(const string &s, char delim, Out result)
{
    stringstream ss(s);
    string item;
    while(getline(ss, item, delim)){
        *(result++) = item;
    }
}

vector<string> split(const string &s, char delim)
{
    vector<string> elems;
    split(s, delim, back_inserter(elems));
    return elems;
}



void init_statically_reachable_funcs(void)
{
    long file_size;
    size_t nmemb_read;

    FILE *fp = fopen("../libc/staticReachableFuncs", "r");
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);  // rewind


    char *file_data = (char *) malloc(file_size);
    nmemb_read = fread(file_data, file_size, 1, fp);
    assert(nmemb_read == 1);

    istringstream iss(file_data);

    vector<string> parts;
    for(string line; getline(iss, line); ){
            //cout << line << endl;
            parts = split(line, ':');
            for(vector<string>::iterator it = parts.begin(); it != parts.end(); it++){
                //cout << *it << endl;
            }
            string func_name = parts[0];
            //int num_statically_reachable_funcs = parts[1];
            parts = split(parts[2], ',');
            for(vector<string>::iterator it = parts.begin(); it != parts.end(); it++){
                //cout << *it << endl;
                statically_reachable_funcs[func_name].insert(*it);
            }


    }

    free(file_data);
    fclose(fp);
}



void blank_func(char *addr_base, int size_cp)
{
    printf("blank_func()\n");
    memset(addr_base + MAGIC_OFFSET, 0, size_cp);
    printf("  memset succeeded\n");
}


void copy_func(void *copy_of_func, char *addr_base, int size_cp)
{
    printf("copy_func():\n");
    memcpy(copy_of_func, addr_base + MAGIC_OFFSET, size_cp);
    printf("  memcpy succeeded\n");
}


void remap_permissions(char *addr, int size, int perm)
{
    char *aligned_addr_base;
    char *aligned_addr_end;
    int size_to_remap;

    printf("remap_permissions():\n");

    aligned_addr_base = (char *) ((unsigned long)(addr) & ~(page_size - 1));
    aligned_addr_end  = (char *) ((unsigned long)(addr+size) & ~(page_size - 1));
    size_to_remap = page_size + (aligned_addr_end - aligned_addr_base);
    printf("  aligned_addr_base: %p\n", aligned_addr_base);
    printf("  aligned_addr_end:  %p\n", aligned_addr_end);
    printf("  size_to_remap:     %d\n", size_to_remap);

    if(mprotect(aligned_addr_base, size_to_remap, perm) == -1){
        cout << "mprotect error" << endl;
    }
    printf("  mprotect succeeded\n");
}


void probe_copy(void *copy_of_func, char *addr_base, unsigned int size_cp)
{
    cout << "probe_copy()\n";

    printf("  copy_of_func: %p\n", copy_of_func);
    printf("  addr_base:    %p\n", addr_base);
    printf("  size_cp:      %d\n", size_cp);
    printf("  name:         %s\n", addr_to_name[addr_base].c_str());

#define BLANKIT_PREDICT
#ifdef BLANKIT_PREDICT
    string predicted_func;
    if(pred_q.size()){
        predicted_func = pred_q.front();
        pred_q.pop_front();
        cout << "predicted = " << predicted_func << endl;

        if(predicted_func == addr_to_name[addr_base]){
            // hit
            if(blank_state == E_BLANK_STATE_INACTIVE){
                blank_state = E_BLANK_STATE_ACTIVE;
                
                // TODO: this is the entry function of a set of
                // predictions. We should proactively map and memcpy
                // all of predictions.
                remap_permissions(addr_base, size_cp, RWX_PERM);
                memcpy(addr_base + MAGIC_OFFSET, copy_of_func, size_cp);
                copied_funcs[addr_base] = size_cp;
            }
            //num_correct_predicts++;
            return;

        }else if(statically_reachable_funcs.find(predicted_func)
                 != statically_reachable_funcs.end()){
            // mispredict
            pred_q.clear();
            //num_mispredicts++;
            printf("num_mispredicts: %d\n", num_mispredicts);
            blank_state = E_BLANK_STATE_MISPREDICTED;
        }else{
            // attack detected
            printf("attack detected\n"); // XXX for debugging
            //exit(1);
        }
    }else{
        // prediction queue is empty
        remap_permissions(addr_base, size_cp, RWX_PERM);
        memcpy(addr_base + MAGIC_OFFSET, copy_of_func, size_cp);
        copied_funcs[addr_base] = size_cp;
        switch(blank_state){
            case E_BLANK_STATE_INACTIVE:
                // We went into glibc without making a prediction.
                num_no_predicts++;
                // FIXME... i guess we need another state so we don't keep
                // incrementing this counter
                //blank_state = E_BLANK_STATE_DONE;
                break;
            case E_BLANK_STATE_ACTIVE:
                num_underpredicts++;
                blank_state = E_BLANK_STATE_MISPREDICTED;
                break;
            case E_BLANK_STATE_MISPREDICTED:
                break;
        }
    }


#else
    remap_permissions(addr_base, size_cp, RWX_PERM);
    memcpy(addr_base + MAGIC_OFFSET, copy_of_func, size_cp);
    copied_funcs[addr_base] = size_cp;
    blank_state = E_BLANK_STATE_ACTIVE;
#endif

}

void probe_blank(void)
{
    cout << "probe_blank()\n";
    if(blank_state){
        if(blank_state == E_BLANK_STATE_ACTIVE && pred_q.size() == 0){
            num_correct_full_chain_predicts++;
        }
        for(map<char *, unsigned int>::iterator it = copied_funcs.begin();
            it != copied_funcs.end();
            ++it){
            char *addr = it->first;
            unsigned int size = it->second;
            printf("  blanking %p %d\n", addr, size);
            memset(addr + MAGIC_OFFSET, 0, size);
            // TODO: inline blank_func
            //remap_permissions(addr, size, RX_PERM);
        }
        copied_funcs.clear();
        blank_state = E_BLANK_STATE_INACTIVE;
        cout << "  succeeded\n";
    }else{
        cout << "  nothing to do\n";
    }
}







void dump_routines(SEC text_sec)
{
    RTN rtn;
    rtn  = SEC_RtnHead(text_sec);
    while(rtn != RTN_Invalid()){
        cout << "RTN name: " << RTN_Name(rtn) << endl;
        rtn  = RTN_Next(rtn);
    }
    cout << "Loop broke. RTN is invalid" << endl;
}

void set_rtn_head(RTN *rtn, IMG &img)
{
    SEC sec;
    sec  = IMG_SecHead(img);
    while(sec != SEC_Invalid()){
        if(SEC_Name(sec) == ".text"){
            *rtn = SEC_RtnHead(sec);
            return;
        }
        sec  = SEC_Next(sec);
    }
    cout << "ERROR: Unable to find head routine for text section" << endl;
    exit(1);
}



BOOL FindAndCheckRtn(IMG img, string rtnName, RTN& rtn)
{
    rtn = RTN_FindByName(img, rtnName.c_str());

    if(RTN_Valid(rtn)){
        if(!RTN_IsSafeForProbedInsertion(rtn)){
            return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}


void image_load_libc(IMG &img)
{
    RTN rtn;
    void *copy_of_func;
    char *addr_base;
    int size_cp = 0;
    int i = 0;

    cout << "image_load_libc()" << endl;

    for(i = 0; i < called_funcs_length; i++){
        printf("  attempting to add probe for %s\n", called_funcs[i]);
        if(!FindAndCheckRtn(img, called_funcs[i], rtn)){
            printf("  not adding probe. failed checks.\n");
            continue;
        }

        size_cp = RTN_Size(rtn) - MAGIC_OFFSET;
        if(size_cp < 1){
            printf("  not adding probe. copy size is too small\n");
            continue;
        }

        addr_base    = (char *) RTN_Address(rtn);
        copy_of_func = malloc(size_cp);
        printf("  copy address: %p\n", copy_of_func);
        printf("  addr_base:    %p\n", addr_base);
        printf("  size_cp:      %d\n", size_cp);

        copy_func(copy_of_func, addr_base, size_cp);
        remap_permissions(addr_base, RTN_Size(rtn), RWX_PERM);
        blank_func(addr_base, size_cp);
        remap_permissions(addr_base, RTN_Size(rtn), RX_PERM);
        RTN_InsertCallProbed(rtn, IPOINT_BEFORE, AFUNPTR( probe_copy ),
                             IARG_PTR, copy_of_func,
                             IARG_PTR, addr_base,
                             // safe cast. see a few lines up where we ignore
                             // continue to the next iteration if size < 1
                             IARG_UINT32, (unsigned int) size_cp,
                             IARG_END);
    }
}

void image_load_libc_all(IMG &img)
{
    RTN rtn;
    void *copy_of_func;
    char *addr_base;
    int size_cp = 0;

    cout << "image_load_libc()" << endl;

    set_rtn_head(&rtn, img);

    SEC sec;
    int found_sect_head = 0;
    sec  = IMG_SecHead(img);
    while(sec != SEC_Invalid()){
        if(SEC_Name(sec) == ".text"){
            rtn = SEC_RtnHead(sec);
            found_sect_head = 1;
            break;
        }
        sec  = SEC_Next(sec);
    }
    assert(found_sect_head);

    while(rtn != RTN_Invalid()){
        cout << "RTN name: " << RTN_Name(rtn) << endl;

        if(!RTN_IsSafeForProbedInsertion(rtn)){
            cout << "  not safe for probed insertion" << endl;
            rtn  = RTN_Next(rtn);
            continue;
        }

        if(ignored_funcs.find(RTN_Name(rtn)) != ignored_funcs.end()){
            cout << "  manually ignoring " << RTN_Name(rtn) << endl;
            rtn  = RTN_Next(rtn);
            continue;
        }

        size_cp = RTN_Size(rtn) - MAGIC_OFFSET;
        if(size_cp < 1){
            cout << "  copy size is too small" << endl;
            rtn  = RTN_Next(rtn);
            continue;
        }

        addr_base    = (char *) RTN_Address(rtn);
        copy_of_func = malloc(size_cp);
        printf("  copy address: %p\n", copy_of_func);
        printf("  addr_base:    %p\n", addr_base);
        printf("  size_cp:      %d\n", size_cp);

        addr_to_name[addr_base] = RTN_Name(rtn);

        copy_func(copy_of_func, addr_base, size_cp);
        remap_permissions(addr_base, RTN_Size(rtn), RWX_PERM);
        blank_func(addr_base, size_cp);
        remap_permissions(addr_base, RTN_Size(rtn), RX_PERM);
        RTN_InsertCallProbed(rtn, IPOINT_BEFORE, AFUNPTR( probe_copy ),
                             IARG_PTR, copy_of_func,
                             IARG_PTR, addr_base,
                             // safe cast. see a few lines up where we ignore
                             // continue to the next iteration if size < 1
                             IARG_UINT32, (unsigned int) size_cp,
                             IARG_END);
        rtn  = RTN_Next(rtn);
    }
}


void image_load_app(IMG &img)
{
    // TODO: Instrument all app functions
    RTN rtn;
    int i;

    cout << "image_load_app()" << endl;

    for(i = 0; i < app_funcs_length; i++){
        printf("  attempting to add probe for %s\n", app_funcs[i]);
        if(!FindAndCheckRtn(img, app_funcs[i], rtn)){
            printf("  not adding probe. failed checks.\n");
            continue;
        }
        RTN_InsertCallProbed(rtn, IPOINT_BEFORE, AFUNPTR( probe_blank ), IARG_END);
    }
}


void image_load_app_all(IMG &img)
{
    SEC sec;
    RTN rtn;

    cout << "image_load_app()" << endl;

    int found_sect_head = 0;
    sec  = IMG_SecHead(img);
    while(sec != SEC_Invalid()){
        if(SEC_Name(sec) == ".text"){
            rtn = SEC_RtnHead(sec);
            found_sect_head = 1;
            break;
        }
        sec  = SEC_Next(sec);
    }
    assert(found_sect_head);

    while(rtn != RTN_Invalid()){
        cout << "app RTN name: " << RTN_Name(rtn) << endl;

        if(!RTN_IsSafeForProbedInsertion(rtn)){
            cout << "  not safe for probed insertion" << endl;
            rtn  = RTN_Next(rtn);
            continue;
        }

        if(ignored_funcs.find(RTN_Name(rtn)) != ignored_funcs.end()){
            cout << "  manually ignoring " << RTN_Name(rtn) << endl;
            rtn  = RTN_Next(rtn);
            continue;
        }
        RTN_InsertCallProbed(rtn, IPOINT_BEFORE, AFUNPTR( probe_blank ), IARG_END);
        rtn  = RTN_Next(rtn);
    }
}

void probe_blankit_predict(char *predict_set)
{
    cout << "probe_blankit_predict()" << endl;
    cout << "predict_set: " << predict_set << endl;

    // XXX OPTIMIZE: iterating can go away if 0th byte has num elements
    // and element size is fixed

    int i = 0;
    string func; // XXX OPTIMIZE no need to use a string here.
    char *curr_preds;

    curr_preds = predict_set;
    while(curr_preds[i] != '\0'){
        if(curr_preds[i] == ';'){
            pred_q.push_back(func);
            func.clear();
            ++i;
            continue;
        }
        func += curr_preds[i];
        ++i;
    }
    //cout<<"done extracting\n";
}


void image_load_libblankit(IMG &img)
{
    RTN rtn;

    cout << "image_load_libblankit()" << endl;

    rtn = RTN_FindByName(img, "blankit_predict");
    if (RTN_Valid(rtn) && RTN_IsSafeForProbedReplacement(rtn)){
        RTN_ReplaceProbed(rtn, AFUNPTR(probe_blankit_predict));
    }else{
        cout << "ERROR: blankit_predict failed checks" << endl;
        exit(1);
    }
}




KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
                            "o", "decrypt_probe.outfile", "specify file name");

INT32 Usage(void)
{
    cerr <<
        "This pin tool tests probe replacement.\n"
        "\n";
    cerr << KNOB_BASE::StringKnobSummary();
    cerr << endl;
    return -1;
}

VOID ImageLoad(IMG img, VOID *v)
{
    // This function is called every time a new image is loaded
    cout << "Image name is: " <<  IMG_Name(img) << endl;

    if(IMG_Name(img).find("libc.so") != string::npos){
        image_load_libc_all(img);
    }else if(IMG_Name(img).find("libblankit.so") != string::npos){
        image_load_libblankit(img);
    }else if(IMG_Name(img).find(APP_NAME) != string::npos){
        image_load_app_all(img);
    }

}


int main(int argc, CHAR *argv[])
{
    PIN_InitSymbols();

    if( PIN_Init(argc,argv) )
    {
        return Usage();
    }

    ofstream TraceFile;
    TraceFile.open(KnobOutputFile.Value().c_str());
    TraceFile << hex;
    TraceFile.setf(ios::showbase);

    IMG_AddInstrumentFunction(ImageLoad, 0);

    page_size = sysconf(_SC_PAGE_SIZE);
    if(page_size == -1){
        cout << "ERROR: Unable to get system page size" << endl;
        return 1;
    }

    init_statically_reachable_funcs();

    PIN_StartProgramProbed();

    return 0;
}
