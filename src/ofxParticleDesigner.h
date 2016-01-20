//
//  ofxParticleDesigner.h
//
//

#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"

// ------------------------------------------------------------------------
// Structures
// ------------------------------------------------------------------------

// Particle type
enum kParticleTypes {
  kParticleTypeGravity = 0,
  kParticleTypeRadial = 1
};

//// Structure that holds the location and size for each point sprite
//typedef struct {
//  float x;
//  float y;
//  float size;
//  float rotation;
//  ofFloatColor color;
//} PointSprite;

typedef struct {
  ofVec2f vertex;
  ofVec2f texture;
  ofFloatColor color;
} TexturedColoredVertex;

typedef struct {
  TexturedColoredVertex bl;
  TexturedColoredVertex br;
  TexturedColoredVertex tl;
  TexturedColoredVertex tr;
} ParticleQuad;

// Structure used to hold particle specific information
typedef struct {
  ofVec2f position;
  ofVec2f direction;
  ofFloatColor color;
  ofFloatColor deltaColor;
  float rotation;
  float rotationDelta;
  float radialAcceleration;
  float tangentialAcceleration;
  float radius;
  float radiusDelta;
  float angle;
  float degreesPerSecond;
  float particleSize;
  float particleSizeDelta;
  float timeToLive;
} Particle;

// ------------------------------------------------------------------------
// ofxParticleEmitter
// ------------------------------------------------------------------------
class ofxParticleEmitter {

public:
  ofxParticleEmitter();
  ~ofxParticleEmitter();

  bool loadFromXml(const std::string& filename);
  void update();
  void draw();
  void cleanUp();
  void setPosition(ofVec2f p) { sourcePosition = p; }
  int getParticlesCount() { return particleCount; }
  int getMaxParticles() { return maxParticles; }
  
protected:
  void parseConfig(ofxXmlSettings* settings);
  void setupArrays();
  void setupShaders();
  void loadTextureFromEncodedData(const char* encodedData);

  void stop();
  bool addParticle();
  void initParticle(Particle* particle);
  void drawParticles();
  

private:
  int         emitterType;
  ofVec2f     sourcePosition, sourcePositionVariance;
  float       angle, angleVariance;
  float       speed, speedVariance;
  float       radialAcceleration, tangentialAcceleration;
  float       radialAccelVariance, tangentialAccelVariance;
  ofVec2f     gravity;
  float       particleLifespan, particleLifespanVariance;
  ofFloatColor			startColor, startColorVariance;
  ofFloatColor			finishColor, finishColorVariance;
  float       startParticleSize, startParticleSizeVariance;
  float       finishParticleSize, finishParticleSizeVariance;
  int         maxParticles;
  int         particleCount;


  float     rotationStart, rotationStartVariance;
  float     rotationEnd, rotationEndVariance;
  float     duration;
  int       blendFuncSource, blendFuncDestination;

  // Particle ivars only used when a maxRadius value is provided.  These values are used for
  // the special purpose of creating the spinning portal emitter
  float			maxRadius;						// Max radius at which particles are drawn when rotating
  float			maxRadiusVariance;				// Variance of the maxRadius
  float     minRadiusVariance;      // Variance of the minRadius
  float			minRadius;						// Radius from source below which a particle dies
  float			rotatePerSecond;				// Number of degrees to rotate a particle around the source position per second
  float			rotatePerSecondVariance;		// Variance in degrees for rotatePerSecond

  float			emissionRate;
  float			emitCounter;
  float			elapsedTime;

  bool			active, useTexture;
  int       particleIndex;	// Stores the number of particles that are going to be rendered

  Particle*     particles;		// Array of particles that hold the particle emitters particle details
  ofImage*      texture;
  float         lastUpdateTime;
  
  
  
  //
  
  ofShader particleShader;
  	ParticleQuad *quads;        // Array holding quad information for each particle;
  
  
      bool _opacityModifyRGB;
  GLuint verticesID;			// Holds the buffer name of the VBO that stores the color and vertices info for the particles

    GLushort *indices;          // Array holding an index reference into an array of quads for rendering  
  GLuint inPositionAttrib,   // Shader program attributes and uniforms
  inColorAttrib,
  inTexCoordAttrib,
  textureUniform,
  MPMtxUniform,
  u_opacityModifyRGB;

  
};
