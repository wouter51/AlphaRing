#pragma once

class c_imgui_render {
public:
	static void main();
	static void halo1();
	static void halo2();
	static void halo3();
	static void halo4();
	static void groundhog();
	static void halo3odst();
	static void haloreach();

	inline static void(*game_render[])(void) {
		halo1,
		halo2,
		halo3,
		halo4,
		groundhog,
		halo3odst,
		haloreach
	};
};
