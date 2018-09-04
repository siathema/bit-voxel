#include <stdio.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include "GL/glew.h"
#if WIN64
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif


//#define GLEW_STATIC

#include "game.hpp"
#include "sync.hpp"
#include "sMath.hpp"
#include "voxel.hpp"
#include "renderer.hpp"
#include "camera.hpp"
#include "assets.hpp"

namespace SMOBA
{
	Sync* GlobalSync;
	Queue_Array<RenderCommand> * Rq2;
	Input* Ip2;

    void Update_Loop(Sync* GameSync)
    {
		GameState game = {};
        game.GameSync = GameSync;

        b8 DebugGame = true;

        if(DebugGame)
        {

            while (GameSync->Running.load())
            {
                std::lock_guard<std::mutex> lock(GameSync->Mutex);
                if(GameSync->UpdateLoop)
                {
                    GameSync->Cams[1].Update(GameSync->Ip);

                     Draw_Voxel_World(GameSync->Rq, GameSync->VoxelWorld);

                    GameSync->UpdateLoop = false;
                }
            }
        }
    }

	i32 proc()
	{
		ViewportInfo viewPortInfo = {};
		viewPortInfo.ScreenHeight = 720;
		viewPortInfo.ScreenWidth = 1080;
		viewPortInfo.Vsync = true;

		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			printf("Could not initialize SDL. ERROR: %s\n", SDL_GetError());
			return -1;
		}

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
							SDL_GL_CONTEXT_PROFILE_CORE);

        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

		SDL_Window* window = SDL_CreateWindow(
			"Bit Voxel",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			viewPortInfo.ScreenWidth,
			viewPortInfo.ScreenHeight,
			SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

		if (window == NULL)
		{
			printf("Could not open window! ERROR: %s\n", SDL_GetError());
			return -1;
		}
		else
		{
			SDL_GLContext context = SDL_GL_CreateContext(window);
			if (context == NULL)
			{
				printf("Could not create opengl context! ERROR: %s\n",
					   SDL_GetError());
				return -1;
			}
			else
			{
				glewExperimental = GL_TRUE;
				if (glewInit() != GLEW_OK)
				{
					printf("Could not initialize GLEW\n");
				}
				if (SDL_GL_SetSwapInterval(1) < 0)
				{
					viewPortInfo.Vsync = false;
					printf("Warning could not enable v-sync! SDL ERROR: %s\n",
						   SDL_GetError());
				}
			}
			glViewport(0,
					   0,
					   viewPortInfo.ScreenWidth,
					   viewPortInfo.ScreenHeight);
			//glEnable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glEnable(GL_ALPHA);
			//glEnable(GL_DEPTH_TEST);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_DEBUG_OUTPUT);
		}

		SDL_Event event;

		//NOTE(matthias):Threading debug stuff
		Rq2 = new Queue_Array<RenderCommand>;
		Ip2 = new Input();
		GlobalSync = new Sync();
		GlobalSync->Rq = new Queue_Array<RenderCommand>;
		GlobalSync->Ip = new Input();
		GlobalSync->UiContext = new UI_Context();
		GlobalSync->Viewport = new ViewportInfo();
		*GlobalSync->UiContext = {};
		*GlobalSync->Ip = {};
		*GlobalSync->Viewport = viewPortInfo;
		GlobalSync->Running.store(true);
		GlobalSync->VoxelWorld = Generate_Voxel_World();
		GlobalSync->UpdateLoop = false;

		ASSETS::Load_Assets(viewPortInfo, GlobalSync->VoxelWorld);

		//NOTE(matthias): Render init code
		Renderer renderer(&viewPortInfo);
		Camera* cameras = new Camera[2];
		Camera* cameras2 = new Camera[2];
		cameras[0].Init(0.0f,
						0.0f,
						0.0f,
						0.0f,
						PI/4,
						viewPortInfo.ScreenWidth,
						viewPortInfo.ScreenHeight,
						0.01f,
						100.0f);
		cameras[1].Init(0.0f,
						90.f,
						0.0f,
						0.0f,
						toRadians(45.f),
						viewPortInfo.ScreenWidth,
						viewPortInfo.ScreenHeight,
						0.01f,
						100.0f);
		cameras[1].SetPerspective();
		cameras2[0] = cameras[0];
		cameras2[1] = cameras[1];
		GlobalSync->Cams = cameras2;

        //TODO: clean this up.
        // maybe create debug ui.
		char format[] = "FPS: %f Camera x:%f y:%f z:%f Pitch: %f Yaw: %f fov: %f";
		char debugMessage[256];
		sprintf(debugMessage, format, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		const u32 targetTime = 1000 / 60;
		u32 elapsedTime;

		std::thread updateLoop(Update_Loop, GlobalSync);

		while (GlobalSync->Running.load())
		{
			u32 startTime = SDL_GetTicks();

			renderer.Render(Rq2, cameras);
			renderer.Render_String(debugMessage,
								  cameras[0],
								  vec2(-(viewPortInfo.ScreenWidth/2.0f),
									   viewPortInfo.ScreenHeight/2.0f),
								  vec2(1.0f, 1.0f),
								  0.0f,
								  vec4(1.0f, 1.0f, 1.0f, 1.0f));
            SDL_GL_SwapWindow(window);

			std::lock_guard<std::mutex> guard(GlobalSync->Mutex);
			// TODO(matthias): consolidate these swaps
			Queue_Array<RenderCommand> * tempQueue = GlobalSync->Rq;
			GlobalSync->Rq = Rq2;
			Rq2 = tempQueue;
			GlobalSync->UpdateLoop = true;

			Camera* tempCams = GlobalSync->Cams;
			GlobalSync->Cams = cameras;
			GlobalSync->Cams[0] = tempCams[0];
			GlobalSync->Cams[1] = tempCams[1];
			cameras = tempCams;

			elapsedTime = SDL_GetTicks() - startTime;
			r32 SecondsPerFrame = (1.0f / 1000) * elapsedTime;
			r32 FPS = 1.0f / SecondsPerFrame;
			sprintf(debugMessage,
					format,
					FPS,
					cameras[1].Pos.x,
					cameras[1].Pos.y,
					cameras[1].Pos.z,
					cameras[1].Pitch,
					cameras[1].Yaw,
					cameras[1].Fov);

			*Ip2 = *(GlobalSync->Ip);
			Ip2->WheelX = 0;
			Ip2->WheelY = 0;
			while (SDL_PollEvent(&event) != 0) {
				i32 keySym = 0;
				u32 mouseState = 0;
				switch (event.type) {
				case SDL_QUIT:
						GlobalSync->Running.store(false);
						break;
				case SDL_KEYDOWN:
						keySym = event.key.keysym.sym;
						switch (keySym) {
						case SDLK_ESCAPE:
							GlobalSync->Running.store(false);
							break;
						case SDLK_LEFT:
							Ip2->Left = true;
							break;
						case SDLK_RIGHT:
							Ip2->Right = true;
							break;
						case SDLK_UP:
							Ip2->Up = true;
							break;
						case SDLK_DOWN:
							Ip2->Down = true;
							break;
						default:
							if (keySym < 128) {
								Ip2->Keys[keySym] = true;
							}
							break;
						}
						break;
				case SDL_KEYUP:
						keySym = event.key.keysym.sym;
						switch (keySym) {
						case SDLK_LEFT:
							Ip2->Left = false;
							break;
						case SDLK_RIGHT:
							Ip2->Right = false;
							break;
						case SDLK_UP:
							Ip2->Up = false;
							break;
						case SDLK_DOWN:
							Ip2->Down = false;
							break;
						default:
							if (keySym < 128) {
								Ip2->Keys[keySym] = false;
							}
							break;
						}
						break;
				case SDL_MOUSEMOTION: case SDL_MOUSEBUTTONDOWN: case SDL_MOUSEBUTTONUP:
						mouseState = SDL_GetMouseState(&Ip2->MouseX, &Ip2->MouseY);
						Ip2->LeftMouseDown =
							(mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) > 0;
						Ip2->RightMouseDown =
							(mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) > 0;
						break;
				case SDL_MOUSEWHEEL:
						Ip2->WheelX = event.wheel.x;
						Ip2->WheelY = event.wheel.y;
						break;
				default:
					break;
				}
			}

			Input* tempIP = GlobalSync->Ip;
			GlobalSync->Ip = Ip2;
			Ip2 = tempIP;

			//NOTE(matthias): Timing stuff
			u32 endTime = SDL_GetTicks();
			elapsedTime = endTime - startTime;
#if 1
			if (elapsedTime < targetTime) {
				u32 wait = targetTime - elapsedTime;
				if (wait < 5)
				{
					while (1)
					{
						elapsedTime = SDL_GetTicks() - startTime;
						if (elapsedTime >= targetTime) break;
					}
				}
				else
				{
					SDL_Delay(wait);
				}
			}
#endif
		}

		//delete[] cameras;
		//delete[] cameras2;
		updateLoop.join();

		SDL_DestroyWindow(window);
		SDL_Quit();

		return 0;
	}

}

int main(int argc, char** argv)
{
	return SMOBA::proc();
}
