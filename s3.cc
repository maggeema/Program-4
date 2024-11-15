#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "image.h"

namespace ComputerVision {

    // Compute the surface normal at a specific pixel
    std::vector<float> ComputeNormal(const std::vector<std::vector<float>>& L, const ComputerVisionProjects::Image& I, int i, int j) {
        std::vector<float> normal(3, 0.0f);

        // Assuming GetPixel returns the intensity of the pixel at (i, j)
        int intensity = I.GetPixel(i, j);

        // Compute normal using the light directions and intensity
        for (size_t k = 0; k < L.size(); ++k) {
            normal[0] += L[k][0] * intensity;
            normal[1] += L[k][1] * intensity;
            normal[2] += L[k][2] * intensity;
        }

        return normal;
    }

    // Compute the albedo at a specific pixel
    float ComputeAlbedo(const ComputerVisionProjects::Image& I, const std::vector<float>& normal, int i, int j) {
        // Assuming GetPixel returns intensity
        int intensity = I.GetPixel(i, j);

        // Albedo computation
        float albedo = intensity / (normal[0] + normal[1] + normal[2] + 1.0f);
        return albedo;
    }

    // Compute the surface normals and albedo for an object
    void ComputeSurfaceNormals(const ComputerVisionProjects::Image& I1, const ComputerVisionProjects::Image& I2, const ComputerVisionProjects::Image& I3, 
                               const std::vector<std::vector<float>>& L, int light_step, 
                               int threshold, ComputerVisionProjects::Image& normals, ComputerVisionProjects::Image& albedo) {
        size_t num_rows = I1.num_rows();
        size_t num_columns = I1.num_columns();

        // Allocate space for normals and albedo images
        normals.AllocateSpaceAndSetSize(num_rows, num_columns);
        albedo.AllocateSpaceAndSetSize(num_rows, num_columns);

        for (int i = 0; i < num_rows; ++i) {
            for (int j = 0; j < num_columns; ++j) {
                // Check if the pixel is visible from all three light sources (intensity > threshold)
                if (I1.GetPixel(i, j) > threshold && I2.GetPixel(i, j) > threshold && I3.GetPixel(i, j) > threshold) {
                    // Compute normal and albedo for each pixel
                    std::vector<float> normal = ComputeNormal(L, I1, i, j);
                    float alb = ComputeAlbedo(I1, normal, i, j);

                    // Set the normal and albedo values for each pixel
                    normals.SetPixel(i, j, static_cast<int>((normal[0] + 1.0f) * 127)); // Normalize normal to 0-255 range
                    albedo.SetPixel(i, j, static_cast<int>(alb * 255));  // Scale albedo to range [0, 255]
                } else {
                    // Set black for non-visible pixels
                    normals.SetPixel(i, j, 0);
                    albedo.SetPixel(i, j, 0);
                }
            }
        }
    }

    // Load light directions from a file
    std::vector<std::vector<float>> LoadLightDirections(const std::string& filename) {
        std::vector<std::vector<float>> L;
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Could not open light directions file." << std::endl;
            return L;
        }

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream stream(line);
            std::vector<float> direction(3);
            stream >> direction[0] >> direction[1] >> direction[2];
            L.push_back(direction);
        }

        file.close();
        return L;
    }

}  // namespace ComputerVision

int main(int argc, char* argv[]) {
    if (argc != 9) {
        std::cerr << "Usage: " << argv[0] 
                  << " output_directions.txt object1.pgm object2.pgm object3.pgm "
                  << "light_step threshold output_normals.pgm output_albedo.pgm" 
                  << std::endl;
        return 1;
    }

    // Parse arguments
    std::string directions_file = argv[1];
    std::string object1_file = argv[2];
    std::string object2_file = argv[3];
    std::string object3_file = argv[4];
    int step = std::stoi(argv[5]);
    int threshold = std::stoi(argv[6]);
    std::string output_normals_file = argv[7];
    std::string output_albedo_file = argv[8];

    // Load light directions from file
    std::vector<std::vector<float>> L = ComputerVision::LoadLightDirections(directions_file);

    // Read input images
    ComputerVisionProjects::Image object1, object2, object3;
    if (!ComputerVisionProjects::ReadImage(object1_file, &object1) ||
        !ComputerVisionProjects::ReadImage(object2_file, &object2) ||
        !ComputerVisionProjects::ReadImage(object3_file, &object3)) {
        std::cerr << "Failed to read one or more image files." << std::endl;
        return 1;
    }

    // Initialize output images as Image objects, not int
    ComputerVisionProjects::Image normals, albedo;

    // Make sure we correctly pass Image references to ComputeSurfaceNormals
    ComputerVision::ComputeSurfaceNormals(object1, object2, object3, L, step, threshold, normals, albedo);

    // Write output images
    if (!ComputerVisionProjects::WriteImage(output_normals_file, normals) ||
        !ComputerVisionProjects::WriteImage(output_albedo_file, albedo)) {
        std::cerr << "Failed to write output image files." << std::endl;
        return 1;
    }

    std::cout << "Surface normals and albedo have been computed and saved." << std::endl;

    return 0;
}
