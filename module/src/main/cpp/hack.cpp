#include <cstdint>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <dlfcn.h>
#include <string>
#include <EGL/egl.h>
#include <GLES3/gl3.h>

#include "hack.h"
#include "log.h"
#include "game.h"
#include "utils.h"
#include "xdl.h"
#include "imgui.h"
#include "imgui_impl_android.h"
#include "imgui_impl_opengl3.h"
#include "MemoryPatch.h"
#include "Unity/Vector2.h"
#include "Unity/Vector3.h"
#include "Unity/Rect.h"
#include "Unity/Obscured.h"
#include "includes/ESPManager.h"
#include "includes/ESPOverlay.h"
//#include "Unity/Obscured.h


static int                  g_GlHeight, g_GlWidth;
static bool                 g_IsSetup = false;
static std::string          g_IniFileName = "";
static utils::module_info   g_TargetModule{};
static bool libLoaded = false;

bool NoRecoil;
bool UnlimitedAmmo;
bool espLine;
bool espRectangle; 

HOOKAF(void, Input, void *thiz, void *ex_ab, void *ex_ac) {
    origInput(thiz, ex_ab, ex_ac);
    ImGui_ImplAndroid_HandleInputEvent((AInputEvent *)thiz);
    return;
}


void (*SetResolution)(int width, int height, bool fullscreen);
int (*get_systemWidth)(void *instance);
int (*get_systemHeight)(void *instance);
void *(*D_get_main)();

void *(*get_transform)(void *instance);
Vector3 (*get_position)(void *instance);
Vector3 (*WorldToScreenPoint)(void *instance, Vector3 position);
void *(*C_get_main)(); 
int (*get_Team)(void *instance);


void (*old_noRecoil) (void*instance);
void noRecoil(void*instance) {
if (instance!=NULL && NoRecoil) {
return;
}
return old_noRecoil(instance);
}

void (*old_bypass)(void *instance, void *reason);
void bypass(void*instance, void *reason){
return;
}

void (*old_upDate)(void *instance);
void upDate(void *instance){
if (instance!=NULL){
bool dead = *(bool *) ((uintptr_t) instance + 0x28);
if (get_Team(instance) && !dead)
espManager->tryAddEnemy(instance);
else
espManager->removeEnemyGivenObject(instance);
}
old_upDate(instance);
}

void (*old_updateWeapon)(void*instance);
void updateWeapon(void *instance){
if (UnlimitedAmmo){
*(int*) ((uintptr_t) instance + 0x2A0 + 0x8) = 999999999;
*(int*) ((uintptr_t) instance + 0x2AC + 0x8) = 999999999;
 }
return old_updateWeapon(instance);
}



void SetupImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    io.IniFilename = g_IniFileName.c_str();
    io.DisplaySize = ImVec2((float)g_GlWidth, (float)g_GlHeight);

    ImGui_ImplAndroid_Init(nullptr);
    ImGui_ImplOpenGL3_Init("#version 300 es");
    ImGui::StyleColorsLight();

    ImFontConfig font_cfg;
    font_cfg.SizePixels = 22.0f;
    io.Fonts->AddFontDefault(&font_cfg);

    ImGui::GetStyle().ScaleAllSizes(3.0f);
}


void DrawESP(ImDrawList *draw) {
if (espLine || espRectangle) {
for (int i = 0; i < espManager->enemies.size(); i++) {
   void *obj = espManager->enemies[i]->object;
    if (obj != NULL) {
        Vector3 objPos = WorldToScreenPoint(C_get_main(), get_position(get_transform(obj)));
        if (objPos.z < 0) 
           continue;
           Vector3 objHeadPos = WorldToScreenPoint(C_get_main(), get_position(get_transform(obj)) + Vector3(0, 1.7, 0));
           ImVec2 toTarget = ImVec2(objPos.x, get_systemHeight(D_get_main()) - objPos.y);
           float rectHeight = abs(objHeadPos.y - objPos.y);
           float rectWidth = 0.6 * rectHeight;
           Rect rectSize = Rect(objPos.x - rectWidth /2, get_systemHeight(D_get_main()) - objPos.y, rectWidth, -rectHeight);
           if (espLine){
          ESP::DrawLine(ImVec2(get_systemWidth(D_get_main()) / 2, get_systemHeight(D_get_main())),  toTarget, ImColor(255, 255, 255, 255), 2);
              }
           if (espRectangle)
              ESP::DrawBox(rectSize, ImColor(255, 0, 0, 255), 2);
              }
             }
            }
           }
    
EGLBoolean (*old_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);
EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    eglQuerySurface(dpy, surface, EGL_WIDTH, &g_GlWidth);
    eglQuerySurface(dpy, surface, EGL_HEIGHT, &g_GlHeight);

    if (!g_IsSetup) {
      SetupImGui();
      g_IsSetup = true;
    }

    ImGuiIO &io = ImGui::GetIO();
    if (libLoaded)
    
    
   SetResolution(get_systemWidth(D_get_main()), get_systemHeight(D_get_main()), true);


    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplAndroid_NewFrame(g_GlWidth, g_GlHeight);
    ImGui::NewFrame();
    
    DrawESP(ImGui::GetBackgroundDrawList());

    ImGui::Begin("MGR Team - Sausage Man");
    if (ImGui::BeginTabBar("Tab", ImGuiTabBarFlags_FittingPolicyScroll)) {
        if (ImGui::BeginTabItem("Weapon Menu")) {
            ImGui::Checkbox("Esp Line", &espLine);
            ImGui::Checkbox("Esp Rectangle", &espRectangle);
            ImGui::Checkbox("No Recoil", &NoRecoil);
            ImGui::Checkbox("Unlimited Ammo", &UnlimitedAmmo);
        }
    }
    ImGui::EndTabItem();
    ImGui::EndTabBar();
    ImGui::End();

    ImGui::EndFrame();
    ImGui::Render();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    return old_eglSwapBuffers(dpy, surface);
}   

void hack_start(const char *_game_data_dir) {
    LOGI("hack start | %s", _game_data_dir);
    do {
        sleep(1);
        g_TargetModule = utils::find_module(TargetLibName);
    } while (g_TargetModule.size <= 0);
    LOGI("%s: %p - %p",TargetLibName, g_TargetModule.start_address, g_TargetModule.end_address);

    // TODO: hooking/patching here
      DobbyHook((void *) ((uintptr_t) g_TargetModule.start_address + 0x14711C0), (void *) noRecoil, (void **) &old_noRecoil);
      DobbyHook((void *) ((uintptr_t) g_TargetModule.start_address + 0x147306C), (void *) upDate, (void **) &old_upDate);
      DobbyHook((void *) ((uintptr_t) g_TargetModule.start_address + 0x15AE930), (void*) updateWeapon,(void**)&old_updateWeapon);                  
      DobbyHook((void *) ((uintptr_t) g_TargetModule.start_address + 0x14F2478), (void *) bypass,(void**)&old_bypass);
                                          
    SetResolution = (void (*)(int, int, bool)) ((uintptr_t) g_TargetModule.start_address + 0xA361EC); //class Screen, method: public static void SetResolution(int width, int height, bool fullscreen, int preferredRefreshRate) { }
    get_systemWidth = (int (*)(void *)) ((uintptr_t) g_TargetModule.start_address + 0xD1ADCC);//class Display, method: public Int get_systemWidth() { }
    get_systemHeight = (int (*)(void *)) ((uintptr_t) g_TargetModule.start_address + 0xD1AEC4);//class Display, method: public Int get_systemHeight() { }
    D_get_main = (void *(*)()) ((uintptr_t) g_TargetModule.start_address + 0xD1B0BC);//class Display, method: public static Display get_main() { } 
     
    get_transform = (void *(*) (void *)) ((uintptr_t) g_TargetModule.start_address + 0xD16C40);
    get_position = (Vector3 (*) (void *)) ((uintptr_t) g_TargetModule.start_address + 0x12BB7B4);
    WorldToScreenPoint = (Vector3 (*) (void *, Vector3)) ((uintptr_t) g_TargetModule.start_address + 0xD13944);
    C_get_main = (void *(*)()) ((uintptr_t) g_TargetModule.start_address + 0xD13EE0); 
    get_Team = (int (*) (void*)) ((uintptr_t) g_TargetModule.start_address + 0x1466B78);
    
    
    
    libLoaded = true;
}


void hack_prepare(const char *_game_data_dir) {
    LOGI("hack thread: %d", gettid());
    int api_level = utils::get_android_api_level();
    LOGI("api level: %d", api_level);
    g_IniFileName = std::string(_game_data_dir) + "/files/imgui.ini";
    sleep(5);

    void *sym_input = DobbySymbolResolver("/system/lib/libinput.so", "_ZN7android13InputConsumer21initializeMotionEventEPNS_11MotionEventEPKNS_12InputMessageE");
    if (NULL != sym_input){
        DobbyHook((void *)sym_input, (dobby_dummy_func_t) myInput, (dobby_dummy_func_t*)&origInput);
    }
    
    void *egl_handle = xdl_open("libEGL.so", 0);
    void *eglSwapBuffers = xdl_sym(egl_handle, "eglSwapBuffers", nullptr);
    if (NULL != eglSwapBuffers) {
        utils::hook((void*)eglSwapBuffers, (func_t)hook_eglSwapBuffers, (func_t*)&old_eglSwapBuffers);
    }
    xdl_close(egl_handle);

    hack_start(_game_data_dir);
}
