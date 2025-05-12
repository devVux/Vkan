#pragma once

#include "RenderPass.h"
#include "GPU.h"

class GraphicsPipelineBuilder {

public:

	GraphicsPipeline create(const RenderPass& renderpass, const GPU& gpu) const;

};