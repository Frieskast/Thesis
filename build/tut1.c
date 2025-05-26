void tut1() {
    TFile *f = TFile::Open("output.root");
    if (!f || f->IsZombie()) {
        printf("Could not open output.root\n");
        return;
    }
    TH2* h2 = (TH2*)f->Get("XZProjection");
    if (!h2) {
        printf("Could not find histogram 'XZProjection'\n");
        return;
    }
    TCanvas* c1 = new TCanvas("c1", "XZProjection as black dots", 800, 600);
    h2->SetMarkerStyle(20);    // Solid circle
    h2->SetMarkerColor(kBlack);
    h2->SetMarkerSize(0.7);    // Adjust size as needed
    h2->Draw("P");             // Draw ONLY points (no boxes, no color map)
}