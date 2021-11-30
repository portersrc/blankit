#include <stdio.h>
#define WINDOW_SIZE 5
int circular_q_window_RDF[WINDOW_SIZE];
//int circular_q_pos = 0;
char first_time = 1;
void initialize_window_RDF(){
  for (int i=0;i<WINDOW_SIZE; i++) {
    circular_q_window_RDF[i] = 0;
  }
}
void insert_window_RDF(int rdfID ) {
  if (first_time) initialize_window_RDF();
  for (int i=1; i<WINDOW_SIZE; i++ ){
    circular_q_window_RDF[i] = circular_q_window_RDF[i-1];
  }
  circular_q_window_RDF[0] = rdfID;
}
int  pbarua_custom_instr_set_rdf(int rdfID ) {
  //insert_window_RDF(rdfID);
  return rdfID;
}
int pbarua_custom_instr_return_callsite(int arg1) {
  return (int) (arg1);
}
int pbarua_custom_instr_callsite_0(int arg1, int func) {
  return (int) (arg1);
}
int pbarua_custom_instr_callsite_1(int arg1, int func, int arg2) {
  return (int) (arg1+arg2);
}
int pbarua_custom_instr_callsite_2(int arg1, int func, int arg2, int arg3) {
  return (int) (arg1+arg2);
}
int pbarua_custom_instr_callsite_3(int arg1, int func, int arg2, int arg3,int arg4) {
  return (int) (arg1+arg4);
}
int pbarua_custom_instr_callsite_4(int arg1, int func, int arg2, int arg3,int arg4, int arg5) {
  return (int) (arg1+arg4+arg5);
}
int pbarua_custom_instr_callsite_5(int arg1, int func, int arg2, int arg3,int arg4, int arg5, int arg6) {
  return (int) (arg1+arg4+arg5);
}
int  pbarua_custom_instr_fun_RDF(int rdfID, int callID, int argID) {
  return rdfID+callID+argID;
}
int pbarua_custom_instr_fun (const char * print_msg) {
  return (int) (print_msg[0]);
      
    //printf("custom %s", print_msg);
}
int pbarua_custom_instr_fun_2 (const char * print_msg, int arg1) {
  //setup the path prefix, and call the python script to predict the callchain
  //now we have to co-relate the path prefix with the function args, 
  //consider only the arguments 
  return (int) (print_msg[0]);
      
    //printf("custom %s", print_msg);
}
int pbarua_custom_instr_fun_3(const char * print_msg, int arg1, int arg2) { 
  return (int) (print_msg[0]);
}
int pbarua_custom_instr_fun_4(const char * print_msg, int arg1, int arg2, int arg3) { 
  return (int) (print_msg[0]);
}
int pbarua_custom_instr_fun_5(const char * print_msg, int arg1, int arg2, int arg3, int arg4) { 
  return (int) (print_msg[0]);
}
int pbarua_custom_instr_fun_6(const char * print_msg, int arg1, int arg2, int arg3, int arg4, int arg5) { 
  return (int) (print_msg[0]);
}
int pbarua_custom_instr_fun_2_float (const char * print_msg, float arg1) {
  //setup the path prefix, and call the python script to predict the callchain
  //now we have to co-relate the path prefix with the function args, 
  //consider only the arguments 
  return (int) (print_msg[0]);
      
    //printf("custom %s", print_msg);
}
void
__cyg_profile_func_enter (void *func,  void *caller)
{
 printf( "e %p %p \n", func, caller );
}
 
void
__cyg_profile_func_exit (void *func, void *caller)
{
 printf( "x %p %p \n", func, caller);
}
//int pbarua_custom_instr_fun (const char * print_msg, int int valPrint) {
//  return printf("custom %s =%u", print_msg, valPrint);
//}
