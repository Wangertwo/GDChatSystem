#pragma once
#include "Singleton.h"
struct SectionInfo {
	SectionInfo() {};
	~SectionInfo() { _section_map.clear(); };
	SectionInfo(const SectionInfo& other) {
		this->_section_map = other._section_map;
	}
	SectionInfo& operator=(const SectionInfo& other) {
		if (this == &other) {
			return *this;
		}
		this->_section_map = other._section_map;
	}
	std::string operator[](const std::string& key) {
		if (_section_map.find(key) == _section_map.end()) {
			return "";
		}
		return _section_map[key];
	}

	std::map<std::string, std::string> _section_map;
};

class ConfigMgr : public Singleton<ConfigMgr>
{
	friend class Singleton<ConfigMgr>;
private:
	ConfigMgr() {
		boost::filesystem::path cur_path = boost::filesystem::current_path();
		boost::filesystem::path config_path = cur_path / "config.ini";
		std::cout << "config path: " << config_path.string() << std::endl;

		boost::property_tree::ptree pt; // 树数据结构
		boost::property_tree::read_ini(config_path.string(), pt);

		for (const auto& section_pair: pt) {
			const std::string& section_name = section_pair.first;
			const boost::property_tree::ptree& section_tree = section_pair.second;

			std::map<std::string, std::string> _section_tmp;
			for (const auto& key_value_pair : section_tree) {
				const std::string& key = key_value_pair.first;
				const std::string& value = key_value_pair.second.get_value<std::string>();
				_section_tmp[key] = value;
			}
			SectionInfo sectionInfo;
			sectionInfo._section_map = _section_tmp; // 拷贝复制
			// 将section的key-value对保存到config_map中  
			_config_map[section_name] = sectionInfo; 
		}
		
		// 输出所有的section和key-value对  
		for (const auto& section_entry : _config_map) {
			const std::string& section_name = section_entry.first;
			SectionInfo section_config = section_entry.second;
			std::cout << "[" << section_name << "]" << std::endl;
			for (const auto& key_value_pair : section_config._section_map) {
				std::cout << key_value_pair.first << "=" << key_value_pair.second << std::endl;
			}
		}
	}

	ConfigMgr(const ConfigMgr& other) {
		this->_config_map = other._config_map;
	}

	ConfigMgr& operator=(const ConfigMgr& other) {
		if (this == &other) {
			return *this;
		}
		this->_config_map = other._config_map;
	}

	std::map<std::string, SectionInfo> _config_map;
public:
	~ConfigMgr() { _config_map.clear(); };
	SectionInfo operator[](const std::string& key) {
		if (_config_map.find(key) == _config_map.end()) {
			return SectionInfo();
		}
		return _config_map[key];
	}
};