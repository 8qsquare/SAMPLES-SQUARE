#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "LibraryManager.h"
#include "LibraryGridComponent.h"

namespace SampleSquare {
    namespace Library {
        class LibraryBrowserComponent : public juce::Component, public juce::Button::Listener {
        public:
            using LibrarySelectedCallback = std::function<void(const LibraryInfo&)>;

            explicit LibraryBrowserComponent(LibraryManager& manager)
                : manager(manager), grid(std::make_unique<LibraryGridComponent>()), selectButton(std::make_unique<juce::TextButton>("Seleccionar carpeta raíz...")) {
                
                addAndMakeVisible(*grid);
                addAndMakeVisible(*selectButton);
                selectButton->addListener(this);
                
                grid->setOnLibraryClicked([this](const LibraryInfo& info) {
                    if (onLibrarySelected) onLibrarySelected(info);
                });
            }

            void resized() override {
                const auto buttonHeight = 32;
                selectButton->setBounds(Branding::PADDING, Branding::PADDING, getWidth() - Branding::PADDING * 2, buttonHeight);
                grid->setBounds(0, buttonHeight + Branding::PADDING * 2, getWidth(), getHeight() - buttonHeight - Branding::PADDING * 3);
            }

            void refresh() {
                grid->setLibraries(manager.getLibraries());
                // Actualizar label de estado si es necesario
            }

            void buttonClicked(juce::Button* button) override {
                if (button == selectButton.get()) {
                    chooser = std::make_unique<juce::FileChooser>(
                        "Seleccionar carpeta raíz de librerías", manager.getLastRootFolder(), "");
                    chooser->launchAsync(juce::FileBrowserComponent::openMode
                                             | juce::FileBrowserComponent::canSelectDirectories,
                                         [this](const juce::FileChooser& fileChooser) {
                                             const auto selectedFolder = fileChooser.getResult();
                                             if (selectedFolder.isDirectory())
                                                 manager.scanRootFolder(selectedFolder, [this](std::vector<LibraryInfo>) {
                                                     refresh();
                                                 });
                                             chooser.reset();
                                         });
                }
            }

            void setOnLibrarySelected(LibrarySelectedCallback callback) {
                onLibrarySelected = std::move(callback);
            }

        private:
            LibraryManager& manager;
            std::unique_ptr<LibraryGridComponent> grid;
            std::unique_ptr<juce::TextButton> selectButton;
            std::unique_ptr<juce::FileChooser> chooser;
            LibrarySelectedCallback onLibrarySelected;

            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LibraryBrowserComponent)
        };
    }
}
