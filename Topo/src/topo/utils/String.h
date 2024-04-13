#pragma once
#include "topo/Core.h"


namespace topo
{
ND std::wstring s2ws(const std::string& str) noexcept;
ND std::string ws2s(const std::wstring& wstr) noexcept;
ND bool ends_with(std::string_view str, std::string_view suffix) noexcept;

}