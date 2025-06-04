#include "physics.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4CoulombScattering.hh"
#include "G4HadronPhysicsQGSP_BIC_HP.hh"
#include "G4HadronElasticPhysicsHP.hh"
#include "G4hIonisation.hh"
#include "G4Proton.hh"
#include "G4ParticleTable.hh"
#include "G4EmParameters.hh"
#include "G4ProductionCutsTable.hh"
#include "G4ProcessManager.hh"
#include "G4DecayPhysics.hh"

MyPhysicsList::MyPhysicsList() {
    // Register electromagnetic physics
    RegisterPhysics(new G4EmStandardPhysics_option4());

    // Optional: Register Livermore physics for low-energy accuracy (<10 keV)
    // RegisterPhysics(new G4EmLivermorePhysics());

    // Register hadronic physics
    RegisterPhysics(new G4HadronPhysicsQGSP_BIC_HP());
    RegisterPhysics(new G4HadronElasticPhysics());
    

    // Configure step-cut parameters
    G4EmParameters* emParams = G4EmParameters::Instance();
    emParams->SetStepFunction(0.02, 0.0001 * CLHEP::um); // DEMAX = 0.05, STMIN = 0.01 μm
    // emParams->SetMscStepLimitType(fUseSafetyPlus);     // Use safety-based step limitation

    // Set global energy range for production cuts
    G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(0.000001 * CLHEP::eV, 100 * CLHEP::GeV);
}

MyPhysicsList::~MyPhysicsList() {}

void MyPhysicsList::SetCuts() {
    // Call the base class method
    G4VUserPhysicsList::SetCuts();

    // Set specific cuts for individual particles
    SetCutValue(0.000001 * CLHEP::keV, "gamma");
    SetCutValue(0.000001 * CLHEP::keV, "e-");
    SetCutValue(0.000001 * CLHEP::keV, "e+");
    SetCutValue(0.000001 * CLHEP::keV, "proton");

    // Print the cuts for verification
    // G4ProductionCutsTable::GetProductionCutsTable()->DumpCouples();
}

void MyPhysicsList::ConstructProcess() {
    // Call the base class implementation to register processes from physics lists
    G4VModularPhysicsList::ConstructProcess();
}