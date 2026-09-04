#pragma once
#include <format>
#include <string>
#include <vector>
#include "location.hpp"

namespace cx
{

    enum class Severity
    {
        Error,
        Warning,
        Note
    };

    struct Diagnostic
    {
        Severity    severity;
        Location    loc;
        std::string message;
    };

    class Diagnostics
    {
    public:
        void report(Diagnostic d)
        {
            if (d.severity == Severity::Error)
            {
                m_errorCount++;
            }
            m_diags.push_back(std::move(d));
        }

        template <class... Args>
        void error(Location loc, std::format_string<Args...> fmt, Args&&... args)
        {
            report({Severity::Error, loc, std::format(fmt, std::forward<Args>(args)...)});
        }

        template <class... Args>
        void warning(Location loc, std::format_string<Args...> fmt, Args&&... args)
        {
            report({Severity::Warning, loc, std::format(fmt, std::forward<Args>(args)...)});
        }

        bool hasErrors() const
        {
            return m_errorCount > 0;
        }
        const std::vector<Diagnostic>& all() const
        {
            return m_diags;
        }

    private:
        std::vector<Diagnostic> m_diags;
        int                     m_errorCount = 0;
    };

} // namespace cx
