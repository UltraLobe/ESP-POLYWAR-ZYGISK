j#include <cstdint>
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

bool espLine;

HOOKAF(void, Input, void *thiz, void *ex_ab, void *ex_ac) {
    origInput(thiz, ex_ab, ex_ac);
    ImGui_ImplAndroid_HandleInputEvent((AInputEvent *)thiz);
    return;
}

jsjsjs

//void (*SetResolution)(int width, int height, bool fullscreen);
/*int (*get_systemWidth)(void *instance);
int (*get_systemHeight)(void *instance);
void *(*D_get_main)();

void *(*get_transform)(void *instance);
Vector3 (*get_position)(void *instance);
Vector3 (*WorldToScreenPoint)(void *instance, Vector3 position);
void *(*C_get_main)(); 

void (*old_upDate)(void*instance);
void upDate(void*instance){
	if (instance!=NULL){
	   espManager->tryAddEnemy(instance);
	}
  old_upDate(instance);
}

void (*old_onDestroy)(void*instance);
void onDestroy(void*instance){
	if (instance!=NULL){
	   espManager->removeEnemyGivenObject(instance);
	}
	old_onDestroy(instance);
   }*/

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

void DrawESP(ImDrawList *draw){
/*if (espLine) {
if (C_get_main() != NULL){
for (int i = 0; i < espManager->enemies.size(); i++) {
   void *obj = espManager->enemies[i]->object;
    if (obj != NULL) {
        Vector3 objPos = WorldToScreenPoint(C_get_main(), get_position(get_transform(obj)));
        if (objPos.z < 0) 
           continue;       
           ImVec2 toTarget = ImVec2(objPos.x, get_systemHeight(D_get_main()) - objPos.y);                
           if (espLine){
              ESP::DrawLine(ImVec2(get_systemWidth(D_get_main()) /2, get_systemHeight(D_get_main())),  toTarget, ImColor(255, 255, 255, 255), 2);        
               }                          
              }
             }
            }
           }*/
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
    
    
   //SetResolution(get_systemWidth(D_get_main()), get_systemHeight(D_get_main()), true);


    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplAndroid_NewFrame(g_GlWidth, g_GlHeight);
    ImGui::NewFrame();
    
    DrawESP(ImGui::GetBackgroundDrawList());

    ImGui::Begin("MGR Team - Sausage Man");
    if (ImGui::BeginTabBar("Tab", ImGuiTabBarFlags_FittingPolicyScroll)) {
        if (ImGui::BeginTabItem("Weapon Menu")) {
            ImGui::Checkbox("Esp Line", &espLine);          
       //     ImGui::Checkbox("No Recoil", &NoRecoil);
        //    ImGui::Checkbox("Unlimited Ammo", &UnlimitedAmmo);
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
        sleep(10);
        g_TargetModule = utils::find_module(TargetLibName);
    } while (g_TargetModule.size <= 0);
    LOGI("%s: %p - %p",TargetLibName, g_TargetModule.start_address, g_TargetModule.end_address);

    // TODO: hooking/patching here
      //DobbyHook((void *) ((uintptr_t) g_TargetModule.start_address + 0x1bbdb18), (void *) upDate, (void **) &old_upDate);
      //DobbyHook((void *) ((uintptr_t) g_TargetModule.start_address + 0x1bc955c), (void *) onDestroy,(void**)&old_onDestroy);                            
                                                                                                               
    //SetResolution = (void (*)(int, int, bool)) ((uintptr_t) g_TargetModule.start_address + 0x2753a34); //class Screen, method: public static void SetResolution(int width, int height, bool fullscreen, int preferredRefreshRate) { }
    //get_systemWidth = (int (*)(void *)) ((uintptr_t) g_TargetModule.start_address + 0x376158c);//class Display, method: public Int get_systemWidth() { }
    //get_systemHeight = (int (*)(void *)) ((uintptr_t) g_TargetModule.start_address + 0x3761674);//class Display, method: public Int get_systemHeight() { }
    //D_get_main = (void *(*)()) ((uintptr_t) g_TargetModule.start_address + 0x37618d8);//class Display, method: public static Display get_main() { } 
     
    //get_transform = (void *(*) (void *)) ((uintptr_t) g_TargetModule.start_address + 0x378ca8c);
    //get_position = (Vector3 (*) (void *)) ((uintptr_t) g_TargetModule.start_address + 0x3799654);
    //WorldToScreenPoint = (Vector3 (*) (void *, Vector3)) ((uintptr_t) g_TargetModule.start_address + 0x3758170);
    //C_get_main = (void *(*)()) ((uintptr_t) g_TargetModule.start_address + 0x37586c8); 
    
    
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
