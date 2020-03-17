SUBDIRS	= src
   
.PHONY: subdirs $(SUBDIRS) clean android

subdirs: $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@

android:
	@for dir in ${SUBDIRS}; do \
		${MAKE} -C $$dir android; \
	done

java:
	@for dir in ${SUBDIRS}; do \
		${MAKE} -C $$dir java; \
	done

clean: 
	@for dir in ${SUBDIRS}; do \
		${MAKE} -C $$dir clean; \
	done

# CXXFLAGS= -wall -werror -std=c++17
# LDFLAGS=
# LIBS=
# BIN= lib

# TARGET= libcppSdk.a
# SRC= src
# OBJS= violas_sdk.o

# all : $(TARGET)

# $(TARGET) : $(OBJS)
# 	$(AR) -crv $(BIN)/$(TARGET) $(OBJS)

# violas_sdk.o : src/violas_sdk.cpp
#  	$(CXX) $(CXXFLAGS) -c src/violas_sdk.cpp -o violas_sdk.o

# %.o : $(SRC)/(%.cpp) $(SRC)/%.h $(SRC)/%.hpp
# 	$(CXX) $(CXXFLAGS) -c $(SRC)/$< -o $@


# libcppSdk : violas_sdk.o
# 	$(AR) -crv lib/libcppSdk.a violas_sdk.o

# android : violas_sdk.o jni_violas_sdk.o
# 	$(AR) -crv lib/libcppSdk-arm64v8a.a violas_sdk.o jni_violas_sdk.o

# java : violas_sdk.o jni_violas_sdk.o
# 	$(AR) -crv lib/libcppSdk.a violas_sdk.o jni_violas_sdk.o

# violas_sdk.o :
# 	$(CXX) -o2 -std=c++17 \
# 	 \
# 	-c src/violas_sdk.cpp \
# 	-o violas_sdk.o

# jni_violas_sdk.o :
# 	$(CXX) -o2 -std=c++17 \
# 	-D $(CXXFLAGS) \
# 	-c src/jni_violas_sdk.cpp \
# 	-o jni_violas_sdk.o

# clean :
# 	rm -f $(TARGET) : $(OBJS)