#include "physics.hh"
#include "G4EmLivermorePhysics.hh"       // Low-energy electromagnetic physics
#include "G4EmStandardPhysics_option3.hh"
// #include "G4EmPenelopePhysics.hh"       // Alternative low-energy electromagnetic physics
#include "G4OpticalPhysics.hh"
// #include "G4DecayPhysics.hh"
// #include "G4RadioactiveDecayPhysics.hh"
#include "G4HadronPhysicsQGSP_BERT.hh"  // Hadronic physics
#include "G4HadronElasticPhysicsHP.hh"  // High-precision elastic physics
#include "G4IonElasticPhysics.hh"       // Low-energy ion elastic physics
#include "G4IonPhysicsPHP.hh"           // High-precision ion physics
#include "G4HadronPhysicsQGSP_BERT_HP.hh"
#include "G4hPairProduction.hh"
#include "G4hBremsstrahlung.hh"
#include "G4hIonisation.hh"
#include "G4hMultipleScattering.hh"
#include "G4Proton.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"

MyPhysicsList::MyPhysicsList() {
    // Electromagnetic physics
    RegisterPhysics(new G4EmStandardPhysics_option3());

    // Define the proton explicitly
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* proton = particleTable->FindParticle("proton");

    if (!proton) {
        G4cerr << "Error: Proton particle is not defined!" << G4endl;
        return;
    }

    // Hadronic processes for protons
    RegisterProcess(new G4hPairProduction(), proton);
    RegisterProcess(new G4hBremsstrahlung(), proton);
    RegisterProcess(new G4hIonisation(), proton);
    RegisterProcess(new G4hMultipleScattering(), proton);
    RegisterPhysics(new G4HadronElasticPhysicsHP());
    RegisterPhysics(new G4HadronPhysicsQGSP_BERT_HP());
}

MyPhysicsList::~MyPhysicsList() {}