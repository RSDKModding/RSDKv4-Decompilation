#include "RetroEngine.hpp"

int(__cdecl *nativeFunction[16])(int, void *);

char gamePath[0x100];
int saveRAM[SAVEDATA_MAX];
Achievement achievements[ACHIEVEMENT_MAX];
LeaderboardEntry leaderboard[LEADERBOARD_MAX];

void InitUserdata()
{
    // userdata files are loaded from this directory
    sprintf(gamePath, "%s", "");

    char buffer[0x100];
    sprintf(buffer, "%ssettings.ini", gamePath);
    FILE *file = fopen(buffer, "rb");
    if (!file) {
        IniParser ini;

        ini.SetBool("Dev", "DevMenu", Engine.devMenu = false);
        ini.SetBool("Dev", "StartingCategory", Engine.startList = 0);
        ini.SetBool("Dev", "StartingScene", Engine.startStage = 0);
        ini.SetBool("Dev", "FastForwardSpeed", Engine.fastForwardSpeed = 8);

        ini.SetBool("Game", "Language", Engine.language = RETRO_EN);

        ini.SetBool("Window", "Fullscreen", Engine.fullScreen = false);
        ini.SetBool("Window", "Borderless", Engine.borderless = false);
        ini.SetBool("Window", "VSync", Engine.vsync = false);
        ini.SetInteger("Window", "WindowScale", Engine.windowScale = 2);
        ini.SetInteger("Window", "ScreenWidth", SCREEN_XSIZE = 424);
        ini.SetInteger("Window", "RefreshRate", Engine.refreshRate = 60);

        ini.SetInteger("Keyboard 1", "Up", inputDevice[0].keyMappings = SDL_SCANCODE_UP);
        ini.SetInteger("Keyboard 1", "Down", inputDevice[1].keyMappings = SDL_SCANCODE_DOWN);
        ini.SetInteger("Keyboard 1", "Left", inputDevice[2].keyMappings = SDL_SCANCODE_LEFT);
        ini.SetInteger("Keyboard 1", "Right", inputDevice[3].keyMappings = SDL_SCANCODE_RIGHT);
        ini.SetInteger("Keyboard 1", "A", inputDevice[4].keyMappings = SDL_SCANCODE_Z);
        ini.SetInteger("Keyboard 1", "B", inputDevice[5].keyMappings = SDL_SCANCODE_X);
        ini.SetInteger("Keyboard 1", "C", inputDevice[6].keyMappings = SDL_SCANCODE_C);
        ini.SetInteger("Keyboard 1", "Start", inputDevice[7].keyMappings = SDL_SCANCODE_RETURN);

        ini.SetInteger("Controller 1", "Up", inputDevice[0].contMappings = SDL_CONTROLLER_BUTTON_DPAD_UP);
        ini.SetInteger("Controller 1", "Down", inputDevice[1].contMappings = SDL_CONTROLLER_BUTTON_DPAD_DOWN);
        ini.SetInteger("Controller 1", "Left", inputDevice[2].contMappings = SDL_CONTROLLER_BUTTON_DPAD_LEFT);
        ini.SetInteger("Controller 1", "Right", inputDevice[3].contMappings = SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
        ini.SetInteger("Controller 1", "A", inputDevice[4].contMappings = SDL_CONTROLLER_BUTTON_A);
        ini.SetInteger("Controller 1", "B", inputDevice[5].contMappings = SDL_CONTROLLER_BUTTON_B);
        ini.SetInteger("Controller 1", "C", inputDevice[6].contMappings = SDL_CONTROLLER_BUTTON_X);
        ini.SetInteger("Controller 1", "Start", inputDevice[7].contMappings = SDL_CONTROLLER_BUTTON_START);

        ini.Write("settings.ini");
    }
    else {
        fclose(file);
        IniParser ini("settings.ini");

        if (!ini.GetBool("Dev", "DevMenu", &Engine.devMenu))
            Engine.devMenu = false;
        if (!ini.GetInteger("Dev", "StartingCategory", &Engine.startList))
            Engine.startList = 0;
        if (!ini.GetInteger("Dev", "StartingScene", &Engine.startStage))
            Engine.startStage = 0;
        if (!ini.GetInteger("Dev", "FastForwardSpeed", &Engine.fastForwardSpeed))
            Engine.fastForwardSpeed = 8;

        if (!ini.GetInteger("Game", "Language", &Engine.language))
            Engine.language = RETRO_EN;

        if (!ini.GetBool("Window", "FullScreen", &Engine.fullScreen))
            Engine.fullScreen = false;
        if (!ini.GetBool("Window", "Borderless", &Engine.borderless))
            Engine.borderless = false;
        if (!ini.GetBool("Window", "VSync", &Engine.vsync))
            Engine.vsync = false;
        if (!ini.GetInteger("Window", "WindowScale", &Engine.windowScale))
            Engine.windowScale = 2;
        if (!ini.GetInteger("Window", "ScreenWidth", &SCREEN_XSIZE))
            SCREEN_XSIZE = 424;
        if (!ini.GetInteger("Window", "Refresh Rate", &Engine.refreshRate))
            Engine.refreshRate = 60;

        if (!ini.GetInteger("Keyboard 1", "Up", &inputDevice[0].keyMappings))
            inputDevice[0].keyMappings = SDL_SCANCODE_UP;
        if (!ini.GetInteger("Keyboard 1", "Down", &inputDevice[1].keyMappings))
            inputDevice[1].keyMappings = SDL_SCANCODE_DOWN;
        if (!ini.GetInteger("Keyboard 1", "Left", &inputDevice[2].keyMappings))
            inputDevice[2].keyMappings = SDL_SCANCODE_LEFT;
        if (!ini.GetInteger("Keyboard 1", "Right", &inputDevice[3].keyMappings))
            inputDevice[3].keyMappings = SDL_SCANCODE_RIGHT;
        if (!ini.GetInteger("Keyboard 1", "A", &inputDevice[4].keyMappings))
            inputDevice[4].keyMappings = SDL_SCANCODE_Z;
        if (!ini.GetInteger("Keyboard 1", "B", &inputDevice[5].keyMappings))
            inputDevice[5].keyMappings = SDL_SCANCODE_X;
        if (!ini.GetInteger("Keyboard 1", "C", &inputDevice[6].keyMappings))
            inputDevice[6].keyMappings = SDL_SCANCODE_C;
        if (!ini.GetInteger("Keyboard 1", "Start", &inputDevice[7].keyMappings))
            inputDevice[7].keyMappings = SDL_SCANCODE_RETURN;

        if (!ini.GetInteger("Controller 1", "Up", &inputDevice[0].contMappings))
            inputDevice[0].contMappings = SDL_CONTROLLER_BUTTON_DPAD_UP;
        if (!ini.GetInteger("Controller 1", "Down", &inputDevice[1].contMappings))
            inputDevice[1].contMappings = SDL_CONTROLLER_BUTTON_DPAD_DOWN;
        if (!ini.GetInteger("Controller 1", "Left", &inputDevice[2].contMappings))
            inputDevice[2].contMappings = SDL_CONTROLLER_BUTTON_DPAD_LEFT;
        if (!ini.GetInteger("Controller 1", "Right", &inputDevice[3].contMappings))
            inputDevice[3].contMappings = SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
        if (!ini.GetInteger("Controller 1", "A", &inputDevice[4].contMappings))
            inputDevice[4].contMappings = SDL_CONTROLLER_BUTTON_A;
        if (!ini.GetInteger("Controller 1", "B", &inputDevice[5].contMappings))
            inputDevice[5].contMappings = SDL_CONTROLLER_BUTTON_B;
        if (!ini.GetInteger("Controller 1", "C", &inputDevice[6].contMappings))
            inputDevice[6].contMappings = SDL_CONTROLLER_BUTTON_X;
        if (!ini.GetInteger("Controller 1", "Start", &inputDevice[7].contMappings))
            inputDevice[7].contMappings = SDL_CONTROLLER_BUTTON_START;
    }
    SetScreenSize(SCREEN_XSIZE, SCREEN_YSIZE);

    sprintf(buffer, "%suserdata.bin", gamePath);
    file = fopen(buffer, "rb");
    if (file) {
        fclose(file);
        ReadUserdata();
    }
    else {
        WriteUserdata();
    }

    sprintf(achievements[0].name, "%s", "88 Miles Per Hour");
    sprintf(achievements[1].name, "%s", "Just One Hug is Enough");
    sprintf(achievements[2].name, "%s", "Paradise Found");
    sprintf(achievements[3].name, "%s", "Take the High Road");
    sprintf(achievements[4].name, "%s", "King of the Rings");
    sprintf(achievements[5].name, "%s", "Statue Saviour");
    sprintf(achievements[6].name, "%s", "Heavy Metal");
    sprintf(achievements[7].name, "%s", "All Stages Clear");
    sprintf(achievements[8].name, "%s", "Treasure Hunter");
    sprintf(achievements[9].name, "%s", "Dr Eggman Got Served");
    sprintf(achievements[10].name, "%s", "Just In Time");
    sprintf(achievements[11].name, "%s", "Saviour of the Planet");
}

void writeSettings() {
    IniParser ini;

    ini.SetBool("Dev", "DevMenu", Engine.devMenu);
    ini.SetBool("Dev", "StartingCategory", Engine.startList);
    ini.SetBool("Dev", "StartingScene", Engine.startStage);
    ini.SetInteger("Dev", "FastForwardSpeed", Engine.fastForwardSpeed);

    ini.SetInteger("Game", "Language", Engine.language);

    ini.SetBool("Window", "Fullscreen", Engine.fullScreen);
    ini.SetBool("Window", "Borderless", Engine.borderless);
    ini.SetBool("Window", "VSync", Engine.vsync);
    ini.SetInteger("Window", "WindowScale", Engine.windowScale);
    ini.SetInteger("Window", "ScreenWidth", SCREEN_XSIZE);
    ini.SetInteger("Window", "RefreshRate", Engine.refreshRate);

    ini.SetInteger("Keyboard 1", "Up", inputDevice[0].keyMappings);
    ini.SetInteger("Keyboard 1", "Down", inputDevice[1].keyMappings);
    ini.SetInteger("Keyboard 1", "Left", inputDevice[2].keyMappings);
    ini.SetInteger("Keyboard 1", "Right", inputDevice[3].keyMappings);
    ini.SetInteger("Keyboard 1", "A", inputDevice[4].keyMappings);
    ini.SetInteger("Keyboard 1", "B", inputDevice[5].keyMappings);
    ini.SetInteger("Keyboard 1", "C", inputDevice[6].keyMappings);
    ini.SetInteger("Keyboard 1", "Start", inputDevice[7].keyMappings);

    ini.SetInteger("Controller 1", "Up", inputDevice[0].contMappings);
    ini.SetInteger("Controller 1", "Down", inputDevice[1].contMappings);
    ini.SetInteger("Controller 1", "Left", inputDevice[2].contMappings);
    ini.SetInteger("Controller 1", "Right", inputDevice[3].contMappings);
    ini.SetInteger("Controller 1", "A", inputDevice[4].contMappings);
    ini.SetInteger("Controller 1", "B", inputDevice[5].contMappings);
    ini.SetInteger("Controller 1", "C", inputDevice[6].contMappings);
    ini.SetInteger("Controller 1", "Start", inputDevice[7].contMappings);

    ini.Write("settings.ini");
}

void ReadUserdata()
{
    char buffer[0x100];
    sprintf(buffer, "%suserdata.bin", gamePath);
    FILE *userFile = fopen(buffer, "rb");
    if (!userFile)
        return;

    int buf = 0;
    for (int a = 0; a < ACHIEVEMENT_MAX; ++a) {
        fread(&buffer, 4, 1, userFile);
        achievements[a].status = buf;
    }
    for (int l = 0; l < LEADERBOARD_MAX; ++l) {
        fread(&buffer, 4, 1, userFile);
        leaderboard[l].status = buf;
    }

    fclose(userFile);

    if (Engine.onlineActive) {
        // Load from online
    }
}

void WriteUserdata()
{
    char buffer[0x100];
    sprintf(buffer, "%suserdata.bin", gamePath);
    FILE *userFile = fopen(buffer, "wb");
    if (!userFile)
        return;

    for (int a = 0; a < ACHIEVEMENT_MAX; ++a) fwrite(&achievements[a].status, 4, 1, userFile);
    for (int l = 0; l < LEADERBOARD_MAX; ++l) fwrite(&leaderboard[l].status, 4, 1, userFile);

    fclose(userFile);

    if (Engine.onlineActive) {
        // Load from online
    }
}