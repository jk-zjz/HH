//
// Created by 35148 on 2024/5/6.
//
#include "config.h"
#include <yaml-cpp/yaml.h>

#include <utility>
void printNode(const YAML::Node& node) {
    switch (node.Type()) {
        case YAML::NodeType::Null:
            std::cout << "null" << std::endl;
            break;
        case YAML::NodeType::Scalar:
            std::cout <<"  " <<node.as<std::string>() << std::endl;
            break;
        case YAML::NodeType::Sequence:
            for (size_t i = 0; i < node.size(); ++i) {

                printNode(node[i]);
            }
            break;
        case YAML::NodeType::Map:
            for (YAML::const_iterator it = node.begin(); it != node.end(); ++it) {
                std::cout <<"key "<< it->first.as<std::string>() << "\n";
                printNode(it->second);
            }
            break;
        default:
            std::cout << "unknown type" << std::endl;
            break;
    }
}
void yaml(){
    YAML::Node root = YAML::LoadFile("/home/hh/HH/bin/conf/test.yml");
    printNode(root);
   // HH_LOG_LEVEL_CHAIN(HH_LOG_ROOT(),hh::LogLevel::INFO)<<root;
}
void Confgi(){
    hh::ConfigVar<int>::ptr g_int_value_config_ = hh::Config::Lookup("hh.values",(int)9876,"hh");
    hh::ConfigVar<std::string>::ptr g_in = hh::Config::Lookup("hh",(std::string)"","");
    hh::ConfigVar<std::vector<int>>::ptr get_vector_int = hh::Config::Lookup<std::vector<int>>("hh.arr",{1,2,3},"sb");
    auto vec = get_vector_int->getValue();
    for(auto &i:vec){
        HH_LOG_INFO(HH_LOG_ROOT(),boost::lexical_cast<std::string>(i));
    }

    HH_LOG_LEVEL_CHAIN(HH_LOG_ROOT(),hh::LogLevel::INFO)<<g_int_value_config_->toString();
    YAML::Node root = YAML::LoadFile("/home/hh/HH/bin/conf/test.yml");
    hh::Config::loadFromYaml(root);


    HH_LOG_LEVEL_CHAIN(HH_LOG_ROOT(),hh::LogLevel::INFO)<<g_int_value_config_->toString();
    HH_LOG_LEVEL_CHAIN(HH_LOG_ROOT(),hh::LogLevel::INFO)<<g_in->getValue();
    vec = get_vector_int->getValue();
    for(auto &i:vec){
        HH_LOG_INFO(HH_LOG_ROOT(),boost::lexical_cast<std::string>(i));
    }
    printf("%s",g_int_value_config_->toString().c_str());
}
void test(){
    hh::ConfigVar<std::string>::ptr str = hh::Config::Lookup<std::string>("Logs[1].formatter","asd","asda");
    hh::ConfigVar<int>::ptr int_port = hh::Config::Lookup<int>("hh.port",1,"asda");
    hh::ConfigVar<std::string>::ptr string_port = hh::Config::Lookup<std::string>("hh.port","1","asda");
    std::cout<<str->getValue()<<std::endl;
    std::cout<<int_port->getValue()<<std::endl;
    hh::ConfigVar<std::list<int>>::ptr  int_list = hh::Config::Lookup<std::list<int>>("hh.list_arr",{1,2,3},"");
    hh::ConfigVar<std::set<int>>::ptr int_set = hh::Config::Lookup<std::set<int>>("hh.set_arr",{2,2,3},"");
    hh::ConfigVar<std::map<std::string,std::string>>::ptr string_map = hh::Config::Lookup("hh",std::map<std::string,std::string>{{"key","2"}},"");
    std::list<int>vec = int_list->getValue();
    std::set<int>m_set= int_set->getValue();
    std::map<std::string,std::string>m_map =string_map->getValue();
    for(auto &i:vec){
        HH_LOG_INFO(HH_LOG_ROOT(),boost::lexical_cast<std::string>(i));
    }
    for(auto &i:m_set){
        HH_LOG_INFO(HH_LOG_ROOT(),boost::lexical_cast<std::string>(i));
    }
    for(auto &i:m_map){
        HH_LOG_LEVEL_CHAIN(HH_LOG_ROOT(),hh::LogLevel::INFO)<<i.first<<"  "<<i.second;
    }
    std::cout<<int_list->toString()<<std::endl;
    YAML::Node root = YAML::LoadFile("/home/hh/HH/bin/conf/test.yml");
    hh::Config::loadFromYaml(root);
    vec = int_list->getValue();
    m_set= int_set->getValue();
    m_map =string_map->getValue();
    for(auto &i:vec){
        HH_LOG_INFO(HH_LOG_ROOT(),boost::lexical_cast<std::string>(i));
    }
    for(auto &i:m_set){
        HH_LOG_INFO(HH_LOG_ROOT(),boost::lexical_cast<std::string>(i));
    }
    for(auto &i:m_map){
        HH_LOG_LEVEL_CHAIN(HH_LOG_ROOT(),hh::LogLevel::INFO)<<i.first<<"  "<<i.second;
    }
    std::cout<<int_list->toString()<<std::endl;
    std::cout<<str->getValue();
    std::cout<<int_set->toString()<<std::endl;
}
class user{
public:
    bool operator==(const user& other) const {
        return this->age==other.getAge()&&this->name==other.getName()? true:false;
    }
    void setage( int m_age){
        this->age=m_age;
    }
    void setname(const std::string basicString) {
        this->name=basicString;
    }
    const std::string &getName() const {
        return name;
    }

    int getAge() const {
        return age;
    }
    std::string ToString() const {
        std::stringstream ss;
        ss<<"name :"<<name<<"age :"<<age;
        return ss.str();
    }

private:
    std::string name;
    int age=0;
};
template<>
class hh::LexicalCast<std::string,user>{
public:
    user operator()(const std::string& var){
        YAML::Node node=YAML::Load(var);
        user u;
        u.setname(node["name"].as<std::string>());
        u.setage(node["age"].as<int>());
        return u;
    }
};
template<>
class hh::LexicalCast<user,std::string>{
public:
    std::string operator()(const user& u){
        YAML::Node node;
        node["name"]=u.getName();
        node["age"]=u.getAge();
        std::stringstream ss;
        ss<<node;
        return ss.str();
    }
};
void T(){
    hh::ConfigVar<user>::ptr user_ =hh::Config::Lookup("class.user",user(),"");
    hh::ConfigVar<std::map<std::string,user>>::ptr user_map =hh::Config::Lookup<std::map<std::string,user>>("class.map",{},"");
    hh::ConfigVar<std::vector<user>>::ptr user_vect =hh::Config::Lookup<std::vector<user>>("class.vector",{},"");
    user_->addOcb(10,[](const user&od_user,const user& new_user){
        std::cout<<new_user.ToString()<<"---"<<od_user.ToString()<<"\n";
    });
    HH_LOG_LEVEL_CHAIN(HH_LOG_ROOT(),hh::LogLevel::INFO)<<user_->toString();
    YAML::Node root = YAML::LoadFile("/home/hh/HH/bin/conf/test.yml");
    hh::Config::loadFromYaml(root);
    HH_LOG_LEVEL_CHAIN(HH_LOG_ROOT(),hh::LogLevel::INFO)<<user_->toString();
    for(auto &i:user_map->getValue()){
        std::cout<<i.second.getName();
        std::cout<<i.second.getAge();
        std::cout<<std::endl;
    }
    for(auto &i:user_vect->getValue()){
        std::cout<<i.getName();
        std::cout<<i.getAge();
        std::cout<<std::endl;
    }
}
void testlogcofig(){
    std::string sb("asdasd");
    hh::Logger::ptr logger = HH_LOG_NAME(sb);
    hh::Logger::ptr root(new hh::Logger("sbsb1234"));


    HH_LOG_LEVEL_CHAIN(logger,hh::LogLevel::INFO)<<"sbsb 1234";
    std::cout<<"---\n";
    root->addAppender(hh::LogAppender::ptr (new hh::StdoutLogAppender));
    HH_LOG_LEVEL_CHAIN(root,hh::LogLevel::INFO)<<"sbsb 1234";

}
void test4(){
    std::cout<<"start"<<std::endl;

    HH_LOG_LEVEL_CHAIN(hh::LoggerMgr::GetInstance()->GetRoot(),hh::LogLevel::INFO)<<"sbsb1234";

    std::cout<<"end"<<std::endl;
}
int main(){
    //Confgi();
    //test();
    //T();
//    testlogcofig();
    test4();
    return 0;
}