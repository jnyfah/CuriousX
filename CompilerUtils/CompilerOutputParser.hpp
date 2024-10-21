// CompilerOutput.hpp
#pragma once

#include <fstream>
#include <nlohmann/json.hpp>
#include <string>


class CompilerOutput
{
  public:
    CompilerOutput() : m_json({{"success", true}}) {}

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
        file << m_json.dump(4); // 4 spaces indent for pretty printing
    }

  private:
    nlohmann::json m_json;
};
