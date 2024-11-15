#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <string>
#include "image.h"  // Include the header for your Image class

using namespace ComputerVisionProjects;

// Function to load light source directions from a file
bool loadDirections(const std::string& filename, std::vector<std::vector<double>>& directions) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error loading directions file!" << std::endl;
        return false;
    }
    double x, y, z;
    while (file >> x >> y >> z) {
        directions.push_back({x, y, z});
    }
    return true;
}

// Function to locate the center of the sphere in the image
bool locateSphere(const Image& image, int& centerX, int& centerY, int& radius) {
    centerX = image.num_columns() / 2;
    centerY = image.num_rows() / 2;
    radius = std::min(image.num_columns(), image.num_rows()) / 4;
    return true;
}

// Function to compute light intensities based on the provided images
bool computeLightIntensities(const std::vector<std::string>& imageFiles, 
                             const std::vector<std::vector<double>>& directions, 
                             const Image& sphereImage, 
                             std::vector<std::vector<double>>& intensities) {
    for (size_t i = 0; i < imageFiles.size(); ++i) {
        Image img;
        if (!ReadImage(imageFiles[i], &img)) {
            std::cerr << "Error reading image: " << imageFiles[i] << std::endl;
            return false;
        }

        // Locate the sphere in the current image
        int centerX, centerY, radius;
        if (!locateSphere(img, centerX, centerY, radius)) {
            std::cerr << "Error locating sphere in image: " << imageFiles[i] << std::endl;
            return false;
        }

        // Compute intensity based on direction and pixel values (simplified)
        for (size_t y = 0; y < img.num_rows(); ++y) {
            for (size_t x = 0; x < img.num_columns(); ++x) {
                int pixelValue = img.GetPixel(y, x);

                // Add the pixel intensity contribution to the corresponding light source's direction
                for (size_t d = 0; d < directions.size(); ++d) {
                    intensities[d][y * img.num_columns() + x] += pixelValue * directions[d][0];  // Simplified
                }
            }
        }
    }
    return true;
}

// Function to compute normals and albedo (simplified example)
bool computeNormalsAndAlbedo(const std::vector<std::vector<double>>& intensities, 
                             const std::vector<std::vector<double>>& directions, 
                             Image& normalsImage, Image& albedoImage) {
    // Now we compute the normals based on the intensities and directions
    for (int y = 0; y < normalsImage.num_rows(); ++y) {
        for (int x = 0; x < normalsImage.num_columns(); ++x) {
            std::vector<double> normal(3, 0.0);
            double totalIntensity = 0.0;

            // Summing intensities for each light source to estimate the normal at this pixel
            for (size_t d = 0; d < directions.size(); ++d) {
                int idx = y * normalsImage.num_columns() + x;  // Flatten the 2D coordinate to 1D index
                double intensity = intensities[d][idx];
                totalIntensity += intensity;

                // Update normal direction based on the light source's direction
                for (int i = 0; i < 3; ++i) {
                    normal[i] += intensity * directions[d][i];  // Direction of the light source multiplied by intensity
                }
            }

            // Normalize the normal vector
            double length = std::sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
            if (length > 0.0) {
                for (int i = 0; i < 3; ++i) {
                    normal[i] /= length;
                }
            }

            // Set the normal and albedo (simplified as intensity for now)
            normalsImage.SetPixel(y, x, static_cast<int>(normal[0] * 255));  // Use one component as normal for simplicity
            albedoImage.SetPixel(y, x, static_cast<int>(totalIntensity * 255));  // Use intensity as albedo for simplicity
        }
    }
    return true;
}

int main(int argc, char** argv) {
    // Ensure correct usage of the program with required arguments
    if (argc < 9) {
        std::cerr << "Usage: s3 {directions file} {image 1} {image 2} {image 3}... {step} {threshold} {normals image} {albedo image}" << std::endl;
        return 1;
    }

    // Read light source directions from the file
    std::vector<std::vector<double>> directions;
    if (!loadDirections(argv[1], directions)) {
        std::cerr << "Failed to load directions!" << std::endl;
        return 1;
    }

    // Store image file paths
    std::vector<std::string> imageFiles;
    for (int i = 2; i < argc - 4; ++i) {
        imageFiles.push_back(argv[i]);
    }

    // Read the first image to initialize image size
    Image firstImage;
    if (!ReadImage(imageFiles[0], &firstImage)) {
        std::cerr << "Failed to read first image!" << std::endl;
        return 1;
    }

    // Initialize the intensities array (one for each light source, with an intensity per pixel)
    std::vector<std::vector<double>> intensities(directions.size(), std::vector<double>(firstImage.num_rows() * firstImage.num_columns(), 0.0));

    // Process the images and compute intensities
    if (!computeLightIntensities(imageFiles, directions, firstImage, intensities)) {
        std::cerr << "Failed to compute light intensities!" << std::endl;
        return 1;
    }

    // Prepare output images for normals and albedo
    Image normalsImage, albedoImage;

    // Use the size of the first image to determine the dimensions of output images
    if (!ReadImage(imageFiles[0], &normalsImage)) {
        std::cerr << "Error reading image for normals!" << std::endl;
        return 1;
    }
    if (!ReadImage(imageFiles[0], &albedoImage)) {
        std::cerr << "Error reading image for albedo!" << std::endl;
        return 1;
    }

    // Compute normals and albedo
    if (!computeNormalsAndAlbedo(intensities, directions, normalsImage, albedoImage)) {
        std::cerr << "Failed to compute normals and albedo!" << std::endl;
        return 1;
    }

    // Save the output images
    if (!WriteImage(argv[argc - 2], normalsImage)) {
        std::cerr << "Error writing normals image!" << std::endl;
        return 1;
    }
    if (!WriteImage(argv[argc - 1], albedoImage)) {
        std::cerr << "Error writing albedo image!" << std::endl;
        return 1;
    }

    std::cout << "Normals and albedo images successfully written!" << std::endl;

    return 0;
}
