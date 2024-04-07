#pragma once


template <typename T>
concept HasFormatterSpecialization = requires(T t)
{
    std::formatter<T>();
};