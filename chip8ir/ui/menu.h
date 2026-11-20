
#pragma once

#include <string>
#include <vector>

enum class Actions { NONE, OPEN, EXIT, RESET, PAUSE, ABOUT };

struct MenuData
{
	enum class Type { SUBMENU, ITEM, SEPARATOR };
	Type type;
	std::string label;
	std::string shortcut;
	Actions action;
};

Actions render_main_menu( const std::vector<MenuData>& menu );
