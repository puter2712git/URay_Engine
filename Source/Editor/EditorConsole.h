#pragma once

#include <imgui/imgui.h>

#include <cctype>

namespace URay
{

class EditorConsole
{
public:
    EditorConsole();
    ~EditorConsole();

public:
    void ClearLog();
    void AddLog(const char* fmt, ...) IM_FMTARGS(2);

    void Draw(const char* title, bool* pOpen);

    void ExecCommand(const char* commandLine);

    int TextEditCallback(ImGuiInputTextCallbackData* data);

private:
    static int Stricmp(const char* s1, const char* s2)
    {
        int d;
        while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1)
        {
            s1++;
            s2++;
        }
        return d;
    }
    static int Strnicmp(const char* s1, const char* s2, int n)
    {
        int d = 0;
        while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1)
        {
            s1++;
            s2++;
            n--;
        }
        return d;
    }
    static char* Strdup(const char* s)
    {
        IM_ASSERT(s);
        size_t len = strlen(s) + 1;
        void* buf = ImGui::MemAlloc(len);
        IM_ASSERT(buf);
        return (char*)memcpy(buf, (const void*)s, len);
    }
    static void Strtrimblanks(char* s)
    {
        char* str_end = s + strlen(s);
        while (str_end > s && str_end[-1] == ' ')
            str_end--;
        *str_end = 0;
    }

private:
    char inputBuffer[256];
    ImVector<char*> items;
    ImVector<const char*> commands;
    ImVector<char*> history;
    int historyPos = -1;
    ImGuiTextFilter filter = {};
    bool autoScroll = false;
    bool scrollToBottom = false;
};

} // namespace URay
