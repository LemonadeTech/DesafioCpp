#include <iostream>
#include <string>

#include "render.h"
#include "tga.h"

using namespace std;

Render::Render(int width, int height, const string &title)
    : m_windowWidth(width)
    , m_windowHeight(height)
    , m_windowTitle(title)
    , m_windowX(100)
    , m_windowY(100)
    , m_shapdeIx(0)
    , m_cameraPosition(0.0, 0.0, 5.0)
    , m_cameraTarget(0.0, 0.0, 0.0)
    , m_rotY(0.0f)
    , m_light0Pos(10.0f, 0.0f, 10.0f)
    , m_light0Color(0.3f, 0.3f, 0.3f)
{

}

Render::~Render()
{
    SDL_GL_DeleteContext(m_glContext);
    SDL_Quit();
}

bool Render::init()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        cout << "SDL_Init Error: " << SDL_GetError() << endl;

        return false;
    }

    // not working properly on my machine (linux AMD 7700M)
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
            SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);

    m_sdlWindow = shared_ptr<SDL_Window>(SDL_CreateWindow(
                m_windowTitle.c_str(),
                m_windowX, m_windowY,
                m_windowWidth, m_windowHeight, SDL_WINDOW_OPENGL |
                SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE), SDL_DestroyWindow);

    if (m_sdlWindow == nullptr) {
        cout << "SDL_CreateWindow Error: " << SDL_GetError() << endl;

        return false;
    }

    m_glContext = SDL_GL_CreateContext(m_sdlWindow.get());

    if (m_glContext == nullptr) {
        cout << "OpenGL context could not be created! SDL Error: " <<
            SDL_GetError() << endl;

        return false;
    }

    cout << "OpenGL v" << glGetString(GL_VERSION) << " using " <<
        glGetString(GL_RENDERER) << endl;

#if defined _WIN32 || defined __linux__
    glewExperimental = GL_TRUE;
    if (GLEW_OK != glewInit()) {
        cout << "Failed to init GLEW" << endl;

        return false;
    }
#endif

    SDL_GL_SetSwapInterval(1);

    compileShader("vertex.vsh", "fragment.fsh");

    bool ret = loadObj("fighter.obj");
    if (!ret)
        return false;

    glClearColor(.2, .2, .2, 1);
    glEnable(GL_DEPTH_TEST);

    loadTextures("fighter.tga");

    return true;
}

#include <fstream>
void Render::compileShader(const std::string &shader, const std::string &frag)
{
    GLuint shaderVertexShader;
    GLuint shaderFragmentShader;

    ifstream ifsvsh(shader);
    string vshader((istreambuf_iterator<char>(ifsvsh)),
            istreambuf_iterator<char>());
    const char *vshader_cstr = vshader.c_str();

    shaderVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(shaderVertexShader, 1, &vshader_cstr, nullptr);
    glCompileShader(shaderVertexShader);

    GLint compileStatus = GL_TRUE;
    glGetShaderiv(shaderVertexShader, GL_COMPILE_STATUS, &compileStatus);

    GLchar log[500];
    memset(log, 0, sizeof(log));

    if (compileStatus == GL_FALSE) {
        glGetShaderInfoLog(shaderVertexShader, sizeof(log), nullptr, log);
        cout << "Vertex shader: " << log << endl;

        glDeleteShader(shaderVertexShader);

        return;
    }

    ifstream ifsfsh(frag);
    string fshader((istreambuf_iterator<char>(ifsfsh)),
            istreambuf_iterator<char>());
    const char *fshader_cstr = fshader.c_str();

    shaderFragmentShader= glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(shaderFragmentShader, 1, &fshader_cstr, nullptr);
    glCompileShader(shaderFragmentShader);

    compileStatus = GL_TRUE;
    glGetShaderiv(shaderFragmentShader, GL_COMPILE_STATUS, &compileStatus);

    memset(log, 0, sizeof(log));

    if (compileStatus == GL_FALSE) {
        glGetShaderInfoLog(shaderFragmentShader, sizeof(log), nullptr, log);
        cout << "Fragment shader: " << log << endl;

        glDeleteShader(shaderVertexShader);
        glDeleteShader(shaderFragmentShader);

        return;
    }

    m_shaderProgram = glCreateProgram();

    glAttachShader(m_shaderProgram, shaderVertexShader);
    glAttachShader(m_shaderProgram, shaderFragmentShader);
    glLinkProgram(m_shaderProgram);

    GLint linkStatus = GL_TRUE;
    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &linkStatus);

    if (linkStatus == GL_FALSE) {
        glGetProgramInfoLog(m_shaderProgram, sizeof(log), nullptr, log);
        cout << log << endl;

        glDeleteShader(shaderVertexShader);
        glDeleteShader(shaderFragmentShader);
        glDeleteProgram(m_shaderProgram);

        return;
    }

    m_normalAttribute = glGetAttribLocation(m_shaderProgram, "normal");
    m_textureAttribute = glGetAttribLocation(m_shaderProgram, "textureCoord");
    m_vertexAttribute = glGetAttribLocation(m_shaderProgram, "vertex");

    m_colorUniform = glGetUniformLocation(m_shaderProgram, "color");
    m_modelViewMatrixUniform = glGetUniformLocation(m_shaderProgram,
            "modelViewMatrix");
    m_projectionMatrixUniform = glGetUniformLocation(m_shaderProgram,
            "projectionMatrix");
    m_textUnit0Uniform = glGetUniformLocation(m_shaderProgram, "textUnit0");
    m_light0ColorUniform = glGetUniformLocation(m_shaderProgram,
            "light0Color");
    m_light0PositionUniform = glGetUniformLocation(m_shaderProgram,
            "light0Position");
}

bool Render::loadObj(const std::string &filename)
{
    // correct way is test if filename is empty, if the file exist...
    string err = tinyobj::LoadObj(m_shapes, m_materials, filename.c_str());

    if (!err.empty()) {
        std::cerr << err << std::endl;

        return false;
    }

    std::cout << "# of shapes    : " << m_shapes.size() << std::endl;
    std::cout << "# of materials : " << m_materials.size() << std::endl;

    glGenVertexArrays(1, &m_sphereVertexArray);
    glBindVertexArray(m_sphereVertexArray);

    GLuint sphereNormalsBuffer;
    glGenBuffers(1, &sphereNormalsBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, sphereNormalsBuffer);
    glBufferData(GL_ARRAY_BUFFER,
            m_shapes[m_shapdeIx].mesh.positions.size() * sizeof(float),
            &m_shapes[m_shapdeIx].mesh.positions[0], GL_STATIC_DRAW);

    glVertexAttribPointer(m_normalAttribute, 3, GL_FLOAT, GL_FALSE,
            sizeof(float) * 3, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(m_normalAttribute);

    GLuint sphereTextureBuffer;
    glGenBuffers(1, &sphereTextureBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, sphereTextureBuffer);
    glBufferData(GL_ARRAY_BUFFER,
            m_shapes[m_shapdeIx].mesh.normals.size() * sizeof(float),
            &m_shapes[m_shapdeIx].mesh.normals[0], GL_STATIC_DRAW);

    glVertexAttribPointer(m_textureAttribute, 3, GL_FLOAT, GL_FALSE,
            sizeof(float) * 3, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(m_textureAttribute);

    GLuint sphereVertexBuffer;
    glGenBuffers(1, &sphereVertexBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, sphereVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER,
            m_shapes[m_shapdeIx].mesh.texcoords.size() * sizeof(float),
            &m_shapes[m_shapdeIx].mesh.texcoords[0], GL_STATIC_DRAW);

    glVertexAttribPointer(m_vertexAttribute, 2, GL_FLOAT, GL_FALSE,
            sizeof(float) * 2, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(m_vertexAttribute);

    GLuint sphereIndicesBuffer;
    glGenBuffers(1, &sphereIndicesBuffer);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereIndicesBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            m_shapes[m_shapdeIx].mesh.indices.size() * sizeof(unsigned int),
            &m_shapes[m_shapdeIx].mesh.indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);

    return true;
}

void Render::loadTextures(const string &filename)
{
    GLint texture0Width;
    GLint texture0Height;
    GLint texture0Components;
    GLenum texture0Format;

    GLbyte *texture0Pixels = gltReadTGABits(filename.c_str(), &texture0Width,
            &texture0Height, &texture0Components, &texture0Format, nullptr);

    glGenTextures(1, &m_texture0);
    glBindTexture(GL_TEXTURE_2D, m_texture0);

    glTexImage2D(GL_TEXTURE_2D, 0, texture0Components, texture0Width,
            texture0Height, 0, texture0Format, GL_UNSIGNED_BYTE,
            texture0Pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

// returns 'false' only when user press Q or ESC
bool Render::poll()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_q ||
                        event.key.keysym.sym == SDLK_ESCAPE)
                    return false;
                break;
            case SDL_QUIT:
                return false;
                break;
            default:
                break;
        }
    }

    return true;
}
void Render::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, m_windowWidth, m_windowHeight);

    // can't make shader work ("empty screen") :(
    //glUseProgram(m_shaderProgram);

    mat4 projection = mat4::Perspective(.78f,
            (float)(m_windowWidth / m_windowHeight), .1f, 1000.0f);
    mat4 view = mat4::LookAt(m_cameraTarget, m_cameraPosition,
            vec3(.0f, 1.0f, .0f));

    mat3 rotation = mat3::RotationY(m_rotY);
    mat4 model = mat4::FromTranslationVector(
            vec3(0.0, 0.0, 8.0)) * mat4::FromRotationMatrix(rotation) *
            mat4::FromRotationMatrix(mat3::RotationX(1.));
    mat4 modelViewMatrix = model * view;

    glUniformMatrix4fv(m_modelViewMatrixUniform, 1, GL_FALSE,
            &modelViewMatrix[0]);
    glUniformMatrix4fv(m_projectionMatrixUniform, 1, GL_FALSE, &projection[0]);

    glUniform4f(m_colorUniform, 1, 0, 0, 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture0);
    glUniform1i(m_textUnit0Uniform, 0);

    glUniform3f(m_light0PositionUniform, m_light0Pos.x(), m_light0Pos.y(),
            m_light0Pos.z());

    glUniform3f(m_light0ColorUniform, m_light0Color.x(), m_light0Color.y(),
            m_light0Color.z());

    glBindVertexArray(m_sphereVertexArray);
    glDrawElements(GL_TRIANGLES,
            (GLsizei)m_shapes[m_shapdeIx].mesh.indices.size(), GL_UNSIGNED_INT,
            BUFFER_OFFSET(0));
    glBindVertexArray(0);

    m_rotY += 0.01f;

    // I've added this simple rotating (shrug)
    glRotatef(0.2f, 1.0f, 1.0f, 1.0f);

    SDL_GL_SwapWindow(m_sdlWindow.get());
}
