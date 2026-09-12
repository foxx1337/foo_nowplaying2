#include "TitleFormatEditor.h"
#include <uxtheme.h>

#include <stdexcept>
#include <system_error>

namespace
{
    unsigned int runtime_users = 0;

    void Check(BOOL result, const char* operation)
    {
        if (!result)
        {
            throw std::system_error(static_cast<int>(GetLastError()), std::system_category(), operation);
        }
    }
}

TitleFormatEditor::~TitleFormatEditor() { Destroy(); }

bool TitleFormatEditor::Create(HINSTANCE module, HWND dialog, int id, std::string_view text, UINT theme_message,
                               bool dark, bool read_only)
{
    try
    {
        if (acquired_)
        {
            throw std::logic_error("Title formatting editor already created");
        }
        if (runtime_users == 0)
        {
            Check(footilla::Initialize(module), "Initialize Footilla");
        }
        ++runtime_users;
        acquired_ = true;

        const HWND placeholder = GetDlgItem(dialog, id);
        if (!placeholder)
        {
            throw std::runtime_error("Title formatting editor placeholder not found");
        }
        RECT bounds{};
        Check(GetWindowRect(placeholder, &bounds), "Get format editor bounds");
        MapWindowPoints(nullptr, dialog, reinterpret_cast<POINT*>(&bounds), 2);

        footilla::Options options;
        options.theme = dark ? footilla::Theme::Dark : footilla::Theme::Light;
        options.readOnly = read_only;
        options.fontSizePoints = 10;
        Check(editor_.Create(dialog, id, bounds, options), "Create Footilla control");
        theme_message_ = theme_message;
        Check(SetWindowSubclass(editor_.Handle(), WindowProc, 1, reinterpret_cast<DWORD_PTR>(this)),
              "Install editor theme handler");
        editor_.SetExtraFields({"datetime"});
        editor_.SetText(text);
        Check(SetWindowPos(editor_.Handle(), placeholder, 0, 0, 0, 0,
                           SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE), "Preserve editor tab order");
        Check(DestroyWindow(placeholder), "Remove format editor placeholder");
        return true;
    }
    catch (const std::exception& error)
    {
        error_ = error.what();
        Destroy();
        return false;
    }
}

void TitleFormatEditor::Destroy()
{
    editor_.Destroy();
    if (acquired_)
    {
        acquired_ = false;
        if (--runtime_users == 0 && !footilla::Shutdown())
        {
            MessageBoxW(nullptr, L"Could not release Footilla editor resources.", L"Now Playing 2", MB_ICONERROR);
        }
    }
}

void TitleFormatEditor::SetText(std::string_view text)
{
    // Avoid clearing undo/caret/scroll when an inherited format hasn't changed.
    if (editor_.GetText() != text)
    {
        editor_.SetText(text);
    }
}

LRESULT CALLBACK TitleFormatEditor::WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam,
                                              UINT_PTR id, DWORD_PTR data)
{
    auto* self = reinterpret_cast<TitleFormatEditor*>(data);
    if (message == WM_NCDESTROY)
    {
        RemoveWindowSubclass(window, WindowProc, id);
        return DefSubclassProc(window, message, wParam, lParam);
    }
    if (message == self->theme_message_)
    {
        if (wParam != static_cast<WPARAM>(-1))
        {
            try
            {
                self->editor_.SetTheme(wParam ? footilla::Theme::Dark : footilla::Theme::Light);
                const HRESULT result = SetWindowTheme(self->editor_.ScintillaHandle(),
                                                       wParam ? L"DarkMode_Explorer" : L"Explorer", nullptr);
                if (FAILED(result))
                {
                    throw std::system_error(static_cast<int>(result), std::system_category(), "Set editor scrollbar theme");
                }
            }
            catch (const std::exception& error)
            {
                MessageBoxA(window, error.what(), "Now Playing 2", MB_ICONERROR);
            }
        }
        return 1;
    }
    return DefSubclassProc(window, message, wParam, lParam);
}
