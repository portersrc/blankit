#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <iterator>
#include <map>
#include <set>
#include <string.h>

using namespace std;

map<string, set<string> > statically_reachable_funcs;

template<typename Out>
void split(const string &s, char delim, Out result)
{
    stringstream ss(s);
    string item;
    while(getline(ss, item, delim)){
        *(result++) = item;
    }
}

vector<string> split(const string &s, char delim)
{
    vector<string> elems;
    split(s, delim, back_inserter(elems));
    return elems;
}


void dump_map_to_stdout(void)
{
    for(map<string, set<string> >::iterator it = statically_reachable_funcs.begin();
        it != statically_reachable_funcs.end();
        it++){
        cout << it->first << endl;
        cout << "  ";
        for(set<string>::iterator itt = (it->second).begin();
            itt != (it->second).end();
            itt++){
            cout << *itt << " ";
        }
        cout << endl;
    }
}

void dump_map_to_include_file(void)
{
    map<string, char *> key_to_setname;
    int count = 0;


    FILE *fp_out = fopen("statically_reachable_funcs.h", "w");

    fprintf(fp_out, "#ifndef STATICALLY_REACHABLE_FUNCS_H\n");
    fprintf(fp_out, "#define STATICALLY_REACHABLE_FUNCS_H\n");

    //fprintf(fp_out, "map<string, set<string> > statically_reachable_funcs;\n");

    fprintf(fp_out, "typedef map<string, set<string> > func_map_t;\n");
    fprintf(fp_out, "typedef set<string> func_set_t;\n");


    /*
const func_set_t::value_type raw_set_data_1[] = {
    func_set_t::value_type("hello"),
    func_set_t::value_type("world"),
};
const func_set_t::value_type raw_set_data_2[] = {
    func_set_t::value_type("what"),
    func_set_t::value_type("up"),
};
const int tmp_set_1_sz = sizeof raw_set_data_1 / sizeof raw_set_data_1[0];
func_set_t tmp_set_1(raw_set_data_1, raw_set_data_1 + tmp_set_1_sz);
const int tmp_set_2_sz = sizeof raw_set_data_2 / sizeof raw_set_data_2[0];
func_set_t tmp_set_2(raw_set_data_2, raw_set_data_2 + tmp_set_2_sz);


const func_map_t::value_type raw_data[] = {
    func_map_t::value_type("key1", tmp_set_1),
    func_map_t::value_type("key2", tmp_set_2),
};
const int num_elems = sizeof raw_data / sizeof raw_data[0];
func_map_t tmp_map(raw_data, raw_data + num_elems);
    */

    for(map<string, set<string> >::iterator it = statically_reachable_funcs.begin();
        it != statically_reachable_funcs.end();
        it++){

        string func_name = it->first;

        char *buf = (char *) malloc(strlen("tmp_set_") + 1 + 10);
        sprintf(buf, "tmp_set_%d", count);
        key_to_setname[func_name] = buf;

        fprintf(fp_out, "const func_set_t::value_type raw_set_data_%d[] = {\n", count);
        for(set<string>::iterator itt = (it->second).begin();
            itt != (it->second).end();
            itt++){
            fprintf(fp_out, "func_set_t::value_type(\"%s\"),\n", (*itt).c_str());
        }

        fprintf(fp_out, "};\n");


        fprintf(fp_out, "const int tmp_set_%d_sz = sizeof raw_set_data_%d / sizeof raw_set_data_%d[0];\n", count, count, count);
        fprintf(fp_out, "func_set_t tmp_set_%d(raw_set_data_%d, raw_set_data_%d + tmp_set_%d_sz);\n", count, count, count, count);


        count++;
    }


    fprintf(fp_out, "const func_map_t::value_type raw_data[] = {\n");
    for(map<string, set<string> >::iterator it = statically_reachable_funcs.begin();
        it != statically_reachable_funcs.end();
        it++){
        fprintf(fp_out, "func_map_t::value_type(\"%s\", %s),\n", (it->first).c_str(), key_to_setname[(it->first)]);
    }
    fprintf(fp_out, "};\n");
    
    fprintf(fp_out, "const int num_elems = sizeof raw_data / sizeof raw_data[0];\n");
    fprintf(fp_out, "func_map_t statically_reachable_funcs(raw_data, raw_data + num_elems);\n");
    fprintf(fp_out, "#endif");

    fclose(fp_out);
}

int main(void)
{
    long file_size;
    size_t nmemb_read;

    FILE *fp = fopen("staticReachableFuncs", "r");
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);  // rewind


    char *file_data = (char *) malloc(file_size);
    nmemb_read = fread(file_data, file_size, 1, fp);
    assert(nmemb_read == 1);

    istringstream iss(file_data);

    vector<string> parts;
    for(string line; getline(iss, line); ){
            //cout << line << endl;
            parts = split(line, ':');
            for(vector<string>::iterator it = parts.begin(); it != parts.end(); it++){
                //cout << *it << endl;
            }
            string func_name = parts[0];
            //int num_statically_reachable_funcs = parts[1];
            parts = split(parts[2], ',');
            for(vector<string>::iterator it = parts.begin(); it != parts.end(); it++){
                //cout << *it << endl;
                statically_reachable_funcs[func_name].insert(*it);
            }
            

    }

    dump_map_to_stdout();
    //dump_map_to_include_file();

    free(file_data);
    fclose(fp);

}
/*

//typedef map<string, int> func_map_t;
//
//const func_map_t::value_type raw_data[] = {
//    func_map_t::value_type("hello", 42),
//    func_map_t::value_type("world", 88),
//};
//const int num_elems = sizeof raw_data / sizeof raw_data[0];
//func_map_t tmp_map(raw_data, raw_data + num_elems);

//map<string, set<string> > tmp_map;
//map<string, string> tmp_map;

typedef map<string, set<string> > func_map_t;
typedef set<string> func_set_t;

const func_set_t::value_type raw_set_data_1[] = {
    func_set_t::value_type("hello"),
    func_set_t::value_type("world"),
};
const func_set_t::value_type raw_set_data_2[] = {
    func_set_t::value_type("what"),
    func_set_t::value_type("up"),
};
const int tmp_set_1_sz = sizeof raw_set_data_1 / sizeof raw_set_data_1[0];
func_set_t tmp_set_1(raw_set_data_1, raw_set_data_1 + tmp_set_1_sz);
const int tmp_set_2_sz = sizeof raw_set_data_2 / sizeof raw_set_data_2[0];
func_set_t tmp_set_2(raw_set_data_2, raw_set_data_2 + tmp_set_2_sz);


const func_map_t::value_type raw_data[] = {
    func_map_t::value_type("key1", tmp_set_1),
    func_map_t::value_type("key2", tmp_set_2),
};
const int num_elems = sizeof raw_data / sizeof raw_data[0];
func_map_t tmp_map(raw_data, raw_data + num_elems);


int main(void)
{
    //for(map<string, int>::iterator it = tmp_map.begin();
    //    it != tmp_map.end();
    //    it++){
    //    cout << it->first << " " << it->second << endl;
    //}

    //for(set<string>::iterator it = tmp_set.begin();
    //    it != tmp_set.end();
    //    it++){
    //    cout << *it  << endl;
    //}

    for(map<string, set<string> >::iterator it = tmp_map.begin();
        it != tmp_map.end();
        it++){
        cout << it->first << endl;
        cout << "  ";
        for(set<string>::iterator itt = (it->second).begin();
            itt != (it->second).end();
            itt++){
            cout << *itt << " ";
        }
        cout << endl;
    }
    return 0;
}*/
