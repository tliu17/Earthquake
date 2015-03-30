#include "App.h"
#include "config.h"
#include "EarthquakeDatabase.h"
#include <iostream>
#include <sstream>

const int PLAYBACK_WINDOW = 365 * 24 * 60 * 60;

using namespace std;

App::App(const GApp::Settings& settings) : GApp(settings) {
  renderDevice->setColorClearValue(Color3(0.1, 0.12, 0.15));
  renderDevice->setSwapBuffersAutomatically(true);
}


void App::onInit() {
  // Typical G3D::GApp initialization options
  createDeveloperHUD();
  debugWindow->setVisible(false);
  developerWindow->setVisible(false);
  developerWindow->cameraControlWindow->setVisible(false);
  showRenderingStats = false;
  m_cameraManipulator.reset();

  activeCamera()->setPosition(Vector3(0,0,2));
  activeCamera()->lookAt(Vector3(0,0,0), Vector3(0,1,0));
  activeCamera()->setFarPlaneZ(-100);

	shared_ptr<Light> light = Light::directional("downlight", Vector3(1,1,1), Color3(1.4, 1.3, 1.2));

	earth.reset(new Earth(activeCamera(), light));

  // Initialization for this Application
  // Note:  DATA_PATH is set in config.h
  eqd = EarthquakeDatabase(DATA_PATH);
  playbackScale = 86400;
  currentTime = eqd.getByIndex(eqd.getMinIndex()).getDate().asSeconds();
  font = GFont::fromFile("$G3D10DATA/font/arial.fnt");
  rotation = Matrix3::identity();
  playing = true;
  rotation_general = CoordinateFrame();
  rotation = rotation_general.rotation;
}


void App::onUserInput(UserInput *ui) {
  // Speedup or slowdown the playback
  if (ui->keyDown(GKey::LEFT)) {
    playbackScale /= 1.3;
  }
  if (ui->keyDown(GKey::RIGHT)) {
    playbackScale *= 1.3;
  }

  // Dolly the camera closer or farther away from the earth
  if (ui->keyDown(GKey::UP)) {
      Vector3 newCamPos = activeCamera()->frame().translation + Vector3(0,0,-0.01);
      if (newCamPos[2] > 1.2) {
        activeCamera()->setPosition(newCamPos);
      }
  }
  if (ui->keyDown(GKey::DOWN)) {
      Vector3 newCamPos = activeCamera()->frame().translation + Vector3(0,0,0.01);
      activeCamera()->setPosition(newCamPos);
  }

  // Rotate the earth when the user clicks and drags the mouse
  if (ui->keyDown(GKey::LEFT_MOUSE)) {
    // TODO: Update the "rotation" matrix based on how the user has dragged the mouse
    // Note: You can get mouse movement since the last frame with:  Vector2 dxy = ui->mouseDXY();
    Vector2 dxy = ui->mouseDXY();
    Vector3 dxyz = Vector3(-dxy.x, dxy.y, 0);

    //perpendecular axis to rotate about
    Vector3 rotAxis = dxyz.cross(Vector3(0,0,-1));
 
    if (!rotAxis.isZero()) {
        rotAxis.unit();
        Matrix3 this_rotation = Matrix3::fromAxisAngle(rotAxis, -rotAxis.magnitude()/100);
        rotation = this_rotation * rotation_general.rotation;
    }
  }
}


void App::onSimulation(RealTime rdt, SimTime sdt, SimTime idt) {
  GApp::onSimulation(rdt, sdt, idt);

  rotation_general = CoordinateFrame(rotation);

  // Advance the current time and loop back to the start if time is past the last earthquake
  currentTime += playbackScale * rdt;
  if (currentTime > eqd.getByIndex(eqd.getMaxIndex()).getDate().asSeconds()) {
    currentTime = eqd.getByIndex(eqd.getMinIndex()).getDate().asSeconds();
  }
  if (currentTime < eqd.getByIndex(eqd.getMinIndex()).getDate().asSeconds()) {
    currentTime = eqd.getByIndex(eqd.getMaxIndex()).getDate().asSeconds();
  }
}



void App::onGraphics3D(RenderDevice* rd, Array<shared_ptr<Surface> >& surface3D) {
  rd->clear();
  rd->setShadeMode(RenderDevice::SHADE_SMOOTH);


  //draws the lines forming the triangle
  //rd->setRenderMode(RenderDevice::RENDER_WIREFRAME);
  //turn off the front/back speficication of image
  //rd->setCullFace(CullFace::NONE);


  // Set a rotation matrix to apply when drawing the earth and earthquakes
  rd->pushState();

  rd->setObjectToWorldMatrix(rotation_general);

  // TODO: Draw the earth by implementing the draw function inside the Earth class
  earth->draw(rd);

  // Draw earthquakes
  int start = eqd.getIndexByDate(Date(currentTime - PLAYBACK_WINDOW));
  int end = eqd.getIndexByDate(Date(currentTime));
  for (int x=start; x<end; x++) {

    Earthquake e = eqd.getByIndex(x);

    // TODO: Draw earthquake e

    double longitude = e.getLongitude();
    double latitude = e.getLatitude();

    //magnitude are apprx. from 6.0 to 8.0
    double magnitude = e.getMagnitude()/100;

    //in lat and lon passed as radians
    Vector3 earthquake = earth->getPosition(toRadians(latitude), toRadians(longitude));

    //Earthquake is RED if magnitude is greater than 7.0, ORANGE if between 7.0 and 6.0, and GREEN if less than 6.0
    if (magnitude > 0.07) {

        Draw::sphere(Sphere(earthquake, magnitude), rd, Color4(1,0,0,0.8), Color4(1,0,0,0));

    } else if (magnitude <= 0.07 && magnitude > 0.06) {

        Draw::sphere(Sphere(earthquake, magnitude * 0.9), rd, Color4(1,0.5,0,0.8), Color4(1,0.5,0,0));

    } else {

        Draw::sphere(Sphere(earthquake, magnitude * 0.8), rd, Color4(0,1,0,0.8), Color4(0,1,0,0));

    }
  }

  rd->popState();

  // Call to make the GApp show the output of debugDraw
  drawDebugShapes();
}


void App::onGraphics2D(RenderDevice* rd, Array<Surface2D::Ref>& posed2D) {
  Surface2D::sortAndRender(rd, posed2D);
  Date d(currentTime);
  stringstream s;
  s << "Current date: " << d.getMonth()
    << "/" << d.getDay()
    << "/" << d.getYear()
    << "  " << d.getHour()
    << ":" << d.getMinute();
  const G3D::String g3dString = s.str().c_str();
  font->draw2D(rd, g3dString, Point2(30,30), 12, Color3::white());
}




