# use JNI to access QADB.groovy from C++

- setup
  - first, make sure you've sourced `../../env.sh`. In particular, `$JYPATH` must
    contain `clasqa/QADB.groovy`
  - next, make sure you have the JNI header files; they are likely somewhere in
    `$JAVA_HOME`, in the `include` subdirectory
  - edit the local `env.sh` file so that `JNI_HEADER_PATH`, `JNI_MD_HEADER_PATH`
    includes `jni.h` and `jni_md.h`, respectively; also make sure that
    `JVM_LIB_PATH` points to the directory that contains `libjvm.so`; the last
    character of these file paths should not be `/`
  - you may need to install the Groovy compiler `groovyc`, as well as `gradle`

- procedure
  - build QADB class with `gradle build`; this will populate the `build`
    subdirectory
  - compile the C++ test code by running `make`
  - execute `main.exe`
    - here I had problems: `java.lang.NoClassDefFoundError: groovy/lang/GroovyObject`; I managed to fix this by unjarring a copy
      of groovy-all.jar from the Maven central repository, to the subdirectory `unjar`, then adding `unjar` to 
      `java.class.path` in `main.cpp`; hopefully this can be done by gradle instead
