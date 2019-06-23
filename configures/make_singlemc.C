void make_singlemc()
{
  auto numEvents = 1000;
  ofstream file("singlemc_neuland_test.egen");
  file << numEvents << endl;
  Double_t pvalue = 1.;
  Double_t angle = 40.;
  auto px = pvalue*TMath::Sin(angle*TMath::DegToRad());
  auto pz = pvalue*TMath::Cos(angle*TMath::DegToRad());
  TVector3 mom1(px,0,pz); // GeV/c
  for (auto ievent=0; ievent<numEvents; ++ievent) {
    file << ievent << " 1 0" << endl;
    file << "2212 " << mom1.X() << " " << mom1.Y() << " " << mom1.Z() << endl;
  }
}
