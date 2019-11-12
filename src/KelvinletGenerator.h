#pragma once

#include "ofMain.h"
#include "DisplacedMesh.h"

typedef function<void(const vector<glm::vec3>&, const vector<glm::vec3>&)> KelvinletGenerator;

KelvinletGenerator kelvinletGenerator(const shared_ptr<DisplacedMesh>& mesh);
