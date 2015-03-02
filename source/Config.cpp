#include "./Config.h"

Config* Config::_instance;

Config* Config::instance()
{
    return _instance;
}

void Config::setGlobalInstance(Config* config)
{
    _instance = config;
}

bool Config::init(const std::string& path)
{
    std::ifstream file(path);
    if (file.is_open() == false)
    {
        return false;
    }

    std::string line;
    std::string key;
    std::string value;
    while (file.eof() == false)
    {
        std::getline(file, line, '\n');
        // std::getline(file, value, '\n');
        if (line[0] == '#') {
            continue;
        }

        int index = line.find_first_of('=');
        if (index == std::string::npos) 
        {
            continue;
        }

        _values[line.substr(0, index)] = line.substr(index + 1);
    }
    file.close();
    return true;
}

// static
Config Config::fromFile(const std::string& path)
{
    Config config;
    if (config.init(path))
    {
        return config;
    }

    return Config();
}

int Config::getInt(const std::string& key, int def) const
{ 
	auto it = _values.find(key);
	if (it == _values.end())
	{
		return def;
	}
	return std::stoi(it->second);
}

float Config::getFloat(const std::string& key, int defaultValue) const
{
    auto it = _values.find(key);
    if (it == _values.end())
    {
        return defaultValue;
    }
    return std::stof(it->second);
}

const std::string& Config::getString(const std::string& key, const std::string& def) const
{
	auto it = _values.find(key);
	if (it == _values.end())
	{
		return def;
	}
	return it->second;
}

void Config::setString(const std::string& key, const std::string& value)
{
    _values[std::string(key)] = std::string(value);
}
