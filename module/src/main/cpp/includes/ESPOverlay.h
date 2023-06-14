#ifndef ImGuiAndroid_ESP
#define ImGuiAndroid_ESP

#include "../ImGui/imgui_internal.h"

namespace ESP {
    void DrawLine(ImVec2 start, ImVec2 end, ImVec4 color, float size) {
        auto background = ImGui::GetBackgroundDrawList();
        if(background) {
            background->AddLine(start, end, ImColor(color.x,color.y,color.z,color.w), size);
        }
    }
	
    void DrawBox(Rect rect, ImVec4 color, float size) {
        ImVec2 v1(rect.x, rect.y);
        ImVec2 v2(rect.x + rect.w, rect.y);
        ImVec2 v3(rect.x + rect.w, rect.y + rect.h);
        ImVec2 v4(rect.x, rect.y + rect.h);

        DrawLine(v1, v2, color, size);
        DrawLine(v2, v3, color, size);
        DrawLine(v3, v4, color, size);
        DrawLine(v4, v1, color, size);
    }
	
	void DrawHorizontalHealthBar(Vector2 screenPos, float width, float maxHealth, float currentHealth) {
		screenPos -= Vector2(0.0f, 8.0f);
		DrawBox(Rect(screenPos.X, screenPos.Y, width + 2, 5.0f), ImColor(0, 0, 0, 255), 3);
		screenPos += Vector2(1.0f, 1.0f);
		ImColor clr = ImColor(0, 255, 0, 255);
		float hpWidth = (currentHealth * width) / maxHealth;
		if (currentHealth <= (maxHealth * 0.6)) {
			clr = ImColor(255, 255, 0, 255);
		}
		if (currentHealth < (maxHealth * 0.3)) {
			clr = ImColor(255, 0, 0, 255);
		}
		DrawBox(Rect(screenPos.X, screenPos.Y, hpWidth, 3.0f), clr, 3);
	}
	
    void DrawCircle(float X, float Y, float radius, bool filled, ImVec4 color) {
        auto background = ImGui::GetBackgroundDrawList();
        if(background) {
            if(filled) {
                background->AddCircleFilled(ImVec2(X, Y), radius, ImColor(color.x,color.y,color.z,color.w));
            } else {
                background->AddCircle(ImVec2(X, Y), radius, ImColor(color.x,color.y,color.z,color.w));
            }
        }
    }
	
    void DrawText(ImVec2 position, ImVec4 color, const char *text) {
        auto background = ImGui::GetBackgroundDrawList();
        if(background) {
            background->AddText(NULL, 15.0f, position, ImColor(color.x,color.y,color.z,color.w), text);
        }
    }
}

#endif ImGuiAndroid_ESP
