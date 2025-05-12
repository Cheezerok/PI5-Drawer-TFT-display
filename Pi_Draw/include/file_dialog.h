#ifndef FILE_DIALOG_H
#define FILE_DIALOG_H

#include <string>
#include <gtkmm/dialog.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/window.h>

class FileDialog {
public:
    static std::string showOpenDialog(const std::string& title, 
                                    const std::string& defaultPath = "",
                                    const std::vector<std::string>& filters = {});
};

#endif // FILE_DIALOG_H 