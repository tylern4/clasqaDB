#include <jni.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

int main() {
  JavaVM * jvm;
  JNIEnv * env;
  JavaVMInitArgs vm_args; // Initialization arguments
  JavaVMOption * options = new JavaVMOption[1]; // JVM invocation options
  //options[0].optionString = "-Djava.class.path=/home/dilks/j/dm/clasqaDB/src/jni/*";
  options[0].optionString = "-Djava.class.path=unjar:build/classes/groovy/main";
  //options[0].optionString = "-Djava.class.path=.";
  options[1].optionString = "-verbose:jni";
  vm_args.nOptions = 2; // number of options
  vm_args.version = JNI_VERSION_1_6; // minimum Java version
  vm_args.options = options;
  vm_args.ignoreUnrecognized = false; // invalid options make the JVM init fail
  //=============== load and initialize Java VM and JNI interface =============
  jint rc = JNI_CreateJavaVM(&jvm, (void**)&env, &vm_args);  // YES !!
  delete options;    // we then no longer need the initialisation options. 
  if (rc != JNI_OK) {
    // TO DO: error processing... 
    cin.get();
    exit(EXIT_FAILURE);
  }
  //=============== Display JVM version =======================================
  cout << "JVM load succeeded: Version ";
  jint ver = env->GetVersion();
  cout << ((ver>>16)&0x0f) << "."<<(ver&0x0f) << endl;

  //===================================================

  // load QADB class
  cout << "ATTEMPT TO FIND QADB CLASS" << endl;
  jclass QADBclass = env->FindClass("clasqa/QADB");
  if(QADBclass==nullptr) {
    cerr << "ERROR: cannot find QADB.class" << endl;

    if( env->ExceptionOccurred() )
      env->ExceptionDescribe();
    else
      cout << "QADBclass is null but no exception was thrown."  << endl;
    return 0;
  } else {
    cout << "successfully loaded QADB.class" << endl;
  };

  // QADB constructor
  jmethodID QADBconstructor = env->GetMethodID(QADBclass,"<init>","()V");
  if(QADBconstructor==nullptr) {
    cerr << "ERROR: cannot find QADB constructor" << endl;
    return 0;
  } else cout << "constructor found" << endl;
  jobject QADBobj = env->NewObject(QADBclass,QADBconstructor);


  // query run 5683 file 115 with event number 12277156
  jmethodID QADB_query = env->GetMethodID(QADBclass,"query","(II)Z");
  if(QADB_query==nullptr) {
    cerr << "ERROR: cannot find QADB_query" << endl;
    return 0;
  } else cout << "QADB_query found" << endl;
  
  jboolean QuerySucceeded = env->CallBooleanMethod(QADBobj,QADB_query,(jint)5683,(jint)12277156);
  printf("query found event ? %s\n",QuerySucceeded?"yep":"nope");

  return 1;
};
