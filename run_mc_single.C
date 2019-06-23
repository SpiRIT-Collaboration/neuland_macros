/**
 * Geant4 Simulation Macro
 *
 * - Geant4 settings can be found in SpiRITROOT/gconfig directory.
 *   By default, QGSP_BERT_HP_EMY physics list is used.
 *
 * - Geometry and material files can be found in SpiRITROOT/geometry
 *   directory. Make sure you run geomSpiRIT.C to create geometry
 *   file. Materials and Geant4 cuts are defined in media.geo. 
 *
 * - Default Field map is constant field. If you want to use field map
 *   file, 1) put SamuraiMap_0.5T.dat in SpiRIT/input directory and 
 *   2) set varialble 'useFieldMapFile' kTRUE. This setting may 
 *   cause longer simulation time.
 *
 * - By default, STEventGenGenerator is used for event generator.
 *   You may use other classes, but you will have to modify this code
 *   and it is not recommanded. The format of event generator file
 *   is described in STEventGenGenerator.hh.
 *
 * - How To Run
 *   In bash,
 *   > root 'run_mc.C("name", "event", kFALSE)'
 *   You do not need to open this file to change variables.
 *
 * - Varialbles
 *   @ name : Name of simulation. Just for indication.
 *   @ event : Name of event file name. 
 *   @ event : kTRUE to use constant field, with 0.5T. 
 *             kFALSE to use field map file 'SamuraiMap_0.5T.dat'.
 */
#include <sys/stat.h>
#include <sys/types.h>

/*************************************************************
* Functions needed to create directory if it is non-existance*
* Only call recursive_mkdir. There's no need to call rek_mkdir   *
**************************************************************/
void rek_mkdir(char* path);
void recursive_mkdir(const char* t_path);
//************End of Directory creation function****************

void AddIons(FairRunSim *fRun, TString event);

void run_mc_single
(
 TString name  = "singlemc_neuland_test", // XXX
 Int_t   nEvent = 100,
 TString inputDir = "",
 TString outputDir = "data/",
 Bool_t  useFieldMapFile = kTRUE,
 Int_t   nSplit=0
)
{
  TString pwd = "/home/ejungwoo/spiritroot/neuland_macros/configures/";
  TString generatorName = pwd + name + ".egen"; // XXX

  //////////////////////////////////////////////////////////
  //                                                      //
  //   In general, the below parts need not be touched.   //
  //                                                      //
  //////////////////////////////////////////////////////////
  
  
  // -----------------------------------------------------------------
  // Set environment
  TString workDir   = gSystem -> Getenv("VMCWORKDIR");
  TString geomDir   = workDir + "/geometry";
  TString g4ConfDir = workDir + "/gconfig_neuland"; // XXX
  TString dataDir   = workDir + "/neuland_macros/data/";

  if(outputDir.IsNull())
    outputDir = dataDir;

  TString version; {
    TString spiritroot = TString(gSystem -> Getenv("VMCWORKDIR"))+"/";
    TString vfname = spiritroot + "VERSION.compiled";
    std::ifstream vfile(vfname);
    vfile >> version;
    vfile.close();
  }
  name = name + "." + version;

  // -----------------------------------------------------------------
  // Set file names
  TString outputFile = outputDir + name + ".mc.root"; 
  TString outParFile = outputDir + name + ".params.root";
  TString loggerFile = outputDir + "log_" + name + ".mc.txt";

  // ----------------------------------------------------------------
  // Create directories for all the output if they are non-existent
  recursive_mkdir(outputFile.Data());
  recursive_mkdir(outParFile.Data());
  recursive_mkdir(loggerFile.Data());


  // -----------------------------------------------------------------
  // MC initialization
  gRandom -> SetSeed(0);
  gSystem -> Setenv("GEOMPATH",   geomDir.Data());
  gSystem -> Setenv("CONFIG_DIR", g4ConfDir.Data());


  // -----------------------------------------------------------------
  // Set FairRunSim
  FairRunSim* fRun = new FairRunSim();
  fRun -> SetName("TGeant4");
  fRun -> SetOutputFile(outputFile);
  fRun -> SetGenerateRunInfo(kFALSE);
  fRun -> SetMaterials("media.geo");
//  AddIons(fRun, event);


  // -----------------------------------------------------------------
  // Logger
  FairLogger *fLogger = FairLogger::GetLogger();
  fLogger -> SetLogFileName(loggerFile); 
  fLogger -> SetLogToScreen(kTRUE); 
  fLogger -> SetLogToFile(kTRUE);
  fLogger -> SetLogVerbosityLevel("LOW");


  // -----------------------------------------------------------------
  // Geometry
  FairModule* fCave= new FairCave("CAVE");
  fCave -> SetGeometryFileName("cave_vacuum_wneuland.geo"); // XXX
  fRun -> AddModule(fCave);

  FairDetector* fSpiRIT = new STDetector("STDetector", kTRUE);
  fSpiRIT -> SetGeometryFileName("geomSpiRIT_nl.root"); // XXX
  fRun -> AddModule(fSpiRIT); 


  /*
  auto fEvent = new STSingleTrackGenerator();
  fEvent->ReadConfig(generatorName.Data());
  fEvent->SetVertexBegin(nSplit);
  //fEvent->SetCocktailEvent(300.);
  //fEvent->SetUniformRandomDirection(true);
  //fEvent->SetParticleList({2212, 211, -211, 1000010020, 1000010030});
  fEvent->SetNEvents(100);
  cout << fEvent->GetNEvents() << endl;
  */

  auto fEvent = new STEventGenGenerator(generatorName.Data());
  fEvent->SetPrimaryVertex(0.,-21.33,-.89);

  FairPrimaryGenerator* fGenerator = new FairPrimaryGenerator();
  fGenerator -> AddGenerator(fEvent);
  fRun -> SetGenerator(fGenerator);  


  // -----------------------------------------------------------------
  // Field
  if (useFieldMapFile) {
    STFieldMap *fField = new STFieldMap("samurai_field_map","A");
    fField -> SetPosition(0., -20.43, 58.);
    fRun -> SetField(fField);
  }
  else {
    FairConstField *fField = new FairConstField();
    fField -> SetField(0., 5., 0.);
    fField -> SetFieldRegion(-150, 150, -150, 150, -150, 150);
    fRun -> SetField(fField);
  }


  // -----------------------------------------------------------------
  // Set data base
  FairParRootFileIo* fMCPar = new FairParRootFileIo(kTRUE);
  fMCPar -> open(outParFile.Data());

  FairRuntimeDb* fDb = fRun -> GetRuntimeDb();
  fDb -> setOutput(fMCPar);
  fDb -> saveOutput();
  fDb -> print();


  // -----------------------------------------------------------------
  // Run initialization
  fRun -> SetStoreTraj(kTRUE);
  fRun -> Init();

  FairTrajFilter* trajFilter = FairTrajFilter::Instance();
  trajFilter -> SetStorePrimaries(kTRUE);
  trajFilter -> SetStoreSecondaries(kTRUE);


  // -----------------------------------------------------------------
  // Run
  fRun -> Run( nEvent==-1 ? fEvent->GetNEvents() : nEvent );


  // -----------------------------------------------------------------
  // Summary
  cout << endl << endl;
  cout << "Macro finished succesfully."  << endl << endl;
  cout << "- Output file : " << outputFile << endl << endl;



  gApplication -> Terminate();
}

void AddIons(FairRunSim *fRun, TString event)
{
  TString symbol[50] = {"H", "He", "Li", "Be", "B", "C", "N", "O", "F", "Ne",
                        "Na", "Mg", "Al", "Si", "P", "S", "Cl", "Ar", "K", "Ca",
                        "Sc", "Ti", "V", "Cr", "Mn", "Fe", "Co", "Ni", "Cu", "Zn",
                        "Ga", "Ge", "As", "Se", "Br", "Kr", "Rb", "Sr", "Y", "Zr",
                        "Nb", "Mo", "Tc", "Ru", "Rh", "Pd", "Ag", "Cd", "In", "Sn"};

  TString input_dir = gSystem -> Getenv("VMCWORKDIR");
  TString fGenFileName = input_dir + "/input/" + event;
  LOG(INFO) << "Opening EventGen file for adding ions: " << fGenFileName <<FairLogger::endl;

  ifstream fGenFile(fGenFileName.Data());
  if(!fGenFile.is_open())
    LOG(FATAL) << "Cannont open EventGen file: " << fGenFileName << FairLogger::endl;

  vector<Int_t> ions;
  Int_t nEvents, pdg, eventID, nTracks;
  Double_t tmp, b;

  fGenFile >> nEvents;

  for (Int_t iEvent = 0; iEvent < nEvents; iEvent++) {
    fGenFile >> eventID >> nTracks >> b;

    for(Int_t iTrack = 0; iTrack < nTracks; iTrack++) {
      fGenFile >> pdg >> tmp >> tmp >> tmp;
      if (pdg > 3000)
        ions.push_back(pdg);
    }
  }

  fGenFile.close();

  std::sort(ions.begin(), ions.end());
  ions.resize(std::distance(ions.begin(), std::unique(ions.begin(), ions.end())));

  for (Int_t iIon = 0; iIon < ions.size(); iIon++) {
    auto z = (ions.at(iIon)%10000000)/10000;
    auto a = (ions.at(iIon)%10000)/10;

    fRun -> AddNewIon(new FairIon(Form("%d", a) + symbol[z - 1], z, a, z));
  }
}

void rek_mkdir(char* path)
{
  char *sep = strrchr(path, '/' );
  if(sep != NULL) {
    *sep = 0;
    rek_mkdir(path);
    *sep = '/';
  }
  if( mkdir(path,0755) && errno != EEXIST )
    printf("error while trying to create '%s'\n%m\n",path ); 
}

void recursive_mkdir(const char* t_path)
{
    char* path = new char[strlen(t_path) + 1];
    strcpy(path, t_path);
    char *sep = strrchr(path, '/' );
    if(sep ) { 
       char *path0 = strdup(path);
       path0[ sep - path ] = 0;
       rek_mkdir(path0);
       free(path0);
    }

    delete[] path;
}
