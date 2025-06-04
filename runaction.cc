#include "runaction.hh"
#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "G4AccumulableManager.hh"
#include <fstream>

MyRunAction::MyRunAction() : G4UserRunAction(), fExitCount(0) {
    G4AccumulableManager::Instance()->Register(fExitCount);

    auto analysisManager = G4AnalysisManager::Instance();

    // Enable ntuple merging for multi-threaded mode
    analysisManager->SetNtupleMerging(true);

    // Set default file type (optional, e.g., root, csv, xml)
    analysisManager->SetDefaultFileType("root");

    // Create histograms
    G4cout << "Creating histograms..." << G4endl;
    analysisManager->CreateH1("Energy", "Energy of exiting particles (keV)", 100, 0., 1000.);
    analysisManager->CreateH1("Elevation", "Elevation angle (degrees)", 720, -1.0, 180.);
    analysisManager->CreateH1("Azimuth", "Azimuth angle (degrees)", 362, -180., 180.);
    analysisManager->CreateH1("Energy", "Exit/Incident Energy", 40, 0.0, 1.0);
    analysisManager->CreateH2("XZProjection", "", 100, 0.0, 1.2, 100, -1.5, 1.5);
    analysisManager->CreateH2("YZProjection", "", 100, 0.0, 1.2, 100, -1.5, 1.5);
    analysisManager->CreateH2("AzimuthVsElevation", "Azimuth vs Elevation (deg);Elevation (deg);Azimuth (deg)",
                              181, 0., 180., 362, -180., 180.);
    analysisManager->CreateH2("1 degree incident ", "Fraction of incident Particles over Exit/incident Energy",
                              10, 0., 0.1, 20, 0.0, 1);

    // Create ntuple
    G4cout << "Creating ntuple..." << G4endl;
    analysisManager->CreateNtuple("ExitingParticles", "Energy, Elevation, Azimuth");
    analysisManager->CreateNtupleDColumn("Energy");
    analysisManager->CreateNtupleDColumn("Elevation");
    analysisManager->CreateNtupleDColumn("Azimuth");
    analysisManager->FinishNtuple();
}

MyRunAction::~MyRunAction() {}

void MyRunAction::BeginOfRunAction(const G4Run*) {
    G4AccumulableManager::Instance()->Reset();

    // Find the first available output file name
    G4String filename;
    for (int i = 0; i < 100; ++i) {
        if (i == 0)
            filename = "output.root";
        else
            filename = "output" + std::to_string(i) + ".root";
        std::ifstream testfile(filename);
        if (!testfile.good()) {
            break;
        }
    }

    G4AnalysisManager *man = G4AnalysisManager::Instance();
    man->OpenFile(filename);
}

void MyRunAction::EndOfRunAction(const G4Run*) {
    // Merge accumulables across threads
    G4AccumulableManager::Instance()->Merge();

    // Print the total number of protons exited the gold block (after merging)
    G4cout << "Total protons exited the gold block: " << fExitCount.GetValue() << G4endl;

    G4AnalysisManager *man = G4AnalysisManager::Instance();
    man->Write();
    man->CloseFile();
}



