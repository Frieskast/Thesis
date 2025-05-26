void tut1() {
    // --- Geant4 data ---
    TFile *f = TFile::Open("output17.root");
    if (!f || f->IsZombie()) {
        printf("Could not open output.root\n");
        return;
    }
    TH1* hG4 = (TH1*)f->Get("1 degree incident ");
    if (!hG4) {
        printf("Could not find histogram 'FractionVsExitEnergy'\n");
        return;
    }

    // --- Simulated SRIM data (replace with real parsing) ---
    double srim_vals[2] = {0.01, 0.03}; // Example values for two bins

    // --- Combined histogram ---
    int nbins = 2;
    double xmin = 0.0, xmax = 1.0;
    TH1D* hCombinedG4 = new TH1D("hCombinedG4", "Exit/Incident Energy Fraction;Exit/Incident Energy;Fraction", nbins, xmin, xmax);
    TH1D* hCombinedSRIM = new TH1D("hCombinedSRIM", "Exit/Incident Energy Fraction;Exit/Incident Energy;Fraction", nbins, xmin, xmax);

    // Fill with Geant4 and SRIM data
    for (int i = 1; i <= nbins; ++i) {
        hCombinedG4->SetBinContent(i, hG4->GetBinContent(i));
        hCombinedSRIM->SetBinContent(i, srim_vals[i-1]);
    }

    // Set colors
    hCombinedG4->SetBarWidth(0.4);
    hCombinedG4->SetBarOffset(0.1);
    hCombinedG4->SetFillColor(kBlue);

    hCombinedSRIM->SetBarWidth(0.4);
    hCombinedSRIM->SetBarOffset(0.55);
    hCombinedSRIM->SetFillColor(kRed);

    // Draw
    hCombinedG4->Draw("b");
    hCombinedSRIM->Draw("b same");

    TLegend* leg = new TLegend(0.6,0.7,0.88,0.88);
    leg->AddEntry(hCombinedG4, "Geant4", "f");
    leg->AddEntry(hCombinedSRIM, "SRIM", "f");
    leg->Draw();

    gPad->SetGrid();

    // --- Additional histogram drawing ---
    TH2* hXZ = (TH2*)f->Get("h2_0"); // or the name you gave to H2(0)
    hXZ->Draw("COLZ"); // For a color map
    // or
    // hXZ->Draw("SCAT"); // For a scatter plot (if supported)
}