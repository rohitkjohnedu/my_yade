/* CWBoon */

#ifdef YADE_POTENTIAL_BLOCKS
#include "Buoyancy.hpp"
#include<pkg/dem/KnKsPBLaw.hpp>
#include<pkg/dem/ScGeom.hpp>

#include <ctime>
#include <cstdlib>
//using namespace std;




void Buoyancy::action(){


	FOREACH(const shared_ptr<Body>& b, *scene->bodies){
		if (!b) continue;
		if (b->isClump() == true) continue;
		 PotentialBlock* pb=static_cast<PotentialBlock*>(b->shape.get()); 
		
		if(!pb) continue;
			//std::cout<<"pb->vertexStruct.size(): "<<pb->vertexStruct.size()<<", pb->verticesCD.size(): "<<pb->verticesCD.size()<<endl;
		if(pb->isBoundary == true ){continue;}	
		State* state1 = b->state.get();
		double volume = getWaterVolume(pb,  state1, waterLevel, waterNormal);
		pb->waterVolume = volume;
		Vector3r buoyancyForce(0.0,0.0,-volume*1000.0*9.81);
		scene->forces.addForce(b->id,buoyancyForce);
	}
	
	
}


double Buoyancy::getWaterVolume(const PotentialBlock*  s1, const State* state1, const double waterElevation, const Vector3r waterNormal){

	
	int edgeNo = s1->edgeStruct.size();
	int vertexNo =s1->verticesCD.size();
	int faceNo = s1->planeStruct.size();
	vector<int> vertexIDunderwater; vector<int> vertexIDabovewater;
	Vector3r waterLevelNormal = state1->ori.conjugate()*waterNormal;
	
	Vector3r globalPtOnWater = waterNormal*waterLevel;
	Vector3r localPtOnWater = state1->ori.conjugate()*(globalPtOnWater-state1->pos);
	double waterLevelHeight = waterLevelNormal.dot(localPtOnWater);
	
	
	for (int i=0; i<vertexNo; i++){
		Vector3r vertex = state1->pos+state1->ori*s1->verticesCD[i]; //s1->verticesCD[i]; //
		double distFromWaterLevel = waterNormal.x()*vertex.x() + waterNormal.y()*vertex.y() + waterNormal.z()*vertex.z()  - waterLevel;
		//double distFromWaterLevel = waterLevelNormal.x()*vertex.x() + waterLevelNormal.y()*vertex.y() + waterLevelNormal.z()*vertex.z()  - waterLevelHeight;
		if (distFromWaterLevel < 0.0){ /* Below waterLevel */
			vertexIDunderwater.push_back(i);
		}else{
			vertexIDabovewater.push_back(i);
		}	
	}
	
	//std::cout<<"vertexIDunderwater.size(): "<<vertexIDunderwater.size()<<", vertexIDabovewater.size(): "<<vertexIDabovewater.size()<<endl;
	//std::cout<<"s1->vertexStruct.size(): "<<s1->vertexStruct.size()<<", s1->verticesCD.size(): "<<s1->verticesCD.size()<<endl;
	//std::cout<<" s1->vertexStruct[vertexIDunderwater[0]].planeID[0]: "<<s1->vertexStruct[vertexIDunderwater[0]].planeID[0]<<endl;
	vector<Vector3r> newIntersections;
	for(int i=0; i<vertexIDunderwater.size(); i++){
			//std::cout<<"veridunderwater: "<<vertexIDunderwater[i]<<", id1: "<<s1->vertexStruct[vertexIDunderwater[i]].planeID[0]<<", id2: "<<s1->vertexStruct[vertexIDunderwater[i]].planeID[1]<<", id3: "<<s1->vertexStruct[vertexIDunderwater[i]].planeID[2]<<endl;
		
		for(int j=0; j<vertexIDabovewater.size(); j++){
			
			//std::cout<<"veridabovewater: "<<vertexIDabovewater[j]<<", id1: "<<s1->vertexStruct[vertexIDabovewater[j]].planeID[0]<<", id2: "<<s1->vertexStruct[vertexIDabovewater[j]].planeID[1]<<", id3: "<<s1->vertexStruct[vertexIDabovewater[j]].planeID[2]<<endl;

			int sameCounter = 0;
			if ( s1->vertexStruct[vertexIDunderwater[i]].planeID[0] == s1->vertexStruct[vertexIDabovewater[j]].planeID[0]  ) { sameCounter++;}
			if ( s1->vertexStruct[vertexIDunderwater[i]].planeID[0] == s1->vertexStruct[vertexIDabovewater[j]].planeID[1]  ) { sameCounter++;}
			if ( s1->vertexStruct[vertexIDunderwater[i]].planeID[0] == s1->vertexStruct[vertexIDabovewater[j]].planeID[2]  ) { sameCounter++;}
			if ( s1->vertexStruct[vertexIDunderwater[i]].planeID[1] == s1->vertexStruct[vertexIDabovewater[j]].planeID[0]  ) { sameCounter++;}	
			if ( s1->vertexStruct[vertexIDunderwater[i]].planeID[1] == s1->vertexStruct[vertexIDabovewater[j]].planeID[1]  ) { sameCounter++;}
			if ( s1->vertexStruct[vertexIDunderwater[i]].planeID[1] == s1->vertexStruct[vertexIDabovewater[j]].planeID[2]  ) { sameCounter++;}
			if ( s1->vertexStruct[vertexIDunderwater[i]].planeID[2] == s1->vertexStruct[vertexIDabovewater[j]].planeID[0]  ) { sameCounter++;}
			if ( s1->vertexStruct[vertexIDunderwater[i]].planeID[2] == s1->vertexStruct[vertexIDabovewater[j]].planeID[1]  ) { sameCounter++;}
			if ( s1->vertexStruct[vertexIDunderwater[i]].planeID[2] == s1->vertexStruct[vertexIDabovewater[j]].planeID[2]  ) { sameCounter++;}
			Vector3r v1 = s1->verticesCD[vertexIDunderwater[i]];
			Vector3r v2 = s1->verticesCD[vertexIDabovewater[j]];
			if(sameCounter>=2 && (v1-v2).norm()>pow(10,-3) ){
					/* taken from Heliot */
					Vector3r vertex1 = s1->verticesCD[vertexIDunderwater[i]];
					Vector3r vertex2 = s1->verticesCD[vertexIDabovewater[j]];
					double d1 = waterLevelNormal.dot(vertex1) - waterLevelHeight;
					double d2 = waterLevelNormal.dot(vertex2) - waterLevelHeight;
					Vector3r intersection = fabs(d2/(d1-d2))*vertex1 + fabs(d1/(d1-d2))*vertex2;
					newIntersections.push_back(intersection); /* LOCAL COORDINATES*/
					
			}
			
		}	
			
	}
	if (newIntersections.size() == 0 && vertexIDabovewater.size() == 0 ){
		return s1->volume; 
	}else if(newIntersections.size() == 0 && vertexIDunderwater.size()==0 ){
		//std::cout<<"vertexIDabovewater.size(): "<<vertexIDabovewater.size()<<", vertexIDunderwater.size(): "<<vertexIDunderwater.size()<<endl;
		return 0.0;
	}	

	//std::cout<<"vertexIDunderwater.size(): "<<vertexIDunderwater.size()<<", vertexIDabovewater.size(): "<<vertexIDabovewater.size()<<", newIntersections: "<<newIntersections.size()<<endl;

	Vector3r partA (0,0,0);
	for (int j=0; j<vertexIDunderwater.size();j++){
		partA += s1->verticesCD[vertexIDunderwater[j]];		 
	}
	
	Vector3r partB (0,0,0);
	for (int j=0; j<newIntersections.size();j++){
		partB += newIntersections[j];		 
	}
	
	Vector3r pointInside = (partA + partB)/static_cast<double>(newIntersections.size()+vertexIDunderwater.size() );
	//std::cout<<"pointInside: "<<pointInside<<endl;
	vector<Vector3r> verticesOnPlane; double totalVolume = 0.0;
	
	for (int j=0; j<s1->a.size(); j++){
		if(verticesOnPlane.size()>0){
			verticesOnPlane.clear();
		}
		int oldVertices = 0; int newVertices = 0;
		Vector3r planeNormal = Vector3r(s1->a[j],s1->b[j],s1->c[j]);
		Vector3r oriNormal(0,0,1); //normal vector of x-y plane
		Vector3r crossProd = oriNormal.cross(planeNormal);
		Quaternionr Qp;
		Qp.w() = 1.0 + oriNormal.dot(planeNormal);
		Qp.x() = crossProd.x(); Qp.y() = crossProd.y();  Qp.z() = crossProd.z();
		Qp.normalize();
		if(crossProd.norm() < pow(10,-5)){
				Qp = Quaternionr::Identity();
		}
		for (int i=0; i<vertexIDunderwater.size();i++){		
			Vector3r vertex =s1->verticesCD[vertexIDunderwater[i]]; /*local coordinates*/
			double plane = s1->a[j]*vertex.x() + s1->b[j]*vertex.y()+s1->c[j]*vertex.z() - s1->d[j]-s1->r; 
			if( fabs(plane) < pow(10,-2) ){
				Vector3r rotatedCoord = Qp.conjugate()*vertex;
				//std::cout<<"rotatedCoord: "<<rotatedCoord<<", oriCoord: "<<vertex<<endl;
				verticesOnPlane.push_back(rotatedCoord);	
				oldVertices++;	
			}
		}
		for(int i=0; i< newIntersections.size(); i++){
			double plane = s1->a[j]*newIntersections[i].x() + s1->b[j]*newIntersections[i].y()+s1->c[j]*newIntersections[i].z() - s1->d[j]-s1->r; 
			if( fabs(plane) < pow(10,-2) ){
				Vector3r rotatedCoord = Qp.conjugate()*newIntersections[i];
				//std::cout<<"rotatedCoord: "<<rotatedCoord<<", oriCoord: "<<newIntersections[i]<<endl;
				verticesOnPlane.push_back(rotatedCoord);	
				newVertices++;
			}
		}
		if(verticesOnPlane.size() == 0 ){continue;}
		/* REORDER VERTICES counterclockwise positive*/
		vector<Vector3r> orderedVerticesOnPlane;
		int h = 0; int k = 1; int m =2;
		Vector3r pt1 = verticesOnPlane[h];
		Vector3r pt2 = verticesOnPlane[k];
		Vector3r pt3 = verticesOnPlane[m];
		orderedVerticesOnPlane.push_back(pt1);
		int counter = 1;
		while(counter<verticesOnPlane.size()){		
				
				while (m<verticesOnPlane.size()){	
					
					pt1 = verticesOnPlane[h];
				 	pt2 = verticesOnPlane[k];
					pt3 = verticesOnPlane[m];
					if (getSignedArea(pt1,pt2,pt3) < 0.0){
						/* clockwise means 3rd point is better than 2nd */ 
						k=m; /*3rd point becomes 2nd point */
					 	pt2 = verticesOnPlane[k];
						
					}/* else counterclockwise is good.  We need to find and see whether there is a point(3rd point) better than the 2nd point */
					/* advance m */
					m=m+1;
					while(m==h || m==k){
						m=m+1; 
					}
					//if(m>= verticesOnPlane.size() ) { m = m%verticesOnPlane.size(); }
				
				}
				//std::cout<<"h: "<<h<<", k :"<<k<<", m: "<<m<<endl;
				orderedVerticesOnPlane.push_back(pt2);	
				h=k;
				/* advance k */
				k=0;
				while(k==h ){
					k=k+1;
				}
				//if(k>= verticesOnPlane.size() ) { k = k%verticesOnPlane.size(); }
				/* advance m */
				m=0;
				while(m==h || m==k){
					m=m+1;
				}
				//if(m>= verticesOnPlane.size() ) { m = m%verticesOnPlane.size(); }
				counter++;
		}
		

		Eigen::MatrixXd vertexOnPlane(orderedVerticesOnPlane.size()+1,2);
		for(int i=0; i< orderedVerticesOnPlane.size(); i++){
			//std::cout<<"orderedVerticesOnPlane[i]: "<<i<<", "<<orderedVerticesOnPlane[i]<<endl;
			vertexOnPlane(i,0)=orderedVerticesOnPlane[i].x(); vertexOnPlane(i,1)=orderedVerticesOnPlane[i].y(); //vertexOnPlane(i,2)=orderedVerticesOnPlane[i].z();		
			//std::cout<<"vertexOnPlane0: "<<vertexOnPlane(i,0)<<", vertexOnPlane1: "<<vertexOnPlane(i,1)<<endl;
		}
		int lastEntry = orderedVerticesOnPlane.size();
		vertexOnPlane(lastEntry,0)=orderedVerticesOnPlane[0].x(); vertexOnPlane(lastEntry,1)=orderedVerticesOnPlane[0].y(); //vertexOnPlane(lastEntry,2)=orderedVerticesOnPlane[0].z();
		//std::cout<<"vertexOnPlane0: "<<vertexOnPlane(lastEntry,0)<<", vertexOnPlane1: "<<vertexOnPlane(lastEntry,1)<<endl;

		double det = getDet(vertexOnPlane);
		double area = 0.5*det; //(vertexOnPlane.determinant());
		double height = -1.0*( s1->a[j]*pointInside.x() + s1->b[j]*pointInside.y() + s1->c[j]*pointInside.z() - s1->d[j]-s1->r ); 
		double volume = 1.0/3.0*area*height;
		//std::cout<<"shape id"<<s1->id<<", oldVertices: "<<oldVertices<<", newVertices: "<<newVertices<<", plane No: "<<j<<", area: "<<area<<endl;
		totalVolume += volume;
		//std::cout<<"orderedVerticesOnPlane.size(): "<<orderedVerticesOnPlane.size()<<", volume: "<<volume<<", area: "<<area<<", height: "<<height<<endl;
		orderedVerticesOnPlane.clear();
	}
	//std::cout<<"totalVolume: "<<totalVolume<<endl;

	
	
	Vector3r planeNormal = Vector3r(waterLevelNormal.x(),waterLevelNormal.y(),waterLevelNormal.z());
	Vector3r oriNormal(0,0,1); //normal vector of x-y plane
	Vector3r crossProd = oriNormal.cross(planeNormal);
	Quaternionr Qp;
	Qp.w() = 1.0 + oriNormal.dot(planeNormal);
	Qp.x() = crossProd.x(); Qp.y() = crossProd.y();  Qp.z() = crossProd.z();
	Qp.normalize();
	if(crossProd.norm() < pow(10,-7)){
		Qp = Quaternionr::Identity();
	}
	vector<Vector3r> rotatedIntersections;
	for (int i=0; i<newIntersections.size(); i++){
		Vector3r rotatedCoord = Qp.conjugate()*newIntersections[i];
		rotatedIntersections.push_back(rotatedCoord);
	}
	vector<Vector3r> orderedVerticesOnPlane;
	int h = 0; int k = 1; int m =2;
	Vector3r pt1 = rotatedIntersections[h];
	Vector3r pt2 = rotatedIntersections[k];
	Vector3r pt3 = rotatedIntersections[m];
	orderedVerticesOnPlane.push_back(pt1);
	int counter = 1;
	while(counter<newIntersections.size()){		
				
				while (m<rotatedIntersections.size()){	
					
					pt1 = rotatedIntersections[h];
				 	pt2 = rotatedIntersections[k];
					pt3 = rotatedIntersections[m];
					if (getSignedArea(pt1,pt2,pt3) < 0.0){
						/* clockwise means 3rd point is better than 2nd */ 
						k=m; /*3rd point becomes 2nd point */
						pt2 = rotatedIntersections[k];
					}/* else counterclockwise is good.  We need to find and see whether there is a point(3rd point) better than the 2nd point */
					/* advance m */
					m=m+1;
					while(m==h || m==k){
						m=m+1;
					}
					//if(m>= verticesOnPlane.size() ) { m = m%verticesOnPlane.size(); }
				}
				orderedVerticesOnPlane.push_back(pt2);	
				h=k;
				/* advance k */
				k=0;
				while(k==h ){
					k=k+1;
				}
				//if(k>= verticesOnPlane.size() ) { k = k%verticesOnPlane.size(); }
				/* advance m */
				m=0;
				while(m==h || m==k){
					m=m+1;
				}
				//if(m>= verticesOnPlane.size() ) { m = m%verticesOnPlane.size(); }
				counter++;
	}
	Eigen::MatrixXd vertexOnPlane(newIntersections.size()+1,2);
	int lastEntry = orderedVerticesOnPlane.size();
	for(int j=0; j< orderedVerticesOnPlane.size(); j++){
		vertexOnPlane(j,0)=orderedVerticesOnPlane[j].x(); vertexOnPlane(j,1)=orderedVerticesOnPlane[j].y(); //vertexOnPlane(j,2)=orderedVerticesOnPlane[j].z();
		//std::cout<<"vertexOnPlane0: "<<vertexOnPlane(j,0)<<", vertexOnPlane1: "<<vertexOnPlane(j,1)<<endl;
	}
	vertexOnPlane(lastEntry,0)=orderedVerticesOnPlane[0].x(); vertexOnPlane(lastEntry,1)=orderedVerticesOnPlane[0].y(); //vertexOnPlane(lastEntry,2)=orderedVerticesOnPlane[0].z();
	//std::cout<<"vertexOnPlane0: "<<vertexOnPlane(lastEntry,0)<<", vertexOnPlane1: "<<vertexOnPlane(lastEntry,1)<<endl;

	double det = getDet(vertexOnPlane);
	double area = 0.5*det;
	double height = -1.0*(waterLevelNormal.x()*pointInside.x() + waterLevelNormal.y()*pointInside.y() + waterLevelNormal.z()*pointInside.z() - waterLevelHeight); 
	double volume = 1.0/3.0* area*height;
	//std::cout<<"shape id"<<s1->id<<", intersection area: "<<area<<endl;
	//std::cout<<"newIntersections.size(): "<<orderedVerticesOnPlane.size()<<", volume: "<<volume<<", area: "<<area<<", height: "<<height<<endl;
	orderedVerticesOnPlane.clear();
	totalVolume += volume;
	//std::cout<<"totalVolume: "<<totalVolume<<endl;
	newIntersections.clear();
	rotatedIntersections.clear();
	verticesOnPlane.clear();
	vertexIDunderwater.clear();
	vertexIDabovewater.clear();
	return totalVolume;
}


double Buoyancy::getSignedArea(const Vector3r pt1, const Vector3r pt2, const Vector3r pt3){ 
	/* if positive, counter clockwise, 2nd point makes a larger angle */
	/* if negative, clockwise, 3rd point makes a larger angle */ 
	
	Eigen::MatrixXd triangle(4,2);
	triangle(0,0) = pt1.x();  triangle(0,1) = pt1.y(); // triangle(0,2) = pt1.z(); 
	triangle(1,0) = pt2.x();  triangle(1,1) = pt2.y(); // triangle(1,2) = pt2.z(); 
	triangle(2,0) = pt3.x();  triangle(2,1) = pt3.y(); // triangle(2,2) = pt3.z(); 
	triangle(3,0) = pt1.x();  triangle(3,1) = pt1.y(); // triangle(3,2) = pt1.z(); 
	double determinant = getDet(triangle);
	return determinant; //triangle.determinant();
}


double Buoyancy::getDet(const Eigen::MatrixXd A){ 
	/* if positive, counter clockwise, 2nd point makes a larger angle */
	/* if negative, clockwise, 3rd point makes a larger angle */ 
	int rowNo = A.rows();  double firstTerm = 0.0; double secondTerm = 0.0;
	for(int i=0; i<rowNo-1; i++){
		firstTerm += A(i,0)*A(i+1,1);
		secondTerm += A(i,1)*A(i+1,0);
	}
	
	return firstTerm-secondTerm;
}


YADE_PLUGIN((Buoyancy));

#endif // YADE_POTENTIAL_BLOCKS
