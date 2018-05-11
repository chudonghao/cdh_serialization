#ifndef CDH_SERIALIZATION_XML_SERIALIZER_H
#define CDH_SERIALIZATION_XML_SERIALIZER_H

#include <iostream>
#include <cassert>
#include <vector>
#include <list>
#include <map>
#include <cstring>
#include <fstream>
#include <sstream>
#include <functional>
#include <libxml/xmlwriter.h>

#include "nvp.hpp"
#include "serializer.hpp"
#include "type_traits.hpp"
#include "processer.hpp"
#include "singleton.hpp"
#include "memory_pointer.hpp"

namespace cdh {
namespace serialization {

class xml_serializer : public serializer {
public:
    typedef xml_serializer self_t;
private:
    xmlTextWriterPtr xml_text_writer_;
    xmlBufferPtr xml_buffer_;

    bool pretty_;
    unsigned int depth_;
    unsigned int current_object_id_;
    std::map<memory_pointer, unsigned int> ptr_id_mapping_;
    process_funcs<self_t> &type_name_serialize_func_mapping_;

    //std::map<void const*, unsigned int> ptr_id_mapping_;

    /**
     *
     */
    unsigned int recard_ptr_id_mapping(char const *type_name, void const *ptr) {
        assert(ptr != nullptr);
        //std::cout << type_name << "@" << ptr << std::endl;
        memory_pointer mp = memory_pointer(type_name, ptr);

        unsigned int &id = ptr_id_mapping_[mp];
        if (!id) {
            ++current_object_id_;
            id = current_object_id_;
        }
        return id;
    }

    /**
     *
     */
    template<typename T>
    bool has_ptr_id_mapping(T *t) {
        assert(t != nullptr);
        memory_pointer mp = memory_pointer(typeid(T).name(), (void const *) t);
        return ptr_id_mapping_.find(mp) != ptr_id_mapping_.end();
    }

public:

    /**
     * 基本类型(basic_type)直接序列化
     * @tparam T
     * @param p
     * @return
     */
    template<typename T>
    typename std::enable_if<std::is_same<T, std::string>::value, self_t &>::type
    operator&(nvp<T> p) {

        xmlTextWriterStartElement(xml_text_writer_, BAD_CAST p.name());
        xmlTextWriterWriteString(xml_text_writer_, BAD_CAST p.const_data_reference().c_str());
        xmlTextWriterEndElement(xml_text_writer_);

        //print_tab();
        //os << "<" << p.name() << ">" << p.const_data_reference() << "</" << p.name() << ">\n";
        return *this;
    }

    /**
     * 基本类型(basic_type)直接序列化
     * @tparam T
     * @param p
     * @return
     */
    template<typename T>
    typename std::enable_if<std::is_arithmetic<T>::value, self_t &>::type
    operator&(nvp<T> p) {

        xmlTextWriterStartElement(xml_text_writer_, BAD_CAST p.name());
        xmlTextWriterWriteString(xml_text_writer_, BAD_CAST std::to_string(p.const_data_reference()).c_str());
        xmlTextWriterEndElement(xml_text_writer_);
        //
        //print_tab();
        //os << "<" << p.name() << ">" << p.const_data_reference() << "</" << p.name() << ">\n";
        return *this;
    }

    /**
     * 暂时不支持
     * @tparam T
     * @param p
     * @return
     */
    template<typename T>
    typename std::enable_if<is_unsupport<T>::value, self_t &>::type
    operator&(nvp<T> p) {
        //TODO
        throw std::runtime_error(std::string("const type,arithmetic pointer and array are not support yet. !TODO"));
    }

    /**
     * 序列化一个不支持rtti的对象
     * @tparam T
     * @param p
     * @return
     */
    template<typename T>
    typename std::enable_if<is_user_class<T>::value, self_t &>::type
    operator&(nvp<T> p) {
        unsigned int id = recard_ptr_id_mapping(typeid(T).name(), &p.const_data_reference());

        xmlTextWriterStartElement(xml_text_writer_, BAD_CAST p.name());
        xmlTextWriterWriteAttribute(xml_text_writer_, BAD_CAST "id", BAD_CAST std::to_string(id).c_str());
        process(*this, p.const_data_reference());
        xmlTextWriterEndElement(xml_text_writer_);

        //print_tab();
        //os << "<" << p.name() << " id=\"" << id << "\">\n";
        //
        //++depth_;
        //process(*this, p.const_data_reference());
        //--depth_;
        //
        //
        //print_tab();
        //os << "</" << p.name() << ">\n";
        return *this;
    }

    /**
     * 序列化基类
     * @tparam T
     * @param p
     * @return
     */
    template<typename T>
    self_t &
    operator&(nvp<T, true> p) {
        unsigned int id = recard_ptr_id_mapping(typeid(T).name(), &p.const_data_reference());

        xmlTextWriterStartElement(xml_text_writer_, BAD_CAST p.name());
        xmlTextWriterWriteAttribute(xml_text_writer_, BAD_CAST "id", BAD_CAST std::to_string(id).c_str());
        process(*this, p.const_data_reference());
        xmlTextWriterEndElement(xml_text_writer_);

        //print_tab();
        //os << "<" << p.name()
        //   << " id=\"" << id
        //   << "\">\n";
        //++depth_;
        //process(*this, p.const_data_reference());
        //--depth_;
        //print_tab();
        //os << "</" << p.name() << ">\n";
        return *this;
    }

    /**
     * 序列化一个支持rtti的对象
     * @tparam T
     * @param p
     * @return
     */
    template<typename T>
    typename std::enable_if<is_user_rtti_class<T>::value, self_t &>::type
    operator&(nvp<T> p) {
        if (type_name_serialize_func_mapping_.find(typeid(p.const_data_reference()).name())
            == type_name_serialize_func_mapping_.end()) {
            //TODO
            throw std::runtime_error("type is not ");
        } else {
            char const *type_name = typeid(p.const_data_reference()).name();
            unsigned int id = recard_ptr_id_mapping(type_name, &p.const_data_reference());

            xmlTextWriterStartElement(xml_text_writer_, BAD_CAST p.name());
            xmlTextWriterWriteAttribute(xml_text_writer_, BAD_CAST "type", BAD_CAST type_name);
            xmlTextWriterWriteAttribute(xml_text_writer_, BAD_CAST "id", BAD_CAST std::to_string(id).c_str());
            type_name_serialize_func_mapping_[type_name](*this, dynamic_cast<void const *>(&p.const_data_reference()));
            xmlTextWriterEndElement(xml_text_writer_);

            //print_tab();
            //os << "<" << p.name()
            //   << " type=\"" << type_name << "\""
            //   << " id=\"" << id << "\""
            //   << ">\n";
            //++depth_;
            //type_name_serialize_func_mapping_[type_name](*this, dynamic_cast<void const *>(&p.const_data_reference()));
            //--depth_;
            //print_tab();
            //os << "</" << p.name() << ">\n";
        }

        return *this;
    }

    /**
     * 序列化一个指针
     * @tparam T
     * @param p
     * @return
     */
    template<typename T>
    typename std::enable_if<is_user_pointer<T, 1>::value, self_t &>::type
    operator&(nvp<T> p) {
        typedef typename std::remove_pointer<T>::type basic_type;
        if (p.const_data_reference() == nullptr) {
            xmlTextWriterStartElement(xml_text_writer_, BAD_CAST p.name());
            xmlTextWriterEndElement(xml_text_writer_);
            //print_tab();
            //os << "<" << p.name() << "/>\n";
            return *this;
        }
        unsigned int id = recard_ptr_id_mapping(typeid(basic_type).name(), p.const_data_reference());

        xmlTextWriterStartElement(xml_text_writer_, BAD_CAST p.name());
        xmlTextWriterWriteAttribute(xml_text_writer_, BAD_CAST "id", BAD_CAST std::to_string(id).c_str());
        xmlTextWriterEndElement(xml_text_writer_);

        //print_tab();
        //os << "<" << p.name() << " id=\"" << id << "\"/>\n";
        return *this;
    }

    explicit xml_serializer() : type_name_serialize_func_mapping_(singleton<process_funcs<self_t>>::instance()),
                                pretty_(false), depth_(0), current_object_id_(0),
                                xml_buffer_(nullptr), xml_text_writer_(nullptr) {
    }

    ~xml_serializer() {
        if (xml_text_writer_) {
            xmlFreeTextWriter(xml_text_writer_);
        }
        if (xml_buffer_) {
            xmlBufferFree(xml_buffer_);
        }
    }

    char const * data(){
        if(!xml_buffer_)
            return nullptr;
        return (const char *)(xml_buffer_->content);
    }

    unsigned int size(){
        if(!xml_buffer_)
            return 0;
        return xml_buffer_->size;
    }

    void reset_serialize() {
        current_object_id_ = 0;
        ptr_id_mapping_.clear();
        if (xml_buffer_) {
            if (xml_text_writer_) {
                xmlBufferEmpty(xml_buffer_);
            } else {
                throw std::runtime_error("");
            }
        } else {
            if (!xml_text_writer_) {
                xml_buffer_ = xmlBufferCreate();
                xml_text_writer_ = xmlNewTextWriterMemory(xml_buffer_, 0);
            } else {
                throw std::runtime_error("");
            }
        }
        //TODO
        if (!xml_buffer_) {
            throw std::runtime_error("");
        }
        if (!xml_text_writer_) {
            throw std::runtime_error("");
        }
    }

    template<typename T>
    void serialize(char const *name, T &t) {
        reset_serialize();
        xmlTextWriterStartDocument(xml_text_writer_, NULL, "utf-8", NULL);
        *this & make_nvp(name, t);
        xmlTextWriterEndDocument(xml_text_writer_);
    }

};

}
}
#endif
