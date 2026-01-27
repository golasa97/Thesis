#include "fitAPI/URRModel.h"
#include "fitAPI/MetropolisFit.h"
#include "fitAPI/MinuitFit.h"
#include "fitAPI/impl/OneDimVec.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>




int main(int argc, char* argv[]) {
    // 1. Define the isotopes to be used in the model
    modeloptimize::IsotopeInput zr90;
    zr90.name = "Zr90";
    zr90.par_file = "data/par/zr90.par";
    zr90.parameter_grid_files.push_back("data/par/zr90_intermediate_pwave");

    modeloptimize::IsotopeInput zr91;
    zr91.name = "Zr91";
    zr91.par_file = "data/par/zr91.par";

    modeloptimize::IsotopeInput zr92;
    zr92.name = "Zr92";
    zr92.par_file = "data/par/zr92.par";

    modeloptimize::IsotopeInput zr94;
    zr94.name = "Zr94";
    zr94.par_file = "data/par/zr94.par";

    modeloptimize::IsotopeInput zr96;
    zr96.name = "Zr96";
    zr96.par_file = "data/par/zr96.par";

    std::vector<modeloptimize::IsotopeInput> isotopes = {zr90, zr91, zr92, zr94, zr96};

    std::vector<std::string> data_files = {
        "data/experimental/zr91_Musgrove_1977_200m_formatted_transmission.txt",
        "data/experimental/nat-zr_Rapp_2019_10cm_formatted_transmission.txt",
        "data/experimental/nat-zr_Rapp_2019_6cm_formatted_transmission.txt",
        "data/experimental/musgrove_200m.dat",
        "data/experimental/ohgama.txt",
    };

    try {
        // 3. Create the URRModel
        modeloptimize::URRModel model(isotopes, data_files);

        int ns = 100000;
        fitter.setNumSamps(ns);
        double mult = 0.3;
        std::vector<double> pmult(model.getNumberParams(), mult);
        double dmult = mult;
        fitter.setSampDistMultiplier(pmult, dmult);
        fitter.initialize(model);
        fitter.execute(model);
        fitter.finalize(model);

        std::cout << "Fit finished." << std::endl;

        std::vector<double> final_params_vec(model.getNumberParams());
        modeloptimize::OneDimVec final_params(final_params_vec);
        model.getModelParam(final_params);

        std::cout << "\n--- Final Parameters ---" << std::endl;
        for (size_t i = 0; i < final_params.getSize(); ++i) {
            std::cout << "Param " << i << ": " << final_params.getValue(i) << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
