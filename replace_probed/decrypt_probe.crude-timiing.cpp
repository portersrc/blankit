#include "pin.H"
#include <iostream>
#include <fstream>
#include <string>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/time.h>
#include <deque>
#include <set>
#include "/home/rudy/debloat/pin_probe_debloat_experiments/blankit/blankit.h"

using namespace std;

//
// Hand-modified settings
//
#define BLANKIT_PROBE_PREDICT
#define BLANKIT_PROBE_TIME
//#define BLANKIT_PROBE_DBG
//#define BLANKIT_PROBE_DO_NOTHING
//#define BLANKIT_PROBE_EMPTY
//#define BLANKIT_PROBE_NO_PREDICT_AUX
//#define BLANKIT_TRACE_COUNT
//#define BLANKIT_AUDIT
//#define BLANKIT_SINGLE_CALL_LOADING


//
// Passed via env variables
//
char BLANKIT_APP_NAME[256];
char BLANKIT_APPROVED_LIST[256]; // glibc
char BLANKIT_APPROVED_LIST_LIBM[256];
char BLANKIT_APPROVED_LIST_LIBGCC[256];
char BLANKIT_APPROVED_LIST_LIBSTDCPP[256];
char BLANKIT_APPROVED_LIST_LIBDL[256];
char BLANKIT_APPROVED_LIST_LIBPTHREAD[256];
char BLANKIT_APPROVED_LIST_LIBCRYPT[256];
char BLANKIT_APPROVED_LIST_LIBPCRE[256];
char BLANKIT_APPROVED_LIST_LIBZ[256];
char BLANKIT_APPROVED_LIST_LIBCRYPTO[256];
char BLANKIT_APPROVED_LIST_LIBUTIL[256];
char BLANKIT_APPROVED_LIST_LIBRESOLV[256];
char BLANKIT_APPROVED_LIST_LIBSSH[256];
char BLANKIT_APPROVED_LIST_LIBNSS_COMPAT[256];
char BLANKIT_APPROVED_LIST_LIBNSL[256];
char BLANKIT_APPROVED_LIST_LIBNSS_NIS[256];
char BLANKIT_APPROVED_LIST_LIBNSS_FILES[256];
char BLANKIT_PREDICT_SETS[256];
int BLANKIT_TRACE = 0; // 1 if BLANKIT_TRACE env variable is set
int BLANKIT_LAZY  = 0; // 1 if BLANKIT_LAZY env variable is set


// FIXME: Might want to make this a more permanent fixture. When working
// with nginx, it forks and screws up the output. Writing to a file fixed.
// No time to check, but that might have broken old spec2006 stuff though.
//FILE *blankit_log;

// https://software.intel.com/sites/landingpage/pintool/docs/71313/Pin/html/
// A probe may be up to 14 bytes long.
#define MAGIC_OFFSET 14


#define REPO_BASE "/home/rudy/debloat/pin_probe_debloat_experiments/"
#define STATIC_REACH_FUNCS_FILE "/home/rudy/debloat/pin_probe_debloat_experiments/libc/staticReachableFuncs"
#define RWX_PERM (PROT_READ | PROT_WRITE | PROT_EXEC)
#define RX_PERM  (PROT_READ | PROT_EXEC)

/*#ifdef BLANKIT_PROBE_DBG
#define PRINTF_FLUSH(...) \
    do{ \
        fprintf(blankit_log, __VA_ARGS__); \
        fflush(blankit_log); \
        printf(__VA_ARGS__); \
        fflush(0); \
    }while(0)
#else
#define PRINTF_FLUSH(...)
#endif*/
#ifdef BLANKIT_PROBE_DBG
#define PRINTF_FLUSH(...) \
    do{ \
        printf(__VA_ARGS__); \
        fflush(0); \
    }while(0)
#else
#define PRINTF_FLUSH(...)
#endif

#ifdef BLANKIT_PROBE_TIME
#define PRINTF_TIME(...) \
    do{ \
        printf(__VA_ARGS__); \
        fflush(0); \
    }while(0)
#else
#define PRINTF_TIME(...)
#endif


typedef enum{
    E_BLANK_STATE_INACTIVE     = 0,
    E_BLANK_STATE_ACTIVE       = 1,
    E_BLANK_STATE_MISPREDICTED = 2,
    E_BLANK_STATE_DONE         = 3,
}blank_state_e;

typedef struct{
    int num_no_predicts;
    int num_mispredicts;
    int num_underpredicts;
    int num_overpredicts;
    int num_correct_predicts;

    // understimation due to our implementation with probe_blank not always
    // getting called at the end
    int num_correct_full_chain_predicts;

    unsigned long num_blankit_predict_probes;
    unsigned long num_copy_probes;
}blankit_stats_t;

blankit_stats_t blankit_stats = {0};

int BLANKIT_TRACE_COUNT_FREQ = 1000;


int page_size;
blank_state_e blank_state = E_BLANK_STATE_INACTIVE;
map<char *, unsigned int> copied_funcs;


vector<string>       instr_funcs_libc;
vector<string>       instr_funcs_libm;
vector<string>       instr_funcs_libgcc;
vector<string>       instr_funcs_libstdcpp;
vector<string>       instr_funcs_libdl;
vector<string>       instr_funcs_libpthread;
vector<string>       instr_funcs_libcrypt;
vector<string>       instr_funcs_libpcre;
vector<string>       instr_funcs_libz;
vector<string>       instr_funcs_libcrypto;
vector<string>       instr_funcs_libutil;
vector<string>       instr_funcs_libresolv;
vector<string>       instr_funcs_libssh;
vector<string>       instr_funcs_libnss_compat;
vector<string>       instr_funcs_libnsl;
vector<string>       instr_funcs_libnss_nis;
vector<string>       instr_funcs_libnss_files;
vector<set<string> > pred_sets;
int pred_set_idx;
int old_pred_set_idx;
map<char *, string> addr_to_name;
//map<char *, string> addr_to_name_libm;
map<char *, unsigned int> addr_to_size;
map<char *, void *> addr_to_copy;
map<string, pair<char *, int> > name_to_addr_and_size;
map<string, set<string> > statically_reachable_funcs;

string *entry_lib_func_p;

//char *addr_of_libc_text;
//int   size_of_libc_text;


static inline
long timestamp_ns(void)
{
    // clock_gettime doesn't play nice with pin. Can't find librt,
    // and rebuilding the probe with -lrt, updating LD_LIBRARY_PATH,
    // etc. etc. is not solving it.
    //struct timespec tv = {0};
    //if(clock_gettime(CLOCK_REALTIME, &tv) != 0){
    //    printf("timestamp_ns: timer error \n");
    //    return 0;
    //}
    //return tv.tv_sec * 1000000000 + tv.tv_nsec;

    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000000000) + (tv.tv_usec * 1000);
}


static inline
void remap_permissions(char *addr, int size, int perm)
{
    char *aligned_addr_base;
    char *aligned_addr_end;
    int size_to_remap;

    PRINTF_FLUSH("remap_permissions():\n");

    aligned_addr_base = (char *) ((unsigned long)(addr) & ~(page_size - 1));
    aligned_addr_end  = (char *) ((unsigned long)(addr+size) & ~(page_size - 1));
    size_to_remap = page_size + (aligned_addr_end - aligned_addr_base);
    PRINTF_FLUSH("  aligned_addr_base: %p\n", aligned_addr_base);
    PRINTF_FLUSH("  aligned_addr_end:  %p\n", aligned_addr_end);
    PRINTF_FLUSH("  size_to_remap:     %d\n", size_to_remap);

    if(mprotect(aligned_addr_base, size_to_remap, perm) == -1){
        PRINTF_FLUSH("mprotect error\n");
    }
    PRINTF_FLUSH("  mprotect succeeded\n");
}

static inline
void remap_permissions_text(char *addr, int size, int perm)
{
    char *aligned_addr_base;
    char *aligned_addr_end;
    int size_to_remap;

    PRINTF_FLUSH("remap_permissions_text():\n");

    aligned_addr_base = (char *) ((unsigned long)(addr) & ~(page_size - 1));
    aligned_addr_end  = (char *) ((unsigned long)(addr+size) & ~(page_size - 1));
    size_to_remap = (aligned_addr_end - aligned_addr_base);
    PRINTF_FLUSH("  aligned_addr_base: %p\n", aligned_addr_base);
    PRINTF_FLUSH("  aligned_addr_end:  %p\n", aligned_addr_end);
    PRINTF_FLUSH("  size_to_remap:     %d\n", size_to_remap);

    if(mprotect(aligned_addr_base, size_to_remap, perm) == -1){
        PRINTF_FLUSH("mprotect error\n");
    }
    PRINTF_FLUSH("  mprotect succeeded\n");
}


static inline
void blank_func(char *addr_base, int size_cp)
{
    PRINTF_FLUSH("blank_func()\n");
    memset(addr_base + MAGIC_OFFSET, 0, size_cp);
    PRINTF_FLUSH("  memset succeeded\n");
}
static inline
void copy_func(void *copy_of_func, char *addr_base, int size_cp)
{
    PRINTF_FLUSH("copy_func():\n");
    memcpy(copy_of_func, addr_base + MAGIC_OFFSET, size_cp);
    PRINTF_FLUSH("  memcpy succeeded\n");
}
static inline
void dump_pred_sets(int pred_set_idx)
{
    for(set<string>::iterator it = pred_sets[pred_set_idx].begin();
        it != pred_sets[pred_set_idx].end();
        it++){
        PRINTF_FLUSH("  %s ", it->c_str());
    }
    PRINTF_FLUSH("\n");
}


static inline
void probe_copy_aux(void *copy_of_func, char *addr_base, unsigned int size_cp)
{
    PRINTF_FLUSH("  copy_of_func: %p\n", copy_of_func);
    PRINTF_FLUSH("  addr_base:    %p\n", addr_base);
    PRINTF_FLUSH("  size_cp:      %d\n", size_cp);
    PRINTF_FLUSH("  name:         %s\n", addr_to_name[addr_base].c_str());

    //PRINTF_FLUSH("before remap\n");
    remap_permissions(addr_base, size_cp, RWX_PERM);
    //PRINTF_FLUSH("before memcpy\n");
    memcpy(addr_base + MAGIC_OFFSET, copy_of_func, size_cp);
    //PRINTF_FLUSH("before seting copied funcs\n");
    copied_funcs[addr_base] = size_cp;
    //PRINTF_FLUSH("probe_copy_aux returning\n");
    remap_permissions(addr_base, size_cp, RX_PERM);
}

void dump_time(struct timeval *start, int n)
{
    long time;
    struct timeval end;
    gettimeofday(&end, NULL);
    time = (1000000 * (end.tv_sec - start->tv_sec))
           + (end.tv_usec - start->tv_usec);
    PRINTF_TIME("%d unblank time: %ld\n", n, time);
}


void probe_copy(void *copy_of_func, char *addr_base, unsigned int size_cp)
{
    struct timeval start;

    gettimeofday(&start, NULL);

    //if(addr_to_name_libm.count(addr_base) > 0){
    //    printf("HIT libmfunc: %s\n", addr_to_name_libm[addr_base].c_str());
    //    fflush(0);
    //}
    //printf("HIT: %s\n", addr_to_name[addr_base].c_str());
    //fflush(0);

#ifdef BLANKIT_AUDIT
    long time;
    if(blank_state == E_BLANK_STATE_INACTIVE){
        printf("audit-entry-func\n");
        fflush(0);
        entry_lib_func_p = &addr_to_name[addr_base];
        blank_state = E_BLANK_STATE_ACTIVE;
    }
#endif

#ifndef BLANKIT_PROBE_EMPTY
    PRINTF_FLUSH("probe_copy()\n");

    // Optimization: don't copy what's already there
    if(copied_funcs.find(addr_base) != copied_funcs.end()){
        PRINTF_FLUSH("  addr already part of copied_funcs\n");
#ifdef BLANKIT_AUDIT
        // useful output when trying to estimate runtime prediction accuracy
        printf("audit-correct-predict\n");
        fflush(0);
#endif
        dump_time(&start, 1);
        return;
    }

#ifdef BLANKIT_PROBE_PREDICT
//#if 0

    // Start by checking if there are any predictions
    //PRINTF_FLUSH("dumping pred_sets at idx %d\n", pred_set_idx);
    //dump_pred_sets(pred_set_idx);

    PRINTF_FLUSH("checking this against: %s\n", addr_to_name[addr_base].c_str());
    // If there are, check if the probe that is currently executing is
    // in the predicted set
    if(pred_sets[pred_set_idx].find(addr_to_name[addr_base])
       != pred_sets[pred_set_idx].end()){
#ifdef BLANKIT_AUDIT
        // useful output when trying to estimate runtime prediction accuracy
        printf("audit-correct-predict\n");
        fflush(0);
#endif


#ifdef BLANKIT_SINGLE_CALL_LOADING
        // Copy just this function into place.
        string func_name = addr_to_name[addr_base];
        pair<char *, int> addr_and_size = name_to_addr_and_size[func_name];
        probe_copy_aux(addr_to_copy[addr_and_size.first],
                       addr_and_size.first,
                       addr_and_size.second);
#else
        // Optimization: Full call chain loading
        // We got a match. We need to copy a list of functions into place.
        // The last state was "inactive", so this is the first function that's
        // gotten hit inside of the library
        for(set<string>::iterator it = pred_sets[pred_set_idx].begin();
            it != pred_sets[pred_set_idx].end();
            it++){
            PRINTF_FLUSH("checking name_to_addr_and_size[%s]\n", (*it).c_str());
            if(name_to_addr_and_size.count(*it) > 0){
                pair<char *, int> addr_and_size = name_to_addr_and_size[*it];
                PRINTF_FLUSH("addr_and_size.first:  %p\n", addr_and_size.first);
                PRINTF_FLUSH("addr_and_size.second: %u\n", addr_and_size.second);
                probe_copy_aux(addr_to_copy[addr_and_size.first],
                               addr_and_size.first,
                               addr_and_size.second);
            }
        }
#endif

        dump_time(&start, 2);
        return;

    }else{
#ifdef BLANKIT_AUDIT
        // mispredict
        if(blank_state == E_BLANK_STATE_ACTIVE){
            time = timestamp_ns();
            printf("audit-mispredict %-20s %ld\n",
                   entry_lib_func_p->c_str(),
                   time);
            fflush(0);
            blank_state = E_BLANK_STATE_MISPREDICTED;
        }
#else
        PRINTF_FLUSH("  mispredict or attack\n");
#endif
    }

    probe_copy_aux(copy_of_func, addr_base, size_cp);

#else
    
    probe_copy_aux(copy_of_func, addr_base, size_cp);
    blank_state = E_BLANK_STATE_ACTIVE;
    dump_time(&start, 3);

#endif // BLANKIT_PROBE_PREDICT
#endif // BLANKIT_PROBE_EMPTY
}




static inline
void probe_blank_aux(void)
{
    long time;
    struct timeval start;
    struct timeval end;
    PRINTF_FLUSH("probe_blank_aux()\n");
    gettimeofday(&start, NULL);
    for(map<char *, unsigned int>::iterator it = copied_funcs.begin();
        it != copied_funcs.end();
        ++it){
        char *addr = it->first;
        unsigned int size = it->second;
        PRINTF_FLUSH("  blanking %p %d %s\n", addr, size, addr_to_name[addr].c_str());
        remap_permissions(addr, size, RWX_PERM);
        blank_func(addr, size);
        remap_permissions(addr, size, RX_PERM);
    }
    copied_funcs.clear();
    gettimeofday(&end, NULL);
    time = (1000000 * (end.tv_sec - start.tv_sec))
           + (end.tv_usec - start.tv_usec);
    PRINTF_TIME("blank time: %ld\n", time);
    PRINTF_FLUSH("  succeeded\n");
}


void probe_blankit_predict(int predict_idx)
{
#ifndef BLANKIT_PROBE_EMPTY
#ifndef BLANKIT_PROBE_NO_PREDICT_AUX
    PRINTF_FLUSH("probe_blankit_predict()\n");
    PRINTF_FLUSH("predict_idx: %d\n", predict_idx);
    probe_blank_aux();
    pred_set_idx = predict_idx;
#endif
#endif
}

void probe_blankit_predict_lazy(int predict_idx)
{
#ifndef BLANKIT_PROBE_EMPTY
    PRINTF_FLUSH("probe_blankit_predict_lazy()\n");
#ifndef BLANKIT_PROBE_NO_PREDICT_AUX
    PRINTF_FLUSH("predict_idx: %d\n", predict_idx);
    old_pred_set_idx = pred_set_idx;
    pred_set_idx = predict_idx;
#endif
    blank_state = E_BLANK_STATE_INACTIVE;
    if(old_pred_set_idx != pred_set_idx){
        probe_blank_aux();
    }
#endif
}



void probe_copy_trace(char *addr)
{
#ifdef BLANKIT_TRACE_COUNT
    blankit_stats.num_copy_probes++;
    if((blankit_stats.num_copy_probes % BLANKIT_TRACE_COUNT_FREQ) == 0){
        PRINTF_FLUSH("probe_copy_trace(): %lu\n", blankit_stats.num_copy_probes);
    }
#else
    PRINTF_FLUSH("probe_copy_trace()\n");
    PRINTF_FLUSH("  PCT trace_addr: %p\n", addr);
    PRINTF_FLUSH("  PCT trace_name: %s\n", addr_to_name[addr].c_str());
#endif
}

void probe_blankit_predict_trace(int predict_idx)
{
#ifdef BLANKIT_TRACE_COUNT
    blankit_stats.num_blankit_predict_probes++;
    if((blankit_stats.num_blankit_predict_probes % BLANKIT_TRACE_COUNT_FREQ) == 0){
        PRINTF_FLUSH("probe_blankit_predict_trace(): %lu\n", blankit_stats.num_blankit_predict_probes);
    }
#else
    PRINTF_FLUSH("probe_blankit_predict_trace()\n");
    dump_pred_sets(predict_idx);
#endif
}




void dump_routines(SEC text_sec)
{
    RTN rtn;
    rtn  = SEC_RtnHead(text_sec);
    while(rtn != RTN_Invalid()){
        PRINTF_FLUSH("RTN name: %s\n", RTN_Name(rtn).c_str());
        rtn  = RTN_Next(rtn);
    }
    PRINTF_FLUSH("Loop broke. RTN is invalid\n");
}

void set_rtn_head(RTN *rtn, IMG &img)
{
    SEC sec;
    sec  = IMG_SecHead(img);
    while(sec != SEC_Invalid()){
        if(SEC_Name(sec) == ".text"){
            *rtn = SEC_RtnHead(sec);
            return;
        }
        sec  = SEC_Next(sec);
    }
    PRINTF_FLUSH("ERROR: Unable to find head routine for text section\n");
    exit(1);
}



BOOL FindAndCheckRtn(IMG img, string rtnName, RTN& rtn)
{
    rtn = RTN_FindByName(img, rtnName.c_str());

    if(RTN_Valid(rtn)){
        if(!RTN_IsSafeForProbedInsertion(rtn)){
            return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}




void remap_entire_text_section_as_writeable(SEC sec)
{
    char *text_addr_aligned;
    ADDRINT text_addr = SEC_Address(sec);
    USIZE   text_size = SEC_Size(sec);

    //cout << "the text_addr is: " << text_addr << endl;
    //cout << "the text_size is: " << text_size << endl;
    text_addr_aligned = (char *) ((unsigned long)(text_addr) & ~(page_size - 1));

    if(mprotect(text_addr_aligned, text_size, RWX_PERM) == -1){
        PRINTF_FLUSH("mprotect error on all of text section\n");
    }
    PRINTF_FLUSH("Mapped all of text section writeable\n");
}

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


void init_pred_sets(void)
{
    long file_size;
    size_t nmemb_read;

    FILE *fp = fopen(BLANKIT_PREDICT_SETS, "r");
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);  // rewind

    char *file_data = (char *) malloc(file_size);
    nmemb_read = fread(file_data, file_size, 1, fp);
    assert(nmemb_read == 1);

    set<string> empty;
    istringstream iss(file_data);
    vector<string> parts;
    for(string line; getline(iss, line); ){
            set<string> preds;
            parts = split(line, ';');
            //cout << parts.size() << endl;
            if(parts.size() == 0){
                pred_sets.push_back(preds); // index 0 is ignored
                continue;
            }
            // Note: If parts.size() == 2 (which could happen for a line that
            // looks like ";,392"), we allow it. We will end up with an empty
            // set. The 1st element is "", which is ignored in the loop
            // below. The second element is ",392", and that gets removed after
            // the loop. But this is fine. We should never receive a prediction
            // for these cases, and we want to maintain this ordering.
            for(vector<string>::iterator it = parts.begin(); it != parts.end(); it++){
                if(*it == ""){
                    continue;
                }
                //cout << *it << endl;
                preds.insert(*it);
            }
            preds.erase(parts.back()); // last element was an id that we dont want
            pred_sets.push_back(preds);
            //cout << line << endl;
    }

    int idx = 0;
    for(vector<set<string> >::iterator it = pred_sets.begin(); it != pred_sets.end(); it++){
        PRINTF_FLUSH("idx: %d\n", idx);
        for(set<string>::iterator itt = (*it).begin(); itt != (*it).end(); itt++){
            PRINTF_FLUSH("  %s", itt->c_str());
        }
        PRINTF_FLUSH("\n");
        idx++;
    }

    free(file_data);
    fclose(fp);
}

void init_pred_sets_stub(void)
{
    set<string> empty;
    pred_sets.push_back(empty);
    pred_set_idx    = 0;
    old_pred_set_idx = 0;
}


void init_instr_funcs(char *filename, vector<string> &instr_funcs)
{
    long file_size;
    size_t nmemb_read;

    FILE *fp = fopen(filename, "r");
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);  // rewind

    char *file_data = (char *) malloc(file_size);
    nmemb_read = fread(file_data, file_size, 1, fp);
    assert(nmemb_read == 1);

    istringstream iss(file_data);
    vector<string> parts;
    for(string line; getline(iss, line); ){
            instr_funcs.push_back(line);
    }

    free(file_data);
    fclose(fp);
}

void init_statically_reachable_funcs(void)
{
    long file_size;
    size_t nmemb_read;

    FILE *fp = fopen(STATIC_REACH_FUNCS_FILE , "r");
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

    free(file_data);
    fclose(fp);
}

void image_routine_dump(IMG &img)
{

    PRINTF_FLUSH("image_routine_dump()\n");

    SEC sec;
    int found_sect_head = 0;
    sec  = IMG_SecHead(img);
    while(sec != SEC_Invalid()){
        if(SEC_Name(sec) == ".text"){
            dump_routines(sec);
            found_sect_head = 1;
            break;
        }
        sec  = SEC_Next(sec);
    }
    assert(found_sect_head);
}


void image_load(IMG &img,
                char *approved_list,
                vector<string> &instr_funcs)
{
    RTN rtn;
    void *copy_of_func;
    char *addr_base;
    int size_cp = 0;
    unsigned int i = 0;

    PRINTF_FLUSH("image_load()\n");

    init_instr_funcs(approved_list, instr_funcs);

    for(i = 0; i < instr_funcs.size(); i++){
        PRINTF_FLUSH("RTN name: %s", instr_funcs[i].c_str());
        if(!FindAndCheckRtn(img, instr_funcs[i], rtn)){
            PRINTF_FLUSH("  not adding probe. failed checks.\n");
            continue;
        }
        PRINTF_FLUSH("\n");

        size_cp = RTN_Size(rtn) - MAGIC_OFFSET;
        if(size_cp < 1){
            PRINTF_FLUSH("  not adding probe. copy size is too small\n");
            continue;
        }

        addr_base    = (char *) RTN_Address(rtn);
        copy_of_func = malloc(size_cp);
        PRINTF_FLUSH("  copy address: %p\n", copy_of_func);
        PRINTF_FLUSH("  addr_base:    %p\n", addr_base);
        PRINTF_FLUSH("  size_cp:      %d\n", size_cp);

        addr_to_name[addr_base] = RTN_Name(rtn);
        addr_to_size[addr_base] = size_cp;
        addr_to_copy[addr_base] = copy_of_func;
        //if(IMG_Name(img).find("libm.so") != string::npos){
        //    addr_to_name_libm[addr_base] = RTN_Name(rtn);
        //}

        name_to_addr_and_size[RTN_Name(rtn)] = make_pair(addr_base, size_cp);

#ifndef BLANKIT_PROBE_EMPTY
        copy_func(copy_of_func, addr_base, size_cp);
        remap_permissions(addr_base, RTN_Size(rtn), RWX_PERM);
        blank_func(addr_base, size_cp);
        remap_permissions(addr_base, RTN_Size(rtn), RX_PERM);
#endif
        RTN_InsertCallProbed(rtn, IPOINT_BEFORE, AFUNPTR( probe_copy ),
                             IARG_PTR, copy_of_func,
                             IARG_PTR, addr_base,
                             // safe cast. see a few lines up where we ignore
                             // continue to the next iteration if size < 1
                             IARG_UINT32, (unsigned int) size_cp,
                             IARG_END);
    }
    PRINTF_FLUSH("done image_load()\n");
}

void image_load_libc_trace(IMG &img)
{
    RTN rtn;

    PRINTF_FLUSH("image_load_libc_trace()\n");

    set_rtn_head(&rtn, img);

    SEC sec;
    int found_sect_head = 0;
    sec  = IMG_SecHead(img);
    while(sec != SEC_Invalid()){
        if(SEC_Name(sec) == ".text"){
            rtn = SEC_RtnHead(sec);
            found_sect_head = 1;
            //remap_entire_text_section_as_writeable(sec);
            break;
        }
        sec  = SEC_Next(sec);
    }
    assert(found_sect_head);

    while(rtn != RTN_Invalid()){
        PRINTF_FLUSH("RTN name: %s\n", RTN_Name(rtn).c_str());

        if(!RTN_IsSafeForProbedInsertion(rtn)){
            PRINTF_FLUSH("  not safe for probed insertion\n");
            rtn  = RTN_Next(rtn);
            continue;
        }

        addr_to_name[(char *)RTN_Address(rtn)] = RTN_Name(rtn);

        RTN_InsertCallProbed(rtn, IPOINT_BEFORE, AFUNPTR( probe_copy_trace ),
                             IARG_PTR, RTN_Address(rtn),
                             IARG_END);
        rtn  = RTN_Next(rtn);
    }
}



void image_load_libblankit_trace(IMG &img)
{
    RTN rtn;

    PRINTF_FLUSH("image_load_libblankit_trace()\n");

    rtn = RTN_FindByName(img, "blankit_predict");
    if(RTN_Valid(rtn) && RTN_IsSafeForProbedReplacement(rtn)){
        RTN_ReplaceProbed(rtn, AFUNPTR(probe_blankit_predict_trace));
    }else{
        PRINTF_FLUSH("ERROR: blankit_predict failed checks\n");
        exit(1);
    }
}


void image_load_libblankit(IMG &img)
{
    RTN rtn;

    PRINTF_FLUSH("image_load_libblankit()\n");

    rtn = RTN_FindByName(img, "blankit_predict");
    if(RTN_Valid(rtn) && RTN_IsSafeForProbedReplacement(rtn)){
        if(BLANKIT_LAZY){
            RTN_ReplaceProbed(rtn, AFUNPTR(probe_blankit_predict_lazy));
        }else{
            RTN_ReplaceProbed(rtn, AFUNPTR(probe_blankit_predict));
        }
        //RTN_ReplaceProbed(rtn, AFUNPTR(probe_blankit_predict));
    }else{
        PRINTF_FLUSH("ERROR: blankit_predict failed checks\n");
        exit(1);
    }
}




KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
                            "o", "decrypt_probe.outfile", "specify file name");

INT32 Usage(void)
{
    cerr <<
        "This pin tool tests probe replacement.\n"
        "\n";
    cerr << KNOB_BASE::StringKnobSummary();
    cerr << endl;
    return -1;
}


VOID ImageLoad(IMG img, VOID *v)
{
    long time;
    struct timeval start;
    struct timeval end;

    gettimeofday(&start, NULL);

    // This function is called every time a new image is loaded
    PRINTF_FLUSH("Image name is: %s\n", IMG_Name(img).c_str());

    if(BLANKIT_TRACE){
        // Get an in-order trace of probes
        if(IMG_Name(img).find("libc.so") != string::npos){
            image_load_libc_trace(img);
        }else if(IMG_Name(img).find("libblankit.so") != string::npos){
            image_load_libblankit_trace(img);
        }
    }else{
        if(IMG_Name(img).find("libc.so") != string::npos){
            image_load(img, BLANKIT_APPROVED_LIST, instr_funcs_libc);
        }else if(IMG_Name(img).find("libm.so") != string::npos){
            image_load(img, BLANKIT_APPROVED_LIST_LIBM, instr_funcs_libm);
        }else if(IMG_Name(img).find("libgcc_s.so") != string::npos){
            image_load(img, BLANKIT_APPROVED_LIST_LIBGCC, instr_funcs_libgcc);
        }else if(IMG_Name(img).find("libstdc++.so") != string::npos){
            image_load(img, BLANKIT_APPROVED_LIST_LIBSTDCPP, instr_funcs_libstdcpp);


        // nginx
        }else if(IMG_Name(img).find("libdl.so") != string::npos){
            // + sshd
            image_load(img, BLANKIT_APPROVED_LIST_LIBDL, instr_funcs_libdl);
        }else if(IMG_Name(img).find("libpthread.so") != string::npos){
            image_load(img, BLANKIT_APPROVED_LIST_LIBPTHREAD, instr_funcs_libpthread);
        }else if(IMG_Name(img).find("libcrypt.so") != string::npos){
            // + sshd
            image_load(img, BLANKIT_APPROVED_LIST_LIBCRYPT, instr_funcs_libcrypt);
        }else if(IMG_Name(img).find("libpcre.so") != string::npos){
            image_load(img, BLANKIT_APPROVED_LIST_LIBPCRE, instr_funcs_libpcre);
        }else if(IMG_Name(img).find("libz.so") != string::npos){
            // + sshd
            image_load(img, BLANKIT_APPROVED_LIST_LIBZ, instr_funcs_libz);


        // sshd
        }else if(IMG_Name(img).find("libcrypto.so") != string::npos){
            image_load(img, BLANKIT_APPROVED_LIST_LIBCRYPTO, instr_funcs_libcrypto);
        }else if(IMG_Name(img).find("libutil.so") != string::npos){
            image_load(img, BLANKIT_APPROVED_LIST_LIBUTIL, instr_funcs_libutil);
        }else if(IMG_Name(img).find("libresolv.so") != string::npos){
            image_load(img, BLANKIT_APPROVED_LIST_LIBRESOLV, instr_funcs_libresolv);
        }else if(IMG_Name(img).find("libssh.so") != string::npos){
            image_load(img, BLANKIT_APPROVED_LIST_LIBSSH, instr_funcs_libssh);
        }else if(IMG_Name(img).find("libnss_compat.so") != string::npos){
            image_load(img, BLANKIT_APPROVED_LIST_LIBNSS_COMPAT, instr_funcs_libnss_compat);
        }else if(IMG_Name(img).find("libnsl.so") != string::npos){
            image_load(img, BLANKIT_APPROVED_LIST_LIBNSL, instr_funcs_libnsl);
        }else if(IMG_Name(img).find("libnss_nis.so") != string::npos){
            image_load(img, BLANKIT_APPROVED_LIST_LIBNSS_NIS, instr_funcs_libnss_nis);
        }else if(IMG_Name(img).find("libnss_files.so") != string::npos){
            image_load(img, BLANKIT_APPROVED_LIST_LIBNSS_FILES, instr_funcs_libnss_files);


        }else if(IMG_Name(img).find("libblankit.so") != string::npos){
            image_load_libblankit(img);
        }else if(IMG_Name(img).find(BLANKIT_APP_NAME) != string::npos){
            //if(!BLANKIT_LAZY){
            //    printf("ERROR: !BLANKIT_LAZY no longer supported. Revert to "
            //           "something before 2018.04.16 to recover this code.\n");
            //    exit(1);
            //}
        }
    }

    // Dump routine names
    //if(IMG_Name(img).find("libc.so") != string::npos){
    //if(IMG_Name(img).find("libm.so") != string::npos){
    //if(IMG_Name(img).find("libgcc_s.so") != string::npos){
    //if(IMG_Name(img).find("libstdc++.so") != string::npos){
    //if(IMG_Name(img).find("libdl.so") != string::npos){
    //if(IMG_Name(img).find("libpthread.so") != string::npos){
    //if(IMG_Name(img).find("libcrypt.so") != string::npos){
    //if(IMG_Name(img).find("libpcre.so") != string::npos){
    //if(IMG_Name(img).find("libz.so") != string::npos){
    //if(IMG_Name(img).find("libcrypto.so") != string::npos){
    //if(IMG_Name(img).find("libutil.so") != string::npos){
    //if(IMG_Name(img).find("libresolv.so") != string::npos){
    //if(IMG_Name(img).find("libssh.so") != string::npos){
    //if(IMG_Name(img).find("libnss_compat.so") != string::npos){
    //if(IMG_Name(img).find("libnsl.so") != string::npos){
    //if(IMG_Name(img).find("libnss_nis.so") != string::npos){
    //if(IMG_Name(img).find("libnss_files.so") != string::npos){
    //    image_routine_dump(img);
    //}

    gettimeofday(&end, NULL);
    time = (1000000 * (end.tv_sec - start.tv_sec))
           + (end.tv_usec - start.tv_usec);

    // the delay of these print statements aren't captured
    PRINTF_TIME("load time for %s (us): %ld\n", IMG_Name(img).c_str(), time);
}


int main(int argc, CHAR *argv[])
{
    long time;
    struct timeval start;
    struct timeval end;

    gettimeofday(&start, NULL);

    PIN_InitSymbols();

#ifndef BLANKIT_PROBE_DO_NOTHING
    if(const char *app_name = getenv("BLANKIT_APP_NAME")){
        printf("BLANKIT_APP_NAME: %s\n", app_name);
        strcpy(BLANKIT_APP_NAME, app_name);
    }else{
        printf("\nERROR: Must supply BLANKIT_APP_NAME env variable\n");
        exit(1);
    }
    if(const char *approved_list = getenv("BLANKIT_APPROVED_LIST")){
        printf("BLANKIT_APPROVED_LIST: %s\n", approved_list);
        strcpy(BLANKIT_APPROVED_LIST, approved_list);
    }else{
        printf("\nERROR: Must supply BLANKIT_APPROVED_LIST env variable\n");
        exit(1);
    }

    //
    // spec 2006
    //
    if(const char *approved_list_libm = getenv("BLANKIT_APPROVED_LIST_LIBM")){
        printf("BLANKIT_APPROVED_LIST_LIBM: %s\n", approved_list_libm);
        strcpy(BLANKIT_APPROVED_LIST_LIBM, approved_list_libm);
    }else{
        printf("\nERROR: Must supply BLANKIT_APPROVED_LIST_LIBM env variable\n");
        exit(1);
    }
    if(const char *approved_list_libgcc = getenv("BLANKIT_APPROVED_LIST_LIBGCC")){
        printf("BLANKIT_APPROVED_LIST_LIBGCC: %s\n", approved_list_libgcc);
        strcpy(BLANKIT_APPROVED_LIST_LIBGCC, approved_list_libgcc);
    }else{
        printf("\nERROR: Must supply BLANKIT_APPROVED_LIST_LIBGCC env variable\n");
        exit(1);
    }
    if(const char *approved_list_libstdcpp = getenv("BLANKIT_APPROVED_LIST_LIBSTDCPP")){
        printf("BLANKIT_APPROVED_LIST_LIBSTDCPP: %s\n", approved_list_libstdcpp);
        strcpy(BLANKIT_APPROVED_LIST_LIBSTDCPP, approved_list_libstdcpp);
    }else{
        printf("\nERROR: Must supply BLANKIT_APPROVED_LIST_LIBSTDCPP env variable\n");
        exit(1);
    }
    if(const char *blankit_predict_sets = getenv("BLANKIT_PREDICT_SETS")){
        printf("BLANKIT_PREDICT_SETS: %s\n", blankit_predict_sets);
        strcpy(BLANKIT_PREDICT_SETS, blankit_predict_sets);
    }else{
        printf("\nERROR: Must supply BLANKIT_PREDICT_SETS env variable\n");
        exit(1);
    }


    //
    // nginx
    //
    /*if(const char *approved_list_libdl = getenv("BLANKIT_APPROVED_LIST_LIBDL")){
        printf("BLANKIT_APPROVED_LIST_LIBDL: %s\n", approved_list_libdl);
        strcpy(BLANKIT_APPROVED_LIST_LIBDL, approved_list_libdl);
    }else{
        printf("\nERROR: Must supply BLANKIT_APPROVED_LIST_LIBDL env variable\n");
        exit(1);
    }
    if(const char *approved_list_libpthread = getenv("BLANKIT_APPROVED_LIST_LIBPTHREAD")){
        printf("BLANKIT_APPROVED_LIST_LIBPTHREAD: %s\n", approved_list_libpthread);
        strcpy(BLANKIT_APPROVED_LIST_LIBPTHREAD, approved_list_libpthread);
    }else{
        printf("\nERROR: Must supply BLANKIT_APPROVED_LIST_LIBPTHREAD env variable\n");
        exit(1);
    }
    if(const char *approved_list_libcrypt = getenv("BLANKIT_APPROVED_LIST_LIBCRYPT")){
        printf("BLANKIT_APPROVED_LIST_LIBCRYPT: %s\n", approved_list_libcrypt);
        strcpy(BLANKIT_APPROVED_LIST_LIBCRYPT, approved_list_libcrypt);
    }else{
        printf("\nERROR: Must supply BLANKIT_APPROVED_LIST_LIBCRYPT env variable\n");
        exit(1);
    }
    if(const char *approved_list_libpcre = getenv("BLANKIT_APPROVED_LIST_LIBPCRE")){
        printf("BLANKIT_APPROVED_LIST_LIBPCRE: %s\n", approved_list_libpcre);
        strcpy(BLANKIT_APPROVED_LIST_LIBPCRE, approved_list_libpcre);
    }else{
        printf("\nERROR: Must supply BLANKIT_APPROVED_LIST_LIBPCRE env variable\n");
        exit(1);
    }
    if(const char *approved_list_libz = getenv("BLANKIT_APPROVED_LIST_LIBZ")){
        printf("BLANKIT_APPROVED_LIST_LIBZ: %s\n", approved_list_libz);
        strcpy(BLANKIT_APPROVED_LIST_LIBZ, approved_list_libz);
    }else{
        printf("\nERROR: Must supply BLANKIT_APPROVED_LIST_LIBZ env variable\n");
        exit(1);
    }*/


    //
    // sshd
    //
    /*if(const char *approved_list_libcrypto = getenv("BLANKIT_APPROVED_LIST_LIBCRYPTO")){
        printf("BLANKIT_APPROVED_LIST_LIBCRYPTO: %s\n", approved_list_libcrypto);
        strcpy(BLANKIT_APPROVED_LIST_LIBCRYPTO, approved_list_libcrypto);
    }else{
        printf("\nERROR: Must supply BLANKIT_APPROVED_LIST_LIBCRYPTO env variable\n");
        exit(1);
    }
    if(const char *approved_list_libdl = getenv("BLANKIT_APPROVED_LIST_LIBDL")){
        printf("BLANKIT_APPROVED_LIST_LIBDL: %s\n", approved_list_libdl);
        strcpy(BLANKIT_APPROVED_LIST_LIBDL, approved_list_libdl);
    }else{
        printf("\nERROR: Must supply BLANKIT_APPROVED_LIST_LIBDL env variable\n");
        exit(1);
    }
    if(const char *approved_list_libutil = getenv("BLANKIT_APPROVED_LIST_LIBUTIL")){
        printf("BLANKIT_APPROVED_LIST_LIBUTIL: %s\n", approved_list_libutil);
        strcpy(BLANKIT_APPROVED_LIST_LIBUTIL, approved_list_libutil);
    }else{
        printf("\nERROR: Must supply BLANKIT_APPROVED_LIST_LIBUTIL env variable\n");
        exit(1);
    }
    if(const char *approved_list_libz = getenv("BLANKIT_APPROVED_LIST_LIBZ")){
        printf("BLANKIT_APPROVED_LIST_LIBZ: %s\n", approved_list_libz);
        strcpy(BLANKIT_APPROVED_LIST_LIBZ, approved_list_libz);
    }else{
        printf("\nERROR: Must supply BLANKIT_APPROVED_LIST_LIBZ env variable\n");
        exit(1);
    }
    if(const char *approved_list_libcrypt = getenv("BLANKIT_APPROVED_LIST_LIBCRYPT")){
        printf("BLANKIT_APPROVED_LIST_LIBCRYPT: %s\n", approved_list_libcrypt);
        strcpy(BLANKIT_APPROVED_LIST_LIBCRYPT, approved_list_libcrypt);
    }else{
        printf("\nERROR: Must supply BLANKIT_APPROVED_LIST_LIBCRYPT env variable\n");
        exit(1);
    }
    if(const char *approved_list_libresolv = getenv("BLANKIT_APPROVED_LIST_LIBRESOLV")){
        printf("BLANKIT_APPROVED_LIST_LIBRESOLV: %s\n", approved_list_libresolv);
        strcpy(BLANKIT_APPROVED_LIST_LIBRESOLV, approved_list_libresolv);
    }else{
        printf("\nERROR: Must supply BLANKIT_APPROVED_LIST_LIBRESOLV env variable\n");
        exit(1);
    }
    if(const char *approved_list_libssh = getenv("BLANKIT_APPROVED_LIST_LIBSSH")){
        printf("BLANKIT_APPROVED_LIST_LIBSSH: %s\n", approved_list_libssh);
        strcpy(BLANKIT_APPROVED_LIST_LIBSSH, approved_list_libssh);
    }else{
        printf("\nERROR: Must supply BLANKIT_APPROVED_LIST_LIBSSH env variable\n");
        exit(1);
    }
    if(const char *approved_list_libnss_compat = getenv("BLANKIT_APPROVED_LIST_LIBNSS_COMPAT")){
        printf("BLANKIT_APPROVED_LIST_LIBNSS_COMPAT: %s\n", approved_list_libnss_compat);
        strcpy(BLANKIT_APPROVED_LIST_LIBNSS_COMPAT, approved_list_libnss_compat);
    }else{
        printf("\nERROR: Must supply BLANKIT_APPROVED_LIST_LIBNSS_COMPAT env variable\n");
        exit(1);
    }
    if(const char *approved_list_libnsl = getenv("BLANKIT_APPROVED_LIST_LIBNSL")){
        printf("BLANKIT_APPROVED_LIST_LIBNSL: %s\n", approved_list_libnsl);
        strcpy(BLANKIT_APPROVED_LIST_LIBNSL, approved_list_libnsl);
    }else{
        printf("\nERROR: Must supply BLANKIT_APPROVED_LIST_LIBNSL env variable\n");
        exit(1);
    }
    if(const char *approved_list_libnss_nis = getenv("BLANKIT_APPROVED_LIST_LIBNSS_NIS")){
        printf("BLANKIT_APPROVED_LIST_LIBNSS_NIS: %s\n", approved_list_libnss_nis);
        strcpy(BLANKIT_APPROVED_LIST_LIBNSS_NIS, approved_list_libnss_nis);
    }else{
        printf("\nERROR: Must supply BLANKIT_APPROVED_LIST_LIBNSS_NIS env variable\n");
        exit(1);
    }
    if(const char *approved_list_libnss_files = getenv("BLANKIT_APPROVED_LIST_LIBNSS_FILES")){
        printf("BLANKIT_APPROVED_LIST_LIBNSS_FILES: %s\n", approved_list_libnss_files);
        strcpy(BLANKIT_APPROVED_LIST_LIBNSS_FILES, approved_list_libnss_files);
    }else{
        printf("\nERROR: Must supply BLANKIT_APPROVED_LIST_LIBNSS_FILES env variable\n");
        exit(1);
    }*/


    if(const char *blankit_trace = getenv("BLANKIT_TRACE")){
        printf("BLANKIT_TRACE: %s\n", blankit_trace);
        BLANKIT_TRACE = 1;
    }
    if(const char *blankit_lazy = getenv("BLANKIT_LAZY")){
        printf("BLANKIT_LAZY: %s\n", blankit_lazy);
        BLANKIT_LAZY = 1;
    }

    init_statically_reachable_funcs();
#ifdef BLANKIT_PROBE_PREDICT

    // spec
    //init_pred_sets();

    // nginx, sshd
    init_pred_sets_stub();

#endif
#endif

    //blankit_log = fopen("blankit.log", "w");

    if( PIN_Init(argc,argv) )
    {
        return Usage();
    }

    ofstream TraceFile;
    TraceFile.open(KnobOutputFile.Value().c_str());
    TraceFile << hex;
    TraceFile.setf(ios::showbase);

#ifndef BLANKIT_PROBE_DO_NOTHING
    IMG_AddInstrumentFunction(ImageLoad, 0);

    page_size = sysconf(_SC_PAGE_SIZE);
    if(page_size == -1){
        PRINTF_FLUSH("ERROR: Unable to get system page size\n");
        return 1;
    }
#endif


    gettimeofday(&end, NULL);
    time = (1000000 * (end.tv_sec - start.tv_sec))
           + (end.tv_usec - start.tv_usec);

    // the delay of these print statements aren't captured
    PRINTF_TIME("pin main time (us): %ld\n", time);

#ifdef BLANKIT_AUDIT
    printf("audit-start-time: %ld\n", timestamp_ns());
    fflush(0);
#endif

    PIN_StartProgramProbed();

    return 0;
}
