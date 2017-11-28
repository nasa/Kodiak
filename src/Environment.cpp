#include "Environment.hpp"

using namespace kodiak;

const BBox kodiak::EmptyBBox;

const Environment &kodiak::EmptyEnvironment {EmptyBBox, EmptyBox, EmptyNamedBox};
