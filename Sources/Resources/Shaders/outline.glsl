#shader vertex
#version 430 core

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec2 vertexTexCoords;
layout (location = 2) in vec3 vertexNormal;

/* Uniforms */
uniform mat4 u_view;
uniform mat4 u_projection;
uniform vec3 u_position;

/* Constants */
float c_outlineScale = 1.005f;

/* Out */
out vec2    v_texCoord;

/* Globals */
vec3 g_position;
mat4 g_model;

void Setup()
{
    v_texCoord = vec2(0.f, 0.f);
}

bool IsTopFace()
{
    return vertexNormal.y > 0;
}

bool IsBottomFace()
{
    return vertexNormal.y < 0;
}

bool IsSideFace()
{
    return vertexNormal.y == 0;
}

bool IsXFace()
{
    return vertexNormal.y == 0 && vertexNormal.x != 0;
}

bool IsZFace()
{
    return vertexNormal.y == 0 && vertexNormal.z != 0;
}

mat4 CalculateModelMatrix()
{
    vec3 translation = u_position;
    mat4 result = mat4(1.0, 0.0, 0.0, 0.0,  0.0, 1.0, 0.0, 0.0,  0.0, 0.0, 1.0, 0.0, translation.x, translation.y, translation.z, 1.0);
    return result;
}

vec3 CalculateWorldPosition()
{
    return vec3(g_model * vec4(vertexPosition * c_outlineScale, 1.0));
}

vec4 CalculateScreenPosition()
{
    return u_projection * u_view * vec4(g_position, 1.0);
}

void main()
{
    Setup();
    g_model = CalculateModelMatrix();
    g_position = CalculateWorldPosition();
    v_texCoord = vertexTexCoords;
    gl_Position = CalculateScreenPosition();
}

#shader fragment
#version 430 core

/* In */ 
in vec2     v_texCoord;

/* Out */
out vec4 FragColor;

void main()
{
    FragColor = vec4(0.f, 0.f, 0.f, 0.5f);
} 