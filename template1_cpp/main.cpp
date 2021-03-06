#include "common.h"
#include "Image.h"
#include "Player.h"
#include <map>
#include <vector>

#define GLFW_DLL

#include <GLFW/glfw3.h>

int **wall = new int *[532];
int step = 0;
bool ke = true;
bool lose = false, win = false;
constexpr GLsizei WINDOW_WIDTH = 816, WINDOW_HEIGHT = 576;

struct InputState {
    bool keys[1024]{}; //массив состояний кнопок - нажата/не нажата
    GLfloat lastX = 400, lastY = 300; //исходное положение мыши
    bool firstMouse = true;
    bool captureMouse = true;  // Мышка захвачена нашим приложением или нет?
    bool capturedMouseJustNow = false;
} static Input;


void TryActin();

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;


void OnKeyboardPressed(GLFWwindow *window, int key, int scancode, int action, int mode) {
    switch (key) {
        case GLFW_KEY_ESCAPE:
            if (action == GLFW_PRESS)
                glfwSetWindowShouldClose(window, GL_TRUE);
            break;
        case GLFW_KEY_1:
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            break;
        case GLFW_KEY_2:
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            break;
        default:
            if (action == GLFW_PRESS)
                Input.keys[key] = true;
            else if (action == GLFW_RELEASE)
                Input.keys[key] = false;
    }
}

void processPlayerMovement(Player &player) {
    if (Input.keys[GLFW_KEY_W])
        player.ProcessInput(MovementDir::UP, wall);
    else if (Input.keys[GLFW_KEY_S])
        player.ProcessInput(MovementDir::DOWN, wall);
    if (Input.keys[GLFW_KEY_A])
        player.ProcessInput(MovementDir::LEFT, wall);
    else if (Input.keys[GLFW_KEY_D])
        player.ProcessInput(MovementDir::RIGHT, wall);
}

void OnMouseButtonClicked(GLFWwindow *window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
        Input.captureMouse = !Input.captureMouse;

    if (Input.captureMouse) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        Input.capturedMouseJustNow = true;
    } else
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

}

void OnMouseMove(GLFWwindow *window, double xpos, double ypos) {
    if (Input.firstMouse) {
        Input.lastX = float(xpos);
        Input.lastY = float(ypos);
        Input.firstMouse = false;
    }

    GLfloat xoffset = float(xpos) - Input.lastX;
    GLfloat yoffset = Input.lastY - float(ypos);

    Input.lastX = float(xpos);
    Input.lastY = float(ypos);
}


void OnMouseScroll(GLFWwindow *window, double xoffset, double yoffset) {
    // ...
}


int initGL() {
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }

    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    std::cout << "Controls: " << std::endl;
    std::cout << "press right mouse button to capture/release mouse cursor  " << std::endl;
    std::cout << "W, A, S, D - movement  " << std::endl;
    std::cout << "press ESC to exit" << std::endl;

    return 0;
}

std::map<char, std::pair<int, int>> m{{'.', {0,   144}},
                                      {'#', {48,  96}},
                                      {'D', {96,  336}},
                                      {'U', {96,  336}},
                                      {'R', {192, 192}},
                                      {'K', {0,   144}},
                                      {'L', {192, 240}}};
std::vector<char> inventory;
std::vector<std::vector<int>> maps{{-1, -1, 1,  5}, {0,  -1, 2,  6}, {1,  -1, 3,  7}, {2,  -1, 4,  8}, {3,  -1, -1, 9},
                                   {-1, 0,  6,  10}, {5,  1,  7,  11}, {6,  2,  8,  12}, {7,  3,  9,  13}, {8,  4,  -1, 14},
                                   {-1, 5,  11, 15}, {10, 6,  12, 16}, {11, 7,  13, 17}, {12, 8,  14, 18}, {13, 9,  -1, 19},
                                   {-1, 10, 16, -1}, {15, 11, 17, -1}, {16, 12, 18, -1}, {17, 13, 19, -1}, {18, 14, -1, 20}};

void OpenDor(std::string adr) {

}

void AddSomeStuff(Image &screen, Image &levelB, Image &Plib, int x, int y, int xl, int yl) {
    for (int j = 0; j < 48; ++j) {
        for (int i = 0; i < 48; ++i) {
            Pixel now = Plib.GetIPixel(xl + i, yl + j);
            if ((now.r + now.g + now.b + now.a) > 0) {
                screen.PutPixel(x + i, y + j, Plib.GetIPixel(xl + i, yl + j));
                levelB.PutPixel(x + i, y + j, Plib.GetIPixel(xl + i, yl + j));
            }
        }
    }
}

void AddSomeAStuff(Image &screen, Image &levelB, Image &Plib, int x, int y, int n) {
    for (int j = 0; j < 48; ++j) {
        for (int i = 0; i < 48; ++i) {
            Pixel now = levelB.GetPixel(x + i,  y + j);
            screen.PutPixel(x + i, y + j, now);
        }
    }
    for (int j = 0; j < 48; ++j) {
        for (int i = 0; i < 48; ++i) {
            Pixel now = Plib.GetIPixel(48 * n + i,  j);
            if ((now.r + now.g + now.b + now.a) > 0) {
                screen.PutPixel(x + i, y + j, now);
            }
        }
    }
}

void LevelUpdate(Image &screen, Image &levelB, Image &Plib, int Nlevel) {
    std::string line;
    std::string s = std::to_string(Nlevel);
    std::string adr = "../resources/map";
    adr = adr + s + ".txt";
    std::ifstream in(adr);
    if (in.is_open()) {
        std::cout << "updates " << adr << std::endl;
        for (int j = 0; j < 11; ++j) {
            getline(in, line);
            //std::cout << line << std::endl;
            for (int i = 0; i < 17; ++i) {
                //std::cout << line[i] << std::endl;
                for (int y = 0; y < 48; ++y) {
                    for (int x = 0; x < 48; ++x) {
                        //std::cout <<x << ' '<< y << std::endl;
                        screen.PutPixel(i * 48 + x, j * 48 + y,Plib.GetIPixel(m[line[i]].first + x, m[line[i]].second + y));
                        //std::cout <<x << ' '<< y << std::endl;
                        levelB.PutPixel(i * 48 + x, j * 48 + y,Plib.GetIPixel(m[line[i]].first + x, m[line[i]].second + y));
                        //std::cout <<x << ' '<< y << std::endl;
                        switch (line[i]) {
                            case '#':
                                wall[j * 48 + y][i * 48 + x] = 1;
                                break;
                            case 'L':
                                wall[j * 48 + y][i * 48 + x] = 100;
                                break;
                            case 'U':
                                wall[j * 48 + y][i * 48 + x] = 101;
                                break;
                            case 'R':
                                wall[j * 48 + y][i * 48 + x] = 102;
                                break;
                            case 'D':
                                wall[j * 48 + y][i * 48 + x] = 103;
                                break;
                            case 'l':
                                wall[j * 48 + y][i * 48 + x] = 110;
                                break;
                            case 'u':
                                wall[j * 48 + y][i * 48 + x] = 111;
                                break;
                            case 'r':
                                wall[j * 48 + y][i * 48 + x] = 112;
                                break;
                            case 'd':
                                wall[j * 48 + y][i * 48 + x] = 113;
                                break;
                            case 'K':
                                if (ke)
                                    wall[j * 48 + y][i * 48 + x] = 5;
                                else
                                    wall[j * 48 + y][i * 48 + x] = 0;
                                break;
                            case 'j':
                                wall[j * 48 + y][i * 48 + x] = 120;
                                break;
                            default:
                                wall[j * 48 + y][i * 48 + x] = 0;
                                break;
                        }
                    }
                }
                switch (line[i]) {
                    case 'j':
                        AddSomeStuff(screen, levelB, Plib, i * 48, j * 48, 0, 336);
                    case 'R':
                        AddSomeStuff(screen, levelB, Plib, 768, 192, 192, 144);
                    default:
                        break;
                }
            }
        }
        in.close();
    }
}

void UpdateInventory(Image &screen, Image &levelB) {
    Image inv("../resources/inv.png");
    for (int j = 528; j < 576; j++) {
        for (int i = 0; i < 816; i++) {
            screen.PutPixel(i, j,inv.GetIPixel(i, j - 528));
            levelB.PutPixel(i , j,inv.GetIPixel(i, j - 528));
        }
    }
    Image k("../resources/key.png");
    for (int i = 0; i < inventory.size(); i++){
        switch (inventory[i]) {
            case 'K':
                AddSomeStuff(screen, levelB, k, 288 + i * 48, 528, 0, 0);
                break;
            default:
                break;
        }
    }
}

void TryActin(Player &player, Image &screen, Image &levelB, Image &Plib, int *Nlevel) {
    if (Input.keys[GLFW_KEY_G]) {
        //std::cout << "lox " << wall[player.GetCoords().y + 24][player.GetCoords().x + 24];

        switch (wall[player.GetCoords().y + 24][player.GetCoords().x + 24]) {
            case 100:
                std::cout << *Nlevel << " ";
                LevelUpdate(screen, levelB, Plib, maps[*Nlevel][0]);
                *Nlevel = maps[*Nlevel][0];
                std::cout << *Nlevel << std::endl;
                player.SetCoords(768, 240);
                break;
            case 101:
                std::cout << *Nlevel << " ";
                LevelUpdate(screen, levelB, Plib, maps[*Nlevel][1]);
                *Nlevel = maps[*Nlevel][1];
                std::cout << *Nlevel << std::endl;
                player.SetCoords(384, 480);
                break;
            case 102:
                std::cout << *Nlevel << " ";
                LevelUpdate(screen, levelB, Plib, maps[*Nlevel][2]);
                *Nlevel = maps[*Nlevel][2];
                std::cout << *Nlevel << std::endl;
                player.SetCoords(0, 240);
                break;
            case 103:
                std::cout << *Nlevel << " ";
                LevelUpdate(screen, levelB, Plib, maps[*Nlevel][3]);
                *Nlevel = maps[*Nlevel][3];
                std::cout << *Nlevel << std::endl;
                player.SetCoords(384, 0);
                break;
            case 120:
                if ((inventory.size() > 0) && (inventory[0] == 'K')) {
                    win = true;
                }
                break;
            case 5:
                //std::cout << "yes" << std::endl;
                for(int i = 96; i < 98 + tileSize; i++) {
                    for(int j = 96; j < 98 + tileSize; j++) {
                        wall[i][j] = 0;
                    }
                }
                ke = false;
                inventory.push_back ('K');
                UpdateInventory(screen, levelB);
                OpenDor("../resources/map19.txt");
                break;
            default:
                /*std::cout << "yes" << std::endl;
                std::fstream file("../resources/map1.txt");
                file.seekp(5);
                file.write(".", 1);
                file.close();*/
                break;
        }
    }
}

int main(int argc, char **argv) {
    if (!glfwInit())
        return -1;

//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, "task1 base project", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, OnKeyboardPressed);
    glfwSetCursorPosCallback(window, OnMouseMove);
    glfwSetMouseButtonCallback(window, OnMouseButtonClicked);
    glfwSetScrollCallback(window, OnMouseScroll);

    if (initGL() != 0)
        return -1;

    //Reset any OpenGL errors which could be present for some reason
    GLenum gl_error = glGetError();
    while (gl_error != GL_NO_ERROR)
        gl_error = glGetError();

    Point starting_pos{.x = 48, .y = 48};
    Player player{starting_pos};
    player.color = {.r = 24, .g = 255, .b = 34, .a = 5};
    Image img("../resources/tex.png");
    Image img2("../resources/pnt.png");
    Image imgp("../resources/pg.png");
    Image key("../resources/key.png");
    Image screenBuffer(WINDOW_WIDTH, WINDOW_HEIGHT, 4);
    Image levelBackground(WINDOW_WIDTH, WINDOW_HEIGHT, 4);
    std::cout << screenBuffer.Height() << ' ' << screenBuffer.Width() << std::endl;
    std::cout << levelBackground.Height() << ' ' << levelBackground.Width() << std::endl;
    //std::string line;
    //std::ifstream in("../resources/map.txt");
    int nowmap = 0;
    for (int i = 0; i < 528; ++i)
        wall[i] = new int[816];

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    GL_CHECK_ERRORS;
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    GL_CHECK_ERRORS;
    LevelUpdate(screenBuffer, levelBackground, img2, 0);
    AddSomeStuff(screenBuffer, levelBackground, img2, 0, 0, 336, 432);
    std::uint8_t hjhk = 8;
    std::cout << unsigned(hjhk) << std::endl;
    std::cout << unsigned(img2.GetIPixel(336, 432).r) << " " << std::endl <<
              unsigned(img2.GetIPixel(336, 432).g) << " " << std::endl <<
              unsigned(img2.GetIPixel(336, 432).b) << " " << std::endl <<
              unsigned(img2.GetIPixel(336, 432).a) << " " << std::endl;

    for (int i = 0; i < (528 / 48); ++i) {
        for (int j = 0; j < (816 / 48); ++j) {
            std::cout << wall[i * 48][j * 48];
        }
        std::cout << std::endl;
    }
    UpdateInventory(screenBuffer, levelBackground);
    int nlvl = 0;
    //game loop
    while (!glfwWindowShouldClose(window)) {
        step += 1;
        step %= 200;
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        //std::cout << lastFrame << std::endl;
        glfwPollEvents();
        if ((!lose) && (!win)) {
            processPlayerMovement(player);
            //if (step % 25 == 0)
            if ((nlvl == 0) && ke)
                AddSomeAStuff(screenBuffer, levelBackground, key, 96, 96, step / 25);
            TryActin(player, screenBuffer, levelBackground, img2, &nlvl);
            player.Draw(screenBuffer, levelBackground, imgp);
        } else if (lose){

        } else if (win){
            Image winn("../resources/win.png");
            for (int j = 0; j < 576; ++j) {
                for (int i = 0; i < 816; ++i) {
                    screenBuffer.PutPixel(i, j, winn.GetIPixel(i, j));
                    levelBackground.PutPixel(i, j, winn.GetIPixel(i, j));
                }
            }
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        GL_CHECK_ERRORS;
        glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer.Data());
        GL_CHECK_ERRORS;

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
