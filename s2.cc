#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <string>
#include <limits>

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

// Function to compute the direction vector from the normal
void computeDirectionVector(int centerX, int centerY, double radius, int pixelX, int pixelY, double &dx, double &dy, double &dz) {
    // Compute the normal vector components in 3D space (using the formula)
    double dx_norm = (pixelX - centerX) / radius;
    double dy_norm = (pixelY - centerY) / radius;
    
    // Calculate dz based on the sphere equation: r^2 = (x-cx)^2 + (y-cy)^2 + (z-cz)^2
    double dz_norm = std::sqrt(1.0 - dx_norm * dx_norm - dy_norm * dy_norm);  // This comes from the sphere equation

    dx = dx_norm;
    dy = dy_norm;
    dz = dz_norm;
}

// Function to find the brightest pixel and its coordinates
void findBrightestPixel(const std::vector<std::vector<int>> &image, int &brightestX, int &brightestY, int &brightness) {
    int maxBrightness = 0;
    brightestX = -1;
    brightestY = -1;

    for (int i = 0; i < image.size(); ++i) {
        for (int j = 0; j < image[0].size(); ++j) {
            if (image[i][j] > maxBrightness) {
                maxBrightness = image[i][j];
                brightestX = j;
                brightestY = i;
            }
        }
    }
    brightness = maxBrightness;
}

}  // namespace ComputerVision

int main(int argc, char *argv[]) {
    if (argc != 6) {
        std::cerr << "Usage: " << argv[0] << " <input parameters file> <sphere image 1> <sphere image 2> <sphere image 3> <output directions file>" << std::endl;
        return 1;
    }

    // Read the parameters file
    std::ifstream paramFile(argv[1]);
    if (!paramFile) {
        std::cerr << "Error: Could not open parameters file " << argv[1] << std::endl;
        return 1;
    }

    int centerX, centerY;
    double radius;
    paramFile >> centerX >> centerY >> radius;
    paramFile.close();

    // Prepare the images
    std::vector<std::vector<int>> image1, image2, image3;
    if (!ComputerVision::readPGM(argv[2], image1) || !ComputerVision::readPGM(argv[3], image2) || !ComputerVision::readPGM(argv[4], image3)) {
        std::cerr << "Error: Could not read one of the sphere images." << std::endl;
        return 1;
    }

    // Output file for the light directions
    std::ofstream outFile(argv[5]);
    if (!outFile) {
        std::cerr << "Error: Could not open output file " << argv[5] << std::endl;
        return 1;
    }

    // Process each image
    for (int i = 0; i < 3; ++i) {
        std::vector<std::vector<int>> *image;
        if (i == 0) image = &image1;
        else if (i == 1) image = &image2;
        else image = &image3;

        // Find the brightest pixel in the image
        int brightestX, brightestY, brightness;
        ComputerVision::findBrightestPixel(*image, brightestX, brightestY, brightness);

        // Compute the direction vector at the brightest pixel
        double dx, dy, dz;
        ComputerVision::computeDirectionVector(centerX, centerY, radius, brightestX, brightestY, dx, dy, dz);

        // Scale the direction vector by the brightness
        double scale = static_cast<double>(brightness);
        dx *= scale;
        dy *= scale;
        dz *= scale;

        // Write the direction to the output file
        outFile << dx << " " << dy << " " << dz << std::endl;
    }

    std::cout << "Light directions written to " << argv[5] << std::endl;
    return 0;
}
