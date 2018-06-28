#shader vertex
#version 430 core

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec2 vertexTexCoords;

layout(std430, binding = 0) buffer StringData
{
    uint content[];
};

/* Uniforms */
uniform mat4 u_view_projection;
uniform vec3 u_transform_position;
uniform vec3 u_transform_scale;
uniform vec4 u_font_color;
uniform float u_spacing;
uniform sampler2D u_textureAtlas;

/* Constants */
const ivec2 c_atlasSize = textureSize(u_textureAtlas, 0);
const float c_textureXResolution = 8.f;
const float c_textureYResolution = 8.f;
const float c_textureWidth = c_atlasSize.x / c_textureXResolution;
const float c_textureHeight = c_atlasSize.y / c_textureYResolution;
const float c_texelBleedingOffset = 0.00001f;

/* Out */
out vec2 v_texCoord;
out vec4 v_font_color;

/* Globals */
vec3 g_position;
mat4 g_model;

mat4 CalculateModelMatrix()
{
    vec3 translation = u_transform_position + vec3(0.f, 0.f, 0.f);
    return mat4(u_transform_scale.x, 0.0, 0.0, 0.0,  0.0, u_transform_scale.y, 0.0, 0.0,  0.0, 0.0, u_transform_scale.z, 0.0, translation.x, translation.y, translation.z, 1.0);
}

vec3 CalculateWorldPosition()
{
    return vec3(g_model * vec4(vertexPosition + vec3(float(gl_InstanceID) * (1.f + u_spacing), 0.f, 0.f), 1.0));
}

vec4 CalculateScreenPosition()
{
    return u_view_projection * vec4(g_position, 1.0);
}

vec2 GetTexture(uint p_x, uint p_y)
{
    vec2 result = vertexTexCoords;
    
    result.x /= c_textureWidth;
    result.y /= c_textureHeight;

    result.x += p_x / c_textureWidth;
    result.y += (c_textureHeight - 1 - p_y) / c_textureHeight;

    if (vertexTexCoords.x == 0)
        result.x += c_texelBleedingOffset;
    if (vertexTexCoords.x == 1)
        result.x -= c_texelBleedingOffset;

    if (vertexTexCoords.y == 0)
        result.y += c_texelBleedingOffset;
    if (vertexTexCoords.y == 1)
        result.y -= c_texelBleedingOffset;

    return result;
}

vec2 CalculateAtlasTextureCoordinates()
{
    uint index = content[gl_InstanceID];
    return GetTexture(index % 16, index / 16);
}

void main()
{
    g_model = CalculateModelMatrix();
    g_position = CalculateWorldPosition();

    gl_Position = CalculateScreenPosition();

    v_texCoord = CalculateAtlasTextureCoordinates();
    v_font_color = u_font_color;
}

#shader fragment
#version 330 core

/* Uniforms */
uniform sampler2D u_textureAtlas;

/* In */
in vec2 v_texCoord;
in vec4 v_font_color;

/* Out */
out vec4 FragColor;

void main()
{
    FragColor = vec4(texture2D(u_textureAtlas, v_texCoord)) * v_font_color;
    if (FragColor.a == 0.0f) discard;
}