#include "MyGLWidget.h"

#include <iostream>

MyGLWidget::MyGLWidget (QWidget* parent) : QOpenGLWidget(parent)
{
  setFocusPolicy(Qt::ClickFocus);  // per rebre events de teclat
  scale = 1.0f;
  rot = 0.0f;
}

MyGLWidget::~MyGLWidget ()
{
  if (program != NULL)
    delete program;
}

void MyGLWidget::initializeGL ()
{
  // Cal inicialitzar l'ús de les funcions d'OpenGL
  initializeOpenGLFunctions();  

  

  glClearColor(0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
  
  glEnable(GL_DEPTH_TEST);

  carregaShaders();
  createBuffers();

  ini_camera();


  glm::vec3 punt_min = glm::vec3(0,-1,0);
  glm::vec3 punt_max = glm::vec3(0,2,0);
  capsa_contenidora(punt_min, punt_max);
}

void MyGLWidget::paintGL () 
{
  // Esborrem el frame-buffer
  //glClear (GL_COLOR_BUFFER_BIT);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Carreguem la transformació de model
  //modelTransform ();

  //ini_camera(); // o a initialize (?)

  // Activem el VAO per a pintar la caseta 
  glBindVertexArray (VAO_Homer);

  modelTransformHomer();

  // pintem
  //glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);

  glDrawArrays(GL_TRIANGLES, 0, homer.faces().size()*3);


  // Ara, despres de Homer, terra:

  glBindVertexArray(VAO_Terra);

  modelTransformTerra();

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glBindVertexArray (0);
}

void MyGLWidget::modelTransform () 
{
  // Matriu de transformació de model
  glm::mat4 transform (1.0f);
  transform = glm::scale(transform, glm::vec3(scale));
  transform = glm::rotate(transform, rot,glm::vec3(0, 1, 0)); // respecte Y
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::modelTransformHomer() {
  modelTransform();
}

// El terra de transformacio nomes te l'escalat
void MyGLWidget::modelTransformTerra() {
  glm::mat4 transform (1.0f);
  transform = glm::scale(transform, glm::vec3(scale));
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::resizeGL (int w, int h) 
{

  glViewport(0, 0, w, h);
  projectTransform();
}

void MyGLWidget::keyPressEvent(QKeyEvent* event) 
{
  makeCurrent();
  switch (event->key()) {
    case Qt::Key_S: { // escalar a més gran
      scale += 0.05;
      break;
    }
    case Qt::Key_D: { // escalar a més petit
      scale -= 0.05;
      break;
    }
    case Qt::Key_R: {
      rot += M_PI/4;
      break;
    }
    default: event->ignore(); break;
  }
  update();
}

void MyGLWidget::createBuffers () 
{
  /*
  // Dades de la caseta
  // Dos VBOs, un amb posició i l'altre amb color
  glm::vec3 posicio[5] = {
	glm::vec3(-0.5, -1.0, -0.5),
	glm::vec3( 0.5, -1.0, -0.5),
	glm::vec3(-0.5,  0.0, -0.5),
	glm::vec3( 0.5,  0.0, -0.5),
	glm::vec3( 0.0,  0.6, -0.5)
  }; 
  glm::vec3 color[5] = {
	glm::vec3(1,0,0),
	glm::vec3(0,1,0),
	glm::vec3(0,0,1),
	glm::vec3(1,0,0),
	glm::vec3(0,1,0)
  };*/


  // Creació del Vertex Array Object per pintar
  /*glGenVertexArrays(1, &VAO_Casa);
  glBindVertexArray(VAO_Casa);*/

  homer.load("HomerProves.obj");

  glGenVertexArrays(1, &VAO_Homer);
  glBindVertexArray(VAO_Homer);

  /*glGenBuffers(1, &VBO_CasaPos);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_CasaPos);
  glBufferData(GL_ARRAY_BUFFER, sizeof(posicio), posicio, GL_STATIC_DRAW);*/

  glGenBuffers(1, &VBO_HomerPos);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_HomerPos);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*homer.faces().size()*3*3, homer.VBO_vertices(), GL_STATIC_DRAW);


  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  /*glGenBuffers(1, &VBO_CasaCol);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_CasaCol);
  glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);*/

  glGenBuffers(1, &VBO_HomerCol);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_HomerCol);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*homer.faces().size()*3*3, homer.VBO_matdiff(), GL_STATIC_DRAW);


  // Activem l'atribut colorLoc
  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);

  // Fins aqui, Homer. A partir d'aqui, Terra:

  // Dos VBOs, un amb posició i l'altre amb color
  // Terra verd de mides 2 en X i Z situat a Y = -1

  glm::vec3 posicio_terra[4] = {
    glm::vec3(-1, -1, 1),
    glm::vec3(1, -1, 1),
    glm::vec3(-1, -1, -1),
    glm::vec3(1, -1, -1)
  }; 
  glm::vec3 color_terra[5] = {
    glm::vec3(0,1,0),
    glm::vec3(0,1,0),
    glm::vec3(0,1,0),
    glm::vec3(0,1,0),
  };

  glGenVertexArrays(1, &VAO_Terra);
  glBindVertexArray(VAO_Terra);

  glGenBuffers(1, &VBO_TerraPos);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_TerraPos);
  glBufferData(GL_ARRAY_BUFFER, sizeof(posicio_terra), posicio_terra, GL_STATIC_DRAW);

  // Activem atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  glGenBuffers(1, &VBO_TerraCol);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_TerraCol);
  glBufferData(GL_ARRAY_BUFFER, sizeof(color_terra), color_terra, GL_STATIC_DRAW);

  // Activem l'atribut colorLoc
  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);

  glBindVertexArray (0);
}

void MyGLWidget::carregaShaders()
{
  // Creem els shaders per al fragment shader i el vertex shader
  QOpenGLShader fs (QOpenGLShader::Fragment, this);
  QOpenGLShader vs (QOpenGLShader::Vertex, this);
  // Carreguem el codi dels fitxers i els compilem
  fs.compileSourceFile("shaders/fragshad.frag");
  vs.compileSourceFile("shaders/vertshad.vert");
  // Creem el program
  program = new QOpenGLShaderProgram(this);
  // Li afegim els shaders corresponents
  program->addShader(&fs);
  program->addShader(&vs);
  // Linkem el program
  program->link();
  // Indiquem que aquest és el program que volem usar
  program->bind();

  // Obtenim identificador per a l'atribut “vertex” del vertex shader
  vertexLoc = glGetAttribLocation (program->programId(), "vertex");
  // Obtenim identificador per a l'atribut “color” del vertex shader
  colorLoc = glGetAttribLocation (program->programId(), "color");
  // Uniform locations
  transLoc = glGetUniformLocation(program->programId(), "TG");

  viewLoc = glGetUniformLocation(program->programId(),"view");
  
  projLoc = glGetUniformLocation(program->programId(),"proj");
}

void MyGLWidget::projectTransform() {
    
  // ra window
  ra = float(width())/float(height());


  // si rav > 1 i raw = rav => la nova (a*)w > aw minima requerida => no es retalla.
  // Aleshores no cal modificar FOV

  // Si rav < 1 => r(a*)w < raw => a*w < aw => retallara;
  // per evitar-ho cal incrementar l'angle d'obertura
  // amb raw = rav i nou FOV. a = alpha
  // FOV = 2(a*)v on (a*)v = arctg(tg(av)/rav)
  // sempre cal calcular el nou angle a partir de l'inicial (window quadrat)
  if (ra < 1 ) {
    alpha = atan(tan(alpha)/ra);
    FOV = 2*alpha;
  }



  // glm::perspective(FOV en radians, ra window, znear, zfar)
  glm::mat4 Proj = glm::perspective(FOV,ra, znear, zfar); 
  glUniformMatrix4fv(projLoc,1,GL_FALSE,&Proj[0][0]);
}

void MyGLWidget::viewTransform() {
  // glm::lookAt(OBS,VRP,UP)
  glm::mat4 View = glm::lookAt(OBS,VRP,UP);
  glUniformMatrix4fv(viewLoc,1,GL_FALSE,&View[0][0]);
}

void MyGLWidget::ini_camera() {
  OBS = glm::vec3(0,0,1);
  VRP = glm::vec3(0,0,0);
  UP = glm::vec3(0,1,0); // abans 0,1,0
  FOV = (float)M_PI/2.0f;
  alpha = FOV/2;
  ra = 1.0f;
  znear = 0.4f;
  zfar = 3.0f;
  projectTransform();
  viewTransform();
}

void MyGLWidget::capsa_contenidora(glm::vec3 punt_min, glm::vec3 punt_max) {
  centre_capsa_contenidora = glm::vec3((punt_max.x-punt_min.x)/2 + punt_min.x,(punt_max.y-punt_min.y)/2 + punt_min.y,(punt_max.z-punt_min.z)/2 + punt_min.z);
  radi_esfera = (punt_max.y - centre_capsa_contenidora.y);
  std::cout << centre_capsa_contenidora.x << " " << centre_capsa_contenidora.y << " " <<centre_capsa_contenidora.z << " " << radi_esfera << std::endl;
}