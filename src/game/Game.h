#pragma once

#include "Scene.h"
#include "core/Time.h"
#include "graphics/PostProcess.h"
#include "graphics/Renderer.h"
#include "core/Memory.h"
#include "input/Input.h"

#include "core/Window.h"
#include "core/platform/Windows.h"
#include "core/platform/Android.h"

#include "graphics/Viewport.h"

#include "core/Application.h"

#include "SceneManager.h"

class Game
{
public:
	Game();

	Scene* LoadScene(const String &name);
	void StartScene(Scene *s);
	void handle_event(Event *e);

	void Start();
	void update();
	void Draw();

	Scene *activescene;

	//Get activegame
	static Game* GetGame();

private:
	static Game *activegame;
};