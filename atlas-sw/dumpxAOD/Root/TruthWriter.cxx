// this class's header
#include "TruthWriter.h"

// HDF5 things
#include "HDF5Utils/HdfTuple.h"
#include "H5Cpp.h"


TruthWriter::TruthWriter(H5::Group& output_group):
  m_truthmet(-1),
  m_truthdRWW(-1),
  m_truthparticles_idx(1),
  m_eventvariables_idx(1),
  m_writer_truthparticles(nullptr),
  m_writer_eventvariables(nullptr)
{
  H5Utils::VariableFillers fillers_truthparticles;
  fillers_truthparticles.add<float>("pt", [this]() -> float {
      size_t idx = this->m_truthparticles_idx.at(0);
      if (this->m_truthparticles.size() <= idx) return NAN;
      if (!(this->m_truthparticles.at(idx))) return NAN;
      return this->m_truthparticles.at(idx)->pt();
    });

  fillers_truthparticles.add<float>("eta", [this]() -> float {
      size_t idx = this->m_truthparticles_idx.at(0);
      if (this->m_truthparticles.size() <= idx) return NAN;
      if (!(this->m_truthparticles.at(idx))) return NAN;
      return this->m_truthparticles.at(idx)->eta();
    });

  fillers_truthparticles.add<float>("phi", [this]() -> float {
      size_t idx = this->m_truthparticles_idx.at(0);
      if (this->m_truthparticles.size() <= idx) return NAN;
      if (!(this->m_truthparticles.at(idx))) return NAN;
      return this->m_truthparticles.at(idx)->phi();
    });

  fillers_truthparticles.add<float>("m", [this]() -> float {
      size_t idx = this->m_truthparticles_idx.at(0);
      if (this->m_truthparticles.size() <= idx) return NAN;
      if (!(this->m_truthparticles.at(idx))) return NAN;
      return this->m_truthparticles.at(idx)->m();
    });

  fillers_truthparticles.add<int>("pdgId", [this]() -> int {
      size_t idx = this->m_truthparticles_idx.at(0);
      if (this->m_truthparticles.size() <= idx) return 0;
      if (!(this->m_truthparticles.at(idx))) return 0;
      return this->m_truthparticles.at(idx)->pdgId();
    });


  H5Utils::VariableFillers fillers_eventvariables;
  fillers_eventvariables.add<float>("truthmet", [this]() -> float {
      return this->m_truthmet;
    });

  fillers_eventvariables.add<float>("truthdRWW", [this]() -> float {
      return this->m_truthdRWW;
    });

  m_writer_truthparticles = new H5Utils::WriterXd(output_group, "truthparticles", fillers_truthparticles, {4});
  m_writer_eventvariables = new H5Utils::WriterXd(output_group, "eventvariables", fillers_eventvariables, {1});
}


TruthWriter::~TruthWriter() {
  if (m_writer_truthparticles) m_writer_truthparticles->flush();
  if (m_writer_eventvariables) m_writer_eventvariables->flush();
  delete m_writer_truthparticles;
  delete m_writer_eventvariables;
}

void TruthWriter::write(const std::vector<const xAOD::TruthParticle*>& truthparticles, float truthmet, float truthdRWW) {
  m_truthparticles.clear();

  for (const auto& truthparticle: truthparticles) {
    m_truthparticles.push_back(truthparticle);
  }

  // truth met
  m_truthmet = truthmet;

  // delta R(WW)
  m_truthdRWW = truthdRWW;

  m_writer_truthparticles->fillWhileIncrementing(m_truthparticles_idx);
  m_writer_eventvariables->fillWhileIncrementing(m_eventvariables_idx);
}
