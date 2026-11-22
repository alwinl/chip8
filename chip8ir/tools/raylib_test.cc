#include <raylib.h>
#include <rlImGui.h>
#include <imgui.h>

int main( int /*argc*/, char ** /*argv*/ )
{
    InitWindow( 800, 600, "test");
    SetTargetFPS( 60 );

    rlImGuiSetup( true );

    const Font my_font = LoadFontEx( "C:/Users/Alwin Leerling/Programming/chip8/resources/fonts/Inter/Inter-VariableFont_opsz,wght.ttf", 32, nullptr, 0);

    while( !WindowShouldClose() ) {

        BeginDrawing();

            ClearBackground( {0x18, 0x18, 0x18, 0xFF} );

            DrawTextEx( my_font, "Hello World",{ 0, 0}, 40, 0, WHITE );
            DrawText( "Hello World", 0, 40, 40, WHITE );

            rlImGuiBegin();

                ImGui::Begin("test");
                ImGui::End();

            rlImGuiEnd();

        EndDrawing();
    }

    rlImGuiShutdown();

    CloseWindow();
}
