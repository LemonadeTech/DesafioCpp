
#ifndef GAMEAPP_H_
#define GAMEAPP_H_

#include <string>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "Shader.h"
#include "Mesh.h"
#include "Transform.h"
#include "Camera.h"
#include "Texture.h"

class GameApp
{
    public:
        // Constructor
        GameApp(const std::string&title, unsigned int screenWidth, unsigned int screenHeight);

        // Destructor
        virtual ~GameApp(void);

        // Initialization functions
        void InitSystem(void);

        // Run
        void Run(void);

    private:

        std::string m_title;
        unsigned int m_screenWidth;
    	unsigned int m_screenHeight;
    	Uint32 m_contextFlags;
        SDL_Window*m_window;
        SDL_GLContext m_glContext;
        SDL_TimerID m_timer;
        bool m_done;

        Shader*shader;
        Mesh*mesh;
        Texture*texture;

        Transform*transform;
        Camera*camera;

        float m_counter;

        bool m_keyUp;
        bool m_keyDown;
        bool m_keyLeft;
        bool m_keyRight;
        bool m_zoomIn;
        bool m_zoomOut;
        float m_angleX;
        float m_angleY;

        void AddTimer(void);
        static Uint32 GameLoopTimer(Uint32 interval, void* param);
        void HandleUserEvents(SDL_Event* event);
        void GameLoop(void);
        void RenderFrame(void);

};

typedef GameApp* GameAppPtr;
typedef GameApp** GameAppHandle;

const int RUN_GAME_LOOP = 1;


#endif /* GAMEAPP_H_ */
