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

#include <raylib.h>
#include <rlImGui.h>
#include <imgui.h>

#include "emulator/chip8.h"


#include <array>
#include <cstdint>

constexpr int CHIP8_WIDTH  = 64;
constexpr int CHIP8_HEIGHT = 32;
constexpr int CHIP8_PIXELS = CHIP8_WIDTH * CHIP8_HEIGHT;

using RgbaBuffer = std::array<uint8_t, CHIP8_PIXELS * 4>;


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
    Image image = GenImageColor(
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
	uint16_t total_pixels = size * 8;

	for( uint16_t pixel = 0; pixel < total_pixels; ++pixel ) {

		Rectangle rect{ .x = 10.0F * (pixel % 64), .y = 10.0F * (pixel / 64), .width = 10.0F, .height = 10.0F };
		bool is_set = ( buffer[pixel / 8] >> ( pixel % 8 ) ) & 0x01;

		DrawRectangleRec( rect, is_set ? WHITE : BLACK );
	}
}


#include "imgui.h"

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


void draw_chip8_display(Texture2D texture)
{
    ImGui::Begin("CHIP-8 Display");

    ImVec2 available = ImGui::GetContentRegionAvail();

    float width  = available.x;
    float height = width / 2.0f;

    if (height > available.y)
    {
        height = available.y;
        width  = height * 2.0f;
    }

    if (width > 0 && height > 0)
    {
        ImGui::Image(
            (ImTextureID)(intptr_t)texture.id,
            ImVec2(width, height)
        );
    }

    ImGui::End();
}

int main(int argc, char** argv)
{
    Chip8 device;
	init_test_buffer( );

    InitWindow(1200, 800, "Chemul8");
    rlImGuiSetup(true);

	Texture2D chip8_texture = create_chip8_texture();
    RgbaBuffer rgba{};

    // Enable docking before creating the dockspace.
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    while (!WindowShouldClose())
    {
		// convert_framebuffer( device.get_display_buffer(), rgba );
		convert_framebuffer( test_buffer, rgba );
		UpdateTexture(chip8_texture, rgba.data());

        BeginDrawing();
        ClearBackground(BLACK);

        rlImGuiBegin();

        draw_dockspace();

		draw_chip8_display( chip8_texture );


        ImGui::Begin("Disassembly");
        ImGui::Text("Disassembly will go here");
        ImGui::End();

        ImGui::Begin("Registers");
        ImGui::Text("Registers will go here");
        ImGui::End();

        ImGui::Begin("Controls");
        if (ImGui::Button("Step"))
        {
            // Execute one instruction here later.
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset"))
        {
            // Reset the emulator here later.
        }
        ImGui::End();

        rlImGuiEnd();

        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
}
// #include <iostream>
// #include <fstream>

// #include "ir/binary_loader.h"
// #include "ir/ir_bundle.h"

// #include "disassembler/disassembler.h"

// #include "emulator/emulator.h"

// int main( int argc, char ** argv )
// {
//     try {
//         ChemulCmdLineParser args;

//         if( !args.parse_args( argc, argv ) )
// 			return 1;

// 		Disassembler disassembler;
// 		disassembler.configure( {args.get_origin()} );

// 		std::ifstream is( args.get_program(), std::ios::binary );
// 		BinImage image = load_binary( is );

// 		IRBundle bundle = disassembler.build_ir( image );

//         Emulator emulator;
//         emulator.configure( args );

//         emulator.run( image, bundle.ir );
//     }

// 	catch (const std::exception &e) {
//         std::cerr << e.what() << std::endl;
//         return 2;
//     }

// 	return 0;
// }
