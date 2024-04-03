#pragma once
#include "Core.h"

namespace topo
{
	class TOPO_API Application
	{
	public:
		Application();
		virtual ~Application();
		
		void Run();
	};

	Application* CreateApplication();
}