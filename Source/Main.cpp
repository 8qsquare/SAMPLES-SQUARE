#include <juce_gui_basics/juce_gui_basics.h>
#include "MainComponent.h"
#include "Branding.h"

namespace SampleSquare {
    class SampleSquareApplication : public juce::JUCEApplication {
    public:
        const juce::String getApplicationName() override { return Branding::APP_NAME; }
        const juce::String getApplicationVersion() override { return Branding::APP_VERSION; }
        bool moreThanOneInstanceAllowed() override { return true; }

        void initialise(const juce::String& commandLine) override {
            mainWindow = std::make_unique<juce::DocumentWindow>(
                Branding::APP_NAME,
                Branding::BG_DARK,
                juce::DocumentWindow::allButtons
            );
            
            auto* mainComponent = new MainComponent();
            mainWindow->setContentOwned(mainComponent, true);
            
            mainWindow->setResizable(true, true);
            mainWindow->setSize(1024, 768);
            mainWindow->setVisible(true);
        }

        void shutdown() override {
            mainWindow = nullptr;
        }

        void systemRequestedQuit() override {
            quit();
        }

        void anotherInstanceStarted(const juce::String& commandLine) override {}

    private:
        std::unique_ptr<juce::DocumentWindow> mainWindow;
    };

    START_JUCE_APPLICATION(SampleSquareApplication)
}
