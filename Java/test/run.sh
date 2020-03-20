export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/Projects/work/ViolasClientSdk/build/debug/lib

java --module-path /usr/share/openjfx/lib/ --add-modules=javafx.base -cp ../bin/violas.jar Demo.java