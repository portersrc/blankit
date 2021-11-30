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
//
// This tool counts the number of times a routine is executed and 
// the number of instructions executed in a routine
//

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string.h>
#include "pin.H"
#include <unordered_map>
#include <set>

#define INSTRUMENT_rdf "pbarua_custom_instr_fun_RDF"
#define INSTRUMENT_callsite0 "pbarua_custom_instr_callsite_0"
#define INSTRUMENT_callsite1 "pbarua_custom_instr_callsite_1"
#define INSTRUMENT_callsite2 "pbarua_custom_instr_callsite_2"
#define INSTRUMENT_callsite3 "pbarua_custom_instr_callsite_3"
#define INSTRUMENT_callsite4 "pbarua_custom_instr_callsite_4"
#define INSTRUMENT_callsite5 "pbarua_custom_instr_callsite_5"
#define INSTRUMENT_callsite6 "blankit_predict"
#define INSTRUMENT_callreturn "pbarua_custom_instr_return_callsite"
//#define is_instrumentation_call(A ) (strcmp(A,INSTRUMENT_rdf) || strmp(A,INSTRUMENT_callsite1) ||strmp(A,INSTRUMENT_callsite2) ||strmp(A,INSTRUMENT_callsite3) ||strmp(A,INSTRUMENT_callsite4) ||strmp(A,INSTRUMENT_callsite5) )
using namespace std;
ofstream outFile;

// Holds instruction count for a single procedure
typedef struct RtnCount
{
    string _name;
    string _image;
    ADDRINT _address;
    RTN _rtn;
    UINT64 _rtnCount;
    UINT64 _icount;
    struct RtnCount * _next;
} RTN_COUNT;

// Linked list of instruction counts for each routine
RTN_COUNT * RtnList = 0;

static string string_controlFlow_opcodes[] = {"jmp","call","je","jne","jz","jg","jge","jl","jle" };
#define  array_of_instrument_calls_SIZE 9
static string array_of_instrument_calls[array_of_instrument_calls_SIZE] = {INSTRUMENT_rdf,INSTRUMENT_callsite0 ,INSTRUMENT_callsite1 ,INSTRUMENT_callsite2 ,INSTRUMENT_callsite3 ,INSTRUMENT_callsite4,INSTRUMENT_callsite5, INSTRUMENT_callreturn, INSTRUMENT_callsite6};
//static set<string> setOf_controlflow_instrs(string_controlFlow_opcodes );
static unordered_map<ADDRINT, string> str_of_ins_at;
// This function is called before every instruction is executed
VOID before_call(RTN_COUNT *rc)
{
    std::cout << "Called "
        << rc->_name << " "
        << rc->_image << " "
        << endl;
//    std::cout << setw(15) << "Called "
//        << setw(23) << rc->_name << " "
//        << setw(15) << rc->_image << " "
//        << endl;
}
VOID after_call(RTN_COUNT *rc)
{
    std::cout << "Return "
        <<  rc->_name << " "
        <<  rc->_image << " "
        << endl;
}
VOID INSTR_called(ADDRINT addr)
{
    std::cout << "Instruction "
        <<  str_of_ins_at[addr] << " "
        << endl;
}
    
const char * StripPath(const char * path)
{
    const char * file = strrchr(path,'/');
    if (file)
        return file+1;
    else
        return path;
}
VOID Arg1Before(ADDRINT funcN_index, int     a1)
{
  const string name = array_of_instrument_calls[funcN_index];
    std::cout << "Called "
        << name << "(" << a1 << ") " 
        << endl;
}
VOID Arg2Before(ADDRINT funcN_index, int     a1, int     a2)
{
  const string name = array_of_instrument_calls[funcN_index];
    std::cout  << "Called "
         << name << "(" << a1 <<","<<a2 << ") " 
        << endl;
}
VOID Arg3Before(ADDRINT funcN_index, int     a1, int     a2, int     a3)
{
  const string name = array_of_instrument_calls[funcN_index];
    std::cout  << "Called "
         << name << "(" << a1 <<","<<a2 <<","<< a3<< ") " 
        << endl;
}
VOID Arg4Before(ADDRINT funcN_index, int  a1, int  a2, int  a3,  int  a4)
{
  const string name = array_of_instrument_calls[funcN_index];
    std::cout  << "Called "
         << name << "(" << a1 <<","<<a2 <<","<< a3<< ","<<a4<<") " 
        << endl;
}
VOID Arg5Before(ADDRINT funcN_index, int  a1, int  a2, int  a3,  int  a4,  int  a5)
{
  const string name = array_of_instrument_calls[funcN_index];
    std::cout  << "Called "
        << name << "(" << a1 <<","<<a2 <<","<< a3<< ","<<a4<<","<<a5<<") " 
        << endl;
}
VOID Arg6Before(ADDRINT funcN_index, int  a1, int  a2, int  a3,  int  a4,  int  a5,  int  a6)
{
  const string name = array_of_instrument_calls[funcN_index];
    std::cout  << "Called "
         << name << "(" << a1 <<","<<a2 <<","<< a3<< ","<<a4<<","<<a5<<","<<a6<<") " 
        << endl;
}
VOID Arg7Before(ADDRINT funcN_index, int  a1, int  a2, int  a3,  int  a4,  int  a5,  int  a6, int a7)
{
  const string name = array_of_instrument_calls[funcN_index];
    std::cout  << "Called "
         << name << "(" << a1 <<","<<a2 <<","<< a3<< ","<<a4<<","<<a5<<","<<a6<<","<<a7<<") " 
        << endl;
}
VOID instrument_all_custom_functions(IMG img, VOID *v)
{
  // Instrument the malloc() and free() functions.  Print the input argument
  // of each malloc() or free(), and the return value of malloc().
  //
  //  Find the malloc() function.
  for (int i =0 ; i < array_of_instrument_calls_SIZE ; i++) {
    std::string instrument_call_func = array_of_instrument_calls[i];
    RTN instrumentRtn = RTN_FindByName(img, instrument_call_func.c_str());
    if (RTN_Valid(instrumentRtn))
    {
      RTN_Open(instrumentRtn);
      const string rtnName = RTN_Name(instrumentRtn);

      // Instrument malloc() to print the input argument value and the return value.
      switch (i ) {
        case 0 : RTN_InsertCall(instrumentRtn, IPOINT_BEFORE, (AFUNPTR)Arg3Before,
          IARG_ADDRINT, i,
          IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
          IARG_FUNCARG_ENTRYPOINT_VALUE, 1,
          IARG_FUNCARG_ENTRYPOINT_VALUE, 2,
          IARG_END);
                 break;
        case 1 : RTN_InsertCall(instrumentRtn, IPOINT_BEFORE, (AFUNPTR)Arg2Before,
          IARG_ADDRINT, i,
          IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
          IARG_FUNCARG_ENTRYPOINT_VALUE, 1,
          IARG_END);
                 break;
        case 2 : RTN_InsertCall(instrumentRtn, IPOINT_BEFORE, (AFUNPTR)Arg3Before,
          IARG_ADDRINT, i,
          IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
          IARG_FUNCARG_ENTRYPOINT_VALUE, 1,
          IARG_FUNCARG_ENTRYPOINT_VALUE, 2,
          IARG_END);
                 break;
        case 3 : RTN_InsertCall(instrumentRtn, IPOINT_BEFORE, (AFUNPTR)Arg4Before,
          IARG_ADDRINT, i,
          IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
          IARG_FUNCARG_ENTRYPOINT_VALUE, 1,
          IARG_FUNCARG_ENTRYPOINT_VALUE, 2,
          IARG_FUNCARG_ENTRYPOINT_VALUE, 3,
          IARG_END);
                 break;
        case 4 : RTN_InsertCall(instrumentRtn, IPOINT_BEFORE, (AFUNPTR)Arg5Before,
          IARG_ADDRINT, i,
          IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
          IARG_FUNCARG_ENTRYPOINT_VALUE, 1,
          IARG_FUNCARG_ENTRYPOINT_VALUE, 2,
          IARG_FUNCARG_ENTRYPOINT_VALUE, 3,
          IARG_FUNCARG_ENTRYPOINT_VALUE, 4,
          IARG_END);
                 break;
        case 5 : RTN_InsertCall(instrumentRtn, IPOINT_BEFORE, (AFUNPTR)Arg6Before,
          IARG_ADDRINT, i,
          IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
          IARG_FUNCARG_ENTRYPOINT_VALUE, 1,
          IARG_FUNCARG_ENTRYPOINT_VALUE, 2,
          IARG_FUNCARG_ENTRYPOINT_VALUE, 3,
          IARG_FUNCARG_ENTRYPOINT_VALUE, 4,
          IARG_FUNCARG_ENTRYPOINT_VALUE, 5,
          IARG_END);
                 break;
        case 6 : RTN_InsertCall(instrumentRtn, IPOINT_BEFORE, (AFUNPTR)Arg7Before,
          IARG_ADDRINT, i,
          IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
          IARG_FUNCARG_ENTRYPOINT_VALUE, 1,
          IARG_FUNCARG_ENTRYPOINT_VALUE, 2,
          IARG_FUNCARG_ENTRYPOINT_VALUE, 3,
          IARG_FUNCARG_ENTRYPOINT_VALUE, 4,
          IARG_FUNCARG_ENTRYPOINT_VALUE, 5,
          IARG_FUNCARG_ENTRYPOINT_VALUE, 6,
          IARG_END);
                 break;
        case 7 : RTN_InsertCall(instrumentRtn, IPOINT_BEFORE, (AFUNPTR)Arg1Before,
          IARG_ADDRINT, i,
          IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
          IARG_END);
                 break;
        case 8 : RTN_InsertCall(instrumentRtn, IPOINT_BEFORE, (AFUNPTR)Arg1Before,
          IARG_ADDRINT, i,
          IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
          IARG_END);
                 break;
      }

      RTN_Close(instrumentRtn);
    }
  }

}
// Pin calls this function every time a new rtn is executed
VOID Routine(RTN rtn, VOID *v)
{

  std::set<std::string> set_of_instruments;
  for (int i =0 ; i < array_of_instrument_calls_SIZE ; i++) {
    set_of_instruments.insert( array_of_instrument_calls[i]);
  }
  std::string routine_name(RTN_Name(rtn));
  if (set_of_instruments.find(routine_name) != set_of_instruments.end()) return; 
    
    // Allocate a counter for this routine
    RTN_COUNT * rc = new RTN_COUNT;

    // The RTN goes away when the image is unloaded, so save it now
    // because we need it in the fini
    rc->_name = RTN_Name(rtn);
    rc->_image = StripPath(IMG_Name(SEC_Img(RTN_Sec(rtn))).c_str());
    rc->_address = RTN_Address(rtn);
    rc->_icount = 0;
    rc->_rtnCount = 0;

    // Add to list of routines
    rc->_next = RtnList;
    RtnList = rc;
            
    RTN_Open(rtn);
            
    // Insert a call at the entry point of a routine to increment the call count
    RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)before_call, IARG_PTR, rc, IARG_END);
    RTN_InsertCall(rtn, IPOINT_AFTER, (AFUNPTR)after_call, IARG_PTR, rc, IARG_END);
   /* 
    // For each instruction of the routine
    for (INS ins = RTN_InsHead(rtn); INS_Valid(ins); ins = INS_Next(ins))
    {
        if (INS_IsBranchOrCall(ins)) {
            ADDRINT addres_int = INS_Address(ins);
            str_of_ins_at[addres_int] = INS_Disassemble(ins);
            // Insert a call to docount to increment the instruction counter for this rtn
            INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)instr_called, IARG_PTR,addres_int, IARG_END);
        }
    }*/

    
    RTN_Close(rtn);
}

// This function is called when the application exits
// It prints the name and count for each procedure
VOID Fini(INT32 code, VOID *v)
{
    std::cout  << "Procedure" << " "
           << "Image" << " "
           << "Address" << " "
           << "Calls" << " "
           << "Instructions" << endl;

    for (RTN_COUNT * rc = RtnList; rc; rc = rc->_next)
    {
        if (rc->_icount > 0)
            std::cout  << rc->_name << " "
                   << rc->_image << " "
                   << hex << rc->_address << dec <<" "
                   << rc->_rtnCount << " "
                   << rc->_icount << endl;
    }

}

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */

INT32 Usage()
{
    cerr << "This Pintool counts the number of times a routine is executed" << endl;
    cerr << "and the number of instructions executed in a routine" << endl;
    cerr << endl << KNOB_BASE::StringKnobSummary() << endl;
    return -1;
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */

int main(int argc, char * argv[])
{
    // Initialize symbol table code, needed for rtn instrumentation
    PIN_InitSymbols();

    outFile.open("proccount.out");

    // Initialize pin
    if (PIN_Init(argc, argv)) return Usage();

    // Register Image to be called to instrument functions.
    IMG_AddInstrumentFunction(instrument_all_custom_functions, 0);
    // Register Routine to be called to instrument rtn
    RTN_AddInstrumentFunction(Routine, 0);

    // Register Fini to be called when the application exits
    PIN_AddFiniFunction(Fini, 0);
    
    // Start the program, never returns
    PIN_StartProgram();
    
    return 0;
}
