#include "ejungwoo.h"
using namespace ejungwoo;

#include <cmath>
using namespace std;

#include "TVector3.h"

TVector3 GetGlobalNeulandPosition(TVector3 local);

Double_t fZTarget = -8.9;
Double_t fZTarget_mm = -89.;
Double_t fDistNeuland = 856.105 + 28.0 + 50.56/2.; //[cm]
Double_t fAnglNeuland = 29.579;
Double_t fOffxNeuland = fDistNeuland * sin( fAnglNeuland*TMath::DegToRad() ); 
Double_t fOffyNeuland = 0.;   
Double_t fOffzNeuland = fDistNeuland * cos( fAnglNeuland*TMath::DegToRad() ) + fZTarget;

void draw_event(TString fileName = "data/singlemc_neuland_test.neulandSim.1816.da94a5c.mc.root")
{
  v(1);

  auto file = new TFile(fileName);
  auto tree = (TTree *) file -> Get("cbmsim");
  tree -> Print("toponly");

  TClonesArray *gtrackArray = nullptr;
  //TClonesArray *mcpointArray = nullptr;
  tree -> SetBranchAddress("GeoTracks",&gtrackArray);
  //tree -> SetBranchAddress("STMCPoint",&mcpointArray);

  bool drawFrame = true;
  bool drawEvent = true;
  bool drawGeoTrack = false;

  bool drawSummary = true;

  if (drawEvent) {
    gstat(0);
    //auto cut = "Entry$<20";
    auto cut = "";
    auto histzx = tp("neuland_test_zx",tree,"10*fX:10*fZ",cut,";#it{z} (mm);#it{x} (mm)",500,-500,9000,500,-800,6000);
    add(histzx);

    auto histzy = tp("neuland_test_zy",tree,"10*fY:10*fZ",cut,";#it{z} (mm);#it{x} (mm)",500,-500,9000,500,-1500,1500);
    addto(1,histzy);
  }

  if (drawGeoTrack)
  {
    auto numEvents = tree -> GetEntries();
    for (auto iEvent=0; iEvent<numEvents; ++iEvent)
    {
      tree -> GetEntry(iEvent);

      if (drawGeoTrack)
      {
        auto numTracks = gtrackArray -> GetEntries();
        Double_t x, y, z, t;

        for (auto iTrack=0; iTrack<1; ++iTrack)
        {
          auto gtrack = (TVirtualGeoTrack *) gtrackArray -> At(iTrack);
          auto npoints = gtrack -> GetNpoints();

          auto graphTop = new TGraph();

          graphTop -> SetLineColor(kRed);
          for (auto ipoint=0; ipoint<npoints; ++ipoint) {
            gtrack -> GetPoint(ipoint,x,y,z,t);
            graphTop -> SetPoint(graphTop->GetN(),10*z,10*x);
          }
          add(graphTop,TString(Form("trackTop_%d_%d;",iEvent,iTrack))+"samel");

          auto graphSide = new TGraph();
          graphSide -> SetLineColor(kBlue);
          for (auto ipoint=0; ipoint<npoints; ++ipoint) {
            gtrack -> GetPoint(ipoint,x,y,z,t);
            graphSide -> SetPoint(graphSide->GetN(),10*z,10*y);
          }
          addto(1,graphSide,TString(Form("trackSide_%d_%d;",iEvent,iTrack))+"samel");
        }
      }

    }
  }

  if (drawSummary)
  {
    auto hist_did = tp("detectorID",tree,"STMCPoint.fDetectorID","","",100,0,5010);
    hist_did -> SetMinimum(1);
    cv("y","detectorID");
    hist_did -> Draw();

    cc("tpc_top");
    tp("tpc_top",tree,"10*STMCPoint.fX:10*STMCPoint.fZ","","",1344,0,1344,864,-432,432) -> Draw("colz");

    cc("tpc_side");
    tp("tpc_side",tree,"10*STMCPoint.fY:10*STMCPoint.fZ","","",1344,0,1344,530,-530,0) -> Draw("colz");
  }

  if (drawFrame)
  {
    Double_t dwNl = 1250.;
    Double_t dzNl = 400.;
    Double_t xMaxNl = 0.;
    Double_t xMinNl = DBL_MAX;
    Double_t zMaxNl = 0.;
    Double_t zMinNl = DBL_MAX;

    Double_t dzTpc = 1344.;
    Double_t dxTpc = 432.;
    Double_t y1Tpc = -530.;
    Double_t y2Tpc = 0.;

    // neuland frame
    {
      auto frameNlTop = new TGraph();

      for (auto point :
          {TVector3(0,0,0),
          TVector3(-dwNl,0,0),
          TVector3(-dwNl,0,dzNl),
          TVector3(+dwNl,0,dzNl),
          TVector3(+dwNl,0,0),
          TVector3(0,0,0)})
      {
        auto gpoint = GetGlobalNeulandPosition(point);
        frameNlTop -> SetPoint(frameNlTop->GetN(), gpoint.Z(), gpoint.X());

        if (xMaxNl < gpoint.X()) xMaxNl = gpoint.X();
        if (xMinNl > gpoint.X()) xMinNl = gpoint.X();
        if (zMaxNl < gpoint.Z()) zMaxNl = gpoint.Z();
        if (zMinNl > gpoint.Z()) zMinNl = gpoint.Z();
      }
      add(frameNlTop, "neuland_frame_top;");


      auto frameNlSideOut = new TGraph();

      for (auto point :
         {TVector3(-dwNl,0,0),
          TVector3(-dwNl,-dwNl,0),
          TVector3(-dwNl,-dwNl,dzNl),
          TVector3(-dwNl,+dwNl,dzNl),
          TVector3(-dwNl,+dwNl,0),
          TVector3(-dwNl,0,0)})
      {
        auto gpoint = GetGlobalNeulandPosition(point);
        frameNlSideOut -> SetPoint(frameNlSideOut->GetN(), gpoint.Z(), gpoint.Y());
      }

      auto frameNlSideIn = new TGraph();
      frameNlSideIn -> SetLineStyle(2);

      for (auto point :
         {TVector3(+dwNl,0,0),
          TVector3(+dwNl,-dwNl,0),
          TVector3(+dwNl,-dwNl,dzNl),
          TVector3(+dwNl,+dwNl,dzNl),
          TVector3(+dwNl,+dwNl,0),
          TVector3(+dwNl,0,0)})
      {
        auto gpoint = GetGlobalNeulandPosition(point);
        frameNlSideIn -> SetPoint(frameNlSideIn->GetN(), gpoint.Z(), gpoint.Y());
      }

      addto(1,frameNlSideOut, "neuland_frame_side;");
      addto(1,frameNlSideIn, "neuland_frame_side;");
    }

    // tpc frame
    {
      auto frameTpcTop = new TGraph();

      for (auto point :
         {TVector3(-dxTpc,0,0),
          TVector3(-dxTpc,0,dzTpc),
          TVector3(+dxTpc,0,dzTpc),
          TVector3(+dxTpc,0,0),
          TVector3(-dxTpc,0,0)})
      {
        frameTpcTop -> SetPoint(frameTpcTop->GetN(), point.Z(), point.X());
      }
      add(frameTpcTop,"tpc_frame_top;");


      auto frameTpcSide = new TGraph();

      for (auto point :
         {TVector3(0,y1Tpc,dzTpc),
          TVector3(0,y2Tpc,dzTpc),
          TVector3(0,y2Tpc,0),
          TVector3(0,y1Tpc,0),
          TVector3(0,y1Tpc,dzTpc)})
      {
        frameTpcSide -> SetPoint(frameTpcSide->GetN(), point.Z(), point.Y());
      }

      addto(1,frameTpcSide,"tpc_frame_side;");
    }

    // guide lines
    {
      auto posNl0 = GetGlobalNeulandPosition(TVector3(0,0,0));
      auto posNlL = GetGlobalNeulandPosition(TVector3(-dwNl,0,0));
      auto posNlR = GetGlobalNeulandPosition(TVector3(+dwNl,0,0));

      auto idxGuidLineColor = 11;

      TGraph *graphGuide;
      graphGuide = new TGraph();
      graphGuide -> SetLineColor(colori(idxGuidLineColor));
      graphGuide -> SetLineStyle(1);
      graphGuide -> SetPoint(0,fZTarget_mm,0);
      graphGuide -> SetPoint(1,posNl0.Z(),posNl0.X());
      add(graphGuide,"reference_line_mid;");

      graphGuide = new TGraph();
      graphGuide -> SetLineColor(colori(idxGuidLineColor));
      graphGuide -> SetLineStyle(1);
      graphGuide -> SetPoint(0,fZTarget_mm,0);
      graphGuide -> SetPoint(1,posNlL.Z(),posNlL.X());
      add(graphGuide,"reference_line_left;");

      graphGuide = new TGraph();
      graphGuide -> SetLineColor(colori(idxGuidLineColor));
      graphGuide -> SetLineStyle(1);
      graphGuide -> SetPoint(0,fZTarget_mm,0);
      graphGuide -> SetPoint(1,posNlR.Z(),posNlR.X());
      add(graphGuide,"reference_line_right;");

      auto angle0 = atan2(abs(posNl0.X()),posNl0.Z()-fZTarget_mm)*TMath::RadToDeg();
      auto tt0 = new TLatex(posNl0.Z(), posNl0.X(),Form("%.1f^{#circ}",angle0));
      tt0 -> SetTextAlign(12);
      tt0 -> SetTextFont(fDefaultFont);
      add(tt0,"reference_text_mid;");

      auto angle1 = atan2(abs(posNlR.X()),posNlR.Z()-fZTarget_mm)*TMath::RadToDeg();
      auto tt1 = new TLatex(posNlR.Z(), posNlR.X(),Form("%.1f^{#circ}",angle1));
      tt1 -> SetTextAlign(31);
      tt1 -> SetTextFont(fDefaultFont);
      add(tt1,"reference_text_right;");

      auto angle2 = atan2(abs(posNlL.X()),posNlL.Z()-fZTarget)*TMath::RadToDeg();
      auto tt2 = new TLatex(posNlL.Z(), posNlL.X(),Form("%.1f^{#circ}",angle2));
      tt2 -> SetTextAlign(13);
      tt2 -> SetTextFont(fDefaultFont);
      add(tt2,"reference_text_left;");
    }

  }

  auto cvs = cv3("neulandZX");
  draw("");

  cv3("neulandZY");
  draw(1,"");
}

TVector3 GetGlobalNeulandPosition(TVector3 local)
{
  TVector3 global = .1 * local;
  global.RotateY(fAnglNeuland*TMath::Pi()/180.);
  global +=  TVector3(fOffxNeuland, fOffyNeuland, fOffzNeuland);
  global = 10. * global;

  return global;
}
