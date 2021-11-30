#include <iostream>

#include <map>
#include <set>
using namespace std;
#include "statically_reachable_funcs.h"

int gimme_a_func_name(string func_name)
{
    set<string> from_func_name = statically_reachable_funcs[func_name];
    cout <<"func_name can reach: " << endl;
    for(set<string>::iterator it = from_func_name.begin();
        it != from_func_name.end();
        it++){
        cout << *it << " ";
    }
    cout << endl;
    return 0;
}

int main(void)
{
    //gimme_a_func_name("usleep");
    gimme_a_func_name("translit_flatten");
    return 0;
}
