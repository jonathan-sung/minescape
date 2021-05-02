#include "scene_menu.h"
#include "../components/cmp_text.h"
#include "../game.h"
#include <SFML/Window/Keyboard.hpp>
#include <iostream>

using namespace std;
using namespace sf;

void MenuScene::Load() 
{
	music.setVolume(20);
	music.setLoop(true);
	cout << "Menu Load";
  Engine::GetWindow().setView(View(Vector2f(640, 360), Vector2f(1280, 720)));
  paused = false;
  MenuScene::selection = 0;
  {
    auto txt = makeEntity();
    auto t = txt->addComponent<TextComponent>("Minescape");
    t->SetSize(64);
    cout << t->getPosition() << endl;

    auto op1 = makeEntity();
    op1->setPosition(Vector2f(0,150));
    t = op1->addComponent<TextComponent>("[ Play ]");
    t->setPosition(op1->getPosition());
    t->SetSize(45);
    options[0] = op1;
    cout << options[0]->getPosition() << endl;

    
    auto op2 = makeEntity();
    op2->setPosition(Vector2f(0,250));
    t = op2->addComponent<TextComponent>("Options");
    t->setPosition(op2->getPosition());
    t->SetSize(45);
    options[1] = op2;

    auto op3 = makeEntity();
    op3->setPosition(Vector2f(0,400));
    t = op3->addComponent<TextComponent>("Quit");
    t->setPosition(op3->getPosition());
    t->SetSize(45);
    options[2] = op3;

	}
	setLoaded(true);
	if (music.openFromFile("res/sounds/music/minescape_menu_theme.ogg")) music.play();
}

void MenuScene::UnLoad()
{
    music.stop();
}

void MenuScene::Update(const double& dt)
{
    if (sf::Keyboard::isKeyPressed(Engine::keyControls[Engine::keybinds::Up]) && buttonCD <= 0)
    {
        if (selection == 0) selection = 2;
        else selection--;
        cout << "Selection: ";
        cout << selection << endl;
        buttonCD = 0.25f;

        changeText();
    }

    if (sf::Keyboard::isKeyPressed(Engine::keyControls[Engine::keybinds::Down]) && buttonCD <= 0)
    {
        if (selection == 2) selection = 0;
        else selection++;
        cout << "Selection: ";
        cout << selection << endl;
        buttonCD = 0.25f;
        changeText();
        
    }

    if (sf::Keyboard::isKeyPressed(Engine::keyControls[Engine::keybinds::Action1]) && !enterDown ||
        sf::Keyboard::isKeyPressed(Engine::keyControls[Engine::keybinds::Action2]) && !enterDown)
    {
        switch (selection)
        {
        case(0):

            Engine::ChangeScene(&level1);
            break;
        case(1):
            Engine::ChangeScene(&optionscene);
            break;
        case(2):
            Engine::GetWindow().close();
            break;
        }
        enterDown = true;
    }

    if (!sf::Keyboard::isKeyPressed(Engine::keyControls[Engine::keybinds::Action1]) && 
        !sf::Keyboard::isKeyPressed(Engine::keyControls[Engine::keybinds::Action2]))
    {
        enterDown = false;
    }

    Scene::Update(dt);
    buttonCD -= dt;
}



void MenuScene::changeText()
{
	auto t = options[0]->GetCompatibleComponent<TextComponent>();

	t[0]->SetText("Play");
	t = options[1]->GetCompatibleComponent<TextComponent>();
	t[0]->SetText("Options");
	t = options[2]->GetCompatibleComponent<TextComponent>();
	t[0]->SetText("Quit");

	switch (selection)
	{
	case(0):
		t = options[0]->GetCompatibleComponent<TextComponent>();
		t[0]->SetText("[ Play ]");
		break;
	case(1):
		t = options[1]->GetCompatibleComponent<TextComponent>();
		t[0]->SetText("[ Options ]");
		break;
	case(2):
		t = options[2]->GetCompatibleComponent<TextComponent>();
		t[0]->SetText("[ Quit ]");
		break;
	}
}
