
#include <iostream>
#include <vector>

#include "GameApp.h"


/*################################################################################*/
GameApp::GameApp(const std::string&title, unsigned int screenWidth, unsigned int screenHeight)
{
	m_title = title;
    m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
    m_contextFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
	m_window = NULL;
	m_glContext = NULL;
    m_timer = 0;
	m_done = false;
	shader = NULL;
	mesh = NULL;
	transform = NULL;
	m_counter = 0.0f;
	camera = NULL;
	texture = NULL;

    m_keyUp = false;
    m_keyDown = false;
    m_keyLeft = false;
    m_keyRight = false;
    m_zoomIn = false;
    m_zoomOut = false;

    m_angleX = -60.0f;
    m_angleY = 140.0f;
}

/*################################################################################*/
GameApp::~GameApp(void)
{
    SDL_RemoveTimer(m_timer);
    SDL_GL_DeleteContext(m_glContext);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

/*################################################################################*/
void GameApp::InitSystem(void)
{
	// Initialize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return;
    }

    // Set GL context default values
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // Create a window
    m_window = SDL_CreateWindow(m_title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    		                    m_screenWidth, m_screenHeight, m_contextFlags);
    if (m_window == NULL){
    	std::cout << "SDL create main window failed! SDL Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return;
    }

    // Create a GL context
    m_glContext = SDL_GL_CreateContext(m_window);
    if( m_glContext == NULL){
    	std::cout << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << std::endl;
        return;
    }

    // Initialize glew
    if (glewInit() != GLEW_OK) {
    	std::cout << "Initialize glew failed!" << std::endl;
    	exit(1);
    }
    std::cout << "OpenGL v" << glGetString(GL_VERSION) << " using " << glGetString(GL_RENDERER) << std::endl;

    // Add timer to create the frame rate
    AddTimer();

    // Set default gl values
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);


    //----------------------------------------------------

    shader = new Shader("resources/basicShader");

    mesh = new Mesh("resources/fighter.obj");

    texture = new Texture("resources/fighter.tga");

    transform = new Transform();

    camera = new Camera(glm::vec3(0.0, 0.0, -2.3), 70.0f, (float)m_screenWidth/(float)m_screenHeight, 0.01f, 1000.0f);
	transform->GetRotation().x = m_angleX;
	transform->GetRotation().y = m_angleY;
}

/*################################################################################*/
void GameApp::AddTimer(void)
{
    m_timer = SDL_AddTimer(30, GameLoopTimer, this);
}

/*################################################################################*/
Uint32 GameApp::GameLoopTimer(Uint32 interval, void* param)
{
	//delete shader;

    // Create a user event to call the game loop.
    SDL_Event event;
    event.type = SDL_USEREVENT;
    event.user.code = RUN_GAME_LOOP;
    event.user.data1 = 0;
    event.user.data2 = 0;
    SDL_PushEvent(&event);

    return interval;
}

/*################################################################################*/
void GameApp::Run(void)
{
    SDL_Event event;

    while((!m_done) && (SDL_WaitEvent(&event))) {
        switch(event.type) {
            case SDL_USEREVENT:
                HandleUserEvents(&event);
                break;
            case SDL_KEYDOWN:
            	if (event.key.keysym.sym == SDLK_UP) m_keyUp = true;
            	if (event.key.keysym.sym == SDLK_DOWN) m_keyDown = true;
            	if (event.key.keysym.sym == SDLK_LEFT) m_keyLeft = true;
            	if (event.key.keysym.sym == SDLK_RIGHT) m_keyRight = true;
                break;
            case SDL_KEYUP:
            	if (event.key.keysym.sym == SDLK_UP) m_keyUp = false;
            	if (event.key.keysym.sym == SDLK_DOWN) m_keyDown = false;
            	if (event.key.keysym.sym == SDLK_LEFT) m_keyLeft = false;
            	if (event.key.keysym.sym == SDLK_RIGHT) m_keyRight = false;
                break;
            case SDL_MOUSEWHEEL:
                if (event.wheel.y < 0) {
                	m_zoomIn = true;
                } else {
                	m_zoomOut = true;
                }
            break;
            case SDL_QUIT:
                m_done = true;
                break;
            default:
                break;
        }
    }
}

/*################################################################################*/
void GameApp::HandleUserEvents(SDL_Event* event)
{
    switch (event->user.code) {
        case RUN_GAME_LOOP:
            GameLoop();
            break;
        default:
            break;
    }
}

/*################################################################################*/
void GameApp::GameLoop(void)
{
    RenderFrame();
}

/*################################################################################*/
void GameApp::RenderFrame(void)
{
	glClearColor(0.0f, 0.15f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (m_keyUp) {
		m_angleX -= 2.0f;
		transform->GetRotation().x = m_angleX;
	}
	else if (m_keyDown) {
		m_angleX += 2.0f;
		transform->GetRotation().x = m_angleX;
	}
	if (m_keyLeft) {
		m_angleY -= 2.0f;
		transform->GetRotation().y = m_angleY;
	}
	else if (m_keyRight) {
		m_angleY += 2.0f;
		transform->GetRotation().y = m_angleY;
	}
	if (m_zoomIn) {
		camera->SetZoomIn();
		m_zoomIn = false;
	}
	else if (m_zoomOut) {
		camera->SetZoomOut();
		m_zoomOut = false;
	}

	shader->Bind();
	mesh->Draw();
	shader->Update(transform, camera);

    SDL_GL_SwapWindow(m_window);
	m_counter += 0.1f;

}

