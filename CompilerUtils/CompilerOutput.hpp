// CompilerOutput.hpp
#pragma once

#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>

class CompilerOutput
{
  public:
    CompilerOutput() : m_json({{"success", true}}) {}

    CompilerOutput(const std::string& filename) : m_json({{"success", true}}), m_filename(filename) {}

    nlohmann::json&       getJson() { return m_json; }
    const nlohmann::json& getJson() const { return m_json; }

    void setError(const std::string& error)
    {
        m_json["success"] = false;
        m_json["error"]   = error;
    }

    void writeToFile(const std::string& filename) const
    {
        std::ofstream file(filename);
        file << m_json.dump(4);
    }

    std::string readFromFile() const
    {
        std::ifstream inputFile(m_filename);
        if (!inputFile.is_open())
        {
            throw std::runtime_error("Unable to open input file: " + m_filename);
        }
        std::ostringstream sstr;
        sstr << inputFile.rdbuf();
        return sstr.str();
    }

  private:
    nlohmann::json m_json;
    std::string m_filename;
};