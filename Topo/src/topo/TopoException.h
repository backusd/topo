#pragma once
#include "Core.h"
#include "utils/Concepts.h"

#define EXCEPTION(message) ::topo::TopoException(message)
#define EXCEPTION_WITH_DATA(message, data) ::topo::TopoExceptionWithData(message, data)

namespace topo
{
class TopoException 
{
public:
    TopoException(const std::string& errorMessage, const std::source_location& loc = std::source_location::current(), std::stacktrace trace = std::stacktrace::current()) :
        m_errorMessage(errorMessage),
        m_location{ loc },
        m_stacktrace{ trace }
    {}
    TopoException(std::string&& errorMessage, const std::source_location& loc = std::source_location::current(), std::stacktrace trace = std::stacktrace::current()) :
        m_errorMessage(std::move(errorMessage)),
        m_location{ loc },
        m_stacktrace{ trace }
    {}

    std::string& what() { return m_errorMessage; }
    const std::string& what() const noexcept { return m_errorMessage; }
    const std::source_location& where() const { return m_location; }
    const std::stacktrace& stack() const { return m_stacktrace; }

    virtual std::string dataAsString() const noexcept { return "(no data)"; }
    virtual bool hasData() const noexcept { return false; }

protected:
    std::string m_errorMessage;
    const std::source_location m_location;
    const std::stacktrace m_stacktrace;
};


template<HasFormatterSpecialization DATA_T>
class TopoExceptionWithData : public TopoException
{
public:
    TopoExceptionWithData(const std::string& errorMessage, const DATA_T& data, const std::source_location& loc = std::source_location::current(), std::stacktrace trace = std::stacktrace::current()) :
        TopoException(errorMessage, loc, trace),
        m_data(data)
    {}
    TopoExceptionWithData(const std::string& errorMessage, DATA_T&& data, const std::source_location& loc = std::source_location::current(), std::stacktrace trace = std::stacktrace::current()) :
        TopoException(errorMessage, loc, trace),
        m_data(data)
    {}
    TopoExceptionWithData(std::string&& errorMessage, const DATA_T& data, const std::source_location& loc = std::source_location::current(), std::stacktrace trace = std::stacktrace::current()) :
        TopoException(errorMessage, loc, trace),
        m_data(std::move(data))
    {}
    TopoExceptionWithData(std::string&& errorMessage, DATA_T&& data, const std::source_location& loc = std::source_location::current(), std::stacktrace trace = std::stacktrace::current()) :
        TopoException(errorMessage, loc, trace),
        m_data(std::move(data))
    {}

    DATA_T& data() { return m_data; }
    const DATA_T& data() const noexcept { return m_data; }

    std::string dataAsString() const noexcept override { return std::format("{0}", m_data); } 
    bool hasData() const noexcept { return true; }

private:
    DATA_T m_data;
};

}

template <>
struct std::formatter<topo::TopoException> : std::formatter<std::string> {
    auto format(topo::TopoException e, format_context& ctx) const 
    {
        auto& location = e.where();

        std::string s = std::format("TopoException:\n\nWHAT: {0}\n", e.what());
        if (e.hasData())
            s += std::format("\tDATA: {0}\n", e.dataAsString());
        s += std::format("WHERE: {0}({1}:{2}), `function` {3}\nSTACK TRACE:\n", location.file_name(), location.line(), location.column(), location.function_name());
        for (auto iter = e.stack().begin(); iter != (e.stack().end() - 3); ++iter)
            s += std::format("\t{0}({1}) : {2}\n", iter->source_file(), iter->source_line(), iter->description());

        return formatter<string>::format(s, ctx);
    }
};