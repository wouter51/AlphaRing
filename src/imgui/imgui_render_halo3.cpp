#include "imgui_render.h"

#include "../halo3/halo3.h"

#include "../dollycam/dollycam_imgui_view.h"

#include <imgui.h>

#include <libmcc/libmcc.h>

using namespace libmcc;
using namespace halo3;

static void render_debug(s_thread_local_storage* tls, bool* show);
static void render_engine(s_thread_local_storage* tls, bool* show);
static void render_director(s_thread_local_storage* tls, bool* show);
static void render_observer(s_thread_local_storage* tls, bool* show);
static void render_dollycam(s_thread_local_storage* tls, bool* show);

void c_imgui_render::halo3() {
    static bool show_engine = false;
    static bool show_director = false;
    static bool show_observer = false;
    static bool show_dollycam = false;
    static bool show_debug = false;

    auto tls = g_thread_local_storage();

    if (ImGui::BeginMainMenuBar()) {
        ImGui::MenuItem("Debug", nullptr, &show_debug);
		ImGui::MenuItem("Engine", nullptr, &show_engine);
        ImGui::MenuItem("Director", nullptr, &show_director);
        ImGui::MenuItem("Observer", nullptr, &show_observer);

        ImGui::EndMainMenuBar();
    }

    if (show_debug) {
		render_debug(tls, &show_debug);
    }

    if (show_engine) {
        render_engine(tls, &show_engine);
    }

    if (show_director) {
		render_director(tls, &show_director);
    }

    if (show_observer) {
		render_observer(tls, &show_observer);
    }
}

static void render_debug(s_thread_local_storage* tls, bool* show) {
    do {
        if (!ImGui::Begin("Debug", show)) {
            break;
        }

        auto options = &g_halo3_options;
        ImGui::Checkbox("Debug Structure", &options->debug_structure);
        ImGui::ColorEdit4("Debug Structure Color", options->debug_structure_color.n);
        ImGui::Checkbox("Debug Instanced Geometry", &options->debug_instanced_geometry);
        ImGui::ColorEdit4("Debug Instanced Geometry Color", options->debug_instanced_geometry_color.n);

    } while (false);
    ImGui::End();
}

static void render_engine(s_thread_local_storage* tls, bool* show) {
    do {
        if (!ImGui::Begin("Engine", show)) {
            break;
        }

        auto engine = tls->game_engine_globals;

        auto map_variant = &engine->map_variant;

		ImGui::Text("Number Of Variant Objects: %d", map_variant->m_number_of_variant_objects);

        if (ImGui::Button("Test")) {
            event_queue_manager()->push(std::make_unique<s_halo3_test_event>());
        }

    } while (false);
    ImGui::End();
}

static void render_director(s_thread_local_storage* tls, bool* show) {
    do {
        if (!ImGui::Begin("Director", show)) {
            break;
        }

        auto director = tls->director_globals;

        auto info = &director->infos[0];

        auto camera = &director->directors[0].director.m_camera.camera;

        ImGui::Text("Camera Type: %d", info->camera_mode);

        switch (info->camera_mode) {
        case _camera_mode_flying: {
            auto flying_camera = reinterpret_cast<c_flying_camera*>(camera);
            ImGui::Text("Position: %f %f %f",
                flying_camera->m_position.x, flying_camera->m_position.y, flying_camera->m_position.z);

            ImGui::Text("Facing: %f %f",
                flying_camera->m_facing.yaw * RAD_TO_DEG, flying_camera->m_facing.pitch * RAD_TO_DEG);

            ImGui::Text("Roll: %f", flying_camera->m_roll * RAD_TO_DEG);
            break;
        }
        default:
            break;
        }

    } while (false);

    ImGui::End();
}

static void render_observer(s_thread_local_storage* tls, bool* show) {
    do {
        if (!ImGui::Begin("Observer", show)) {
            break;
        }
        auto observer = tls->g_observer_globals;

        auto position = &observer->observers[0].result.focus_point;

        real_euler_angles2d angles(observer->observers[0].result.forward);

		real_euler_angles2d up(observer->observers[0].result.up);

        ImGui::Text("%.3f %.3f %.3f / %.2f %.2f / %.2f %.2f",
            position->x, position->y, position->z,
            angles.yaw * RAD_TO_DEG, angles.pitch * RAD_TO_DEG,
            up.yaw * RAD_TO_DEG, up.pitch * RAD_TO_DEG);

        if (ImGui::Button("Tets")) {
            s_halo3_flying_camera_adjust_event event;

            event.player = _local_player_0;

            event.roll = 10.0f * DEG_TO_RAD;

            event_queue_manager()->push(std::make_unique<s_halo3_flying_camera_adjust_event>(event));
        }

    } while (false);

    ImGui::End();
}

static void render_dollycam(s_thread_local_storage* tls, bool* show) {
    do {
        if (!ImGui::Begin("Property Editor", show)) {
            break;
        }

        auto director_globals = tls->director_globals;
        auto observer_globals = tls->g_observer_globals;
        auto gametime_globals = tls->game_time_globals;

        auto info = &director_globals->infos[0];

        auto camera = &director_globals->directors[0].director.m_camera.camera;

        if (info->camera_mode != _camera_mode_flying) {
            ImGui::Text("Please Change Camera Mode To Flying Mode!");
            break;
        }

        auto observer = observer_globals->observers + 0;
        auto flying_camera = reinterpret_cast<c_flying_camera*>(camera);

        {
            s_halo3_flying_camera_update_event event;

            event.player = _local_player_0;

            event.update_position = false;
            event.position = flying_camera->m_position;

            event.update_facing = false;
            event.facing = real_euler_angles2d(flying_camera->m_facing.yaw * RAD_TO_DEG, flying_camera->m_facing.pitch * RAD_TO_DEG);

            event.update_roll = false;
            event.roll = flying_camera->m_roll * RAD_TO_DEG;

            if (ImGui::DragFloat3("Position", event.position.n, 0.01f)) {
                event.update_position = true;
            }

            if (ImGui::DragFloat2("Facing", event.facing.n)) {
                event.update_facing = true;
                event.facing.yaw *= DEG_TO_RAD;
                event.facing.pitch *= DEG_TO_RAD;
            }

            if (ImGui::DragFloat("Roll", &event.roll)) {
                event.update_roll = true;
                event.roll *= DEG_TO_RAD;
            }

            if (event.update_position || event.update_facing || event.update_roll) {
                event_queue_manager()->push(std::make_unique<s_halo3_flying_camera_update_event>(event));
            }
        }

        {
            s_halo3_player_fov_update_event event;

            event.player = _local_player_0;
            event.fov = observer->horizontal_field_of_view * RAD_TO_DEG;

            if (ImGui::DragFloat("Fov", &event.fov)) {
                event_queue_manager()->push(std::make_unique<s_halo3_player_fov_update_event>(event));
            }
        }

        if (ImGui::Button("Insert Keyframe")) {
            s_dollycam_value value;
			s_dollycam_value_attribute attribute;

            attribute.n = -1;

			value.position = flying_camera->m_position;
			value.facing = real_euler_angles2d(flying_camera->m_facing.yaw * RAD_TO_DEG, flying_camera->m_facing.pitch * RAD_TO_DEG);
			value.roll = flying_camera->m_roll;
			value.fov = observer->horizontal_field_of_view * RAD_TO_DEG;

            g_dollycam_model.add(gametime_globals->elapsed_ticks, &value, &attribute);
        }

    } while (false);

    ImGui::End();
}
