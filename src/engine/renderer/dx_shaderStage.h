#pragma once

#include "dx.h"

typedef struct {
	qboolean		active;

	textureBundle_t	bundle[NUM_TEXTURE_BUNDLES];

	waveForm_t		rgbWave;
	colorGen_t		rgbGen;

	waveForm_t		alphaWave;
	alphaGen_t		alphaGen;

	byte			constantColor[4];			// for CGEN_CONST and AGEN_CONST

	unsigned		stateBits;					// GLS_xxxx mask

	acff_t			adjustColorsForFog;

	qboolean		isDetail;

	// VULKAN
//	VkPipeline		vk_pipeline = VK_NULL_HANDLE;
//	VkPipeline		vk_portal_pipeline = VK_NULL_HANDLE;
//	VkPipeline		vk_mirror_pipeline = VK_NULL_HANDLE;

	// DX12
	ID3D12PipelineState* dx_pipeline = nullptr;
	ID3D12PipelineState* dx_portal_pipeline = nullptr;
	ID3D12PipelineState* dx_mirror_pipeline = nullptr;

} shaderStage_t;