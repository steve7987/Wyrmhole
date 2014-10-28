#include "trackmanager.h"

TrackManager::TrackManager(){

}

TrackManager::~TrackManager(){

}

bool TrackManager::Initialize(int seed, char * tubedatafile){
	srand(seed);
	std::ostringstream seedss;
	seedss << "Current seed is: " << seed;
	textDump(seedss.str());

	//load data file
	ifstream fin;
	char input;
	fin.open(tubedatafile);
	if (fin.fail()){
		textDump("failed to open tube data file");
		return false;
	}
	for (fin.get(input); input != '"'; fin.get(input));
	//read texture file
	char texfile[128];
	int j = 0;
	for (fin.get(input); input != '"'; fin.get(input)){
		texfile[j] = input;
		j++;
	}
	texfile[j] = '\0';
	WCHAR wtex[128];
	MultiByteToWideChar(CP_ACP, 0, texfile, -1,  wtex, 128);
	//read numerical data
	for (fin.get(input); input != ':'; fin.get(input));
	int numSegments;
	fin >> numSegments;
	for (fin.get(input); input != ':'; fin.get(input));
	float minLength;
	fin >> minLength;
	for (fin.get(input); input != ':'; fin.get(input));
	float maxLength;
	fin >> maxLength;
	for (fin.get(input); input != ':'; fin.get(input));
	float turnAngle;
	fin >> turnAngle;
	for (fin.get(input); input != ':'; fin.get(input));
	fin >> trackRadius;
	for (fin.get(input); input != ':'; fin.get(input));
	float texrepeat;
	fin >> texrepeat;
	for (fin.get(input); input != ':'; fin.get(input));
	int tubesides;
	fin >> tubesides;
	for (fin.get(input); input != ':'; fin.get(input));
	int tubesegments;
	fin >> tubesegments;
	for (fin.get(input); input != ':'; fin.get(input));
	float randomness;
	fin >> randomness;
	for (fin.get(input); input != ':'; fin.get(input));
	int smoothingPasses;
	fin >> smoothingPasses;
	fin.close();


	//create tube
	Vector p1 = Vector(0,0,0);
	Vector p2 = Vector(randb(minLength, maxLength), 0, 0);
	Vector p3, p4;
	Quaternion beginrot = Quaternion(0, 0, 0, 1);
	float startdist = 0;
	for (int i = 0; i < numSegments; i++){
		//Vector d3 = Vector(randb(-1*maxDisplacement, maxDisplacement), randb(-1*maxDisplacement, maxDisplacement), randb(-1*maxDisplacement, maxDisplacement)) ;
		//p3 = p2 + d3 + (p2 - p1);
		Vector tangent = (p2 - p1)/(p2-p1).length();
		Quaternion tanrot = Quaternion(Vector(1, 0, 0), tangent);
		Quaternion p3rot = Quaternion(tanrot*Vector(0, 1, 0), randb(-1*turnAngle, turnAngle))*
						   Quaternion(tanrot*Vector(0, 0, 1), randb(-1*turnAngle, turnAngle));
		p3 = p2 + p3rot*tangent*randb(minLength, maxLength);



		//compute circle components
		Vector r = ((p1-p2).cross(p3-p2)).cross(p1-p2);  //vector in direction of p2 to center
		float radius = (p3-p2)*(p3-p2)*(r.length()) / (2*r*(p3-p2));
		Vector center = p2 + r/r.length()*radius;
		
		
		
		p4 = (p2 - center).cross((p3 - center)).cross(p3 - center);
		if ((p2-center).cross(p1-p2)*(p2-center).cross(p3-center) >= 0){
			p4 = -1*p4;
		}
		
		p4 = p4 / p4.length();
		p4 = p3 + p4 * randb(minLength, maxLength) / 2;
		TrackSegment * ts = new TrackSegment();
		if (!ts){
			textDump("unable to create track segment");
			return false;
		}
		if (!ts->Initialize(p1, p2, p3, p4, beginrot, startdist, trackRadius, texrepeat, tubesides, tubesegments, wtex, randomness, smoothingPasses)){
			textDump("unable to initialize track segment");
			return false;
		}
		startdist += ts->GetLength();
		segmentList.push_back(ts);
		p1 = p4;
		p2 = p4 + (p4 - p3);
		beginrot = Quaternion(ts->GetTangent(0), ts->GetTangent(ts->GetLength()))*beginrot;
		beginrot = beginrot/beginrot.length();
	}

	
	return true;
}

void TrackManager::Shutdown(){
	for (std::deque<TrackSegment*>::iterator it = segmentList.begin(); it != segmentList.end(); ++it){
		(*it)->Shutdown();
		delete (*it);
		(*it) = 0;
	}
}

Vector TrackManager::GetPoint(float distance){
	float distCovered = 0;
	for (std::deque<TrackSegment*>::iterator it = segmentList.begin(); it != segmentList.end(); ++it){
		if (distCovered + (*it)->GetLength() > distance){
			distance -= distCovered;
			return (*it)->GetPoint(distance);
		}
		distCovered += (*it)->GetLength();
	}
	return segmentList.back()->GetPoint(distance - distCovered + segmentList.back()->GetLength());
}

Vector TrackManager::GetTangent(float distance){
	float distCovered = 0;
	for (std::deque<TrackSegment*>::iterator it = segmentList.begin(); it != segmentList.end(); ++it){
		if (distCovered + (*it)->GetLength() > distance){
			distance -= distCovered;
			return (*it)->GetTangent(distance);
		}
		distCovered += (*it)->GetLength();
	}
	return segmentList.back()->GetTangent(distance - distCovered + segmentList.back()->GetLength());
}

Quaternion TrackManager::GetRotation(float distance){
	float distCovered = 0;
	for (std::deque<TrackSegment*>::iterator it = segmentList.begin(); it != segmentList.end(); ++it){
		if (distCovered + (*it)->GetLength() > distance){
			distance -= distCovered;
			return (*it)->GetRotation(distance);
		}
		distCovered += (*it)->GetLength();
	}
	return segmentList.back()->GetRotation(distance - distCovered + segmentList.back()->GetLength());
	
}

float TrackManager::GetLength(){
	return totalLength;
}

float TrackManager::GetSegmentRadius(float distance){
	float distCovered = 0;
	for (std::deque<TrackSegment*>::iterator it = segmentList.begin(); it != segmentList.end(); ++it){
		if (distCovered + (*it)->GetLength() > distance){
			distance -= distCovered;
			return (*it)->GetSegmentRadius(distance);
		}
		distCovered += (*it)->GetLength();
	}
	return segmentList.back()->GetSegmentRadius(distance - distCovered + segmentList.back()->GetLength());
}

float TrackManager::GetTrackRadius(){
	return trackRadius;
}

void TrackManager::Update(float distance, float t){
	float distCovered = 0;
	for (std::deque<TrackSegment*>::iterator it = segmentList.begin(); it != segmentList.end(); ++it){
		if (distCovered + (*it)->GetLength() > distance - trackRadius){  
			//reached correct point in track so render next two segments (- radius to avoid gaps)
			(*it)->Update(t);
			++it;
			if (it != segmentList.end()){
				(*it)->Update(t);
			}
			return;
		}
		distCovered += (*it)->GetLength();
	}
}

void TrackManager::Render(float distance){
	float distCovered = 0;
	for (std::deque<TrackSegment*>::iterator it = segmentList.begin(); it != segmentList.end(); ++it){
		if (distCovered + (*it)->GetLength() > distance - trackRadius){  
			//reached correct point in track so render next two segments (- radius to avoid gaps)
			(*it)->Render();
			++it;
			if (it != segmentList.end()){
				(*it)->Render();
			}
			return;
		}
		distCovered += (*it)->GetLength();
	}
}

std::list<Collidable*> * TrackManager::GetCollidables(float distance){
	float distCovered = 0;
	for (std::deque<TrackSegment*>::iterator it = segmentList.begin(); it != segmentList.end(); ++it){
		if (distCovered + (*it)->GetLength() > distance){
			distance -= distCovered;
			return (*it)->GetCollidables();
		}
		distCovered += (*it)->GetLength();
	}
	return segmentList.back()->GetCollidables();
}


Vector TrackManager::GetLightDirection(float distance){
	float distCovered = 0;
	for (std::deque<TrackSegment*>::iterator it = segmentList.begin(); it != segmentList.end(); ++it){
		if (distCovered + (*it)->GetLength() > distance){
			if ((*it)->GetSegment(distance - distCovered) == 3){  //in third segment, so advance to next segments circle start and go 400 into it
				++it;
				return -1 * (*it)->GetTangent((*it)->GetSegmentStart(2) + 400);
			}
			else if ((*it)->GetSegment(distance - distCovered) == 1){  //in first segment, so just go to circle segment and go 400 into it

				return -1 * (*it)->GetTangent((*it)->GetSegmentStart(2) + 400);
			}
			else {  //in circle portion check where 400 forward is
				if ((*it)->GetSegment(distance - distCovered + 400) == 2){  //still within circle
					return -1 * (*it)->GetTangent(distance - distCovered + 400);
				}
				else {  //need to advance to next segment
					float overshoot = distance - distCovered + 400 - (*it)->GetSegmentStart(3);
					++it;
					return -1 * (*it)->GetTangent((*it)->GetSegmentStart(2) + overshoot);
				}
			}
		}
		distCovered += (*it)->GetLength();
	}
	
	return -1*GetTangent(distance + 400);
}