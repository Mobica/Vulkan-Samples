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

#pragma once

#include "api_vulkan_sample.h"

class gltfskinning : public ApiVulkanSample
{
  public:
  /* lrm TODO leave out for simplicity
  bool wireframe          = false;
  */


  /* sascha - 
  struct Vertex
  {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec3 color;
    glm::vec4 jointIndices;
    glm::vec4 jointWeights;
  } vertex;    
  */

  struct UniformBuffers
  {
    std::unique_ptr<vkb::core::Buffer> scene; // lrm TODO name appropriately
  } uniform_buffers;

  struct
  {
    glm::mat4 projection;
    glm::mat4 modelview;
    glm::vec4 light_pos = glm::vec4(5.0f, 5.0f, 5.0f, 1.0f);  
  } ubo_values;

  /* sascha uses 2 pipelines -
  VkPipelineLayout pipelineLayout;
  struct Pipelines
  {
    VkPipeline solid;
    VkPipeline wireframe = VK_NULL_HANDLE;
  } pipelines;
  */



	gltfskinning();
	virtual ~gltfskinning();

	// Create pipeline
	void prepare_pipelines();

  void setup_descriptor_pool();
  void setup_descriptor_set_layout();
  void setup_descriptor_sets();
  void prepare_uniform_buffers();
  void update_uniform_buffers();

	// Override basic framework functionality
	void build_command_buffers() override;
	void render(float delta_time) override;
	bool prepare(const vkb::ApplicationOptions &options) override;


  /* lrm may leave this out for simplicity
  virtual void on_update_ui_overlay(vkb::Drawer &drawer) override;
  */

  virtual bool resize(const uint32_t width, const uint32_t height) override;

  private:
	// Sample specific data
	VkPipeline       sample_pipeline{};
	VkPipelineLayout sample_pipeline_layout{};
};

std::unique_ptr<vkb::VulkanSample> create_gltfskinning();
