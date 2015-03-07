#pragma once

#include <iostream>
#include <string>
#include <map>
#include <fstream>

class Config
{
    public:
        static Config fromFile(const std::string& path);
        static Config* instance();
        static void setGlobalInstance(Config* config);

		int getInt(const std::string& key, int defaultValue = 0) const;
		float getFloat(const std::string& key, float defaultValue = 0.f) const;
		const std::string& getString(const std::string& key, const std::string& defaultValue = "") const;

		void setString(const std::string& key, const std::string& value);

    private:
        std::map<std::string, std::string> _values;
        static Config* _instance;

        bool init(const std::string& path);
};