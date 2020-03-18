export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/hunter/Projects/work/ViolasClientSdk/Java/src/jni

cd test;

java --module-path /usr/share/openjfx/lib/ --add-modules=javafx.base -cp ../bin/violas.jar Demo.java