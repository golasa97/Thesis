#include "fitAPI/URRModel.h"
#include "fitAPI/GradientDescentFit.h"
#include "fitAPI/impl/OneDimVec.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <mpi.h>

// Helper function to read the mean hyper-vector from a file
std::vector<double> readMeanHype(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    std::string header;
    std::getline(file, header); // Skip header

    std::vector<double> hype_vec;
    double value;
    while (file >> value) {
        hype_vec.push_back(value);
    }
    file.close();
    return hype_vec;
}

// Helper function to read the covariance matrix from a file
std::vector<std::vector<double>> readCovariance(const std::string& filename, size_t hype_size) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    std::vector<std::vector<double>> cov_matrix(hype_size, std::vector<double>(hype_size));
    for (size_t i = 0; i < hype_size; ++i) {
        for (size_t j = 0; j < hype_size; ++j) {
            if (!(file >> cov_matrix[i][j])) {
                throw std::runtime_error("Error reading covariance matrix from file: " + filename);
            }
        }
    }
    file.close();
    return cov_matrix;
}

// Helper function to read the parameter-only covariance matrix from a file
std::vector<std::vector<double>> readParamCovariance(const std::string& filename, size_t num_params) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    std::vector<std::vector<double>> cov_matrix(num_params, std::vector<double>(num_params));
    for (size_t i = 0; i < num_params; ++i) {
        for (size_t j = 0; j < num_params; ++j) {
            if (!(file >> cov_matrix[i][j])) {
                throw std::runtime_error("Error reading parameter covariance matrix from file: " + filename);
            }
        }
    }
    file.close();
    return cov_matrix;
}

int main(int argc, char* argv[]) {

    MPI_Init(&argc, &argv);
    // 1. Define the isotopes to be used in the model
    modeloptimize::IsotopeInput ta181;
    ta181.name = "ta181";
    ta181.par_file = "data/par/ta181.par";


    std::vector<modeloptimize::IsotopeInput> isotopes = {ta181};

    // 2. Define the experimental data files
    std::vector<std::string> data_files = {
        "data/experimental/sammy_t8.0_r80.0_b1.00.dat",
            };

    try {
        // 3. Create the URRModel
        modeloptimize::URRModel model(isotopes, data_files);

        bool rerun_with_covariance = false;

        std::cout << "Running Gradient Descent Fit" << std::endl;
        modeloptimize::GradientDescentFit fitter;
        fitter.setNumIterations(0);
        fitter.setLearningRate(0.2);

        fitter.initialize(model);
        fitter.execute(model);
        fitter.finalize(model);

    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    }

    MPI_Finalize();

    return 0;
}
