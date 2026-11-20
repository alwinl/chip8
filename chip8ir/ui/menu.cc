#include "menu.h"

#include "imgui.h"

Actions render_main_menu( const std::vector<MenuData>& menu )
{
	Actions new_action { Actions::NONE };
	bool render_submenu { false };

	if( ImGui::BeginMainMenuBar() ) {

		for( auto& menu_data_item : menu ) {
			switch( menu_data_item.type ) {
			case MenuData::Type::SUBMENU:
				if( render_submenu )
					ImGui::EndMenu();

				render_submenu = ImGui::BeginMenu( menu_data_item.label.c_str() );
				break;

			case MenuData::Type::ITEM:
				if( ! render_submenu )
					break;
				if( ImGui::MenuItem(menu_data_item.label.c_str(), menu_data_item.shortcut.c_str()) )
					new_action = menu_data_item.action;
				break;

			case MenuData::Type::SEPARATOR:
				if( ! render_submenu )
					break;
				ImGui::Separator();
				break;
			}
		}

		if( render_submenu )
			ImGui::EndMenu();
		
		ImGui::EndMainMenuBar();
	}
	return new_action;
}

