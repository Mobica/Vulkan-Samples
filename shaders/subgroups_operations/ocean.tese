#version 450

/* Copyright (c) 2023, Mobica Limited
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
 layout(quads, equal_spacing, ccw) in;


 layout (binding = 0) uniform Ubo 
{
	mat4 projection;
	mat4 view;
	mat4 model;
} ubo;


 void main()
 {
	vec4 p00 = gl_in[0].gl_Position;
	vec4 p01 = gl_in[1].gl_Position;
	vec4 p10 = gl_in[2].gl_Position;
	vec4 p11 = gl_in[3].gl_Position;

	vec4 p0 = (p01 - p00) * gl_TessCoord.x + p00;
	vec4 p1 = (p11 - p10) * gl_TessCoord.x + p10;

	vec4 p = (p1 - p0) * gl_TessCoord.y + p0;

	gl_Position = ubo.projection * ubo.view * ubo.model * p;
 }