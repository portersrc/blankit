//
// Created by girish on 11/11/17.
//

#include <fstream>
#include <sstream>
#include "llvm/ADT/Statistic.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/GlobalsModRef.h"
#include <llvm/IR/Constants.h>
#include "llvm/IR/Module.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/Analysis/CFG.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/ADT/PostOrderIterator.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Analysis/IteratedDominanceFrontier.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Analysis/LoopInfo.h"
#include <unordered_set>

#define WINDOW_SIZE 10
#define MIN_RDF_P 2 
#define MAX_RDF_P 11
#define MIN_F_ARG_P 12
#define MAX_F_ARG_P  21



#define DEBUG_TYPE "pathPredInstrument"
using namespace std;
using namespace llvm;

namespace{

  struct embedDtree : public FunctionPass {

    static char ID;
    static int basicBlockCoutner;
    static int rdfIDCounter;
    static int funcIDCounter;
    LLVMContext *C;
    Module *thisModule;
    static unsigned int CIcounter;
    Type *int32Ty;
    map<string, vector<string>> dtree;
    map<string, int > rdf_id_map;
    map<int ,string> paramMap;
    map<Instruction*, int> mCallSiteID;
    std::map<BasicBlock*, unsigned int > basicBlock_id_map;
    SmallPtrSet<BasicBlock *, 16> needRDFofBBs;
    std::map<std::string, unsigned int > funcName_id_map;
    map<Instruction*, set<BasicBlock*>> instruction_to_rdf_succ_map;
    map<Instruction *, AllocaInst*> argI_to_rdfFlag_map;
    map<int, std::string> global_id2string_map;
    map< std::string, int> global_string2id_map;
    map< int , int> stringId_2counter_map;
    Function *instrFunc_Pred;
    std::unordered_set<std::string > applicationFunctions;
    //Girish
    LoopInfo *LI;
    map<Loop*,set<string>> mDtreeInsertset;

    embedDtree() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override;
    bool doInitialization(Module &M) override;
    void insertDtree(CallSite &CS);
    void recursiveIfElseTree(Instruction *insBef, string node, CallSite &CS);
    void set_needRDFofBBs(Function &F) ;
    void trackReachingDefs(Instruction *argI  );
    void instrumentRDF(SmallVector<BasicBlock *, 32> RDFBlocks,unsigned  int callSite_num, int argument_num, string varName);

    void computeControlDependence(PostDominatorTree &PDT, SmallPtrSet<BasicBlock *, 16> getRDFofBlocks, SmallVector<BasicBlock *, 32> &RDFBlocks) ;

    void getAnalysisUsage(AnalysisUsage &AU) const override {
      AU.addRequired<PostDominatorTreeWrapperPass>();
      AU.addRequired<DominatorTreeWrapperPass>();
      AU.addRequired<LoopInfoWrapperPass>();
      //AU.setPreservesCFG();
      //AU.addPreserved<GlobalsAAWrapperPass>();
    }
  };
}

void embedDtree::computeControlDependence(PostDominatorTree &PDT, SmallPtrSet<BasicBlock *, 16> getRDFofBlocks, SmallVector<BasicBlock *, 32> &RDFBlocks) {
      ReverseIDFCalculator RDF(PDT) ;
      RDF.setDefiningBlocks(getRDFofBlocks );
      RDF.calculate(RDFBlocks);
    }
char embedDtree::ID = 0;
int embedDtree::basicBlockCoutner = 0;
int embedDtree::rdfIDCounter=0;
int embedDtree::funcIDCounter=0;
unsigned int embedDtree::CIcounter  = 0;
static RegisterPass<embedDtree> X("embedDtree", "embed desicion tree in IR");

/*
static void registerSkeletonPass(const PassManagerBuilder &,
                                 legacy::PassManagerBase &PM) {
    PM.add(new embedDtree());
}
//
//static RegisterStandardPasses RegisterMyPass(PassManagerBuilder:: EP_OptimizerLast,
                      // EP_LoopOptimizerEnd, //EP_EnabledOnOptLevel0,//EP_OptimizerLast,// EP_EarlyAsPossible,
 //                      registerSkeletonPass);
*/

/**********************************************
 * @param M
 * @return
 ***********************************************/
bool embedDtree::doInitialization(Module &M)
{
  bool bRet = false;

  C = &M.getContext();
  thisModule = &M;

  for(auto &f : M){
    if (f.hasName() && !f.isDeclaration()) {
      errs()<<"Skipping function::"<<f.getName();
      if (f.hasName())
      applicationFunctions.insert(f.getName());
    }

    if(f.isDeclaration()) continue;
    for(auto &b : f){
      for(auto &i : b){
        CallInst *CI = dyn_cast<CallInst>(&i);
        if(CI){
              Function *calledf = CI->getCalledFunction();
              if (calledf == NULL) continue;
              if (calledf->isIntrinsic()) continue;
          if (mCallSiteID.find(CI) == mCallSiteID.end()) {
            mCallSiteID[&i] = ++CIcounter;
            //errs()<<"\n id:"<<CIcounter<<"callsite ::"<<*CI;
          }
        }
      }
    }

  }

  //get all parameter mapping
  ifstream fgetParams;
  fgetParams.open("paramList");
  if(fgetParams.is_open()){
    string line;
    int pindex = 0;
    getline(fgetParams,line);{ // first line of a csv file
      stringstream ss(line);
      while(ss.good()){
        string param;
        getline(ss,param,',');
        if(param.empty() == false)
          paramMap[pindex++] = param;
      }
    }
  }
  fgetParams.close();

  int counter_strdID = 0;
  ifstream func2idmap;
  func2idmap.open("string2id_map.csv");
  if(func2idmap.is_open()){
    string line;
    while(getline(func2idmap,line)) {
      if (line.empty()){
        continue;
      }
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
      global_id2string_map[id] = funcName;
      global_string2id_map[funcName] = id;
      stringId_2counter_map[id] = ++counter_strdID;
      errs()<<"\nid:"<<id<<"="<<funcName<<":counter="<<counter_strdID;

      while (getline(ss, token, ',')){
        errs()<<"\n token for func2id:"<<token<<":";
      }
    }
  }
  func2idmap.close();

  func2idmap.open("funcName_id_map.csv");
  if(func2idmap.is_open()){
    string line;
    while(getline(func2idmap,line)) {
      if (line.empty()){
        continue;
      }
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
      funcName_id_map[funcName] = id;
      errs()<<"\nid:"<<id<<"="<<funcName<<":counter="<<counter_strdID;

      while (getline(ss, token, ',')){
        errs()<<"\n token for func2id:"<<token<<":";
      }
    }
  }
  func2idmap.close();
  //get the decision tree
  ifstream fdtree;
  fdtree.open("decisionTree");
  if(fdtree.is_open()){
    string line;
    while(getline(fdtree,line)) {
      stringstream ss(line);
      ss>>ws;//skip whitespaces
      string node;
      getline(ss, node, '-');
      string iforleaf;
      getline(ss, iforleaf, '.');
      if (iforleaf.compare("if") == 0) { //else a leaf node
        string cond;
        getline(ss, cond, ':');
        string true_branch;
        getline(ss, true_branch, ';');
        string elseDummy;
        getline(ss, elseDummy, ':');
        string false_branch;
        getline(ss, false_branch, ';');
        dtree[node] = {cond, true_branch, false_branch};
      } else {
        auto pos = line.find("=");
        errs()<<"\n leaf::"<<line;
        string leafPred = line.substr(pos+1,string::npos);
        errs()<<"leaf::"<<leafPred;

        dtree[node] = {leafPred};
      }
    }
  }
  fdtree.close();

  for (auto a: dtree) {
    errs()<<"\n() "<<a.first;
    for (auto b: a.second ) {
      errs()<<"  str:"<<b;
    }
  }
  //      ifstream map_file;
  //      map_file.open ("rdf_id_map.txt");
  //      if(map_file.is_open()){
  //        string line;
  //        while(getline(fdtree,line)) {
  //          stringstream ss(line);
  //          }
  //        }
  //      
  //          map_file.close();
  //      map_file.open ("funcName_id_map.txt");
  //      for (auto map_it : funcName_id_map) {
  //        map_file<<map_it.first <<" "<<map_it.second<<"\n";
  //        //myfile << "Writing this to a file.\n";
  //      }
  //      map_file.close();
  return  bRet;
}
/*********************************************************
 * @param pindex
 * @param numArgs
 * @return
 *  A helper function for recurisveIfElseTree, checks if the
 *  argument in the decision node is valid for this function
 *********************************************************/
bool isPindexValid(int pindex, int numArgs)
{
  if (pindex == 0 || pindex == 1) return true;
  if (numArgs == 0 ) return false;
  if (pindex >=MIN_RDF_P && pindex <=MAX_RDF_P)  
  {  if ( (numArgs - (pindex - MIN_RDF_P )) <= 0) return false;}
  else if (pindex >=MIN_F_ARG_P  && pindex <=MAX_F_ARG_P)
  {  if ( (numArgs - (pindex - MIN_F_ARG_P )) <= 0) return false;}
  return true;


  // if(pindex >= MIN_RDF_P ){
  //   if(pindex < MIN_F_ARG_P){//The pindex corresponds to RDF
  //     if (numArgs == 0) return  false;
  //     else if( (numArgs - (pindex - MIN_RDF_P )) < 0)//corresponding RDF is greater than numArgs
  //       bRet = false;
  //   }else{//The pindex corresponds to func arg
  //     if((numArgs - (pindex - MIN_F_ARG_P )) < 0)//func arg exceeds number of Args in this function
  //       bRet = false;
  //   }
  // }
  // return  bRet;
}
/*************************************************************************************
 * @param insBef
 * @param node
 * @param CS
 *  recursively  inserts If else clasuses of the decision tree
 ************************************************************************************/
void embedDtree::recursiveIfElseTree(Instruction *insBef, string node, CallSite &CS)
{
  //errs()<<"embed node:"<<node;
  int numArgs = CS.getNumArgOperands();
  vector<string> predicate = dtree[node];
  if(predicate.size() > 2) {//if else node
    string cond = predicate[0];
    string if_node = predicate[1];
    string else_node = predicate[2];
    stringstream streamCond(cond);
    int pindex;
    float fvalue; 
    int intVal;
    string dummyparam, dummypred;
    streamCond >> dummyparam >> pindex >> dummypred >> fvalue;
    intVal = fvalue;
    DEBUG(dbgs()<<"\n dummyparam:"<<dummyparam<<" pindex" <<pindex<<" dummypred"<<dummypred<<" fvalue"<<fvalue); 
    //errs()<<"\n ins::"<<*insBef;

    Value *vfArg = nullptr ; 
    int const_value = -1;
    bool is_const = false;
    if(isPindexValid(pindex, numArgs)) {
      //check if its a func arg or rdf or other
      if (pindex == 1) {//entryfunc
        string fname = CS.getCalledFunction()->getName();
        int funcID ;
        if (funcName_id_map.find(fname) != funcName_id_map.end()){
            funcID = funcName_id_map[fname] ;
        } else if (global_string2id_map.find(fname) != global_string2id_map.end()) {
          funcID = global_string2id_map[fname];
        } else {
          funcName_id_map[fname] = ++funcIDCounter;
          funcID = funcIDCounter;
        }
        //errs()<<"\n funcname:"<<fname<<":id:"<<funcID;
        vfArg = ConstantInt::get( int32Ty, funcID);
        const_value = funcID;
        is_const = true;
      }else if (pindex == 0) {
        int callSiteID = mCallSiteID[CS.getInstruction()];
        //errs()<<"\n callsite:"<<CS.getInstruction()<<":id:"<<callSiteID;
        vfArg = ConstantInt::get( int32Ty, callSiteID);
        const_value = callSiteID;
        is_const = true;
      } else if (pindex <MIN_F_ARG_P) {//TODO; this case not handled
        unsigned argnum = pindex - MIN_RDF_P;
        Value *vArg = CS.getArgOperand(argnum);
        if (Instruction *argI = dyn_cast<Instruction>(vArg) ) {
          if (argI_to_rdfFlag_map.find(argI) != argI_to_rdfFlag_map.end() ){
            AllocaInst *ptr = argI_to_rdfFlag_map[argI];
            if (isa<PHINode>(argI))
              argI = &*argI->getParent()->getFirstInsertionPt();
            IRBuilder<> builder(argI );
            vfArg = builder.CreateLoad(ptr);
          }
        }
        //vfArg = nullptr;
      } else if (pindex < MAX_F_ARG_P) {
        //a function arg
        unsigned argnum = pindex - MIN_F_ARG_P;
        Value *funcArg = CS.getArgOperand(argnum);
        Value *castedArg = nullptr;
        IRBuilder<> builder(insBef);
        if (funcArg != NULL ) {
          if (funcArg->getType()->isFloatTy()||funcArg->getType()->isDoubleTy() )
            castedArg = builder.CreateFPToSI(funcArg, int32Ty );
          else if ( funcArg->getType()->isIntegerTy() )
            castedArg = builder.CreateIntCast(funcArg , int32Ty, true)  ;
          vfArg = castedArg;
        }
      }

    }else{//if invalid then just -1
      Value *p1 = ConstantInt::get(int32Ty, -1);
      vfArg = p1;
      const_value = -1;
      is_const = true;
    }
    if (vfArg != nullptr) {
      bool compare_const_result ; 
      if (is_const) {
        compare_const_result = const_value <= fvalue;
        DEBUG(dbgs()<<"\n Const p2::"<<fvalue<<"\n args:"<<const_value);
        if (compare_const_result) {

          recursiveIfElseTree(insBef, if_node, CS);
        } else {
          recursiveIfElseTree(insBef, else_node, CS);
        }
      } else {
        Value *p2 = ConstantInt::get(int32Ty, fvalue);
        DEBUG(dbgs()<<"\n p2::"<<*p2<<"\n args:"<<*vfArg);
        ICmpInst *fb = new  ICmpInst(insBef, ICmpInst::ICMP_SLE, vfArg, p2);
        TerminatorInst *ThenTerm, *ElseTerm;
        SplitBlockAndInsertIfThenElse(fb, insBef, &ThenTerm, &ElseTerm);
        //if clause
        recursiveIfElseTree(ThenTerm, if_node, CS);
        //else clause
        recursiveIfElseTree(ElseTerm, else_node, CS);
      }
    }
  }else { //else leaf node, do prediction here
    IRBuilder<> builder(insBef);
    //AllocaInst *ptr = builder.CreateAlloca(int32Ty);
    Value *cnst = ConstantInt::get(int32Ty, rand()%100 );
    DEBUG(dbgs()<<"\n storing value to mem:"<<*cnst);
    unsigned predId = stoi(dtree[node].at(0));
    //errs()<<"\n first id prediction::"<<predId;
    //errs()<<"\n prediction="<<global_id2string_map[predId];
    predId = stringId_2counter_map[predId];
    //errs()<<"\n prediction::"<<predId;
    //builder.CreateStore(cnst , ptr) ;
    std::vector<Value *> ArgsV;
    Type *int32Ty = IntegerType::getInt32Ty(*C );
    ArgsV.push_back(llvm::ConstantInt::get(int32Ty, predId,false));
    std::string funcTrace_str;
    if ( global_id2string_map.find(predId) != global_id2string_map.end() ) {
      funcTrace_str = global_id2string_map[predId];
    }
    if (funcTrace_str.compare(";") != 0) {

      llvm::Type *ArgTypes_callsite[] = {int32Ty  }	;
      string custom_instr_func_name = "pbarua_custom_instr_callsite_0";//"printf"
      custom_instr_func_name = "blankit_predict";
      instrFunc_Pred =dyn_cast<Function>( thisModule->getOrInsertFunction(custom_instr_func_name.c_str() ,
            FunctionType::get(int32Ty ,
              ArgTypes_callsite,false /*this is var arg func type*/) 
            ) );
      builder.CreateCall(instrFunc_Pred ,ArgsV  );
      //Type *ptr_i8 = PointerType::get(Type::getInt8Ty(thisModule->getContext() ), 0);
      //llvm::Type *ArgTypes[] = { ptr_i8 }	;
      //std::string custom_instrstr_func_name("pbarua_custom_instr_fun");//"printf"
      //custom_instrstr_func_name = "blankit_predict";
      //Function *printfFunction0 =dyn_cast<Function>( thisModule->getOrInsertFunction(custom_instrstr_func_name.c_str() ,
      //      FunctionType::get(IntegerType::getInt32Ty(thisModule->getContext()),
      //        ArgTypes,true /*this is var arg func type*/) 
      //      ) );
      //Constant *Init =ConstantDataArray::getString(thisModule->getContext(),funcTrace_str.c_str());
      //// Create the global variable and record it in the module
      //// The GV will be renamed to a unique name if needed.
      //GlobalVariable *GV = new GlobalVariable(Init->getType(), true,
      //    GlobalValue::InternalLinkage, Init,
      //    "trstr");
      //thisModule->getGlobalList().push_back(GV);
      //GlobalVariable *string_printf = GV;
      //PointerType *pty = dyn_cast<PointerType>(string_printf->getType());
      ////DEBUG(dbgs()<<"printf func:: "<<*userInstrumentFunc <<"::zero_i64:: "<<*zero_i64<<":: string  "<<*string_printf<<":: pty gelem type:: "<<* pty->getElementType() );
      //Constant* zero_i64 = Constant::getNullValue(IntegerType::getInt64Ty(thisModule->getContext()));
      //ArrayRef< Value * > indices = {zero_i64,zero_i64};
      //GetElementPtrInst *gep_printf_string = GetElementPtrInst::Create( pty->getElementType(), string_printf,indices );
      ////DEBUG(dbgs()<<"printf string load:: " <<*gep_printf_string);
      //Value *printf_str_gep = builder.Insert(gep_printf_string,"tmp_blockstride_compute" );
      ////DEBUG(dbgs()<<"printf string load:: " <<*printf_str_gep);
      //std::vector<Value *> ArgsV2;
      //ArgsV2.push_back(printf_str_gep);  
      //errs()<<"called  func "<<*instrFunc_Pred;
      //errs()<<"printf func:: "<<*printfFunction0 <<"::zero_i64:: "<<*zero_i64<<":: string  "<<*string_printf<<":: pty gelem type:: "<<* pty->getElementType() ;
      //builder.CreateCall(printfFunction0 ,ArgsV2  );
    }
  }
}
/*********************************************
 * @param CS
 * @return
 ********************************************/
void embedDtree::insertDtree(CallSite &CS)
{
  do{

    //insert the dtree before this callsite
  string firstNode;
  for (auto a: dtree) {
      if (a.first.find("node") != string::npos ) {
        firstNode = a.first;
      break;
      }
    }
    DEBUG(dbgs()<<"\n  recursively insert with :"<<firstNode);
    if (true ) {
      recursiveIfElseTree(CS.getInstruction(),firstNode, CS);
      return;
    }
    //get the instruction to insert before 
    //if the callsite is within a loop check if it can be hoiste
    //outside the loop and hoist it
    Instruction *insBef = CS.getInstruction();
    if(LI  && LI->getLoopFor(insBef->getParent())) {//check if this insBef is within loop
      Loop *L = LI->getLoopFor(insBef->getParent());
      //check if the instruction dominates loop latch
      //a latch is a block that has a backedge to the header
      BasicBlock *latchBB = L->getLoopLatch();//returns if there is a single loop latch
      BasicBlock *preHeaderBB = L->getLoopPreheader();
      if(latchBB && preHeaderBB){
        DominatorTree &DT = getAnalysis<DominatorTreeWrapperPass>().getDomTree();
        if(DT.dominates(insBef,latchBB)){
          //the instruction dominates latchBB, so the tree can be inserted outside the loop
          string calledFunName = CS.getCalledFunction()->getName();
          if(mDtreeInsertset.count(L) >  0){
            if(mDtreeInsertset[L].count(calledFunName) > 0)
              break;//the prediction for one instance of this function is already inserted
            else{
              insBef = preHeaderBB->getTerminator();
              mDtreeInsertset[L].insert(calledFunName);
            }
          }else{
            //insert for a new called func
            insBef = preHeaderBB->getTerminator();
            set<string> insertset;
            insertset.insert(calledFunName);
            mDtreeInsertset[L] = insertset;
          }
        }
      }
    }
    recursiveIfElseTree(insBef,firstNode, CS);

  }while(false);
}

void embedDtree::trackReachingDefs(Instruction *argI  ){
  if (argI == NULL) return ;
  DEBUG(dbgs() << "\n Tracking def ::"<<*argI << '\n');
  if (PHINode *phiDef= dyn_cast<PHINode>(argI ) ) {
    DEBUG(dbgs() << "pathPredInstrument: gpt phi node "<<*phiDef);
    for (auto defBB = phiDef->block_begin() , bbEnd = phiDef->block_end(); defBB != bbEnd ; defBB++){
      BasicBlock *bb = *defBB;
      needRDFofBBs.insert(bb);
    }
    return;
  }
  for (int i= 0, num = argI->getNumOperands(); i < num ; i++) {
    Instruction *opI =dyn_cast<Instruction>(  argI->getOperand(i));
    if (opI == NULL) continue;
    trackReachingDefs(opI);
  }
}

void embedDtree::instrumentRDF(SmallVector<BasicBlock *, 32> RDFBlocks,unsigned  int callSite_num, int argument_num, string varName){
  if (RDFBlocks.size() == 0) return;
  std::set<BasicBlock* > basicBlock_done_set;
  Instruction *i = &*RDFBlocks[0]->getFirstInsertionPt();
  IRBuilder<> builder(i);
  AllocaInst *rdfFlag = builder.CreateAlloca(Type::getInt32Ty(*C));//, nullptr, varName);
  //errs()<<"\n alloca::"<<*rdfFlag;
  for (auto *BB : RDFBlocks) {
    DEBUG(dbgs() << "live control in: " <<* BB->getTerminator() << '\n');
    if (basicBlock_done_set.find(BB) != basicBlock_done_set.end()) continue; //if already instrumented goto next
    basicBlock_done_set.insert(BB);
    std::ostringstream parent_s;
    unsigned int bbID = basicBlock_id_map[BB];
    TerminatorInst *ti = BB->getTerminator();
    //unsigned int succ_num = 0;
    for (BasicBlock *succBB : ti->successors()){
      //worklistBBs.push_back(succBB);
      {
        Instruction * instrInstrumentBefore = &*succBB->getFirstInsertionPt();
        IRBuilder<> builder(instrInstrumentBefore);
        std::ostringstream s;
        unsigned int succ_bbID = basicBlock_id_map[succBB];
        s<<bbID<<"_"<<succ_bbID ;
        //s<<"_"<<succ_num++ <<":";
        std::string rdf_s = s.str();
        if (rdf_id_map.find(rdf_s ) == rdf_id_map.end()) {
          rdf_id_map[rdf_s] = ++rdfIDCounter;
        }
        int rdf_id = rdf_id_map[rdf_s];
        Constant *p_init = ConstantInt::get(Type::getInt32Ty(*C), rdf_id);
         builder.CreateStore(p_init, rdfFlag);
        //errs()<<"store::"<<*st;
          //insertFcall(succBB->getFirstInsertionPtOrLifetime(), print_msg, bbID, NULL );
      }
    }
  }

}

extern std::unordered_set<std::string> libcFuncArray ;
/***************************************
 * @param M
 * @return
 ***************************************/
bool embedDtree::runOnFunction(Function &F)
{
  bool bRet = false;
  BasicBlock *firstBB= nullptr;
  if(F.isDeclaration())
    return false;

  errs()<<"Probing function "<<F.getName();

  LI  = &getAnalysis<LoopInfoWrapperPass>().getLoopInfo();

    int32Ty = Type::getInt32Ty(*C );
  vector<Instruction*> callInsts;
  for(auto &b : F){
    if (firstBB == nullptr) 
      firstBB = &b;
    for(auto &i : b){
      CallSite CS(&i);
      if(CS){
        Function *cf = CS.getCalledFunction();
        if(cf && cf->isIntrinsic() == false && cf->hasName()){
          //if (applicationFunctions.find(cf) != applicationFunctions.end() ){
          //  errs()<<"\tsk pping::"<<cf->getName();
          //  continue;
          //}
          errs()<<"\t Pushing::"<<cf->getName();
          errs().flush();
          callInsts.push_back(&i);
          bRet = true;
        }
      }
    }

  }
  errs()<<"\n ";
  for(auto &CI : callInsts){
    CallSite CS(CI);
    Function *fi = CS.getCalledFunction();
    // if (libcFuncArray.find(std::string(F->getName())) == libcFuncArray.end()) {
    //   //errs()<<"\t Skipping Instrumenting Function :"<<F->getName();
    //   continue;
    // }
    errs()<<"\t Got :"<<fi->getName();
    if (applicationFunctions.find(fi->getName()) != applicationFunctions.end() ){
      errs()<<"\t skipping::";
      continue;
    }
    errs()<<"\n Instrumenting Function :"<<fi->getName();

    insertDtree(CS);
  }

  for(auto &CI : callInsts){
    CallInst *callIns = dyn_cast<CallInst> (CI);
    if (callIns == NULL) continue;
    Function *calledF=    callIns->getCalledFunction();
    if (calledF == NULL) continue;
    if (calledF->isIntrinsic()|| !calledF->hasName() ) continue;
    //    if (libcFuncArray.find(calledF->getName()) == libcFuncArray.end()) 
    if (callIns->getDereferenceableBytes(0)) continue;
    if (dyn_cast<InvokeInst>(callIns ) ) continue;
    //errs()<<"\n call instruction::"<<*callIns;

    unsigned int numArgs = callIns->getNumArgOperands();
numArgs = 0;
    for (unsigned int i = 0 ; i < numArgs ; i++) {
      Value * argV = callIns->getArgOperand(i);
      if (Instruction *argI = dyn_cast<Instruction>(argV) ) {
        IRBuilder<> builder(&*firstBB->getFirstInsertionPt());
        AllocaInst *alloca = builder.CreateAlloca(int32Ty);
        argI_to_rdfFlag_map[argI] = alloca;

        set<BasicBlock*> temp;
        if (instruction_to_rdf_succ_map.find(argI) != instruction_to_rdf_succ_map.end())
          temp = instruction_to_rdf_succ_map[argI];
        needRDFofBBs.clear();
        trackReachingDefs(argI );
        SmallVector<BasicBlock *, 32> RDFBlocks;
        PostDominatorTree &PDT = getAnalysis<PostDominatorTreeWrapperPass>().getPostDomTree();
        computeControlDependence(PDT, needRDFofBBs, RDFBlocks);
        for (auto BB : RDFBlocks ){
          TerminatorInst *ti = BB->getTerminator();
          //unsigned int succ_num = 0;
          for (BasicBlock *succBB : ti->successors()){
            temp.insert(succBB);
            std::string rdf_s = BB->getName().str() +"_"+succBB->getName().str();
            if (rdf_id_map.find(rdf_s) == rdf_id_map.end())
              rdf_id_map[rdf_s] = ++rdfIDCounter;
            IRBuilder<> builder(&*succBB->getFirstInsertionPt());
            Value *rdfID_v = ConstantInt::get(int32Ty, rdf_id_map[rdf_s]); 
            builder.CreateStore(rdfID_v, alloca);
          }
        }
        instruction_to_rdf_succ_map[argI] = temp;
      }
    }
  }
  //if (0)
  //for (auto &argI : instruction_to_rdf_succ_map ) {
  //  //errs()<<"\n  arg instru:"<<&argI;
  //  for (auto &bb : argI.second ) {
  //    errs()<<"\t rdf bb:"<<bb->getFirstNonPHI();
  //  }
  //}
  callInsts.clear();
  return bRet;

}

