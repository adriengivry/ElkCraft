#shader vertex
#version 330 core

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec2 vertexTexCoords;

/* Uniforms */
uniform mat4 u_view_projection;
uniform vec3 u_transform_position;
uniform vec3 u_transform_scale;
uniform vec4 u_color;

/* Out */
out vec2 v_texCoord;
out vec4 v_color;

/* Globals */
vec3 g_position;
mat4 g_model;

mat4 CalculateModelMatrix()
{
    return mat4(u_transform_scale.x, 0.0, 0.0, 0.0,  0.0, u_transform_scale.y, 0.0, 0.0,  0.0, 0.0, u_transform_scale.z, 0.0, u_transform_position.x, u_transform_position.y, u_transform_position.z, 1.0);
}

vec3 CalculateWorldPosition()
{
    return vec3(g_model * vec4(vertexPosition, 1.0));
}

vec4 CalculateScreenPosition()
{
    return u_view_projection * vec4(g_position, 1.0);
}

void main()
{
    g_model = CalculateModelMatrix();
    g_position = CalculateWorldPosition();

    gl_Position = CalculateScreenPosition();

    v_texCoord = vertexTexCoords;
    v_color = u_color;
}

#shader fragment
#version 330 core

/* Uniforms */
uniform sampler2D u_texture;

/* In */
in vec2 v_texCoord;
in vec4 v_color;

/* Out */
out vec4 FragColor;

void main()
{
    FragColor = vec4(texture2D(u_texture, v_texCoord)) * v_color;
    if (FragColor.a == 0.0f)
        discard;
}