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
		static void                                   trajMeasurementDistanceSquared(const TrajectoryMeasurement& lhs, const TrajectoryMeasurement& rhs, float& distanceSquared, float& dxSquared, float& dySquared);
		static float                                  trajMeasurementDistance(const TrajectoryMeasurement& lhs, const TrajectoryMeasurement& rhs);
		static void                                   trajMeasurementDistance(const TrajectoryMeasurement& lhs, const TrajectoryMeasurement& rhs, float& distance, float& dx, float& dy);
		static reco::VertexCollection::const_iterator findClosestVertexToTrack(const reco::TrackRef& track, const edm::Handle<reco::VertexCollection>& vertexCollectionHandle);
		static float                                  getClosestOtherTrackDistanceByLooping(const TrajectoryMeasurement& measurement, edm::Handle<TrajTrackAssociationCollection>& trajTrackCollectionHandle);
		static void                                   getClosestOtherTrackDistanceByLooping(const TrajectoryMeasurement& measurement, edm::Handle<TrajTrackAssociationCollection>& trajTrackCollectionHandle, float& distance, float& dx, float& dy);
};

#endif