#pragma once

/* MANAGERS */
#include "ElkGameEngine/Managers/EngineManager.h"
#include "ElkGameEngine/Managers/WindowManager.h"
#include "ElkGameEngine/Managers/SceneManager.h"
#include "ElkGameEngine/Managers/InputManager.h"
#include <ElkRendering/Managers/RenderingManager.h>
#include <ElkPhysics/Managers/PhysicsManager.h>

/* OBJECTS */
#include "ElkGameEngine/Objects/GameObject.h"
#include "ElkGameEngine/Objects/Components/Behaviours/AButton.h"
#include "ElkGameEngine/Objects/Components/Behaviours/Rigidbody.h"
#include "ElkGameEngine/Objects/Components/Behaviours/Chunk.h"
#include "ElkGameEngine/Objects/Components/Behaviours/UICamera.h"
#include "ElkGameEngine/Objects/Components/Behaviours/Camera.h"
#include "ElkGameEngine/Objects/Components/Behaviours/AudioListener.h"
#include "ElkGameEngine/Objects/Components/Behaviours/Sprite.h"
#include "ElkGameEngine/Objects/Components/Behaviours/Text.h"
#include "ElkGameEngine/Objects/Components/Transform.h"

/* TOOLS */
#include <ElkTools/Debug/Log.h>
#include <ElkTools/Debug/Assertion.h>
#include <ElkTools/Utils/Time.h>
#include <ElkTools/Utils/Benchmark.h>
#include <ElkTools/Utils/Profiler.h>
#include <ElkTools/Utils/Date.h>
#include <ElkTools/Utils/Random.h>
#include <ElkTools/Utils/BlocksInfo.h>
#include <ElkTools/Utils/Math.h>
#include <ElkTools/Utils/ManagerLocator.h>
#include <ElkTools/Managers/AudioManager.h>