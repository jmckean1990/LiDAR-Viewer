#include "Dataset.hpp"

#include <filesystem>
#include <iostream>
#include <algorithm>

std::filesystem::path Dataset::framePath(int frame) const {
    if(frames == 0) {
        std::cout << "File or folder not opened. Cannot return current path\n";
        return {};
    }

    if(frame < 0 || frame > frames) {
        std::cout << "Frame index out of range. Cannot return current path\n";
        return {};
    }

    return framePaths[frame];
}

bool Dataset::openFolder(std::filesystem::path folderPath, DatasetFormat format) {
    framePaths.clear();
    frames = 0;
    
    for(auto const& dir_entry : std::filesystem::directory_iterator(folderPath)) {
        if(dir_entry.path().extension() != ".bin") continue;
        
        framePaths.push_back(dir_entry.path());
    }

    if(framePaths.empty()) {
        std::cout << "No files found in the folder.\n";
        return false;
    }

    std::sort(framePaths.begin(), framePaths.end());

    frames = framePaths.size() - 1;
    setFormat(format);

    return true;
}

bool Dataset::openFile(std::filesystem::path filePath, DatasetFormat format) {
    framePaths.clear();
    frames = 0;
    
    if(filePath.extension() != ".bin") {
        std::cout << "Incorrect file extension. Extension: " << filePath.extension() << "\n";
        return false;
    }

    framePaths.push_back(filePath);
    frames = 1;
    setFormat(format);

    return true;
}

void Dataset::setFormat(DatasetFormat format) {
    switch(format) {
        case DatasetFormat::KITTI:
            _datasetFormat = DatasetFormat::KITTI;
            _formatInfo.stride = 4;
            _formatInfo.groundOffset = 1.73f;
            _formatInfo.frameInterval = 10.0f;
            break;
        case DatasetFormat::NUSCENES:
            _datasetFormat = DatasetFormat::NUSCENES;
            _formatInfo.stride = 5;
            _formatInfo.groundOffset = 1.84f;
            _formatInfo.frameInterval = 20.0f;
    }
}





