#include "render_player_view.h"

#include "../../halo3.h"

#include "../../../dollycam/dollycam_game_view.h"

using namespace libmcc;
using namespace libmcc::halo3;

struct s_tag_cache {
	s_tag_cache() {
		header = &g_cache_file_globals()->header;
		tag_cache_minimum_address = reinterpret_cast<int*>(physical_memory_globals()->allocation_base_address - k_tag_cache_minimum_address);
	}

	template<typename T>
	std::initializer_list<T> get_tag_block(c_typed_tag_block<T> block) {
		auto block_base_address = reinterpret_cast<T*>(tag_cache_minimum_address + block.address);
		return std::initializer_list<T>(block_base_address, block_base_address + block.count);
	}

	template<typename T>
	T* get_tag_reference(c_typed_tag_reference<T> reference) {
		if (reference.index == -1) {
			return nullptr;
		}

		auto element = header->tags_header->tag_instances.m_elements + reference.index;

		if (element->address == 0) {
			return nullptr;
		}

		return reinterpret_cast<T*>(tag_cache_minimum_address + element->address);
	}

	s_cache_file_header* header;
	int* tag_cache_minimum_address;
};

namespace halo3 {
	static void player_view_render(c_player_view* view);

	c_view_vftable<c_player_view> g_player_view_vftable {
		.render = player_view_render,
	};

	static void player_view_render(c_player_view* view) {
		g_player_view_vftable_original.render(view);

		g_dollycam_game_view.render();

		do {
			if (!g_halo3_options.debug_structure && !g_halo3_options.debug_instanced_geometry) {
				break;
			}

			auto tls = g_thread_local_storage();

			if (!game_in_progress()) {
				break;
			}

			auto scnr = global_scenario();

			if (scnr == nullptr) {
				break;
			}

			s_tag_cache tag_cache;
			c_render_debug_line_drawer drawer;

			if (g_halo3_options.debug_structure) {
				drawer.set_color(&g_halo3_options.debug_structure_color);

				auto bsp_index = 0;

				auto active_bsp_mask = tls->game_globals->active_structure_bsp_mask;

				for (auto& structure_bsp_block : tag_cache.get_tag_block(scnr->structure_bsps)) {
					if ((active_bsp_mask & (1 << bsp_index++)) == 0) {
						continue;
					}

					auto structure_bsp = tag_cache.get_tag_reference(structure_bsp_block.structure_bsp);

					if (structure_bsp == nullptr) {
						continue;
					}

					auto structure_bsp_resources = structure_bsp->resource_interface.get_resources();

					for (auto& collision_bsp : tag_cache.get_tag_block(structure_bsp_resources->collision_bsp)) {
						auto vertices = tag_cache.get_tag_block(collision_bsp.vertices).begin();

						for (auto& edge : tag_cache.get_tag_block(collision_bsp.edges)) {
							auto v0 = &vertices[edge.vertex_indices[0]].point;
							auto v1 = &vertices[edge.vertex_indices[1]].point;

							drawer.add_line_3d(v0, v1);
						}
					}
				}

				drawer.flush();
			}

			if (g_halo3_options.debug_instanced_geometry) {
				drawer.set_color(&g_halo3_options.debug_instanced_geometry_color);

				for (auto& visible_instance : c_visible_items::m_item()->instances) {
					auto instance_index = visible_instance.instanced_geometry_instances_index;

					auto bsp_index = visible_instance.structure_bsp_index;

					auto bsp = tag_cache.get_tag_reference((tag_cache.get_tag_block(scnr->structure_bsps).begin() + bsp_index)->structure_bsp);

					auto instance = tag_cache.get_tag_block(bsp->instanced_geometry_instances).begin() + instance_index;

					auto instance_matrix = reinterpret_cast<const real_matrix4x3*>(&instance->scale);

					auto instance_definition_index = instance->instance_definition;

					if (instance_definition_index == -1) {
						continue;
					}

					auto bsp_resource = bsp->resource_interface.get_resources();

					auto instance_definition = tag_cache.get_tag_block(bsp_resource->instanced_geometries_definitions).begin() + instance_definition_index;

					auto vertices = tag_cache.get_tag_block(instance_definition->vertices).begin();

					for (auto& edge : tag_cache.get_tag_block(instance_definition->edges)) {
						auto v0 = instance_matrix->transform(vertices[edge.vertex_indices[0]].point);
						auto v1 = instance_matrix->transform(vertices[edge.vertex_indices[1]].point);

						drawer.add_line_3d(&v0, &v1);
					}
				}

				drawer.flush();
			}
		} while (false);
	}
}
