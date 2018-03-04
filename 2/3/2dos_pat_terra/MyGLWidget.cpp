#include "MyGLWidget.h"

#include <iostream>

#include <cmath>


MyGLWidget::MyGLWidget (QWidget* parent) : QOpenGLWidget(parent)
{
  setFocusPolicy(Qt::ClickFocus);  // per rebre events de teclat
  scale = 1.0f;
  rot = 0.0f;
  theta = 0.0f;
  psi = 0.0f;
  x_old = 0;
  y_old = 0;
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
  
  // punts extrems, max i min, de la capsa (del pat)
  calcul_punts_capsa();

  /*punt_min = glm::vec3(-2,0,-2);
  punt_max = glm::vec3(2,1,2);*/
  capsa_contenidora(punt_min, punt_max);

  //modelTransformInicialPatricio();

  ini_camera();

}

void MyGLWidget::paintGL () 
{
  // Esborrem el frame-buffer
  //glClear (GL_COLOR_BUFFER_BIT);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Carreguem la transformació de model
  //modelTransform ();
  //modelTransformPatricio();

  //ini_camera(); // o a initialize (?)

  // Activem el VAO per a pintar
  glBindVertexArray (VAO_Patricio);

  modelTransformPatricio();

  // pintem
  //glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);

  glDrawArrays(GL_TRIANGLES, 0, patricio.faces().size()*3);

  glBindVertexArray (VAO_Patricio);

  modelTransformPatricio2();

  // pintem
  //glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);

  glDrawArrays(GL_TRIANGLES, 0, patricio.faces().size()*3);


  // Ara, despres de Patricio, terra:

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

void MyGLWidget::modelTransformPatricio() {
  /*if (first) {
    first = false;
    modelTransformInicialPatricio();
  }
  else {
    glm::mat4 transform (1.0f);
    glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
  }
  //modelTransform();*/
  glm::mat4 transform (1.0f);
  transform = glm::translate(transform,glm::vec3(1,0,1));
  transform = glm::scale(transform, glm::vec3(1/altura_pat));
  transform = glm::translate(transform,glm::vec3(0,altura_pat/2,0));
  transform = glm::translate(transform,glm::vec3(-centre_capsa_contenidora.x,-centre_capsa_contenidora.y,-centre_capsa_contenidora.z));
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);

}

void MyGLWidget::modelTransformPatricio2() {
  /*if (first) {
    first = false;
    modelTransformInicialPatricio();
  }
  else {
    glm::mat4 transform (1.0f);
    glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
  }
  //modelTransform();*/
  glm::mat4 transform (1.0f);

  transform = glm::translate(transform,glm::vec3(-1,0,-1));
  transform = glm::rotate(transform, (float)M_PI,glm::vec3(0, 1, 0)); 
  transform = glm::scale(transform, glm::vec3(1/altura_pat));
  transform = glm::translate(transform,glm::vec3(0,altura_pat/2,0));
  transform = glm::translate(transform,glm::vec3(-centre_capsa_contenidora.x,-centre_capsa_contenidora.y,-centre_capsa_contenidora.z));
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);

}

// El terra de transformacio nomes te l'escalat
void MyGLWidget::modelTransformTerra() {
  glm::mat4 transform (1.0f);
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
    /*
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
    }*/
    // Per fer zoom, disminuim FOV
    // Els limits son 0 i pi radiants, o sigui 180 graus
    case Qt::Key_Z:
      if (alpha >= 0.1) {
        alpha -= 0.1; 
        FOV = 2*alpha;
        projectTransformPostZoom();
      }
      break;
    case Qt::Key_X:
    if (alpha <= (M_PI/2 - 0.1)) {
      alpha += 0.1; 
      FOV = 2*alpha;
      projectTransformPostZoom();
    }
      break;
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

  patricio.load("Patricio.obj");

  glGenVertexArrays(1, &VAO_Patricio);
  glBindVertexArray(VAO_Patricio);

  /*glGenBuffers(1, &VBO_CasaPos);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_CasaPos);
  glBufferData(GL_ARRAY_BUFFER, sizeof(posicio), posicio, GL_STATIC_DRAW);*/

  glGenBuffers(1, &VBO_PatricioPos);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_PatricioPos);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*patricio.faces().size()*3*3, patricio.VBO_vertices(), GL_STATIC_DRAW);


  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  /*glGenBuffers(1, &VBO_CasaCol);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_CasaCol);
  glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);*/

  glGenBuffers(1, &VBO_PatricioCol);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_PatricioCol);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*patricio.faces().size()*3*3, patricio.VBO_matdiff(), GL_STATIC_DRAW);


  // Activem l'atribut colorLoc
  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);



   // Patricio 2:

    patricio.load("Patricio.obj");

  glGenVertexArrays(1, &VAO_Patricio2);
  glBindVertexArray(VAO_Patricio2);

  /*glGenBuffers(1, &VBO_CasaPos);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_CasaPos);
  glBufferData(GL_ARRAY_BUFFER, sizeof(posicio), posicio, GL_STATIC_DRAW);*/

  glGenBuffers(1, &VBO_Patricio2Pos);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Patricio2Pos);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*patricio.faces().size()*3*3, patricio.VBO_vertices(), GL_STATIC_DRAW);


  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  /*glGenBuffers(1, &VBO_CasaCol);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_CasaCol);
  glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);*/

  glGenBuffers(1, &VBO_Patricio2Col);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Patricio2Col);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*patricio.faces().size()*3*3, patricio.VBO_matdiff(), GL_STATIC_DRAW);


  // Activem l'atribut colorLoc
  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);


  // Fins aqui, Patricio. A partir d'aqui, Terra:

  // Dos VBOs, un amb posició i l'altre amb color
  // Terra lila 4x4 situat a Y = -0

  glm::vec3 posicio_terra[4] = {
    glm::vec3(-2, 0, 2),
    glm::vec3(2, 0, 2),
    glm::vec3(-2, 0, -2),
    glm::vec3(2, 0, -2)
  }; 
  glm::vec3 color_terra[5] = {
    glm::vec3(0.5,0,0.5),
    glm::vec3(0.5,0,0.5),
    glm::vec3(0.5,0,0.5),
    glm::vec3(0.5,0,0.5),
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


  // Segons caixa contenidora
  /*FOV = atan(radi_esfera)/distancia_punts(OBS,VRP);
  alpha = FOV/2;
  znear = distancia_punts(OBS,VRP) - radi_esfera;
  zfar = 2*radi_esfera+znear;*/

  float d = 2*radi_esfera;
  znear = d - radi_esfera;
  zfar = d + radi_esfera;
  alpha = asin(radi_esfera/d);
  //alpha = atan(tan(alpha)/ra);
  FOV = 2*alpha;


  //


  // glm::perspective(FOV en radians, ra window, znear, zfar)
  glm::mat4 Proj = glm::perspective(FOV,ra, znear, zfar); 
  glUniformMatrix4fv(projLoc,1,GL_FALSE,&Proj[0][0]);
}

void MyGLWidget::viewTransform() {
  // Segons caixa contenidora
  //VRP = centre_capsa_contenidora;

  /*VRP = glm::vec3(0,0,0);
  
  float d = 2*radi_esfera;*/
  // OBA = VRP +d*v, v normalitzat en qualsevol direcció. Per exemple: v= (1,1,1)/|(1,1,1)|
  //glm::vec3 v = glm::vec3(1/sqrt(3),1/sqrt(3),1/sqrt(3));

  // Per l'exercici 5 (Euler) comentem les seguents dues linies


  /*glm::vec3 v = glm::vec3(0,0,1);
  OBS = glm::vec3(VRP.x + d*v.x,VRP.y + d*v.y,VRP.z + d*v.z);*/


  // UP qualsevol que no sigui paral·lel a v. En aquest cas, y
  // glm::lookAt(OBS,VRP,UP)

  // Per l'exercici 5 també comentem les seguents dues linies
  /*glm::mat4 View = glm::lookAt(OBS,VRP,UP);
  glUniformMatrix4fv(viewLoc,1,GL_FALSE,&View[0][0]);*/



  /*glm::mat4 VM(1.0f);
  VM = glm::translate(VM,glm::vec3(0,0,-d));
  VM = glm::rotate(VM, theta,glm::vec3(1, 0, 0));
  VM = glm::rotate(VM, psi,glm::vec3(0, 1, 0));
  VM = glm::translate(VM,glm::vec3(-VRP.x,-VRP.y,-VRP.z));
  glUniformMatrix4fv(viewLoc,1,GL_FALSE,&VM[0][0]);*/
  glm::mat4 View = glm::lookAt(OBS,VRP,UP);
  glUniformMatrix4fv(viewLoc,1,GL_FALSE,&View[0][0]);


}

void MyGLWidget::ini_camera() {
  OBS = glm::vec3(0,5,5);
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
  /*centre_capsa_contenidora = glm::vec3((punt_max.x-punt_min.x)/2 + punt_min.x,(punt_max.y-punt_min.y)/2 + punt_min.y,(punt_max.z-punt_min.z)/2 + punt_min.z);
  radi_esfera = (punt_max.y - centre_capsa_contenidora.y);*/
  centre_capsa_contenidora = glm::vec3((punt_max.x+punt_min.x)/2,(punt_max.y+punt_min.y)/2,(punt_max.z+punt_min.z)/2);
  radi_esfera = distancia_punts(punt_max,punt_min)/2;

  std::cout << centre_capsa_contenidora.x << " " << centre_capsa_contenidora.y << " " <<centre_capsa_contenidora.z << " " << radi_esfera << std::endl;
}

double MyGLWidget::distancia_punts(glm::vec3 a, glm::vec3 b) {
  return sqrt(pow((b.x-a.x),2)+pow((b.y-a.y),2)+pow((b.z-a.z),2));
}

void MyGLWidget::calcul_punts_capsa() {

  // (del Patricio)
  // Recorrem tots els vertex buscant el maxim i el minim
  // Anirem de 3 en 3 perque per cada iteracio mirarem
  // els 3 vertexs, x, y, z
  punt_min.x = punt_max.x = patricio.vertices()[0];
  punt_min.y = punt_max.y = patricio.vertices()[1];
  punt_min.z = punt_max.z = patricio.vertices()[2];
  unsigned int i = 3; // evitem warning. .size() es unsigned
  while (i < patricio.vertices().size()) {
    float aux_x = patricio.vertices()[i];
    float aux_y = patricio.vertices()[i+1];
    float aux_z = patricio.vertices()[i+2];
    if (aux_x > punt_max.x)
      punt_max.x = aux_x;
    else if (aux_x < punt_min.x)
      punt_min.x = aux_x;
    if (aux_y > punt_max.y)
      punt_max.y = aux_y;
    else if (aux_y < punt_min.y)
      punt_min.y = aux_y;
    if (aux_z > punt_max.z)
      punt_max.z = aux_z;
    else if (aux_z < punt_min.z)
      punt_min.z = aux_z;
    i += 3;
  }
  altura_pat = punt_max.y - punt_min.y;
  std::cout << "Punt min: " << punt_min.x << " " << punt_min.y << " " << punt_min.z;
  std::cout << "Punt max: " << punt_max.x << " " << punt_max.y << " " << punt_max.z;
}

/*void MyGLWidget::modelTransformInicialPatricio() {
  // El model original no està centrat. L'hem de centrar.
  glm::mat4 TG (1.0f);
  TG = glm::translate(TG,glm::vec3(-centre_capsa_contenidora.x,-centre_capsa_contenidora.y,-centre_capsa_contenidora.z));
  TG = glm::scale(TG, glm::vec3(99));
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}*/

// psi x, theta y
void MyGLWidget::mouseMoveEvent(QMouseEvent *e) {
  makeCurrent();
  // controla que s'ha premut el boto esquerre i cap modificador, com a les diapositives
  if (e->buttons() == Qt::LeftButton && !(e->modifiers() & (Qt::ShiftModifier | Qt::AltModifier | Qt::ControlModifier))) {
    int x_new = e->x();
    int y_new = e->y();
    if (x_new > x_old)
      psi += 0.1;
    else if (x_new < x_old)
      psi -= 0.1;
    if (y_new > y_old)
      theta += 0.1;
    else if (y_new < y_old)
      theta -= 0.1;

    x_old = x_new;
    y_old = y_new;

    viewTransform();
    update();
  }
}

void MyGLWidget::projectTransformPostZoom() {
  glm::mat4 Proj = glm::perspective(FOV,ra, znear, zfar); 
  glUniformMatrix4fv(projLoc,1,GL_FALSE,&Proj[0][0]);
}