#include <iostream>
#include <fstream>
#include "G4MTRunManager.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "physics.hh"
#include "detector.hh"
#include "action.hh"
#include "G4VExceptionHandler.hh"
#include "G4ios.hh"
#include "construction.hh"
#include "G4UserLimits.hh"


class MyExceptionHandler : public G4VExceptionHandler {
public:
    G4bool Notify(const char* originOfException, const char* exceptionCode,
        G4ExceptionSeverity severity, const char* description) override {
        G4cerr << "\n*** Custom G4Exception Handler ***" << G4endl;
        G4cerr << "Exception from: " << originOfException << G4endl;
        G4cerr << "Code: " << exceptionCode << G4endl;
        G4cerr << "Severity: " << severity << G4endl;
        G4cerr << "Description: " << description << G4endl;

        if (severity == FatalException) {
            G4cerr << "\nFatal error encountered! Pausing instead of exiting...\n";
            G4cerr << "Press Enter to continue." << G4endl;
            std::cin.get();  // Wait for user input before continuing
            return true;  // Continue execution instead of aborting
        }
        return false;  // Use default behavior for non-fatal exceptions
    }
};

int main(int argc, char** argv) {
    #ifdef G4MULTITHREADED
    G4MTRunManager* runManager = new G4MTRunManager();
    runManager->SetNumberOfThreads(1); // Set the number of threads to 4
    #else
    G4RunManager* runManager = new G4RunManager();
    #endif

    runManager->SetUserInitialization(new MyDetectorConstruction());
    runManager->SetUserInitialization(new MyPhysicsList());
    runManager->SetUserInitialization(new MyActionInitialization());

    runManager->Initialize();

    G4UIExecutive* ui = new G4UIExecutive(argc, argv);

    G4VisManager* visManager = new G4VisExecutive();
    visManager->Initialize();

    G4UImanager* UImanager = G4UImanager::GetUIpointer();

    UImanager->ApplyCommand("/tracking/verbose 1"); // Enable verbose tracking

    // // Redirect G4cout to a file
    // std::ofstream overlapFile("overlap_check.log");
    // std::streambuf* oldBuffer = G4cout.rdbuf(); // Save the old buffer
    // G4cout.rdbuf(overlapFile.rdbuf());          // Redirect G4cout to the file

    // // Check for overlaps
    // UImanager->ApplyCommand("/geometry/test/recursive_world 1");
    // UImanager->ApplyCommand("/geometry/test/run");

    // // Restore G4cout to its original state
    // G4cout.rdbuf(oldBuffer);
    // overlapFile.close();

    UImanager->ApplyCommand("/vis/open OGL");
    UImanager->ApplyCommand("/vis/viewer/set/viewpointVector 1 1 1");
    UImanager->ApplyCommand("/vis/drawVolume");
    UImanager->ApplyCommand("/vis/viewer/set/autoRefresh true");
    UImanager->ApplyCommand("/vis/scene/add/trajectories smooth");
    UImanager->ApplyCommand("/vis/scene/endOfEventAction accumulate");
    UImanager->ApplyCommand("/vis/scene/add/axes 0 0 0 0.1"); // Axes length 10 cm, centered at origin
    UImanager->ApplyCommand("/cuts/setLowEdge 10 eV");
    UImanager->ApplyCommand("/run/setCut 1 um");
    UImanager->ApplyCommand("/run/beamOn 100");

    G4cout << "Physics processes and models for protons:" << G4endl;

    G4ProcessManager* pManager = G4ParticleTable::GetParticleTable()->FindParticle("proton")->GetProcessManager();
    if (pManager) {
        G4ProcessVector* processVector = pManager->GetProcessList();
        for (size_t i = 0; i < processVector->size(); ++i) {
            G4VProcess* process = (*processVector)[i];
            G4cout << "Process: " << process->GetProcessName() << G4endl;

            // Use DumpInfo() to output detailed information about the process
            process->DumpInfo();
        }
    
    }
    
    ui->SessionStart();
    // UImanager->ApplyCommand("/run/beamOn 1000");
    /*G4UIExecutive* ui = 0;



    G4VExceptionHandler* myHandler = new MyExceptionHandler();
    G4StateManager::GetStateManager()->SetExceptionHandler(myHandler);

    runManager->SetUserInitialization(new MyDetectorConstruction());
    runManager->SetUserInitialization(new MyPhysicsList());
    runManager->SetUserInitialization(new MyActionInitialization());

    G4VModularPhysicsList* physics = new QGSP_BERT();
    runManager->SetUserInitialization(physics);

    if (argc == 1)
    {
        ui = new G4UIExecutive(argc, argv);
    }

    G4VisManager *visManager = new G4VisExecutive();
    visManager->Initialize();

    G4UImanager *UImanager = G4UImanager::GetUIpointer();

    if(ui)
    {
        UImanager->ApplyCommand("/control/execute vis.mac");
        ui->SessionStart();
    }
    else
    {
        G4String command = "/control/execute ";
        G4String fileName = argv[1];
        UImanager->ApplyCommand(command+fileName);
    }*/

    return 0;
}

