#include "tracksegment.h"

TrackSegment::TrackSegment(){
	tline = 0;
	ttube = 0;
}

TrackSegment::~TrackSegment(){

}


bool TrackSegment::Initialize(Vector p1, Vector p2, Vector p3, Vector p4, Quaternion beginrot, double startdist, double rad, 
							  double texturerepeat, int tubesides, int tubesegments, WCHAR * texturefile){
	this->p1 = p1;
	this->p2 = p2;
	this->p3 = p3;
	this->p4 = p4;
	this->startdist = startdist;
	this->beginrot = beginrot;
	//compute dist of two line segments
	s1 = sqrt((p1-p2)*(p1-p2));
	s3 = sqrt((p3-p4)*(p3-p4));
	//compute circle components
	Vector r = ((p1-p2).cross(p3-p2)).cross(p1-p2);  //vector in direction of p2 to center
	radius = (p3-p2)*(p3-p2)*(r.length()) / (2*r*(p3-p2));
	center = p2 + r/r.length()*radius;
	v1 = (p2 - center)/(p2 - center).length();
	v2 = v1.cross(p3 - center).cross(v1);
	v2 = v2/v2.length();
	maxAngle = acos(v1*(p3-center)/(p3-center).length());
	//TODO need to account for case angle is bigger than pi
	if ((p1-p2).cross(v1)*v1.cross(v2) <= 0){
		v2 = -1*v2;
		maxAngle = 2*PI - maxAngle;
		std::ostringstream oss;
		oss << "Vector on wrong side at dist: " << startdist;	
		oss << "\nRadius is: " << radius;
		textDump(oss.str());

	}
	s2 = maxAngle*radius;
	length = s1 + s2 + s3;

	tline = new TrackLine();
	if (!tline){
		textDump("error creating track line");
		return false;
	}
	if (!tline->Initialize(g_graphics->GetDevice(), this)){
		textDump("error initializing track line");
		return false;
	}
	ttube = new TrackTube();
	if (!ttube){
		textDump("error making track tube");
		return false;
	}
	if (!ttube->Initialize(g_graphics->GetDevice(), this, texturefile, rad, texturerepeat, startdist, tubesides, tubesegments)){
		textDump("error initializing ttube");
		return false;
	}
	//setup rings
	for (int i = 0; i < 5; i++){
		Ring * r = new Ring();
		Vector offset = Vector(randb(-1, 1), randb(-1,1), randb(-1,1));
		offset = offset / offset.length();
		offset = offset * randb(0, rad - 12);
		r->Initialize("./Assets/ring.txt", L"./Assets/rock.dds", GetPoint(i*length/5) + GetRotation(i*length/5)*offset, GetRotation(i*length/5), 10, 3);
		collidables.push_front(r);
	}
	//setup debris
	for (int i = 0; i < 10; i++){
		Debris * d = new Debris();
		Vector offset = Vector(randb(-1, 1), randb(-1,1), randb(-1,1));
		offset = offset / offset.length();
		offset = offset * randb(0, rad - 12);
		offset = Vector(0,0,0);
		d->Initialize("./Assets/sphere.txt", L"./Assets/rock.dds", GetPoint(i*length/10) + GetRotation(i*length/5)*offset, GetRotation(i*length/5), Vector(1,1,1));
		collidables.push_front(d);
	}
	return true;
}

void TrackSegment::Shutdown(){
	while (!collidables.empty()){
		Collidable * t = collidables.front();
		collidables.pop_front();
		if (t){
			t->Shutdown();
			delete t;
		}
	}

	if (ttube){
		ttube->Shutdown();
		delete ttube;
		ttube = 0;
	}
	
	if (tline){
		tline->Shutdown();
		delete tline;
		tline = 0;
	}
}


Vector TrackSegment::GetPoint(double distance){
	if (distance <= s1){
		double t = distance / s1;
		return (1-t)* p1 + t*p2;
	}
	distance -= s1;
	if (distance < s2){  //in circle part
		double t = distance / s2 *maxAngle;
		return (center + radius*(v1*cos(t) + v2*sin(t)));
	}
	distance -= s2;
	double t = distance / s3;
	return (1-t)*p3 + t*p4;
}

Vector TrackSegment::GetTangent(double distance){
	if (distance <= s1){
		return (p2 - p1)/(p2 - p1).length();
	}
	distance -= s1;
	if (distance < s2){  //in circle part
		double t = distance / s2 *maxAngle;
		return -1*v1*sin(t) + v2*cos(t);
	}
	return (p4 - p3) / (p4 - p3).length();
}

Quaternion TrackSegment::GetRotation(double distance){
	Vector tan = GetTangent(distance);
	Vector starttan = GetTangent(0);
	if ((tan - starttan).length() > 0.001){
		Quaternion update = Quaternion(starttan, tan);
		Quaternion ret = update*beginrot;
		ret = ret/ret.length();
		return ret;
	}
	return beginrot;
}

float TrackSegment::GetSegmentRadius(double distance){
	return radius;

	/*
	if (distance <= s1){
		return radius;
	}
	distance -= s1;
	if (distance < s2){  //in circle part
		return radius;
	}
	return -1;
	*/
}

double TrackSegment::GetLength(){
	return length;
}

void TrackSegment::Render(){
	//g_graphics->RenderObject(tline, SHADER_COLOR);
	g_graphics->RenderObject(ttube, SHADER_LIGHT);
	for (std::list<Collidable*>::iterator it = collidables.begin(); it != collidables.end();++it){
		if (*it){
			(*it)->Render(0);	
		}
	}
}

void TrackSegment::Update(float t){
	for (std::list<Collidable*>::iterator it = collidables.begin(); it != collidables.end(); ){
		if (*it){
			if (!(*it)->Update(t)){
				//need to remove this collidable
				Collidable * t = *it;
				it = collidables.erase(it);
				t->Shutdown();
				delete t;
			}
			else {
				//proceed to next item
				++it;
			}
		}
	}
}

std::list<Collidable*> * TrackSegment::GetCollidables(){
	return &collidables;
}