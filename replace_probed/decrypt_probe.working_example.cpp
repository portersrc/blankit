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


using namespace std;

/* ===================================================================== */
/* Global Variables */
/* ===================================================================== */

ofstream TraceFile;
typedef VOID (*VOIDFUNC)();

static VOIDFUNC origFptrNotify1;

ADDRINT PinInitAddress;
ADDRINT PinInitAddressIns;

void *copy_of_func = NULL;
//void *ref_handle = NULL;
#define MAGIC_OFFSET 50
char *addr_base ;

//const char *FUNC_NAME = "PinInit_aux";
//int FUNC_SIZE = 19;
const char *FUNC_NAME = "PinInit";
int FUNC_SIZE = 236;



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


void Notify1(void)
{
    if(origFptrNotify1){
        TraceFile << "Notify 1 before" << endl;

        cout << "Hello before\n";
        //ref_PinInit = dlsym(ref_handle, "PinInit");
        //if((error = dlerror()) != NULL){
        //    fprintf(stderr, "%s\n", error);
        //    exit(1);
        //}
        //ref_PinInit();


        // mmap approach
        //void (*PinInit_aux_fp)(void) = (void (*)(void)) (copy_of_func);
        //PinInit_aux_fp();

        //memcpy((void *)origFptrNotify1, copy_of_func, FUNC_SIZE);
        //(*origFptrNotify1)();

        memcpy((void *)origFptrNotify1, copy_of_func, 8);
        cout << "afterz memcpy\n";
        origFptrNotify1();

        cout << "Hello after\n";

        //TraceFile << "Notify 1 after." << endl;
    }
}


void NotifyBefore(void)
{
	cout << "Notify before called\n";
    /*
    typedef void (*PinInitFp_t)(void);
    PinInitFp_t fp;
    long long unsigned int x;
    cout << "value of address before: " << PinInitAddress << endl;
    x = ((long long unsigned int) (PinInitAddress)) + 14;
    cout << "value of address after: " << x << endl;
    //fp = (void (*)(void)) (PinInitAddress + 14);
    fp = (void (*)(void)) (x);
    //fp = (void (*)(void)) (PinInitAddressIns);
    fp();
    */
    memcpy(addr_base + MAGIC_OFFSET, copy_of_func, 4);
}


BOOL FindAndCheckRtn(IMG img, string rtnName, RTN& rtn)
{
    rtn = RTN_FindByName(img, rtnName.c_str());

    if(RTN_Valid(rtn)){
        if(!RTN_IsSafeForProbedReplacement(rtn)){
            TraceFile << "Cannot replace " << RTN_Name(rtn) << " in " << IMG_Name(img) << endl;
            return FALSE;
        }
        return TRUE;
    }
    TraceFile << "RTN_Valid is failing\n";
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

void dump_sections(IMG img)
{
    SEC sec;
    sec  = IMG_SecHead(img);
    while(sec != SEC_Invalid()){
        cout << "SEC name: " << SEC_Name(sec) << endl;
        if(SEC_Name(sec) == ".text"){
            dump_routines(sec);
        }
        sec  = SEC_Next(sec);
    }
    cout << "Loop broke. SEC is invalid" << endl;


}

SEC get_text_section(IMG img)
{
    SEC sec;
    sec  = IMG_SecHead(img);
    while(sec != SEC_Invalid()){
        cout << "SEC name: " << SEC_Name(sec) << endl;
        if(SEC_Name(sec) == ".text"){
            return sec;
        }
        sec  = SEC_Next(sec);
    }
    return SEC_Invalid();
}

/* ===================================================================== */
// Called every time a new image is loaded
// Lok for routines that we want to probe
VOID ImageLoad(IMG img, VOID *v)
{
    RTN rtn;
    //INS ins;

    if((IMG_Name(img).find("libpin.so") == string::npos)){
        return;
    }
    //if((IMG_Name(img).find("example_app") == string::npos)){
    //    return;
    //}

    //dump_sections(img);

    //SEC text_section;
    //text_section = get_text_section(img);
    //cout << "txt section address: " << SEC_Address(text_section) << endl;
    //cout << "size: "       << SEC_Size(text_section)         << endl;
    //cout << "readable: "   << SEC_IsReadable(text_section)   << endl;
    //cout << "writeable: "  << SEC_IsWriteable(text_section)  << endl;
    //cout << "executable: " << SEC_IsExecutable(text_section) << endl;


    cout << "Image name is: " <<  IMG_Name(img) << endl;
    if (!FindAndCheckRtn(img, FUNC_NAME, rtn)){
        return;
    }
    //if (!FindAndCheckRtn(img, "Summ", rtn)){
    //    return;
    //}


    //open a libc version
    //ref_handle = dlopen("pin/libpin.so", RTLD_LAZY);
    //if(!ref_handle){
    //    fputs(dlerror(), stderr);
    //    exit(1);
    //}

    //AFUNPTR rtnfunptr;
    //rtnfunptr = RTN_Funptr(rtn);

    //cout << "size before: " << RTN_Size(rtn) << endl;
    //cout << "Range before: " << RTN_Range(rtn) << endl;
    //cout << "address before: " << RTN_Address(rtn) << endl;
    //printf("rtnfunptr before: %p\n", rtnfunptr);

    //// Replace the function with a probe
    //origFptrNotify1 = (void (*)(void))RTN_ReplaceProbed( rtn, AFUNPTR( Notify1 ) );
    //cout << "outside of replacement probe\n";

    //cout << "size after: " << RTN_Size(rtn) << endl;
    //cout << "Range after: " << RTN_Range(rtn) << endl;
    //cout << "orig afunptr: " << origFptrNotify1 << endl;
    //printf("c printf orig afunptr: %p\n", origFptrNotify1);
    //cout << "address after: " << RTN_Address(rtn) << endl;
    //printf("rtnfunptr after: %p\n", rtnfunptr);




    // This attempt fails before getting into the probe code
    //memset((void *)origFptrNotify1, 0, RTN_Size(rtn));

    // This attempt fails when it tries to go into (or is it when it tries
    // to return from) the library code. This doesn't make sense, because
    // supposedly the probe shouldn't have even worked, because we're
    // overwriting the 0th byte.
    //memset((void *)origFptrNotify1, 0, 4);

    // .. and if the above  didn't work, then this should never make it to
    // the probe call. Indeed, it has the same problem. (If it made it to
    // the probe call, we could have attempted to memcpy the values back
    // before calling the original function.)
    //copy_of_func = malloc(RTN_Size(rtn));
    //memcpy(copy_of_func, (void *)origFptrNotify1, RTN_Size(rtn));
    //memset((void *)origFptrNotify1, 0, RTN_Size(rtn));

    // This "works". It's just a random choice to go 20 bytes beyond the
    // base, and then to write 4 bytes. The only reason this test is useful
    // is to show that you can indeed write to these addresses without
    // failures.
    // --- this fails after changing to fpic
    //unsigned long addr_base = (unsigned long)  origFptrNotify1;
    //memset((void *)(addr_base + 20), 0, 4);

    // Both of these fail.
    //char *addr_base = (char *) rtnfunptr;
    //memset((void *)(addr_base + 14), 0, RTN_Size(rtn) - 14);
    //char *addr_base = (char *) origFptrNotify1;
    //memset((void *)(addr_base + 14), 0, RTN_Size(rtn) - 14);

    //PinInitAddress = RTN_Address(rtn);
    //RTN impl = RTN_IFuncImplementation(rtn);
    //cout << "address of rtn is: " << RTN_Address(rtn) << endl;
    //cout << "address of impl is: " << RTN_Address(impl) << endl;

    //RTN_Open(rtn);
    //ins = RTN_InsHead(rtn);
    //RTN_Close(rtn);
    //PinInitAddressIns = INS_Address(ins);


    //cout << "address of rtn-ins-head is: " << PinInitAddressIns << endl;

    //Insert the probe before the function
	RTN_InsertCallProbed(rtn, IPOINT_BEFORE, AFUNPTR( NotifyBefore ), IARG_END);

    //cout << "address of rtn after inserting probe is: " << RTN_Address(rtn) << endl;
    //cout << "address of impl after insreting probe is: " << RTN_Address(impl) << endl;

    //PinInitAddressIns = INS_Address(ins);
    //cout << "address of rtn-ins-head after inserting probe is: " << PinInitAddressIns << endl;

    AFUNPTR rtnfunptr;
    rtnfunptr = RTN_Funptr(rtn);

    cout << "size before: " << RTN_Size(rtn) << endl;
    cout << "Range before: " << RTN_Range(rtn) << endl;
    cout << "address before:   0x" << std::hex << RTN_Address(rtn) << endl;
    printf("rtnfunptr before: %p\n", rtnfunptr);

    // mmap approach before
    //copy_of_func = mmap(NULL, RTN_Size(rtn), PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    //memcpy(copy_of_func, (void *)RTN_Address(rtn), RTN_Size(rtn));




    //#define MAGIC_OFFSET 20
    //char *addr_base = (char *) RTN_Address(rtn);
    //int size_to_zero_out = RTN_Size(rtn) - MAGIC_OFFSET;
    //if(size_to_zero_out > 0){
    //    cout << "zeroing out function" << endl;
    //    memset(addr_base + MAGIC_OFFSET, 0, 0);
    //}
    //cout << "done zeroing out function" << endl;

    // Replace the function with a probe
    //origFptrNotify1 = (void (*)(void))RTN_ReplaceProbed( rtn, AFUNPTR( Notify1 ) );
    //cout << "outside of replacement probe\n";



    // Write the function's bytes to file, based on pin's RTN address
    //int pin_func_size = RTN_Size(rtn);
    //char *pin_func_addr = (char *) RTN_Address(rtn);
    //FILE *pin_bytes_fp;
    //pin_bytes_fp = fopen("insert_pin_bytes", "wb");
    //size_t nmemb_written = fwrite(pin_func_addr, pin_func_size, 1, pin_bytes_fp);
    //assert(nmemb_written == 1);
    //fclose(pin_bytes_fp);

    //// Write the function's bytes to file, based on pin's originFptr address
    //int orig_func_size = FUNC_SIZE;
    //char *orig_func_addr = (char *) origFptrNotify1;
    //FILE *orig_bytes_fp;
    //orig_bytes_fp = fopen("orig_bytes", "wb");
    //size_t o_nmemb_written = fwrite(orig_func_addr, orig_func_size, 1, orig_bytes_fp);
    //assert(o_nmemb_written == 1);
    //fclose(orig_bytes_fp);

    //// Write the function's bytes to file, based on the actual bytes within
    //// the lib shared object.
    //const char *lib = "pin/libpin.so";
    ////int lib_func_offset = 0xe80;
    //int lib_func_offset = 0x1061;
    //int lib_func_size = FUNC_SIZE;
    //FILE *lib_fp = fopen(lib, "rb");
    //fseek(lib_fp, 0, SEEK_END);
    //int lib_size = ftell(lib_fp);
    //fseek(lib_fp, 0, SEEK_SET);  //same as rewind(f);
    //char *lib_buf = (char *) malloc(lib_size);
    //size_t nmemb_read = fread(lib_buf, lib_size, 1, lib_fp);
    //assert(nmemb_read == 1);
    //fclose(lib_fp);
    //FILE *lib_bytes_fp = fopen("lib_bytes", "wb");
    //nmemb_written = fwrite(lib_buf+lib_func_offset, lib_func_size, 1, lib_bytes_fp);
    //assert(nmemb_written == 1);
    //fclose(lib_bytes_fp);




    //cout << "size after: " << RTN_Size(rtn) << endl;
    //cout << "Range after: " << RTN_Range(rtn) << endl;
    //cout << "orig afunptr: " << origFptrNotify1 << endl;
    //printf("c printf orig afunptr: %p\n", origFptrNotify1);
    //cout << "address after:   0x" << std::hex << RTN_Address(rtn) << endl;
    //printf("rtnfunptr after: %p\n", rtnfunptr);



    //#define MAGIC_OFFSET 20
    //char *addr_base = (char *) origFptrNotify1;
    //int size_to_zero_out = RTN_Size(rtn) - 2 * MAGIC_OFFSET;
    //if(size_to_zero_out > 0){
    //    cout << "zeroing out function" << endl;
    //    cout << "rtnsize: " << RTN_Size(rtn) << endl;
    //    cout << "size to zero out: " << size_to_zero_out << endl;
    //    memset(addr_base + MAGIC_OFFSET, 0, 4);
    //}
    //cout << "done zeroing out function" << endl;


    // mmap approach after
    ////copy_of_func = mmap(NULL, RTN_Size(rtn), PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    //copy_of_func = mmap(NULL, RTN_Size(rtn), PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    ////memcpy(copy_of_func, (void *)pin_func_addr, RTN_Size(rtn));
    //memcpy(copy_of_func, (void *)(lib_buf+lib_func_offset), lib_func_size);
    ////cout << "memcpy experiment" << endl;
    ////memcpy(pin_func_addr, (void *)(lib_buf+lib_func_offset), lib_func_size);
    ////cout << "memcpy experiment done" << endl;
    ////cout << "memset experiment" << endl;
    ////memset(pin_func_addr, 0, lib_func_size);
    ////cout << "memset experiment done" << endl;
    //origFptrNotify1 = (void (*)(void))RTN_ReplaceProbed( rtn, AFUNPTR( Notify1 ) );


    /*
    //WORKS: the zeroing out of few bytes of origFptrNotify1 after copying
    //those bytes and then copying it back when we want to execute this in the
    //replaceProbeFn
    copy_of_func = malloc(8);
    memcpy(copy_of_func, (void *)(origFptrNotify1), 8);
    memset((void *)origFptrNotify1, 0, 8);
    */


    int pagesize;

    //sa.sa_flags = SA_SIGINFO;
    //sigemptyset(&sa.sa_mask);
    //sa.sa_sigaction = handler;
    //if (sigaction(SIGSEGV, &sa, NULL) == -1)
    //    cout  << "sigaction error" << edl;
 
    pagesize = sysconf(_SC_PAGE_SIZE);
    if (pagesize == -1)
        cout << "sysconf error" << endl;
 
    /* Allocate a buffer aligned on a page boundary;
       initial protection is PROT_READ | PROT_WRITE */
 
    addr_base = (char *) RTN_Address(rtn);
    printf("addr_base before: %p\n", addr_base);
    printf("page size: 0x%x\n", pagesize);
    char *aligned_addr_base = (char *) ((unsigned long)(addr_base) & ~(pagesize - 1));
    printf("aligned_addr_base after: %p\n", aligned_addr_base);

 
    if (mprotect(aligned_addr_base, pagesize,
                PROT_READ | PROT_WRITE | PROT_EXEC) == -1){
        cout << "mprotect error" << endl;
    }
    if (mprotect(aligned_addr_base+0x1000, pagesize,
                PROT_READ | PROT_WRITE | PROT_EXEC) == -1){
        cout << "mprotect error" << endl;
    }
    printf("got beyond mprotect\n");


    cout << "memcpy memcpying" << endl;
    copy_of_func = malloc(4);
    memcpy(copy_of_func, addr_base + MAGIC_OFFSET, 4);
    cout << "done memcpying" << endl;
    cout << "zeroing out function" << endl;
    memset(addr_base + MAGIC_OFFSET, 0, 4);
    cout << "done zeroing out function" << endl;

    //memcpy(copy_of_func, (void *)(lib_buf+lib_func_offset), lib_func_size);
    //cout << "hi" << endl;
    //memset((void *)origFptrNotify1, 0, lib_func_size);


    TraceFile << "Inserted probe for " << "PinInit" << endl;
}

/* ===================================================================== */

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

    PIN_StartProgramProbed();

    return 0;
}

/* ===================================================================== */
/* eof */
/* ===================================================================== */
