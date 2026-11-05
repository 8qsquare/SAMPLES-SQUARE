#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "LibraryInfo.h"
#include "../Branding.h"

namespace SampleSquare {
    namespace Library {
        class LibraryGridComponent : public juce::Component, public juce::MouseListener {
        public:
            using LibraryClickedCallback = std::function<void(const LibraryInfo&)>;

            explicit LibraryGridComponent() {
                addMouseListener(this, true);
            }

            void setLibraries(std::vector<LibraryInfo> libs) {
                libraries = std::move(libs);
                repaint();
            }

            void setOnLibraryClicked(LibraryClickedCallback callback) {
                onLibraryClicked = std::move(callback);
            }

            void resized() override {
                // Cálculo de cuadrícula (4 columnas)
                const int cols = Branding::COLUMNS;
                const int itemWidth = (getWidth() - (cols + 1) * Branding::PADDING) / cols;
                const int itemHeight = itemWidth + Branding::LABEL_HEIGHT;
                
                // Solo para estructura, el paint se encarga del render
            }

            void paint(juce::Graphics& g) override {
                g.fillAll(Branding::BG_DARK);
                
                const int cols = Branding::COLUMNS;
                const int itemWidth = (getWidth() - (cols + 1) * Branding::PADDING) / cols;
                const int itemHeight = Branding::THUMB_SIZE + Branding::LABEL_HEIGHT + Branding::PADDING * 2;

                for (size_t i = 0; i < libraries.size(); ++i) {
                    const auto& lib = libraries[i];
                    const int col = static_cast<int>(i) % cols;
                    const int row = static_cast<int>(i) / cols;
                    
                    const juce::Rectangle<int> cellBounds = getCellBounds(static_cast<int>(i));
                    
                    // Hover highlight
                    if (cellBounds.contains(getMouseXYRelative())) {
                        g.setColour(Branding::BORDER.withAlpha(0.3f));
                        g.fillRoundedRectangle(cellBounds.toFloat(), static_cast<float>(Branding::CORNER_R));
                    }

                    // Imagen escalada centrada
                    juce::Rectangle<int> thumbRect = cellBounds.reduced(Branding::PADDING).removeFromTop(Branding::THUMB_SIZE);
                    g.drawImageTransformed(lib.coverImage, juce::AffineTransform::scaled(
                        static_cast<float>(thumbRect.getWidth()) / lib.coverImage.getWidth(),
                        static_cast<float>(thumbRect.getHeight()) / lib.coverImage.getHeight(),
                        thumbRect.getX() + (thumbRect.getWidth() - lib.coverImage.getWidth() * (static_cast<float>(thumbRect.getWidth()) / lib.coverImage.getWidth())) / 2.0f,
                        thumbRect.getY()
                    ));

                    // Nombre abajo
                    g.setColour(Branding::TEXT_PRIMARY);
                    juce::Font font(juce::FontOptions(14.0f));
                    g.setFont(font);
                    auto labelBounds = cellBounds;
                    const auto labelArea = labelBounds.removeFromBottom(Branding::LABEL_HEIGHT);
                    g.drawFittedText(lib.name, labelArea, juce::Justification::centred, 1);
                }
            }

            void mouseDown(const juce::MouseEvent& e) override {
                for (size_t i = 0; i < libraries.size(); ++i) {
                    if (getCellBounds(static_cast<int>(i)).contains(e.getPosition())) {
                        if (onLibraryClicked) onLibraryClicked(libraries[i]);
                        break;
                    }
                }
            }

            juce::Rectangle<int> getCellBounds(int index) const {
                const int cols = Branding::COLUMNS;
                const int row = index / cols;
                const int col = index % cols;
                
                const int itemWidth = (getWidth() - (cols + 1) * Branding::PADDING) / cols;
                const int itemHeight = Branding::THUMB_SIZE + Branding::LABEL_HEIGHT + Branding::PADDING * 2;
                
                const int x = Branding::PADDING + col * (itemWidth + Branding::PADDING);
                const int y = Branding::PADDING + row * (itemHeight + Branding::PADDING);
                
                return {x, y, itemWidth, itemHeight};
            }

        private:
            std::vector<LibraryInfo> libraries;
            LibraryClickedCallback onLibraryClicked;

            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LibraryGridComponent)
        };
    }
}
