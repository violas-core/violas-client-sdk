#include <iostream>
#include "diem_framework.hpp"

using namespace std;
namespace df = diem_framework;
namespace dt = diem_types;

void test();

int main(int argc, char *argv[])
{
    using namespace diem_framework;
    using namespace diem_types;

    // AccountAddress address{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    StructTag tag{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, Identifier{""}, Identifier{""}, {}};

    // TypeTag::Struct {tag};

    auto script = encode_add_currency_to_account_script({TypeTag::Struct{tag}});

    test();

    return 0;
}

void test()
{
   
}

