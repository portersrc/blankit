#include <string>
#include <fstream>
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <stack>
#include <map>
#include <set>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <unordered_set>
#define windowSize 5
using namespace std;
typedef std::map<std::string,unsigned int >::iterator iterOf_str2int_map;
  map<int, std::string> global_id2string_map;
  map<int, std::unordered_set<std::string>> global_id2Setstring_map;
class callsite_trace_context {
  std::vector<unsigned int> callSite_seq;
  std::vector<std::set<unsigned int> > rdf_seq;
  std::vector<iterOf_str2int_map> calledFuncs_seq;
  std::vector<std::vector< float >> paramsPassed_seq;
  std::map<unsigned int, std::string > callsite2fName_map;
  std::map<unsigned int, unsigned > callsite2fID_map;
  std::map<unsigned int, std::set<unsigned int> > callsite2rdfSet_map;
  std::map<std::string,unsigned int > calledFuncs_str2id_map;
  std::vector<unsigned> call_stack;
    map< std::string, int> global_string2id_map;

  unsigned int currentCallsite_index;
  std::set<std::string> currentCalledFunc_set;
  bool callsiteid_isSet;
  std::map<std::string, unsigned> global_string2integer_map;
  std::set<unsigned> strIds_set;
  unsigned getId4String(std::string str, bool sequence=false) {
    if (global_string2integer_map.find(str) == global_string2integer_map.end() ){
      unsigned id;
      do {
        id = (std::rand()%1000)+calledFuncs_str2id_map.size();//will make sure uniq ids
      }while (strIds_set.find(id) != strIds_set.end() );
      global_string2integer_map[str] = id;
      strIds_set.insert(id);
    }
    unsigned ret_id = global_string2integer_map[str];
    if (sequence) 
      ret_id = std::distance(global_string2integer_map.begin(), global_string2integer_map.find(str));
    return ret_id   ;
  }

  std::string flattenSet (std::set<std::string> setOfStr) {
    std::string concatStr(";");
    for (std::string str:setOfStr) {
      concatStr = concatStr + str+ ";";
    }
    return concatStr;
  }
  std::string flattenSet (std::vector<unsigned > setOfStr) {
    std::string concatStr(";");
    for (unsigned str:setOfStr) {
      concatStr = concatStr + std::to_string(str)+ ";";
    }
    return concatStr;
  }
  std::string flattenSet (std::vector<float> setOfStr, char delimiter=';') {
    std::string concatStr;
    int i=0;
    for (i = 0; i < setOfStr.size() ; i++) {
      float str = setOfStr.at(i);
      if (i==0)
        concatStr =std::to_string(str) ;
      else 
        concatStr =  concatStr + delimiter+std::to_string(str);
    }
    for ( ; i < 5 ; i++) {
      if (i==0)
        concatStr ="0";
      else 
        concatStr = concatStr + delimiter+ "0";
    }

    return concatStr;
  }
  std::string flattenSet (std::set<unsigned int> setOfStr) {
    std::string concatStr(";");
    for (unsigned int str:setOfStr) {
      concatStr = concatStr + std::to_string(str)+ ";";
    }
    return concatStr;
  }
  std::set<unsigned int> getRDF4callsite(int callsiteID ){ 
    std::set<unsigned int> t_rdfSet;
    if (callsite2rdfSet_map.find(callsiteID) != callsite2rdfSet_map.end() )
      t_rdfSet = callsite2rdfSet_map[callsiteID];
    return t_rdfSet;
  }
  void clearRDF4callsite(int callsiteID ){//forget rdf set, once call site is visited, 
    if (callsite2rdfSet_map.find(callsiteID) != callsite2rdfSet_map.end() )
      callsite2rdfSet_map[callsiteID].clear();
  }
  public:
  void print() {
    std::ofstream traceFile; 
    traceFile.open("training_calltrace.csv");
    traceFile<<"calledFuncs_strid,callsite_id,function,rdf_set, "<<
      "params_passed0,params_passed1,params_passed2,params_passed3,params_passed4";
    for (int i= 0; i < callSite_seq.size() ; i++ ){
      //unsigned it = std::distance(callsite2fName_map.find(callSite_seq.at(i)),callsite2fName_map.begin() );
      traceFile<<"\n";
      //traceFile<<"\""<<getId4String(calledFuncs_seq.at(i)->first)<<"\",";
      if (i < calledFuncs_seq.size())
        traceFile<<calledFuncs_seq.at(i)->second<<",";
      else 
        traceFile<<"0,";
      traceFile<<callSite_seq.at(i)<<",";
      traceFile<<(callsite2fID_map[callSite_seq.at(i)])<<",";
      //traceFile<<getId4String(callsite2fName_map[callSite_seq.at(i)])<<",";
      traceFile<<getId4String(flattenSet(rdf_seq.at(i)))<<",";
      traceFile<<flattenSet(paramsPassed_seq.at(i),',');
    }
    traceFile.close();
    traceFile.open("calltrace.csv");
    traceFile<<"callsite_id,function,rdf_set, calledFuncs_str,"<<
      "calledFuncs_id, params_passed";
    for (int i= 0; i < callSite_seq.size() ; i++ ){
      traceFile<<"\n";
      traceFile<<callSite_seq.at(i)<<",";
      traceFile<<(callsite2fID_map[callSite_seq.at(i)])<<",";
      //traceFile<<callsite2fName_map[callSite_seq.at(i)]<<",";
      traceFile<<flattenSet(rdf_seq.at(i))<<",";
      if (i < calledFuncs_seq.size()) {
        traceFile<<calledFuncs_seq.at(i)->first<<",";
      traceFile<<calledFuncs_seq.at(i)->second<<",";}
      else {
        traceFile<<",,";}
      traceFile<<flattenSet(paramsPassed_seq.at(i));
    }
    traceFile.close();
    traceFile.open("string2id_map.csv");
    for (auto it : calledFuncs_str2id_map) {
      traceFile<<"\n";
      traceFile<<it.first<<","<<it.second;
    }
    for (auto it : global_string2integer_map) {
      traceFile<<"\n";
      traceFile<<it.first<<","<<it.second;
    }
    traceFile.close();
  }
  bool insideCall;
  callsite_trace_context () {
    insideCall = false;//set only after callsite and enter lib
    callsiteid_isSet = false;//set after callsite to funcname is set
    currentCallsite_index = -1;//Very important, since incremented before use
  int counter_strdID = 0;
  ifstream func2idmap;
  func2idmap.open("string2id_map.csv");
  if(func2idmap.is_open()){
    string line;
    unsigned lineNumber = 0;
    while(getline(func2idmap,line)) {
      if (line.empty()){
        continue;
      }
      lineNumber++;
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
      id = lineNumber;
      global_id2string_map[id] = funcName;
      calledFuncs_str2id_map[funcName] = id;
      cout<<"\nid:"<<id<<"="<<funcName<<":counter="<<counter_strdID;
      stringstream listFunc(funcName);
      std::string eachFunc;
      std::unordered_set<std::string> setOfpredictionsStr;
      while (getline(listFunc, eachFunc, ';')){
        //std::cout<<"\n each func::"<<eachFunc;
        setOfpredictionsStr.insert(eachFunc);
      }
      global_id2Setstring_map[id] = setOfpredictionsStr;

      while (getline(ss, token, ',')){
        cout<<"\n token for func2id:"<<token<<":";
      }
    }
  }
  func2idmap.close();

  }
  void recordRDF(int rdfID, int callsiteID ){//pbarua_custom_instr_fun_RDF
    if (!insideCall) return;
    callsite2rdfSet_map[callsiteID].insert(rdfID);
  }
  void undoLastCallSite(){
  }
  void recordCallsite(unsigned int callsiteID, unsigned calledFuncID ){//on every callsite
    if (insideCall) {
      //handle inconsistent stack
      clearCurrentCalledFuncs();
    }
    currentCallsite_index++;
    currentCalledFunc_set.clear();
    callSite_seq.push_back(callsiteID);
    rdf_seq.push_back(getRDF4callsite(callsiteID));
    clearRDF4callsite(callsiteID);
    paramsPassed_seq.push_back(std::vector<float >() );
    callsite2fID_map[callsiteID] = calledFuncID;
    callsiteid_isSet = false;
    insideCall = true;
    //std::cout<<"\n pushed to stack:"<<callsiteID<<" calledfuncid:"<<calledFuncID;
    call_stack.push_back(callsiteID);
  }
  void addFuncParams(float val ){//foreach func param
    if (!insideCall) return;
    paramsPassed_seq.at(currentCallsite_index).push_back(val);
  }
  void addCalledFuncs(std::string calledFuncName ){ // on every called
    if (!insideCall) return;
    if (!callsiteid_isSet) {
      //std::cout<<"\n called f name::"<<calledFuncName
       // <<"current call side it"<<currentCallsite_index ;
      callsite2fName_map[callSite_seq.at(currentCallsite_index )] = calledFuncName;
    }
    callsiteid_isSet = true;
    currentCalledFunc_set.insert(calledFuncName);
  }
  void clearCurrentCalledFuncs(){
    if (!insideCall) return;
    currentCalledFunc_set.clear();
    finalizeCalledFuncs( callSite_seq.at(currentCallsite_index) );

  }
  void finalizeCalledFuncs(int callsiteID ){
    if (!insideCall) return;
    if (callSite_seq.at(currentCallsite_index) != callsiteID) {
      std::cout<<"\n Error return call:"<<callsiteID<<" does not match call site id:"<<callSite_seq.at(currentCallsite_index);
    }
    std::string calledFuncs_str = flattenSet(currentCalledFunc_set);
    if (calledFuncs_str2id_map.find(calledFuncs_str) == calledFuncs_str2id_map.end()) {
      unsigned int calledFunc_set_uniqID = calledFuncs_str2id_map.size();
      calledFuncs_str2id_map[calledFuncs_str] = calledFunc_set_uniqID;
    }
    calledFuncs_seq.push_back(calledFuncs_str2id_map.find(calledFuncs_str));
    call_stack.pop_back();
    //std::cout<<"\n call return::"<<callsiteID<<std::endl;
    //std::cout<<"\n call stack after return::"<<flattenSet(call_stack);
    insideCall = false;
  }
};
typedef  std::vector<std::string> vectorOfString;
vectorOfString split(const std::string& s, char delimiter)
{
  std::vector<std::string> tokens_vector;
  std::string token;
  std::istringstream tokenStream(s);
  while (std::getline(tokenStream, token, delimiter))
  {
    if (token.empty()) continue;
    //std::cout<<"\n token::"<<token;
      tokens_vector.push_back(token);
  }
  return tokens_vector;
}
void parse_instream(std::istream& infile){
  std::string line;
  const std::string called_str = "Called";
  const std::string libc_str = "libc.so.6";
  const std::string rdfInstrument_str = "pbarua_custom_instr_fun_RDF";
  const std::string callsiteInstrument_str = "pbarua_custom_instr_callsite";
  const std::string returnInstrument_str = "pbarua_custom_instr_return_callsite";
  const std::string blankitInstrument_str = "blankit_predict ";
  const std::string startRecording = "Called main ";
  const std::string stopRecording = "Called exit";
  std::string application_lb_str ;
  bool embedVerifyMode=false;
  std::vector<std::string> lineBuffer;
      vectorOfString mostRecent_callsite_args_str;
  callsite_trace_context call_trace_obj;
  std::unordered_set<std::string> predictedFunctions;
  std::unordered_set<std::string> actualCalledFunctions;
  unsigned missedCount=0,totalLibcCount=0;
  unsigned totalCallSites=0 , actualGreaterThanPredicted=0, actualLessthanPredicted = 0,actualEqualthanPredicted= 0 ;
  std::string topLevelCalledFunc = "";
  while (std::getline(infile, line))
  {   
    if (line.length() == 0) continue;
    replace(line.begin(), line.end(),')',' ' );
    replace(line.begin(), line.end(),'(',' ' );

    vectorOfString tokens = split(line,' ' );
    if (!embedVerifyMode && line.find(startRecording )!= std::string::npos ){
      embedVerifyMode = true;
      application_lb_str = tokens.at(2);
    }
    if (actualCalledFunctions.size() == 0 && line.find(called_str) != std::string::npos) {
      topLevelCalledFunc = tokens.at(1);
    }
    if (line.find(blankitInstrument_str )!= std::string::npos && embedVerifyMode){
      //std::cout<<"\n blankit"<<"\t line:"<<line;
      embedVerifyMode = true;
      if (tokens.size() > 2) {
        std::string numStr = tokens.at(2);
        //cout<<"\n tokens 1:"<<numStr<<":\n";
        int seqID = std::stoi(numStr);
        //std::cout<<"\n sequence::"<<seqID;
        if (actualCalledFunctions.size() > predictedFunctions.size()){//for previous call site
          actualGreaterThanPredicted++;
        }
        else if (actualCalledFunctions.size() < predictedFunctions.size()){//for previous call site
          actualLessthanPredicted++;
        }
        else if (actualCalledFunctions.size() == predictedFunctions.size()){//for previous call site
          actualEqualthanPredicted++;
        }
        if (global_id2string_map.find(seqID) != global_id2string_map.end()) {
          //std::cout<<"\n blankit predict is ::"<< global_id2string_map[seqID];
          predictedFunctions = global_id2Setstring_map[seqID];
          totalCallSites++;
          actualCalledFunctions.clear();
          //for (auto func: predictedFunctions) {
          //  std::cout<<"\n predicted f::"<<func;
          //}
        }
      }
    }  else if (   line.find(application_lb_str ) == std::string::npos &&  
        line.find(called_str) != std::string::npos) {
      //line.find(libc_str ) != std::string::npos && embedVerifyMode &&
      std::string calledF = tokens.at(1);
      actualCalledFunctions.insert(calledF);
      //std::cout<<"\n called : "<<calledF<<" ";
      if (predictedFunctions.find(calledF) == predictedFunctions.end()){
        std::cout<<"==== Top Level:"<<topLevelCalledFunc<<"=>";
        std::cout<<"==== Missed:"<<calledF<<":";
        missedCount++;
      }
      //std::cout<<"\n HIT";
      totalLibcCount++;
    } else if (line.find(stopRecording) != std::string::npos) {
      break;
    }
  }
  std::cout<<"\n ~~~ Missed/Total="<<missedCount<<"/"<<totalLibcCount<<", Accuracy="<<((float)  (totalLibcCount-missedCount )/totalLibcCount)<<" ~~~ \n\n";
  std::cout<<"\n ~~~ Overpredicted/Total="<<actualGreaterThanPredicted<<"/"<<totalCallSites<<" ~~~ \n\n";
  std::cout<<"\n ~~~ Underpredicted/Total="<<actualLessthanPredicted<<"/"<<totalCallSites<<" ~~~ \n\n";
  std::cout<<"\n ~~~ Equalpredicted/Total="<<actualEqualthanPredicted<<"/"<<totalCallSites<<" ~~~ \n\n";
}
/*
 *
  void recordRDF(int rdfID, int callsiteID );// called on pbarua_custom_instr_fun_RDF
  void recordCallsite(unsigned int callsiteID );//on every callsite 
  void addFuncParams(float val );//foreach func param
  void addCalledFuncs(std::string calledFuncName ); // on every called
  void finalizeCalledFuncs(int callsiteID ); // on every return 
  */
int main(int argc, char *argv[] ) { 
  //std::string filename(argv[1]);
  //std::ifstream infile(filename.c_str());
  
  try {
  parse_instream(std::cin);
  }catch (...) {}

  return 0; 
}
//      //std::cout<<line.substr(4);
//      windowBB[index_window ] = line.substr(4);
//      index_window = (index_window +1) % windowSize;
//      //for (int k=0; k< windowSize ; k++) 
//      //	    std::cout<<"[["<<windowBB[k];
//    } else if (line.find(" Func:") != std::string::npos) {
//      std::cout<<" "<<line.substr(6)<<"=";
//
//      int i_win = index_window; 
//      do {
//
//        //std::cout<<"i="<<i_win;
//        std::cout<<"~"<<windowBB[i_win ];
//
//        i_win = i_win == 0? (windowSize -1) :( i_win -1); 
//      } while (i_win != index_window);
//      std::cout<<std::endl;
//    } else {
//      //std::cout<<"no";
//      continue;
//    }	       
//    // process pair (a,b)
//    //  
//  }   
//  return 0;
//}

