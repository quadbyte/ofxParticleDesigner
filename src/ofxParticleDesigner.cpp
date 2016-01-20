//
//  ofxParticleDesigner.cpp
//
// Based on :
// https://github.com/llahiru/ofParticleDesigner
// https://github.com/sroske/ofxParticleEmitter
// https://github.com/71squared/ParticleEmitterDemo-ES2-ARC/tree/master/ParticleEmitterDemo-ES2-ARC/Particle%20Emitter


#include "ofxParticleDesigner.h"
#include "Poco/Base64Decoder.h"
#include "Poco/InflatingStream.h"
#include "Poco/StreamCopier.h"

// ------------------------------------------------------------------------
ofxParticleEmitter::ofxParticleEmitter()
{
  emitterType = kParticleTypeGravity;

  sourcePosition.x = sourcePosition.y = 0.0f;
  sourcePositionVariance.x = sourcePositionVariance.y = 0.0f;
  angle = angleVariance = 0.0f;
  speed = speedVariance = 0.0f;
  radialAcceleration = tangentialAcceleration = 0.0f;
  radialAccelVariance = tangentialAccelVariance = 0.0f;
  gravity.x = gravity.y = 0.0f;
  particleLifespan = particleLifespanVariance = 0.0f;
  startColor.r = startColor.g = startColor.b = startColor.a = 1.0f;
  startColorVariance.r = startColorVariance.g = startColorVariance.b = startColorVariance.a = 1.0f;
  finishColor.r = finishColor.g = finishColor.b = finishColor.a = 1.0f;
  finishColorVariance.r = finishColorVariance.g = finishColorVariance.b = finishColorVariance.a = 1.0f;
  startParticleSize = startParticleSizeVariance = 0.0f;
  finishParticleSize = finishParticleSizeVariance = 0.0f;
  maxParticles = 0.0f;
  particleCount = 0;
  emissionRate = 0.0f;
  emitCounter = 0.0f;
  elapsedTime = 0.0f;
  duration = -1;

  blendFuncSource = blendFuncDestination = 0;

  maxRadius = maxRadiusVariance = minRadiusVariance = minRadius = 0.0f;
  rotatePerSecond = rotatePerSecondVariance = 0.0f;

  active = useTexture = false;
  particleIndex = 0;

  texture = NULL;
  particles = NULL;
  lastUpdateTime = 0;
}
// ------------------------------------------------------------------------
ofxParticleEmitter::~ofxParticleEmitter() {
  cleanUp();
}
// ------------------------------------------------------------------------
void ofxParticleEmitter::cleanUp() {
  active = false;

  if (texture != NULL) {
    delete texture;
    texture = NULL;
  }

  if (particles != NULL) {
    delete particles;
    particles = NULL;
  }
}
// ------------------------------------------------------------------------
bool ofxParticleEmitter::loadFromXml(const std::string& filename) {
  // Clean up things
  cleanUp();

  ofxXmlSettings*	settings = new ofxXmlSettings();
  active = settings->loadFile(filename);
  if (active) {
    parseConfig(settings);
    setupArrays();
    setupShaders();
  }

  delete settings;
  return active;
}
// ------------------------------------------------------------------------
void ofxParticleEmitter::parseConfig(ofxXmlSettings*	settings)
{
  if (settings == NULL) {
    ofLog(OF_LOG_ERROR, "ofxParticleDesigner::parseConfig() - XML settings is invalid!");
    return;
  }

  settings->pushTag("particleEmitterConfig");

  std::string imageData     = settings->getAttribute("texture", "data", "");
  loadTextureFromEncodedData(imageData.c_str());

  emitterType = settings->getAttribute("emitterType", "value", emitterType);

  sourcePosition.x = settings->getAttribute("sourcePosition", "x", sourcePosition.x);
  sourcePosition.y = settings->getAttribute("sourcePosition", "y", sourcePosition.y);

  sourcePositionVariance.x = settings->getAttribute("sourcePositionVariance", "x", sourcePositionVariance.x);
  sourcePositionVariance.y = settings->getAttribute("sourcePositionVariance", "y", sourcePositionVariance.y);

  speed = settings->getAttribute("speed", "value", speed);
  speedVariance = settings->getAttribute("speedVariance", "value", speedVariance);
  
  particleLifespan = settings->getAttribute("particleLifeSpan", "value", particleLifespan);
  particleLifespanVariance = settings->getAttribute("particleLifespanVariance", "value", particleLifespanVariance);
  
  angle = settings->getAttribute("angle", "value", angle);
  angleVariance = settings->getAttribute("angleVariance", "value", angleVariance);

  gravity.x = settings->getAttribute("gravity", "x", gravity.x);
  gravity.y = settings->getAttribute("gravity", "y", gravity.y);

  radialAcceleration = settings->getAttribute("radialAcceleration", "value", radialAcceleration);
  tangentialAcceleration = settings->getAttribute("tangentialAcceleration", "value", tangentialAcceleration);
  radialAccelVariance = settings->getAttribute("radialAccelVariance", "value", radialAccelVariance);
  tangentialAccelVariance = settings->getAttribute("tangentialAccelVariance", "value", tangentialAccelVariance);

  startColor.r = settings->getAttribute("startColor", "red", startColor.r);
  startColor.g = settings->getAttribute("startColor", "green", startColor.g);
  startColor.b = settings->getAttribute("startColor", "blue", startColor.b);
  startColor.a = settings->getAttribute("startColor", "alpha", startColor.a);

  startColorVariance.r = settings->getAttribute("startColorVariance", "red", startColorVariance.r);
  startColorVariance.g = settings->getAttribute("startColorVariance", "green", startColorVariance.g);
  startColorVariance.b = settings->getAttribute("startColorVariance", "blue", startColorVariance.b);
  startColorVariance.a = settings->getAttribute("startColorVariance", "alpha", startColorVariance.a);

  finishColor.r	= settings->getAttribute("finishColor", "red", finishColor.r);
  finishColor.g	= settings->getAttribute("finishColor", "green", finishColor.g);
  finishColor.b	= settings->getAttribute("finishColor", "blue", finishColor.b);
  finishColor.a	= settings->getAttribute("finishColor", "alpha", finishColor.a);

  finishColorVariance.r	= settings->getAttribute("finishColorVariance", "red", finishColorVariance.r);
  finishColorVariance.g	= settings->getAttribute("finishColorVariance", "green", finishColorVariance.g);
  finishColorVariance.b	= settings->getAttribute("finishColorVariance", "blue", finishColorVariance.b);
  finishColorVariance.a	= settings->getAttribute("finishColorVariance", "alpha", finishColorVariance.a);

  maxParticles = settings->getAttribute("maxParticles", "value", maxParticles);
  startParticleSize = settings->getAttribute("startParticleSize", "value", startParticleSize);
  startParticleSizeVariance = settings->getAttribute("startParticleSizeVariance", "value", startParticleSizeVariance);
  finishParticleSize = settings->getAttribute("finishParticleSize", "value", finishParticleSize);
  finishParticleSizeVariance = settings->getAttribute("finishParticleSizeVariance", "value", finishParticleSizeVariance);
  duration = settings->getAttribute("duration", "value", duration);
  blendFuncSource = settings->getAttribute("blendFuncSource", "value", blendFuncSource);
  blendFuncDestination = settings->getAttribute("blendFuncDestination", "value", blendFuncDestination);

  minRadius	= settings->getAttribute("minRadius", "value", minRadius);
  minRadiusVariance	= settings->getAttribute("minRadiusVariance", "value", minRadiusVariance);
  maxRadius	= settings->getAttribute("maxRadius", "value", maxRadius);
  maxRadiusVariance	= settings->getAttribute("maxRadiusVariance", "value", maxRadiusVariance);

  rotatePerSecond = settings->getAttribute("rotatePerSecond", "value", rotatePerSecond);
  rotatePerSecondVariance = settings->getAttribute("rotatePerSecondVariance", "value", rotatePerSecondVariance);

  rotationStart = settings->getAttribute("rotationStart", "value", rotationStart);
  rotationStartVariance = settings->getAttribute("rotationStartVariance", "value", rotationStartVariance);
  rotationEnd = settings->getAttribute("rotationEnd", "value", rotationEnd);
  rotationEndVariance = settings->getAttribute("rotationEndVariance", "value", rotationEndVariance);

  // Calculate the emission rate
  emissionRate = maxParticles / particleLifespan;
  
//  // Create a UIImage from the tiff data to extract colorspace and alpha info
//  UIImage *image = [UIImage imageWithData:tiffData];
//  CGImageAlphaInfo info = CGImageGetAlphaInfo(image.CGImage);
//  CGColorSpaceRef colorSpace = CGImageGetColorSpace(image.CGImage);
//  
//  // Detect if the image contains alpha data
//  BOOL hasAlpha = ((info == kCGImageAlphaPremultipliedLast) ||
//                   (info == kCGImageAlphaPremultipliedFirst) ||
//                   (info == kCGImageAlphaLast) ||
//                   (info == kCGImageAlphaFirst) ? YES : NO);
//  
//  // Detect if alpha data is premultiplied
//  BOOL premultiplied = colorSpace && hasAlpha;
//  
//  // Is opacity modification required
//  _opacityModifyRGB = NO;
//  if (blendFuncSource == GL_ONE && blendFuncDestination == GL_ONE_MINUS_SRC_ALPHA) {
//    if (premultiplied)
//      _opacityModifyRGB = YES;
//    else {
//      blendFuncSource = GL_SRC_ALPHA;
//      blendFuncDestination = GL_ONE_MINUS_SRC_ALPHA;
//    }
//  }

  
  _opacityModifyRGB = false;

}
// ------------------------------------------------------------------------
void ofxParticleEmitter::loadTextureFromEncodedData(const char* encodedData) {
  // Texture data is a actualy a .tiff file gziped and base64 encoded.

  // First decode base64
  std::istringstream encodedStream(encodedData);
  Poco::Base64Decoder decoder(encodedStream);
  std::string decodedString;
  int c = decoder.get();
  while (c != -1) { decodedString += char(c); c = decoder.get(); }

  // DEBUG WRITE GZIP TO FILE
  //  {
  //    ofBuffer buff2;
  //    buff2.set(decodedString.c_str(), decodedString.size());
  //    string filename = "texture.gzip";
  //    ofFile file(filename, ofFile::WriteOnly);
  //    file.writeFromBuffer(buff2);
  //    file.close();
  //  }

  // Second inflate gzip to get the tiff
  std::ostringstream unzippedStream;
  Poco::InflatingOutputStream
  gunzipper(unzippedStream, Poco::InflatingStreamBuf::STREAM_GZIP);
  gunzipper << decodedString;
  gunzipper.close();
  std::string tiffString = unzippedStream.str();

  // DEBUG WRITE TIFF TO FILE
  //  {
  //    ofBuffer buff2;
  //    buff2.set(tiffString.c_str(), tiffString.size());
  //    string filename = "texture.tiff";
  //    ofFile file(filename, ofFile::WriteOnly);
  //    file.writeFromBuffer(buff2);
  //    file.close();
  //  }

  // Load into an ofImage
  ofBuffer buff;
  buff.set(tiffString.c_str(), tiffString.size());
  texture = new ofImage();
  texture->load(buff);
  texture->setAnchorPercent(0.5f, 0.5f);
  texture->setUseTexture(true);
  
//  CGImageAlphaInfo info = CGImageGetAlphaInfo(textue.getPixel().getData());
//  CGColorSpaceRef colorSpace = CGImageGetColorSpace(image.CGImage);
//  
//  // Detect if the image contains alpha data
//  BOOL hasAlpha = ((info == kCGImageAlphaPremultipliedLast) ||
//                   (info == kCGImageAlphaPremultipliedFirst) ||
//                   (info == kCGImageAlphaLast) ||
//                   (info == kCGImageAlphaFirst) ? YES : NO);

}
// ------------------------------------------------------------------------
void ofxParticleEmitter::stop() {
  elapsedTime = 0;
  emitCounter = 0;
  particleCount = 0;
  active = false;
}

// ------------------------------------------------------------------------
// Particle Management
// ------------------------------------------------------------------------
bool ofxParticleEmitter::addParticle() {
  // If we have already reached the maximum number of particles then do nothing
  if ((particleCount == maxParticles) || !active) {
    return false;
  }

  // Take the next particle out of the particle pool we have created and initialize it
  Particle *particle = &particles[particleCount];
  initParticle(particle);

  // Increment the particle count
  particleCount++;

  // Return true to show that a particle has been created
  return true;
}
// ------------------------------------------------------------------------
void ofxParticleEmitter::initParticle(Particle* particle) {
  // Init the position of the particle.  This is based on the source position of the particle emitter
  // plus a configured variance.  The ofRandomf macro allows the number to be both positive
  // and negative
  particle->position.x = sourcePositionVariance.x * ofRandomf();
  particle->position.y = sourcePositionVariance.y * ofRandomf();

  // Init the direction of the particle.  The newAngle is calculated using the angle passed in and the
  // angle variance.
  float newAngle = DEG_TO_RAD * (angle + angleVariance * ofRandomf());

  // Create a new Vector2f using the newAngle
  ofVec2f vector = ofVec2f(cosf(newAngle), sinf(newAngle));

  // Calculate the vectorSpeed using the speed and speedVariance which has been passed in
  float vectorSpeed = speed + speedVariance * ofRandomf();

  // The particles direction vector is calculated by taking the vector calculated above and
  // multiplying that by the speed
  particle->direction =  vector * vectorSpeed;

  // Calculate the particles life span using the life span and variance passed in
  particle->timeToLive = MAX(0, (particleLifespan + particleLifespanVariance * ofRandomf()));

  float startRadius = maxRadius + maxRadiusVariance * ofRandomf();
  float endRadius = minRadius + minRadiusVariance * ofRandomf();

  // Set the default diameter of the particle from the source position
  particle->radius = startRadius;
  particle->radiusDelta = (endRadius - startRadius) / particle->timeToLive;
  particle->angle = DEG_TO_RAD * (angle + angleVariance * ofRandomf());
  particle->degreesPerSecond = DEG_TO_RAD * (rotatePerSecond + rotatePerSecondVariance * ofRandomf());

  particle->radialAcceleration = radialAcceleration + radialAccelVariance * ofRandomf();
  particle->tangentialAcceleration = tangentialAcceleration + tangentialAccelVariance * ofRandomf();

  // Calculate the particle size using the start and finish particle sizes
  float particleStartSize = startParticleSize + startParticleSizeVariance * ofRandomf();
  float particleFinishSize = finishParticleSize + finishParticleSizeVariance * ofRandomf();
  particle->particleSize = MAX(0, particleStartSize);
  if (particle->timeToLive) {
    particle->particleSizeDelta = (particleFinishSize - particleStartSize) / particle->timeToLive;
  }
  else {
    particle->particleSizeDelta = 0;
  }

  // Calculate the color the particle should have when it starts its life.  All the elements
  // of the start color passed in along with the variance are used to calculate the star color
  ofFloatColor start;
  start.r = (startColor.r + startColorVariance.r * ofRandomf());
  start.g = (startColor.g + startColorVariance.g * ofRandomf());
  start.b = (startColor.b + startColorVariance.b * ofRandomf());
  start.a = (startColor.a + startColorVariance.a * ofRandomf());

  // Calculate the color the particle should be when its life is over.  This is done the same
  // way as the start color above
  ofFloatColor end;
  end.r = (finishColor.r + finishColorVariance.r * ofRandomf());
  end.g = (finishColor.g + finishColorVariance.g * ofRandomf());
  end.b = (finishColor.b + finishColorVariance.b * ofRandomf());
  end.a = (finishColor.a + finishColorVariance.a * ofRandomf());

  // Calculate the delta which is to be applied to the particles color during each cycle of its
  // life.  The delta calculation uses the life span of the particle to make sure that the
  // particles color will transition from the start to end color during its life time.  As the game
  // loop is using a fixed delta value we can calculate the delta color once saving cycles in the
  // update method
  particle->color = start;
  if (particle->timeToLive) {
    particle->deltaColor.r = (end.r - start.r) / particle->timeToLive;
    particle->deltaColor.g = (end.g - start.g) / particle->timeToLive;
    particle->deltaColor.b = (end.b - start.b) / particle->timeToLive;
    particle->deltaColor.a = (end.a - start.a) / particle->timeToLive;
  }
  else {
    particle->deltaColor = 0;
  }

  // Calculate the rotation
  float startA = rotationStart + rotationStartVariance * ofRandomf();
  float endA = rotationEnd + rotationEndVariance * ofRandomf();
  particle->rotation = startA;
  particle->rotationDelta = (endA - startA) / particle->timeToLive;
}

// ------------------------------------------------------------------------
// Update
// ------------------------------------------------------------------------
void ofxParticleEmitter::update() {
  if (!active) {
    return;
  }
  
  float aDelta = 0;

  if (!lastUpdateTime) {
    lastUpdateTime = ofGetElapsedTimef();
  }
  else {
    float time = ofGetElapsedTimef();
    aDelta = time - lastUpdateTime;
    lastUpdateTime = time;
  }
  

  // If the emitter is active and the emission rate is greater than zero then emit particles
  if (active && emissionRate) {
    float rate = 1.0f / emissionRate;

     if (particleCount < maxParticles) {
      emitCounter += aDelta;
      while (particleCount < maxParticles && emitCounter > rate) {
        addParticle();
        emitCounter -= rate;
      }
    }

    elapsedTime += aDelta;
    if (duration != -1 && duration < elapsedTime) {
      stop();
    }
  }

  // Reset the particle index before updating the particles in this emitter
  particleIndex = 0;

  // Loop through all the particles updating their location and color
  while (particleIndex < particleCount) {
    // Get the particle for the current particle index
    Particle *currentParticle = &particles[particleIndex];

    // FIX 1
    // Reduce the life span of the particle
    currentParticle->timeToLive -= aDelta;

    // If the current particle is alive then update it
    if(currentParticle->timeToLive > 0) {

      // If maxRadius is greater than 0 then the particles are going to spin otherwise
      // they are effected by speed and gravity
      if (emitterType == kParticleTypeRadial) {

        // FIX 2
        // Update the angle of the particle from the sourcePosition and the radius.  This is only
        // done of the particles are rotating
        currentParticle->angle += currentParticle->degreesPerSecond * aDelta;
        currentParticle->radius += currentParticle->radiusDelta * aDelta;

        ofVec2f tmp;
        tmp.x = -cosf(currentParticle->angle) * currentParticle->radius;
        tmp.y = -sinf(currentParticle->angle) * currentParticle->radius;
        currentParticle->position = tmp;

        if (currentParticle->radius < minRadius) {
          currentParticle->timeToLive = 0;
        }
      }
      else {
        ofVec2f tmp, radial, tangential;

        if (currentParticle->position.x || currentParticle->position.y) {
          radial = currentParticle->position;
          radial.normalize();
        }

        tangential = radial;
        radial *= currentParticle->radialAcceleration;

        float newy = tangential.x;
        tangential.x = -tangential.y;
        tangential.y = newy;
        tangential *= currentParticle->tangentialAcceleration;

        tmp = radial + tangential + gravity;
        tmp *= aDelta;
        currentParticle->direction += tmp;

        tmp = currentParticle->direction * aDelta;
        currentParticle->position += tmp;
      }

      // Update the particles color
      currentParticle->color += currentParticle->deltaColor * aDelta;

      // Update the particle size
      currentParticle->particleSize += currentParticle->particleSizeDelta * aDelta;
      currentParticle->particleSize = MAX(0, currentParticle->particleSize);

      // Update the rotation of the particle
      currentParticle->rotation += currentParticle->rotationDelta * aDelta;

      // As we are rendering the particles as quads, we need to define 6 vertices for each particle
      GLfloat halfSize = currentParticle->particleSize * 0.5f;
      
      // If a rotation has been defined for this particle then apply the rotation to the vertices that define
      // the particle
      if (currentParticle->rotation) {
        float x1 = -halfSize;
        float y1 = -halfSize;
        float x2 = halfSize;
        float y2 = halfSize;
        float x = currentParticle->position.x;
        float y = currentParticle->position.y;
        float r = currentParticle->rotation * DEG_TO_RAD;
        float cr = cosf(r);
        float sr = sinf(r);
        float ax = x1 * cr - y1 * sr + x;
        float ay = x1 * sr + y1 * cr + y;
        float bx = x2 * cr - y1 * sr + x;
        float by = x2 * sr + y1 * cr + y;
        float cx = x2 * cr - y2 * sr + x;
        float cy = x2 * sr + y2 * cr + y;
        float dx = x1 * cr - y2 * sr + x;
        float dy = x1 * sr + y2 * cr + y;
        
        quads[particleIndex].bl.vertex.x = ax;
        quads[particleIndex].bl.vertex.y = ay;
        quads[particleIndex].bl.color = currentParticle->color;
        
        quads[particleIndex].br.vertex.x = bx;
        quads[particleIndex].br.vertex.y = by;
        quads[particleIndex].br.color = currentParticle->color;
        
        quads[particleIndex].tl.vertex.x = dx;
        quads[particleIndex].tl.vertex.y = dy;
        quads[particleIndex].tl.color = currentParticle->color;
        
        quads[particleIndex].tr.vertex.x = cx;
        quads[particleIndex].tr.vertex.y = cy;
        quads[particleIndex].tr.color = currentParticle->color;
      } else {
        // Using the position of the particle, work out the four vertices for the quad that will hold the particle
        // and load those into the quads array.
        quads[particleIndex].bl.vertex.x = currentParticle->position.x - halfSize;
        quads[particleIndex].bl.vertex.y = currentParticle->position.y - halfSize;
        quads[particleIndex].bl.color = currentParticle->color;
        
        quads[particleIndex].br.vertex.x = currentParticle->position.x + halfSize;
        quads[particleIndex].br.vertex.y = currentParticle->position.y - halfSize;
        quads[particleIndex].br.color = currentParticle->color;
        
        quads[particleIndex].tl.vertex.x = currentParticle->position.x - halfSize;
        quads[particleIndex].tl.vertex.y = currentParticle->position.y + halfSize;
        quads[particleIndex].tl.color = currentParticle->color;
        
        quads[particleIndex].tr.vertex.x = currentParticle->position.x + halfSize;
        quads[particleIndex].tr.vertex.y = currentParticle->position.y + halfSize;
        quads[particleIndex].tr.color = currentParticle->color;
      }

      
      // Update the particle counter
      particleIndex++;
    }
    else {
      // As the particle is not alive anymore replace it with the last active particle
      // in the array and reduce the count of particles by one.  This causes all active particles
      // to be packed together at the start of the array so that a particle which has run out of
      // life will only drop into this clause once
      if (particleIndex != particleCount - 1) {
        particles[particleIndex] = particles[particleCount - 1];
      }
      particleCount--;
    }
  }

}

// ------------------------------------------------------------------------
// Render
// ------------------------------------------------------------------------
void ofxParticleEmitter::draw() {
  if (!active) {
    return;
  }
  
  ofPushMatrix();
  ofTranslate(sourcePosition.x, sourcePosition.y, 0);
  drawParticles();
  ofPopMatrix();
}
// ------------------------------------------------------------------------
void ofxParticleEmitter::drawParticles() {
//  
//  glColor3f(1.0f, 1.0f, 1.0f);
//  glRectf(0, 0, 30, 30);

  
  //return;
//  ofEnablePointSprites();
////  ofDisableArbTex();
//
 // particleShader.begin();

//  //glClear(GL_COLOR_BUFFER_BIT);
//  
//  // Bind to the texture that has been loaded for this particle system
    texture->getTexture().bind();
  
  // Bind to the verticesID VBO and popuate it with the necessary vertex, color and texture informaiton
  glBindBuffer(GL_ARRAY_BUFFER, verticesID);
  
  // Using glBufferSubData means that a copy is done from the quads array to the buffer rather than recreating the buffer which
  // would be an allocation and copy. The copy also only takes over the number of live particles. This provides a nice performance
  // boost.
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(ParticleQuad) * particleIndex, quads);
  
  // Make sure that the vertex attributes we are using are enabled. This is a cheap call so OK to do each frame
  glEnableVertexAttribArray(inPositionAttrib);
  glEnableVertexAttribArray(inColorAttrib);
  glEnableVertexAttribArray(inTexCoordAttrib);
  
  // Configure the vertex pointer which will use the currently bound VBO for its data
  glVertexAttribPointer(inPositionAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedColoredVertex), 0);
  glVertexAttribPointer(inColorAttrib, 4, GL_FLOAT, GL_FALSE, sizeof(TexturedColoredVertex), (GLvoid*) offsetof(TexturedColoredVertex, color));
  glVertexAttribPointer(inTexCoordAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedColoredVertex), (GLvoid*) offsetof(TexturedColoredVertex, texture));
  
  // Set the blend function based on the configuration
  glBlendFunc(blendFuncSource, blendFuncDestination);
  
  // Set the opacity modifier shader parameter
  glUniform1i(u_opacityModifyRGB, _opacityModifyRGB);
  
  // Now that all of the VBOs have been used to configure the vertices, pointer size and color
  // use glDrawArrays to draw the points
  glDrawElements(GL_TRIANGLES, particleIndex * 6, GL_UNSIGNED_SHORT, indices);
  
  // Unbind the current VBO
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  texture->getTexture().unbind();
  
  //particleShader.end();
  
  /*
  
  // clear vbo
  vboMesh.clear();
  
  // Copy all vertices to the billboard
//  vboMesh.addVertices(mesh.getVerticesPointer(), mesh.getNumVertices());
  //vboMesh.getNormals().resize(mesh.getNumVertices(), ofVec3f(0));
  vboMesh.setUsage(GL_DYNAMIC_DRAW);
  vboMesh.setMode(OF_PRIMITIVE_POINTS);
  
  
  
  // Draw points
  shader.begin();
//  ofEnablePointSprites();
  texture.getTexture().bind();
    //billboards.draw();
  texture.getTexture().unbind();
  //ofDisablePointSprites();
  shader.end();
  
*/
  

  
#if 0
  // BLENDING MODE ARE NOT PROPERLY RESPECTED FOR NOW
  ofEnableAlphaBlending();
  glBlendFunc(blendFuncSource, blendFuncDestination);
  for (int i=0; i<particleCount; i++) {
    Particle *p = &particles[i];
    ofSetColor(p->color.r*255.0f, p->color.g*255.0f, p->color.b*255.0f, p->color.a*255.0f);

    ofPushMatrix();
      ofTranslate(p->position.x, -p->position.y);
      ofRotate(p->rotation);
      texture->draw(0, 0, p->particleSize, p->particleSize);
    ofPopMatrix();
  }
#endif

  
  
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
void ofxParticleEmitter::setupArrays() {
  // Allocate the memory necessary for the particle emitter arrays
  particles = (Particle*)malloc(sizeof(Particle) * maxParticles);
  quads = (ParticleQuad*)calloc(sizeof(ParticleQuad), maxParticles);
  indices = (GLushort*)calloc(sizeof(GLushort), maxParticles * 6);

  // Set up the indices for all particles. This provides an array of indices into the quads array that is used during
  // rendering. As we are rendering quads there are six indices for each particle as each particle is made of two triangles
  // that are each defined by three vertices.
  for( int i=0; i<maxParticles; i++) {
    indices[i*6+0] = i*4+0;
    indices[i*6+1] = i*4+1;
    indices[i*6+2] = i*4+2;
    
    indices[i*6+5] = i*4+2;
    indices[i*6+4] = i*4+3;
    indices[i*6+3] = i*4+1;
  }
  
  // Set up texture coordinates for all particles as these will not change.
  for(int i=0; i<maxParticles; i++) {
    quads[i].bl.texture.x = 0;
    quads[i].bl.texture.y = 0;
    
    quads[i].br.texture.x = 1;
    quads[i].br.texture.y = 0;
    
    quads[i].tl.texture.x = 0;
    quads[i].tl.texture.y = 1;
    
    quads[i].tr.texture.x = 1;
    quads[i].tr.texture.y = 1;
  }
  
  // If one of the arrays cannot be allocated throw an assertion as this is bad
//  NSAssert(particles && quads, @"ERROR - ParticleEmitter: Could not allocate arrays.");
  
  // Generate the vertices VBO
  glGenBuffers(1, &verticesID);
  glBindBuffer(GL_ARRAY_BUFFER, verticesID);
  glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleQuad) * maxParticles, quads, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  
  // By default the particle emitter is active when created
  active = true;

  // Set the particle count to zero
  particleCount = 0;
  
  // Reset the elapsed time
  elapsedTime = 0;
}


void ofxParticleEmitter::setupShaders() {
  
  particleShader.load("shaders/particle");
  particleShader.printActiveAttributes();
  particleShader.printActiveUniforms();
  
  inPositionAttrib = 0;
  inColorAttrib = 1;
  inTexCoordAttrib = 2;

//  
//  particleShader.bindAttribute(inPositionAttrib, "inPosition");
//  particleShader.bindAttribute(inColorAttrib, "inColor");
//  particleShader.bindAttribute(inTexCoordAttrib, "inTexCoord");
//  
//  if (!particleShader.linkProgram()) {
//    exit(1);
//  }
  
  // Setup our uniform pointer indexes. This must be done after the program is linked and used as uniform indexes are allocated
  // dynamically by OpenGL
  textureUniform = particleShader.getUniformLocation("texture");
  MPMtxUniform = particleShader.getUniformLocation("MPMatrix");
  u_opacityModifyRGB = particleShader.getUniformLocation("u_opacityModifyRGB");
  
  
  // Set up the projection matrix to be used for the emitter, this only needs to be set once
  // which is why it is in here.
  ofMatrix4x4 projectionMatrix;
  projectionMatrix.makeOrthoMatrix(0, ofGetWidth(), 0, ofGetHeight(), 1, -1);
  glUniformMatrix4fv(MPMtxUniform, 1, GL_FALSE, (float*)projectionMatrix._mat);
  
#if 0
//  // Compile the shaders we are using...
//  particleShader = [[GLSLProgram alloc] initWithVertexShaderFilename:@"particleVertexShader"
//                                              fragmentShaderFilename:@"particleFragmentShader"];
  
  // ... and add the attributes the shader needs for the vertex position, color and texture st information
//  [particleShader addAttribute:@"inPosition"];
//  [particleShader addAttribute:@"inColor"];
//  [particleShader addAttribute:@"inTexCoord"];
  
  // Check to make sure everything lnked OK
  if (![particleShader link]) {
    NSLog(@"Linking failed");
    NSLog(@"Program log: %@", [particleShader programLog]);
    NSLog(@"Vertex log: %@", [particleShader vertexShaderLog]);
    NSLog(@"Fragment log: %@", [particleShader fragmentShaderLog]);
    particleShader = nil;
    exit(1);
  }
  
  // Setup the index pointers into the shader for our attributes
  inPositionAttrib = [particleShader attributeIndex:@"inPosition"];
  inColorAttrib = [particleShader attributeIndex:@"inColor"];
  inTexCoordAttrib = [particleShader attributeIndex:@"inTexCoord"];
  
  // Tell OpenGL we want to use this program. This must be done before we set up the pointer indexes for the uniform values
  // we need
  [particleShader use];
  
  // Setup our uniform pointer indexes. This must be done after the program is linked and used as uniform indexes are allocated
  // dynamically by OpenGL
  textureUniform = [particleShader uniformIndex:@"texture"];
  MPMtxUniform = [particleShader uniformIndex:@"MPMatrix"];
  u_opacityModifyRGB = [particleShader uniformIndex:@"u_opacityModifyRGB"];
#endif
}