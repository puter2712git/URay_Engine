#include "ConsoleWidget.h"
#include "ConsoleLogSink.h"

#include "Core/Log/Log.h"

#include <cstdio>

namespace URay
{

ConsoleWidget::ConsoleWidget()
{
    logSink = new EditorConsoleLogSink(*this);

    Logger::RegisterSink(logSink);
}

ConsoleWidget::~ConsoleWidget()
{
    Logger::UnregisterSink(logSink);
    delete logSink;
}

void ConsoleWidget::ClearLog()
{
    for (int i = 0; i < items.Size; ++i)
    {
        ImGui::MemFree(items[i]);
    }
    items.clear();
}

static int TextEditCallbackStub(ImGuiInputTextCallbackData* data);

void ConsoleWidget::OnDraw()
{
    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Console"))
    {
        ImGui::End();
        return;
    }

    ImGui::TextWrapped(
        "This example implements a console with basic coloring, completion (TAB key) and history (Up/Down keys). A more elaborate "
        "implementation may want to store entries along with extra data such as timestamp, emitter, etc.");
    ImGui::TextWrapped("Enter 'HELP' for help.");

    if (ImGui::SmallButton("Add Debug Text"))
    {
        AddLog("%d some text", items.Size);
        AddLog("some more text");
        AddLog("display very important message here!");
    }
    ImGui::SameLine();
    if (ImGui::SmallButton("Add Debug Error"))
    {
        AddLog("[error] something went wrong");
    }
    ImGui::SameLine();
    if (ImGui::SmallButton("Clear"))
    {
        ClearLog();
    }
    ImGui::SameLine();
    bool copy_to_clipboard = ImGui::SmallButton("Copy");

    ImGui::Separator();

    if (ImGui::BeginPopup("Options"))
    {
        ImGui::Checkbox("Auto-scroll", &autoScroll);
        ImGui::EndPopup();
    }

    ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_O, ImGuiInputFlags_Tooltip);
    if (ImGui::Button("Options"))
        ImGui::OpenPopup("Options");
    ImGui::SameLine();
    filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
    ImGui::Separator();

    // Reserve enough left-over height for 1 separator + 1 input text
    ImGuiStyle& style = ImGui::GetStyle();
    const float footer_height_to_reserve = style.SeparatorSize + style.ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_HorizontalScrollbar))
    {
        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::Selectable("Clear"))
                ClearLog();
            ImGui::EndPopup();
        }

        // Display every line as a separate entry so we can change their color or add custom widgets.
        // If you only want raw text you can use ImGui::TextUnformatted(log.begin(), log.end());
        // NB- if you have thousands of entries this approach may be too inefficient and may require user-side clipping
        // to only process visible items. The clipper will automatically measure the height of your first item and then
        // "seek" to display only items in the visible area.
        // To use the clipper we can replace your standard loop:
        //      for (int i = 0; i < Items.Size; i++)
        //   With:
        //      ImGuiListClipper clipper;
        //      clipper.Begin(Items.Size);
        //      while (clipper.Step())
        //         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
        // - That your items are evenly spaced (same height)
        // - That you have cheap random access to your elements (you can access them given their index,
        //   without processing all the ones before)
        // You cannot this code as-is if a filter is active because it breaks the 'cheap random-access' property.
        // We would need random-access on the post-filtered list.
        // A typical application wanting coarse clipping and filtering may want to pre-compute an array of indices
        // or offsets of items that passed the filtering test, recomputing this array when user changes the filter,
        // and appending newly elements as they are inserted. This is left as a task to the user until we can manage
        // to improve this example code!
        // If your items are of variable height:
        // - Split them into same height items would be simpler and facilitate random-seeking into your list.
        // - Consider using manual call to IsRectVisible() and skipping extraneous decoration from your items.
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
        if (copy_to_clipboard)
            ImGui::LogToClipboard();
        for (const char* item : items)
        {
            if (!filter.PassFilter(item))
                continue;

            // Normally you would store more information in your item than just a string.
            // (e.g. make Items[] an array of structure, store color/type etc.)
            ImVec4 color;
            bool has_color = false;
            if (strstr(item, "[error]"))
            {
                color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                has_color = true;
            }
            else if (strncmp(item, "# ", 2) == 0)
            {
                color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f);
                has_color = true;
            }
            if (has_color)
                ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextUnformatted(item);
            if (has_color)
                ImGui::PopStyleColor();
        }
        if (copy_to_clipboard)
            ImGui::LogFinish();

        // Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
        // Using a scrollbar or mouse-wheel will take away from the bottom edge.
        if (scrollToBottom || (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
            ImGui::SetScrollHereY(1.0f);
        scrollToBottom = false;

        ImGui::PopStyleVar();
    }
    ImGui::EndChild();
    ImGui::Separator();

    // Command-line
    bool reclaim_focus = false;
    ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
    if (ImGui::InputText("Input", inputBuffer, IM_COUNTOF(inputBuffer), input_text_flags, &TextEditCallbackStub, (void*)this))
    {
        char* s = inputBuffer;
        Strtrimblanks(s);
        if (s[0])
            ExecCommand(s);
        s[0] = 0;
        reclaim_focus = true;
    }

    // Auto-focus on window apparition
    ImGui::SetItemDefaultFocus();
    if (reclaim_focus)
        ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

    ImGui::End();
}

void ConsoleWidget::AddLog(const char* fmt, ...)
{
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, IM_COUNTOF(buf), fmt, args);
    buf[IM_COUNTOF(buf) - 1] = 0;
    va_end(args);
    items.push_back(Strdup(buf));
}

void ConsoleWidget::ExecCommand(const char* commandLine)
{
    AddLog("# %s\n", commandLine);

    // Insert into history. First find match and delete it so it can be pushed to the back.
    // This isn't trying to be smart or optimal.
    historyPos = -1;
    for (int i = history.Size - 1; i >= 0; i--)
        if (Stricmp(history[i], commandLine) == 0)
        {
            ImGui::MemFree(history[i]);
            history.erase(history.begin() + i);
            break;
        }
    history.push_back(Strdup(commandLine));

    // Process command
    if (Stricmp(commandLine, "CLEAR") == 0)
    {
        ClearLog();
    }
    else if (Stricmp(commandLine, "HELP") == 0)
    {
        AddLog("Commands:");
        for (int i = 0; i < commands.Size; i++)
            AddLog("- %s", commands[i]);
    }
    else if (Stricmp(commandLine, "HISTORY") == 0)
    {
        int first = history.Size - 10;
        for (int i = first > 0 ? first : 0; i < history.Size; i++)
            AddLog("%3d: %s\n", i, history[i]);
    }
    else
    {
        AddLog("Unknown command: '%s'\n", commandLine);
    }

    // On command input, we scroll to bottom even if AutoScroll==false
    scrollToBottom = true;
}

static int TextEditCallbackStub(ImGuiInputTextCallbackData* data)
{
    ConsoleWidget* console = (ConsoleWidget*)data->UserData;
    return console->TextEditCallback(data);
}

int ConsoleWidget::TextEditCallback(ImGuiInputTextCallbackData* data)
{
    // AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
    switch (data->EventFlag)
    {
    case ImGuiInputTextFlags_CallbackCompletion:
    {
        // Example of TEXT COMPLETION

        // Locate beginning of current word
        const char* word_end = data->Buf + data->CursorPos;
        const char* word_start = word_end;
        while (word_start > data->Buf)
        {
            const char c = word_start[-1];
            if (c == ' ' || c == '\t' || c == ',' || c == ';')
                break;
            word_start--;
        }

        // Build a list of candidates
        ImVector<const char*> candidates;
        for (int i = 0; i < commands.Size; i++)
            if (Strnicmp(commands[i], word_start, (int)(word_end - word_start)) == 0)
                candidates.push_back(commands[i]);

        if (candidates.Size == 0)
        {
            // No match
            AddLog("No match for \"%.*s\"!\n", (int)(word_end - word_start), word_start);
        }
        else if (candidates.Size == 1)
        {
            // Single match. Delete the beginning of the word and replace it entirely so we've got nice casing.
            data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
            data->InsertChars(data->CursorPos, candidates[0]);
            data->InsertChars(data->CursorPos, " ");
        }
        else
        {
            // Multiple matches. Complete as much as we can..
            // So inputting "C"+Tab will complete to "CL" then display "CLEAR" and "CLASSIFY" as matches.
            int match_len = (int)(word_end - word_start);
            for (;;)
            {
                int c = 0;
                bool all_candidates_matches = true;
                for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
                    if (i == 0)
                        c = toupper(candidates[i][match_len]);
                    else if (c == 0 || c != toupper(candidates[i][match_len]))
                        all_candidates_matches = false;
                if (!all_candidates_matches)
                    break;
                match_len++;
            }

            if (match_len > 0)
            {
                data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
                data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
            }

            // List matches
            AddLog("Possible matches:\n");
            for (int i = 0; i < candidates.Size; i++)
                AddLog("- %s\n", candidates[i]);
        }

        break;
    }
    case ImGuiInputTextFlags_CallbackHistory:
    {
        // Example of HISTORY
        const int prev_history_pos = historyPos;
        if (data->EventKey == ImGuiKey_UpArrow)
        {
            if (historyPos == -1)
                historyPos = history.Size - 1;
            else if (historyPos > 0)
                historyPos--;
        }
        else if (data->EventKey == ImGuiKey_DownArrow)
        {
            if (historyPos != -1)
                if (++historyPos >= history.Size)
                    historyPos = -1;
        }

        // A better implementation would preserve the data on the current input line along with cursor position.
        if (prev_history_pos != historyPos)
        {
            const char* history_str = (historyPos >= 0) ? history[historyPos] : "";
            data->DeleteChars(0, data->BufTextLen);
            data->InsertChars(0, history_str);
        }
    }
    }
    return 0;
}

} // namespace URay
