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
#define windowSize 5
std::string definedFuncs[] = {"main","compare" };
std::set<std::string> userDefinedFunctions(definedFuncs, definedFuncs+2);
class callStackHandler {
    std::forward_list<std::string> callStack_list;
    std::forward_list<std::string>::iterator lastPos_it;
    bool setupPhase ; 
    int num_pops;
    std::string currentFuncName, lastFuncName;
   void issue_command(bool only_push=true, std::string func=""){
       if (only_push) {
           std::cout<<"\n PUSH :"<<currentFuncName;
       } else {
           std::cout<<"\n POP :"<<func;
       }
   }
    public:
    callStackHandler () {
        setupPhase = true;
        num_pops= 0;
    }
   void  gotNewLine(int number, std::string funcName) {
        if (number == 0) {
            callStack_list.emplace_front(funcName);
            lastPos_it = callStack_list.begin();
            currentFuncName = funcName;

        }else if (number == 1) {
            if (!setupPhase) {
                auto tmp_it = lastPos_it;//record the iterator before incrementing it, since forward list, cannot get the previous it otherwise

                ++lastPos_it;
                if (lastPos_it == callStack_list.end()) {
                    std::cout<<"\n1. PANIC call stack ilformed";
                    exit(0);
                }
                lastFuncName = *lastPos_it;
                if (lastFuncName != funcName) 
                {
                    num_pops=0;
                    do {
                        //std::cout<<" deleting ::"<<*lastPos_it;
                        //we need to delete the node "lastPos_it", but need the previous node iterator to do that, so this statement will always use the tmp_it that actually points to the head.
                        issue_command(false, *lastPos_it);
                        lastPos_it = callStack_list.erase_after(tmp_it);
                        //std::cout<<" now pointing to ::"<<*lastPos_it;
                        num_pops++;
                    } while ((*lastPos_it) != funcName );
                    //std::cout<<"\n after deleting::";
                    //print();
                }
            }else if (setupPhase ) {
                setupPhase = false;
                callStack_list.emplace_after(lastPos_it, funcName);
                ++lastPos_it;
            }
            issue_command();
        } else {
            ++lastPos_it;
            if (lastPos_it == callStack_list.end()) {
                std::cout<<"\n2. PANIC call stack ilformed";
                exit(0);
            }
            std::string stackFuncName = *lastPos_it;
            if (stackFuncName == funcName)  {
                std::cout<<"\n All's Good";
            } else {
                    lastPos_it = callStack_list.erase_after(lastPos_it);

                std::cout<<"\n3. PANIC call stack ilformed";
                exit(0);
            }
        }
        //std::cout<<"\n current pos:"<<*lastPos_it;
    }
   void print() {
       std::cout<<"\n";
       for (auto it : callStack_list) {
           std::cout<<"\t func:"<<it;
       }
       std::cout<<"\n";

   }
};
int main(int argc, char *argv[] ) { 
    std::string filename(argv[1]);
    std::ifstream infile(filename.c_str());
    std::string line;
    std::string windowBB[windowSize];
    std::array <std::string, 20> callStack;
    std::map<std::set<std::string>, int> function_trace_count;
    int index_window=0;
    std::string delimiter = " " ;
    std::string stack_num, func_name ; 
     callStackHandler csh_obj;
    while (std::getline(infile, line))
    {   
        size_t delim_pos = line.find(delimiter);
        stack_num = line.substr(0,delim_pos );  
        func_name = line.substr(delim_pos); 
        int stackNum = std::stoi(stack_num);
        std::cout<<"\n Stack pos:"<<stackNum<<" func::"<<func_name ;
        csh_obj.gotNewLine(stackNum, func_name);
        //csh_obj.print();
        //If pushing, then append to call chain
        //else if popped then pushed then a different chain
    }   
    return 0;
}

