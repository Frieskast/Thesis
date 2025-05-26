#ifndef PHYSICS_HH
#define PHYSICS_HH

#include "G4VModularPhysicsList.hh"
#include "G4EmStandardPhysics.hh"
#include "G4OpticalPhysics.hh"
#include "G4EmLowEPPhysics.hh"  
#include "G4DecayPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4HadronPhysicsQGSP_BERT_HP.hh" // Hadronic physics
#include "G4IonPhysics.hh"            // Ion physics
#include "G4HadronElasticPhysicsHP.hh"
#include "G4EmStandardPhysics_option3.hh"
#include "G4hPairProduction.hh"
#include "G4hBremsstrahlung.hh"
#include "G4hIonisation.hh"
#include "G4hMultipleScattering.hh"

class MyPhysicsList : public G4VModularPhysicsList
{
public:
    MyPhysicsList();
    ~MyPhysicsList();

    void SetCuts() override;
    void ConstructProcess() override; // Add this declaration
    void ConstructEM();       // Electromagnetic processes
    void ConstructGeneral();  // General processes
};

#endif
