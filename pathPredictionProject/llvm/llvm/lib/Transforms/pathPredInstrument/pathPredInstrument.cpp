//===- pathPredInstrument.cpp - Example code from "Writing an LLVM Pass" ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements two versions of the LLVM "pathPredInstrument World" pass described
// in docs/WritingAnLLVMPass.html
//
//===----------------------------------------------------------------------===//

#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Analysis/CFG.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Transforms/Scalar.h"
#include <fstream>
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/ADT/PostOrderIterator.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Analysis/GlobalsModRef.h"
#include "llvm/Analysis/IteratedDominanceFrontier.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instructions.h"
#include "llvm/ProfileData/InstrProf.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Analysis/LoopInfo.h"
#include <iostream>
#include<sstream>
#include "llvm-c/Core.h"
#include <unordered_set>
using namespace llvm;
using namespace std;

#define DEBUG_TYPE "pathPredInstrument"
#define MIN(A,B)  (((A) > (B))?(B):(A) )
#define ENABLE_ONLY_LIBRARY 0

STATISTIC(pathPredInstrumentCounter, "Counts number of functions greeted");
STATISTIC(basicBlockCoutner, "Basic Block ID");
STATISTIC(funcNameID, "Function ID");
STATISTIC(rdfIDCounter,"RDF ID");


namespace {
  struct pathPredInstrument : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    private:
//    SmallPtrSet<BasicBlock *, 16> needRDFofBBs;
    Function *printfFunction0, *printfFunction2, *instrument_func_set_RDF , *printfFunction2_float;
    Function *printfFunction_1int, *printfFunction_2ints, *printfFunction_3ints, *printfFunction_4ints, *printfFunction_5ints,*instrFunc_RDF;
    Function *instrFunc_callsite0,*instrFunc_callsite1,*instrFunc_callsite2,*instrFunc_callsite3,*instrFunc_callsite4,*instrFunc_callsite5, *instrFunc_return_callsite;
    string parent_function_name;
    Constant* zero_i64;
    static unsigned CIcounter;
    std::map<BasicBlock*, unsigned int > basicBlock_id_map;
    std::map<std::string, unsigned int > funcName_id_map;
    std::map<CallInst *, unsigned int > callSite_id_map;
    std::map<std::string, unsigned int > rdf_id_map;
    set<string > nonDiv_func_skiplist;
    std::unordered_set<Function*> applicationFunctions;
    void instrument_profile(Function &F) {
      for (Function::iterator bbIt = F.begin(), bbEnd=F.end(); bbIt != bbEnd ; ++bbIt) {
        BasicBlock *bb = &*bbIt;
        if (basicBlock_id_map.find(bb ) == basicBlock_id_map.end()) {
          basicBlock_id_map[bb] = ++basicBlockCoutner;
        }
        for (BasicBlock::iterator insIt = bb->begin(), insEnd= bb->end(); insIt != insEnd ; ++insIt) {
          //DEBUG(dbgs() << "\n iterating block ::"<<*insIt << '\n');
          if (dyn_cast<InvokeInst>(&*insIt ) ) continue;
          if (auto *callIns = dyn_cast<CallInst>(&*insIt)) {
            Function *calledF = callIns->getCalledFunction();
            if (calledF == NULL ) continue;
            if (calledF->isIntrinsic()|| !calledF->hasName() ) continue;
            if (applicationFunctions.find(calledF ) != applicationFunctions.end() ){
              if (ENABLE_ONLY_LIBRARY )
                continue;
            }
            //errs()<<"\n callinst"<<*callIns;
            //if (libcFuncArray.find(calledF->getName()) == libcFuncArray.end()) 
            //  continue;
            if ((calledF == printfFunction0) ||( calledF == printfFunction2) || (calledF == instrument_func_set_RDF ) || calledF == instrFunc_callsite0||calledF == instrFunc_callsite1||calledF == instrFunc_callsite2||calledF == instrFunc_callsite3||calledF == instrFunc_callsite4||calledF == instrFunc_callsite5 || calledF == instrFunc_return_callsite )
              continue;
            //if (!calledF->hasInternalLinkage()) continue;
            //if ( nonDiv_func_skiplist.find(calledF->getName()) != nonDiv_func_skiplist.end())
            //    continue;
            if (callIns->getDereferenceableBytes(0)) {
              DEBUG(dbgs()<<"\n Skipping derefereceable bytes :"<<*callIns);
              continue;
            }
            std::string callInstrString;
            llvm::raw_string_ostream callrso(callInstrString);
            callIns->print(callrso);
            std::string toFindin = callrso.str();
            std::string ignoreclassStr("%class.");
            if (toFindin.find(ignoreclassStr) != std::string::npos){
              DEBUG(dbgs()<<"\n Skipping ignoreclass :"<<*callIns);
              continue;
            }
            //errs()<<"\n callinst:"<<*callIns;
            //errs()<<"\n called func:"<<calledF->getName();
            DEBUG(dbgs()<<"\n callinst:"<<*callIns);
            DEBUG(dbgs()<<"\n called func:"<<calledF->getName());
            DEBUG(dbgs()<<"\n does not throw:"<<callIns->doesNotThrow());
            DEBUG(dbgs()<<"\n does not throw:"<<calledF->doesNotThrow());
            DEBUG(dbgs()<<"\n get getDereferenceableBytes:"<<callIns->getDereferenceableBytes(0));
            if (callSite_id_map.find(callIns) == callSite_id_map.end())
              callSite_id_map[callIns] = ++ CIcounter;
            DEBUG(dbgs() <<"\ninstrument_profile CIcounter:"<<CIcounter);
            DEBUG(dbgs() << " pathPredInstrument: got call instr "<<*callIns<<"\n");
            std::string calledFName = calledF->getName();
            if (funcName_id_map.find(calledFName) == funcName_id_map.end() )
              funcName_id_map[calledFName] = ++funcNameID;
            //errs()<<"\n functionname :"<<calledFName<<": id:"<<funcName_id_map[calledFName]; 
            DEBUG(dbgs()<<"\n with arguments ::" );
            //s<<"\n#Func:"<<calledFName<<":"<<callSite_id_map[callIns]<<":"<<funcName_id_map[calledFName]<<":";
            unsigned int numArgs = callIns->getNumArgOperands();
            if (numArgs >5) numArgs = 0;
            numArgs = 0;
                  std::ostringstream callsite_info;
                  callsite_info<<"\n#_CALL_instrument:funcID:"<<funcName_id_map[calledFName]<<":CIcounter:"<<callSite_id_map[callIns]<<":"; //<<":argNum:"<<i<<":";
                  callsite_info<<"_max_args_:"<<numArgs<<":";
                  set<Value*> func_arguments_set;
                  bool cannotInstrument = false;

                  for (unsigned int i = 0 ; i < numArgs ; i++) {
                    Value * argV = callIns->getArgOperand(i);
                    std::string prnt_type;
                    llvm::raw_string_ostream rso(prnt_type);
                    argV->getType()->print(rso);
                    DEBUG(dbgs()<<"argument::"<<i<<" = "<<*argV<<" of type::"<< rso.str()
                        );
                    if (dyn_cast<InvokeInst>(argV ) ) {
                      cannotInstrument = true;
                      DEBUG(dbgs()<<"\n IS invoke instr should ignore:"<<*argV);
                      break;
                    }
                    if (Instruction *argI = dyn_cast<Instruction>(argV) ) {
                      if (auto *c = dyn_cast<CallInst>(argI)) {
                        if (c->getDereferenceableBytes(0)) {
                          cannotInstrument = true;
                          DEBUG(dbgs()<<"\n IS invoke instr should ignore:"<<*argV);
                          break;
                        }
                      }
                      
                      SmallPtrSet<BasicBlock *, 16> needRDFofBBs;
                      get_parent_PHI_def_for(argI, needRDFofBBs );
                      //Now the needrdf is set, so get the rdfs and then instrument them, before moving on to next function call
                    DEBUG(dbgs()<<"argument::"<<*argI);
                      if ( dyn_cast<Instruction>(argV) && (argV->getType()->isIntegerTy() || argV->getType()->isFloatTy() || 
                            argV->getType()->isDoubleTy() 
                          || argV->getType()->isPointerTy())
                          ) {
                      DEBUG(dbgs()<<"valid type argument::"<<*argI);
                        func_arguments_set.insert(argV);
                      }
                      //std::string print_msg = s.str();
                      //insertFcall(callIns,print_msg,-1, argV);
                      SmallVector<BasicBlock *, 32> RDFBlocks;
                      PostDominatorTree &PDT = getAnalysis<PostDominatorTreeWrapperPass>().getPostDomTree();
                      computeControlDependence(PDT, needRDFofBBs, RDFBlocks);
                      DEBUG(dbgs()<<"Calling rdf instrument::"<<RDFBlocks.size());
                      std::string rdf_info_str = instrumentRDF(RDFBlocks, callSite_id_map[callIns], i+1 );
                      callsite_info<<rdf_info_str;
                      //needRDFofBBs.clear();
                    }
                  }
                  if (cannotInstrument) continue;
            //SmallVector<BasicBlock *, 32> RDFBlocks;
            //PostDominatorTree &PDT = getAnalysis<PostDominatorTreeWrapperPass>().getPostDomTree();
            //computeControlDependence(PDT, needRDFofBBs, RDFBlocks);
            //callSiteBB.insert(bb);
            //break;

            std::string print_msg = callsite_info.str();
            instrumentCallSite(callIns,callSite_id_map[callIns], func_arguments_set, funcName_id_map[calledFName] );
            //insertFcall(callIns,print_msg, func_arguments_set);
          }//foreach callsite
        }
      }
      return;
    }
    void get_parent_PHI_def_for(Instruction *argI, SmallPtrSet<BasicBlock *, 16> &needRDFofBBs  ){
      if (argI == NULL) return ;
      //DEBUG(dbgs() << "\n Tracking def ::"<<*argI << '\n');
      if (PHINode *phiDef= dyn_cast<PHINode>(argI ) ) {
        //DEBUG(dbgs() << "pathPredInstrument: gpt phi node "<<*phiDef);
        for (auto defBB = phiDef->block_begin() , bbEnd = phiDef->block_end(); defBB != bbEnd ; defBB++){
          BasicBlock *bb = *defBB;
          needRDFofBBs.insert(bb);
        }
        return;
      }
      for (int i= 0, num = argI->getNumOperands(); i < num ; i++) {
        Instruction *opI =dyn_cast<Instruction>(  argI->getOperand(i));
        if (opI == NULL) continue;
        get_parent_PHI_def_for(opI, needRDFofBBs);
      }
    }
    void insertRDFCall(Instruction *instrInstrumentBefore,const int rdf_num, const int callSite_num, 
        const int argument_num) {
      Module *thisModule = instrInstrumentBefore->getModule();
      //if ( instrFunc_RDF == 0 ) setPrintfFunc(thisModule);
      //if (funcArg != NULL && printfFunction2 == 0 ) setPrintfFunc(thisModule);
      if (instrFunc_RDF== 0 ) {
        errs()<<"\\n PANIC, ERROR cannot find function to be instrumented with";
        exit(-1);
      }
      Function *userInstrumentFunc = instrFunc_RDF;
      DEBUG(dbgs()<<"\n inserting rdf function::"<<*userInstrumentFunc);
      //errs()<<"\n inserting rdf function::"<<*userInstrumentFunc;

      IRBuilder<> builder(instrInstrumentBefore);
      std::vector<Value *> ArgsV;
      // GlobalVariable *string_printf = getStringRef(thisModule, instrMsg);

      // Create a constant internal string reference...
      // Create the global variable and record it in the module
      // The GV will be renamed to a unique name if needed.
      Value *rdf_Value = llvm::ConstantInt::get(thisModule->getContext(), llvm::APInt(32,rdf_num,false));
      Value *callsite_Value = llvm::ConstantInt::get(thisModule->getContext(), llvm::APInt(32,callSite_num,false));
      Value *callsite_Arg   = llvm::ConstantInt::get(thisModule->getContext(), llvm::APInt(32,argument_num,false));
      ArgsV.push_back(rdf_Value );
      ArgsV.push_back(callsite_Value);
      ArgsV.push_back(callsite_Arg);
      Value* callinstr = builder.CreateCall(userInstrumentFunc ,ArgsV  );
      DEBUG(dbgs()<<"\n callinstr::"<<*callinstr);
      DEBUG(dbgs()<<" Instrumented at RDF::"<<rdf_num<<" for callsite::"<<callSite_num);
    }
    void instrumentCallSite(Instruction *instrInstrumentBefore,unsigned int callsite_id,set<Value *> func_arguments_set, unsigned calledFID) {
      Module *thisModule = instrInstrumentBefore->getModule();
      if ( printfFunction0 == 0 ) setPrintfFunc(thisModule);
      //if (funcArg != NULL && printfFunction2 == 0 ) setPrintfFunc(thisModule);
      if (printfFunction0 == 0 ) {
        errs()<<"\\n PANIC, ERROR cannot find function to be instrumented with";
        exit(-1);
      }
      int f_max_args = MIN((func_arguments_set.size()),5);
      Function *userInstrumentFunc = nullptr; 
      //(funcArg == NULL ? printfFunction0 : printfFunction2 );
      if (f_max_args == 0 ){
        userInstrumentFunc = instrFunc_callsite0;
      }else if (f_max_args == 1){
        userInstrumentFunc = instrFunc_callsite1;
      }else if (f_max_args == 2){
        userInstrumentFunc = instrFunc_callsite2;
      }else if (f_max_args == 3){
        userInstrumentFunc = instrFunc_callsite3;
      }else if (f_max_args == 4){
        userInstrumentFunc = instrFunc_callsite4;
      }else if (f_max_args == 5){
        userInstrumentFunc = instrFunc_callsite5;
      }
      DEBUG(dbgs()<<"\n function::"<<*userInstrumentFunc);
      DEBUG(dbgs()<<"Instrumented for callins::"<<instrInstrumentBefore<<" callsite::"
          <<callsite_id);

      IRBuilder<> builder(instrInstrumentBefore);
      std::vector<Value *> ArgsV;
      Type *int32Ty = IntegerType::getInt32Ty(thisModule->getContext() );
      ArgsV.push_back(llvm::ConstantInt::get(int32Ty, callsite_id,false));
      ArgsV.push_back(llvm::ConstantInt::get(int32Ty, calledFID,false));
      for (Value* funcArg : func_arguments_set ) {
        Value * castedArg = nullptr;
        if (funcArg != NULL ) {
          if (funcArg->getType()->isFloatTy()||funcArg->getType()->isDoubleTy() )
            castedArg = builder.CreateFPToSI(funcArg, int32Ty );
          else if ( funcArg->getType()->isIntegerTy() )
            castedArg = builder.CreateIntCast(funcArg , int32Ty, true)  ;
          else if (funcArg->getType()->isPointerTy())
            castedArg = builder.CreatePtrToInt(funcArg , int32Ty)  ;

          if (castedArg == nullptr) continue;
          ArgsV.push_back(castedArg);  
          DEBUG(dbgs()<<"\n pushing::"<<*castedArg);
        }
      }
      Value* callinstr = builder.CreateCall(userInstrumentFunc ,ArgsV  );
      DEBUG(dbgs()<<"\n callinstr::"<<*callinstr);
      builder.SetInsertPoint(instrInstrumentBefore->getNextNode() );
      std::vector<Value *> ArgsVr;
      ArgsVr.push_back(llvm::ConstantInt::get(int32Ty, callsite_id,false));
      builder.CreateCall(instrFunc_return_callsite ,ArgsVr  );
    }
    void insertFcall(Instruction *instrInstrumentBefore, std::string instrMsg, 
        set<Value *> func_arguments_set) {
      Module *thisModule = instrInstrumentBefore->getModule();
      if ( printfFunction0 == 0 ) setPrintfFunc(thisModule);
      //if (funcArg != NULL && printfFunction2 == 0 ) setPrintfFunc(thisModule);
      if (printfFunction0 == 0 ) {
        errs()<<"\\n PANIC, ERROR cannot find function to be instrumented with";
        exit(-1);
      }
      int f_max_args = func_arguments_set.size();
      Function *userInstrumentFunc = nullptr; 
      //(funcArg == NULL ? printfFunction0 : printfFunction2 );
      if (f_max_args == 0 ){
        userInstrumentFunc = printfFunction0;
      }else if (f_max_args == 1){
        userInstrumentFunc = printfFunction_1int;
      }else if (f_max_args == 2){
        userInstrumentFunc = printfFunction_2ints;
      }else if (f_max_args == 3){
        userInstrumentFunc = printfFunction_3ints;
      }else if (f_max_args == 4){
        userInstrumentFunc = printfFunction_4ints;
      }else if (f_max_args == 5){
        userInstrumentFunc = printfFunction_5ints;
      }
      DEBUG(dbgs()<<"\n function::"<<*userInstrumentFunc);

      IRBuilder<> builder(instrInstrumentBefore);
      std::vector<Value *> ArgsV;
      Constant* zero_i64 = Constant::getNullValue(IntegerType::getInt64Ty(thisModule->getContext()));
      ArrayRef< Value * > indices = {zero_i64,zero_i64};
      // GlobalVariable *string_printf = getStringRef(thisModule, instrMsg);

      // Create a constant internal string reference...
      DEBUG(dbgs() << "\n get global for ::"<<instrMsg << '\n');
      Constant *Init =ConstantDataArray::getString(thisModule->getContext(),instrMsg);
      // Create the global variable and record it in the module
      // The GV will be renamed to a unique name if needed.
      GlobalVariable *GV = new GlobalVariable(Init->getType(), true,
          GlobalValue::InternalLinkage, Init,
          "trstr");
      thisModule->getGlobalList().push_back(GV);
      GlobalVariable *string_printf = GV;
      PointerType *pty = dyn_cast<PointerType>(string_printf->getType());
      //DEBUG(dbgs()<<"printf func:: "<<*userInstrumentFunc <<"::zero_i64:: "<<*zero_i64<<":: string  "<<*string_printf<<":: pty gelem type:: "<<* pty->getElementType() );
      GetElementPtrInst *gep_printf_string = GetElementPtrInst::Create( pty->getElementType(), string_printf,indices );
      //DEBUG(dbgs()<<"printf string load:: " <<*gep_printf_string);
      Value *printf_str_gep = builder.Insert(gep_printf_string,"tmp_blockstride_compute" );
      //DEBUG(dbgs()<<"printf string load:: " <<*printf_str_gep);
      ArgsV.push_back(printf_str_gep);  
      Type *int32Ty = IntegerType::getInt32Ty(thisModule->getContext() );
      for (Value* funcArg : func_arguments_set ) {
        Value * castedArg = nullptr;
        if (funcArg != NULL ) {
          if (funcArg->getType()->isFloatTy()||funcArg->getType()->isDoubleTy() )
            castedArg = builder.CreateFPToSI(funcArg, int32Ty );
          else if ( funcArg->getType()->isIntegerTy() )
            castedArg = builder.CreateIntCast(funcArg , int32Ty, true)  ;
          if (castedArg == nullptr) continue;
            ArgsV.push_back(castedArg);  
            DEBUG(dbgs()<<"\n pushing::"<<*castedArg);
        }
      }
      Value* callinstr = builder.CreateCall(userInstrumentFunc ,ArgsV  );
      DEBUG(dbgs()<<"\n callinstr::"<<*callinstr);
    }
    void setPrintfFunc(Module *thisModule) {
      LLVMContext &ctxt = thisModule->getContext();
      Type *ptr_i8 = PointerType::get(Type::getInt8Ty(ctxt ), 0);
      llvm::Type *ArgTypes[] = { ptr_i8 }	;
      std::string custom_instr_func_name("pbarua_custom_instr_fun");//"printf"
      printfFunction0 =dyn_cast<Function>( thisModule->getOrInsertFunction(custom_instr_func_name.c_str() ,
            FunctionType::get(IntegerType::getInt32Ty(ctxt),
              ArgTypes,true /*this is var arg func type*/) 
            ) );
      Type *int32Ty =  IntegerType::getInt32Ty(ctxt );
      llvm::Type *ArgTypes2[] = { ptr_i8,  int32Ty  }	;
      custom_instr_func_name = "pbarua_custom_instr_fun_2";//"printf"
      printfFunction_1int =dyn_cast<Function>( thisModule->getOrInsertFunction(custom_instr_func_name.c_str() ,
            FunctionType::get(int32Ty ,
              ArgTypes2,false /*this is var arg func type*/) 
            ) );

      llvm::Type *ArgTypes_3[] = { ptr_i8,  int32Ty, int32Ty  }	;
      custom_instr_func_name = "pbarua_custom_instr_fun_3";//"printf"
      printfFunction_2ints =dyn_cast<Function>( thisModule->getOrInsertFunction(custom_instr_func_name.c_str() ,
            FunctionType::get(int32Ty ,
              ArgTypes_3,false /*this is var arg func type*/) 
            ) );

      llvm::Type *ArgTypes_4[] = { ptr_i8,  int32Ty, int32Ty , int32Ty }	;
      custom_instr_func_name = "pbarua_custom_instr_fun_4";//"printf"
      printfFunction_3ints =dyn_cast<Function>( thisModule->getOrInsertFunction(custom_instr_func_name.c_str() ,
            FunctionType::get(int32Ty ,
              ArgTypes_4,false /*this is var arg func type*/) 
            ) );

      llvm::Type *ArgTypes_5[] = { ptr_i8,  int32Ty, int32Ty , int32Ty, int32Ty};
      custom_instr_func_name = "pbarua_custom_instr_fun_5";//"printf"
      printfFunction_4ints =dyn_cast<Function>( thisModule->getOrInsertFunction(custom_instr_func_name.c_str() ,
            FunctionType::get(int32Ty ,
              ArgTypes_5,false /*this is var arg func type*/) 
            ) );
      llvm::Type *ArgTypes_6[] = { ptr_i8,  int32Ty, int32Ty , int32Ty,int32Ty, int32Ty};
      custom_instr_func_name = "pbarua_custom_instr_fun_6";//"printf"
      printfFunction_5ints =dyn_cast<Function>( thisModule->getOrInsertFunction(custom_instr_func_name.c_str() ,
            FunctionType::get(int32Ty ,
              ArgTypes_6,false /*this is var arg func type*/) 
            ) );
      Type *fpType =  Type::getFloatTy(ctxt );
      llvm::Type *ArgTypes4[] = { ptr_i8,  fpType }	;
      custom_instr_func_name = "pbarua_custom_instr_fun_2_float";//"printf"
      printfFunction2_float =dyn_cast<Function>( thisModule->getOrInsertFunction(custom_instr_func_name.c_str() ,
            FunctionType::get(int32Ty ,
              ArgTypes4,false /*this is var arg func type*/) 
            ) );
      llvm::Type *ArgTypes3[] = { int32Ty  }	;
      custom_instr_func_name = "pbarua_custom_instr_set_rdf";//"printf"
      instrument_func_set_RDF =dyn_cast<Function>( thisModule->getOrInsertFunction(custom_instr_func_name.c_str() ,
            FunctionType::get(int32Ty ,
              ArgTypes3,false /*this is var arg func type*/) 
            ) );

      zero_i64 = Constant::getNullValue(IntegerType::getInt64Ty(ctxt));
      llvm::Type *ArgTypes_rdf[] = {int32Ty, int32Ty,  int32Ty  }	;
      custom_instr_func_name = "pbarua_custom_instr_fun_RDF";//"printf"
      instrFunc_RDF =dyn_cast<Function>( thisModule->getOrInsertFunction(custom_instr_func_name.c_str() ,
            FunctionType::get(int32Ty ,
              ArgTypes_rdf,false /*this is var arg func type*/) 
            ) );
      {
        llvm::Type *ArgTypes_callsite[] = {int32Ty  }	;
        custom_instr_func_name = "pbarua_custom_instr_return_callsite";//"printf"
        instrFunc_return_callsite =dyn_cast<Function>( thisModule->getOrInsertFunction(custom_instr_func_name.c_str() ,
              FunctionType::get(int32Ty ,
                ArgTypes_callsite,false /*this is var arg func type*/) 
              ) );
      }
      {
        llvm::Type *ArgTypes_callsite[] = {int32Ty, int32Ty }	;
        custom_instr_func_name = "pbarua_custom_instr_callsite_0";//"printf"
        instrFunc_callsite0 =dyn_cast<Function>( thisModule->getOrInsertFunction(custom_instr_func_name.c_str() ,
              FunctionType::get(int32Ty ,
                ArgTypes_callsite,false /*this is var arg func type*/) 
              ) );
      }
      {
        llvm::Type *ArgTypes_callsite[] = {int32Ty,int32Ty , int32Ty }	;
        custom_instr_func_name = "pbarua_custom_instr_callsite_1";//"printf"
        instrFunc_callsite1 =dyn_cast<Function>( thisModule->getOrInsertFunction(custom_instr_func_name.c_str() ,
              FunctionType::get(int32Ty ,
                ArgTypes_callsite,false /*this is var arg func type*/) 
              ) );
      }
      {
        llvm::Type *ArgTypes_callsite[] = {int32Ty,int32Ty ,int32Ty, int32Ty }	;
        custom_instr_func_name = "pbarua_custom_instr_callsite_2";//"printf"
        instrFunc_callsite2 =dyn_cast<Function>( thisModule->getOrInsertFunction(custom_instr_func_name.c_str() ,
              FunctionType::get(int32Ty ,
                ArgTypes_callsite,false /*this is var arg func type*/) 
              ) );
      }
      {
        llvm::Type *ArgTypes_callsite[] = {int32Ty ,int32Ty ,int32Ty,int32Ty, int32Ty}	;
        custom_instr_func_name = "pbarua_custom_instr_callsite_3";//"printf"
        instrFunc_callsite3 =dyn_cast<Function>( thisModule->getOrInsertFunction(custom_instr_func_name.c_str() ,
              FunctionType::get(int32Ty ,
                ArgTypes_callsite,false /*this is var arg func type*/) 
              ) );
      }
      {
        llvm::Type *ArgTypes_callsite[] = {int32Ty ,int32Ty,int32Ty ,int32Ty,int32Ty, int32Ty}	;
        custom_instr_func_name = "pbarua_custom_instr_callsite_4";//"printf"
        instrFunc_callsite4 =dyn_cast<Function>( thisModule->getOrInsertFunction(custom_instr_func_name.c_str() ,
              FunctionType::get(int32Ty ,
                ArgTypes_callsite,false /*this is var arg func type*/) 
              ) );
      }
      {
        llvm::Type *ArgTypes_callsite[] = {int32Ty ,int32Ty ,int32Ty,int32Ty,int32Ty,int32Ty, int32Ty}	;
        custom_instr_func_name = "pbarua_custom_instr_callsite_5";//"printf"
        instrFunc_callsite5 =dyn_cast<Function>( thisModule->getOrInsertFunction(custom_instr_func_name.c_str() ,
              FunctionType::get(int32Ty ,
                ArgTypes_callsite,false /*this is var arg func type*/) 
              ) );
      }
      //printfFunction0 =dyn_cast<Function>( thisModule->getOrInsertFunction("pbarua_custom_instr_fun",
      //      FunctionType::get(IntegerType::getInt32Ty(ctxt),
      //        ArgTypes,false/*this is var arg func type*/) 
      //      ) );
    }
    void computeControlDependence(PostDominatorTree &PDT, SmallPtrSet<BasicBlock *, 16> getRDFofBlocks, SmallVector<BasicBlock *, 32> &RDFBlocks) {
      ReverseIDFCalculator RDF(PDT) ;
      RDF.setDefiningBlocks(getRDFofBlocks );
      RDF.calculate(RDFBlocks);

    }
    std::string  instrumentRDF(SmallVector<BasicBlock *, 32> RDFBlocks,unsigned  int callSite_num, int argument_num){
      SmallVector<BasicBlock *, 32> worklistBBs;
      std::string return_str;
      int num_of_rdfs = 0;
      std::ostringstream rdf_info_s;
      std::string str1 = "_"+to_string(argument_num) + "_rdf_num_";
      std::string str2 = "_"+ to_string(argument_num) + "_num_of_rdfs_";//<<":"<<rdfIDCounter <<":";
      std::set<int> printed_rdfids;
      for (auto *BB : RDFBlocks) {
        TerminatorInst *ti = BB->getTerminator();
        if (auto *invokeI = dyn_cast<InvokeInst>(ti)){
          continue;
        }
        DEBUG(dbgs() << "live control in: " <<* BB->getTerminator() << '\n');
        //unsigned int bbID = basicBlock_id_map[BB];
        //parent_s<<"\n#_control_dep_BB:"<<bbID <<":"<<callSite_num<<":";
        //std::string parent_print_msg = parent_s.str();
        //insertFcall(BB->getTerminator(),parent_print_msg, bbID, NULL );
            std::string callInstrString;
            llvm::raw_string_ostream callrso(callInstrString);
            ti->print(callrso);
            std::string toFindin = callrso.str();
            std::string ignoreclassStr("%class.");
            if (toFindin.find(ignoreclassStr) != std::string::npos){
              continue;
            }
            ignoreclassStr="dereferenceable("; 
            if (toFindin.find(ignoreclassStr) != std::string::npos){
              continue;
            }
        //unsigned int succ_num = 0;
          DEBUG(dbgs()<<"\n RDF iterating bb::"<<BB->getName() );
        for (BasicBlock *succBB : ti->successors()){
          
            num_of_rdfs++;
            std::ostringstream s;
            //unsigned int succ_bbID = basicBlock_id_map[succBB];
            std::string rdf_s =parent_function_name+ BB->getName().str()+ "_"+ succBB->getName().str();
            if (rdf_id_map.find(rdf_s ) == rdf_id_map.end()) {
              rdf_id_map[rdf_s] = ++rdfIDCounter;
                rdf_info_s<<str1<<num_of_rdfs<<":"<<rdfIDCounter<<":";
            } else {//if this string exists that means already instrumented this block
              //rdf_info_s<<"_rdf_num_"<<num_of_rdfs<<":"<<rdfIDCounter <<":";
              if (printed_rdfids.find(rdfIDCounter ) == printed_rdfids.end())
                rdf_info_s<<str1<<num_of_rdfs<<":"<<rdfIDCounter<<":";
              printed_rdfids.insert(rdfIDCounter);

              continue;
            }
            s<<"\n#_control_dep_BB:"<<rdfIDCounter <<":";// <<callSite_num<<":"<<argument_num<<":";
            DEBUG(dbgs()<<"\n RDF string::"<<s.str() );
            //s<<"_"<<succ_num++ <<":";
            std::string print_msg = s.str();
            set<Value*> empty_args;
            //insertFcall(succBB->getFirstNonPHIOrDbgOrLifetime(), print_msg, empty_args );
            insertRDFCall(succBB->getFirstNonPHIOrDbgOrLifetime(),rdfIDCounter, callSite_num, argument_num);
          
        }
      }
      return_str = "_"+to_string(argument_num ) +"_total_rdf_count:"+to_string(num_of_rdfs )+":"+rdf_info_s.str();
      return return_str;//must containt the rdf_ids that this call site depends on
    }
    public:
    pathPredInstrument() : FunctionPass(ID), printfFunction0(0), zero_i64(0) {}

    ~pathPredInstrument() {
      ofstream map_file;
      map_file.open ("rdf_id_map.txt");
      for (auto map_it : rdf_id_map) {
        map_file<<map_it.first <<","<<map_it.second<<"\n";
        //errs()<<"\n writing:"<<map_it.first<<","<<map_it.second;
        //myfile << "Writing this to a file.\n";
      }
      map_file.close();

      map_file.open ("funcName_id_map.csv",std::ofstream::out  );
      for (auto map_it : funcName_id_map) {
        map_file<<map_it.first <<","<<map_it.second<<"\n";
        //myfile << "Writing this to a file.\n";
      }
      map_file.close();
    }
    bool runOnFunction(Function &F) override {
      ++pathPredInstrumentCounter;
      //errs() << "pathPredInstrument: ";
      //errs().write_escaped(F.getName()) << '\n';
      parent_function_name = F.getName();
      instrument_profile(F);
      //SmallVector<BasicBlock *, 32> RDFBlocks;
      //PostDominatorTree &PDT = getAnalysis<PostDominatorTreeWrapperPass>().getPostDomTree();
      //computeControlDependence(PDT, needRDFofBBs, RDFBlocks);
      //instrument_print_call_args(F );

      return false;
    }

    bool doInitialization(Module &M) override {
      setPrintfFunc(&M);
      for(auto &f : M){
        if (f.hasName() && !f.isDeclaration()) {
          //errs()<<"Skipping function::"<<f.getName();
          if (f.hasName())
            applicationFunctions.insert(&f);
        }

        if(f.isDeclaration()) continue;
        for(auto &b : f){
          for(auto &i : b){
            CallInst *CI = dyn_cast<CallInst>(&i);
            if(CI){
              Function *calledf = CI->getCalledFunction();
              if (calledf == NULL) continue;
              if (calledf->isIntrinsic()) continue;
              if (callSite_id_map.find(CI) == callSite_id_map.end()) {
                callSite_id_map[CI] = ++CIcounter;
                //errs()<<"\n id:"<<CIcounter<<"::callsite ::"<<*CI;
              }
            }
          }
        }

      }

      //for (auto &F: M) {
      //  if (F.hasName() && !F.isDeclaration()) {
      //    //errs()<<"\t skipping function::"<<F.getName();
      //    applicationFunctions.insert(&F);
      //  }
      //}
      ifstream func2idmap;
      func2idmap.open("funcName_id_map.csv");
      if(func2idmap.is_open()){
        string line;
        while(getline(func2idmap,line)) {
          stringstream ss(line);
          ss>>ws;//skip whitespaces
          string token, funcName;
          int id=0;
          if (getline(ss, token, ',')){
            funcName = token;
          }
          if (getline(ss, token, ',')){
            id = std::stoi(token);
          }
          //errs()<<"\nid:"<<id<<"="<<funcName;
          funcName_id_map[funcName] = id;

          while (getline(ss, token, ',')){
            //errs()<<"\n token for func2id:"<<token<<":";
          }
        }
      }
      return false;
    }
    void getAnalysisUsage(AnalysisUsage &AU) const override {
      AU.addRequired<PostDominatorTreeWrapperPass>();
      AU.addRequired<LoopInfoWrapperPass>();
      AU.setPreservesCFG();
      AU.addPreserved<GlobalsAAWrapperPass>();
    }
    private:

  };
}

unsigned pathPredInstrument::CIcounter=0;
char pathPredInstrument::ID = 0;
static RegisterPass<pathPredInstrument>
Y("pathPredInstrument", "pathPredInstrument Pass (with getAnalysisUsage implemented)");

static void registerSkeletonPass(const PassManagerBuilder &,
                         legacy::PassManagerBase &PM) {
      PM.add(new pathPredInstrument());
}
static RegisterStandardPasses
  RegisterMyPass(PassManagerBuilder::EP_OptimizerLast,//EP_LoopOptimizerEnd, //EP_EnabledOnOptLevel0,//EP_OptimizerLast,// EP_EarlyAsPossible,
                   registerSkeletonPass);




