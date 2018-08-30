#include "game.hpp"
#include "sync.hpp"
#include "assets.hpp"
#include "renderer.hpp"
#include "ui.hpp"


namespace SMOBA
{
    void Game::Update_Loop(Sync* GameSync)
    {
		Game game(GameSync);

        b8 DebugGame = true;

        if(DebugGame)
        {

            while (GameSync->Running.load())
            {
                std::lock_guard<std::mutex> lock(GameSync->Mutex);
                if(GameSync->UpdateLoop)
                {
                    // update
                    game.Update();
                    // render
                    game.Render();
                    GameSync->UpdateLoop = false;
                }
            }
        }
    }

    Game::Game(Sync* gameSync)
    {
        GameSync = gameSync;
        ButtonTest = UI_Gen_UI_ID();
        PanelTest = UI_Gen_UI_ID();
    }

    Game::~Game()
    {
        delete World;
    }

    void Game::Update()
    {
        GameSync->Cams[1].Update(GameSync->Ip);
    }

    void Game::Render()
    {
        World->Render(GameSync->Rq);


        Draw_Voxel_World(GameSync->Rq, GameSync->VoxelWorld);
    }
}
