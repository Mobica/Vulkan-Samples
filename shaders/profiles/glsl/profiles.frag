#version 450
/* Copyright (c) 2022-2024, Sascha Willems
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

#extension GL_EXT_nonuniform_qualifier : require

layout (set = 0, binding = 1) uniform sampler2D textures[];

layout (location = 0) in vec2 inUV;
layout (location = 1) flat in int inTexIndex;

layout (location = 0) out vec4 outFragColor;

void main() 
{
	outFragColor = texture(textures[nonuniformEXT(inTexIndex)], inUV);	
}