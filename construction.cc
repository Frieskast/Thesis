#include "construction.hh"
#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"
#include "G4VisAttributes.hh"
#include <cmath>
#include <vector>
#include <fstream>
#include <iomanip>
#include "G4OpticalSurface.hh"       // For G4OpticalSurface
#include "G4LogicalSkinSurface.hh"   // For G4LogicalSkinSurface
#include "G4MaterialPropertiesTable.hh" // For material properties
#include "G4OpticalPhysics.hh"       // For G4OpticalPhysics
#include "physics.hh"
#include "G4UserLimits.hh"
#include "G4GDMLParser.hh"

// Constants for the gold block and detector
const double goldBlockThickness = 0.0002 * mm;    // 200 nm
const double epoxyThickness = 0.012 * mm;         // 12 um
const double aluminiumThickness = 0.152 * mm;     // 0.152 mm
const double blockWidth = 0.1 * m;
const double blockHeight = 0.1 * m;
const double detectorThickness = 0.01 * m;  // Thickness of the detector
const double detectorRadius = 0.05 * m;     // Radius of the detector
const double detectorXPosition = 0.8 * m;   // Position of the detector along the z-axis

MyDetectorConstruction::MyDetectorConstruction() : G4VUserDetectorConstruction() {
    // Constructor implementation (if needed)
}

MyDetectorConstruction::~MyDetectorConstruction() {
    // Destructor implementation (if needed)
}

G4VPhysicalVolume* MyDetectorConstruction::Construct() {
    G4NistManager* nist = G4NistManager::Instance();

    // Define materials
    G4Material* gold = nist->FindOrBuildMaterial("G4_Au");
    G4Material* nickel = nist->FindOrBuildMaterial("G4_Ni");
    G4Material* worldMat = nist->FindOrBuildMaterial("G4_Galactic");
    G4Material* silicon = nist->FindOrBuildMaterial("G4_Si");

    // Aluminium from NIST
    G4Material* aluminium = nist->FindOrBuildMaterial("G4_Al");

    // Epoxy: C₂H₅O (approximate, density ~1.2 g/cm³)
    G4Material* epoxy = new G4Material("Epoxy", 1.18 * g/cm3, 3);
    epoxy->AddElement(nist->FindOrBuildElement("C"), 18);
    epoxy->AddElement(nist->FindOrBuildElement("H"), 19);
    epoxy->AddElement(nist->FindOrBuildElement("O"), 3);

    // Define world volume
    G4double xWorld = 1 * m;
    G4double yWorld = 1 * m;
    G4double zWorld = 1 * m;

    G4Box* solidWorld = new G4Box("solidWorld", xWorld, yWorld, zWorld);
    G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, worldMat, "logicWorld");
    G4VPhysicalVolume* physWorld = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), logicWorld, "physWorld", 0, false, 0, true);

    // Add a step limiter to the vacuum (world volume)
    G4double maxStepVacuum = 0.1 * mm; // Set a maximum step size for the vacuum
    logicWorld->SetUserLimits(new G4UserLimits(maxStepVacuum));

    // Place the gold block
    G4Box* solidGoldBlock = new G4Box("solidGoldBlock", goldBlockThickness / 2, blockWidth / 2, blockHeight / 2);
    G4LogicalVolume* logicGoldBlock = new G4LogicalVolume(solidGoldBlock, gold, "logicGoldBlock");
    new G4PVPlacement(
        0,
        G4ThreeVector(goldBlockThickness / 2, 0., 0.), // Gold at lowest x
        logicGoldBlock,
        "physGoldBlock",
        logicWorld,
        false,
        0,
        true
    );

    // Place the epoxy block directly behind gold
    G4Box* solidEpoxyBlock = new G4Box("solidEpoxyBlock", epoxyThickness / 2, blockWidth / 2, blockHeight / 2);
    G4LogicalVolume* logicEpoxyBlock = new G4LogicalVolume(solidEpoxyBlock, epoxy, "logicEpoxyBlock");
    new G4PVPlacement(
        0,
        G4ThreeVector(goldBlockThickness + (epoxyThickness / 2), 0., 0.), // Epoxy behind gold
        logicEpoxyBlock,
        "physEpoxyBlock",
        logicWorld,
        false,
        0,
        true
    );

    // Place the aluminium block directly behind epoxy
    G4Box* solidAlBlock = new G4Box("solidAlBlock", aluminiumThickness / 2, blockWidth / 2, blockHeight / 2);
    G4LogicalVolume* logicAlBlock = new G4LogicalVolume(solidAlBlock, aluminium, "logicAlBlock");
    new G4PVPlacement(
        0,
        G4ThreeVector(goldBlockThickness + epoxyThickness + (aluminiumThickness / 2), 0., 0.), // Al behind epoxy
        logicAlBlock,
        "physAlBlock",
        logicWorld,
        false,
        0,
        true
    );

    // // Load the GDML file
    // G4GDMLParser parser;
    // parser.Read("/home/frisoe/geant4/geant4-v11.3.1/examples/Projects/thesis/gdml/cocomirror.gdml"); // Path to the GDML file

    // // Place the GDML geometry into the world volume
    // G4LogicalVolume* gdmlLogic = parser.GetWorldVolume()->GetLogicalVolume();
    // new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), gdmlLogic, "gdmlPhys", logicWorld, false, 0, true);

    // Place the detector
    // G4RotationMatrix* rotation = new G4RotationMatrix();
    // rotation->rotateY(90.0 * deg); // Rotate the detector 90 degrees around the y-axis

    // G4Tubs* siliconDetector = new G4Tubs("siliconDetector", 0, detectorRadius, detectorThickness / 2, 0, 2 * M_PI);
    // logicDetector = new G4LogicalVolume(siliconDetector, silicon, "logicDetector");
    // new G4PVPlacement(rotation, G4ThreeVector(-detectorXPosition, 0., 0.), logicDetector, "physDetector", logicWorld, false, 0, true);

    return physWorld;
}

// const double FL = 0.8 * m; // Focal length
// const double Lparabolic = 0.1 * m;
// const double Lhyperbolic = 0.1 * m;
// const double Rconcentrator = 0.05 * m;
// const double Rdumbell = 0.01 * m;
// const double tshell = 2e-6 * m; // Thickness of the mirror (now only gold)
// const double Dspacing_lim = 0.001 * m;
// const double detectorThickness = 0.01 * m; // Thickness of the detector

// struct MirrorShell {
//     std::vector<std::pair<double, double>> parabolic;
//     std::vector<std::pair<double, double>> hyperbolic;
// };

// std::vector<MirrorShell> mirror_coords;

// MirrorShell calculateMirrorShell(double Ypmin) {
//     double theta_max = std::asin(Ypmin / FL) / 4;
//     double p = Ypmin * std::tan(theta_max);
//     double c = FL / 2;
//     double a = FL * ((2 * std::cos(2 * theta_max) - 1) / 2);
//     double b = std::sqrt(c * c - a * a);

//     double Xpmin = FL * std::cos(4 * theta_max) + 2 * c;
//     double Xpmax = Xpmin + Lparabolic;

//     double Xhmin = Xpmin - Lhyperbolic;
//     double Xhmax = Xpmin;

//     std::vector<std::pair<double, double>> parabolic, hyperbolic;

//     for (double xp = Xpmax; xp >= Xpmin; xp -= (Xpmax - Xpmin) / 100) {
//         double yp = std::sqrt(p * (2 * xp + p));
//         parabolic.emplace_back(xp, yp);
//     }

//     for (double xh = Xhmax; xh >= Xhmin; xh -= (Xhmax - Xhmin) / 100) {
//         double yh = b * std::sqrt(((xh - c) * (xh - c) / (a * a)) - 1);
//         hyperbolic.emplace_back(xh, yh);
//     }

//     return {parabolic, hyperbolic};
// }

// double findBestStartingYpmin() {
//     double Ypmin = Rdumbell + Dspacing_lim;
//     int maxIterations = 1000000; // Safeguard to prevent infinite loop
//     int iteration = 0;

//     while (true) {
//         if (iteration++ > maxIterations) {
//             G4cerr << "Error: Maximum iterations reached in findBestStartingYpmin. Check your parameters." << G4endl;
//             break;
//         }

//         MirrorShell shell = calculateMirrorShell(Ypmin);
//         if (shell.parabolic.back().second >= Rdumbell + Dspacing_lim &&
//             shell.hyperbolic.back().second >= Rdumbell + Dspacing_lim) {
//             return Ypmin;
//         }

//         Ypmin += 0.0001;

//         // Debugging output
//         if (iteration % 100000 == 0) {
//             G4cout << "Iteration: " << iteration << ", Ypmin: " << Ypmin / m << " m" << G4endl;
//         }
//     }

//     return Ypmin; // Return the last value of Ypmin if the loop exits
// }

// double findBestNextMirror(double Ypmin, double Ypmax, double Yhmax) {
//     while (true) {
//         MirrorShell shell = calculateMirrorShell(Ypmin);
//         double Dspacing_p = shell.parabolic.back().second - Ypmax;
//         double Dspacing_h = shell.hyperbolic.back().second - Yhmax;

//         if (Dspacing_p >= Dspacing_lim && Dspacing_h >= Dspacing_lim) {
//             return Ypmin;
//         }
//         Ypmin += 0.000001;
//     }
// }

// void nestedMirrors() {
//     double Ypmin = findBestStartingYpmin();
//     MirrorShell shell = calculateMirrorShell(Ypmin);
//     mirror_coords.push_back(shell);

//     while (shell.parabolic.front().second < Rconcentrator) {
//         Ypmin = findBestNextMirror(shell.parabolic.front().second + tshell, shell.parabolic.back().second, shell.hyperbolic.back().second);
//         shell = calculateMirrorShell(Ypmin);
//         mirror_coords.push_back(shell);
//     }
// }

// void saveToSCRFile(const std::vector<MirrorShell>& coords, const std::string& filename) {
//     std::ofstream file(filename);
//     file << std::fixed << std::setprecision(6);

//     for (size_t i = 0; i < coords.size(); ++i) {
//         file << "pline\n";
//         file << ";parabolic\n";
//         for (const auto& coord : coords[i].parabolic) {
//             file << coord.first * 100 << "," << coord.second * 100 << "\n"; // Convert to cm
//         }
//         file << "\n;hyperbolic\npline\n";
//         for (const auto& coord : coords[i].hyperbolic) {
//             file << coord.first * 100 << "," << coord.second * 100 << "\n"; // Convert to cm
//         }
//         file << "\n";
//     }
//     file.close();
// }

// void DefineOpticalProperties() {
//     // Get the gold material
//     G4Material* gold = G4Material::GetMaterial("G4_Au");

//     // Define photon energy range (in eV)
//     const G4int nEntries = 2;
//     G4double photonEnergy[nEntries] = {1.0 * keV, 15.0 * keV}; // X-ray energy range

//     // Define refractive indices for gold (real and imaginary parts)
//     G4double refractiveIndexReal[nEntries] = {0.999, 0.999}; // Example values
//     G4double refractiveIndexImag[nEntries] = {0.001, 0.001}; // Example values

//     // Calculate reflectivity based on grazing angle and refractive indices
//     G4double reflectivity[nEntries];
//     for (int i = 0; i < nEntries; ++i) {
//         // Example: Reflectivity is simplified here; use Fresnel equations for accuracy
//         reflectivity[i] = 0.95; // Placeholder value
//     }

//     // Create a material properties table
//     G4MaterialPropertiesTable* goldMPT = new G4MaterialPropertiesTable();
//     goldMPT->AddProperty("REFLECTIVITY", photonEnergy, reflectivity, nEntries);

//     // Assign the material properties table to gold
//     gold->SetMaterialPropertiesTable(goldMPT);
// }

// void DefineMirrorSurface(G4LogicalVolume* logicMirror) {
//     // Create a dielectric-metal surface
//     G4OpticalSurface* mirrorSurface = new G4OpticalSurface("MirrorSurface");
//     mirrorSurface->SetType(dielectric_metal);
//     mirrorSurface->SetFinish(polished);
//     mirrorSurface->SetModel(unified);

//     // Assign the material properties table (reflectivity) to the surface
//     G4Material* gold = G4Material::GetMaterial("G4_Au");
//     mirrorSurface->SetMaterialPropertiesTable(gold->GetMaterialPropertiesTable());

//     // Attach the surface to the logical volume
//     new G4LogicalSkinSurface("MirrorSurface", logicMirror, mirrorSurface);
// }

// MyDetectorConstruction::MyDetectorConstruction() : G4VUserDetectorConstruction() {
//     // Constructor implementation (if needed)
// }

// MyDetectorConstruction::~MyDetectorConstruction() {
//     // Destructor implementation (if needed)
// }

// G4VPhysicalVolume* MyDetectorConstruction::Construct() {
//     G4NistManager* nist = G4NistManager::Instance();

//     // Define materials
//     G4Material* gold = nist->FindOrBuildMaterial("G4_Au");
//     G4Material* worldMat = nist->FindOrBuildMaterial("G4_Galactic"); // Use vacuum instead of air
//     G4Material* silicon = nist->FindOrBuildMaterial("G4_Si");

//     // Define world volume
//     G4double xWorld = 2 * m;
//     G4double yWorld = 2 * m;
//     G4double zWorld = 2 * m;

//     G4Box* solidWorld = new G4Box("solidWorld", xWorld, yWorld, zWorld);
//     G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, worldMat, "logicWorld");
//     G4VPhysicalVolume* physWorld = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), logicWorld, "physWorld", 0, false, 0, true);

//     // Generate nested mirrors
//     nestedMirrors();
    
//     double mirror_shells = 0;
//     // Place mirrors
//     for (const auto& shell : mirror_coords) {
//         for (size_t i = 0; i < shell.parabolic.size(); ++i) {
//             double innerRadius = shell.parabolic[i].second;
//             double outerRadius = innerRadius + tshell;
//             double zPosition = shell.parabolic[i].first;

//             G4Tubs* mirror = new G4Tubs("mirror", innerRadius, outerRadius, Lparabolic / 2, 0, 2 * M_PI);
//             G4LogicalVolume* logicMirror = new G4LogicalVolume(mirror, gold, "logicMirror");
//             DefineMirrorSurface(logicMirror);
//             new G4PVPlacement(0, G4ThreeVector(0, 0, zPosition), logicMirror, "physMirror", logicWorld, false, 0, true);
//             mirror_shells += 1;
//         }
//     }
//     G4cout << "Total number of mirror shells: " << mirror_coords.size() << G4endl;

//     // Place a single circular detector
//     double detectorZPosition = FL - (0.5 * mm); // Adjust position along the z-axis
//     double detectorRadius = 0.05 * m;
//     double detectorThickness = 0.5 * mm;
//     G4Tubs* siliconDetector = new G4Tubs("siliconDetector", 0, detectorRadius, detectorThickness / 2, 0, 2 * M_PI);
//     logicDetector = new G4LogicalVolume(siliconDetector, silicon, "logicDetector");
//     new G4PVPlacement(0, G4ThreeVector(0., 0., detectorZPosition), logicDetector, "physDetector", logicWorld, false, 0, true);

//     // Save mirror coordinates to SCR file
//     saveToSCRFile(mirror_coords, "output_outward_g4.scr");

//     return physWorld;
// }

void MyDetectorConstruction::ConstructSDandField()
{
    // MySensitiveDetector *sensDet = new MySensitiveDetector("SensitiveDetector");
    // if (logicDetector) {
    //     logicDetector->SetSensitiveDetector(sensDet);
    // }
}
