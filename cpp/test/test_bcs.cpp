#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <tuple>
#include <unordered_set>
#include <array>
#include <optional>
#include <sstream>
#include <cassert>
#include <serde_bcs.hpp>

using namespace std;

void test_serialization();

struct Employer
{
    string name;
    uint16_t age;
    bool gender;
    vector<uint8_t> courses;
    map<int, string> score;

    BcsSerde &serde(BcsSerde &bs)
    {
        return bs && name && age && gender && courses && score;
        //bs << name << age << gender << nums;
        //bs >> name >> age >> gender >> nums;
        //return bs;
    }
};
bool operator<(const Employer &l, const Employer &r)
{
    return l.name < r.name;
}

ostream &operator<<(ostream &os, const Employer &e)
{
    os << "name:" << e.name << ", age:" << e.age << ", gender:" << e.gender;

    {
        cout << ", courses : "
             << "{";

        for (const auto v : e.courses)
            cout << (short)v << ",";

        if (!e.courses.empty())
            cout << '\b';

        cout << "}";
    }

    return os;
}

struct Company
{
    string name;
    tuple<int16_t, string> libray_record;
    set<Employer> employers;

    BcsSerde &serde(BcsSerde &bs)
    {
        return bs && name && libray_record && employers;
    }
};

template <class P>
concept check_serde = requires(P p)
{
    std::same_as<decltype(p.serde(declval<BcsSerde &>())), BcsSerde &>;
};

void test_serialization()
{
    BcsSerde bs;
    Employer e;
    Company Dell;
    Employer Tom = {"Tom", 20, true, {1, 2, 3}, {{0, "English"}}};

    //tom.serde(bs);
    //bs &&tom;
    Dell.name = "dell";
    Dell.libray_record = make_tuple(128, "Mathematics");
    Dell.employers.insert(Tom);

    bs &&Dell &&Tom;
    Dell.serde(bs);

    cout << bs.dump() << endl;

    //swich to deserialization mode
    bs.set_des();

    bs &&Dell &&e;

    cout << e << endl;

    // Dell.name = "dell";
    // Dell.employers.insert(tom);

    // Dell.serde(bs);

    // constexpr bool has_serde_t = requires()
    // {
    //     declval<Employer>().serde();
    // };

    if constexpr (check_serde<Employer>)
    {
        cout << "Employer is Fooable" << endl;
    }

    // vector<uint8_t> output;
    // ofstream ofs("b.data", ios::binary);

    // int a = 0x1234;

    // serialize(back_insert_iterator(output), a);

    // serialize(ostreambuf_iterator<char>(ofs), a);

    // //copy(begin(output), end(output), ostreambuf_iterator<uint8_t>(cout));
    // for (auto i : output)
    // {
    //     //ostringstream oss;

    //     cout << hex << (short)i << " ";

    //     //cout << oss.str() << " ";
    // }

    // int b = 0;

    // deserialize(begin(output), b);

    // assert(a == b);

    // vector<int> v = {1, 2, 3};

    // serialize(begin(output), begin(v), end(v));

    // deserialize(begin(output), v);
}

int main(int argc, char *argv[])
{
    // Employer e{"Tom", 20, true};

    // cout << e << endl;

    // ofstream ofs("a.dat", ios::binary);

    // vector<uint8_t> v = {1, 2, 3, 4, 5};

    // serialize(ofs, v);

    // map<int, string> names = {{0, "Tome"}, {1, "Jerry"}, {2, "Jim"}};
    // serialize(ofs, names);

    // for (auto &v : names)
    // {
    //     cout << v.first << v.second << endl;
    // }

    // unordered_set<string> boys = {"Tom", "Jerry", "Jim"};
    // serialize(ofs, boys);

    // auto name_age = make_pair<string, int>("Tom", 12);

    // serialize(ofs, name_age);

    // auto name_gender_age = make_tuple<int8_t, string>(-1, "Diem");
    // serialize(ofs, name_gender_age);

    // map<int, tuple<string, bool, int>> tree = {{1, make_tuple<string, bool, int>("Jerry", false, 16)},
    //                                            {2, make_tuple<string, bool, int>("Jerry", false, 16)}};

    // serialize(ofs, tree);

    // array<uint8_t, 16> addr = {0x1, 0x2};
    // serialize(ofs, addr);

    // optional<uint8_t> opt_int = {8}; // nullopt;
    // serialize(ofs, opt_int);

    // // encode_integer(ofs, 128);
    // // encode_integer(ofs, 16384);
    // // encode_integer(ofs, 2097152);
    // // encode_integer(ofs, 268435456);
    // // encode_integer(ofs, 9487);

    // auto hello = "你好啊";
    // cout << hello << endl;

    // string utf8_str = "çå∞≠¢õß∂ƒ∫";
    // cout << utf8_str << endl;

    // setlocale(LC_CTYPE, "zh_CN.UTF-8");

    // std::wstring str = L"123,abc:我是谁！";
    // wcout << str << endl;

    test_serialization();

    return 0;
}
