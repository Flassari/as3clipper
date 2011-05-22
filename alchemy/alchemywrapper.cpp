/*******************************************************************************
*                                                                              *
* Author    :  Ari Þór H. Arnbjörnsson                                         *
* Date      :  22 May 2011                                                     *
* Website   :  http://flassari.is                                              *
*                                                                              *
* License:                                                                     *
* Use, modification & distribution is subject to Boost Software License Ver 1. *
* http://www.boost.org/LICENSE_1_0.txt                                         *
*                                                                              *
*******************************************************************************/

#include "clipper.cpp"

#include "AS3.h"


using namespace clipper;
using namespace std;

static AS3_Val clipPolygon(void* self, AS3_Val args) {

	AS3_Val asSubjectVertices;
	AS3_Val asClipVertices;
	int subjectVerticeCount, clipVerticeCount, clipTypeArg;
	// Get the function arguments (subjectVertices:Array, subjectVerticeCount:int, extractVertices:Array, extractVerticeCount:int, clipType:int)
	AS3_ArrayValue(
			args,
			"AS3ValType, IntType, AS3ValType, IntType, IntType",
			&asSubjectVertices, &subjectVerticeCount, &asClipVertices, &clipVerticeCount, &clipTypeArg
	);

	Polygon subjectPolygon(subjectVerticeCount / 2), clipPolygon(clipVerticeCount / 2);
	Polygons solution;

	// Populate the subject polygon
	for (int i = 0; i < subjectVerticeCount; i += 2) {
		subjectPolygon[i / 2] = IntPoint(
			AS3_IntValue(AS3_Get( asSubjectVertices, AS3_Int(i) )),
			AS3_IntValue(AS3_Get( asSubjectVertices, AS3_Int(i+1) ))
		);
	}

	// Populate the clip polygon
	for (int i = 0; i < clipVerticeCount; i += 2) {
		clipPolygon[i / 2] = IntPoint(
			AS3_IntValue(AS3_Get( asClipVertices, AS3_Int(i) )),
			AS3_IntValue(AS3_Get( asClipVertices, AS3_Int(i+1) ))
		);
	}

	// Create the AS3 return array
	AS3_Val returnArray = AS3_Array("");

	ClipType clipType;
	switch (clipTypeArg) {
		case 0: clipType = ctIntersection; break;
		case 1: clipType = ctUnion; break;
		case 2: clipType = ctDifference; break;
		case 3: clipType = ctXor; break;
		default: clipType = ctIntersection; break;
	}

	Clipper c;
	c.AddPolygon(subjectPolygon, ptSubject);
	c.AddPolygon(clipPolygon, ptClip);
	if (c.Execute(clipType, solution)) {
		for (int i = 0; i < (int)solution.size(); i++) {
			// Create a new AS3 array
			AS3_Val verticeArray = AS3_Array("");

			for (int j = 0; j < (int)solution[i].size(); j++) {
				// Push all the vertices into the array
				AS3_Set(verticeArray, AS3_Int(j * 2), AS3_Int(solution[i][j].X));
				AS3_Set(verticeArray, AS3_Int(j * 2 + 1), AS3_Int(solution[i][j].Y));
			}
			// Insert the array into the returnArray
			AS3_Set(returnArray, AS3_Int(i), verticeArray);
		}
	}

	// Cleanup
	AS3_Release(asSubjectVertices);
	AS3_Release(asClipVertices);

	return returnArray;
}

int main(void) {
	//define the methods exposed to ActionScript
	//typed as an ActionScript Function instance
	AS3_Val clipPolygonMethod = AS3_Function( NULL, clipPolygon );

	// construct an object that holds references to the functions
	AS3_Val result = AS3_Object( "clipPolygon: AS3ValType", clipPolygonMethod );

	// Release
	AS3_Release( clipPolygonMethod );

	// notify that we initialized -- THIS DOES NOT RETURN!
	AS3_LibInit( result );

	// should never get here!
	return 0;
}
