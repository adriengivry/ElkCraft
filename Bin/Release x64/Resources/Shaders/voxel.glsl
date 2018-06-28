#shader vertex
#version 430 core

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec2 vertexTexCoords;
layout (location = 2) in vec3 vertexNormal;

layout(std430, binding = 0) buffer BlocksData
{
    uint data[];
};

/* Uniforms */
uniform mat4        u_view;
uniform mat4        u_projection;
uniform vec3        u_transform_position;
uniform vec3        u_camera_position;
uniform int         u_render_mode;
uniform float       u_time_since_start;
uniform float       u_daytime_light_coefficient;
uniform vec3        u_destruction_position;
uniform int         u_destruction_level;
uniform sampler2D   u_textureAtlas;

/* Constants */
const vec3  c_chunkHalfSize = vec3(8.f, 8.f, 8.f);
const vec3  c_skyColor = vec3(1.0f, 1.0f, 1.0f);
const float c_fogDensity = 0.0022f;
const float c_fogGradient = 3.0f;
const ivec2 c_atlasSize = textureSize(u_textureAtlas, 0);
const float c_textureResolution = 16.0f;
const float c_textureWidth = c_atlasSize.x / c_textureResolution;
const float c_textureHeight = c_atlasSize.y / c_textureResolution;
const float c_texelBleedingOffset = 0.00001f;

/* Animation constants */
const float c_waterMovementAmplitude = 0.02f;
const float c_waterMovementFrequency = 1.0f;
const float c_flowerMovementXAmplitude = 0.1f;
const float c_flowerMovementXFrequency = 2.0f;
const float c_flowerMovementYAmplitude = 0.05f;
const float c_flowerMovementYFrequency = 0.5f;
const float c_flowerMovementZAmplitude = 0.1f;
const float c_flowerMovementZFrequency = 2.0f;
const float c_leavesMovementXAmplitude = 0.05f;
const float c_leavesMovementXFrequency = 1.5f;
const float c_leavesMovementZAmplitude = 0.03f;
const float c_leavesMovementZFrequency = 1.2f;

/* Out */
        out vec2    v_texCoord;
        out vec2    v_destroyTexCoord;
flat    out float   v_obscurity;
flat    out uint    v_blockType;
flat    out float   v_visiblity;
flat    out int     v_renderMode;
flat    out vec3    v_normalColor;
flat    out vec3    v_skyColor;

/* Globals */
vec3 g_position;
mat4 g_model;
vec3 g_blockLocalPosition;
vec3 g_blockWorldPosition;
vec3 g_vertexTranslation;

void Setup()
{
    v_texCoord = vec2(0.f, 0.f);
    v_obscurity = 1.0f;
    v_blockType = 0;
    v_visiblity = 1.0f;
    v_renderMode = 0;
    v_normalColor = vec3(0.f, 0.f, 0.f);
    g_position = vec3(0.f, 0.f, 0.f);
    g_blockLocalPosition = vec3(0.f, 0.f, 0.f);
    g_blockWorldPosition = vec3(0.f, 0.f, 0.f);
}

vec3 CalculateBlockLocalPosition()
{
    float posX = float(data[gl_InstanceID] % 16);
    float posY = float((data[gl_InstanceID] >> 4) % 16);
    float posZ = float((data[gl_InstanceID] >> 8) % 16);

    return vec3(posX, posY, posZ);
}

vec3 CalculateBlockWorldPosition()
{
    return u_transform_position + g_blockLocalPosition - c_chunkHalfSize;
}

uint CalculateBlockType()
{
    return (data[gl_InstanceID] >> 20) % 256;
}

uint CalculateLightLevel()
{
    return (data[gl_InstanceID] >> 12) % 16;
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

bool IsFlat() 
{ 
    switch (v_blockType) 
    { 
        case 15: return true; 
        case 16: return true; 
    } 
 
    return false; 
} 

bool IsWater()
{
    return v_blockType == 13;
}
 
bool IsFlower()
{
    return v_blockType == 15 || v_blockType == 16;
}

bool IsLeaves()
{
    return v_blockType == 12;
}

float GetBlockHeight()
{
    return 1.f;
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

vec2 GetTripleTexture(uint p_topX, uint p_topY, uint p_bottomX, uint p_bottomY, uint p_sideX, uint p_sideY)
{
            if (IsTopFace())       return GetTexture(p_topX,    p_topY);
    else    if (IsBottomFace())    return GetTexture(p_bottomX, p_bottomY);
    else    if (IsSideFace())      return GetTexture(p_sideX,   p_sideY);
}

vec2 CalculateAtlasTextureCoordinates()
{
    switch (v_blockType)
    {
        case 1: return GetTripleTexture(0,0, 2,0, 1,0);     // 01: GRASS
        case 2: return GetTexture(2,0);                     // 02: DIRT
        case 3: return GetTexture(3,1);                     // 03: GRAVEL
        case 4: return GetTexture(0,1);                     // 04: STONE
        case 5: return GetTexture(4,1);                     // 05: COBBLESTONE
        case 6: return GetTripleTexture(1,3, 1,3, 0,3);     // 06: WOOD
        case 7: return GetTexture(2,2);                     // 07: WOODEN_PLANKS
        case 8: return GetTexture(2,1);                     // 08: SAND
        case 9: return GetTexture(1,2);                     // 09: GLASS
        case 10: return GetTexture(2,3);                    // 10: BRICK
        case 11: return GetTexture(3,0);                    // 11: STONE_BRICK
        case 12: return GetTexture(4,0);                    // 12: LEAVES
        case 13: return GetTripleTexture(1,1, 5,5, 5,5);    // 13: WATER
        case 14: return GetTexture(0,2);                    // 14: LAVA
        case 15: return GetTripleTexture(5,5, 5,5, 3,2);    // 15: RED_FLOWER
        case 16: return GetTripleTexture(5,5, 5,5, 3,3);    // 16: YELLOW_FLOWER
        case 17: return GetTexture(5,0);                    // 17: COAL_ORE
    }

    return vec2(0, 0);
}

float CalculateVisibility()
{
    return clamp(exp(-pow((distance(g_blockWorldPosition, u_camera_position) * c_fogDensity), c_fogGradient)), 0.0f, 1.0f);
}

float CalculateMovement(float p_frequency, float p_amplitude, float p_offset, bool p_signed)
{
    float movement = 0;

    movement = cos((u_time_since_start + p_offset) / p_frequency) * p_amplitude;

    if (p_signed)
        movement += p_amplitude;
    
    return movement;
}

vec3 CalculateVertexTranslation()
{   
    vec3 translation = vec3(0.f, 0.f, 0.f);

    if (IsTopFace() || IsSideFace() && vertexPosition.y > 0)
        translation.y -= (1 - GetBlockHeight());

    if (IsFlat()) 
    { 
        if (IsSideFace() && vertexPosition.z > 0) 
            translation.z -= 0.5f; 
 
        if (IsSideFace() && vertexPosition.z < 0) 
            translation.z += 0.5f; 
    } 

    if (IsWater() && IsTopFace())
    {
        translation.y -= CalculateMovement(c_waterMovementFrequency, c_waterMovementAmplitude, 0, true);
    }

    if (IsFlower() && vertexPosition.y > 0)
    {
        float offset = (g_blockLocalPosition.x + g_blockLocalPosition.y + g_blockLocalPosition.z) * 5.0f;
        translation.x += CalculateMovement(c_flowerMovementXFrequency, c_flowerMovementXAmplitude, offset, false);
        translation.y += CalculateMovement(c_flowerMovementYFrequency, c_flowerMovementYAmplitude, offset, false);
        translation.z += CalculateMovement(c_flowerMovementZFrequency, c_flowerMovementZAmplitude, 0, false);
    }

    if (IsLeaves())
    {
        float offset = (g_blockLocalPosition.x + g_blockLocalPosition.y + g_blockLocalPosition.z) * 5.0f;
        float xMovement = CalculateMovement(c_leavesMovementXFrequency, c_leavesMovementXAmplitude, offset, false);
        float zMovement = CalculateMovement(c_leavesMovementZFrequency, c_leavesMovementZAmplitude, offset + 10, false);

        if (vertexPosition.y > 0)
        {
            translation.x += xMovement;
            translation.z += zMovement;
        }
        else
        {
            translation.x -= xMovement;
            translation.x -= zMovement;
        }
    }

    return translation;
}

mat4 CalculateModelMatrix()
{
    vec3 worldVertexPosition =  g_blockWorldPosition + g_vertexTranslation;
    mat4 result = mat4(1.0f, 0.0f, 0.0f, 0.0f, /**/  0.0f, 1.0f, 0.0f, 0.0f, /**/  0.0, 0.0, 1.0, 0.0, worldVertexPosition.x, worldVertexPosition.y, worldVertexPosition.z, 1.0f);
    return result;
}

vec3 CalculateWorldPosition()
{
    return vec3(g_model * vec4(vertexPosition, 1.0));
}

vec4 CalculateScreenPosition()
{
    return u_projection * u_view * vec4(g_position, 1.0);
}

float LightLevelToObscurity(uint p_lightLevel)
{
    if (p_lightLevel > 15)
        p_lightLevel = 15;

    return pow(0.8f, 15 - p_lightLevel);
}

vec2 CalculateDestroyTextureCoordinates()
{
    if (!IsFlat() && g_blockWorldPosition == u_destruction_position)
    {
        switch (u_destruction_level)
        {
            default:     return GetTexture(5,5);

            case 1:     return GetTexture(0,4);
            case 2:     return GetTexture(1,4);
            case 3:     return GetTexture(2,4);
            case 4:     return GetTexture(3,4);
            case 5:     return GetTexture(4,4);
            case 6:     return GetTexture(5,4);
            case 7:     return GetTexture(0,5);
            case 8:     return GetTexture(1,5);
            case 9:     return GetTexture(2,5);
            case 10:    return GetTexture(3,5);
        }
    }
    else
        return GetTexture(5, 5);
}

float CalculateObscurity()
{
    float result;
    float lightApplication = 1.0f;

    if (IsXFace())
        result = 0.9f;

    if (IsZFace())
        result = 0.8f;
    
    if (IsBottomFace())
        result = 0.6f;

    if (IsTopFace())
    {
        result = 1.1f;
        lightApplication = LightLevelToObscurity(CalculateLightLevel());
    }
        
    return result * lightApplication * u_daytime_light_coefficient;
}

vec3 CalculateNormalColor()
{
    return vertexNormal * 0.5f + 0.5f;
}

void main()
{
    Setup();

    v_renderMode = u_render_mode;

    v_blockType = CalculateBlockType();
    g_blockLocalPosition = CalculateBlockLocalPosition();
    g_blockWorldPosition = CalculateBlockWorldPosition();
    
    v_texCoord = CalculateAtlasTextureCoordinates();
    v_destroyTexCoord = CalculateDestroyTextureCoordinates();
    v_visiblity = CalculateVisibility();
    v_obscurity = CalculateObscurity();
    v_normalColor = CalculateNormalColor();
    v_skyColor = c_skyColor * u_daytime_light_coefficient;

    g_vertexTranslation = CalculateVertexTranslation();
    g_model = CalculateModelMatrix();
    g_position = CalculateWorldPosition();
    gl_Position = CalculateScreenPosition();

}

#shader fragment
#version 430 core

/* Uniforms */
uniform sampler2D u_textureAtlas;

/* Constants */
vec4 c_defaultColor = vec4(1.f, 0.f, 1.f, 1.f);

/* In */ 
        in vec2     v_texCoord;
        in vec2     v_destroyTexCoord;
flat    in float    v_obscurity;
flat    in uint     v_blockType;
flat    in float    v_visiblity;
flat    in int      v_renderMode;
flat    in vec3     v_normalColor;
flat    in vec3     v_skyColor;

/* Out */
out vec4 FragColor;

void RenderInWireframe()
{
    FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);
}

void RenderNormalColors()
{
    FragColor = vec4(v_normalColor, 1.f);
}

vec3 CalculateColorFilter()
{
    if (v_blockType == 1 || v_blockType == 2)
        return vec3(0.75f, 0.75f, 0.75f);
    else if (v_blockType == 12)
        return vec3(0.95f, 0.95f, 0.95f);
    else
        return vec3(1.f, 1.f, 1.f);
}

void RenderDefault()
{

    vec4 texture = texture2D(u_textureAtlas, v_texCoord) * vec4(CalculateColorFilter(), 1.f);
    vec4 destroyTexture = texture2D(u_textureAtlas, v_destroyTexCoord);
    
    FragColor = vec4(texture.xyz * destroyTexture.xyz * v_obscurity, texture.a);

    if (FragColor.a == 0.f && destroyTexture.a >= 0.1f)
        FragColor.a = 1.f;

    if (FragColor.a == 0.f) discard;

    FragColor = mix(vec4(v_skyColor, 1.0f), FragColor, v_visiblity);
}

void RenderError()
{
    FragColor = c_defaultColor;
}

void main()
{
    if (v_blockType == 0) discard;

    switch (v_renderMode)
    {
        case 0:
            RenderDefault();
            break;
        case 1:
            RenderInWireframe();
            break;
        case 2:
            RenderNormalColors();
            break;
        default:
            RenderError();
            break;
    }    
} 