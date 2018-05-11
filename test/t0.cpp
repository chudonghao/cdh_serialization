

#include <cdh/serialization/serialization.hpp>
#include <libxml/debugXML.h>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/timer.hpp>
#include <cdh/serialization/xml_deserializer.hpp>

namespace test {

class b_t {
public:
    int i;
};

class a_t {
public:
    a_t() : name("a_t"), i(0), j(3.1f), b1(), b2_p(nullptr), ints() {}

    std::string name;
    int i;
    float j;
    std::vector<int> ints;
    b_t b1;
    b_t *b2_p;
};

class c_t : public a_t {
public:
    c_t() : a_t(), k(0) {}

    int k;
};

class d_t {
public:

    int i;

    virtual ~d_t() {}
};

class e_t : public d_t {
public:
    int j;
};

}


namespace cdh {
namespace serialization {

using namespace std;
using namespace test;
using namespace cdh::serialization;

template<typename Processer>
void process(Processer &pc, test::b_t &v) {
    pc & make_nvp("i", v.i);
}

template<typename Processer>
void process(Processer &pc, test::a_t &v) {
    pc & make_nvp("name", v.name);
    pc & make_nvp("i", v.i);
    pc & make_nvp("j", v.j);
    pc & make_nvp("ints", v.ints);
    pc & make_nvp("b2_p", v.b2_p);
    pc & make_nvp("b1", v.b1);
}

template<typename Processer>
void process(Processer &pc, test::c_t &v) {
    pc & make_nvp("a_t", base_object<a_t>(v));
    pc & make_nvp("k", v.k);
}

template<typename Processer>
void process(Processer &pc, test::d_t &v) {
    pc & make_nvp("i", v.i);
}

template<typename Processer>
void process(Processer &pc, test::e_t &v) {
    pc & make_nvp("d_t", base_object<d_t>(v));
    pc & make_nvp("j", v.j);
}

}
}

namespace boost {
namespace serialization {

template<typename Processer>
void serialize(Processer &pc, test::b_t &v, const unsigned int version) {
    pc & make_nvp("i", v.i);
}

template<typename Processer>
void serialize(Processer &pc, test::a_t &v, const unsigned int version) {
    pc & make_nvp("name", v.name);
    pc & make_nvp("i", v.i);
    pc & make_nvp("j", v.j);
    pc & make_nvp("ints", v.ints);
    pc & make_nvp("b1", v.b1);
    pc & make_nvp("b2", v.b2_p);
}

template<typename Processer>
void serialize(Processer &pc, test::c_t &v, const unsigned int version) {
    pc & make_nvp("a_t", base_object<test::a_t>(v));
    pc & make_nvp("k", v.k);
}

template<typename Processer>
void serialize(Processer &pc, test::d_t &v, const unsigned int version) {
    pc & make_nvp("i", v.i);
}

template<typename Processer>
void serialize(Processer &pc, test::e_t &v, const unsigned int version) {
    pc & make_nvp("d_t", base_object<test::d_t>(v));
    pc & make_nvp("j", v.j);
}

}
}

namespace {
using namespace std;
using namespace test;

}


int main() {

    cdh::serialization::singleton<
            cdh::serialization::process_funcs<
                    cdh::serialization::xml_deserializer<
                            cdh::serialization::xml_deserializer_type::normal>>>
    ::instance().enable_rtti<e_t>();
    cdh::serialization::singleton<
            cdh::serialization::process_funcs<
                    cdh::serialization::xml_serializer>>
    ::instance().enable_rtti<e_t>();

    c_t c;
    c.ints.push_back(1);
    c.b2_p = &c.b1;
    c.k = 1001;

    boost::timer t;
    double t1 = t.elapsed();

    for (int i = 0; i < 1; ++i) {
        ostringstream oss;
        cdh::serialization::xml_serializer xs;
        xs.serialize("c", c);
        oss << xs.data();
    }
    double t2 = t.elapsed();

    //for (int j = 0; j < 1; ++j) {
    //    ostringstream oss;
    //    boost::archive::xml_oarchive xo(oss);
    //    xo << boost::serialization::make_nvp("c", c);
    //}
    double t3 = t.elapsed();

    cout << t2 - t1 << " " << t3 - t2 << endl;

    for (int k = 0; k < 1; ++k) {

        //ofstream ofs("test.xml");
        //cdh::serialization::xml_serializer xs;
        //xs.serialize("c", c);
        //ofs << xs.xml_buffer()->content;
        //ofs.flush();

        FILE *f = fopen("test.xml", "r");
        fseek(f, 0, SEEK_END);
        auto size = (size_t) ftell(f);
        fseek(f, 0, SEEK_SET);
        char buf[1024] = {};
        fread((void *) buf, 1, size, f);

        c_t tmp;

        cdh::serialization::xml_deserializer<cdh::serialization::normal> xd;
        xd.data(reinterpret_cast<const char *>(buf), size);
        xd.deserialize("c", tmp);
        std::cout << endl;

        cdh::serialization::xml_serializer xs;
        xs.serialize("c", tmp);
        std::cout << xs.data();

        tmp = c_t();
        xd.data(xs.data(),xs.size());
        xd.deserialize("c",tmp);

        //e_t e;
        //xs.serialize("d", e);
        //std::cout << xs.data();

    }

    return 0;
}
