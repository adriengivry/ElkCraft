#include "stdafx.h"

#include "ElkCraft/System/GameStateMenu.h"

using namespace ElkTools::Utils;
using namespace ElkTools::Managers;
using namespace ElkGameEngine::Managers;
using namespace ElkGameEngine::Objects::Components;
using namespace ElkGameEngine::Objects::Components::Behaviours;
using namespace ElkRendering::Managers;
using namespace ElkPhysics::Managers;
using namespace ElkCraft::Terrain;
using namespace ElkCraft::Gameplay;
using namespace ElkCraft::UI;
using namespace ElkCraft::System;

ElkCraft::System::GameStateMenu::GameStateMenu(ElkGameEngine::Managers::EngineManager & p_engineManager) :
	m_engineManager(p_engineManager),
	m_sceneManager(ManagerLocator::Get<SceneManager>()),
	m_windowManager(ManagerLocator::Get<WindowManager>()),
	m_textureManager(ManagerLocator::Get<TextureManager>()),
	m_physicsManager(ManagerLocator::Get<PhysicsManager>()),
	m_inputManager(ManagerLocator::Get<InputManager>()),
	m_audioManager(ManagerLocator::Get<AudioManager>()),
	m_windowWidth(static_cast<float>(m_windowManager.GetWidth())),
	m_windowHeight(static_cast<float>(m_windowManager.GetHeight()))
{
	m_sceneManager.CreateScene("Menu");

	CreateGameObjects();
	InitGameObjects();

	m_currentButtonScale = 0.005f;
	m_targetButtonScale = 0.006f;

	ElkTools::Utils::Time::SetTimeScale(1.f);

	m_audioManager.StopAllSounds();
	m_audioManager.Play2DSound("../Resources/Audio/Musics/title.ogg", true, 1.f);

	m_engineManager.ParseScene();
}

ElkCraft::System::GameStateMenu::~GameStateMenu()
{
	m_sceneManager.RemoveScene("Menu");
}

void ElkCraft::System::GameStateMenu::CreateGameObjects()
{
	/* Create UI Camera */
	m_UICamera = &m_sceneManager.GetCurrentScene().CreateGameObject("UICamera");
	m_UICamera->AddComponent<UICamera>();

	/* Create background */
	m_background = &m_sceneManager.GetCurrentScene().CreateGameObject("Background");
	m_background->AddComponent<Sprite>();

	/* Create ElkCraft Logo */
	m_logo = &m_sceneManager.GetCurrentScene().CreateGameObject("Logo");
	m_logo->AddComponent<Sprite>();
	
	/* Create Play Button */
	m_playButton = &m_sceneManager.GetCurrentScene().CreateGameObject("PlayButton");
	m_playButton->AddComponent<PlayButton>();
	m_playButton->AddComponent<Sprite>();
	m_playButton->AddComponent<Text>();

	/* Create Continue Button */
	m_continueButton = &m_sceneManager.GetCurrentScene().CreateGameObject("ContinueButton");
	m_continueButton->AddComponent<ContinueButton>();
	m_continueButton->AddComponent<Sprite>();
	m_continueButton->AddComponent<Text>();

	/* Create Quit Button */
	m_quitButton = &m_sceneManager.GetCurrentScene().CreateGameObject("QuitButton");
	m_quitButton->AddComponent<QuitButton>();
	m_quitButton->AddComponent<Sprite>();
	m_quitButton->AddComponent<Text>();
}

void ElkCraft::System::GameStateMenu::InitGameObjects()
{
	/* Buttons global parameters */
	glm::vec2 buttonExtend = glm::vec2(0.13f * m_windowWidth, 0.082f * m_windowHeight);
	glm::vec2 buttonPosition = glm::vec2(m_windowWidth * 0.5f, m_windowHeight * 0.5f);
	float buttonYOffset = 0.09f * m_windowHeight;
	float spacing = 0.3f;
	float fontSize = 0.003f;
	
	/* Init UI camera */
	m_UICamera->transform->SetPosition(glm::vec3(0, 0, 0.1f));
	m_UICamera->transform->SetRotation(glm::vec3(0, -90, 0));

	/* Init background */
	Sprite& backgroundSprite = *m_background->GetComponent<Sprite>();
	backgroundSprite.SetTexture(*m_textureManager.RequireAndGet("Menu_Background"));
	backgroundSprite.SetRenderLayer(1);
	m_background->transform->SetPosition(glm::vec3(0.f, 0.f, -1.415f));
	m_background->transform->SetScale(glm::vec3(1.777f, 1.f, 1.f));

	/* Init ElkCraft Logo */
	Sprite& logoSprite = *m_logo->GetComponent<Sprite>();
	m_logo->transform->SetPosition(glm::vec3(0.f, 0.55f, -1.414f));
	m_logo->transform->SetScale(glm::vec3(2.3684f * 0.2f, 1.f * 0.2f, 1.f));
	logoSprite.SetTexture(*m_textureManager.RequireAndGet("ElkCraft_Logo"));

	/* Init Play Button */
	Sprite& playButtonSprite = *m_playButton->GetComponent<Sprite>();
	Text& playButtonText = *m_playButton->GetComponent<Text>();
	PlayButton& playButton = *m_playButton->GetComponent<PlayButton>();
	playButtonSprite.SetTexture(*m_textureManager.RequireAndGet("Button"));
	playButtonSprite.SetRenderLayer(0);
	m_playButton->transform->Translate(glm::vec3(0, +0.01f, 0));
	playButton.SetExtends(buttonExtend.x, buttonExtend.y);
	playButton.SetPosition(buttonPosition.x, buttonPosition.y - buttonYOffset);
	playButtonText.SetString("PLAY");
	playButtonText.SetSpacing(spacing);
	playButtonText.SetFontSize(fontSize);
	playButtonText.SetScalingMode(Text::ScalingMode::FONT_SIZE_ONLY);

	/* Init Continue Button */
	Sprite& continueButtonSprite = *m_continueButton->GetComponent<Sprite>();
	Text& continueButtonText = *m_continueButton->GetComponent<Text>();
	ContinueButton& continueButton = *m_continueButton->GetComponent<ContinueButton>();
	continueButtonSprite.SetTexture(*m_textureManager.RequireAndGet("Button"));
	continueButtonSprite.SetRenderLayer(0);
	m_continueButton->transform->Translate(glm::vec3(0, -0.005f, 0));
	continueButton.SetExtends(buttonExtend.x, buttonExtend.y);
	continueButton.SetPosition(buttonPosition.x, buttonPosition.y + buttonYOffset * 0.5f);
	continueButtonText.SetString("LOAD");
	continueButtonText.SetSpacing(spacing);
	continueButtonText.SetFontSize(fontSize);
	continueButtonText.SetScalingMode(Text::ScalingMode::FONT_SIZE_ONLY);

	/* init Quit Button */
	Sprite& quitButtonSprite = *m_quitButton->GetComponent<Sprite>();
	Text& quitButtonText = *m_quitButton->GetComponent<Text>();
	QuitButton& quitButton = *m_quitButton->GetComponent<QuitButton>();
	quitButtonSprite.SetTexture(*m_textureManager.RequireAndGet("Button"));
	quitButtonSprite.SetRenderLayer(1);
	m_quitButton->transform->Translate(glm::vec3(0, -0.02f, 0));
	quitButton.SetExtends(buttonExtend.x, buttonExtend.y);
	quitButton.SetPosition(buttonPosition.x, buttonPosition.y + buttonYOffset * 2);
	quitButtonText.SetString("QUIT");
	quitButtonText.SetSpacing(spacing);
	quitButtonText.SetFontSize(fontSize);
	quitButtonText.SetScalingMode(Text::ScalingMode::FONT_SIZE_ONLY);
}

void ElkCraft::System::GameStateMenu::UpdateButtonsAnimationSettings()
{
	/* Calculate button animation settings */
	if (m_currentButtonScale > 0.0055f)
		m_targetButtonScale = 0.004f;

	if (m_currentButtonScale < 0.0045f)
		m_targetButtonScale = 0.006f;

	float lerpTime = 0.8f * Time::GetDeltaTime();
	m_currentButtonScale = ElkTools::Utils::Math::Lerp(m_currentButtonScale, m_targetButtonScale, lerpTime > 1.f ? 1.f : lerpTime);
}

void ElkCraft::System::GameStateMenu::UpdateButton(ElkGameEngine::Objects::GameObject& p_button)
{
	p_button.UpdateComponents();
	UpdateButtonAnimation(p_button);
}

void ElkCraft::System::GameStateMenu::UpdateButtonAnimation(ElkGameEngine::Objects::GameObject & p_button)
{
	p_button.transform->SetScale(glm::vec3(m_currentButtonScale * 2.5f, m_currentButtonScale, m_currentButtonScale));
	p_button.GetComponent<Text>()->SetFontSize(m_currentButtonScale - 0.0018f);
	p_button.GetComponent<Text>()->SetPositionOffset(glm::vec3(0.0005f - m_currentButtonScale, 0.f, 0.f));
}

void ElkCraft::System::GameStateMenu::Update()
{
	UpdateButtonsAnimationSettings();
	UpdateButton(*m_playButton);
	UpdateButton(*m_continueButton);
	UpdateButton(*m_quitButton);
}

void ElkCraft::System::GameStateMenu::HandleInputs()
{
}
