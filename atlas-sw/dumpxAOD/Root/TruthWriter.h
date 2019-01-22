#ifndef TRUTH_WRITER_H
#define TRUTH_WRITER_H

// forward declare HDF5 things
namespace H5 {
  class Group;
}
namespace H5Utils {
  class VariableFillers;
  class WriterXd;
}

// forward declare EDM things
namespace xAOD {
  class TruthParticle_v1;
  typedef TruthParticle_v1 TruthParticle;
}

// EDM includes
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"

class TruthWriter
{
public:
  TruthWriter(H5::Group& output_group);
  ~TruthWriter();

  TruthWriter(TruthWriter&) = delete;
  TruthWriter operator=(TruthWriter&) = delete;

  void write(const std::vector<const xAOD::TruthParticle*>& truthparticles, float truthmet, float truthdRWW);

private:
  std::vector<const xAOD::TruthParticle*> m_truthparticles;
  float m_truthmet;
  float m_truthdRWW;

  std::vector<size_t> m_truthparticles_idx;
  std::vector<size_t> m_eventvariables_idx;

  H5Utils::WriterXd* m_writer_truthparticles;
  H5Utils::WriterXd* m_writer_eventvariables;
};

#endif
