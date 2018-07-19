#ifndef CONFIGITEM_H
#define CONFIGITEM_H

#include "IConfigItem.h"
#include "Core/YAML/YamlConverter.h"
#include "easylogging++.h"
#include <yaml-cpp/yaml.h>
#include <fmt/format.h>
#include <vector>

#define CONFIG_ITEM(name,type,default_value)\
  ConfigItem<type> name{#name,default_value, this};
#define CUSTOM_CONFIG_ITEM(name,default_value)\
  name name{#name,default_value, this};

class Config;


template <typename T>
class ConfigItem : public IConfigItem {
protected:
  T value_{};
public:
  //constructor
  explicit ConfigItem(std::string name, const T& default_value, Config* config = nullptr);

  // destructor
  virtual ~ConfigItem() = default;
  // copy constructor
  ConfigItem(const ConfigItem&) = delete;

  // copy assignment
  ConfigItem& operator=(const ConfigItem&) = delete;

  // move constructor
  ConfigItem(ConfigItem&&) = delete;

  // move assignment
  ConfigItem& operator=(ConfigItem&&) = delete;

  virtual T& operator()();

  void set_value(const YAML::Node& node) override;
};


template <typename T>
ConfigItem<T>::ConfigItem(std::string name, const T& default_value, Config* config) : IConfigItem(config, name),
                                                                                      value_{default_value} {
}

template <typename T>
T& ConfigItem<T>::operator()() {
  return value_;
};

template <typename T>
void ConfigItem<T>::set_value(const YAML::Node& node) {
  {
    if (node[name_]) {
      value_ = node[name_].as<T>();
    }
    else {
      LOG(WARNING) << name_ << " used default value of " << value_;
    }
  }
}

template < class T >
inline std::ostream& operator << (std::ostream& os, const std::vector<T>& v)
{
	auto sep = "";
	os << "[";
	for (const auto&i : v)
	{
		os << sep << i;
		sep = " , ";
	}
	os << " ]";
	return os;
}

 template <typename T>
 class ConfigItem<std::vector<T>> : public IConfigItem {
 protected:
   std::vector<T> value_{};
 public:
   //constructor
   explicit ConfigItem(std::string name, std::vector<T> default_value, Config* config = nullptr);

   // destructor
   virtual ~ConfigItem() = default;
   // copy constructor
   ConfigItem(const ConfigItem&) = delete;

   // copy assignment
   ConfigItem& operator=(const ConfigItem&) = delete;

   // move constructor
   ConfigItem(ConfigItem&&) = delete;

   // move assignment
   ConfigItem& operator=(ConfigItem&&) = delete;

   virtual std::vector<T>& operator()();
   void set_value(const YAML::Node& node) override;
 };

 template <typename T>
 ConfigItem<std::vector<T>>::
 ConfigItem(std::string name, std::vector<T> default_value, Config* config): IConfigItem(config, name),
                                                                             value_{std::move(default_value)} {
   // ReSharper disable once CppClassIsIncomplete
 }

 template <typename T>
 std::vector<T>& ConfigItem<std::vector<T>>::operator()() {
   return value_;
 }


 template <typename T>
 void ConfigItem<std::vector<T>>::set_value(const YAML::Node& node) {
   if (node[name_]) {   
     value_ = node[name_].as<std::vector<T>>();
   }
   else {
     std::stringstream ss;
     auto sep = "";
     ss << "[";
     for (const auto& value : value_) {
       ss << sep << value;
       sep = " , ";
     }
     ss << "]";
     LOG(WARNING) << fmt::format("{} used default value of {}", name_, ss.str());
   }
 }



#endif // CONFIGITEM_H
