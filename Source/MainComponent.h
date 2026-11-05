#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include "Branding.h"
#include "Library/LibraryManager.h"
#include "Library/LibraryBrowserComponent.h"
#include "Synth/CustomSamplerProcessor.h"
#include "UI/PlayerPanelComponent.h"
#include "UI/SampleSquareLookAndFeel.h"
#include <cmath>

namespace SampleSquare {
    class MainComponent : public juce::Component,
                          public juce::FileDragAndDropTarget {
    public:
        explicit MainComponent()
            : audioDeviceManager(),
              processor(),
              audioPlayer(),
              midiKeyboardComponent(processor.midiKeyboardState, juce::MidiKeyboardComponent::horizontalKeyboard),
              libraryManager(),
              browser(libraryManager) {
            audioPlayer.setProcessor(&processor);
            
            setLookAndFeel(&lookAndFeel);
            
            addAndMakeVisible(browser);
            addAndMakeVisible(*processor.playerPanel);
            addAndMakeVisible(midiKeyboardComponent);

            browser.setOnLibrarySelected([this](const Library::LibraryInfo& info) {
                processor.loadLibraryAsync(info.presetFile, [this]() {
                    // Callback de carga completada
                });
            });

            browser.refresh();

            audioDeviceManager.initialiseWithDefaultDevices(0, 2);
            audioDeviceManager.addAudioCallback(&audioPlayer);
            audioDeviceManager.addMidiInputCallback(juce::String::empty, &audioPlayer);
            
            setOpaque(true);
        }

        ~MainComponent() override {
            audioDeviceManager.removeMidiInputCallback(juce::String::empty, &audioPlayer);
            audioDeviceManager.removeAudioCallback(&audioPlayer);
            audioPlayer.setProcessor(nullptr);
            setLookAndFeel(nullptr);
        }

        void resized() override {
            const int panelHeight = 140;
            const int keyboardHeight = 80;
            
            processor.playerPanel->setBounds(0, 0, getWidth(), panelHeight);
            browser.setBounds(0, panelHeight, getWidth(), getHeight() - panelHeight - keyboardHeight);
            midiKeyboardComponent.setBounds(0, getHeight() - keyboardHeight, getWidth(), keyboardHeight);
        }

        void paint(juce::Graphics& g) override {
            g.fillAll(Branding::BG_DARK);
            
            // Logo de onda senoidal en el header (dibujado a mano)
            juce::Path sinePath;
            sinePath.startNewSubPath(0, getHeight() / 20.0f);
            for (float x = 0; x < static_cast<float>(getWidth()); x += 5.0f) {
                const float y = getHeight() / 20.0f + std::sin(x * 0.05f) * 10.0f;
                sinePath.lineTo(x, y);
            }
            g.setColour(Branding::ACCENT);
            g.strokePath(sinePath, juce::PathStrokeType(2.0f));
            
            // Texto App Name
            juce::Font font(juce::FontOptions(18.0f).withStyle("Bold"));
            g.setFont(font);
            g.setColour(Branding::TEXT_PRIMARY);
            g.drawText(Branding::APP_NAME + " " + Branding::APP_VERSION, 20, 5, 200, 30, juce::Justification::left);
        }

        // FileDragAndDropTarget
        bool isInterestedInFileDrag(const juce::StringArray& files) override {
            return true;
        }

        void filesDropped(const juce::StringArray& files, int x, int y) override {
            for (const auto& fileStr : files) {
                juce::File file(fileStr);
                if (file.isDirectory()) {
                    libraryManager.scanRootFolder(file, [this](std::vector<Library::LibraryInfo>) {
                        browser.refresh();
                    });
                    break;
                }
            }
        }

    private:
        juce::AudioDeviceManager audioDeviceManager;
        SampleSquare::Synth::CustomSamplerProcessor processor;
        juce::AudioProcessorPlayer audioPlayer;
        juce::MidiKeyboardComponent midiKeyboardComponent;
        Library::LibraryManager libraryManager;
        Library::LibraryBrowserComponent browser;
        SampleSquare::UI::SampleSquareLookAndFeel lookAndFeel;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
    };
}
