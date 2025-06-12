#ifndef MYSTEPPINGACTION_HH
#define MYSTEPPINGACTION_HH

#include "G4UserSteppingAction.hh"
#include "G4Accumulable.hh"

class MySteppingAction : public G4UserSteppingAction {
public:
    MySteppingAction();
    virtual ~MySteppingAction();
    virtual void UserSteppingAction(const G4Step*);

private:
    G4Accumulable<G4int> fExitCount;
    int fIonizationCount = 0; // Add this line
public:
    int GetIonizationCount() const { return fIonizationCount; } // Optional getter
    void ResetIonizationCount() { fIonizationCount = 0; }       // Optional reset
};

#endif