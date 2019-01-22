// local tools
#include "Root/TruthWriter.h"

// EDM things
#include "xAODMissingET/MissingETContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertex.h"

// AnalysisBase tool include(s):
#include "FourMomUtils/xAODP4Helpers.h"
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/tools/ReturnCheck.h"


// 3rd party includes
#include "TFile.h"
#include "H5Cpp.h"

// stl includes
#include <stdexcept>
#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <cassert>

//////////////////////////////
// simple options struct    //
//////////////////////////////
struct Options
{
  std::vector<std::string> files;
  bool verbose = false;
};
// simple options parser
Options get_options(int argc, char *argv[]);


/////////////////////////////////
// hard-coded global variables //
/////////////////////////////////

const int pdgId_Zp = 55;
const int pdgId_scalar = 54;
const int pdgId_Wpos = 24;
const int pdgId_Wneg = -24;


///////////////////////
// helper functions  //
///////////////////////

const xAOD::TruthParticle* GetFirstChainLink(const xAOD::TruthParticle* TruthPart);
bool IsInOutGoing(const xAOD::TruthParticle* P);


//////////////////
// main routine //
//////////////////
int main (int argc, char *argv[])
{
  const char* ALG = argv[0];
  Options opts = get_options(argc, argv);

  // set up xAOD basics
  RETURN_CHECK(ALG, xAOD::Init());
  xAOD::TEvent event(xAOD::TEvent::kClassAccess);

  // set up output file
  H5::H5File output("output.h5", H5F_ACC_TRUNC);
  
  TruthWriter truth_writer(output);

  // Loop over the specified files:
  for (std::string file_name: opts.files) {

    // Open the file:
    std::unique_ptr<TFile> ifile(TFile::Open(file_name.c_str(), "READ"));
    if ( ! ifile.get() || ifile->IsZombie()) {
      throw std::logic_error("Couldn't open file: " + file_name);
      return 1;
    }
    std::cout << "Opened file: " << file_name << std::endl;

    // Connect the event object to it:
    RETURN_CHECK(ALG, event.readFrom(ifile.get()));

    // Loop over its events:
    const unsigned long long entries = event.getEntries();
    std::cout << "got " << entries << " entries" << std::endl;
    for (unsigned long long entry = 0; entry < entries; ++entry) {

      // Print some status
      if ( ! (entry % 1000)) {
        std::cout << "Processing " << entry << "/" << entries << "\n";
      }

      // Load the event:
      bool ok = event.getEntry(entry) >= 0;
      if (!ok) throw std::logic_error("getEntry failed");

      const xAOD::TruthParticleContainer *truthparticles = nullptr;
      RETURN_CHECK(ALG, event.retrieve(truthparticles, "TruthParticles"));


      // define pointers to truth particles
      const xAOD::TruthParticle *tp_Zp = nullptr;
      const xAOD::TruthParticle *tp_scalar = nullptr;
      const xAOD::TruthParticle *tp_Wpos = nullptr;
      const xAOD::TruthParticle *tp_Wneg = nullptr;

      for (const xAOD::TruthParticle *truthparticle : *truthparticles) {
        // search for Z' boson (use first chain link)
        if (!(tp_Zp) && truthparticle->pdgId() == pdgId_Zp) {
          tp_Zp = GetFirstChainLink(truthparticle);
          if(opts.verbose) {
            std::cout << "Z' boson:     "
                      << "pt   " << tp_Zp->pt() 
                      << " eta " << tp_Zp->eta() 
                      << " phi " << tp_Zp->phi() 
                      << " e   " << tp_Zp->e() 
                      << " m   " << tp_Zp->m() 
                      << std::endl;
          }
        }
        // search for scalar boson (use first chain link)
        if (!(tp_scalar) && truthparticle->pdgId() == pdgId_scalar) {
          tp_scalar = GetFirstChainLink(truthparticle);
          if(opts.verbose) {
            std::cout << "scalar boson: "
                      << "pt   " << tp_scalar->pt() 
                      << " eta " << tp_scalar->eta() 
                      << " phi " << tp_scalar->phi() 
                      << " e   " << tp_scalar->e() 
                      << " m   " << tp_scalar->m() 
                      << std::endl;
            }
        }
        // search for positively charged W boson (use first chain link)
        if (!(tp_Wpos) && truthparticle->pdgId() == pdgId_Wpos) {
          tp_Wpos = GetFirstChainLink(truthparticle);
          if(opts.verbose) {
            std::cout << "W+     boson: "
                      << "pt   " << tp_Wpos->pt() 
                      << " eta " << tp_Wpos->eta() 
                      << " phi " << tp_Wpos->phi() 
                      << " e   " << tp_Wpos->e() 
                      << " m   " << tp_Wpos->m() 
                      << std::endl;
          }
        }

        // search for negatively charged W boson (use first chain link)
        if (!(tp_Wneg) && truthparticle->pdgId() == pdgId_Wneg) {
          tp_Wneg = GetFirstChainLink(truthparticle);
          if(opts.verbose) {
            std::cout << "W-    boson: "
                      << "pt   " << tp_Wneg->pt() 
                      << " eta " << tp_Wneg->eta() 
                      << " phi " << tp_Wneg->phi() 
                      << " e   " << tp_Wneg->e() 
                      << " m   " << tp_Wneg->m() 
                      << std::endl;
          }
        }
      }

      // if W bosons were not found in TruthParticles container, try TruthBoson container
      const std::string label_bosoncontainer = "TruthBoson";
      if (event.contains<xAOD::TruthParticleContainer> (label_bosoncontainer)) {
        const xAOD::TruthParticleContainer *truthbosons = nullptr;
        RETURN_CHECK(ALG, event.retrieve(truthbosons, label_bosoncontainer));
        for (const xAOD::TruthParticle *truthparticle : *truthbosons) {
          // search for positively charged W boson (use first chain link)
          if (!(tp_Wpos) && truthparticle->pdgId() == pdgId_Wpos) {
            tp_Wpos = GetFirstChainLink(truthparticle);
            if(opts.verbose) {
              std::cout << "W+     boson: "
                        << "pt   " << tp_Wpos->pt() 
                        << " eta " << tp_Wpos->eta() 
                        << " phi " << tp_Wpos->phi() 
                        << " e   " << tp_Wpos->e() 
                        << " m   " << tp_Wpos->m() 
                        << std::endl;
            }
          }

          // search for negatively charged W boson (use first chain link)
          if (!(tp_Wneg) && truthparticle->pdgId() == pdgId_Wneg) {
            tp_Wneg = GetFirstChainLink(truthparticle);
            if(opts.verbose) {
              std::cout << "W-    boson: "
                        << "pt   " << tp_Wneg->pt() 
                        << " eta " << tp_Wneg->eta() 
                        << " phi " << tp_Wneg->phi() 
                        << " e   " << tp_Wneg->e() 
                        << " m   " << tp_Wneg->m() 
                        << std::endl;
            }
          }
        }
      }

      // get truth MET from event
      const xAOD::MissingETContainer *metcontainer = nullptr;
      RETURN_CHECK(ALG, event.retrieve(metcontainer, "MET_Truth"));
      const float met = ((*metcontainer)["NonInt"])->met();
      if(opts.verbose) {
        std::cout << "met: " << met << std::endl;
      }

      // calculate dR(WW)
      float dRWW = NAN;
      if (tp_Wpos && tp_Wneg)
        dRWW = xAOD::P4Helpers::deltaR(tp_Wpos, tp_Wneg, false);
      if(opts.verbose) {
        std::cout << "dR(WW): " << dRWW << std::endl;
      }

      // write output
      std::vector<const xAOD::TruthParticle*> result;
      result.push_back(tp_Zp);
      result.push_back(tp_scalar);
      result.push_back(tp_Wpos);
      result.push_back(tp_Wneg);

      truth_writer.write(result, met, dRWW);

    } // end event loop
  } // end file loop


  return 0;
}


// define the options parser
void usage(std::string name) {
  std::cout << "usage: " << name << " [-h]"
    " [-v (verbose)]"
    " <AOD>..." << std::endl;
}

Options get_options(int argc, char *argv[]) {
  Options opts;
  for (int argn = 1; argn < argc; argn++) {
    std::string arg(argv[argn]);
    if (arg == "-h") {
      usage(argv[0]);
      exit(1);
    } else if (arg == "-v") {
      opts.verbose = true;
    } else {
      opts.files.push_back(arg);
    }
  }
  if (opts.files.size() == 0) {
    usage(argv[0]);
    exit(1);
  }
  return opts;
}


// helper functions
const xAOD::TruthParticle* GetFirstChainLink(const xAOD::TruthParticle* TruthPart) {
    if (!TruthPart) {
        Error("GetFirstChainLink", "No truth particle was given");
        return nullptr;
    }
    if (TruthPart->hasProdVtx()) {
        const xAOD::TruthVertex* Vtx = TruthPart->prodVtx();
        for (size_t n = 0; n < Vtx->nIncomingParticles(); ++n) {
            const xAOD::TruthParticle* P = Vtx->incomingParticle(n);
            if (P && P != TruthPart && P->pdgId() == TruthPart->pdgId() && !(P == TruthPart) && !IsInOutGoing(P))
                return GetFirstChainLink(P);
        }
    }
    return TruthPart;
}

bool IsInOutGoing(const xAOD::TruthParticle* P) {
    if (!P) {
        Error("IsInOutGoing", "No truth particle was given");
        return false;
    }
    if (P->hasProdVtx()) {
        const xAOD::TruthVertex* Vtx = P->prodVtx();
        for (size_t n = 0; n < Vtx->nIncomingParticles(); ++n)
            if (P == (Vtx->incomingParticle(n))) return true;
    }
    if (P->hasDecayVtx()) {
        const xAOD::TruthVertex* Vtx = P->decayVtx();
        for (size_t n = 0; n < Vtx->nOutgoingParticles(); ++n)
            if (P == (Vtx->outgoingParticle(n))) return true;
    }
    return false;
}
