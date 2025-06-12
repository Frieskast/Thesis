#include "runaction.hh"
#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "G4AccumulableManager.hh"
#include "G4SystemOfUnits.hh"
#include <fstream>
#include <sstream>
#include <iomanip>
#include "generator.hh"
#include "TParameter.h"
#include <TFile.h>
#include "G4EmParameters.hh"
#include <TROOT.h>
#include "G4Run.hh"

MyRunAction::MyRunAction() : G4UserRunAction(), fExitCount(0) {
    G4AccumulableManager::Instance()->Register(fExitCount);

    auto analysisManager = G4AnalysisManager::Instance();

    // Enable ntuple merging for multi-threaded mode
    analysisManager->SetNtupleMerging(true);

    // Set default file type (optional, e.g., root, csv, xml)
    analysisManager->SetDefaultFileType("root");

    // Create histograms
    G4cout << "Creating histograms..." << G4endl;
    // analysisManager->CreateH1("Energy", "Energy of exiting particles (keV)", 100, 0., 1000.);
    analysisManager->CreateH1("Elevation", "Elevation angle (degrees)", 720, -1.0, 180.);
    analysisManager->CreateH1("Azimuth", "Azimuth angle (degrees)", 182, -90., 90.);
    analysisManager->CreateH1("Energy", "Exit/Incident Energy", 40, 0.0, 1.0);
    analysisManager->CreateH2("XZProjection", "", 100, 0.0, 1.2, 100, -1.5, 1.5);
    analysisManager->CreateH2("YZProjection", "", 100, 0.0, 1.2, 100, -1.5, 1.5);
    analysisManager->CreateH2("AzimuthVsElevation", "Azimuth vs Elevation (deg);Elevation (deg);Azimuth (deg)",
                              181, 0., 90., 181, -90.0, 90.0);
    // analysisManager->CreateH2("1 degree incident ", "Fraction of incident Particles over Exit/incident Energy",
    //                           10, 0., 0.1, 20, 0.0, 1);

    // Create ntuple
    G4cout << "Creating ntuple..." << G4endl;
    analysisManager->CreateNtuple("ExitData", "EnergyRatio, Energy, Elevation, Azimuth, x, y, z");
    analysisManager->CreateNtupleDColumn("EnergyRatio");
    analysisManager->CreateNtupleDColumn("Energy");      // in keV
    analysisManager->CreateNtupleDColumn("Elevation");   // in deg
    analysisManager->CreateNtupleDColumn("Azimuth");     // in deg
    analysisManager->CreateNtupleDColumn("x");           // unit vector x
    analysisManager->CreateNtupleDColumn("y");           // unit vector y
    analysisManager->CreateNtupleDColumn("z");           // unit vector z

    analysisManager->FinishNtuple();
}

MyRunAction::~MyRunAction() {}

void MyRunAction::BeginOfRunAction(const G4Run* run) {
    G4AccumulableManager::Instance()->Reset();

    // Ensure the directory exists
    system("mkdir -p build/root");

    // Use run ID to make filename unique
    std::ostringstream oss;
    oss << "build/root/output_pending_run" << run->GetRunID() << ".root";
    G4String filename = oss.str();

    G4AnalysisManager *man = G4AnalysisManager::Instance();
    man->OpenFile(filename);

    // Store for later renaming
    fPendingFileName = filename;
}

void MyRunAction::EndOfRunAction(const G4Run*) {
    G4AccumulableManager::Instance()->Merge();
    G4cout << "Total protons exited the gold block: " << fExitCount.GetValue() << G4endl;

    G4AnalysisManager *man = G4AnalysisManager::Instance();
    man->Write();
    man->CloseFile();

    // Rename the file to include the correct angle and energy
    std::string oldName = fPendingFileName;
    std::string newName = "build/root/" + GenerateFileName();
    std::rename(oldName.c_str(), newName.c_str());
}

G4String MyRunAction::GenerateFileName() {
    double thetaLimit = G4EmParameters::Instance()->MscThetaLimit();
    std::ostringstream oss;
    oss << "output_"
        << std::fixed << std::setprecision(0)
        << fIncidentAngleDeg << "deg_"
        << fEnergyKeV << "keV_"
        << "TL" << std::setprecision(3) << thetaLimit << "rad"
        << ".root";
    return oss.str();
}



