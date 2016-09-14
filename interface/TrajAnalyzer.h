#ifndef TRAJ_ANALYZER_H
#define TRAJ_ANALYZER_H

#include "TrackingTools/PatternTools/interface/TrajTrackAssociation.h"
#include "DataFormats/TrackReco/interface/Track.h"
// Position
#include "TrackingTools/TrackFitters/interface/TrajectoryStateCombiner.h"
// Vertex collection handling
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

class TrajAnalyzer
{
	private:
		TrajAnalyzer() = delete;
	public:
		~TrajAnalyzer();
		static int                                    trajectoryHasPixelHit(const edm::Ref<std::vector<Trajectory>>& trajectory);
		static int                                    subdetidIsOnPixel(uint32_t subdetid);
		static std::pair<float, float>                getLocalXY(const TrajectoryMeasurement& measurement);
		static float                                  trajMeasurementDistanceSquared(const TrajectoryMeasurement& lhs, const TrajectoryMeasurement& rhs);
		static float                                  trajMeasurementDistance(const TrajectoryMeasurement& lhs, const TrajectoryMeasurement& rhs);
		static reco::VertexCollection::const_iterator findClosestVertexToTrack(const reco::TrackRef& track, const edm::Handle<reco::VertexCollection>& vertexCollectionHandle);
};

#endif