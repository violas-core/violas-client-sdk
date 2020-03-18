#all flags
CXXFLAGS= -Wall -Werror -std=c++17 -fPIC
LDFLAGS=
LIBS= -lc++std-fs
BIN= ../lib
# all target
TARGET= libcppSdk
STATIC_TARGET= $(TARGET).a
SHARED_TARGE= $(TARGET).so
JAVA_TARGET = $(TARGET).a
ANDROID_TARGET= $(TARGET)-arm64v8a.a 
# all object files
OBJS= violas_sdk.o
ANDROID_OBJS= $(OBJS) jni_violas_sdk.o

# default target
all : $(STATIC_TARGET)

# build target for static lib
$(STATIC_TARGET) : $(OBJS)
	$(AR) $(ARFLAGS) $(BIN)/$@ $^

# target for for shared lib
# use 'make libcppSdk.so' to build it  
$(SHARED_TARGE) : CXXFLAGS+= -fPIC
$(SHARED_TARGE) : LDFLAGS+= -shared
$(SHARED_TARGE) : $(OBJS) 	
	$(CXX) $(LDFLAGS) -o $(BIN)/$@ $(OBJS);

# target for Android
$(ANDROID_TARGET) : $(ANDROID_OBJS)
	$(AR) $(ARFLAGS) $(BIN)/$@ $^

$(JAVA_TARGET) : $(ANDROID_OBJS)
	$(AR) $(ARFLAGS) $(BIN)/$@ $^

# build for all object files
%.o : %.cpp %.h %.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# for build andorid
android : CXXFLAGS+= -DANDROID
android : $(ANDROID_TARGET)

java : CXXFLAGS+= -g -I/usr/lib/jvm/java-11-openjdk-amd64/include -I/usr/lib/jvm/java-11-openjdk-amd64/include/linux/
java : $(JAVA_TARGET)

#for build with debug
debug : CXXFLAGS+= -g
debug : all

# for build with release
release : CXXFLAGS+= -O3
release : all

clean :
	$(RM) $(BIN)/$(STATIC_TARGET) $(BIN)/$(SHARED_TARGE) $(BIN)/$(ANDROID_TARGET) $(ANDROID_OBJS) 