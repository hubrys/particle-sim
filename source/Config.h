#pragma once

#include <iostream>
#include <string>
#include <map>
#include <fstream>

class Config
{
    public:
        static Config fromFile(const std::string& path);

		int getInt(const std::string& key, int defaultValue = 0) const;
		const std::string& getString(const std::string& key, const std::string& defaultValue = "") const;

    private:
        std::map<std::string, std::string> _values;
        bool init(const std::string& path);
};