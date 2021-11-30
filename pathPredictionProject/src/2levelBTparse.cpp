#include <string>
#include <fstream>
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <array>
#include <map>
#include <set>
#include <forward_list>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string.h>
#define windowSize 10
#define DEBUG 0
using namespace std;
//std::string definedFuncs[] = {"main","compare" };
//std::set<std::string> userDefinedFunctions(definedFuncs, definedFuncs+2);
class path_prefix_window_class {
  public:
  std::string pathPrefix_str[windowSize];
  std::string funcArg_values_str[windowSize];
  static std::map<std::string, int> str_to_int_map;
  static int str_id ; 
  path_prefix_window_class(){
    for (int i =0 ; i < windowSize ; i++) {
      pathPrefix_str[i] = "-1";
    funcArg_values_str[i] = "-1";
    }
  }
  path_prefix_window_class(const path_prefix_window_class &copy ){
    if (DEBUG) std::cout<<"\n copy constructor" ;
    for (int i =0 ; i < windowSize ; i++)  {
      pathPrefix_str[i] = copy.pathPrefix_str[i];
      funcArg_values_str[i] = copy.funcArg_values_str[i];
    }
  }
  void clear_func_args() {
    //forget the arg values, because call site execution over, get ready to track next values
    for (int i =0 ; i < windowSize ; i++) {
      funcArg_values_str[i] = "-1";
    }
  }
  std::string get(bool commaDelim=false){
    std::string ret_str; 
    std::string delim = commaDelim ? ",":"_";
    for (int i=0 ; i < windowSize ; i++) {
      std::string s = pathPrefix_str[i];
      if (s == "-1")
        ret_str = ret_str +delim+"-1" ;
      else {
        if (str_to_int_map.find(s) == str_to_int_map.end() )
          str_to_int_map[s] = std::stoi (s ); //++str_id;
        ret_str = ret_str +delim+std::to_string(str_to_int_map[s] ) ;
      }
    }

    for (int i=0 ; i < windowSize ; i++) {
      std::string s = funcArg_values_str[i];
        ret_str = ret_str +delim+s ;
      
    }
    return ret_str;
  }
  std::string concatStrArray(std::string strArray[windowSize] ){
    std::string ret_str; 
    for (int i=0 ; i < windowSize ; i++) {
      ret_str = ret_str +"_"+ strArray[i];
    }
    return ret_str;
  }
  void insertfuncArg_value( std::string value_str, int argument_num){
    funcArg_values_str[argument_num-1] = value_str;
    return;
    //first left shift
    for (int i=0;i<windowSize-1;i++) {
     funcArg_values_str[i]=funcArg_values_str[i +1];
    }
    //then push back the latest bb
    funcArg_values_str[windowSize-1] = value_str;
  }
  void insert( std::string bb_str, int argument_num){
    pathPrefix_str[argument_num-1] = bb_str;
    return;
    for (int i=0;i<windowSize;i++) {
     if ( pathPrefix_str[i]== bb_str) return;
    }
    //first left shift
    for (int i=0;i<windowSize-1;i++) {
      pathPrefix_str[i]=pathPrefix_str[i +1];
    }
    //then push back the latest bb
    pathPrefix_str[windowSize-1] = bb_str;
  }
};
int path_prefix_window_class ::str_id = 10;
std::map<std::string, int> path_prefix_window_class::str_to_int_map;
std::map<int, path_prefix_window_class  > callsite_to_pathprefix_map;
class callStack_state {
    bool state_out_libc, state_in_libc, state_entering_libc, state_exiting_libc;
    std::string stackTopFunc, stackSecondFunc;
    std::string enteringFunc, enteringFunc_only_func_name;
    std::set<std::string> currentCallChain_set;
    std::map<std::set<std::string> , int > funcSet_to_count_map;
    std::map<std::string, int> entryFunc_to_count_map;
    std::map<std::string, int> entryFunc_to_ID_map;
    std::multimap<std::string, std::set<std::string> > enteringFunc_to_callChain_map;
    int enteringFunc_static_ID, enteringFunc_callsite_ID;

    void set_state_out_libc(){
        state_entering_libc = false;
        state_in_libc = false;
        state_out_libc = true;
        state_exiting_libc = false;
        if (DEBUG) std::cout<<"\n out_libc";
    }
    void set_state_entering_libc(std::string pathPrefix_str){
        state_entering_libc = true ;
        state_in_libc = false;
        state_out_libc = false;
        state_exiting_libc = false;
        currentCallChain_set.clear();
        enteringFunc = stackTopFunc+pathPrefix_str;
        enteringFunc_only_func_name = stackTopFunc;
        //if (entryFunc_to_ID_map.find(enteringFunc_only_func_name) == entryFunc_to_ID_map.end())
        //  entryFunc_to_ID_map[enteringFunc_only_func_name] = enteringFunc_static_ID;
       // if( entryFunc_to_ID_map[enteringFunc_only_func_name] != enteringFunc_static_ID ){
       //   std::cout<<"\n Error incosistent funcname id got::"<<enteringFunc_only_func_name
       //     <<" prev::"<<entryFunc_to_ID_map[enteringFunc_only_func_name]
       //     <<" latest:: " << enteringFunc_static_ID;
       //   exit(0);
       // }
        if (entryFunc_to_count_map.find(enteringFunc) == entryFunc_to_count_map.end())
            entryFunc_to_count_map[enteringFunc] = 1;
        else 
            entryFunc_to_count_map[enteringFunc]++;

        if (DEBUG) std::cout<<"\n entering_libc";
        set_state_in_libc();
    }
    void set_state_in_libc(){
        state_entering_libc = false;
        state_in_libc = true ;
        state_out_libc = false;
        state_exiting_libc = false;
        currentCallChain_set.insert(stackTopFunc );
        if (DEBUG) std::cout<<"\nin_libc";
    }
    void set_state_exiting_libc(bool top_in_libc ){
      path_prefix_window_class pathPrefix_obj = callsite_to_pathprefix_map[enteringFunc_callsite_ID];
      std::string pathPrefix_str(pathPrefix_obj.get());
        state_entering_libc = false;
        state_in_libc = false;
        state_out_libc = false;
        state_exiting_libc = true ;
        if (DEBUG) std::cout<<"\n exiting_libc";
        bool entryFunc_CallChain_alreadyFound = false;
        auto entry_to_set = enteringFunc_to_callChain_map.equal_range(enteringFunc );
        for ( auto it_set = entry_to_set.first ; it_set != entry_to_set.second ; it_set++){
            std::set<std::string > callchain_s = it_set->second; 
            if (callchain_s == currentCallChain_set )  {
                entryFunc_CallChain_alreadyFound = true;
                break;
            }
        }
        std::cout<<"\n%"<<getSetConcat();
        std::cout<<","<<enteringFunc_static_ID ;
        std::cout<<","<<enteringFunc_callsite_ID;
        std::cout<<" "<<pathPrefix_obj.get(true)<<"\n";
        if (DEBUG) std::cout<<"\n pathPrefix_str:"<<pathPrefix_str;
        if (DEBUG) std::cout<<" Entering_func:"<<enteringFunc;
        if (DEBUG) std::cout<<": callChain:"<<getSetConcat();
        if (!entryFunc_CallChain_alreadyFound)
            enteringFunc_to_callChain_map.emplace(enteringFunc,  currentCallChain_set);
        if (funcSet_to_count_map.find(currentCallChain_set) == funcSet_to_count_map.end())
            funcSet_to_count_map[currentCallChain_set] =1;
        else
            funcSet_to_count_map[currentCallChain_set] ++;
        if (top_in_libc )
            set_state_entering_libc(pathPrefix_str);
        else 
            set_state_out_libc();

        callsite_to_pathprefix_map[enteringFunc_callsite_ID].clear_func_args();
    }
    std::string getSetConcat(){
      std::string retStr;
      for (auto f : currentCallChain_set ) {
        retStr = retStr + ";"+f;
      }
      return retStr;
    }
    public:
    void out_of_last_callchain() {
      if (state_in_libc)
      set_state_exiting_libc(false);
    }
    void set_func_id ( int id) {
      enteringFunc_static_ID = id;
    }
    void set_callsite_id ( int id) {
      enteringFunc_callsite_ID = id;
      if (DEBUG) std::cout<<"\n setting  callsiteid::"<<enteringFunc_callsite_ID;
    }
    void print(){
        for (auto f_count : entryFunc_to_count_map) {
            std::string entry_f = f_count.first;
            std::cout<<"\nentry_func;"<<f_count.first<<";Called_times;"<<f_count.second;
            auto entry_to_set = enteringFunc_to_callChain_map.equal_range(entry_f );
            for ( auto it_set = entry_to_set.first ; it_set != entry_to_set.second ; it_set++){
                std::set<std::string > callchain_s = it_set->second; 
                std::cout<<"\n call chain count="<<funcSet_to_count_map[callchain_s];
            }
        }

        for (auto callChain_set : funcSet_to_count_map ) {
            std::cout<<"\n{ ";
            for (auto f: callChain_set.first ) 
                std::cout<<f<<";";
            std::cout<<" } = "<<callChain_set.second;
        }
        for (auto f_to_set : enteringFunc_to_callChain_map ) {
            std::cout<<"\n entry func::"<<f_to_set.first;
            std::cout<<"\n{ ";
            for (auto f : f_to_set.second ){
                std::cout<<f<<";";
            }
            std::cout<<" } ";
        }
    }
    callStack_state(){
        state_entering_libc = false;
        state_in_libc = false;
        state_out_libc = true;
        state_exiting_libc = false;
         stackTopFunc = "";
         std::cout<<"%CallChain,entryFunc,call_site_id";
         for (int i=0;i<windowSize;i++) {
          std::cout<<",RDF"<<i;
         }
         for (int i=0;i<windowSize;i++) {
          std::cout<<",F_Arg_"<<i;
         }
    }
    void record_stack(std::string top_func, bool top_func_isLibc, 
        std::string second_func , bool second_func_isLibc ) {
      path_prefix_window_class pathPrefix_obj = callsite_to_pathprefix_map[enteringFunc_callsite_ID];
        if (DEBUG) std::cout<<"\n record stack pathpr::"<<enteringFunc_callsite_ID<<"="<<callsite_to_pathprefix_map[enteringFunc_callsite_ID].get(true);
      if (DEBUG) std::cout<<"\n "<<top_func<<"=  "<<top_func_isLibc<<
        "; "<<second_func<<"="<<second_func_isLibc;
      std::string pathPrefix_str = pathPrefix_obj.get();
      stackTopFunc = top_func;
      stackSecondFunc = second_func;
      //if (top_func_isLibc ) 
      if (state_out_libc) {
        if (top_func_isLibc ) 
          set_state_entering_libc(pathPrefix_str );

      }else if (state_in_libc) {
        set_state_in_libc();
       // if (second_func_isLibc ) 
       //   set_state_in_libc();
       // else 
       //   set_state_exiting_libc(top_func_isLibc, pathPrefix_obj);
      }
    }
};
std::vector<std::string> appFiles ;
//grep -o ' \w*\.c\b' Makefile
// grep -o ' \w*\.c\b' Makefile | tr '\n' ',' | sed 's/ / "/g' | sed 's/c,/c" ,/g'
struct {
    std::string fName;
    bool libcFunc;
}funcNameType;
void get_appFiles()
{
  char cmd[] = "grep -o \' \\w*\\.c\\b' Makefile |sed \'s/ //\' |sort|uniq";
  std::array<char, 128> buffer;
  std::string result;
  std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
  if (!pipe) throw std::runtime_error("popen() failed!");
  while (!feof(pipe.get())) {
    if (fgets(buffer.data(), 128, pipe.get()) != nullptr) {
      std::string s = buffer.data();
      s = s.substr(0,s.length()-2);
      appFiles.push_back( s);
      if (DEBUG) std::cout<<"buffer::"<<s<<";";
    }
  }
}
class  func_arg_info {
    set<int > possible_rdf_set;
    int func_arg_value;
    int arg_num;
  public:
    void print() {
      std::cout<<"\n arg::"<<arg_num;
      for (auto x: possible_rdf_set ){
        std::cout<<"\t possiblerdf:"<<x;
      }
    }
    func_arg_info(){
      func_arg_value = -1;
      arg_num = -1;
    }
    func_arg_info( const func_arg_info &copy){
      func_arg_value = copy.func_arg_value;
      arg_num = copy.arg_num;
      possible_rdf_set = copy.possible_rdf_set;
      //std::cout<<"\n copy construc::";
    }
    bool notEmpty() {
      if (possible_rdf_set.empty()) return false;
      return true ;
    }
    bool contains_rdf(int rdfID){
      if (possible_rdf_set.find(rdfID) == possible_rdf_set.end()) return false;
      return true;
    }
    int get_func_arg(){
      return arg_num;
    }
    void set_func_arg(int a){
      arg_num = a;
    }
    void insert_rdf(int rdfID) {
      possible_rdf_set.insert(rdfID);
      //std::cout<<"\n inserting:"<<rdfID;
      //print();
    }
};
class callsite_info {
  public:
    int funcID;
    map<int, func_arg_info> argument_to_info_map;
    callsite_info() {
      funcID = -1;
      //argument_to_info_map 
    }
    callsite_info(const callsite_info &copy) {
      argument_to_info_map  = copy.argument_to_info_map;
      funcID = copy.funcID;
    }
    void set_info_for_arg_num(int argnum , func_arg_info fai ){
      if (fai.notEmpty())
      argument_to_info_map[argnum] = fai;
      //std::cout<<"\n printing::";
      //argument_to_info_map[argnum].print();
      //std::cout<<"\n Done    ::";
    }
    int  callsite_dependson_rdf(int rdfID) {
      for (auto &arg_info : argument_to_info_map ) {
        if  (arg_info.second.contains_rdf(rdfID)) {
          if (DEBUG) cout<<"\n contains rdf arg:"<<arg_info.first;
          return arg_info.first; //return the argument number
        }
      }
          if (DEBUG) cout<<"\n does not depend:";
      return -1;
    }
    void print() {
      std::cout<<"\n funcid:"<<funcID;
      for (auto x: argument_to_info_map) {
        x.second.print();
      }
    }
};
map<int, callsite_info> callsite_info_map;
void check_rdf_for_every_callsite(string rdfID_s) {
  int rdfID = stoi(rdfID_s);
  for (auto callsite_info : callsite_info_map) {
    int callsite_id = callsite_info.first;
    int arg_dependson = callsite_info.second.callsite_dependson_rdf(rdfID);
    if (arg_dependson >0 ){
          if (DEBUG) std::cout<<"\n callsite:"<<callsite_id<<" depends on :"<<rdfID;
          path_prefix_window_class temp ; 
          if (callsite_to_pathprefix_map.find(callsite_id) != callsite_to_pathprefix_map.end()) {
            temp = callsite_to_pathprefix_map[callsite_id] ; 
          }
          temp.insert((rdfID_s), arg_dependson);
          callsite_to_pathprefix_map[callsite_id]  =temp  ; 
          
    }
  }
}
std::string getTokenAfterTag(std::string line , std::string tag, std::string delimiter=":", std::string delim_start="" ){
  auto tagPos = line.find(tag);//"\0A#_CALL_instrument:funcID:12:callsiteID:41:_max_args_:4:_2_total_rdf_count:0:_4_total_rdf_count:0:\00"
  if (tagPos == std::string::npos) return "";
  std::string afterTag = line.substr(tagPos+tag.size());//:funcID:12:callsiteID:41:_max_args_:4:_2_total_rdf_count:0:_4_total_rdf_count:0:\00"
  if (delim_start.empty())
    delim_start = delimiter;
  auto delimPos = afterTag.find(delim_start);
  afterTag = afterTag.substr(delimPos+1);//funcID:12:callsiteID:41:_max_args_:4:_2_total_rdf_count:0:_4_total_rdf_count:0:\00"
  //std::cout<<"\n delim finding in :"<<afterTag<<" tag:"<<delimiter;
  std::string token = afterTag.substr(0,afterTag.find(delimiter));//from 0 till size
  return token;
}
  callStack_state csh_obj;
  void parseCallStatement(std::string line, bool firstTime= false ){
    //CALL_instrument:funcID:15:callsiteID:55:_max_args_:5:_1_total_rdf_count:0:_2_total_rdf_count:2:_2_rdf_num_1:8:_2_rdf_num_2:8:_3_total_rdf_count:0:_4_      total_rdf_count:0:_5_total_rdf_count:2:_5_rdf_num_1:8:_5_rdf_num_2:8:
    std::string tag1="#_CALL_instrument";
    std::string afterTag = line.substr(line.find(tag1)+tag1.size());
    //:funcID:15:callsiteID:55:_max_args_:5:_1_total_rdf_count:0:_2_total_rdf_count:2:_2_rdf_num_1:8:_2_rdf_num_2:8:_3_total_rdf_count:0:_4_      total_rdf_count:0:_5_total_rdf_count:2:_5_rdf_num_1:8:_5_rdf_num_2:8:
    std::string funcID_s = getTokenAfterTag(afterTag, "funcID", ":" );
    int funcID = stoi(funcID_s);
    std::string callsiteID_s = getTokenAfterTag(afterTag, "callsiteID", ":" );
    int callsiteID = stoi(callsiteID_s);
    std::string maxArgs_s = getTokenAfterTag(afterTag, "_max_args_", ":");
    cout<<"\n funcid;"<<funcID<<"; callsite ;"<<callsiteID<<"; maxargs ; "<<maxArgs_s;
    if (firstTime) {
      if (callsite_info_map.find(callsiteID) != callsite_info_map.end()) return;
      callsite_info_map[callsiteID].funcID = funcID;
    } else {
      if (callsite_to_pathprefix_map.find(callsiteID) != callsite_to_pathprefix_map.end()) {
        std::cout<<";  have seen path prefix"<<callsite_to_pathprefix_map[callsiteID].get(true);
      }
      //remember the call id, use this number to map to call chain
      csh_obj.set_func_id(funcID);
      csh_obj.set_callsite_id(callsiteID  );
    }
    int maxArgs = std::stoi(maxArgs_s);
    for (int i=1;i<=maxArgs;i++){
      std::string tag_rdf = "_"+std::to_string(i)+"_total_rdf_count";
      std::string rdf_count_s = getTokenAfterTag(afterTag, tag_rdf);
      if (rdf_count_s == "") continue;
      std::cout<<"\n count:"<<rdf_count_s;
      int rdf_count = std::stoi(rdf_count_s);
      func_arg_info save_rdf_for_arg;
      for (int j=1;j<=rdf_count;j++ ){
        std::string tag_rdf_num = "_"+to_string(i)+"_rdf_num_" +to_string(j);
        string rdfid_s = getTokenAfterTag(afterTag, tag_rdf_num);
        if (rdfid_s == "") continue;
        std::cout<<"\t rdf:"<<rdfid_s;
        int rdfID = stoi(rdfid_s);

        if (firstTime) {
          save_rdf_for_arg.set_func_arg(i);
          save_rdf_for_arg.insert_rdf(rdfID);
        }
      }

      if (firstTime) {
        save_rdf_for_arg.print();
        if (save_rdf_for_arg.notEmpty())
          callsite_info_map[callsiteID].set_info_for_arg_num(i, save_rdf_for_arg);
      } else {
        std::string argTag = "arg"+to_string(i);
        std::string arg_value;
        if (line.find(argTag) == string::npos) continue;
        if (i != maxArgs) 
          arg_value= getTokenAfterTag(line, argTag,",","=");
        else 
          arg_value= getTokenAfterTag(line, argTag,")","=");
        if (arg_value.find(")") != string::npos) 
          arg_value= getTokenAfterTag(line, argTag,")","=");
        if (arg_value == "") continue;
        //std::cout<<"\n line ::"<<line<<"\n arg found::"<<arg_value<<" tag::"<<argTag;
        path_prefix_window_class temp ; 
        if (callsite_to_pathprefix_map.find(callsiteID) != callsite_to_pathprefix_map.end()) {
          temp = callsite_to_pathprefix_map[callsiteID] ; 
        }
        temp.insertfuncArg_value(arg_value,i );
        callsite_to_pathprefix_map[callsiteID]  =temp  ; 
      }
    }
  }
int main(int argc, char *argv[] ) { 
  std::string filename(argv[1]);
  std::ifstream infile(filename.c_str());
  std::string line;
  std::string windowBB[windowSize];
  std::array <std::string, 20> callStack;
  std::map<std::set<std::string>, int> function_trace_count;
  //int index_window=0;
  std::string delimiter = " " ;
  std::string stack_num, func_name , top_func, second_func; 
  path_prefix_window_class pathPrefix_obj;
  bool top_func_isLibc=false, second_func_isLibc=false;
  get_appFiles();
//  const char rdf_instr_function[] = "pbarua_custom_instr_set_rdf";
  //const char rdfID_tag[] = "rdfID=";
  //const char value_func_tag[] = "#Value_Func_site:";
  //const char value_arg[] = "arg1=";
  const char funcCall_instr_function[] = "pbarua_custom_instr_fun";
  //const char rdfID_tag[] = "rdfID=";
  const char rdfID_tag[] = "#_control_dep_BB";
  const char func_tag[] = "#_CALL_instrument";
  while (std::getline(infile, line))
  {   
    if (line.find("#") != 0) continue;
    auto f_pos = line.find(funcCall_instr_function);
    if ( f_pos != std::string::npos) {
      //std::cout<<std::endl<<line<<std::endl;
      if (std::string::npos != line.find("pbarua_custom_instr_fun")){//value found, parse it
        if (line.find(func_tag )!= std::string::npos)
          parseCallStatement(line, true);
      }
    }
  }
  for (auto x: callsite_info_map) {
    std::cout<<"\n site:"<<x.first;
    x.second.print();
  }
  infile.close();

  //return 0;
  std::ifstream infile_2(filename.c_str());
  while (std::getline(infile_2, line))
  {   
    if (line.find("#") != 0) continue;
    auto f_pos = line.find(funcCall_instr_function);
    if ( f_pos != std::string::npos) {
      //std::cout<<std::endl<<line<<std::endl;
      csh_obj.out_of_last_callchain();

        if (line.find(func_tag )!= std::string::npos)
          parseCallStatement(line);
        f_pos = line.find(rdfID_tag);
        if (f_pos != std::string::npos) {
          std::string rdfBB = getTokenAfterTag(line, rdfID_tag, ":");
          std::cout<<"\n RDF BB  : "<<rdfBB   ;
          check_rdf_for_every_callsite(rdfBB);
        }
        continue;
    }//my instrumentation parsing done, 
    
    line = line.substr(1);//remove the hash
    size_t delim_pos1 = line.find(delimiter);
    std::string line_part2 = line.substr(delim_pos1+2);
    size_t delim_pos2 = line_part2.find(delimiter);

    stack_num = line.substr(0,delim_pos1 );  
    func_name = line_part2.substr(0, delim_pos2 ); 
    //std::cout<<"line_part2::"<<line_part2<<" delim pos2;"<<delim_pos2;
    size_t pos_in = line.find(" in ");
    if (pos_in != std::string::npos) {
      std::string after_in = line.substr(pos_in+4);
      size_t delim_pos3 = after_in.find(delimiter);
      //std::cout<<"\n after_in::"<<after_in<<" delim_pos3: "<<delim_pos3;
      func_name = after_in.substr(0, delim_pos3 ); 
    }
    bool notLibCFunc=false;
    if (func_name.find( "instrument_static_lib") != std::string::npos) {
      notLibCFunc=true;
    }else {
      for (auto appFilename : appFiles) {
        if (line.find(" at " +appFilename) != std::string::npos) {
          if (DEBUG) std::cout<<"\n found ::"<<appFilename<<" in line::"<<line;
          notLibCFunc = true;//assumption is that every func, from user application has the filename 
          break;
        }
      }
    }
    //std::cout<<"\n stacknum:"<<stack_num;
    int stackNum = std::stoi(stack_num);
    if (DEBUG) std::cout<<"\n Stack pos:"<<stackNum<<" func::"<<func_name << " is libc?"<<!notLibCFunc;
    if (stackNum == 0) {
      if (!top_func.empty())
        csh_obj.record_stack(top_func,top_func_isLibc, second_func, second_func_isLibc);
      top_func = func_name;
      top_func_isLibc = !notLibCFunc;
    } else {
      second_func = func_name;
      second_func_isLibc = !notLibCFunc;
    }
    //csh_obj.gotNewLine(stackNum, func_name);
    //If pushing, then append to call chain
    //else if popped then pushed then a different chain
  }   
  csh_obj.print();
  return 0;
}
//  FILE *fp;
//  char path[1035];
//
//  /* Open the command for reading. */
//  printf("grep -o \' \\w*\\.c\b\' Makefile | tr \'\\n\' \',\' | sed \'s/ / \"/g\' | sed \'s/c,/c\" ,/g\'");
//  //grep -o ' \w*\.c\b' Makefile
//  char command[] = "grep -o \' \\w*\\.c\\b' Makefile |sed \'s/ //\' ";
//  fp = popen(command, "r");
//  if (fp == NULL) {
//    printf("Failed to run command\n" );
//    exit(1);
//  }
//
//  /* Read the output a line at a time - output it. */
//  while (fgets(path, sizeof(path)-1, fp) != NULL) {
//    appFiles.insert(std::string(path));
//    printf("%s", path);
//  }
//
//  /* close */
//  pclose(fp);
//
//}

//class callStackHandler {
//    std::forward_list<std::string> callStack_list;
//    std::forward_list<std::string>::iterator lastPos_it;
//    bool setupPhase ; 
//    int num_pops;
//    std::string currentFuncName, lastFuncName;
//   void issue_command(bool only_push=true, std::string func=""){
//       if (only_push) {
//           std::cout<<"\n PUSH :"<<currentFuncName;
//       } else {
//           std::cout<<"\n POP :"<<func;
//       }
//   }
//    public:
//    callStackHandler () {
//        setupPhase = true;
//        num_pops= 0;
//    }
//   void  gotNewLine(int number, std::string funcName) {
//        if (number == 0) {
//            callStack_list.emplace_front(funcName);
//            lastPos_it = callStack_list.begin();
//            currentFuncName = funcName;
//
//        }else if (number == 1) {
//            if (!setupPhase) {
//                auto tmp_it = lastPos_it;//record the iterator before incrementing it, since forward list, cannot get the previous it otherwise
//
//                ++lastPos_it;
//                if (lastPos_it == callStack_list.end()) {
//                    std::cout<<"\n1. PANIC call stack ilformed";
//                    exit(0);
//                }
//                lastFuncName = *lastPos_it;
//                if (lastFuncName != funcName) 
//                {
//                    num_pops=0;
//                    do {
//                        //std::cout<<" deleting ::"<<*lastPos_it;
//                        //we need to delete the node "lastPos_it", but need the previous node iterator to do that, so this statement will always use the tmp_it that actually points to the head.
//                        issue_command(false, *lastPos_it);
//                        lastPos_it = callStack_list.erase_after(tmp_it);
//                        //std::cout<<" now pointing to ::"<<*lastPos_it;
//                        num_pops++;
//                    } while ((*lastPos_it) != funcName );
//                    //std::cout<<"\n after deleting::";
//                    //print();
//                }
//            }else if (setupPhase ) {
//                setupPhase = false;
//                callStack_list.emplace_after(lastPos_it, funcName);
//                ++lastPos_it;
//            }
//            issue_command();
//        } else {
//            ++lastPos_it;
//            if (lastPos_it == callStack_list.end()) {
//                std::cout<<"\n2. PANIC call stack ilformed";
//                exit(0);
//            }
//            std::string stackFuncName = *lastPos_it;
//            if (stackFuncName == funcName)  {
//                std::cout<<"\n All's Good";
//            } else {
//                    lastPos_it = callStack_list.erase_after(lastPos_it);
//
//                std::cout<<"\n3. PANIC call stack ilformed";
//                exit(0);
//            }
//        }
//        //std::cout<<"\n current pos:"<<*lastPos_it;
//    }
//   void print() {
//       std::cout<<"\n";
//       for (auto it : callStack_list) {
//           std::cout<<"\t func:"<<it;
//       }
//       std::cout<<"\n";
//
//   }
//};
//std::string appFiles[] = { "ansi2knr.c" , "jcapimin.c" , "jcapistd.c" , "jccoefct.c" , "jccolor.c" , "jcdctmgr.c" , "jchuff.c" , "jcinit.c" , "jcmainct.c" , "jcmarker.c" , "jcmaster.c" , "jcomapi.c" , "jcparam.c" , "jcphuff.c" , "jcprepct.c" , "jcsample.c" , "jctrans.c" , "jdapimin.c" , "jdapistd.c" , "jdatadst.c" , "jdatasrc.c" , "jdcoefct.c" , "jdcolor.c" , "jddctmgr.c" , "jdhuff.c" , "jdinput.c" , "jdmainct.c" , "jdmarker.c" , "jdmaster.c" , "jdmerge.c" , "jdphuff.c" , "jdpostct.c" , "jdsample.c" , "jdtrans.c" , "jerror.c" , "jfdctflt.c" , "jfdctfst.c" , "jfdctint.c" , "jidctflt.c" , "jidctfst.c" , "jidctint.c" , "jidctred.c" , "jquant1.c" , "jquant2.c" , "jutils.c" , "jmemmgr.c" , "jmemansi.c" , "jmemname.c" , "jmemnobs.c" , "jmemdos.c" , "jmemmac.c" , "cjpeg.c" , "djpeg.c" , "jpegtran.c" , "cdjpeg.c" , "rdcolmap.c" , "rdswitch.c" , "rdjpgcom.c" , "wrjpgcom.c" , "rdppm.c" , "wrppm.c" , "rdgif.c" , "wrgif.c" , "rdtarga.c" , "wrtarga.c" , "rdbmp.c" , "wrbmp.c" , "rdrle.c" , "wrrle.c" , "example.c" , "ckconfig.c" , "ansi2knr.c" ,  "ansi2knr.c" , "ansi2knr.c" , "jcapimin.c" , "jcapistd.c" , "jccoefct.c" , "jccolor.c" , "jcdctmgr.c" , "jchuff.c" , "jcinit.c" , "jcmainct.c" , "jcmarker.c" , "jcmaster.c" , "jcomapi.c" , "jcparam.c" , "jcphuff.c" , "jcprepct.c" , "jcsample.c" , "jctrans.c" , "jdapimin.c" , "jdapistd.c" , "jdatadst.c" , "jdatasrc.c" , "jdcoefct.c" , "jdcolor.c" , "jddctmgr.c" , "jdhuff.c" , "jdinput.c" , "jdmainct.c" , "jdmarker.c" , "jdmaster.c" , "jdmerge.c" , "jdphuff.c" , "jdpostct.c" , "jdsample.c" , "jdtrans.c" , "jerror.c" , "jfdctflt.c" , "jfdctfst.c" , "jfdctint.c" , "jidctflt.c" , "jidctfst.c" , "jidctint.c" , "jidctred.c" , "jquant1.c" , "jquant2.c" , "jutils.c" , "jmemmgr.c" , "jmemansi.c" , "jmemname.c" , "jmemnobs.c" , "jmemdos.c" , "jmemmac.c" , "cjpeg.c" , "djpeg.c" , "jpegtran.c" , "cdjpeg.c" , "rdcolmap.c" , "rdswitch.c" , "rdjpgcom.c" , "wrjpgcom.c" , "rdppm.c" , "wrppm.c" , "rdgif.c" , "wrgif.c" , "rdtarga.c" , "wrtarga.c" , "rdbmp.c" , "wrbmp.c" , "rdrle.c" , "wrrle.c"};
// [] = {"fftmisc.c" , "fourierf.c" , "main.c" , "fftmisc.c" , "fourierf.c" , "fftmisc.c" , "fftmisc.c" , "fourierf.c" , "fourierf.c" , "main.c" , "main.c"};//  {"crc_32.c"};
// "fftmisc.c" , "fourierf.c" , "main.c" , "fftmisc.c" , "fourierf.c" , "fftmisc.c" , "fftmisc.c" , "fourierf.c" , "fourierf.c" , "main.c" , "main.c" 
//  {"adpcm.c" , "timing.c" , "rawcaudio.c" , "suncaudio.c" , "sgicaudio.c" , "sgidaudio.c" , "adpcm.c"};// {"basicmath_small.c" , "rad2deg.c" , "cubic.c" , "isqrt.c" , "basicmath_large.c" , "rad2deg.c" , "cubic.c" , "isqrt.c"};
//std::string appFiles[] = {"z02.c" , "externs.h",  "z03.c" , "z04.c" , "z05.c" , "z06.c" , "z07.c" , "z08.c" , "z09.c" , "z10.c" , "z11.c" , "z12.c" , "z13.c" , "z14.c" , "z15.c" , "z16.c" , "z17.c" , "z18.c" , "z19.c" , "z20.c" , "z21.c" , "z22.c" , "z23.c" , "z26.c" , "z28.c" , "z29.c" , "z30.c" , "z31.c" , "z32.c" , "z33.c" , "z34.c" , "z35.c" , "z36.c" , "z37.c" , "z38.c" , "z39.c" , "z40.c" , "z41.c" , "z42.c" , "z43.c" , "z44.c" , "z45.c" , "z46.c" , "z47.c" , "z48.c" , "z49.c" , "z50.c" , "z51.c" , "z01.c" };
//std::string appFiles[] = { "brhist.c" , "id3tag.c" , "lame.c" , "newmdct.c" , "parse.c" , "VbrTag.c" , "version.c" , "gtkanal.c" , "gpkplotting.c" };
//
//          //path_prefix_window_class temp ; 
//          //if (callsite_to_pathprefix_map.find(callsite_id) != callsite_to_pathprefix_map.end()) {
//          //  temp = callsite_to_pathprefix_map[callsite_id] ; 
//          //}
//          //temp.insert(rdfBB, argument_num);
//          //callsite_to_pathprefix_map[callsite_id]  =temp  ; 
//        //}
//        //TODO MAP values
//      //  auto site_pos =line.find(value_func_tag); 
//      //  std::string afterTag = line.substr(site_pos+strlen(value_func_tag));
//      //  site_pos = afterTag.find(":");
//      //  std::string callsite_id_s = afterTag.substr(0,site_pos );
//      //  std::string argument_num_s = afterTag.substr(site_pos+1);
//      //  //std::cout<<"\n temp arg:"<<argument_num_s;
//      //  argument_num_s = argument_num_s.substr(0,argument_num_s.find(":"));
//      //  //argument_num_s = afterTag.substr(0,argument_num_s.find(":"));
//      //  std::cout<<"\n value site :"<<callsite_id_s<<"arg::"<<argument_num_s;
//      //  int argument_num = std::stoi(argument_num_s);
//      //  site_pos = afterTag.find(value_arg);
//      //  std::string func_arg_s = afterTag.substr(site_pos+strlen(value_arg),
//      //      afterTag.find(")")-site_pos-strlen(value_arg) );
//      //  std::cout<<"\n value arg sent::"<<func_arg_s;
//      //    int callsite_id = std::stoi( callsite_id_s );
//      //    path_prefix_window_class temp ; 
//      //    if (callsite_to_pathprefix_map.find(callsite_id) != callsite_to_pathprefix_map.end()) {
//      //      temp = callsite_to_pathprefix_map[callsite_id] ; 
//      //    }
//      //    temp.insertfuncArg_value(func_arg_s, argument_num );
//      //    callsite_to_pathprefix_map[callsite_id]  =temp  ; 
//      } 
//      //else {
//       // f_pos = line.find(rdfID_tag);
//       // if (f_pos != std::string::npos) {
//       //   std::string afterTag = line.substr(line.find (":",  f_pos+2)+1 );
//       //   f_pos = afterTag.find(":");
//       //   std::string rdfBB     = afterTag.substr(0,f_pos);
//       //   afterTag = afterTag.substr(f_pos+1);
//       //   f_pos = afterTag.find(":");
//       //   std::string callsite_id_s = afterTag.substr(0,f_pos );
//       //   std::string argument_num_s  = afterTag.substr(f_pos+1); 
//       //   argument_num_s = argument_num_s.substr(0,argument_num_s.find(":"));
//       //   int argument_num = std::stoi(argument_num_s);
//       //   std::cout<<"\n RDF BB  : "<<rdfBB   ;
//       //   int callsite_id = std::stoi( callsite_id_s );
//       //   std::cout<<"\n Callsite: "<<callsite_id<<"argu:"<<argument_num;
//       //   path_prefix_window_class temp ; 
//       //   if (callsite_to_pathprefix_map.find(callsite_id) != callsite_to_pathprefix_map.end()) {
//       //     temp = callsite_to_pathprefix_map[callsite_id] ; 
//       //   }
//       //   temp.insert(rdfBB, argument_num);
//       //   callsite_to_pathprefix_map[callsite_id]  =temp  ; 
//       //   std::cout<<"\n pathpr::"<<callsite_id<<"="<<callsite_to_pathprefix_map[callsite_id].get(true);
//       //// } else if ((f_pos = line.find(func_tag )) != std::string::npos ) {
//        //  std::string afterTag = line.substr(line.find (":",  f_pos+2)+1 );
//        //  f_pos = afterTag.find(":");
//        //  std::string calledFname  = afterTag.substr(0,f_pos);
//        //  afterTag = afterTag.substr(f_pos+1);
//        //  //43:23:
//        //  std::string callsite_id_s = afterTag.substr(0, afterTag.find(":"));
//        //  std::string calledFnumber = afterTag.substr(callsite_id_s.size()+1, afterTag.find(":", callsite_id_s.size()+1)-callsite_id_s.size()-1 );
//        //  std::cout<<"\n called F: "<<calledFname<<" num:"<<calledFnumber;
//        //  std::cout<<"\n Callsite: "<<callsite_id_s;
//        //  int callsite_id = std::stoi( callsite_id_s );
//        //  if (callsite_to_pathprefix_map.find(callsite_id) != callsite_to_pathprefix_map.end()) {
//        //    std::cout<<";  have seen path prefix"<<callsite_to_pathprefix_map[callsite_id].get(true);
//        //  }
//        //  //remember the call id, use this number to map to call chain
//        //  csh_obj.set_func_id(std::stoi( calledFnumber ));
//        //  csh_obj.set_callsite_id((callsite_id ) );
//        //}
//      //}
//      //pathPrefix_str = pathPrefix_str+ "_"+  rdfBB;
//      continue;
//    }
