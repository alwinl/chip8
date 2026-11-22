/*
 * chemul8.cc Copyright 2026 Alwin Leerling dna.leerling@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include <iostream>

#include <raylib.h>
#include <rlImGui.h>
#include <imgui.h>
#include <imgui_stdlib.h>


#include "emulator/cmdlineparser.h"
#include "emulator/chip8.h"


#include <array>
#include <cstdint>

#include "menu.h"

constexpr int CHIP8_WIDTH  = 64;
constexpr int CHIP8_HEIGHT = 32;
constexpr int CHIP8_PIXELS = CHIP8_WIDTH * CHIP8_HEIGHT;
constexpr int CHIP8_BUF_SIZE = CHIP8_PIXELS * 4;

using RgbaBuffer = std::array<uint8_t, CHIP8_BUF_SIZE>;


uint8_t test_buffer[64*32/8];

void init_test_buffer()
{
	uint8_t content = 0x55;

	for( size_t row = 0; row < 32; ++row ) {
		for( size_t col = 0; col < 64/8; ++col )
			test_buffer[row * 64/8 + col] = content;
		content = (content == 0x55) ? 0xAA : 0x55;
	}
}


Texture2D create_chip8_texture()
{
    const Image image = GenImageColor(
        CHIP8_WIDTH,
        CHIP8_HEIGHT,
        BLACK
    );

    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);

    SetTextureFilter(texture, TEXTURE_FILTER_POINT);

    return texture;
}



void convert_framebuffer( const uint8_t* framebuffer, RgbaBuffer& rgba)
{
    for (int y = 0; y < CHIP8_HEIGHT; ++y) {
        for (int x = 0; x < CHIP8_WIDTH; ++x) {

            const int pixel_index = y * CHIP8_WIDTH + x;

            const uint8_t packed = framebuffer[pixel_index / 8];

            const int bit = 7 - (pixel_index % 8);
            const bool on = (packed & (1u << bit)) != 0;

            const uint8_t colour = on ? 192 : 96;
            const int dst = pixel_index * 4;

            rgba[dst + 0] = 0; //colour; // Red
            rgba[dst + 1] = colour; // Green
            rgba[dst + 2] = 0; //colour; // Blue
            rgba[dst + 3] = 255;    // Alpha
        }
    }
}

void render_screen( uint8_t *buffer, uint16_t size)
{
	const uint16_t total_pixels = size * 8;

	for( uint16_t pixel = 0; pixel < total_pixels; ++pixel ) {

		Rectangle rect{ .x = 10.0F * (pixel % 64), .y = 10.0F * (pixel / 64), .width = 10.0F, .height = 10.0F };
		bool is_set = ( buffer[pixel / 8] >> ( pixel % 8 ) ) & 0x01;

		DrawRectangleRec( rect, is_set ? WHITE : BLACK );
	}
}


void draw_dockspace()
{
    // Enable docking.
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Create a host window that fills the main viewport.
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags host_flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoDocking;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("Chemul8DockHost", nullptr, host_flags);
    ImGui::PopStyleVar(3);

    ImGuiID dockspace_id = ImGui::GetID("Chemul8Dockspace");

    ImGui::DockSpace(
        dockspace_id,
        ImVec2(0.0f, 0.0f),
        ImGuiDockNodeFlags_None
    );

    ImGui::End();
}


void render_chip8_display(Texture2D texture)
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 20));

    ImGui::Begin("CHIP-8 Display");

		ImVec2 available = ImGui::GetContentRegionAvail();

		float width  = available.x;
		float height = width / 2.0f;

		if( height > available.y ) {
			height = available.y;
			width  = height * 2.0f;
		}

		if( width > 0 && height > 0 )
			ImGui::Image( (ImTextureID)(intptr_t)texture.id, ImVec2(width, height) );

    ImGui::End();

	ImGui::PopStyleVar();
}

void render_disassebly_panel()
{
	ImGui::Begin("Disassembly");

		ImGui::Text("Disassembly will go here");

	ImGui::End();
}

void render_registers_panel( std::vector<Actions>& action_list )
{
	ImGui::Begin("Registers");

		ImGui::Text("Registers will go here");

		for( auto& action : action_list ) {
			switch( action ) {
			case Actions::NONE: ImGui::Text("NONE"); break;
			case Actions::OPEN: ImGui::Text("OPEN"); break;
			case Actions::EXIT: ImGui::Text("EXIT"); break;
			case Actions::RESET: ImGui::Text("RESET"); break;
			case Actions::PAUSE: ImGui::Text("PAUSE"); break;
			case Actions::ABOUT: ImGui::Text("ABOUT"); break;
			}
		}

	ImGui::End();

}

void ShowMessageBox(const char* title, const char* message)
{
    ImGui::OpenPopup(title);

    if (ImGui::BeginPopupModal(title, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextWrapped("%s", message);
        ImGui::Spacing();

        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

class ControlPanel
{
public:
	void render();

	void set_result( std::string result ) { load_result = result; }

	std::string get_filename() const { return filename; };
	Chip8::eQuirkType get_quirk_type() const { return selected_quirk_type; }

	bool get_load_program() const { return load_program; }

private:
	std::string filename;
	Chip8::eQuirkType selected_quirk_type = Chip8::eQuirkType::CHIP8;
	bool load_program{};

	std::string load_result;
};

void ControlPanel::render( )
{
	int* ptr = NULL;

	ImGui::Begin("Controls");

		if (ImGui::Button("Step")) {
			// Execute one instruction here later.
		}

		ImGui::SameLine();

		if (ImGui::Button("Reset")) {
			// Reset the emulator here later.
		}

		ImGui::InputText( "Filename", &filename );

        const char* combo_preview_value =
			(selected_quirk_type == Chip8::eQuirkType::XOCHIP) ? "XOChip" :
			(selected_quirk_type == Chip8::eQuirkType::SCHIP) ? "SChip" :
			"Chip8";

        if (ImGui::BeginCombo("Quirks", combo_preview_value, 0))
        {
			if (ImGui::Selectable( "Chip8", selected_quirk_type == Chip8::eQuirkType::CHIP8) )
				selected_quirk_type = Chip8::eQuirkType::CHIP8;

			if (ImGui::Selectable( "SChip", selected_quirk_type == Chip8::eQuirkType::SCHIP) )
				selected_quirk_type = Chip8::eQuirkType::SCHIP;

			if (ImGui::Selectable( "XOChip", selected_quirk_type == Chip8::eQuirkType::XOCHIP) )
				selected_quirk_type = Chip8::eQuirkType::XOCHIP;

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			// if (is_selected)
			// 	ImGui::SetItemDefaultFocus();
            ImGui::EndCombo();
        }

		// ShowMessageBox( "File load result", "Test" );

		load_program = ImGui::Button("Load");

		if( ! load_result.empty() ) {

			ImGui::OpenPopup("File load result");

			if (ImGui::BeginPopupModal("File load result", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::TextWrapped("%s", load_result.c_str());
				ImGui::Spacing();

				if (ImGui::Button("OK", ImVec2(120, 0))) {
					ImGui::CloseCurrentPopup();
					load_result.clear();
				}

				ImGui::EndPopup();
			}
		}

	ImGui::End();
}

std::vector<MenuData> menu_data = {
	{ MenuData::Type::SUBMENU, "File", "", Actions::NONE },
	{ MenuData::Type::ITEM, "Open Rom", "Ctrl+O", Actions::OPEN },
	{ MenuData::Type::SEPARATOR, "", "", Actions::NONE },
	{ MenuData::Type::ITEM, "Exit", "", Actions::EXIT },
	{ MenuData::Type::SUBMENU, "Emulator", "", Actions::NONE },
	{ MenuData::Type::ITEM, "Reset", "", Actions::RESET },
	{ MenuData::Type::ITEM, "Pause", "", Actions::PAUSE },
	{ MenuData::Type::SUBMENU, "Help", "", Actions::NONE },
	{ MenuData::Type::ITEM, "About", "", Actions::ABOUT },
};

int main(int argc, char** argv)
{
	std::vector<Actions> action_list;

	try {
		Chip8 device;
		init_test_buffer( );

		InitWindow(1200, 800, "Chemul8");
		rlImGuiSetup(true);

		Texture2D chip8_texture = create_chip8_texture();
		RgbaBuffer rgba{};

		// Enable docking before creating the dockspace.
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		Actions current_action = Actions::NONE;
		ControlPanel control_data;

		while (!WindowShouldClose())
		{
			if( control_data.get_load_program() ) {
				control_data.set_result( "Still to be implemented");
			}

			// convert_framebuffer( device.get_display_buffer(), rgba );
			convert_framebuffer( test_buffer, rgba );
			UpdateTexture(chip8_texture, rgba.data());

			BeginDrawing();
				ClearBackground(BLACK);

				rlImGuiBegin();

					draw_dockspace();

					current_action = render_main_menu( menu_data );
					control_data.render();
					render_chip8_display( chip8_texture );
					render_registers_panel( action_list );

				rlImGuiEnd();

			EndDrawing();

			if( current_action != Actions::NONE )
				action_list.push_back( current_action );

		}

		rlImGuiShutdown();
		CloseWindow();
	}

	catch( const std::exception &e ) {
		std::cerr << e.what() << std::endl;
        return 2;
	}

	return 0;
}
