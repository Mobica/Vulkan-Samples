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

layout (triangles, equal_spacing, ccw) in;

layout (location = 0) in vec2 inUv[];

layout (location = 0) out vec4 outPos;
layout (location = 1) out vec2 outUV;

layout (binding = 0) uniform Ubo
{
    mat4 projection;
    mat4 view;
    mat4 model;
} ubo;

layout (binding = 1, rgba32f) uniform image2D fft_displacement_map;

layout (binding = 2) uniform TessellationParams
{
    float choppines;
    float displacement_scale;
} tessParams;

vec2 interpolate_2d(vec2 v0, vec2 v1, vec2 v2)
{
    return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}

vec3 interpolate_3d(vec3 v0, vec3 v1, vec3 v2)
{
    return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}

vec4 interpolate_4d(vec4 v0, vec4 v1, vec4 v2)
{
    return vec4(gl_TessCoord.x) * v0 + vec4(gl_TessCoord.y) * v1 + vec4(gl_TessCoord.z) * v2;
}

void main()
{
    vec3 world_pos = interpolate_3d(gl_in[0].gl_Position.xyz, gl_in[1].gl_Position.xyz, gl_in[2].gl_Position.xyz);

    vec4 fft_texel_at_vertex[3];
    fft_texel_at_vertex[0] = imageLoad(fft_displacement_map, ivec2(inUv[0]));
    fft_texel_at_vertex[1] = imageLoad(fft_displacement_map, ivec2(inUv[1]));
    fft_texel_at_vertex[2] = imageLoad(fft_displacement_map, ivec2(inUv[2]));

    vec4 fft_texel = interpolate_4d(fft_texel_at_vertex[0], fft_texel_at_vertex[1], fft_texel_at_vertex[2]);

    world_pos.y += fft_texel.y * tessParams.displacement_scale;
    world_pos.x -= fft_texel.x * tessParams.choppines;
    world_pos.z -= fft_texel.z * tessParams.choppines;

    outUV = interpolate_2d(inUv[0], inUv[1], inUv[2]) / 256;
    outPos = ubo.view * vec4(world_pos, 1.0f);
    gl_Position = ubo.projection * ubo.view * ubo.model * vec4(world_pos, 1.0f);
}