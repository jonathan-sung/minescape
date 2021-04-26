#include "scene_menu.h"
#include "../components/cmp_text.h"
#include "../game.h"
#include <SFML/Window/Keyboard.hpp>
#include <iostream>

using namespace std;
using namespace sf;

void MenuScene::Load() {
  cout << "Menu Load";
  MenuScene::selection = 0;
  {
    auto txt = makeEntity();
    auto t = txt->addComponent<TextComponent>("Minescape");
    
    auto op1 = makeEntity();
    op1->setPosition(Vector2f(0,100));
    t = op1->addComponent<TextComponent>("[ Play ]");
    t->setPosition(op1->getPosition());
    options[0] = op1;
    cout << options[0]->getPosition() << endl;

    
    auto op2 = makeEntity();
    op2->setPosition(Vector2f(0,200));
    t = op2->addComponent<TextComponent>("Options");
    t->setPosition(op2->getPosition());
    options[1] = op2;

    auto op3 = makeEntity();
    op3->setPosition(Vector2f(0,300));
    t = op3->addComponent<TextComponent>("Quit");
    t->setPosition(op3->getPosition());
    options[2] = op3;
    
  }
  setLoaded(true);
}

void MenuScene::Update(const double& dt)
{
    // cout << "Menu Update "<<dt<<"\n";
    static float buttonCD;

    if (sf::Keyboard::isKeyPressed(keyControls[keybinds::Up]) && buttonCD <= 0)
    {
        if (selection == 0) selection = 2;
        else selection--;
        cout << "Selection: ";
        cout << selection << endl;
        buttonCD = 0.25f;

        changeText();
    }

    if (sf::Keyboard::isKeyPressed(keyControls[keybinds::Down]) && buttonCD <= 0)
    {
        if (selection == 2) selection = 0;
        else selection++;
        cout << "Selection: ";
        cout << selection << endl;
        buttonCD = 0.25f;
        changeText();
        
    }

    if (sf::Keyboard::isKeyPressed(keyControls[keybinds::Action1]) && !enterDown || sf::Keyboard::isKeyPressed(keyControls[keybinds::Action2]) && !enterDown)
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

    if (!sf::Keyboard::isKeyPressed(keyControls[keybinds::Action1]) && !sf::Keyboard::isKeyPressed(keyControls[keybinds::Action2]))
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
