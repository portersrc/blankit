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
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <dlfcn.h>
#include <sys/mman.h>
#include <unistd.h>
#include <set>


using namespace std;

#define RWX_PERM (PROT_READ | PROT_WRITE | PROT_EXEC)
#define RX_PERM  (PROT_READ | PROT_EXEC)
#define RO_PERM  (PROT_READ)

/* ===================================================================== */
/* Global Variables */
/* ===================================================================== */

ofstream TraceFile;
typedef VOID (*VOIDFUNC)();




// https://software.intel.com/sites/landingpage/pintool/docs/71313/Pin/html/
// A probe may be up to 14 bytes long.
#define MAGIC_OFFSET 14
#define MAGIC_COPY_SIZE 1000000000 // a copy may be HUGE REALLY HUGE

int page_size;
int blank_flag = 0;
map<char *, unsigned int> copied_funcs;
map<char *, string> addr_to_name_dbgmap;
map<char *, void *> addr_to_copy_dbgmap;


// TODO: These should be defined in a header file, one for each benchmark.
// And then we just #include the header file here.
//#define APP_NAME "/home/rudy/debloat/pin_probe_debloat_experiments/example_app/example_app"
#define APP_NAME "/home/rudy/debloat/pin_probe_debloat_experiments/basicmath/basicmath_small"
const char *called_funcs[] = {"strlen", "strrchr", "write", "_IO_puts"};
int called_funcs_length = sizeof(called_funcs)/sizeof(*called_funcs);
const char *app_funcs[] = {"fib", "Summ"};
int app_funcs_length = sizeof(app_funcs)/sizeof(*app_funcs);

// https://www.google.com/search?q=magic+gif&client=ubuntu&hs=fx6&channel=fs&tbm=isch&source=iu&ictx=1&fir=RlfimoK3CK8v1M%253A%252C35N61XzR0TwJqM%252C_&usg=__3LZm8zPuqAjNhKzvNYMY7H2C3ms%3D&sa=X&ved=0ahUKEwjy3O_R66DaAhVJulMKHR5UCucQ9QEIKzAA#imgrc=RlfimoK3CK8v1M:
std::string the_magic_list[] = {
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

std::set<std::string> ignored_funcs(the_magic_list,
                                    the_magic_list
                                    + sizeof(the_magic_list)
                                    / sizeof(*the_magic_list));


/* ===================================================================== */
/* Commandline Switches */
/* ===================================================================== */

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
                            "o", "decrypt_probe.outfile", "specify file name");

/* ===================================================================== */

INT32 Usage(void)
{
    cerr <<
        "This pin tool tests probe replacement.\n"
        "\n";
    cerr << KNOB_BASE::StringKnobSummary();
    cerr << endl;
    return -1;
}





static inline
void blank_func(char *addr_base, int size_cp)
{
    printf("blank_func()\n");
    if(MAGIC_COPY_SIZE > size_cp){
        memset(addr_base + MAGIC_OFFSET, 0, size_cp);
    }else{
        memset(addr_base + MAGIC_OFFSET, 0, MAGIC_COPY_SIZE);
    }
    printf("  memset succeeded\n");
}


void copy_func(void *copy_of_func, char *addr_base, int size_cp)
{
    printf("copy_func():\n");
    if(MAGIC_COPY_SIZE > size_cp){
        memcpy(copy_of_func, addr_base + MAGIC_OFFSET, size_cp);
    }else{
        memcpy(copy_of_func, addr_base + MAGIC_OFFSET, MAGIC_COPY_SIZE);
    }
    printf("  memcpy succeeded\n");
}






void remap_permissions(char *addr, int size, int perm)
{
    char *aligned_addr_base;
    char *aligned_addr_end;
    int size_to_remap;

    //printf("remap_permissions():\n");

    aligned_addr_base = (char *) ((unsigned long)(addr) & ~(page_size - 1));
    aligned_addr_end  = (char *) ((unsigned long)(addr+size) & ~(page_size - 1));
    size_to_remap = page_size + (aligned_addr_end - aligned_addr_base);
    //printf("  aligned_addr_base: %p\n", aligned_addr_base);
    //printf("  aligned_addr_end:  %p\n", aligned_addr_end);
    //printf("  size_to_remap:     %d\n", size_to_remap);

    if(mprotect(aligned_addr_base, size_to_remap, perm) == -1){
        cout << "mprotect error" << endl;
    }
    //printf("  mprotect succeeded\n");
}



void probe_copy(void *copy_of_func, char *addr_base, unsigned int size_cp)
{
	cout << "probe_copy()\n";
    
    //printf("  copy_of_func: %p\n", copy_of_func);
    //printf("  addr_base:    %p\n", addr_base);
    //printf("  size_cp:      %d\n", size_cp);
    //fflush(0);
    //blank_previous_funcs();

    remap_permissions(addr_base, size_cp, RWX_PERM);
    if(MAGIC_COPY_SIZE > size_cp){
        memcpy(addr_base + MAGIC_OFFSET, copy_of_func, size_cp);
    }else{
        memcpy(addr_base + MAGIC_OFFSET, copy_of_func, MAGIC_COPY_SIZE);
    }
    copied_funcs[addr_base] = size_cp;
    blank_flag = 1;
}


void probe_blank(void)
{
	cout << "probe_blank()\n";
    if(blank_flag){
        for(map<char *, unsigned int>::iterator it = copied_funcs.begin();
            it != copied_funcs.end();
            ++it){
            char *addr = it->first;
            unsigned int size = it->second;
            printf("  %p %d\n", addr, size);
            //memset(addr + MAGIC_OFFSET, 0, size);
            // TODO: inline blank_func

            //remap_permissions(addr, size, RWX_PERM);
            blank_func(addr, size);

            // FIXME: This remap is a bug. if two addresses are in the same
            // page, we could end up setting the page to RX and screw up
            // a subsequent loop iteration that tries to memset that page.
            //remap_permissions(addr, size, RX_PERM);
            printf("  mprotect succeeded\n");
        }
        copied_funcs.clear();
        blank_flag = 0;
	    cout << "  succeeded\n";
    }else{
        cout << "  nothing to do\n";
    }
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


void image_load_libc(IMG &img)
{
    RTN rtn;
    void *copy_of_func;
    char *addr_base;
    int size_cp = 0;
    int i = 0;

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
        i++;
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

        addr_to_name_dbgmap[addr_base] = RTN_Name(rtn);
        addr_to_copy_dbgmap[addr_base] = copy_of_func;

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


// Called every time a new image is loaded
VOID ImageLoad(IMG img, VOID *v)
{
    cout << "Image name is: " <<  IMG_Name(img) << endl;

    /*if(IMG_Name(img).find("ld-linux-x86-64.so") != string::npos){
        image_load_libc(img);
    }*/
    
    if(IMG_Name(img).find("libc.so") != string::npos){
        image_load_libc(img);
    }else if(IMG_Name(img).find(APP_NAME) != string::npos){
        image_load_app(img);
    }
}


int main(int argc, CHAR *argv[])
{
    PIN_InitSymbols();

    if( PIN_Init(argc,argv) )
    {
        return Usage();
    }

    TraceFile.open(KnobOutputFile.Value().c_str());
    TraceFile << hex;
    TraceFile.setf(ios::showbase);

    IMG_AddInstrumentFunction(ImageLoad, 0);

    page_size = sysconf(_SC_PAGE_SIZE);
    if(page_size == -1){
        cout << "ERROR: Unable to get system page size" << endl;
        return 1;
    }

    PIN_StartProgramProbed();

    return 0;
}
