Violas SDK for Java on Linux / macOS

Dependences 
    sudo apt install openjfx

generate .h files in jni folder
    javac -h jni Client.java  --module-path /usr/share/openjfx/lib --add-modules=javafx.base
    javac -h jni Token.java  --module-path /usr/share/openjfx/lib --add-modules=javafx.base

compile java files 
    javac Client.java --module-path /usr/share/openjfx/lib --add-modules=javafx.base

build jar
    jar -cvmf MANIFEST.MF app1.jar *.class
    jar -cvf ../violas.jar *
    
Run
    java --module-path /usr/share/openjfx/lib/ --add-modules=javafx.base HelloWorld