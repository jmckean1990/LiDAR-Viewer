#pragma once

#include <filesystem>
#include <vector>


enum class DatasetFormat {KITTI, NUSCENES};

struct FormatInfo {
    int stride;
    float groundOffset;
    float frameInterval;
};

class Dataset {
public:
    // getters
    size_t frameCount() const {return frames;}
    std::filesystem::path framePath(int frame) const;
    FormatInfo formatInfo() const {return _formatInfo;}
    DatasetFormat datasetFormat() {return _datasetFormat;}

    bool openFolder(std::filesystem::path folderPath, DatasetFormat format);
    bool openFile(std::filesystem::path filePath, DatasetFormat format);

private:
    size_t frames {0};
    DatasetFormat _datasetFormat {DatasetFormat::KITTI};
    FormatInfo _formatInfo {4, 1.73f, 10.0f};   // default KITTI
    std::vector<std::filesystem::path> framePaths;

    void setFormat(DatasetFormat format);
};