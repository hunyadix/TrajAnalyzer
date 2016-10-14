#include "../interface/TrajAnalyzer.h"

int TrajAnalyzer::trajectoryHasPixelHit(const edm::Ref<std::vector<Trajectory>>& trajectory)
{
	bool isBarrelPixelTrack = false;
	bool isEndcapPixelTrack = false;
	// Looping on the full track to check if we have pixel hits 
	// and to count the number of strip hits 
	for(auto& measurement: trajectory -> measurements())
	{
		// Check measurement validity
		if(!measurement.updatedState().isValid()) continue;
		auto hit = measurement.recHit();
		// Check hit quality
		if(!hit -> isValid()) continue;
		DetId det_id = hit -> geographicalId();
		uint32_t subdetid = (det_id.subdetId());
		// For saving the pixel hits
		if(subdetid == PixelSubdetector::PixelBarrel) isBarrelPixelTrack = true;
		if(subdetid == PixelSubdetector::PixelEndcap) isEndcapPixelTrack = true;
	}
	if(!isBarrelPixelTrack && !isEndcapPixelTrack)
	{
		return 0;
	}
	return 1;
}

int TrajAnalyzer::subdetidIsOnPixel(uint32_t subdetid)
{
	bool isPixelHit = false;
	isPixelHit |= subdetid == PixelSubdetector::PixelBarrel;
	isPixelHit |= subdetid == PixelSubdetector::PixelEndcap;
	return isPixelHit;
}

std::pair<float, float> TrajAnalyzer::getLocalXY(const TrajectoryMeasurement& measurement)
{
	std::pair<float, float> returnValue;
	static TrajectoryStateCombiner trajStateComb;
	TrajectoryStateOnSurface trajStateOnSurface = trajStateComb(measurement.forwardPredictedState(), measurement.backwardPredictedState());
	LocalPoint localPosition = trajStateOnSurface.localPosition();
	returnValue.first  = localPosition.x();
	returnValue.second = localPosition.y();
	return returnValue;
}

float TrajAnalyzer::trajMeasurementDistanceSquared(const TrajectoryMeasurement& lhs, const TrajectoryMeasurement& rhs)
{
	std::pair<float, float> lhsLocalXY = getLocalXY(lhs);
	std::pair<float, float> rhsLocalXY = getLocalXY(rhs);
	float dxHit = lhsLocalXY.first  - rhsLocalXY.first;
	float dyHit = lhsLocalXY.second - rhsLocalXY.second;
	float distanceSquared = dxHit * dxHit + dyHit * dyHit;
	return distanceSquared;
}
void TrajAnalyzer::trajMeasurementDistanceSquared(const TrajectoryMeasurement& lhs, const TrajectoryMeasurement& rhs, float& distanceSquared, float& dxSquared, float& dySquared)
{
	std::pair<float, float> lhsLocalXY = getLocalXY(lhs);
	std::pair<float, float> rhsLocalXY = getLocalXY(rhs);
	float dxHit = lhsLocalXY.first  - rhsLocalXY.first;
	float dyHit = lhsLocalXY.second - rhsLocalXY.second;
	dxSquared = dxHit * dxHit;
	dySquared = dyHit * dyHit;
	distanceSquared = dxSquared * dySquared;
}

float TrajAnalyzer::trajMeasurementDistance(const TrajectoryMeasurement& lhs, const TrajectoryMeasurement& rhs)
{
	return sqrt(trajMeasurementDistanceSquared(lhs, rhs));
}
void TrajAnalyzer::trajMeasurementDistance(const TrajectoryMeasurement& lhs, const TrajectoryMeasurement& rhs, float& distance, float& dx, float& dy)
{
	trajMeasurementDistanceSquared(lhs, rhs, distance, dx, dy);
	distance = sqrt(distance);
	dx       = sqrt(dx);
	dy       = sqrt(dy);
	if(dx = NOVAL_F || dy == NOVAL_F) distance = NOVAL_F;
}

reco::VertexCollection::const_iterator TrajAnalyzer::findClosestVertexToTrack(const reco::TrackRef& track, const edm::Handle<reco::VertexCollection>& vertexCollectionHandle)
{
	reco::VertexCollection::const_iterator closestVtx = vertexCollectionHandle -> end();
	// FIXME: This is awkward, change the minDistance to the distance of the first valid vertex
	double minDistance = 9999;
	for(reco::VertexCollection::const_iterator it = vertexCollectionHandle -> begin() ; it != vertexCollectionHandle -> end() ; ++it)
	{
		// Filter out invalid vertices
		if(!it -> isValid()) continue;
		double trkVtxD0 = track -> dxy(it -> position()) * -1.0;
		double trkVtxDz = track -> dz (it -> position());
		// Comparing squareroots should be quick enough, if required, change this to a comparison of squares
		double trkVtxDB = sqrt(trkVtxD0 * trkVtxD0 + trkVtxDz * trkVtxDz);
		if(trkVtxDB < minDistance)
		{
			minDistance = trkVtxDB;
			closestVtx=it;
		}
	}
	return closestVtx;
}


float TrajAnalyzer::getClosestOtherTrackDistanceByLooping(const TrajectoryMeasurement& measurement, edm::Handle<TrajTrackAssociationCollection>& trajTrackCollectionHandle)
{
	std::vector<TrajectoryMeasurement>::const_iterator closestMeasurementIt = trajTrackCollectionHandle -> begin() -> key -> measurements().begin();
	if(&*closestMeasurementIt == &measurement) ++closestMeasurementIt;
	double closestTrajMeasurementDistanceSquared = trajMeasurementDistanceSquared(measurement, *closestMeasurementIt);
	for(const auto& otherTrackKeypair: *trajTrackCollectionHandle)
	{
		const edm::Ref<std::vector<Trajectory>> otherTraj = otherTrackKeypair.key;
		for(auto otherTrajMeasurementIt = otherTraj -> measurements().begin(); otherTrajMeasurementIt != otherTraj -> measurements().end(); ++otherTrajMeasurementIt)
		{
			if(&*otherTrajMeasurementIt == &measurement) continue;
			float distanceSquared = trajMeasurementDistanceSquared(measurement, *otherTrajMeasurementIt);
			if(distanceSquared < closestTrajMeasurementDistanceSquared)
			{
				// closestMeasurementIt = otherTrajMeasurementIt;
				closestTrajMeasurementDistanceSquared = distanceSquared;
			}
		}
	}
	return sqrt(closestTrajMeasurementDistanceSquared);
}
void TrajAnalyzer::getClosestOtherTrackDistanceByLooping(const TrajectoryMeasurement& measurement, edm::Handle<TrajTrackAssociationCollection>& trajTrackCollectionHandle, float& distance, float& dx, float& dy)
{
	dx = NOVAL_F;
	dy = NOVAL_F;
	std::vector<TrajectoryMeasurement>::const_iterator closestMeasurementIt = trajTrackCollectionHandle -> begin() -> key -> measurements().begin();
	if(&*closestMeasurementIt == &measurement) ++closestMeasurementIt;
	double closestTrajMeasurementDistanceSquared = trajMeasurementDistanceSquared(measurement, *closestMeasurementIt);
	for(const auto& otherTrackKeypair: *trajTrackCollectionHandle)
	{
		const edm::Ref<std::vector<Trajectory>> otherTraj = otherTrackKeypair.key;
		for(auto otherTrajMeasurementIt = otherTraj -> measurements().begin(); otherTrajMeasurementIt != otherTraj -> measurements().end(); ++otherTrajMeasurementIt)
		{
			if(&*otherTrajMeasurementIt == &measurement) continue;
			float distanceSquared = trajMeasurementDistanceSquared(measurement, *otherTrajMeasurementIt);
			if(distanceSquared < closestTrajMeasurementDistanceSquared)
			{
				closestMeasurementIt = otherTrajMeasurementIt;
				closestTrajMeasurementDistanceSquared = distanceSquared;
			}
		}
	}
	trajMeasurementDistance(measurement, *closestMeasurementIt, distance, dx, dy);
}