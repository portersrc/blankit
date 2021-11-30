//===- profilePath.cpp - Example code from "Writing an LLVM Pass" ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements two versions of the LLVM "profilePath World" pass described
// in docs/WritingAnLLVMPass.html
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include <algorithm>
#include <deque>
#include <fstream>
#include <iostream>
#include "llvm/ADT/Statistic.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/AliasSetTracker.h"
#include "llvm/Analysis/BasicAliasAnalysis.h"
#include "llvm/Analysis/CaptureTracking.h"
#include "llvm/Analysis/ConstantFolding.h"
#include "llvm/Analysis/GlobalsModRef.h"
#include "llvm/Analysis/Loads.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopPass.h"
//#include "llvm/Analysis/LoopPassManager.h"
#include "llvm/Analysis/MemoryBuiltins.h"
#include "llvm/Analysis/ScalarEvolutionAliasAnalysis.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/Analysis/ValueTracking.h"
#include "llvm/Analysis/VectorUtils.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Metadata.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PredIteratorCache.h"
#include "llvm/IR/TypeBuilder.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/LICM.h"
#include "llvm/Transforms/Utils/Local.h"
#include "llvm/Transforms/Utils/LoopUtils.h"
#include "llvm/Transforms/Utils/SSAUpdater.h"
#include <map>
#include <ostream>
#include <set>
#include<sstream>
#include <vector>
using namespace llvm;
/*{{{*/
#define DEBUG_TYPE "profilePath"
#define debug_stride_pass 0
STATISTIC(BasicBlockCounter, "Counter for basic blocks");
/*}}}*/

namespace {
  // profilePath - The second implementation with getAnalysisUsage implemented.
  struct profilePath : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    profilePath() : FunctionPass(ID) {}
    Module *thisMod;

    GlobalVariable *getStringRef(std::string print_msg ) {
        Module *M = thisMod;
        // Create a constant internal string reference...
        if (debug_stride_pass) errs()<<"\n get global for ::"<<print_msg;
        Constant *Init =ConstantDataArray::getString(M->getContext(),print_msg);
        // Create the global variable and record it in the module
        // The GV will be renamed to a unique name if needed.
        GlobalVariable *GV = new GlobalVariable(Init->getType(), true,
                GlobalValue::InternalLinkage, Init,
                "trstr");
        M->getGlobalList().push_back(GV);
        return GV;
    }
    bool runOnFunction(Function &F) override {

        thisMod = (*inst_begin(F)).getModule() ;
        errs() << "profilePath: ";
        if (F.hasName()  )errs().write_escaped(F.getName()) << '\n';

        Type *ptr_i8 = PointerType::get(Type::getInt8Ty(thisMod->getContext()), 0);
        llvm::Type *ArgTypes[] = { ptr_i8 }	;
        Function *printfFunction =dyn_cast<Function>( thisMod->getOrInsertFunction("printf",
                    FunctionType::get(IntegerType::getInt32Ty(thisMod->getContext()),
                        ArgTypes,true /*this is var arg func type*/) 
                    ) );
        for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
            //errs() << *I << "\n";
            Instruction *ins = &*I;
            //if (CallInst *CI = dyn_cast<CallInst>(ins )) {}
            if (auto *ci = dyn_cast<CallInst>(&*I)) {

                Function *calledFunc = ci->getCalledFunction();

                if (calledFunc == NULL || !calledFunc->hasName()  ) continue;
                std::cout<<"\n Func:"<<calledFunc->getName().str();


                std::ostringstream s;
                s<<"\n Func:"<<calledFunc->getName().str()<<"\n";
                std::string print_msg = s.str();
                if (printfFunction != NULL) {
                    if (debug_stride_pass) errs()<<"\n printf::"<<*printfFunction;

                    GlobalVariable *string_printf = getStringRef(print_msg );
                    //BasicBlock* entry = BasicBlock::Create(thisMod->getContext(), "entry",&F );
                    IRBuilder<> builder(ins);
                    std::vector<Value *> ArgsV;
                    Constant* zero_i64 = Constant::getNullValue(IntegerType::getInt64Ty(thisMod->getContext()));
                    ArrayRef< Value * > indices = {zero_i64,zero_i64};
                    PointerType *pty = dyn_cast<PointerType>(string_printf->getType());
                    GetElementPtrInst *gep_printf_string = GetElementPtrInst::Create( pty->getElementType(), string_printf,indices );
                    if (debug_stride_pass) errs()<<"printf string load:: " <<*gep_printf_string;
                    Value *printf_str_gep = builder.Insert(gep_printf_string,"tmp_blockstride_compute" );
                    if (debug_stride_pass) errs()<<"printf string load:: " <<*printf_str_gep;
                    ArgsV.push_back(printf_str_gep);  
                    Value* callinstr = builder.CreateCall(printfFunction,ArgsV  );
                    if (debug_stride_pass) errs()<<"\n callinstr::"<<*callinstr;
                }
            }
        }
        for (Function::iterator bbI = F.begin() , bbEnd= F.end() ; bbI != bbEnd ; ++bbI) {
            //BasicBlock *block = bbI;
            Instruction *I = bbI->getFirstNonPHI();
            ++BasicBlockCounter;
            std::ostringstream s;
            s<<"\n BB:"<<BasicBlockCounter;
            std::string print_msg = s.str();
            if (printfFunction != NULL) {
                if (debug_stride_pass) errs()<<"\n printf::"<<*printfFunction;

                GlobalVariable *string_printf = getStringRef(print_msg );
                //BasicBlock* entry = BasicBlock::Create(thisMod->getContext(), "entry",&F );
                IRBuilder<> builder(I);
                std::vector<Value *> ArgsV;
                Constant* zero_i64 = Constant::getNullValue(IntegerType::getInt64Ty(thisMod->getContext()));
                ArrayRef< Value * > indices = {zero_i64,zero_i64};
                PointerType *pty = dyn_cast<PointerType>(string_printf->getType());
                GetElementPtrInst *gep_printf_string = GetElementPtrInst::Create( pty->getElementType(), string_printf,indices );
                if (debug_stride_pass) errs()<<"printf string load:: " <<*gep_printf_string;
                Value *printf_str_gep = builder.Insert(gep_printf_string,"tmp_blockstride_compute" );
                if (debug_stride_pass) errs()<<"printf string load:: " <<*printf_str_gep;
                ArgsV.push_back(printf_str_gep);  
                Value* callinstr = builder.CreateCall(printfFunction,ArgsV  );
                if (debug_stride_pass) errs()<<"\n callinstr::"<<*callinstr;
                //builder.CreateCall(printfFunction,string_printf);
                //insert_printf(string_printf,  builder );
            }
        }

        //s<<"\n Block Stride in "<<(block_stride_X==0?"X":"Y")<<" dim for ::";
        //s<<std::string(mem->getName());
        return false;
    }

    // We don't modify the program, so we preserve all analyses.
    void getAnalysisUsage(AnalysisUsage &AU) const override {
      //AU.setPreservesAll();
    }
    void insert_printf(GlobalVariable *string_printf,IRBuilder<> &builder ) {
        //if (compile_prinit) print_instr(computedStride);
        //return; //TODO REMOVE THIS
        std::vector<Value *> ArgsV;
        //= getStringRef(print_msg );
        Constant* zero_i64 = Constant::getNullValue(IntegerType::getInt64Ty(thisMod->getContext()));
        ArrayRef< Value * > indices = {zero_i64,zero_i64};
        PointerType *pty = dyn_cast<PointerType>(string_printf->getType());
        GetElementPtrInst *gep_printf_string = GetElementPtrInst::Create( pty->getElementType(), string_printf,indices );
        if (debug_stride_pass) errs()<<"printf string load:: " <<*gep_printf_string;
        Value *printf_str_gep = builder.Insert(gep_printf_string,"tmp_blockstride_compute" );
        ////ArgsV.push_back( geti8StrVal(*thisMod));
        //errs()<<"\n param type::"<<*computedStride->getType()<<"function wrapper::"<<*wrapper_printfFunction;
        //Type *struct_ty = StructType::get(computedStride->getType(),nullptr );
        //Type *struct_ty = StructType::get(IntegerType::getInt32Ty(thisMod->getContext()),nullptr );
        //if (debug_stride_pass) errs()<<"\n struct type:: "<<*struct_ty;
        //Value * tmp_print_mem = builder.CreateAlloca(struct_ty ,nullptr, "tmp_stride_comp" );
        //if (debug_stride_pass) errs()<<"\n alloca instr::"<<*tmp_print_mem<<"\n";

        //Constant* zero_i32 = Constant::getNullValue(IntegerType::getInt32Ty(thisMod->getContext()));
        //ArrayRef< Value * > indices2 = {zero_i64,zero_i32};
        //if (debug_stride_pass) errs()<<"\n Creating gep with pointer::"<<*tmp_print_mem<< "\n and indiex::"<<*indices[0]<<" and ::"<<*indices[1]<<"\n";
        //if (debug_stride_pass) errs()<<"\n pointer type::"<<*cast<PointerType>(tmp_print_mem->getType()->getScalarType())->getElementType()<<"\n"; 
        //GetElementPtrInst *gepInstr = GetElementPtrInst::Create( struct_ty, tmp_print_mem,indices2 );
  //      Value *print_args_pointer = builder.Insert(gepInstr,"tmp_blockstride_compute" );// 	 builder.CreateGEP(tmp_print_mem,Constant::getNullValue(IntegerType::getInt32Ty(thisMod->getContext())));//indices);
//        if (debug_stride_pass) errs()<<"\n printargspointer::"<<*print_args_pointer<<"\n ";
        //Value *stored_arg = builder.CreateStore(computedStride,print_args_pointer );
        //if (debug_stride_pass) errs()<<"store::"<<*stored_arg;
        //Value *bitcast_arg = builder.CreateBitCast(tmp_print_mem,PointerType::get(IntegerType::getInt8Ty(thisMod->getContext()),0 ));
        ArgsV.push_back(printf_str_gep);  
        //ArgsV.push_back(bitcast_arg);
        //if (debug_stride_pass) errs()<<"\n finally got bitcase as:"<<*bitcast_arg<<"\n and stored arg::"<<*stored_arg;
        Type *ptr_i8 = PointerType::get(Type::getInt8Ty(thisMod->getContext()), 0);
        llvm::Type *ArgTypes[] = { ptr_i8,ptr_i8 }	;
        Function *vprintfFunction =dyn_cast<Function>( thisMod->getOrInsertFunction("vprintf",
                    FunctionType::get(IntegerType::getInt32Ty(thisMod->getContext()),
                        ArgTypes,false /*
                                          this is var arg func type*/) 
                    ) );
        if (vprintfFunction == nullptr ) {
            if (debug_stride_pass) errs()<<"\n func def not found::";
            return;
        }
        if (debug_stride_pass) errs()<<"\n vprintf::"<<*vprintfFunction;
        Value* callinstr = builder.CreateCall(vprintfFunction,ArgsV  );
        if (debug_stride_pass) errs()<<"\n callinstr::"<<*callinstr;

    }
  };
}

char profilePath::ID = 0;
static void registerSkeletonPass(const PassManagerBuilder &,
                         legacy::PassManagerBase &PM) {
      PM.add(new profilePath());
}
static RegisterStandardPasses
  RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                   registerSkeletonPass);
//Y("profilePath", "profilePath World Pass (with getAnalysisUsage implemented)");
