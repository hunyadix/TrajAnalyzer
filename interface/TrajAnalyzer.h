#ifndef TRAJ_ANALYZER_H
#define TRAJ_ANALYZER_H

#include "TrackingTools/PatternTools/interface/TrajTrackAssociation.h"
#include "DataFormats/TrackReco/interface/Track.h"

class TrajAnalyzer
{
	private:
		TrajAnalyzer() = delete;
	public:
		~TrajAnalyzer();
		static int trajectoryHasPixelHit(const edm::Ref<std::vector<Trajectory>>& trajectory);
};

#endif