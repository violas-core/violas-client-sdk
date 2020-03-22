echo "clean folder bin ..."
rm -rf bin/*
# 
cd src/java
# 
export SRC_PATH='io/violas/sdk'
echo $SRC_PATH
#
echo "generate jni header files ..."
javac -h ../jni $SRC_PATH/Client.java  --module-path /usr/share/openjfx/lib --add-modules=javafx.base
javac -h ../jni $SRC_PATH/Token.java  --module-path /usr/share/openjfx/lib --add-modules=javafx.base
#
echo "compile ..."
javac $SRC_PATH/Client.java --module-path /usr/share/openjfx/lib --add-modules=javafx.base
javac $SRC_PATH/Token.java --module-path /usr/share/openjfx/lib --add-modules=javafx.base
#
echo "create jar ..."
jar -cvf ../../bin/violas.jar *
#
cd ..
