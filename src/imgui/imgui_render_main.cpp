#include "imgui_render.h"

#include "../mcc/player_manager.h"
#include "../d3d11/d3d11_manager.h"

#include <imgui.h>
#include <libmcc/libmcc.h>

#include "../dollycam/dollycam_imgui_view.h"

using namespace libmcc;

void c_imgui_render::main() {
    char buffer[256];
    static bool show_demo = false;
    static bool show_splitscreen = false;
	static bool show_game_engine = false;
	static bool show_dollycam = false;

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("...")) {
            
			/*auto wireframe = d3d11_manager()->get_wireframe();
            if (ImGui::MenuItem("WireFrame", nullptr, &wireframe)) {
				d3d11_manager()->set_wireframe(wireframe);
            }*/

            ImGui::MenuItem("ImGui Demo", nullptr, &show_demo);

            ImGui::EndMenu();
        }
        ImGui::MenuItem("GameEngine", nullptr, &show_game_engine);
        ImGui::MenuItem("Splitscreen", nullptr, &show_splitscreen);
		//ImGui::MenuItem("DollyCam", nullptr, &show_dollycam);
        ImGui::EndMainMenuBar();
    }

    if (show_demo) {
		ImGui::ShowDemoWindow(&show_demo);
    }

    if (show_game_engine) {
		if (ImGui::Begin("GameEngine", &show_game_engine)) {
            auto engine = *mcc::g_game_engine();

			if (engine == nullptr) {
				ImGui::Text("Game Engine is not created!");
			} else {
                static char command[0x1000] = GAME_ENGINE_COMMAND_PREFIX;

				char* buffer = command + GAME_ENGINE_COMMAND_PREFIX_LENGTH;

                if (ImGui::InputTextMultiline(GAME_ENGINE_COMMAND_PREFIX, buffer, 0x1000 - GAME_ENGINE_COMMAND_PREFIX_LENGTH)) {
					buffer[0x1000 - 1] = 0;
                }

                if (ImGui::Button("Execute")) {
                    engine->insert_command(command);
                }
			}
		}
		ImGui::End();
	}

    if (show_splitscreen) {
        if (ImGui::Begin("Splitscreen", &show_splitscreen)) {
            c_critical_section cs(_critical_section_player);

            auto player = player_manager()->get_local_player_count();
            if (ImGui::SliderInt("Player", &player, 1, 4)) {
                player_manager()->set_local_player_count(player);
            }

            for (int i = 0; i < k_local_player_count; ++i) {
                sprintf(buffer, "Player %d", i);

                int input_device = player_manager()->get_input_device(i);

                if (!ImGui::BeginCombo(buffer, k_player_input_device_names[input_device])) {
                    continue;
                }

                for (int j = 0; j < k_player_input_device_count - (i != 0); ++j) {
                    bool selected = j == input_device;
                    if (!ImGui::Selectable(k_player_input_device_names[j], &selected) || input_device == j) {
                        continue;
                    }

                    player_manager()->set_input_device(i, static_cast<e_player_input_device>(j));
                }

                ImGui::EndCombo();
            }
        }
        ImGui::End();
    }

	if (show_dollycam) {
		g_dollycam_imgui_view.render(&show_dollycam);
	}
}


