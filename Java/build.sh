echo "clean folder bin ..."
rm -rf bin/*
# 
cd src
# 
export SRC_PATH='io/violas/sdk/java'
echo $SRC_PATH
#
echo "generate jni header files ..."
javac -h jni $SRC_PATH/Client.java  --module-path /usr/share/openjfx/lib --add-modules=javafx.base
javac -h jni $SRC_PATH/Token.java  --module-path /usr/share/openjfx/lib --add-modules=javafx.base
#
echo "compile ..."
javac io/violas/sdk/java/Client.java --module-path /usr/share/openjfx/lib --add-modules=javafx.base
javac io/violas/sdk/java/Token.java --module-path /usr/share/openjfx/lib --add-modules=javafx.base
#
echo "create jar ..."
jar -cvf ../bin/violas.jar *
#
cd ..
