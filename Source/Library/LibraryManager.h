#pragma once
#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_events/juce_events.h>
#include <vector>
#include "LibraryInfo.h"
#include "ThumbnailCache.h"

namespace SampleSquare {
    namespace Library {
        class LibraryManager {
        public:
            explicit LibraryManager()
                : threadPool(4), propertiesFile([] {
                    juce::PropertiesFile::Options options;
                    options.applicationName = "SampleSquare";
                    options.filenameSuffix = "properties";
                    options.folderName = "SampleSquare";
                    options.osxLibrarySubFolder = "Application Support";
                    return options;
                }()) {
                lastRootFolder = juce::File(propertiesFile.getValue("lastRootFolder"));
            }

            ~LibraryManager() {
                threadPool.removeAllJobs(true, 5000);
            }

            void scanRootFolder(const juce::File& folder, std::function<void(std::vector<LibraryInfo>)> onFinished) {
                lastRootFolder = folder;
                propertiesFile.setValue("lastRootFolder", folder.getFullPathName());
                propertiesFile.save();

                threadPool.addJob(new ScanJob(folder, [this, onFinished](std::vector<LibraryInfo> infos) {
                    libraries = std::move(infos);
                    juce::MessageManager::callAsync([onFinished, this]() {
                        onFinished(libraries);
                    });
                }), true);
            }

            const std::vector<LibraryInfo>& getLibraries() const { return libraries; }
            juce::File getLastRootFolder() const { return lastRootFolder; }

        private:
            class ScanJob : public juce::ThreadPoolJob {
            public:
                ScanJob(const juce::File& folder, std::function<void(std::vector<LibraryInfo>)> callback)
                    : juce::ThreadPoolJob("ScanJob"), rootFolder(folder), resultCallback(std::move(callback)) {}

                JobStatus runJob() override {
                    std::vector<LibraryInfo> found;
                    for (const auto& entry : juce::RangedDirectoryIterator(rootFolder, true, "*.dspreset")) {
                        const auto& file = entry.getFile();
                        LibraryInfo info;
                        info.name = file.getParentDirectory().getFileName();
                        info.folder = file.getParentDirectory();
                        info.presetFile = file;
                        info.coverImage = ThumbnailCache::getInstance().getThumbnail(file);
                        found.push_back(std::move(info));
                    }

                    resultCallback(std::move(found));
                    return JobStatus::jobHasFinished;
                }

            private:
                juce::File rootFolder;
                std::function<void(std::vector<LibraryInfo>)> resultCallback;
            };

            juce::ThreadPool threadPool;
            std::vector<LibraryInfo> libraries;
            juce::File lastRootFolder;
            juce::PropertiesFile propertiesFile;

            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LibraryManager)
        };
    }
}
