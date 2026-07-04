#include "../core/gpu.hpp"
#include <windows.h>
#include <d3d9.h>

#include "../core/gpu.hpp"

bool SetupScene() {
	Gpu::device->SetRenderState(D3DRS_LIGHTING, false);
	return true;
}