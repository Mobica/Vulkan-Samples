/* Copyright (c) 2023, Arm Limited and Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 the "License";
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gltfskinning.h"

gltfskinning::gltfskinning()
{

	// lrm - sascha
	/*
	camera.type  = Camera::CameraType::lookat;
	camera.flipY = true;
	camera.setPosition(glm::vec3(0.0f, 0.75f, -2.0f));
	camera.setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
	camera.setPerspective(60.0f, (float) width / (float) height, 0.1f, 256.0f);
	*/

	camera.type = vkb::CameraType::LookAt;
	camera.set_position(glm::vec3(0.0f, 0.75f, -2.0f));
	camera.set_rotation(glm::vec3(0.0f, 0.0f, 0.0f));
	camera.set_perspective(60.0f, static_cast<float>(width) / static_cast<float>(height), 0.1f, 256.0f);
}

gltfskinning::~gltfskinning()
{
	if (device)
	{
		vkDestroyPipeline(get_device().get_handle(), sample_pipeline, nullptr);
		vkDestroyPipelineLayout(get_device().get_handle(), sample_pipeline_layout, nullptr);
	}
}

// lrm TODO
void gltfskinning::setup_descriptor_pool()
{


}

// lrm TODO
void gltfskinning::setup_descriptor_set_layout()
{


}

// lrm TODO
void gltfskinning::setup_descriptor_sets()
{


}

void gltfskinning::prepare_uniform_buffers()
{

	// lrm - sascha:
	/*
	VK_CHECK_RESULT(vulkanDevice->createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &shaderData.buffer, sizeof(shaderData.values)));
	VK_CHECK_RESULT(shaderData.buffer.map());
	updateUniformBuffers();
	*/
	// end Sascha

	uniform_buffers.scene = std::make_unique<vkb::core::Buffer>(get_device(),
	                                                            sizeof(ubo_values),
	                                                            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
	                                                            VMA_MEMORY_USAGE_CPU_TO_GPU);	// lrm TODO  is this equibalent to HOST_VISIBLE | HOST_COHERENT?

	update_uniform_buffers();
}

bool gltfskinning::prepare(const vkb::ApplicationOptions &options)
{
	if (!ApiVulkanSample::prepare(options))
	{
		return false;
	}

	load_scene("assets\\scenes\\CesiumMan\\glTF-Embedded\\CesiumMan.gltf");

	/* sascha - 
	VulkanExampleBase::prepare();
	loadAssets();
	prepareUniformBuffers();
	setupDescriptors();
	preparePipelines();
	buildCommandBuffers();
	prepared = true;
	*/

	prepare_uniform_buffers();
	setup_descriptor_set_layout();
	prepare_pipelines();
	setup_descriptor_pool();
	setup_descriptor_sets();
	build_command_buffers();
	prepared = true;
	return true;
}

void gltfskinning::prepare_pipelines()
{
	// Specify we will use triangle lists to draw geometry.
	VkPipelineInputAssemblyStateCreateInfo input_assembly_state =
		vkb::initializers::pipeline_input_assembly_state_create_info(
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			0,
			VK_FALSE);

	// Specify rasterization state.
	VkPipelineRasterizationStateCreateInfo rasterization_state =
		vkb::initializers::pipeline_rasterization_state_create_info(
			VK_POLYGON_MODE_FILL,
			VK_CULL_MODE_BACK_BIT,
			VK_FRONT_FACE_COUNTER_CLOCKWISE);

	// Our attachment will write to all color channels, but no blending is enabled.
	VkPipelineColorBlendAttachmentState blend_attachment_state =
		vkb::initializers::pipeline_color_blend_attachment_state(
			VK_COLOR_COMPONENT_R_BIT |
			VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT,
			VK_FALSE);

	VkPipelineColorBlendStateCreateInfo color_blend_state =
		vkb::initializers::pipeline_color_blend_state_create_info(
			1,
			&blend_attachment_state);

	// We will have one viewport and scissor box.
	VkPipelineViewportStateCreateInfo viewport_state =
		vkb::initializers::pipeline_viewport_state_create_info(1, 1, 0);

	// Enable depth testing (using reversed depth-buffer for increased precision).
	VkPipelineDepthStencilStateCreateInfo depth_stencil_state =
		vkb::initializers::pipeline_depth_stencil_state_create_info(
			VK_TRUE,
			VK_TRUE,
			VK_COMPARE_OP_LESS_OR_EQUAL);

	// No multisampling.
	VkPipelineMultisampleStateCreateInfo multisample_state =
		vkb::initializers::pipeline_multisample_state_create_info(
			VK_SAMPLE_COUNT_1_BIT,
			0);

	// Specify that these states will be dynamic, i.e. not part of pipeline state object.
	std::array<VkDynamicState, 2>    dynamics{VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
	VkPipelineDynamicStateCreateInfo dynamic_state =
		vkb::initializers::pipeline_dynamic_state_create_info(
			dynamics.data(),
			vkb::to_u32(dynamics.size()),
			0);

	// Vertex bindings and attributes:
	// Binding description
	std::vector<VkVertexInputBindingDescription> vertex_input_bindings = {
	    vkb::initializers::vertex_input_binding_description(0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX),
	};

	// Attribute descriptions
	std::vector<VkVertexInputAttributeDescription> vertex_input_attributes = {
	    vkb::initializers::vertex_input_attribute_description(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos)),			// Position
	    vkb::initializers::vertex_input_attribute_description(0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)),      // Normal
	    vkb::initializers::vertex_input_attribute_description(0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv)),             // UV
	    // lrm TODO colour, jointIndices, jointWeights:
	    //vkb::initializers::vertex_input_attribute_description(0, 3, VK_FORMAT_R32G32_SFLOAT, sizeof(float) * 6),              // lrm the api sample Vertex struct has no colour member
	  	vkb::initializers::vertex_input_attribute_description(0, 4, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, joint0)),   // joint indices
	    vkb::initializers::vertex_input_attribute_description(0, 5, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, weight0)),  // joint weights
	};	

	VkPipelineVertexInputStateCreateInfo vertex_input_state = vkb::initializers::pipeline_vertex_input_state_create_info();
	vertex_input_state.vertexBindingDescriptionCount        = static_cast<uint32_t>(vertex_input_bindings.size());
	vertex_input_state.pVertexBindingDescriptions           = vertex_input_bindings.data();
	vertex_input_state.vertexAttributeDescriptionCount      = static_cast<uint32_t>(vertex_input_attributes.size());
	vertex_input_state.pVertexAttributeDescriptions         = vertex_input_attributes.data();


	// Load our SPIR-V shaders.
	std::array<VkPipelineShaderStageCreateInfo, 2> shader_stages{};

	// Vertex stage of the pipeline
	shader_stages[0] = load_shader("skinnedmodel.vert", VK_SHADER_STAGE_VERTEX_BIT);	// lrm TODO - Sascha loads .spv
	shader_stages[1] = load_shader("skinnedmodel.frag", VK_SHADER_STAGE_FRAGMENT_BIT);

	// We need to specify the pipeline layout and the render pass description up front as well.
	VkGraphicsPipelineCreateInfo pipeline_create_info =
		vkb::initializers::pipeline_create_info(
			sample_pipeline_layout,
			render_pass,
			0);

	pipeline_create_info.pVertexInputState            = &vertex_input_state;
	pipeline_create_info.pInputAssemblyState          = &input_assembly_state;
	pipeline_create_info.pRasterizationState          = &rasterization_state;
	pipeline_create_info.pColorBlendState             = &color_blend_state;
	pipeline_create_info.pMultisampleState            = &multisample_state;
	pipeline_create_info.pViewportState               = &viewport_state;
	pipeline_create_info.pDepthStencilState           = &depth_stencil_state;
	pipeline_create_info.pDynamicState                = &dynamic_state;
	pipeline_create_info.stageCount                   = vkb::to_u32(shader_stages.size());
	pipeline_create_info.pStages                      = shader_stages.data();

	// solid rendering pipeline
	VK_CHECK(vkCreateGraphicsPipelines(get_device().get_handle(), pipeline_cache, 1, &pipeline_create_info, nullptr, &sample_pipeline));

	// lrm TODO leaving out wireframe pipeline for simplicity
}

void gltfskinning::build_command_buffers()
{
	VkCommandBufferBeginInfo command_buffer_begin_info = vkb::initializers::command_buffer_begin_info();

	// Clear color and depth values.
	VkClearValue clear_values[2];
	clear_values[0].color        = {{0.0f, 0.0f, 0.0f, 0.0f}};
	clear_values[1].depthStencil = {0.0f, 0};

	// Begin the render pass.
	VkRenderPassBeginInfo render_pass_begin_info    = vkb::initializers::render_pass_begin_info();
	render_pass_begin_info.renderPass               = render_pass;
	render_pass_begin_info.renderArea.offset.x      = 0;
	render_pass_begin_info.renderArea.offset.y      = 0;
	render_pass_begin_info.renderArea.extent.width  = width;
	render_pass_begin_info.renderArea.extent.height = height;
	render_pass_begin_info.clearValueCount          = 2;
	render_pass_begin_info.pClearValues             = clear_values;

	for (int32_t i = 0; i < draw_cmd_buffers.size(); ++i)
	{
		auto cmd = draw_cmd_buffers[i];

		// Begin command buffer.
		vkBeginCommandBuffer(cmd, &command_buffer_begin_info);

		// Set framebuffer for this command buffer.
		render_pass_begin_info.framebuffer = framebuffers[i];
		// We will add draw commands in the same command buffer.
		vkCmdBeginRenderPass(cmd, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

		// Bind the graphics pipeline.
		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, sample_pipeline);

		// Set viewport dynamically
		VkViewport viewport = vkb::initializers::viewport(static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f);
		vkCmdSetViewport(cmd, 0, 1, &viewport);

		// Set scissor dynamically
		VkRect2D scissor = vkb::initializers::rect2D(width, height, 0, 0);
		vkCmdSetScissor(cmd, 0, 1, &scissor);

		// Draw three vertices with one instance.
		vkCmdDraw(cmd, 3, 1, 0, 0);

		// Draw user interface.
		draw_ui(draw_cmd_buffers[i]);

		// Complete render pass.
		vkCmdEndRenderPass(cmd);

		// Complete the command buffer.
		VK_CHECK(vkEndCommandBuffer(cmd));
	}
}


void gltfskinning::update_uniform_buffers()
{
	/* sascha - 
	shaderData.values.projection = camera.matrices.perspective;
	shaderData.values.model      = camera.matrices.view;
	memcpy(shaderData.buffer.mapped, &shaderData.values, sizeof(shaderData.values));
	*/

	ubo_values.projection = camera.matrices.perspective;
	ubo_values.modelview = camera.matrices.view;
	uniform_buffers.scene->convert_and_update(ubo_values);
}

void gltfskinning::render(float delta_time)
{
	/* sascha
	renderFrame();
	if (camera.updated)
	{
		updateUniformBuffers();
	}
	// POI: Advance animation
	if (!paused)
	{
		glTFModel.updateAnimation(frameTimer);
	}
	*/

	if (!prepared)
	{
		return;
	}
	ApiVulkanSample::prepare_frame();
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers    = &draw_cmd_buffers[current_buffer];
	VK_CHECK(vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE));
	ApiVulkanSample::submit_frame();

	if (camera.updated)
	{
		update_uniform_buffers();
	}
	// POI: advance animation
	if (!paused)
	{
		update(delta_time);		// TODO - sascha just updates the animation here
	}	

}

/* lrm TODO leave out for simplicity
void gltfskinning::on_update_ui_overlay(vkb::Drawer &drawer)
{
	if (drawer.header("Settings"))
	{
		if (drawer.checkbox("Wireframe", &wireframe))
		{
			build_command_buffers();
		}
	}
}
*/

bool gltfskinning::resize(const uint32_t width, const uint32_t height)
{
	ApiVulkanSample::resize(width, height);
	update_uniform_buffers();
	return true;
}

std::unique_ptr<vkb::VulkanSample> create_gltfskinning()
{
	return std::make_unique<gltfskinning>();
}
