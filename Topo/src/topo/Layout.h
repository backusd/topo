#pragma once
#include "Core.h"


namespace topo
{
class TOPO_API Layout
{
public:
	Layout(float height, float width) : m_height(height), m_width(width) 
	{}

private:
	float m_height;
	float m_width;
};
}