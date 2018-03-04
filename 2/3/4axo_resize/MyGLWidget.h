#define GLM_FORCE_RADIANS
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Model/model.h"

class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core 
{
  Q_OBJECT

  public:
    MyGLWidget (QWidget *parent=0);
    ~MyGLWidget ();

  protected:
    // initializeGL - Aqui incluim les inicialitzacions del contexte grafic.
    virtual void initializeGL ( );
    // paintGL - Mètode cridat cada cop que cal refrescar la finestra.
    // Tot el que es dibuixa es dibuixa aqui.
    virtual void paintGL ( );
    // resizeGL - És cridat quan canvia la mida del widget
    virtual void resizeGL (int width, int height);
    // keyPressEvent - Es cridat quan es prem una tecla
    virtual void keyPressEvent (QKeyEvent *event);

    virtual void mouseMoveEvent(QMouseEvent *e);

  private:
    void createBuffers ();
    void carregaShaders ();
    void modelTransform ();

    void modelTransformPatricio();
    void modelTransformTerra();

    void projectTransform();

    void viewTransform();

    void ini_camera();

    void capsa_contenidora(glm::vec3 punt_min, glm::vec3 punt_max);

    double distancia_punts(glm::vec3 a, glm::vec3 b);

    void calcul_punts_capsa();

    void modelTransformInicialPatricio();
    
    void projectTransformPostZoom();

    void modelTransformPatricio2();


    // attribute locations
    GLuint vertexLoc, colorLoc;
    // uniform locations
    GLuint transLoc, projLoc, viewLoc;
    // VAO i VBO names
    //GLuint VAO_Casa, VBO_CasaPos, VBO_CasaCol;

    GLuint VAO_Patricio, VBO_PatricioPos, VBO_PatricioCol;
    GLuint VAO_Patricio2, VBO_Patricio2Pos, VBO_Patricio2Col;

    GLuint VAO_Terra, VBO_TerraPos, VBO_TerraCol;
    
    // Program
    QOpenGLShaderProgram *program;
    // Internal vars
    float scale;
    glm::vec3 pos;

    float rot;


    glm::vec3 OBS;
    glm::vec3 VRP;
    glm::vec3 UP;
    float FOV;
    float ra;
    float znear;
    float zfar;

    float alpha;

    float theta, psi;

    Model patricio;

    glm::vec3 centre_capsa_contenidora;
    float radi_esfera;

    glm::vec3 punt_min;
    glm::vec3 punt_max;
    


    int x_old;
    int y_old;

    float altura_pat;


    float left, right, bottom, top;
};

