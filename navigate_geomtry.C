ofstream geometry_file;

void listNode(TGeoNode *node, Int_t rank = 0)
{
  auto list = node -> GetNodes();
  auto numNodes = list -> GetEntries();
  for (auto iv=0; iv<numNodes; ++iv) {
    auto nodei = (TGeoNode *) list -> At(iv);
    for (auto irank=0; irank<rank; ++irank)
      cout << "    ";
    cout << left << setw(20) << nodei -> GetVolume() -> GetName() << " " << nodei -> GetNumber() << endl;
    geometry_file << nodei -> GetVolume() -> GetName() << " " << nodei -> GetNumber() << endl;

    auto nodeArray = nodei->GetNodes();
    if (nodeArray!=0&&nodeArray->GetEntries()>0)
      listNode(nodei,rank+1);
  }
}

void navigate_geomtry()
{
  geometry_file.open("list_geometry.dat");

  TString dir = gSystem -> Getenv("VMCWORKDIR");
  TString dirGeom = dir + "/geometry/";
  TString geoManFileName = dirGeom + "geomSpiRIT_nl.man.root";
  auto file = new TFile(geoManFileName);
  auto top = (TGeoManager *) file -> Get("SpiRIT");
  listNode(top -> GetNode(0));
}
