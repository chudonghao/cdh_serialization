#ifndef CDH_SERIALIZATION_XML_DESERIALIZER_H
#define CDH_SERIALIZATION_XML_DESERIALIZER_H

#include "processer.hpp"
#include "deserializer.hpp"
#include "type_traits.hpp"
#include "nvp.hpp"
#include "singleton.hpp"
#include "std_lib.hpp"
#include "std_string.hpp"

#include <libxml/xmlreader.h>
#include <type_traits>
#include <iomanip>
#include <iostream>
#include <stack>

namespace cdh {
namespace serialization {

enum xml_deserializer_type {
    strict,
    normal
};

template<xml_deserializer_type type>
class xml_deserializer;

template<>
class xml_deserializer<strict> : public deserializer {
public:
    typedef xml_deserializer<strict> self_t;
private:
    xmlTextReaderPtr xml_text_reader_;

    process_funcs<self_t> &type_name_deserialize_func_mapping_;

    void debug() {
        auto name = xmlTextReaderConstName(xml_text_reader_);
        std::cout << std::setw(8) << (name ? (char const *) name : "!");
        auto id = xmlTextReaderGetAttribute(xml_text_reader_, BAD_CAST "id");
        std::cout << std::setw(8) << (id ? (char const *) id : "!");
        auto type = xmlTextReaderGetAttribute(xml_text_reader_, BAD_CAST "type");
        std::cout << std::setw(8) << (type ? (char const *) type : "!");
        auto value = xmlTextReaderConstValue(xml_text_reader_);
        std::cout << std::setw(8) << (value ? (char const *) value : "!");
        std::cout << std::endl;
    }

public:

    /**
     * 基本类型(std::string)直接反序列化
     * @tparam T
     * @param p
     * @return
     */
    template<typename T>
    typename std::enable_if<std::is_same<T, std::string>::value, self_t &>::type
    operator&(nvp<T> p) {
        xmlTextReaderRead(xml_text_reader_);
        std::cout << std::setw(8) << p.name();
        debug();
        xmlTextReaderRead(xml_text_reader_);
        if (xmlTextReaderNodeType(xml_text_reader_) == xmlReaderTypes::XML_READER_TYPE_END_ELEMENT) {
            p.data_reference() = T();
        } else {
            std::cout << std::setw(8) << xmlTextReaderName(xml_text_reader_);
            debug();
            p.data_reference() = T((char const *) xmlTextReaderConstValue(xml_text_reader_));
            xmlTextReaderRead(xml_text_reader_);
        }
        return *this;
    }

    /**
     * 基本类型(arithmetic)直接反序列化
     * @tparam T
     * @param p
     * @return
     */
    template<typename T>
    typename std::enable_if<std::is_arithmetic<T>::value, self_t &>::type
    operator&(nvp<T> p) {
        xmlTextReaderRead(xml_text_reader_);
        std::cout << std::setw(8) << p.name();
        debug();
        xmlTextReaderRead(xml_text_reader_);
        if (xmlTextReaderNodeType(xml_text_reader_) == xmlReaderTypes::XML_READER_TYPE_END_ELEMENT) {
            p.data_reference() = static_cast<T>(0);
        } else {
            std::cout << std::setw(8) << xmlTextReaderName(xml_text_reader_);
            debug();
            p.data_reference() = static_cast<T>(std::stoll((char const *) xmlTextReaderConstValue(xml_text_reader_)));
            xmlTextReaderRead(xml_text_reader_);
        }
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
        xmlTextReaderRead(xml_text_reader_);
        std::cout << std::setw(8) << p.name();
        debug();
        xmlTextReaderRead(xml_text_reader_);
        if (xmlTextReaderNodeType(xml_text_reader_) == xmlReaderTypes::XML_READER_TYPE_END_ELEMENT) {
        } else {
            xmlTextReaderRead(xml_text_reader_);
        }
        //TODO
        throw std::runtime_error(std::string("const type,arithmetic pointer and array are not support yet. !TODO"));
    }

    /**
     * 反序列化一个不支持rtti的对象
     * @tparam T
     * @param p
     * @return
     */
    template<typename T>
    typename std::enable_if<is_user_class<T>::value, self_t &>::type
    operator&(nvp<T> p) {
        xmlTextReaderRead(xml_text_reader_);
        std::cout << std::setw(8) << p.name();
        debug();
        process(*this, p.const_data_reference());
        xmlTextReaderRead(xml_text_reader_);

        return *this;
    }

    /**
     * 反序列化基类
     * @tparam T
     * @param p
     * @return
     */
    template<typename T>
    self_t &
    operator&(nvp<T, true> p) {
        xmlTextReaderRead(xml_text_reader_);
        std::cout << std::setw(8) << p.name();
        debug();
        process(*this, p.const_data_reference());
        xmlTextReaderRead(xml_text_reader_);
        return *this;
    }

    /**
     * 反序列化一个支持rtti的对象
     * @tparam T
     * @param p
     * @return
     */
    template<typename T>
    typename std::enable_if<is_user_rtti_class<T>::value, self_t &>::type
    operator&(nvp<T> p) {
        xmlTextReaderRead(xml_text_reader_);
        std::cout << std::setw(8) << p.name();
        debug();
        //TODO
        xmlTextReaderRead(xml_text_reader_);
        return *this;
    }

    /**
     * 反序列化一个指针
     * @tparam T
     * @param p
     * @return
     */
    template<typename T>
    typename std::enable_if<is_user_pointer<T, 1>::value, self_t &>::type
    operator&(nvp<T> p) {
        typedef typename std::remove_pointer<T>::type basic_type;
        xmlTextReaderRead(xml_text_reader_);
        std::cout << std::setw(8) << p.name();
        debug();
        return *this;
    }

    explicit xml_deserializer() : type_name_deserialize_func_mapping_(singleton<process_funcs<self_t>>::instance()),
                                  xml_text_reader_(nullptr) {
    }

    ~xml_deserializer() {

    }


    void data(char const *data, size_t size) {
        if (xml_text_reader_) {
            xmlReaderNewMemory(xml_text_reader_, data, static_cast<int>(size), NULL, NULL, XML_PARSE_NOBLANKS);
        } else
            xml_text_reader_ = xmlReaderForMemory(data, static_cast<int>(size), NULL, NULL, XML_PARSE_NOBLANKS);
    }

    template<typename T>
    void deserialize(char const *name, T &t) {
        *this & make_nvp(name, t);
    }
};

template<>
class xml_deserializer<normal> : public deserializer {
public:
    typedef xml_deserializer<normal> self_t;
private:

    std::map<int, void *> id_pointer_mapping_;
    std::multimap<int, void **> id_undetermined_pointer_mapping_;

    xmlNodePtr current_xml_node_parent_;
    xmlNodePtr current_xml_node_;
    xmlDocPtr xml_doc_;

    const process_funcs<self_t> &type_name_deserialize_func_mapping_;

    void debug() {
        std::cout << std::setw(16)
                  << (current_xml_node_->name ? (char const *) current_xml_node_->name : "!");
        std::cout << std::setw(16)
                  << (current_xml_node_->content ? (char const *) current_xml_node_->content : "!");
        for (auto att = current_xml_node_->properties; att; att = att->next) {
            std::cout << std::setw(16) << att->name << std::setw(16) << atoll((const char *) (att->children->content));
        }
        std::cout << std::endl;
    }

    char const *get_property(xmlNodePtr xml_node, char const *name) {
        if (xml_node) {
            for (auto att = xml_node->properties; att; att = att->next) {
                if (strcmp(name, (const char *) (att->name)) == 0) {
                    //found
                    return (const char *) (att->children->content);
                } else continue;
            }
            //not found
            return nullptr;
        } else
            //!xml_node
            return nullptr;
    }

    void determine_undetermined_pointer() {
        for (auto &&id_undetermined_pointer_pair : id_undetermined_pointer_mapping_) {
            auto id_pinter_pair = id_pointer_mapping_.find(id_undetermined_pointer_pair.first);
            if (id_pinter_pair == id_pointer_mapping_.end()) {
                *id_undetermined_pointer_pair.second = nullptr;
            } else {
                *id_undetermined_pointer_pair.second = id_pinter_pair->second;
            }
        }
    }

public:

    /**
     * 基本类型(basic_type)直接反序列化
     * @tparam T
     * @param p
     * @return
     */
    template<typename T>
    typename std::enable_if<is_basic_type<T>::value, self_t &>::type
    operator&(nvp<T> p) {
        if (!current_xml_node_) {
            throw std::runtime_error("!current_xml_node_");
        } else {
            debug();
            if (strcmp((char const *) (current_xml_node_->name), p.name()) != 0) {
                throw std::runtime_error("not same name");
            } else {
                auto node = current_xml_node_->children;
                if (!node) {
                    p.data_reference() = strto<T>(nullptr);
                } else {
                    current_xml_node_ = node;
                    debug();
                    current_xml_node_ = node->parent;
                    p.data_reference() = strto<T>(reinterpret_cast<const char *>(node->content));
                }
            }
        }
        //进入下一个节点
        if (current_xml_node_->next)
            current_xml_node_ = current_xml_node_->next;
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
        //if (!current_xml_node_) {
        //    throw std::runtime_error("!current_xml_node_");
        //} else {
        //    debug();
        //    if (strcmp((const char *) (current_xml_node_->name), p.name()) != 0) {
        //        throw std::runtime_error("not same name");
        //    } else {}
        //}
        ////进入下一个节点
        //if (current_xml_node_->next)
        //    current_xml_node_ = current_xml_node_->next;
        //TODO
        throw std::runtime_error(std::string("const type,arithmetic pointer and array are not support yet. !TODO"));
    }

    /**
     * 反序列化一个不支持rtti的对象
     * @tparam T
     * @param p
     * @return
     */
    template<typename T>
    typename std::enable_if<is_user_class<T>::value, self_t &>::type
    operator&(nvp<T> p) {
        if (!current_xml_node_) {
            throw std::runtime_error("!current_xml_node_");
        } else {
            debug();
            if (strcmp((const char *) (current_xml_node_->name), p.name()) != 0) {
                throw std::runtime_error("not same name");
            } else {
                int id = strto<int>(get_property(current_xml_node_, "id"));
                if (id) {
                    id_pointer_mapping_[id] = &p.data_reference();
                }
            }
        }
        //进入子节点，并且防止没有子节点而不能正确退出
        current_xml_node_parent_ = current_xml_node_;
        current_xml_node_ = current_xml_node_->children;

        process(*this, p.data_reference());

        //退出子节点并进入下一个节点
        current_xml_node_ = current_xml_node_parent_->next;
        current_xml_node_parent_ = current_xml_node_parent_->parent;
        return *this;
    }

    /**
     * 反序列化基类
     * @tparam T
     * @param p
     * @return
     */
    template<typename T>
    self_t &
    operator&(nvp<T, true> p) {
        if (!current_xml_node_) {
            throw std::runtime_error("!current_xml_node_");
        } else {
            debug();
            if (strcmp((const char *) (current_xml_node_->name), p.name()) != 0) {
                throw std::runtime_error("not same name");
            } else {
                int id = strto<int>(get_property(current_xml_node_, "id"));
                if (id) {
                    id_pointer_mapping_[id] = &p.data_reference();
                }
            }
        }
        //进入子节点，并且防止没有子节点而不能正确退出
        current_xml_node_parent_ = current_xml_node_;
        current_xml_node_ = current_xml_node_->children;

        process(*this, p.data_reference());

        //退出子节点并进入下一个节点
        current_xml_node_ = current_xml_node_parent_->next;
        current_xml_node_parent_ = current_xml_node_parent_->parent;
        return *this;
    }

    /**
     * 反序列化一个支持rtti的对象
     * @tparam T
     * @param p
     * @return
     */
    template<typename T>
    typename std::enable_if<is_user_rtti_class<T>::value, self_t &>::type
    operator&(nvp<T> p) {
        if (!current_xml_node_) {
            throw std::runtime_error("!current_xml_node_");
        } else {
            debug();
            if (strcmp((const char *) (current_xml_node_->name), p.name()) != 0) {
                throw std::runtime_error("not same name");
            } else {
                //TODO
            }
        }
        //进入子节点，并且防止没有子节点而不能正确退出
        current_xml_node_parent_ = current_xml_node_;
        current_xml_node_ = current_xml_node_->children;

        process(*this, p.data_reference());

        //退出子节点并进入下一个节点
        current_xml_node_ = current_xml_node_parent_->next;
        current_xml_node_parent_ = current_xml_node_parent_->parent;
        return *this;
    }

    /**
     * 反序列化一个指针
     * @tparam T
     * @param p
     * @return
     */
    template<typename T>
    typename std::enable_if<is_user_pointer<T, 1>::value, self_t &>::type
    operator&(nvp<T> p) {
        typedef typename std::remove_pointer<T>::type basic_type;
        if (!current_xml_node_) {
            throw std::runtime_error("!current_xml_node_");
        } else {
            debug();
            if (strcmp((const char *) (current_xml_node_->name), p.name()) != 0) {
                throw std::runtime_error("not same name");
            } else {
                int id = strto<int>(get_property(current_xml_node_, "id"));
                if (id) {
                    auto pair = id_pointer_mapping_.find(id);
                    if (pair == id_pointer_mapping_.end()) {
                        id_undetermined_pointer_mapping_.insert(std::make_pair(id, (void **) &p.data_reference()));
                    } else {
                        p.data_reference() = (T) pair->second;
                    }
                }
            }
        }
        //进入下一个节点
        if (current_xml_node_->next)
            current_xml_node_ = current_xml_node_->next;
        return *this;
    }

    explicit xml_deserializer() : type_name_deserialize_func_mapping_(
            singleton<process_funcs<self_t >>::instance()), xml_doc_(nullptr) {

    }

    ~xml_deserializer() {

    }

    void data(char const *data, size_t size) {
        id_pointer_mapping_.clear();
        id_undetermined_pointer_mapping_.clear();

        if (xml_doc_) {
            xmlFreeDoc(xml_doc_);
        } else {}
        xml_doc_ = xmlReadMemory(data, static_cast<int>(size), NULL, NULL, XML_PARSE_NOBLANKS);
        if (!xml_doc_) {
            throw std::runtime_error("!xml_doc_");
        } else {}
    }

    template<typename T>
    void deserialize(char const *name, T &t) {
        current_xml_node_ = xmlDocGetRootElement(xml_doc_);
        if (!current_xml_node_) {
            throw std::runtime_error("!root_ele_");
        }
        //进入第一个对象节点
        current_xml_node_parent_ = current_xml_node_;
        *this & make_nvp(name, t);
        determine_undetermined_pointer();
    }
};

}
}

#endif
