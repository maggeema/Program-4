#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <string>

namespace ComputerVision {

// Function to read PGM file (P5 format)
bool readPGM(const std::string &filename, std::vector<std::vector<int>> &image) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return false;
    }

    std::string line;
    std::getline(file, line);  // Read the "P5"
    if (line != "P5") {
        std::cerr << "Error: Not a valid PGM file (expected P5)." << std::endl;
        return false;
    }

    // Skip over any comments (lines starting with '#')
    while (std::getline(file, line)) {
        if (line[0] != '#') break;
    }

    // Read the image dimensions
    int width, height;
    std::istringstream(line) >> width >> height;
    if (width <= 0 || height <= 0) {
        std::cerr << "Error: Invalid image dimensions." << std::endl;
        return false;
    }

    // Skip the "255" line (max pixel value)
    std::getline(file, line);
    if (line != "255") {
        std::cerr << "Error: Expected max pixel value to be 255." << std::endl;
        return false;
    }

    // Now read the pixel data
    image.resize(height, std::vector<int>(width));
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            image[i][j] = file.get();  // Read one byte (pixel value)
        }
    }

    return true;
}

// Function to threshold the image and create a binary image
void thresholdImage(const std::vector<std::vector<int>> &image, std::vector<std::vector<int>> &binaryImage, int threshold) {
    int height = image.size();
    int width = image[0].size();
    binaryImage.resize(height, std::vector<int>(width));

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            binaryImage[i][j] = (image[i][j] >= threshold) ? 255 : 0;
        }
    }
}

// Function to compute the centroid of a binary image (assumes a circular shape)
void computeCentroid(const std::vector<std::vector<int>> &binaryImage, int &centerX, int &centerY) {
    int height = binaryImage.size();
    int width = binaryImage[0].size();

    int totalX = 0, totalY = 0, count = 0;

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (binaryImage[i][j] == 255) {
                totalX += j;
                totalY += i;
                count++;
            }
        }
    }

    if (count == 0) {
        centerX = centerY = -1;  // No pixels found
    } else {
        centerX = totalX / count;
        centerY = totalY / count;
    }
}

// Function to compute the radius of a binary circle
double computeRadius(const std::vector<std::vector<int>> &binaryImage, int centerX, int centerY) {
    int height = binaryImage.size();
    int width = binaryImage[0].size();

    int left = width, right = 0, top = height, bottom = 0;

    // Loop to find boundary points
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (binaryImage[i][j] == 255) {
                if (j < left) left = j;           // Leftmost point
                if (j > right) right = j;         // Rightmost point
                if (i < top) top = i;             // Topmost point
                if (i > bottom) bottom = i;       // Bottommost point
            }
        }
    }

    // Compute horizontal and vertical diameters
    double horizontalDiameter = right - left;
    double verticalDiameter = bottom - top;

    // Average the diameters and divide by 2 to get the radius
    double radius = (horizontalDiameter + verticalDiameter) / 4.0;

    return radius;
}

// Function to write the sphere parameters to a file
void writeParameters(const std::string &filename, int centerX, int centerY, double radius) {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Error: Could not open output file " << filename << std::endl;
        return;
    }
    file << centerX << " " << centerY << " " << radius << std::endl;
}

}  // namespace ComputerVision

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <input gray-level sphere image> <threshold value> <output parameters file>" << std::endl;
        return 1;
    }

    std::string inputImage = argv[1];
    int threshold = std::stoi(argv[2]);
    std::string outputFile = argv[3];

    // Read the PGM file
    std::vector<std::vector<int>> image;
    if (!ComputerVision::readPGM(inputImage, image)) {
        std::cerr << "Error: Unable to read the PGM file." << std::endl;
        return 1;
    }

    // Debug: Print image dimensions
    std::cout << "Image Loaded. Size: " << image.size() << " x " << image[0].size() << std::endl;

    // Threshold the image to create a binary image
    std::vector<std::vector<int>> binaryImage;
    ComputerVision::thresholdImage(image, binaryImage, threshold);

    // Compute the centroid of the binary image
    int centerX, centerY;
    ComputerVision::computeCentroid(binaryImage, centerX, centerY);
    if (centerX == -1 || centerY == -1) {
        std::cerr << "Error: No circle detected in the binary image." << std::endl;
        return 1;
    }

    // Compute the radius of the detected circle
    double radius = ComputerVision::computeRadius(binaryImage, centerX, centerY);

    // Write the parameters to the output file
    ComputerVision::writeParameters(outputFile, centerX, centerY, radius);

    std::cout << "Sphere center: (" << centerX << ", " << centerY << "), Radius: " << radius << std::endl;

    return 0;
}
