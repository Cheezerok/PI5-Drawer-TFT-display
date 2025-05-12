#include "file_dialog.h"
#include <gtkmm/application.h>
#include <iostream>

std::string FileDialog::showOpenDialog(const std::string& title,
                                     const std::string& defaultPath,
                                     const std::vector<std::string>& filters) {
    // Initialize GTK
    int argc = 0;
    char** argv = nullptr;
    auto app = Gtk::Application::create(argc, argv, "org.gtkmm.example");

    // Create dialog
    Gtk::FileChooserDialog dialog(title, Gtk::FILE_CHOOSER_ACTION_OPEN);
    dialog.set_transient_for(*Gtk::Window::get_default());
    
    // Add buttons
    dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
    dialog.add_button("_Open", Gtk::RESPONSE_OK);

    // Set default path if provided
    if (!defaultPath.empty()) {
        dialog.set_current_folder(defaultPath);
    }

    // Add filters
    if (!filters.empty()) {
        auto filter = Gtk::FileFilter::create();
        for (const auto& f : filters) {
            filter->add_pattern(f);
        }
        dialog.add_filter(filter);
    }

    // Show dialog and get result
    std::string result;
    if (dialog.run() == Gtk::RESPONSE_OK) {
        result = dialog.get_filename();
    }

    return result;
} 