rm -f cpp_closures_d.o cpp_closures.o

find  ../../rust/violas-client/target/debug/ -name cpp_closures.o | xargs -i cp {} cpp_closures_d.o
find  ../../rust/violas-client/target/release/ -name cpp_closures.o | xargs -i cp {} .
