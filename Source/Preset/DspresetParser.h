#pragma once
#include <juce_core/juce_core.h>
#include <vector>

namespace SampleSquare {
    namespace Preset {
        struct Zone {
            juce::File path;
            int rootNote = 60;
            int loNote = 0;
            int hiNote = 127;
        };

        struct ParsedPreset {
            juce::String name;
            juce::File coverArtFile;
            std::vector<Zone> zones;
        };

        class DspresetParser {
        public:
            static ParsedPreset parse(const juce::File& file) {
                ParsedPreset result;
                result.name = file.getFileNameWithoutExtension();

                juce::XmlDocument doc(file);
                if (auto* root = doc.getDocumentElement()) {
                    if (root->hasTagName("DecentSampler")) {
                        // UI Cover Art
                        if (auto* uiNode = root->getChildByName("ui")) {
                            if (auto* coverAttr = uiNode->getAttributePointer("coverArt")) {
                                result.coverArtFile = file.getParentDirectory().getChildFile(coverAttr->getValue());
                            }
                        }

                        // Groups
                        if (auto* groupsNode = root->getChildByName("groups")) {
                            for (auto* groupNode : groupsNode->getChildIterator()) {
                                if (groupNode->hasTagName("group")) {
                                    for (auto* sampleNode : groupNode->getChildIterator()) {
                                        if (sampleNode->hasTagName("sample")) {
                                            Zone zone;
                                            if (auto* pathAttr = sampleNode->getAttributePointer("path")) {
                                                juce::String pathStr = pathAttr->getValue();
                                                pathStr = pathStr.replaceCharacter('\\', '/');
                                                zone.path = file.getParentDirectory().getChildFile(pathStr);
                                            }
                                            if (auto* rootAttr = sampleNode->getAttributePointer("rootNote")) {
                                                zone.rootNote = rootAttr->getIntValue();
                                            }
                                            if (auto* loAttr = sampleNode->getAttributePointer("loNote")) {
                                                zone.loNote = loAttr->getIntValue();
                                            }
                                            if (auto* hiAttr = sampleNode->getAttributePointer("hiNote")) {
                                                zone.hiNote = hiAttr->getIntValue();
                                            }
                                            result.zones.push_back(zone);
                                        }
                                    }
                                }
                            }
                        }
                    }
                    delete root;
                }
                return result;
            }
        };
    }
}
