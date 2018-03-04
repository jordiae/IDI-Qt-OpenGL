#include "MyGLWidget.h"

#include <iostream>

MyGLWidget::MyGLWidget (QWidget* parent) : QOpenGLWidget(parent)
{
  setFocusPolicy(Qt::ClickFocus);  // per rebre events de teclat
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
  
  angle = 0.0;
  angle2 = 0.0;
  
  glClearColor (0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
  carregaShaders();
  createBuffers();
}

void MyGLWidget::paintGL ()
{
  glClear (GL_COLOR_BUFFER_BIT);  // Esborrem el frame-buffer

  // Activem l'Array a pintar 
  glBindVertexArray(VAO);
 
  // Pintem l'escena
  glDrawArrays(GL_TRIANGLES, 0, 3);
  
  // Desactivem el VAO
  glBindVertexArray(0);
  
  // 2n objecte
  
  glBindVertexArray(VAO2);
  
  glDrawArrays(GL_TRIANGLES, 0, 3);
  
  glBindVertexArray(0);
  
  modelTransform(); // al paint en comptes de a carrega shaders
  
  
}

void MyGLWidget::resizeGL (int w, int h)
{
  glViewport (0, 0, w, h);
}

void MyGLWidget::createBuffers ()
{
  glm::vec3 Vertices[3];  // Tres vèrtexs amb X, Y i Z
  Vertices[0] = glm::vec3(-0.5, -0.5, 0.0);
  Vertices[1] = glm::vec3(-0.5, -0.25, 0.0);
  Vertices[2] = glm::vec3(-0.25, -0.25, 0.0);
  
  glm::vec3 Vertices2[3];  // Tres vèrtexs amb X, Y i Z
  Vertices2[0] = glm::vec3(0.5, 0.5, 0.0);
  Vertices2[1] = glm::vec3(0.5, 0.25, 0.0);
  Vertices2[2] = glm::vec3(0.25, 0.25, 0.0);
  
  // OBJECTE 1
  
  // Creació del Vertex Array Object (VAO) que usarem per pintar
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // Creació del buffer amb les dades dels vèrtexs
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
  // Activem l'atribut que farem servir per vèrtex	
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);
  
  transLoc = glGetUniformLocation(program->programId(),"TG");


  // Desactivem el VAO
  glBindVertexArray(0);
  
  
  // OBJECTE 2
  
  // Creació del Vertex Array Object (VAO) que usarem per pintar
  glGenVertexArrays(1, &VAO2);
  glBindVertexArray(VAO2);

  // Creació del buffer amb les dades dels vèrtexs
  glGenBuffers(1, &VBO2);
  glBindBuffer(GL_ARRAY_BUFFER, VBO2);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices2), Vertices2, GL_STATIC_DRAW);
  // Activem l'atribut que farem servir per vèrtex	
  glVertexAttribPointer(vertexLoc2, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc2);
  
  transLoc2 = glGetUniformLocation(program->programId(),"TG2");
  

  // Desactivem el VAO
  glBindVertexArray(0);
  
  
  
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
  
  vertexLoc2 = glGetAttribLocation (program->programId(), "vertex2");
  
  transLoc = glGetUniformLocation(program->programId(),"TG");
  
  transLoc2 = glGetUniformLocation(program->programId(),"TG2");
  
  //modelTransform();
  
}

void MyGLWidget::keyPressEvent(QKeyEvent* e)
{
    makeCurrent(); // fa actiu el nostre context d'OpenGL
    switch(e->key()) {
        case Qt::Key_P:
            angle += float(M_PI/6);
            angle2 -= float(M_PI/6);
            //close();
            break;
        default: e->ignore(); // propagar al pare
    }
    update(); // provoca que es torni a pintar l'escena
}


void MyGLWidget::modelTransform() {
    glm::mat4 TG(1.0); // Matriu de transformació, inicialment identitat

    TG = glm::rotate(TG,angle,glm::vec3(0, 0, 1));
    glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
    
    glm::mat4 TG2(1.0); // Matriu de transformació, inicialment identitat

    TG2 = glm::rotate(TG2,angle2,glm::vec3(0, 0, 1));
    glUniformMatrix4fv(transLoc2, 1, GL_FALSE, &TG2[0][0]);
    
    /*TG = glm::rotate(TG,float(-M_PI/6),glm::vec3(0, 0, 1));
    glUniformMatrix4fv(transLoc2, 1, GL_FALSE, &TG[0][0]);*/
}
