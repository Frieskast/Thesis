#include "physics.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4CoulombScattering.hh"
#include "G4HadronPhysicsQGSP_BIC_HP.hh"
#include "G4hIonisation.hh"
#include "G4Proton.hh"
#include "G4ParticleTable.hh"
#include "G4EmParameters.hh"
#include "G4ProductionCutsTable.hh"
#include "G4ProcessManager.hh"


MyPhysicsList::MyPhysicsList() {
    // Register electromagnetic physics
    RegisterPhysics(new G4EmStandardPhysics_option4());

    // Optional: Register Livermore physics for low-energy accuracy (<10 keV)
    RegisterPhysics(new G4EmLivermorePhysics());

    // Register hadronic physics
    RegisterPhysics(new G4HadronPhysicsQGSP_BIC_HP());

    // Configure step-cut parameters
    G4EmParameters* emParams = G4EmParameters::Instance();
    emParams->SetStepFunction(0.05, 0.01 * CLHEP::um); // DEMAX = 0.05, STMIN = 0.01 μm
    emParams->SetMscStepLimitType(fUseSafetyPlus);     // Use safety-based step limitation
}

MyPhysicsList::~MyPhysicsList() {}

void MyPhysicsList::SetCuts() {
    // Call the base class method
    G4VUserPhysicsList::SetCuts();

    // Set specific cuts for individual particles
    SetCutValue(0.1 * CLHEP::mm, "gamma");
    SetCutValue(0.1 * CLHEP::mm, "e-");
    SetCutValue(0.1 * CLHEP::mm, "e+");
    SetCutValue(0.1 * CLHEP::mm, "proton");

    // Print the cuts for verification
    // G4ProductionCutsTable::GetProductionCutsTable()->DumpCouples();
}

void MyPhysicsList::ConstructProcess() {
    // Call the base class implementation to register processes from physics lists
    G4VModularPhysicsList::ConstructProcess();
}