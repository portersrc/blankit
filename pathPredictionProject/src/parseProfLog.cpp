#include <string>
#include <fstream>
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#define windowSize 5
class path_prefix_window_class {
  std::string pathPrefix_str[windowSize];
  public:
  path_prefix_window_class(){
    for (int i =0 ; i < windowSize ; i++) 
      pathPrefix_str[i] = "-1";
  }
  std::string get(){
    std::string ret_str; 
    for (int i=0 ; i < windowSize ; i++) {
      ret_str = ret_str +"_"+ pathPrefix_str[i];
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
  void insert( std::string bb_str){
    //first left shift
    for (int i=0;i<windowSize-1;i++) {
      pathPrefix_str[i]=pathPrefix_str[i +1];
    }
    //then push back the latest bb
    pathPrefix_str[windowSize-1] = bb_str;
  }
};
int main(int argc, char *argv[] ) { 
  std::string filename(argv[1]);
  std::ifstream infile(filename.c_str());
  std::string line;
  std::vector<std::string > funcName_vector;
  std::vector<std::string > pathPrefix_vector;
  path_prefix_window_class pathPrefix_obj;
  unsigned int windowIndex = 0;
  while (std::getline(infile, line))
  {   
    if (line.length() == 0) continue;
    //std::istringstream iss(line);
    if (line.find("#") != 0 ) continue;
    if (line.find("#Func") != std::string::npos && (line.find(":Done:") != std::string::npos)) {
      std::string afterFunc = line.substr(6 );
      std::string funcName = afterFunc.substr(0,afterFunc.find(':') );
      //std::cout<<"\n func name : "<<funcName;
      funcName_vector.push_back(funcName);
      pathPrefix_vector.push_back( (pathPrefix_obj.get()));
    } 
    else if (line.find("#_control_dep_BB") != std::string::npos) {
      std::string afterTag = line.substr(17 );
      std::string rdfBB     = afterTag.substr(0,afterTag.find(":"));
      //std::cout<<"\n RDF BB  : "<<rdfBB   ;
      pathPrefix_obj.insert(rdfBB);
      //pathPrefix_str[windowIndex] = rdfBB;
      windowIndex = (windowIndex + 1) %windowSize;
//pathPrefix_str = pathPrefix_str+ "_"+  rdfBB;
    }
    else if (line.find("#_control_dep_succBB") != std::string::npos) {
      std::string rdfBB     = line.substr(20 );
      //std::cout<<"\n SUCC RDF BB  : "<<rdfBB   ;
    }
  }
  for (unsigned int i = 0 ; i < funcName_vector.size() ; i++ ){
    std::cout<<"\nfunc::"<<funcName_vector[i]<<", ";
    std::cout<<""<<pathPrefix_vector[i];
  }

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

