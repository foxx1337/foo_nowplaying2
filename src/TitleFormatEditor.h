#pragma once

#include <footilla/editor.h>

#include <string>
#include <string_view>

// Replaces a dialog-template EDIT without changing its bounds or tab order.
class TitleFormatEditor
{
public:
    ~TitleFormatEditor();
    bool Create(HINSTANCE module, HWND dialog, int id, std::string_view text, UINT theme_message,
                bool dark, bool read_only);
    void Destroy();
    // Called on component shutdown, never on preferences close. Live editors defer cleanup.
    static bool ShutdownRuntime();
    HWND Handle() const { return editor_.Handle(); }
    HWND InputHandle() const { return editor_.ScintillaHandle(); }
    const std::string& Error() const { return error_; }
    void SetText(std::string_view text);
    std::string GetText() const { return editor_.GetText(); }
    void SetReadOnly(bool value) { editor_.SetReadOnly(value); }

private:
    static LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
    footilla::Editor editor_;
    bool acquired_ = false;
    UINT theme_message_ = 0;
    std::string error_;
};
