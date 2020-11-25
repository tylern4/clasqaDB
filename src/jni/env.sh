#!/bin/bash

# location of `jni.h`
export JNI_HEADER_PATH="${JAVA_HOME}/include"

# location of `jni_md.h`
export JNI_MD_HEADER_PATH="${JAVA_HOME}/include/linux"

# location of `libjvm.so`
export JVM_LIB_PATH="${JAVA_HOME}/jre/lib/amd64/server"


export LD_LIBRARY_PATH=$(echo ${JVM_LIB_PATH}:${LD_LIBRARY_PATH} | sed 's/:$//g')
export CLASSPATH=$(echo $(pwd)/\*:${CLASSPATH} | sed 's/:$//g')
