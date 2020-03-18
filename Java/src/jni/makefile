CXXFLAGS = 	-Wall -Werror -std=c++17 -fPIC
CXXFLAGS += -I/usr/lib/jvm/java-11-openjdk-amd64/include \
			-I/usr/lib/jvm/java-11-openjdk-amd64/include/linux/ \
			-I../../../cppSdk/src			
LDFLAGS = -shared -L../../../cppSdk/lib/ -L../../../rust-client-proxy/target/release/
LIBS = -l:libcppSdk.a -l:libclient_proxy.a -lc++std-fs

TARGET = libviolas-sdk-lib.so
OBJS = io_violas_sdk_java_Client.o io_violas_sdk_java_Token.o

all: $(TARGET)	
        
$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)
	
%.o : %.cpp %.h %.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

debug : CXXFLAGS+= -g
debug : all

release : CXXFLAGS+= -O3
release : all
	strip $(TARGET);
	cp $(TARGET) ../../bin/;
	
clean:
	$(RM) $(TARGET) $(OBJS)